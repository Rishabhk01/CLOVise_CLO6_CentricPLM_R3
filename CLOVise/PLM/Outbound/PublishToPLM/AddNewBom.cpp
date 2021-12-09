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
		json mergedJsonArray = json::array();

		json jsonvalueArray = json::array();
		string attJson = DirectoryUtil::GetPLMPluginDirectory() + "CentricBomAttributes.json";//Reading Columns from json
		jsonvalueArray = Helper::ReadJSONFile(attJson);
		//UTILITY_API->DisplayMessageBox("jsonvalue:: " + to_string(jsonvalue));
		//createFieldsJson(to_string(fieldsJsonArray), jsonvalue);

		int sizeOfFormDefinitionJSON = formDefinitionJson.size();
		int sizeOfDefaultJSON = jsonvalueArray.size();

		for (int i = 0; i < sizeOfFormDefinitionJSON + sizeOfDefaultJSON; i++)
		{
			if (i < sizeOfDefaultJSON)
			{
				json jsonvalue = Helper::GetJSONParsedValue<int>(jsonvalueArray, i, false);
				string internalName = Helper::GetJSONValue<string>(jsonvalue, "rest_api_name", true);
				//m_localAttributesList.push_back(QString::fromStdString(internalName));
				mergedJsonArray[i] = jsonvalueArray[i];
			}
			else
			{
				json fieldsJson = Helper::GetJSONParsedValue<int>(formDefinitionJson, i - sizeOfDefaultJSON, false);
				string internalName = Helper::GetJSONValue<string>(fieldsJson, "rest_api_name", true);
			//	m_plmAttributesList.push_back(QString::fromStdString(internalName));
				mergedJsonArray[i] = formDefinitionJson[i - sizeOfDefaultJSON];
			}
		}

		drawWidget(mergedJsonArray, m_createBomTreeWidget);
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
			m_materialTypeNameIdMap.insert(make_pair( attId, attName));
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
			//QObject::connect(m_buttonSignalMapper, SIGNAL(mapped(int)), this, SLOT(OnClickAddColorButton(int)));
			QObject::connect(m_buttonSignalMapper, SIGNAL(mapped(const QString &)),this, SLOT(OnClickAddColorButton(const QString &)));
			QObject::connect(m_deleteButtonSignalMapper, SIGNAL(mapped(int)), this, SLOT(OnClickDeleteButton(int)));



		}
		else
		{
			QObject::disconnect(m_backButton, SIGNAL(clicked()), this, SLOT(onBackButtonClicked()));
			QObject::disconnect(m_createButton, SIGNAL(clicked()), this, SLOT(onCreateButtonClicked()));
			//QObject::disconnect(m_buttonSignalMapper, SIGNAL(mapped(int)), this, SLOT(OnClickAddColorButton(int)));
			QObject::disconnect(m_buttonSignalMapper, SIGNAL(mapped(const QString &)), this, SLOT(OnClickAddColorButton(const QString &)));
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
					
						if (internalName == "bom_template")
						{
							responseJson = Helper::makeRestcallGet(RESTAPI::BOM_TEMPLATE_API, "?is_template=true&limit=" + Configuration::GetInstance()->GetMaximumLimitForRefAttValue(), "", "Loading template details..");
							m_bomTemplateJson = responseJson;
						}
					else if (internalName == "subtype")
					{
						responseJson = Helper::makeRestcallGet(RESTAPI::APPAREL_BOM_SUBTYPE_API, "?limit=" + Configuration::GetInstance()->GetMaximumLimitForRefAttValue(), "", "Loading subtype details..");
					}
					//else if (attributeName == "Style Type")
					//{
					//	responseJson = Helper::makeRestcallGet(RESTAPI::STYLE_TYPE_API, "?available=true&limit=100", "", "Loading style type details..");
					//}
					for (int i = 0; i < responseJson.size(); i++)
					{
						attJson = Helper::GetJSONParsedValue<int>(responseJson, i, false);;///use new method
						attName = Helper::GetJSONValue<string>(attJson, ATTRIBUTE_NAME, true);
						Logger::Debug("PublishToPLMData -> SetDocumentConfigJSON attName: " + attName);
						attId = Helper::GetJSONValue<string>(attJson, ATTRIBUTE_ID, true);
						Logger::Debug("PublishToPLMData -> SetDocumentConfigJSON attId: " + attId);
						valueList.append(QString::fromStdString(attName));
						comboBox->setProperty(attName.c_str(), QString::fromStdString(attId));
						comboBox->setProperty(attId.c_str(), QString::fromStdString(attName));
					}
				}
				QObject::connect(comboBox, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(OnHandleDropDownValue(const QString&)));
				
				if (!isEditable)
				{
					comboBox->setDisabled(true);
					comboBox->setStyleSheet(DISABLED_COMBOBOX_STYLE);
				}
				comboBox->setProperty("attName", QString::fromStdString(attributeName));			
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
				comboBox->setStyleSheet(COMBOBOX_STYLE);
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
		
		try
		{
			QTreeWidget *tree = new QTreeWidget();
			UIHelper::ValidateRquired3DModelData(m_createBomTreeWidget);
			m_BomMetaData = CreateProduct::GetInstance()->collectCriteriaFields(m_createBomTreeWidget, tree);
			string bomName = Helper::GetJSONValue<string>(m_BomMetaData, "node_name", true);
			string bomTemplateId = Helper::GetJSONValue<string>(m_BomMetaData, "bom_template", true);
			string latestRevision;
			//CreateTableforEachSection();
			for (int i = 0; i < m_bomTemplateJson.size(); i++)
			{
				json attJson = Helper::GetJSONParsedValue<int>(m_bomTemplateJson, i, false);;///use new method
				//attName = Helper::GetJSONValue<string>(attJson, ATTRIBUTE_NAME, true);
				//Logger::Debug("PublishToPLMData -> SetDocumentConfigJSON attName: " + attName);
				string attId = Helper::GetJSONValue<string>(attJson, ATTRIBUTE_ID, true);
				Logger::Debug("PublishToPLMData -> SetDocumentConfigJSON attId: " + attId);
				if(attId== bomTemplateId)
				{
					latestRevision = Helper::GetJSONValue<string>(attJson, "latest_revision", true);
					break;
				}
			}
			if (!latestRevision.empty())

			{
				string resultResponse = RESTAPI::CentricRestCallGet(Configuration::GetInstance()->GetPLMServerURL() + RESTAPI::BOM_REVISION_API + "/" + latestRevision, APPLICATION_JSON_TYPE, "");
				//json responseJson = Helper::makeRestcallGet(RESTAPI::BOM_REVISION_API + "/" + latestRevision, "", "", "");
				json responseJson = json::parse(resultResponse);
				Logger::Debug("AddNewBom onCreateButtonClicked() responseJson...." + to_string(responseJson));
				json sectionIdsJson = Helper::GetJSONParsedValue<string>(responseJson, "all_sections", false);
				Logger::Debug("AddNewBom onCreateButtonClicked() sectionIdsJson...." + to_string(sectionIdsJson));
				CreateTableforEachSection(sectionIdsJson);
			}

			UTILITY_API->DisplayMessageBox("m_BomMetaData"+ to_string(m_BomMetaData));
			CreateProduct::GetInstance()->m_bomAddButton->setDisabled(true);
			CreateProduct::GetInstance()->m_bomName->setText(QString::fromStdString(bomName));
			this->hide();
			CreateProduct::GetInstance()->setModal(true);
			CreateProduct::GetInstance()->show();
		}
		catch (exception& e)
		{
			RESTAPI::SetProgressBarData(0, "", false);
			Logger::Error("AddNewBom-> AddNewBom Exception - " + string(e.what()));
			UTILITY_API->DisplayMessageBox(e.what());
			//Helper::RemoveDirectory(QString::fromStdString(Configuration::GetInstance()->TURNTABLE_IMAGES_TEMP_DIRECTORY));
			//dir.mkpath(QString::fromStdString(Configuration::GetInstance()->TURNTABLE_IMAGES_TEMP_DIRECTORY));

			this->show();
		}
		catch (const char* msg)
		{
			Logger::Error("AddNewBom-> AddNewBom Exception - " + string(msg));
			wstring wstr(msg, msg + strlen(msg));
			RESTAPI::SetProgressBarData(0, "", false);
			UTILITY_API->DisplayMessageBoxW(wstr);
			this->show();
		}
		catch (string str)
		{
			RESTAPI::SetProgressBarData(0, "", false);
			Logger::Error("Create product-> Create product Exception - " + str);
			UTILITY_API->DisplayMessageBox(str);
			//Helper::RemoveDirectory(QString::fromStdString(Configuration::GetInstance()->TURNTABLE_IMAGES_TEMP_DIRECTORY));
			//dir.mkpath(QString::fromStdString(Configuration::GetInstance()->TURNTABLE_IMAGES_TEMP_DIRECTORY));

			this->show();
		}

		Logger::Debug("AddNewBom onCreateButtonClicked() End....");
	}

	void AddNewBom::CreateTableforEachSection(json _sectionIdsjson)
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
		tablecolumnList = m_bomTableColumnlist;
		bomTableColumnKeys = m_bomTableColumnKeys;
		if (CreateProduct::GetInstance()->m_mappedColorways.size())
		{		
			tablecolumnList.append(CreateProduct::GetInstance()->m_mappedColorways);		
			bomTableColumnKeys.append(CreateProduct::GetInstance()->m_mappedColorways);

		}
		string sectionId;
		for (int sectionCount = 0; sectionCount < _sectionIdsjson.size(); sectionCount++)
		{
			string section = Helper::GetJSONValue<int>(_sectionIdsjson, sectionCount, true);
			Logger::Debug("AddNewBom -> CreateTableforEachSection() -> apiMetadataStr" + sectionId);
			sectionId += "id=" + section + "&";
		}
		
		sectionId = sectionId.substr(0, sectionId.length() - 1);

		string sectionDefinitions = RESTAPI::CentricRestCallGet(Configuration::GetInstance()->GetPLMServerURL() + RESTAPI::BOM_SECTION_DEFINITION_API + "?" + sectionId + "&sort=sort_order&limit=1000", APPLICATION_JSON_TYPE, "");
		Logger::Debug("AddNewBom -> CreateTableforEachSection() -> resultResponse" + sectionDefinitions);

		int sectionCountOnBomTab = 0;
		json sectionDefinitionsJson = json::parse(sectionDefinitions);
		json placementProductTypeJson;
		for (int sectionCount = 0; sectionCount < sectionDefinitionsJson.size(); sectionCount++)
		{
			Logger::Debug("AddNewBom -> CreateTableforEachSection() -> 1");
			json sectionCountJson = Helper::GetJSONParsedValue<int>(sectionDefinitionsJson, sectionCount, false);;///use new method
			Logger::Debug("AddNewBom -> CreateTableforEachSection() -> 1");
			Logger::Debug("AddNewBom -> CreateTableforEachSection() -> sectionCountJson" + to_string(sectionCountJson));
			string sectionId = Helper::GetJSONValue<string>(sectionCountJson, ATTRIBUTE_ID, true);
			Logger::Debug("AddNewBom -> CreateTableforEachSection() -> sectionId" + sectionId);
			string sectionName = Helper::GetJSONValue<string>(sectionCountJson, "node_name", true);


			Section* section = new Section(QString::fromStdString(sectionName), 300);

			sectionTable = new QTableWidget(section);
			sectionTable->setProperty("TableName", QString::fromStdString(sectionName));
			sectionTable->setProperty("SectionId", QString::fromStdString(sectionId));
			sectionTable->setColumnCount(tablecolumnList.size());
			sectionTable->setHorizontalHeaderLabels(tablecolumnList);
			Logger::Debug("AddNewBom -> CreateTableforEachSection() -> sectionName" + sectionName);
			if (sectionName == "Fabrics")
			{
				
				Logger::Debug("AddNewBom -> CreateTableforEachSection() -> sectionTable" + to_string(long(sectionTable)));
				getMaterialDetails("fabricList", CreateProduct::GetInstance()->m_techPackJson, true, sectionTable, QString::fromStdString(sectionName),true);
			}
			if (sectionName == "Trims")
			{
				Logger::Debug("AddNewBom -> CreateTableforEachSection() -> sectionTable" + to_string(long(sectionTable)));
				getMaterialDetails("buttonHeadList", CreateProduct::GetInstance()->m_techPackJson, false, sectionTable, QString::fromStdString(sectionName), false);
				getMaterialDetails("buttonHoleList", CreateProduct::GetInstance()->m_techPackJson, false, sectionTable, QString::fromStdString(sectionName), false);
				getMaterialDetails("zipperList", CreateProduct::GetInstance()->m_techPackJson, false, sectionTable, QString::fromStdString(sectionName), false);
			}


			m_bomSectionTableInfoMap.insert(make_pair(sectionName, sectionTable));
			//sectionTable->setStyleSheet("QTableWidget{ background-color: #262628; border-right: 1px solid #000000; border-top: 1px solid #000000; border-left: 1px solid #000000; font-face: ArialMT; font-size: 12px; color: #FFFFFF; }");
			sectionTable->verticalHeader()->hide();
			sectionTable->setShowGrid(false);
			sectionTable->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
			sectionTable->setSelectionMode(QAbstractItemView::NoSelection);
			sectionTable->horizontalHeader()->setStretchLastSection(true);
			auto* anyLayout = new QVBoxLayout();
			auto* addMaterialButtonLayout = new QHBoxLayout();
			QPushButton* addPlmMaterialButton = CVWidgetGenerator::CreatePushButton("New From Material", ADD_HOVER_ICON_PATH, "New From Material", PUSH_BUTTON_STYLE, 30, true);
			addPlmMaterialButton->setParent(sectionTable);
			addPlmMaterialButton->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);

			QPushButton* addSpecialMaterialButton = CVWidgetGenerator::CreatePushButton("New Special", ADD_HOVER_ICON_PATH, "New Special", PUSH_BUTTON_STYLE, 30, true);
			addSpecialMaterialButton->setParent(sectionTable);
			addSpecialMaterialButton->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);


			//QAction* AddFromMaterialAction = new QAction(tr("New From Material"), this);
			//connect(AddFromMaterialAction, SIGNAL(triggered()), this, SLOT(onClickAddFromMaterialButton()));

			//QAction* AddSpecialMaterialAction = new QAction(tr("New Special"), this);
			//connect(AddSpecialMaterialAction, SIGNAL(triggered()), this, SLOT(onClickAddSpecialMaterialButton()));

			m_addMaterialButtonAndTableMap.insert(make_pair(addPlmMaterialButton, sectionTable));
			m_addSpecialMaterialButtonAndTableMap.insert(make_pair(addSpecialMaterialButton, sectionTable));
			//QMenu* menu = new QMenu(addMaterialButton);
			//menu->addAction(AddFromMaterialAction);
			//menu->addAction(AddSpecialMaterialAction);
			//addMaterialButton->setMenu(menu);

			QSpacerItem *horizontalSpacer = new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Fixed);
			connect(addPlmMaterialButton, SIGNAL(clicked()), this, SLOT(onClickAddFromMaterialButton()));
			connect(addSpecialMaterialButton, SIGNAL(clicked()), this, SLOT(onClickAddSpecialMaterialButton()));

			addMaterialButtonLayout->insertSpacerItem(0, horizontalSpacer);
			addMaterialButtonLayout->insertWidget(1, addPlmMaterialButton);
			//addMaterialButtonLayout->insertSpacerItem(2, horizontalSpacer);
			addMaterialButtonLayout->insertWidget(2, addSpecialMaterialButton);
			anyLayout->insertLayout(0, addMaterialButtonLayout);
			anyLayout->insertWidget(1, sectionTable);
			section->setContentLayout(*anyLayout);
			section->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
			CreateProduct::GetInstance()->ui_sectionLayout->insertWidget(sectionCount, section);

		}

	}

	void AddNewBom::getMaterialDetails(string _str, json _techPackJson, bool _isfabric, QTableWidget* _sectionTable, QString _tableName, bool _isFabric)
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
				getColorInfo(materialCountJson, rowDataJson, objectId, _isFabric);




				rowDataJson["Code"] = code;
				rowDataJson["material_name"] = name;
				rowDataJson["Type"] = type;
				rowDataJson["comment"] = description;
				rowDataJson["qty_default"] = quantityVal;
				rowDataJson["uom"] = uom;
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
			 if (bomTableColumnKeys[columnIndex] == "Type")
			{
				ComboBoxItem* comboType = new ComboBoxItem();
				comboType->setStyleSheet("QComboBox{max-height: 25px; min-width: 100px;}");
				comboType->setFocusPolicy(Qt::StrongFocus);
				QStringList materialType;
				for (auto itr = m_materialTypeNameIdMap.begin(); itr != m_materialTypeNameIdMap.end(); itr++)
				{
					itr->first;
					materialType.append(QString::fromStdString(itr->second));
					comboType->setProperty(itr->second.c_str(), QString::fromStdString(itr->first));
				}

				comboType->addItems(materialType);
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
				comboType->setProperty("rest_api_name", bomTableColumnKeys[columnIndex]);

				//comboType->setProperty("Id", QString::fromStdString(_objectId));
				QWidget *pWidget = CVWidgetGenerator::InsertWidgetInCenter(comboType);
				//comboColorwayItem->clear();
				//comboColorwayItem->addItems(_colorwayNamesList);
				_sectionTable->setCellWidget(rowCount, columnIndex, pWidget);

				//QObject::connect(comboColorwayItem, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(OnHandleColorwayNameComboBox(const QString&)));
			}
			else{
					
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
					newColumn->setProperty("rest_api_name", bomTableColumnKeys[columnIndex]);

					_sectionTable->setCellWidget(rowCount, columnIndex, pLineEditWidget);
					Logger::Debug("AddNewBom -> AddBomRows() -> 8");
				}
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
											//int number = rowCount * 10 + columnIndex;
											//QString("%1-%2").arg(row).arg(col)
											m_buttonSignalMapper->setMapping(pushButton_2, QString("%1-%2").arg(rowCount).arg(columnIndex));
										}
										pushButton_2->setProperty("TableName", _tableName);
										gridLayout->addWidget(pushButton_2, 0, 1, 1, 1, Qt::AlignHCenter);
										p_widget->setLayout(gridLayout);
										p_widget->setProperty("rest_api_name", bomTableColumnKeys[columnIndex]);
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
									newButton->setProperty("rest_api_name", bomTableColumnKeys[columnIndex]);
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

						QPixmap pixmap;
						Logger::Debug("AddNewBom -> AddBomRows() -> 2");
						QLabel* label = new QLabel();

						QImage styleIcon;
						QImageReader imageReader(":/CLOVise/PLM/Images/NoImage.png");
						imageReader.setDecideFormatFromContent(true);
						styleIcon = imageReader.read();
						pixmap = QPixmap::fromImage(styleIcon);

						label->setMaximumSize(QSize(20, 20));
						int w = label->width();
						int h = label->height();
						label->setPixmap(QPixmap(pixmap.scaled(w, h, Qt::KeepAspectRatio)));
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
							//int number = rowCount * 10 + columnIndex;
							m_buttonSignalMapper->setMapping(pushButton_2, QString("%1-%2").arg(rowCount).arg(columnIndex));
							//m_buttonSignalMapper->setMapping(pushButton_2, number);
						}
						pushButton_2->setProperty("TableName", _tableName);
						gridLayout->addWidget(pushButton_2, 0, 1, 1, 1, Qt::AlignHCenter);
						p_widget->setLayout(gridLayout);
						p_widget->setProperty("rest_api_name", bomTableColumnKeys[columnIndex]);
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

						QPixmap pixmap;
						Logger::Debug("AddNewBom -> AddBomRows() -> 2");
						QLabel* label = new QLabel();

						QImage styleIcon;
						QImageReader imageReader(":/CLOVise/PLM/Images/NoImage.png");
						imageReader.setDecideFormatFromContent(true);
						styleIcon = imageReader.read();
						pixmap = QPixmap::fromImage(styleIcon);

						label->setMaximumSize(QSize(20, 20));
						int w = label->width();
						int h = label->height();
						label->setPixmap(QPixmap(pixmap.scaled(w, h, Qt::KeepAspectRatio)));
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
							//int number = rowCount * 10 + columnIndex;
							//m_buttonSignalMapper->setMapping(pushButton_2, number);
							m_buttonSignalMapper->setMapping(pushButton_2, QString("%1-%2").arg(rowCount).arg(columnIndex));
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
						p_widget->setProperty("rest_api_name", bomTableColumnKeys[columnIndex]);
						//QWidget *pLineEditWidget = CVWidgetGenerator::InsertWidgetInCenter(p_widget);
						Logger::Debug("AddNewBom -> AddBomRows() -> 10");
						_sectionTable->setCellWidget(rowCount, columnIndex, p_widget);
					}
					else if (bomTableColumnKeys[columnIndex] == "Delete")
					{
						QPushButton *deleteButton = CVWidgetGenerator::CreatePushButton("", ":/CLOVise/PLM/Images/icon_delete_over.svg", "Delete", PUSH_BUTTON_STYLE, 30, true);
						deleteButton->setStyleSheet(BUTTON_STYLE);
						deleteButton->setProperty("materialId", QString::fromStdString(materialId));
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
						deleteButton->setProperty("rest_api_name", bomTableColumnKeys[columnIndex]);
					//	pPushButton->setProperty("materialId", QString::fromStdString(materialId));
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
						newColumn->setProperty("rest_api_name", bomTableColumnKeys[columnIndex]);
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
		QPushButton* button = (QPushButton*)sender();
		QTableWidget* sectionTable;
		currentAddMaterialButtonClicked = button;
		auto it = AddNewBom::GetInstance()->m_addMaterialButtonAndTableMap.find(button);
		if (it != AddNewBom::GetInstance()->m_addMaterialButtonAndTableMap.end())
		{
			sectionTable = it->second;
			QString tableName = sectionTable->property("TableName").toString();
			/*auto itr = m_bomSectionTableInfoMap.find(tableName.toStdString());
			json placementProductTypeJson;
			string queryParamsForMaterial;
			if (itr != m_bomSectionTableInfoMap.end())
			{
				sectionInfo sectionInfoObj = itr->second;
				placementProductTypeJson = sectionInfoObj.bomPlacementProductTypeJson;
				for (int i = 0; i < placementProductTypeJson.size(); i++)
				{
					string bomPlacementProductTypeId = Helper::GetJSONValue<int>(placementProductTypeJson, i, true);
					queryParamsForMaterial = queryParamsForMaterial + "&product_type=" + bomPlacementProductTypeId;

				}
				Configuration::GetInstance()->SetQueryParameterForMaterial(queryParamsForMaterial);
			}*/
		}
		Logger::Debug("AddNewBom -> onClickAddFromMaterialButton () button" + to_string(long(button)));
		CreateProduct::GetInstance()->hide();
		UTILITY_API->CreateProgressBar();
		Configuration::GetInstance()->SetProgressBarProgress(qrand() % 101);
		MaterialConfig::GetInstance()->SetMaximumLimitForMaterialResult();
		RESTAPI::SetProgressBarData(Configuration::GetInstance()->GetProgressBarProgress(), "Loading " + Configuration::GetInstance()->GetLocalizedMaterialClassName() + " Search", true);
		int isFromConstructor = false;
		if (!MaterialConfig::GetInstance()->GetIsModelExecuted())
		{
			MaterialConfig::GetInstance()->InitializeMaterialData();
			isFromConstructor = true;
		}
		else
			PLMMaterialSearch::GetInstance()->DrawSearchWidget(isFromConstructor);

		MaterialConfig::GetInstance()->SetIsRadioButton(true);

		PLMMaterialSearch::GetInstance()->setModal(true);
		UTILITY_API->DeleteProgressBar(true);
		MaterialConfig::GetInstance()->SetIsModelExecuted(true);
		PLMMaterialSearch::GetInstance()->exec();
		PLMMaterialSearch::GetInstance()->show();
		Logger::Debug("AddNewBom -> onClickAddFromMaterialButton () End");
	}
	void AddNewBom::onClickAddSpecialMaterialButton()
	{
		Logger::Debug("AddNewBom -> onClickAddSpecialMaterialButton () Start");
		QTableWidget* sectionTable;
		QPushButton* button = (QPushButton*)sender();
		auto it = AddNewBom::GetInstance()->m_addSpecialMaterialButtonAndTableMap.find(button);
		if (it != AddNewBom::GetInstance()->m_addSpecialMaterialButtonAndTableMap.end())
		{
			sectionTable = it->second;
			QString tableName = sectionTable->property("TableName").toString();
			json rowDataJson = json::object();
			rowDataJson["Code"] = "";
			rowDataJson["material_name"] = "";
			rowDataJson["Type"] = "";
			rowDataJson["comment"] = "";
			rowDataJson["qty_default"] = "";
			rowDataJson["uom"] = "";
			rowDataJson["materialId"] = "";
			AddBomRows(sectionTable, rowDataJson, tableName);
		}

		Logger::Debug("AddNewBom -> onClickAddSpecialMaterialButton () End");
	}


	void AddNewBom::getColorInfo(json _FabricJson, json& _rowDataJson, string _materailId, bool _isFabric)
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
			string frontJsonStr;
			if(_isFabric)
				frontJsonStr = Helper::GetJSONValue<string>(cloColorwayJson, "materialFront", false);
			else
				frontJsonStr = Helper::GetJSONValue<string>(cloColorwayJson, "bodyMaterial", false);
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
		tablecolumnList = m_bomTableColumnlist;
		bomTableColumnKeys = m_bomTableColumnKeys;
		if (CreateProduct::GetInstance()->m_mappedColorways.size() && m_bomSectionTableInfoMap.size() > 0)
		{
			
			tablecolumnList.append(CreateProduct::GetInstance()->m_mappedColorways);			
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
			QString tableName = QString::fromStdString(itr->first);
			//Logger::Debug("AddNewBom -> UpdateColorwayColumns () 11");
			sectionTable->setColumnCount(bomTableColumnKeys.size());
			sectionTable->setHorizontalHeaderLabels(tablecolumnList);
			Logger::Debug("AddNewBom -> UpdateColorwayColumns () sectionTable->rowCount()" + to_string(sectionTable->rowCount()));
			for (int rowCount = 0; rowCount < sectionTable->rowCount(); rowCount++)
			{
				QComboBox* typeCombo;
				typeCombo = static_cast<QComboBox*>(sectionTable->cellWidget(rowCount, 0)->children().last());
				QString matrialId = typeCombo->property("materialId").toString();
				Logger::Debug("AddNewBom -> UpdateColorwayColumns () matrialId" + matrialId.toStdString());
				
					Logger::Debug("AddNewBom -> UpdateColorwayColumns () sectionTable->columnCount()" + to_string(sectionTable->columnCount()));
					for (int columnIndex = 0; columnIndex < sectionTable->columnCount(); columnIndex++)
					{
						QString columnName = sectionTable->horizontalHeaderItem(columnIndex)->text();
						if (FormatHelper::HasContent(matrialId.toStdString()))
						{
							
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
										QWidget* p_widget = new QWidget(sectionTable);
										QGridLayout* gridLayout = new QGridLayout(sectionTable);
										gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
										gridLayout->setContentsMargins(0, 0, 0, 0);
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

										QWidget *colorchip = nullptr;
										colorchip = CVWidgetGenerator::InsertWidgetInCenter(label);
										gridLayout->addWidget(colorchip, 0, 0, 1, 1, Qt::AlignHCenter);
										QPushButton* pushButton_2 = CVWidgetGenerator::CreatePushButton("", ADD_HOVER_ICON_PATH, "", PUSH_BUTTON_STYLE, 30, true);
										pushButton_2->setFixedHeight(20);
										pushButton_2->setFixedWidth(20);
										if (m_buttonSignalMapper != nullptr)
										{
											m_buttonSignalMapper->setProperty("TableName", tableName);
											connect(pushButton_2, SIGNAL(clicked()), m_buttonSignalMapper, SLOT(map()));
											//int number = rowCount * 10 + columnIndex;
											m_buttonSignalMapper->setMapping(pushButton_2, QString("%1-%2").arg(rowCount).arg(columnIndex));
											//m_buttonSignalMapper->setMapping(pushButton_2, number);
										}
										pushButton_2->setProperty("TableName", tableName);
										gridLayout->addWidget(pushButton_2, 0, 1, 1, 1, Qt::AlignHCenter);
										p_widget->setLayout(gridLayout);
										sectionTable->setCellWidget(rowCount, columnIndex, p_widget);
										Logger::Debug("AddNewBom -> AddBomRows() -> 7");
										//colorChip = true;
									}
								}
							}

						}
						else
						{
							if (CreateProduct::GetInstance()->m_mappedColorways.contains(columnName))
							{
								QWidget* p_widget = new QWidget(sectionTable);
								QGridLayout* gridLayout = new QGridLayout(sectionTable);
								gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
								gridLayout->setContentsMargins(0, 0, 0, 0);

								QPixmap pixmap;
								Logger::Debug("AddNewBom -> AddBomRows() -> 2");
								QLabel* label = new QLabel();

								QImage styleIcon;
								QImageReader imageReader(":/CLOVise/PLM/Images/NoImage.png");
								imageReader.setDecideFormatFromContent(true);
								styleIcon = imageReader.read();
								pixmap = QPixmap::fromImage(styleIcon);

								label->setMaximumSize(QSize(20, 20));
								int w = label->width();
								int h = label->height();
								label->setPixmap(QPixmap(pixmap.scaled(w, h, Qt::KeepAspectRatio)));
								Logger::Debug("AddNewBom -> AddBomRows() -> 3");
								QWidget *colorchip = nullptr;
								colorchip = CVWidgetGenerator::InsertWidgetInCenter(label);

								gridLayout->addWidget(colorchip, 0, 0, 1, 1, Qt::AlignHCenter);
								QPushButton* pushButton_2 = CVWidgetGenerator::CreatePushButton("", ADD_HOVER_ICON_PATH, "", PUSH_BUTTON_STYLE, 30, true);
								pushButton_2->setFixedHeight(20);
								pushButton_2->setFixedWidth(20);
								if (m_buttonSignalMapper != nullptr)
								{
									m_buttonSignalMapper->setProperty("TableName", tableName);
									connect(pushButton_2, SIGNAL(clicked()), m_buttonSignalMapper, SLOT(map()));
									//int number = rowCount * 10 + columnIndex;
									m_buttonSignalMapper->setMapping(pushButton_2, QString("%1-%2").arg(rowCount).arg(columnIndex));
									//m_buttonSignalMapper->setMapping(pushButton_2, number);
								}
								pushButton_2->setProperty("TableName", tableName);
								gridLayout->addWidget(pushButton_2, 0, 1, 1, 1, Qt::AlignHCenter);
								p_widget->setLayout(gridLayout);

								Logger::Debug("AddNewBom -> AddBomRows() -> 10");
								sectionTable->setCellWidget(rowCount, columnIndex, p_widget);
							}
						}
					}
				
			}
		}

		Logger::Debug("AddNewBom -> UpdateColorwayColumns () End");
	}

	void AddNewBom::OnClickAddColorButton(const QString &position)
	{
		Logger::Debug("AddNewBom -> OnClickUpdateColorButton () Start");
		CreateProduct::GetInstance()->hide();
		QSignalMapper* button = (QSignalMapper*)sender();
		Logger::Debug("AddNewBom -> OnClickUpdateColorButton () string" + position.toStdString());
		QStringList coordinates = position.split("-");
		int row = coordinates[0].toInt();
		int col = coordinates[1].toInt();
		//int row, column;
		//if (number < 10)
		//{
		//	if (number == 0)
		//	{
		//		row = 0;
		//		column = 0;
		//	}
		//	else
		//	{
		//		row = 0;
		//		column = number;
		//	}
		//}
		//else
		//{
		//	row = number / 10;
		//	column = number % 10;
		//}
		Logger::Debug("AddNewBom -> OnClickUpdateColorButton () row" + to_string(row));
		Logger::Debug("AddNewBom -> OnClickUpdateColorButton () column" + to_string(col));

		m_currentTableName = button->property("TableName").toString().toStdString();
		m_currentRow = row;
		m_currentColumn = col;
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

		Logger::Debug("AddNewBom -> OnClickDeleteButton () tableName" + tableName);
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
	void AddNewBom::OnHandleDropDownValue(const QString& _item)
	{

		try
		{
			Logger::Debug("Create product OnHandleDropDownValue() Start");
			
			if (!_item.isEmpty())
			{
				map<string, string>::iterator it;
				map < string, string> nameIdMap;
				string id;

				QString labelName = sender()->property("LabelName").toString();
				string Id = sender()->property(_item.toStdString().c_str()).toString().toStdString();
				string apiUrl = "";
				QString comboboxtofill = "";

				Logger::Debug("CreateProduct -> OnHandleDropDownValue() Id: " + Id);
				Logger::Debug("CreateProduct -> OnHandleDropDownValue() LabelName: " + labelName.toStdString());
				string progressbarText;
				Logger::Debug("PublishToPLMData -> OnHandleDropDownValue() _item: " + _item.toStdString());

				string subtypeId;
					for (int i = 0; i < m_bomTemplateJson.size(); i++)
					{
						json attJson = Helper::GetJSONParsedValue<int>(m_bomTemplateJson, i, false);;///use new method
						
						string attId = Helper::GetJSONValue<string>(attJson, ATTRIBUTE_ID, true);
						if (attId == Id)
						{
							Logger::Debug("CreateProduct -> OnHandleDropDownValue() attId: " + attId);
							subtypeId = Helper::GetJSONValue<string>(attJson, "subtype", true);
							Logger::Debug("CreateProduct -> OnHandleDropDownValue() subtype: " + subtypeId);
							break;
						}
					}


					for (int itemIndex = 0; itemIndex < m_createBomTreeWidget->topLevelItemCount(); ++itemIndex)
					{
						QTreeWidgetItem* topItem = m_createBomTreeWidget->topLevelItem(itemIndex);
						QWidget* qWidgetColumn_0 = m_createBomTreeWidget->itemWidget(topItem, 0);
						QWidget* qWidgetColumn_1 = m_createBomTreeWidget->itemWidget(topItem, 1);
						if (!qWidgetColumn_0 || !qWidgetColumn_1)
						{
							continue;
						}
						QLabel* qlabel = qobject_cast<QLabel*>(qWidgetColumn_0);
						QString lableText = qlabel->property(ATTRIBUTE_KEY.c_str()).toString();

						Logger::Debug("PublishToPLMData -> OnHandleDropDownValue() Label text: " + (lableText.toStdString()));

							ComboBoxItem* qComboBox = qobject_cast<ComboBoxItem*>(qWidgetColumn_1);
							if ((qComboBox))
							{
								if (labelName.contains("Template") && lableText.contains("subtype"))
								{
									QString subtypeValue;
									if(!subtypeId.empty())
									 subtypeValue = qComboBox->property(subtypeId.c_str()).toString();

									int valueIndex = qComboBox->findText(subtypeValue);
									Logger::Debug("PublishToPLMData -> OnHandleDropDownValue() valueIndex: " + to_string(valueIndex));

									if (valueIndex == -1) // -1 for not found
									{
										int index = qComboBox->findText(QString::fromStdString(BLANK));
										qComboBox->setCurrentIndex(index);
									}
									else
									{
										qComboBox->setCurrentIndex(valueIndex);
									}
									break;
								}
								
								
							}
						}
					}



			UTILITY_API->DeleteProgressBar(true);
		}
		catch (string msg)
		{
			UTILITY_API->DeleteProgressBar(true);
			Logger::Error("Create product OnHandleDropDownValue()-> Exception string:: " + msg);
			UTILITY_API->DisplayMessageBox(msg);
			this->show();
		}
		catch (exception& e)
		{
			UTILITY_API->DeleteProgressBar(true);
			Logger::Error("Create product OnHandleDropDownValue()-> Exception e:: " + string(e.what()));
			UTILITY_API->DisplayMessageBox(e.what());
			this->show();
		}
		catch (const char* msg)
		{
			UTILITY_API->DeleteProgressBar(true);
			Logger::Error("Create product OnHandleDropDownValue()-> Exception char:: " + string(msg));
			wstring wstr(msg, msg + strlen(msg));
			UTILITY_API->DisplayMessageBoxW(wstr);
			this->show();
		}

		Logger::Debug("Create product OnHandleDropDownValue() End");
	}

	void AddNewBom::BackupBomDetails()
	{
		Logger::Debug("AddNewBom -> BackupBomDetails() -> Start");
		try {
			for (auto itr = m_bomSectionTableInfoMap.begin(); itr != m_bomSectionTableInfoMap.end(); itr++)
			{
				//sectionInfo sectionInfoObj = 
				QTableWidget* sectionTable = itr->second;
				string sectionId = sectionTable->property("SectionId").toString().toStdString();
				for (int rowCount = 0; rowCount < sectionTable->rowCount(); rowCount++)
				{
					json attJson = json::object();
					for (int columnCount = 0; columnCount < sectionTable->columnCount(); columnCount++)
					{
						string fieldValue;

						QWidget* qcolumnWidget = (QWidget*)sectionTable->cellWidget(rowCount, columnCount)->children().last();
						string attInternalName = qcolumnWidget->property("rest_api_name").toString().toStdString();
						Logger::Debug("Create product CreateBom() attInternalName" + attInternalName);
						if (QLineEdit* qLineEditC1 = qobject_cast<QLineEdit*>(qcolumnWidget))
						{

							fieldValue = qLineEditC1->text().toStdString();
							//QString columnName = sectionTable->horizontalHeaderItem(columnCount)->text();
						}
						else if (QTextEdit* qTextC1 = qobject_cast<QTextEdit*>(qcolumnWidget))
						{

							fieldValue = qTextC1->toPlainText().toStdString();
						}
						else if (QDateEdit* qDateC1 = qobject_cast<QDateEdit*>(qcolumnWidget))
						{
fieldValue = FormatHelper::RetrieveDate(qDateC1);
if (fieldValue.find(DATE_FORMAT_TEXT.toStdString()) != string::npos)
{
	fieldValue = "";
}
else
{
	//UTILITY_API->DisplayMessageBox("fieldValue::" + fieldValue);
	fieldValue = fieldValue + "T00:00:00Z";
}
						}
						else if (QListWidget* listC1 = qobject_cast<QListWidget*>(qcolumnWidget))
						{
						string tempValue = "";
						QListWidgetItem* listItem = nullptr;
						for (int row = 0; row < listC1->count(); row++)
						{
							listItem = listC1->item(row);
							if (listItem->checkState())
							{
								tempValue = listItem->text().toStdString();
								if (FormatHelper::HasContent(tempValue))
								{
									tempValue = listC1->property(tempValue.c_str()).toString().toStdString();
									if (FormatHelper::HasContent(tempValue))
									{
										tempValue = tempValue + DELIMITER_NEGATION;
										fieldValue = fieldValue + tempValue;
									}
								}
							}
						}
						}
						else if (QPushButton* pushButton = qobject_cast<QPushButton*>(qcolumnWidget))
						{
						if (attInternalName == "Delete")
						{
							fieldValue = pushButton->property("materialId").toString().toStdString();
							attInternalName = "actual";
						}
						}
						else if (QSpinBox* SpinC1 = qobject_cast<QSpinBox*>(qcolumnWidget))
						{
						if (SpinC1->value() != 0)
						{
							fieldValue = to_string(SpinC1->value());
						}
						}
						else if (QComboBox* qComboBoxC1 = qobject_cast<QComboBox*>(qcolumnWidget))
						{
						fieldValue = qComboBoxC1->currentText().toStdString();

						Logger::Debug("AddNewBom BackupBomDetails() QComboBox->fieldLabel" + attInternalName);
						//Logger::Debug("Create product ReadVisualUIFieldValue() QComboBox->labelText" + labelText);

						string fieldVal = qComboBoxC1->property(fieldValue.c_str()).toString().toStdString();
						Logger::Debug("Create product BackupBomDetails() QComboBox->fieldVal" + fieldVal);
						if (!fieldVal.empty())
						{
							fieldValue = fieldVal;
						}
						Logger::Debug("AddNewBom BackupBomDetails() QComboBox->fieldValue" + fieldValue);
						}
						if (!attInternalName.empty() && !fieldValue.empty())
						{
							if (attInternalName == "qty_default")
								attJson[attInternalName] = atoi(fieldValue.c_str());
							else
								attJson[attInternalName] = fieldValue;
						}
						Logger::Debug("AddNewBom BackupBomDetails() fieldValue" + fieldValue);
					}
					attJson["ds_section"] = sectionId;


					Logger::Debug("AddNewBom BackupBomDetails() attJson" + to_string(attJson));


					//UTILITY_API->DisplayMessageBox("attJson" + to_string(attJson));
				}
				//sectionTable->model()->removeRows(0, sectionTable->rowCount());
				sectionTable->clearContents();
				sectionTable->setRowCount(0);
				//sectionTable->clear();

			}
			Logger::Debug("AddNewBom -> BackupBomDetails() -> End");
		}
		catch (exception& e)
		{
			RESTAPI::SetProgressBarData(0, "", false);
			Logger::Error("AddNewBom -> AddNewBom Exception - " + string(e.what()));
			UTILITY_API->DisplayMessageBox(e.what());
			//Helper::RemoveDirectory(QString::fromStdString(Configuration::GetInstance()->TURNTABLE_IMAGES_TEMP_DIRECTORY));
			//dir.mkpath(QString::fromStdString(Configuration::GetInstance()->TURNTABLE_IMAGES_TEMP_DIRECTORY));

			this->show();
		}
	}
	void AddNewBom::ClearBomData()
	{
	m_addMaterialButtonAndTableMap.clear();
	m_addSpecialMaterialButtonAndTableMap.clear();
	m_colorwayMapForBom.clear();
	m_bomSectionTableInfoMap.clear();
	m_mappedColorwaysArr.clear();
	m_colorwayOverridesJson.clear();
	m_BomMetaData.clear();
}
}