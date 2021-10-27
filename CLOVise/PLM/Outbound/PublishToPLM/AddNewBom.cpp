/*
* Copyright 2020-2021 CLO-Vise. All rights reserved
*
* @file PublishToPLM.cpp
*
* @brief Class implementation for publish Product and Document from CLO to PLM.
* This class has all the variable and methods implementation which are used to PLM Product and Document instance data update and Publish from CLO to PLM.
*
* @author GoVise
*
* @date 14-AUG-2020
*/
#include "AddNewBom.h"

#include <cstring>

#include <QTextEdit>
#include <QComboBox>
#include <QLabel>
#include "qtreewidget.h"
#include <QFile>
#include "qdir.h"


#ifdef __APPLE__

#include "zlib.h"
#include "CLOVise/PLM/Libraries/zlib/include/zip.h"
#include "CLOVise/PLM/Libraries/zlib/zipper/zipper.h"

#if defined(MSDOS) || defined(OS2) || defined(WIN32) || defined(__CYGWIN__)
#  include <fcntl.h>
#  include <io.h>
#  define SET_BINARY_MODE(file) setmode(fileno(file), O_BINARY)
#else
#  define SET_BINARY_MODE(file)
#endif

#define CHUNK 16384

using namespace zipper;

#else

#include "CLOVise/PLM/Libraries/ZipLib/ZipFile.h"
#include "CLOVise/PLM/Libraries/ZipLib/streams/memstream.h"
#include "CLOVise/PLM/Libraries/ZipLib/methods/Bzip2Method.h"

#endif

#include "CLOAPIInterface.h"
#include "CLOVise/PLM/Helper/Util/Helper.h"
#include "CLOVise/PLM/Helper/Util/Configuration.h"
#include "CLOVise/PLM/Inbound/Document/DocumentConfig.h"
#include "CLOVise/PLM/Helper/Util/FlexTypeHelper.h"
#include "CLOVise/PLM/Helper/UIHelper/UIHelper.h"
#include "CLOVise/PLM/Helper/Util/Logger.h"
#include "CLOVise/PLM/CLOViseSuite/CLOViseSuite.h"
#include "CLOVise/PLM/Helper/UIHelper/CVHoverDelegate.h"
#include "CLOVise/PLM/Inbound/Color/PLMColorSearch.h"
#include "CLOVise/PLM/Inbound/Color/ColorConfig.h"
#include "CLOVise/PLM/Outbound/PublishToPLM/Section.h"
#include "CLOVise/PLM/Outbound/PublishToPLM/CreateProduct.h"
#include "CLOVise/PLM/Inbound/Material/PLMMaterialSearch.h"
#include "CLOVise/PLM/Inbound/Material/MaterialConfig.h"

using namespace std;

namespace CLOVise
{
	AddNewBom* AddNewBom::_instance = NULL;

	AddNewBom* AddNewBom::GetInstance()
	{
		if (_instance == NULL) {
			_instance = new AddNewBom();
		}

		return _instance;
	}

	void AddNewBom::Destroy()
	{
		if (_instance) {
			delete _instance;
			_instance = NULL;
		}
	}

	AddNewBom::AddNewBom(QWidget* parent) : MVDialog(parent)
	{
		setupUi(this);
		Logger::Debug("AddNewBom -> Constructor() -> Start");


		QString windowTitle = "Create New BOM ";
		this->setWindowTitle(windowTitle);

#ifdef __APPLE__
		this->setWindowFlags(Qt::Dialog | Qt::CustomizeWindowHint | Qt::WindowMinMaxButtonsHint | Qt::WindowCloseButtonHint);
#else
		this->setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint | Qt::CustomizeWindowHint);
		m_pTitleBar = new MVTitleBar(windowTitle, this);
		layout()->setMenuBar(m_pTitleBar);
#endif // !__APPLE__
		m_buttonSignalMapper = new QSignalMapper();
		m_deleteButtonSignalMapper = new QSignalMapper();
		m_mappedColorwaysArr = json::array();
		m_colorwayOverridesJson = json::array();
		m_createButton = CVWidgetGenerator::CreatePushButton("Create", ADD_HOVER_ICON_PATH, "Create", PUSH_BUTTON_STYLE, 30, true);
		m_backButton = CVWidgetGenerator::CreatePushButton("Back", BACK_HOVER_PATH, "Back", PUSH_BUTTON_STYLE, 30, true);

		m_createBomTreeWidget = CVWidgetGenerator::CreatePublishTreeWidget("QTreeWidget { background-color: #262628; border: 1px solid #000;}""QTreeWidget::item { padding :5px; height: 20px; color: #FFFFFF; font-face: ArialMT; font-size: 10px; width: 90px; margin-left: 5px; margin-right: 5px; margin-top: 5px; margin-bottom: 5px; border: none;}""QTreeWidget::item:hover{background-color: #262628;}", true);
		m_createBomTreeWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
		horizontalLayout->insertWidget(0, m_createBomTreeWidget);
		m_createBomTreeWidget->setMinimumHeight(600);
		QSpacerItem *horizontalSpacer = new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Expanding);
		QSpacerItem *horizontalSpacer1 = new QSpacerItem(1, 1, QSizePolicy::Fixed, QSizePolicy::Fixed);

		ui_buttonsLayout->insertSpacerItem(0, horizontalSpacer);
		ui_buttonsLayout->insertWidget(1, m_backButton);
		ui_buttonsLayout->insertSpacerItem(2, horizontalSpacer1);
		ui_buttonsLayout->insertWidget(3, m_createButton);
		ui_buttonsLayout->insertSpacerItem(4, horizontalSpacer);
		json jsonbbj;


		string initialConfigJsonString = RESTAPI::CentricRestCallGet(Configuration::GetInstance()->GetPLMServerURL() + RESTAPI::SEARCH_ATT_API, APPLICATION_JSON_TYPE, "skip=0&limit=" + Configuration::GetInstance()->GetMaximumLimitForRefAttValue() + "&node_name=Data Sheet, BOM");

		Logger::RestAPIDebug("initialConfigJsonString::" + initialConfigJsonString);
		if (FormatHelper::HasError(initialConfigJsonString))
		{
			Helper::GetCentricErrorMessage(initialConfigJsonString);
			throw runtime_error(initialConfigJsonString);
		}
		json initialConfigJson = json::parse(initialConfigJsonString);
		string formDefinition = Helper::GetJSONValue<string>(initialConfigJson[0], CREATE_DEFINITION_KEY, true);

		string fieldsconfigJsonString = RESTAPI::CentricRestCallGet(Configuration::GetInstance()->GetPLMServerURL() + RESTAPI::SEARCH_ATT_DEFINITION_API + formDefinition + RESTAPI::SEARCH_ATT_DEFINITION_ADDITIONAL_API, APPLICATION_JSON_TYPE, "");

		Logger::RestAPIDebug("fieldsconfigJsonString::" + fieldsconfigJsonString);
		if (FormatHelper::HasError(fieldsconfigJsonString))
		{
			Helper::GetCentricErrorMessage(fieldsconfigJsonString);
			throw runtime_error(fieldsconfigJsonString);
		}

		json formDefinitionJson = json::parse(fieldsconfigJsonString);
		drawWidget(formDefinitionJson, m_createBomTreeWidget);
		connectSignalSlots(true);

		m_bomTableColumnJson = json::object();
		string columnsNames = DirectoryUtil::GetPLMPluginDirectory() + "BomTableColumn.json";//Reading Columns from json
		m_bomTableColumnJson = Helper::ReadJSONFile(columnsNames);


		for (int i = 0; i < m_bomTableColumnJson.size(); i++)
		{
			json fieldsJson = Helper::GetJSONParsedValue<int>(m_bomTableColumnJson, i, false);
			string displayName = Helper::GetJSONValue<string>(fieldsJson, "display_name", true);
			string internalName = Helper::GetJSONValue<string>(fieldsJson, "internal_name", true);
			m_bomTableColumnlist.append(QString::fromStdString(displayName));
			m_bomTableColumnKeys.append(QString::fromStdString(internalName));
		}

		json responseJson = Helper::makeRestcallGet(RESTAPI::MATERIAL_TYPE_SEARCH_API, "?&limit=100", "", "Loading materail type details..");

		m_materialTypeList.append(QString::fromStdString(BLANK));
		for (int i = 0; i < responseJson.size(); i++)
		{
			json attJson = Helper::GetJSONParsedValue<int>(responseJson, i, false);;///use new method
			string attName = Helper::GetJSONValue<string>(attJson, ATTRIBUTE_NAME, true);
			Logger::Debug("AddNewBom -> AddNewBom attName: " + attName);
			string attId = Helper::GetJSONValue<string>(attJson, ATTRIBUTE_ID, true);
			Logger::Debug("AddNewBom -> AddNewBom attId: " + attId);
			m_materialTypeList.append(QString::fromStdString(attName));
			//m_seasonNameIdMap.insert(make_pair(attName, attId));
			//m_styleTypeNameIdMap.insert(make_pair(attName, attId));
			//comboBox->setProperty(attName.c_str(), QString::fromStdString(attId));
		}

		Logger::Debug("AddNewBom -> Constructor() -> End");
		/*if (!PublishToPLMData::GetInstance()->isModelExecuted)*/
		//RESTAPI::SetProgressBarData(0, "", false);
