/*
* Copyright 2021-2022 CLO-Vise. All rights reserved
*
* @file AddNewBom.cpp
*
* @brief Class implementation for create Bom table on tab.
* This class has all the variable and methods implementation which are used to create bom table and publish bom lines from CLO to PLM.
*
* @author GoVise
*
* @date 10-OCT-2021
*/
#include "AddNewBom.h"

#include <cstring>

#include <QTextEdit>
#include <QComboBox>
#include <QLabel>
#include "qtreewidget.h"
#include <QFile>
#include "qdir.h"

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
#include "CLOVise/PLM/Outbound/PublishToPLM/UpdateProduct.h"
#include "CLOVise/PLM/Inbound/Material/PLMMaterialSearch.h"
#include "CLOVise/PLM/Inbound/Material/MaterialConfig.h"
#include "CLOVise/PLM/Outbound/PublishToPLM/BOM/UpdateProductBOMHandler.h"
#include "CLOVise/PLM/Outbound/PublishToPLM/BOM/CreateProductBOMHandler.h"

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
	
		m_createButton = CVWidgetGenerator::CreatePushButton("Create", ADD_HOVER_ICON_PATH, "Create", PUSH_BUTTON_STYLE, 30, true);
		m_backButton = CVWidgetGenerator::CreatePushButton("Back", BACK_HOVER_PATH, "Back", PUSH_BUTTON_STYLE, 30, true);

		m_createBOMTreeWidget = CVWidgetGenerator::CreatePublishTreeWidget("QTreeWidget { background-color: #262628; border: 1px solid #000;}""QTreeWidget::item { padding :5px; height: 20px; color: #FFFFFF; font-face: ArialMT; font-size: 10px; width: 90px; margin-left: 5px; margin-right: 5px; margin-top: 5px; margin-bottom: 5px; border: none;}""QTreeWidget::item:hover{background-color: #262628;}", true);
		m_createBOMTreeWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
		horizontalLayout->insertWidget(0, m_createBOMTreeWidget);
		m_createBOMTreeWidget->setMinimumHeight(600);
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
		json bomConfigjson = json::object();
		string attJson = DirectoryUtil::GetPLMPluginDirectory() + "BomConfig.json";//Reading Columns from json
		bomConfigjson = Helper::ReadJSONFile(attJson);
		jsonvalueArray = Helper::GetJSONParsedValue<string>(bomConfigjson, "CreateBomAttributes", false);

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

		drawWidget(mergedJsonArray, m_createBOMTreeWidget);
		connectSignalSlots(true);

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


		}
		else
		{
			QObject::disconnect(m_backButton, SIGNAL(clicked()), this, SLOT(onBackButtonClicked()));
			QObject::disconnect(m_createButton, SIGNAL(clicked()), this, SLOT(onCreateButtonClicked()));
		}
	}

	/*
* Description - drawWidget() method used to create input fields.
* Parameter -  json, QTreeWidget
* Exception -
* Return -
*/

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
						responseJson = Helper::makeRestcallGet(RESTAPI::BOM_TEMPLATE_API, "?parent=centric:&limit=" + Configuration::GetInstance()->GetMaximumLimitForRefAttValue(), "", "Loading template details..");
						m_BOMTemplateJson = responseJson;
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

	/*
	* Description - onBackButtonClicked() method is a slot for back button click and close the create bom dialog.
	* Parameter -
	* Exception -
	* Return -
	*/
	void AddNewBom::onBackButtonClicked()
	{
		Logger::Debug("AddNewBom onBackButtonClicked() Start....");
		this->hide();
		if (Configuration::GetInstance()->GetCurrentScreen() == CREATE_PRODUCT_CLICKED)
		{
			CreateProduct::GetInstance()->setModal(true);
			CreateProduct::GetInstance()->show();
		}
		if (Configuration::GetInstance()->GetCurrentScreen() == UPDATE_PRODUCT_CLICKED)
		{
			UpdateProduct::GetInstance()->setModal(true);
			UpdateProduct::GetInstance()->show();
		}
		Logger::Debug("AddNewBom onBackButtonClicked() End....");
	}

	/*
* Description - onCreateButtonClicked() method is a slot for create button and create sections as per the template selected for create Bom.
* Parameter -
* Exception -
* Return -
*/
	void AddNewBom::onCreateButtonClicked()
	{
		Logger::Debug("AddNewBom onCreateButtonClicked() Start....");

		try
		{
			this->hide();
			Configuration::GetInstance()->SetProgressBarProgress(0);
			UTILITY_API->CreateProgressBar();
			Configuration::GetInstance()->SetProgressBarProgress(qrand() % 101);
			RESTAPI::SetProgressBarData(20, "Loading BOM data ", true);
			QTreeWidget *tree = new QTreeWidget();
			UIHelper::ValidateRquired3DModelData(m_createBOMTreeWidget);
			if (Configuration::GetInstance()->GetCurrentScreen() == CREATE_PRODUCT_CLICKED)
			m_BOMMetaData = CreateProduct::GetInstance()->collectCriteriaFields(m_createBOMTreeWidget, tree);
			if (Configuration::GetInstance()->GetCurrentScreen() == UPDATE_PRODUCT_CLICKED)
			{
				string bomMetaDataStr = UpdateProduct::GetInstance()->collectCriteriaFields(m_createBOMTreeWidget, tree);
				m_BOMMetaData = json::parse(bomMetaDataStr);
			}

			string bomName = Helper::GetJSONValue<string>(m_BOMMetaData, "node_name", true);
			string bomTemplateId = Helper::GetJSONValue<string>(m_BOMMetaData, "bom_template", true);
			string latestRevision;

			for (int i = 0; i < m_BOMTemplateJson.size(); i++)
			{
				json attJson = Helper::GetJSONParsedValue<int>(m_BOMTemplateJson, i, false);;///use new method
				string attId = Helper::GetJSONValue<string>(attJson, ATTRIBUTE_ID, true);
				Logger::Debug("PublishToPLMData -> SetDocumentConfigJSON attId: " + attId);
				if (attId == bomTemplateId)
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

				if (Configuration::GetInstance()->GetCurrentScreen() == CREATE_PRODUCT_CLICKED)
					CreateProductBOMHandler::GetInstance()->CreateBom(sectionIdsJson);

				if (Configuration::GetInstance()->GetCurrentScreen() == UPDATE_PRODUCT_CLICKED)
					UpdateProductBOMHandler::GetInstance()->CreateBom(sectionIdsJson);
				
			}

			if (Configuration::GetInstance()->GetCurrentScreen() == UPDATE_PRODUCT_CLICKED)

			{
				UpdateProduct::GetInstance()->m_bomAddButton->hide();
				if (FormatHelper::HasContent(bomName))
					UpdateProduct::GetInstance()->m_bomName->setText(QString::fromStdString(bomName));
				else
					UpdateProduct::GetInstance()->m_bomName->setText("");
				UpdateProductBOMHandler::GetInstance()->m_bomCreated = true;
				UTILITY_API->DeleteProgressBar(true);
				UpdateProduct::GetInstance()->setModal(true);
				UpdateProduct::GetInstance()->show();
			}
			if (Configuration::GetInstance()->GetCurrentScreen() == CREATE_PRODUCT_CLICKED)

			{
				CreateProduct::GetInstance()->m_bomAddButton->hide();
				if (FormatHelper::HasContent(bomName))
					CreateProduct::GetInstance()->m_bomName->setText(QString::fromStdString(bomName));
				else
					CreateProduct::GetInstance()->m_bomName->setText("");
				CreateProductBOMHandler::GetInstance()->m_bomCreated = true;
				UTILITY_API->DeleteProgressBar(true);
				CreateProduct::GetInstance()->setModal(true);
				CreateProduct::GetInstance()->show();
			}
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


	/*
Description - OnHandleDropDownValue() method used to fill the dependecy fields on bom create dialog.
	* Parameter -QString
	* Exception -
	*Return -
	*/
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
				for (int i = 0; i < m_BOMTemplateJson.size(); i++)
				{
					json attJson = Helper::GetJSONParsedValue<int>(m_BOMTemplateJson, i, false);;///use new method

					string attId = Helper::GetJSONValue<string>(attJson, ATTRIBUTE_ID, true);
					if (attId == Id)
					{
						Logger::Debug("CreateProduct -> OnHandleDropDownValue() attId: " + attId);
						subtypeId = Helper::GetJSONValue<string>(attJson, "subtype", true);
						Logger::Debug("CreateProduct -> OnHandleDropDownValue() subtype: " + subtypeId);
						break;
					}
				}


				for (int itemIndex = 0; itemIndex < m_createBOMTreeWidget->topLevelItemCount(); ++itemIndex)
				{
					QTreeWidgetItem* topItem = m_createBOMTreeWidget->topLevelItem(itemIndex);
					QWidget* qWidgetColumn_0 = m_createBOMTreeWidget->itemWidget(topItem, 0);
					QWidget* qWidgetColumn_1 = m_createBOMTreeWidget->itemWidget(topItem, 1);
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
							if (!subtypeId.empty())
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






	
}