#ifdef __APPLE__
		RESTAPI::SetProgressBarData(0, "", false);
#endif
	}

	AddNewBom::~AddNewBom()
	{

	}


	/* Description - connectSignalSlots() method used to connect and disconnect signal and slots.
	* Parameter -  bool.
	* Exception -
	* Return -
	*/
	void AddNewBom::connectSignalSlots(bool _b)
	{

		if (_b)
		{

			QObject::connect(m_backButton, SIGNAL(clicked()), this, SLOT(onBackButtonClicked()));
			QObject::connect(m_createButton, SIGNAL(clicked()), this, SLOT(onCreateButtonClicked()));
			QObject::connect(m_buttonSignalMapper, SIGNAL(mapped(int)), this, SLOT(OnClickAddColorButton(int)));
			QObject::connect(m_deleteButtonSignalMapper, SIGNAL(mapped(int)), this, SLOT(OnClickDeleteButton(int)));



		}
		else
		{
			QObject::disconnect(m_backButton, SIGNAL(clicked()), this, SLOT(onBackButtonClicked()));
			QObject::disconnect(m_createButton, SIGNAL(clicked()), this, SLOT(onCreateButtonClicked()));
			QObject::disconnect(m_buttonSignalMapper, SIGNAL(mapped(int)), this, SLOT(OnClickAddColorButton(int)));
			QObject::disconnect(m_deleteButtonSignalMapper, SIGNAL(mapped(int)), this, SLOT(OnClickDeleteButton(int)));
		}
	}

	void AddNewBom::drawWidget(json _feildsJson, QTreeWidget* m_TreeWidget)
	{
		Logger::Debug("AddNewBom drawCriteriaWidget() start....");

		Logger::Logger("_feildsJson=============== " + to_string(_feildsJson));
		json enumJson = json::object();
		json attEnumsJson = json::array();
		json responseJson = json::object();
		json attJson = json::object();
		string attId = BLANK;
		string attName = BLANK;
		string attributeType = "";
		string attributeName = "";
		string attributeDisplayName = "";
		string attributeRequired = "";
		string restrictEdit = "";
		string internalName = "";
		QString attributeInitiaValue = "";
		string attRestApiExposed = "";

		string duplicateAttribute = "";
		Logger::Debug("AddNewBom drawCriteriaWidget() 1....");
		for (int i = 0; i < _feildsJson.size(); i++)
		{
			Logger::Debug("AddNewBom drawCriteriaWidget() 2....");
			attJson = Helper::GetJSONParsedValue<int>(_feildsJson, i, false);
			Logger::Debug("AddNewBom drawCriteriaWidget() 3");
			bool isEditable = false;
			bool required = true;
			responseJson.clear();
			Logger::Debug("AddNewBom drawCriteriaWidget() 3");
			attRestApiExposed = Helper::GetJSONValue<string>(attJson, ATTRIBUTE_REST_API_EXPOSED, true);
			if (attRestApiExposed == "false")
				continue;
			Logger::Debug("AddNewBom drawCriteriaWidget() 4");
			attributeType = Helper::GetJSONValue<string>(attJson, ATTRIBUTE_TYPE, true);
			Logger::Debug("AddNewBom drawCriteriaWidget() attributeType...." + attributeType);
			internalName = Helper::GetJSONValue<string>(attJson, ATTRIBUTE_INTERNAL_NAME, true);
			Logger::Debug("AddNewBom drawCriteriaWidget() internalName...." + internalName);

			attributeName = Helper::GetJSONValue<string>(attJson, ATTRIBUTE_NAME, true);
			attributeDisplayName = Helper::GetJSONValue<string>(attJson, ATTRIBUTE_DISPLAY_NAME, true);
			Logger::Debug("AddNewBom drawCriteriaWidget() attributeDisplayName...." + attributeDisplayName);

			restrictEdit = Helper::GetJSONValue<string>(attJson, ATTRIBUTE_EDITABLE, true);
			Logger::Debug("AddNewBom drawCriteriaWidget() attributeEditable...." + restrictEdit);

			attributeRequired = Helper::GetJSONValue<string>(attJson, ATTRIBUTE_REQUIRED, true);
			attributeInitiaValue = QString::fromStdString(Helper::GetJSONValue<string>(attJson, ATTRIBUTE_INITIAL_VALUE, true));
			Logger::Debug("AddNewBom drawCriteriaWidget() attributeDefaultValue...." + attributeInitiaValue.toStdString());


			if (!FormatHelper::HasContent(attributeInitiaValue.toStdString()) || attributeInitiaValue.contains("ref(centric:)"))
			{
				attributeInitiaValue = "";
			}
			else
			{
				int found = attributeInitiaValue.toStdString().find_first_of(":");
				if (found != -1)
					attributeInitiaValue = QString::fromStdString(attributeInitiaValue.toStdString().substr(found + 1));
			}

			if (attributeRequired == "false")
				required = false;
			if (restrictEdit == "false")
			{
				isEditable = true;
				Logger::Debug("AddNewBom drawCriteriaWidget() isEditable...." + to_string(isEditable));
			}

			if (attributeType == INTEGER_ATT_TYPE_KEY || attributeType == CONSTANT_ATT_TYPE_KEY)
			{
				QTreeWidgetItem* topLevel = new QTreeWidgetItem();  // Creating new TreeWidgetItem
				m_TreeWidget->addTopLevelItem(topLevel);
				// Adding ToplevelItem
				m_TreeWidget->setItemWidget(topLevel, 0, CVWidgetGenerator::CreateLabelWidget(attributeDisplayName, internalName, "", required, !isEditable));
				m_TreeWidget->setItemWidget(topLevel, 1, CVWidgetGenerator::CreateSpinBoxWidget(attributeInitiaValue, isEditable, ""));

			}
			else if (attributeType == DATE_ATT_TYPE_KEY || attributeType == TIME_ATT_TYPE_KEY)
			{
				ColorConfig::GetInstance()->SetDateFlag(true);
				MaterialConfig::GetInstance()->SetDateFlag(true);
				ProductConfig::GetInstance()->SetDateFlag(true);
				QStringList dateList;
				QTreeWidgetItem* topLevel = new QTreeWidgetItem();  // Creating new TreeWidgetItem
				{
					/*if (stoi(attributeEditable) > 0)
					{
						isEditable = false;
					}*/
					m_TreeWidget->addTopLevelItem(topLevel);			// Adding ToplevelItem
					m_TreeWidget->setItemWidget(topLevel, 0, CVWidgetGenerator::CreateLabelWidget(attributeDisplayName, internalName, "", required, !isEditable));	// Adding label at column 1
					m_TreeWidget->setItemWidget(topLevel, 1, CVWidgetGenerator::CreateDateWidget(attributeInitiaValue.toStdString(), isEditable));
				}
			}
			else if (attributeType == BOOLEAN_ATT_TYPE_KEY)
			{
				QTreeWidgetItem* topLevel = new QTreeWidgetItem();  // Creating new TreeWidgetItem
				/*if (stoi(attributeEditable) > 0)
				{
					isEditable = false;
				}*/
				m_TreeWidget->addTopLevelItem(topLevel);						// Adding ToplevelItem
				m_TreeWidget->setItemWidget(topLevel, 0, CVWidgetGenerator::CreateLabelWidget(attributeDisplayName, internalName, "", required, !isEditable));	// Adding label at column 1
				m_TreeWidget->setItemWidget(topLevel, 1, CVWidgetGenerator::CreateBooleanWidget(attributeInitiaValue.toStdString(), isEditable, topLevel, 1));	// Adding label at column 2

			}
			else if (attributeType == REF_ATT_TYPE_KEY || attributeType == ENUM_ATT_TYPE_KEY)
			{
				Logger::Debug("AddNewBom drawWidget() ref  6....");

				QTreeWidgetItem* topLevel = new QTreeWidgetItem();
				ComboBoxItem* comboBox = new ComboBoxItem();
				QStringList valueList;
				QString selectedValue = "";
				valueList.append(QString::fromStdString(BLANK));
				comboBox->setFocusPolicy(Qt::StrongFocus);

				map<string, string> enumKeyValueMap;
				if (attributeType == ENUM_ATT_TYPE_KEY)
				{
					string format = Helper::GetJSONValue<string>(attJson, "format", true);
					if (FormatHelper::HasContent(format))
						UIHelper::GetEnumValues(format, valueList, comboBox);
				}
				else
				{
					if (internalName == "subtype")
					{
						responseJson = Helper::makeRestcallGet(RESTAPI::APPAREL_BOM_SUBTYPE_API, "?limit=" + Configuration::GetInstance()->GetMaximumLimitForRefAttValue(), "", "Loading season details..");
					}
					else if (attributeName == "Style Type")
					{
						responseJson = Helper::makeRestcallGet(RESTAPI::STYLE_TYPE_API, "?available=true&limit=100", "", "Loading style type details..");
					}
					for (int i = 0; i < responseJson.size(); i++)
					{
						attJson = Helper::GetJSONParsedValue<int>(responseJson, i, false);;///use new method
						attName = Helper::GetJSONValue<string>(attJson, ATTRIBUTE_NAME, true);
						Logger::Debug("PublishToPLMData -> SetDocumentConfigJSON attName: " + attName);
						attId = Helper::GetJSONValue<string>(attJson, ATTRIBUTE_ID, true);
						Logger::Debug("PublishToPLMData -> SetDocumentConfigJSON attId: " + attId);
						valueList.append(QString::fromStdString(attName));
						m_subTypeNameIdMap.insert(make_pair(attName, attId));
						//m_styleTypeNameIdMap.insert(make_pair(attName, attId));
						comboBox->setProperty(attName.c_str(), QString::fromStdString(attId));
					}
				}

				if (!isEditable)
				{
					comboBox->setDisabled(true);
					comboBox->setStyleSheet(DISABLED_COMBOBOX_STYLE);
				}
				comboBox->setProperty("attName", QString::fromStdString(attributeName));

				comboBox->setStyleSheet(COMBOBOX_STYLE);
				m_TreeWidget->addTopLevelItem(topLevel);
				m_TreeWidget->setItemWidget(topLevel, 0, CVWidgetGenerator::CreateLabelWidget(attributeDisplayName, internalName, "", required, !isEditable));
				m_TreeWidget->setItemWidget(topLevel, 1, comboBox);
				valueList.sort();
				comboBox->addItems(valueList);


				comboBox->setProperty(ATTRIBUTE_INITIAL_VALUE.c_str(), attributeInitiaValue);

				int valueIndex = comboBox->findText(attributeInitiaValue);

				if (valueIndex == -1) // -1 for not found
				{
					int index = comboBox->findText(QString::fromStdString(BLANK));
					comboBox->setCurrentIndex(index);
				}
				else
				{
					comboBox->setCurrentIndex(valueIndex);
				}
				comboBox->setProperty("LabelName", QString::fromStdString(attributeName));
				if (comboBox->isEnabled())
				{
					comboBox->setEditable(true);
					comboBox->fillItemListAndDefaultValue(valueList, comboBox->currentText());
					QCompleter* m_nameCompleter = new QCompleter(valueList, comboBox); // using QCompleter class
					m_nameCompleter->setCompletionMode(QCompleter::PopupCompletion);
					m_nameCompleter->setCaseSensitivity(Qt::CaseInsensitive);
					comboBox->setCompleter(m_nameCompleter);
				}
				QObject::connect(comboBox, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(OnHandleDropDownValue(const QString&)));
				Logger::Debug("AddNewBom drawWidget() ref  End....");

			}
			else if (attributeType == STRING_ATT_TYPE_KEY || attributeType == TEXT_ATT_TYPE_KEY)
			{
				Logger::Debug("AddNewBom drawWidget() string  Start....");
				QTreeWidgetItem* topLevel = new QTreeWidgetItem();  // Creating new TreeWidgetItem
				QLineEdit* LineEdit = CVWidgetGenerator::CreateLineEdit("", "", true);
				LineEdit->setStyleSheet(LINEEDIT_STYLE);
				if (!isEditable)
				{
					//isEditable = false;

					LineEdit->setEnabled(false);
					LineEdit->setStyleSheet(DISABLED_LINEEDIT_STYLE);
				}
				LineEdit->setAttribute(Qt::WA_MacShowFocusRect, false);

				//LineEdit->setText("");
				LineEdit->setProperty(ATTRIBUTE_INITIAL_VALUE.c_str(), attributeInitiaValue);
				LineEdit->setText(attributeInitiaValue);

				m_TreeWidget->addTopLevelItem(topLevel);						// Adding ToplevelItem
				m_TreeWidget->setItemWidget(topLevel, 0, CVWidgetGenerator::CreateLabelWidget(attributeDisplayName, internalName, "", required, !isEditable));	// Adding label at column 1
				m_TreeWidget->setItemWidget(topLevel, 1, LineEdit);
				Logger::Debug("AddNewBom drawWidget() string  End....");
			}
		}
		Logger::Debug("AddNewBom drawWidget() end....");
	}
	void AddNewBom::onBackButtonClicked()
	{
		Logger::Debug("AddNewBom onBackButtonClicked() Start....");
		this->hide();
		CreateProduct::GetInstance()->setModal(true);
		CreateProduct::GetInstance()->show();
		Logger::Debug("AddNewBom onBackButtonClicked() End....");
	}
	void AddNewBom::onCreateButtonClicked()
	{
		Logger::Debug("AddNewBom onCreateButtonClicked() Start....");
		CreateTableforEachSection();
		QTreeWidget *tree = new QTreeWidget();
		m_BomMetaData = CreateProduct::GetInstance()->collectCriteriaFields(m_createBomTreeWidget, tree);
		UTILITY_API->DisplayMessageBox("m_BomMetaData"+ to_string(m_BomMetaData));
		this->hide();
		CreateProduct::GetInstance()->setModal(true);
		CreateProduct::GetInstance()->show();
		Logger::Debug("AddNewBom onCreateButtonClicked() End....");
	}

	void AddNewBom::CreateTableforEachSection()
	{
		Logger::Debug("AddNewBom CreateTableforEachSection Start: ");

		json colorwayListJsonArr = json::array();
		string colorwayList = Helper::GetJSONValue<string>(CreateProduct::GetInstance()->m_techPackJson, "colorwayList", false);
		colorwayListJsonArr = json::parse(colorwayList);
		for (int colorwayListCount = 0; colorwayListCount < colorwayListJsonArr.size(); colorwayListCount++)
		{
			json colorwayJson = json::object();
			json colorJson = json::object();
			string cloColorwayStr = colorwayListJsonArr[colorwayListCount].dump();
			json cloColorwayJson = json::parse(cloColorwayStr);

			string colorwayName = Helper::GetJSONValue<string>(cloColorwayJson, "name", true);
			colorwayJson["colorwayName"] = colorwayName;
			colorwayJson["colorwayIndex"] = to_string(colorwayListCount);
			m_mappedColorwaysArr.push_back(colorwayJson);

		}

		QStringList sectionList;
		sectionList << "FABRICS" << "TRIMS" << "LABELS";
		QTableWidget* sectionTable;
		QStringList tablecolumnList;
		QStringList bomTableColumnKeys;
		if (CreateProduct::GetInstance()->m_mappedColorways.size())
		{
			tablecolumnList = m_bomTableColumnlist;
			tablecolumnList.append(CreateProduct::GetInstance()->m_mappedColorways);
			bomTableColumnKeys = m_bomTableColumnKeys;
			bomTableColumnKeys.append(CreateProduct::GetInstance()->m_mappedColorways);

		}
		for (int i = 0; i < sectionList.size(); i++)
		{


			Section* section = new Section(sectionList[i], 300);

			sectionTable = new QTableWidget(section);
			sectionTable->setProperty("TableName", sectionList[i]);
			sectionTable->setColumnCount(tablecolumnList.size());
			sectionTable->setHorizontalHeaderLabels(tablecolumnList);
			if (sectionList[i] == "FABRICS")
			{
				Logger::Debug("AddNewBom -> CreateTableforEachSection() -> sectionTable" + to_string(long(sectionTable)));
				getMaterialDetails("fabricList", CreateProduct::GetInstance()->m_techPackJson, true, sectionTable, sectionList[i]);
			}
			if (sectionList[i] == "TRIMS")
			{
				Logger::Debug("AddNewBom -> CreateTableforEachSection() -> sectionTable" + to_string(long(sectionTable)));
				getMaterialDetails("buttonHeadList", CreateProduct::GetInstance()->m_techPackJson, false, sectionTable, sectionList[i]);
				getMaterialDetails("buttonHoleList", CreateProduct::GetInstance()->m_techPackJson, false, sectionTable, sectionList[i]);
				getMaterialDetails("zipperList", CreateProduct::GetInstance()->m_techPackJson, false, sectionTable, sectionList[i]);
			}


			m_bomSectionTableInfoMap.insert(make_pair(sectionList[i].toStdString(), sectionTable));
			//sectionTable->setStyleSheet("QTableWidget{ background-color: #262628; border-right: 1px solid #000000; border-top: 1px solid #000000; border-left: 1px solid #000000; font-face: ArialMT; font-size: 12px; color: #FFFFFF; }");
			sectionTable->verticalHeader()->hide();
			sectionTable->setShowGrid(false);
			sectionTable->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
			sectionTable->setSelectionMode(QAbstractItemView::NoSelection);
			sectionTable->horizontalHeader()->setStretchLastSection(true);
			auto* anyLayout = new QVBoxLayout();
			auto* addMaterialButtonLayout = new QHBoxLayout();
			QPushButton* addMaterialButton = CVWidgetGenerator::CreatePushButton("", ADD_HOVER_ICON_PATH, "Add Material", PUSH_BUTTON_STYLE, 30, true);
			addMaterialButton->setParent(sectionTable);

			addMaterialButton->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);

			QAction* AddFromMaterialAction = new QAction(tr("New From Material"), this);
			connect(AddFromMaterialAction, SIGNAL(triggered()), this, SLOT(onClickAddFromMaterialButton()));

			QAction* AddSpecialMaterialAction = new QAction(tr("New Special"), this);
			connect(AddSpecialMaterialAction, SIGNAL(triggered()), this, SLOT(onClickAddSpecialMaterialButton()));

			m_addMaterialButtonAndTableMap.insert(make_pair(AddFromMaterialAction, sectionTable));
			m_addMaterialButtonAndTableMap.insert(make_pair(AddSpecialMaterialAction, sectionTable));
			QMenu* menu = new QMenu(addMaterialButton);
			menu->addAction(AddFromMaterialAction);
			menu->addAction(AddSpecialMaterialAction);
			addMaterialButton->setMenu(menu);

			QSpacerItem *horizontalSpacer = new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Fixed);
			//connect(addMaterialButton, SIGNAL(clicked()), this, SLOT(onClickAddMaterialButton()));
			addMaterialButtonLayout->insertWidget(1, addMaterialButton);
			addMaterialButtonLayout->insertSpacerItem(0, horizontalSpacer);
			anyLayout->insertLayout(0, addMaterialButtonLayout);
			anyLayout->insertWidget(1, sectionTable);
			section->setContentLayout(*anyLayout);
			section->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
			CreateProduct::GetInstance()->ui_sectionLayout->insertWidget(i, section);

		}

	}

	void AddNewBom::getMaterialDetails(string _str, json _techPackJson, bool _isfabric, QTableWidget* _sectionTable, QString _tableName)
	{

		string fabStrValue = _techPackJson[_str].dump();
		string materialCount, cwlStrVal, costInfoStrVal;
		json fabJson = json::parse(fabStrValue);
		json bomLineJson;
		string Id, Name, costVal, quantityVal, apiMetaDataVal, costUnitVal, quantityUnitVal;
		int cwlIndex = 0;
		int count = 0;
		for (int fabCount = 0; fabCount < fabJson.size(); fabCount++)
		{

			materialCount = fabJson[fabCount].dump();
			// Utility::InfoLogger("getFabricTrimDetails ::: JSON :: " + materialCount);
			json materialCountJson = json::parse(materialCount);
			cwlStrVal = materialCountJson["colorwayList"][cwlIndex].dump();
			json colorwayListJson = json::parse(cwlStrVal);
			if (materialCountJson.contains("apiMetaData"))// For In Library Materials
			{



				string apiMetadataStr = Helper::GetJSONValue<string>(materialCountJson, "apiMetaData", false);
				json apimetaDataJson = json::parse(apiMetadataStr);

				string code = Helper::GetJSONValue<string>(apimetaDataJson, "Code", true);
				//string dataType = Helper::GetJSONValue<string>(apimetaDataJson, "Data Type", true);
				string name = Helper::GetJSONValue<string>(apimetaDataJson, "Name", true);
				string type = Helper::GetJSONValue<string>(apimetaDataJson, "Type", true);
				string description = Helper::GetJSONValue<string>(apimetaDataJson, "Description", true);
				string objectId = Helper::GetJSONValue<string>(apimetaDataJson, "objectId", true);

				costInfoStrVal = colorwayListJson["colorwayCostInfo"].dump();
				json costInfoJson = json::parse(costInfoStrVal);
				quantityVal = Helper::GetJSONValue<string>(costInfoJson, "rollLength", true);
				string uom = Helper::GetJSONValue<string>(costInfoJson, "rollLengthUnit", true);

				json rowDataJson = json::object();
				getColorInfo(materialCountJson, rowDataJson, objectId);




				rowDataJson["Code"] = code;
				rowDataJson["Name"] = name;
				rowDataJson["Type"] = type;
				rowDataJson["Description"] = description;
				rowDataJson["Quantity"] = quantityVal;
				rowDataJson["UOM"] = uom;
				rowDataJson["materialId"] = objectId;
				AddBomRows(_sectionTable, rowDataJson, _tableName);
			}


		}

	}


	void AddNewBom::AddBomRows(QTableWidget* _sectionTable, json _rowDataJson, QString _tableName)
	{

		Logger::Debug("AddNewBom -> AddBomRows() -> Start");
		Logger::Debug("AddNewBom -> AddBomRows() -> _sectionTable" + to_string(long(_sectionTable)));
		int rowCount = _sectionTable->rowCount();
		_sectionTable->setRowCount(rowCount);
		_sectionTable->insertRow(rowCount);
		//bool colorChip = false;
		//Logger::Debug("AddNewBom -> AddBomRows() -> Code"+ _code);
		//Logger::Debug("AddNewBom -> AddBomRows() -> Name"+ _objectName);
		//Logger::Debug("AddNewBom -> AddBomRows() -> Quantity"+ _quantity);
		int columnCount = _sectionTable->columnCount();
		//" << "Code" <<"Name"<< "Type"<< "Placement"<< "Quantity"<<"Comment";
		QStringList bomTableColumnKeys;
		bomTableColumnKeys = m_bomTableColumnKeys;
		bomTableColumnKeys.append(CreateProduct::GetInstance()->m_mappedColorways);
		string materialId = Helper::GetJSONValue<string>(_rowDataJson, "materialId", true);
		Logger::Debug("AddNewBom -> AddBomRows() -> materialId" + materialId);
		for (int columnIndex = 0; columnIndex < bomTableColumnKeys.size(); columnIndex++)
		{
			bool colorChip = false;
			Logger::Debug("AddNewBom -> AddBomRows() -> 1");

			string text;
			if (_rowDataJson.contains(bomTableColumnKeys[columnIndex].toStdString()))
			{
				text = Helper::GetJSONValue<string>(_rowDataJson, bomTableColumnKeys[columnIndex].toStdString(), true);
				QLineEdit* newColumn = new QLineEdit();
				newColumn->setStyleSheet(LINEEDIT_STYLE);
				if (FormatHelper::HasContent(text))
					newColumn->setText(QString::fromStdString(text));
				else
					newColumn->setText("");
				Logger::Debug("AddNewBom -> AddBomRows() -> 6");
				QWidget *pLineEditWidget = CVWidgetGenerator::InsertWidgetInCenter(newColumn);
				newColumn->setProperty("row", rowCount);
				Logger::Debug("AddNewBom -> AddBomRows() -> 7");
				_sectionTable->setCellWidget(rowCount, columnIndex, pLineEditWidget);
				Logger::Debug("AddNewBom -> AddBomRows() -> 8");
			}
			else
			{
				text = "";

				if (CreateProduct::GetInstance()->m_mappedColorways.contains(bomTableColumnKeys[columnIndex]))
				{
					string colorData = _rowDataJson["color"].dump();

					json colorDataJson = json::parse(colorData);
					Logger::Debug("AddNewBom -> AddBomRows() -> colorDataJson" + to_string(colorDataJson));
					if (!colorDataJson.empty())
					{
						for (int colorwayListCount = 0; colorwayListCount < colorDataJson.size(); colorwayListCount++)
						{
							json colorwayJson = json::object();
							json colorJson = json::object();
							string cloColorwayStr = colorDataJson[colorwayListCount].dump();

							json cloColorwayJson = json::parse(cloColorwayStr);
							Logger::Debug("AddNewBom -> AddBomRows() -> cloColorwayJson" + to_string(cloColorwayJson));
							string colorId = Helper::GetJSONValue<string>(cloColorwayJson, "colorObjectId", true);
							Logger::Debug("AddNewBom -> AddBomRows() -> colorId" + colorId);

							string colorwayName = Helper::GetJSONValue<string>(cloColorwayJson, "colorwayName", true);
							Logger::Debug("AddNewBom -> AddBomRows() -> colorwayName" + colorwayName);

							Logger::Debug("AddNewBom -> AddBomRows() -> horizontalHeaderItem Text" + _sectionTable->horizontalHeaderItem(columnIndex)->text().toStdString());
							if (colorwayName == _sectionTable->horizontalHeaderItem(columnIndex)->text().toStdString())
							{

								if (FormatHelper::HasContent(colorId))
								{
									//json dependentFieldJson = Helper::makeRestcallGet("csi-requesthandler/api/v2/color_specifications/", "&skip=0&limit=100", "" + colorId, "");
									string resultResponse = RESTAPI::CentricRestCallGet(Configuration::GetInstance()->GetPLMServerURL() + RESTAPI::SEARCH_COLOR_API + "/" + colorId, APPLICATION_JSON_TYPE, "");
									Logger::Debug("AddNewBom -> AddBomRows() -> resultResponse" + resultResponse);
									json ColoreResultJson = json::parse(resultResponse);
									Logger::Debug("AddNewBom -> AddBomRows() -> ColoreResultJson" + to_string(ColoreResultJson));
									string rgbValue = Helper::GetJSONValue<string>(ColoreResultJson, RGB_VALUE_KEY, true);
									string colorName = Helper::GetJSONValue<string>(ColoreResultJson, ATTRIBUTE_NAME, true);
									Logger::Debug("AddNewBom -> AddBomRows() -> rgbValue" + rgbValue);

									rgbValue = Helper::FindAndReplace(rgbValue, "(", "");
									rgbValue = Helper::FindAndReplace(rgbValue, ")", "");
									rgbValue = Helper::FindAndReplace(rgbValue, " ", "");
									Logger::Debug("AddNewBom -> AddBomRows() -> 2");
									if (FormatHelper::HasContent(rgbValue))
									{
										QStringList listRGB;
										QString colorRGB = QString::fromStdString(rgbValue);
										listRGB = colorRGB.split(',');
										int red = listRGB.at(0).toInt();
										int green = listRGB.at(1).toInt();
										int blue = listRGB.at(2).toInt();
										QColor color(red, green, blue);
										QImage image(20, 20, QImage::Format_ARGB32);
										image.fill(color);
										QPixmap pixmap;
										Logger::Debug("AddNewBom -> AddBomRows() -> 2");
										QLabel* label = new QLabel();
										label->setToolTip(QString::fromStdString(colorName));
										pixmap = QPixmap::fromImage(image);
										label->setPixmap(QPixmap(pixmap));
										

										QWidget* p_widget = new QWidget(_sectionTable);
										QGridLayout* gridLayout = new QGridLayout(_sectionTable);
										gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
										gridLayout->setContentsMargins(0, 0, 0, 0);

										QWidget *colorchip = nullptr;
										colorchip = CVWidgetGenerator::InsertWidgetInCenter(label);
										gridLayout->addWidget(colorchip, 0, 0, 1, 1, Qt::AlignHCenter);
										QPushButton* pushButton_2 = CVWidgetGenerator::CreatePushButton("", ADD_HOVER_ICON_PATH, "", PUSH_BUTTON_STYLE, 30, true);
										pushButton_2->setFixedHeight(20);
										pushButton_2->setFixedWidth(20);
										if (m_buttonSignalMapper != nullptr)
										{
											m_buttonSignalMapper->setProperty("TableName", _tableName);
											connect(pushButton_2, SIGNAL(clicked()), m_buttonSignalMapper, SLOT(map()));
											int number = rowCount * 10 + columnIndex;
											m_buttonSignalMapper->setMapping(pushButton_2, number);
										}
										pushButton_2->setProperty("TableName", _tableName);
										gridLayout->addWidget(pushButton_2, 0, 1, 1, 1, Qt::AlignHCenter);
										p_widget->setLayout(gridLayout);

										Logger::Debug("AddNewBom -> AddBomRows() -> 10");
										_sectionTable->setCellWidget(rowCount, columnIndex, p_widget);

										colorChip = true;
									}
								}
								else
								{
									QPushButton* newButton = new QPushButton();
									QWidget *pPushButtonWidget = CVWidgetGenerator::InsertWidgetInCenter(newButton);
									newButton->setProperty("row", rowCount);
									Logger::Debug("AddNewBom -> AddBomRows() -> 7");
									_sectionTable->setCellWidget(rowCount, columnIndex, pPushButtonWidget);
									Logger::Debug("AddNewBom -> AddBomRows() -> 8");
									colorChip = true;
								}
							}
						}
					}
					else
					{
						QWidget* p_widget = new QWidget(_sectionTable);
						QGridLayout* gridLayout = new QGridLayout(_sectionTable);
						gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
						gridLayout->setContentsMargins(0, 0, 0, 0);

						QColor color(0, 0, 0);
						QImage image(20, 20, QImage::Format_ARGB32);
						image.fill(color);
						QPixmap pixmap;
						Logger::Debug("AddNewBom -> AddBomRows() -> 2");
						QLabel* label = new QLabel();
						pixmap = QPixmap::fromImage(image);
						label->setPixmap(QPixmap(pixmap));
						Logger::Debug("AddNewBom -> AddBomRows() -> 3");
						QWidget *colorchip = nullptr;
						colorchip = CVWidgetGenerator::InsertWidgetInCenter(label);
						gridLayout->addWidget(colorchip, 0, 0, 1, 1, Qt::AlignHCenter);
						QPushButton* pushButton_2 = CVWidgetGenerator::CreatePushButton("", ADD_HOVER_ICON_PATH, "", PUSH_BUTTON_STYLE, 30, true);
						pushButton_2->setFixedHeight(20);
						pushButton_2->setFixedWidth(20);
						if (m_buttonSignalMapper != nullptr)
						{
							m_buttonSignalMapper->setProperty("TableName", _tableName);
							connect(pushButton_2, SIGNAL(clicked()), m_buttonSignalMapper, SLOT(map()));
							int number = rowCount * 10 + columnIndex;
							m_buttonSignalMapper->setMapping(pushButton_2, number);
						}
						pushButton_2->setProperty("TableName", _tableName);
						gridLayout->addWidget(pushButton_2, 0, 1, 1, 1, Qt::AlignHCenter);
						p_widget->setLayout(gridLayout);

						Logger::Debug("AddNewBom -> AddBomRows() -> 10");
						_sectionTable->setCellWidget(rowCount, columnIndex, p_widget);
						colorChip = true;
					}
				}
				else
				{


					Logger::Debug("AddNewBom -> AddBomRows() -> m_bomTableColumnKeys[columnIndex]" + bomTableColumnKeys[columnIndex].toStdString());
					if (bomTableColumnKeys[columnIndex] == "Common Color")
					{
						Logger::Debug("AddNewBom -> AddBomRows() -> 9");

						QPushButton* buttonColumn = new QPushButton("Add color");

						//newColumn->setProperty("row", rowCount);

						QWidget* p_widget = new QWidget(_sectionTable);
						QGridLayout* gridLayout = new QGridLayout(_sectionTable);
						gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
						gridLayout->setContentsMargins(0, 0, 0, 0);

						QColor color(0, 0, 0);
						QImage image(20, 20, QImage::Format_ARGB32);
						image.fill(color);
						QPixmap pixmap;
						Logger::Debug("AddNewBom -> AddBomRows() -> 2");
						QLabel* label = new QLabel();
						pixmap = QPixmap::fromImage(image);
						label->setPixmap(QPixmap(pixmap));
						Logger::Debug("AddNewBom -> AddBomRows() -> 3");
						QWidget *colorchip = nullptr;
						colorchip = CVWidgetGenerator::InsertWidgetInCenter(label);

						//QPushButton* pushButton = new QPushButton(_sectionTable);
						//pushButton->setObjectName(QString::fromUtf8("Add"));
						gridLayout->addWidget(colorchip, 0, 0, 1, 1, Qt::AlignHCenter);
						QPushButton* pushButton_2 = CVWidgetGenerator::CreatePushButton("", ADD_HOVER_ICON_PATH, "", PUSH_BUTTON_STYLE, 30, true);
						pushButton_2->setFixedHeight(20);
						pushButton_2->setFixedWidth(20);
						if (m_buttonSignalMapper != nullptr)
						{
							m_buttonSignalMapper->setProperty("TableName", _tableName);
							connect(pushButton_2, SIGNAL(clicked()), m_buttonSignalMapper, SLOT(map()));
							int number = rowCount * 10 + columnIndex;
							m_buttonSignalMapper->setMapping(pushButton_2, number);
						}
						pushButton_2->setProperty("TableName", _tableName);
						//pushButton_2->setObjectName(QString::fromUtf8(""));
						//QObject::connect(pushButton_2, SIGNAL(clicked()), this, SLOT(OnClickAddColorButton()));
						//connect(updateColorButton, SIGNAL(clicked()), m_buttonSignalMapper, SLOT(map()));
						gridLayout->addWidget(pushButton_2, 0, 1, 1, 1, Qt::AlignHCenter);
						//QLabel* label1 = new QLabel(_sectionTable);
						//label1->setText("Color Name Demo");
						//label->setObjectName(QString::fromUtf8("pushButton_3"));
						//gridLayout->addWidget(label1, 1, 0, 1, 2, Qt::AlignHCenter);
						p_widget->setLayout(gridLayout);
						//QWidget *pLineEditWidget = CVWidgetGenerator::InsertWidgetInCenter(p_widget);
						Logger::Debug("AddNewBom -> AddBomRows() -> 10");
						_sectionTable->setCellWidget(rowCount, columnIndex, p_widget);
					}
					else if (bomTableColumnKeys[columnIndex] == "Type")
					{
						ComboBoxItem* comboType = new ComboBoxItem();
						comboType->setStyleSheet("QComboBox{max-height: 25px; min-width: 100px;}");
						comboType->setFocusPolicy(Qt::StrongFocus);
						comboType->addItems(m_materialTypeList);
						comboType->setProperty("materialId", QString::fromStdString(materialId));
						int valueIndex = comboType->findText(QString::fromStdString(text));

						if (valueIndex == -1) // -1 for not found
						{
							int index = comboType->findText(QString::fromStdString(BLANK));
							comboType->setCurrentIndex(index);
						}
						else
						{
							comboType->setCurrentIndex(valueIndex);
						}

						//comboType->setProperty("Id", QString::fromStdString(_objectId));
						QWidget *pWidget = CVWidgetGenerator::InsertWidgetInCenter(comboType);

						//comboColorwayItem->clear();
						//comboColorwayItem->addItems(_colorwayNamesList);
						_sectionTable->setCellWidget(rowCount, columnIndex, pWidget);

						//QObject::connect(comboColorwayItem, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(OnHandleColorwayNameComboBox(const QString&)));
					}
					else if (bomTableColumnKeys[columnIndex] == "Delete")
					{
						QPushButton *deleteButton = CVWidgetGenerator::CreatePushButton("", ":/CLOVise/PLM/Images/icon_delete_over.svg", "Delete", PUSH_BUTTON_STYLE, 30, true);
						deleteButton->setStyleSheet(BUTTON_STYLE);
						if (m_deleteButtonSignalMapper != nullptr)
						{
							m_deleteButtonSignalMapper->setProperty("TableName", _tableName);
							connect(deleteButton, SIGNAL(clicked()), m_deleteButtonSignalMapper, SLOT(map()));

							

							m_deleteButtonSignalMapper->setMapping(deleteButton, rowCount);
						}
						//deleteButton->setProperty("TableName", _tableName);
						//QObject::connect(deleteButton, SIGNAL(clicked()), this, SLOT(OnClickDeleteButton()));
						Logger::Debug("AddNewBom -> AddBomRows() -> 9");
						QWidget *pPushButton = CVWidgetGenerator::InsertWidgetInCenter(deleteButton);
						Logger::Debug("AddNewBom -> AddBomRows() -> 10");
						_sectionTable->setCellWidget(rowCount, columnIndex, pPushButton);
						Logger::Debug("AddNewBom -> AddBomRows() -> 11");
					}
					else
					{
						QLineEdit* newColumn = new QLineEdit();
						newColumn->setStyleSheet(LINEEDIT_STYLE);
						if (FormatHelper::HasContent(text))
							newColumn->setText(QString::fromStdString(text));
						else
							newColumn->setText("");
						Logger::Debug("AddNewBom -> AddBomRows() -> 6");
						QWidget *pLineEditWidget = CVWidgetGenerator::InsertWidgetInCenter(newColumn);
						newColumn->setProperty("row", rowCount);
						Logger::Debug("AddNewBom -> AddBomRows() -> 7");
						_sectionTable->setCellWidget(rowCount, columnIndex, pLineEditWidget);
						Logger::Debug("AddNewBom -> AddBomRows() -> 8");
					}

				}

 

			}
		
			
			Logger::Debug("AddNewBom -> AddBomRows() -> End");
		}
	}
	void AddNewBom::onClickAddFromMaterialButton()
	{
		Logger::Debug("AddNewBom -> onClickAddFromMaterialButton () Start");
		QAction* button = (QAction*)sender();
		currentAddMaterialButtonClicked = button;
		Logger::Debug("AddNewBom -> onClickAddFromMaterialButton () button" + to_string(long(button)));
		CreateProduct::GetInstance()->hide();
		UTILITY_API->CreateProgressBar();
		Configuration::GetInstance()->SetProgressBarProgress(qrand() % 101);
		MaterialConfig::GetInstance()->SetMaximumLimitForMaterialResult();
		RESTAPI::SetProgressBarData(Configuration::GetInstance()->GetProgressBarProgress(), "Loading " + Configuration::GetInstance()->GetLocalizedMaterialClassName() + " Search", true);
		if (!MaterialConfig::GetInstance()->GetIsModelExecuted())
		{
			MaterialConfig::GetInstance()->InitializeMaterialData();
		}
		MaterialConfig::GetInstance()->SetIsRadioButton(true);

		PLMMaterialSearch::GetInstance()->setModal(true);
		UTILITY_API->DeleteProgressBar(true);
		PLMMaterialSearch::GetInstance()->exec();
		Logger::Debug("AddNewBom -> onClickAddFromMaterialButton () End");
	}
	void AddNewBom::onClickAddSpecialMaterialButton()
	{
		Logger::Debug("AddNewBom -> onClickAddSpecialMaterialButton () Start");
		QTableWidget* sectionTable;
		QAction* button = (QAction*)sender();
		auto it = AddNewBom::GetInstance()->m_addMaterialButtonAndTableMap.find(button);
		if (it != AddNewBom::GetInstance()->m_addMaterialButtonAndTableMap.end())
		{
			sectionTable = it->second;
			QString tableName = sectionTable->property("TableName").toString();
			json rowDataJson = json::object();
			rowDataJson["Code"] = "";
			rowDataJson["Name"] = "";
			rowDataJson["Type"] = "";
			rowDataJson["Description"] = "";
			rowDataJson["Quantity"] = "";
			rowDataJson["UOM"] = "";
			AddBomRows(sectionTable, rowDataJson, tableName);
		}

		Logger::Debug("AddNewBom -> onClickAddSpecialMaterialButton () End");
	}


	void AddNewBom::getColorInfo(json _FabricJson, json& _rowDataJson, string _materailId)
	{
		Logger::Debug("AddNewBom -> getColorInfo () Start");

		json colorwayJson = json::object();
		json colorJson = json::object();
		json colorwayListJsonArr = json::array();
		string colorwayList = Helper::GetJSONValue<string>(_FabricJson, "colorwayList", false);
		colorwayListJsonArr = json::parse(colorwayList);
		int count = 0;
		int Tempcount = 0;
		json MaterialColorwayDetailsJson = json::object();
		for (int colorwayListCount = 0; colorwayListCount < colorwayListJsonArr.size(); colorwayListCount++)
		{
			colorwayJson = json::object();
			colorJson = json::object();


			string cloColorwayStr = colorwayListJsonArr[colorwayListCount].dump();
			json cloColorwayJson = json::parse(cloColorwayStr);
			Logger::Logger("cloColorwayJsoncloColorwayJson - " + to_string(cloColorwayJson));
			string frontJsonStr = Helper::GetJSONValue<string>(cloColorwayJson, "materialFront", false);
			json frontJson = json::parse(frontJsonStr);
			Logger::Logger("frontJsonfrontJsonfrontJson - " + to_string(frontJson));
			string cloColorName = Helper::GetJSONValue<string>(frontJson, "baseColorName", true);
			string colorApiMetadataStr = Helper::GetJSONValue<string>(frontJson, TECHPACK_API_META_DATA_KEY, false);
			json colorApimetaDataJson = json::parse(colorApiMetadataStr);
			Logger::Logger("colorApimetaDataJsoncolorApimetaDataJsoncolorApimetaDataJson - " + to_string(colorApimetaDataJson));
			string colorId = Helper::GetJSONValue<string>(colorApimetaDataJson, "CLOVISE_COLOR_ID", true);
			for (int mappedColorwaysCount = 0; mappedColorwaysCount < m_mappedColorwaysArr.size(); mappedColorwaysCount++)
			{
				string mappedColorwaysStr = m_mappedColorwaysArr[mappedColorwaysCount].dump();
				json mappedColorwaysJson = json::parse(mappedColorwaysStr);
				Logger::Logger("mappedColorwaysJsonmappedColorwaysJsonmappedColorwaysJson - " + to_string(mappedColorwaysJson));
				string colorwayIndex = Helper::GetJSONValue<string>(mappedColorwaysJson, "colorwayIndex", true);
				string colorwayName = Helper::GetJSONValue<string>(mappedColorwaysJson, "colorwayName", true);
				Logger::Logger("colorwayIndexcolorwayIndex - " + (colorwayIndex));
				if (to_string(colorwayListCount) == colorwayIndex)
				{
					string colorwayObjectId = Helper::GetJSONValue<string>(mappedColorwaysJson, "plmColorwayObjId", true);
					Logger::Logger("colorwayObjectIdcolorwayObjectId - " + (colorwayObjectId));
					/*colorwayJson["colorwayObjectId"] = colorwayObjectId;
					colorwayJson["colorwayName"] = colorwayName;
					colorwayJson["colorObjectId"] = colorId;
					colorwayJson["colorName"] = colorwayName;*/
					colorJson["colorObjectId"] = colorId;
					colorJson["colorwayName"] = colorwayName;
					colorJson["colorName"] = cloColorName;
					_rowDataJson["color"][count++] = colorJson;


					MaterialColorwayDetailsJson[colorwayName] = colorJson;
					//m_colorwayOverridesJson.push_back(colorwayJson);
				}
			}
		}
		Logger::Debug("AddNewBom -> getColorInfo () MaterialColorwayDetailsJson" + to_string(MaterialColorwayDetailsJson));
		m_colorwayMapForBom.insert(make_pair(_materailId, MaterialColorwayDetailsJson));
		Logger::Debug("AddNewBom -> getColorInfo () m_colorwayOverridesJson" + to_string(m_colorwayOverridesJson));
		Logger::Debug("AddNewBom -> getColorInfo () End");
	}

	void AddNewBom::UpdateColorwayColumns()
	{

		Logger::Debug("AddNewBom -> UpdateColorwayColumns () Start");


		QStringList tablecolumnList;
		QStringList bomTableColumnKeys;
		if (CreateProduct::GetInstance()->m_mappedColorways.size() && m_bomSectionTableInfoMap.size() > 0)
		{
			tablecolumnList = m_bomTableColumnlist;
			tablecolumnList.append(CreateProduct::GetInstance()->m_mappedColorways);
			bomTableColumnKeys = m_bomTableColumnKeys;
			bomTableColumnKeys.append(CreateProduct::GetInstance()->m_mappedColorways);

		}
		int rowCount = 0;
		for (auto itr = m_bomSectionTableInfoMap.begin(); itr != m_bomSectionTableInfoMap.end(); itr++)// map contain section name and corresponding table pointer  
		{
			Logger::Debug("AddNewBom -> UpdateColorwayColumns () 1");
			QTableWidget* sectionTable = itr->second;
			int columnCount = sectionTable->columnCount();
			int columnCountWithoutcolorway = m_bomTableColumnlist.size();
			if (columnCount > columnCountWithoutcolorway)// means colorway column in table
			{
				int diff = columnCount - columnCountWithoutcolorway;
				Logger::Debug("AddNewBom -> UpdateColorwayColumns () diff" + to_string(diff));
				while (diff)
				{
					int columnNumber = columnCountWithoutcolorway + diff;
					sectionTable->removeColumn(columnNumber - 1);
					diff--;
				}
			}
		}
		Logger::Debug("AddNewBom -> UpdateColorwayColumns () m_bomSectionTableInfoMap.size()" + to_string(m_bomSectionTableInfoMap.size()));
		for (auto itr = m_bomSectionTableInfoMap.begin(); itr != m_bomSectionTableInfoMap.end(); itr++)// map contain section name and corresponding table pointer  
		{
			QTableWidget* sectionTable = itr->second;
			Logger::Debug("AddNewBom -> UpdateColorwayColumns () itr->first" + itr->first);
			//Logger::Debug("AddNewBom -> UpdateColorwayColumns () 11");
			sectionTable->setColumnCount(bomTableColumnKeys.size());
			sectionTable->setHorizontalHeaderLabels(tablecolumnList);
			Logger::Debug("AddNewBom -> UpdateColorwayColumns () sectionTable->rowCount()" + to_string(sectionTable->rowCount()));
			for (int rowCount = 0; rowCount < sectionTable->rowCount(); rowCount++)
			{
				QComboBox* typeCombo;
				typeCombo = static_cast<QComboBox*>(sectionTable->cellWidget(rowCount, TYPE_COLUMN)->children().last());
				QString matrialId = typeCombo->property("materialId").toString();
				Logger::Debug("AddNewBom -> UpdateColorwayColumns () matrialId" + matrialId.toStdString());
				if (FormatHelper::HasContent(matrialId.toStdString()))
				{
					Logger::Debug("AddNewBom -> UpdateColorwayColumns () sectionTable->columnCount()" + to_string(sectionTable->columnCount()));
					for (int columnIndex = 0; columnIndex < sectionTable->columnCount(); columnIndex++)
					{
						QString columnName = sectionTable->horizontalHeaderItem(columnIndex)->text();
						//sectionTabl
						Logger::Debug("AddNewBom -> UpdateColorwayColumns () 2");
						if (CreateProduct::GetInstance()->m_mappedColorways.contains(columnName))
						{
							Logger::Debug("AddNewBom -> UpdateColorwayColumns () columnName" + columnName.toStdString());
							auto colorwayJsonItr = m_colorwayMapForBom.find(matrialId.toStdString());
							json colorwayJson = colorwayJsonItr->second;
							Logger::Debug("AddNewBom -> UpdateColorwayColumns () colorwayJson" + to_string(colorwayJson));
							string colorwayNameStr = Helper::GetJSONValue<string>(colorwayJson, columnName.toStdString(), false);
							json colorwayNameJson1 = json::parse(colorwayNameStr);
							Logger::Debug("AddNewBom -> UpdateColorwayColumns () colorwayNameJson1" + to_string(colorwayNameJson1));
							string colorObjId = Helper::GetJSONValue<string>(colorwayNameJson1, "colorObjectId", true);
							Logger::Debug("AddNewBom -> UpdateColorwayColumns () colorObjId" + colorObjId);
							if (FormatHelper::HasContent(colorObjId))
							{
								//json dependentFieldJson = Helper::makeRestcallGet("csi-requesthandler/api/v2/color_specifications/", "&skip=0&limit=100", "" + colorId, "");
								string resultResponse = RESTAPI::CentricRestCallGet(Configuration::GetInstance()->GetPLMServerURL() + RESTAPI::SEARCH_COLOR_API + "/" + colorObjId, APPLICATION_JSON_TYPE, "");
								Logger::Debug("AddNewBom -> UpdateColorwayColumns() -> resultResponse" + resultResponse);
								json ColoreResultJson = json::parse(resultResponse);
								Logger::Debug("AddNewBom -> UpdateColorwayColumns() -> ColoreResultJson" + to_string(ColoreResultJson));
								string rgbValue = Helper::GetJSONValue<string>(ColoreResultJson, RGB_VALUE_KEY, true);
								string colorName = Helper::GetJSONValue<string>(ColoreResultJson, ATTRIBUTE_NAME, true);
								Logger::Debug("AddNewBom -> UpdateColorwayColumns() -> rgbValue" + rgbValue);

								rgbValue = Helper::FindAndReplace(rgbValue, "(", "");
								rgbValue = Helper::FindAndReplace(rgbValue, ")", "");
								rgbValue = Helper::FindAndReplace(rgbValue, " ", "");
								Logger::Debug("AddNewBom -> UpdateColorwayColumns() -> 3");
								if (FormatHelper::HasContent(rgbValue))
								{
									QStringList listRGB;
									QString colorRGB = QString::fromStdString(rgbValue);
									listRGB = colorRGB.split(',');
									int red = listRGB.at(0).toInt();
									int green = listRGB.at(1).toInt();
									int blue = listRGB.at(2).toInt();
									QColor color(red, green, blue);
									QImage image(20, 20, QImage::Format_ARGB32);
									image.fill(color);
									QPixmap pixmap;
									Logger::Debug("AddNewBom -> UpdateColorwayColumns() -> 4");
									QLabel* label = new QLabel();
									label->setToolTip(QString::fromStdString(colorName));
									pixmap = QPixmap::fromImage(image);
									label->setPixmap(QPixmap(pixmap));
									Logger::Debug("AddNewBom -> UpdateColorwayColumns() -> 5");
									QWidget *pWidget = nullptr;
									pWidget = CVWidgetGenerator::InsertWidgetInCenter(label);
									Logger::Debug("AddNewBom -> UpdateColorwayColumns() -> 6");
									sectionTable->setCellWidget(rowCount, columnIndex, pWidget);
									Logger::Debug("AddNewBom -> AddBomRows() -> 7");
									//colorChip = true;
								}
							}
						}

					}
				}
			}
		}

		Logger::Debug("AddNewBom -> UpdateColorwayColumns () End");
	}

	void AddNewBom::OnClickAddColorButton(int number)
	{
		Logger::Debug("AddNewBom -> OnClickUpdateColorButton () Start");
		CreateProduct::GetInstance()->hide();
		QSignalMapper* button = (QSignalMapper*)sender();
		Logger::Debug("AddNewBom -> OnClickUpdateColorButton () number" + to_string(number));
		int row, column;
		if (number < 10)
		{
			if (number == 0)
			{
				row = 0;
				column = 0;
			}
			else
			{
				row = 0;
				column = number;
			}
		}
		else
		{
			row = number / 10;
			column = number % 10;
		}
		Logger::Debug("AddNewBom -> OnClickUpdateColorButton () row" + to_string(row));
		Logger::Debug("AddNewBom -> OnClickUpdateColorButton () column" + to_string(column));

		m_currentTableName = button->property("TableName").toString().toStdString();
		m_currentRow = row;
		m_currentColumn = column;
		Logger::Debug("AddNewBom -> OnClickUpdateColorButton () tableName" + m_currentTableName);
		//Configuration::GetInstance()->SetIsUpdateColorClicked(true);
	//	m_selectedRow = i;
		//m_currentColorSpec = BLANK;

		Configuration::GetInstance()->SetIsUpdateColorClicked(true);
		//QWidget* widget = ui_colorwayTable->cellWidget(m_selectedRow, 1);
/*		QComboBox *colorwayNameCombo = static_cast<QComboBox*>(ui_colorwayTable->cellWidget(m_selectedRow, CLO_COLORWAY_COLUMN)->children().last());
		string colorSpecId = colorwayNameCombo->property("Id").toString().toStdString();
		if (!colorSpecId.empty())
			m_currentColorSpec = colorSpecId;*/

		ColorConfig::GetInstance()->InitializeColorData();
		UIHelper::ClearAllFieldsForSearch(PLMColorSearch::GetInstance()->GetTreewidget(0));
		UIHelper::ClearAllFieldsForSearch(PLMColorSearch::GetInstance()->GetTreewidget(1));
		PLMColorSearch::GetInstance()->setModal(true);
		UTILITY_API->DeleteProgressBar(true);
		PLMColorSearch::GetInstance()->exec();
		RESTAPI::SetProgressBarData(0, "", false);

		Logger::Debug("AddNewBom -> OnClickUpdateColorButton () End");

	}

	void AddNewBom::OnClickDeleteButton(int _rowCount)
	{
		Logger::Debug("AddNewBom -> OnClickDeleteButton () Start");
		QSignalMapper* button = (QSignalMapper*)sender();
	//	QPushButton* button = (QPushButton*)sender();



		string tableName = button->property("TableName").toString().toStdString();

		auto itr = m_bomSectionTableInfoMap.find(tableName);
		if (itr != m_bomSectionTableInfoMap.end())
		{
			QTableWidget* sectionTable = itr->second;


			sectionTable->removeRow(_rowCount);
			if (m_deleteButtonSignalMapper != nullptr)
			{
				for (int i = 0; i < sectionTable->rowCount(); i++)
				{
					QPushButton *deleteButoon = static_cast<QPushButton*>(sectionTable->cellWidget(i, DELETE_BUTTON_COLUMN)->children().last());
					m_deleteButtonSignalMapper->setMapping(deleteButoon, i);
				}
			}
		}
		Logger::Debug("AddNewBom -> OnClickDeleteButton () End");
	}

}