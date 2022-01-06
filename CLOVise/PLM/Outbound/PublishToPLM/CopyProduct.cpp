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
#include "CopyProduct.h"
#include <string>
#include <cstring>

#include <QTextEdit>
#include <QComboBox>
#include <QtGui>
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
#include "CLOVise/PLM/Inbound/Product/PLMProductSearch.h"

#include "CLOVise/PLM/Inbound/Product/PLMProductResults.h"


using namespace std;

namespace CLOVise
{
	CopyProduct* CopyProduct::_instance = NULL;

	CopyProduct* CopyProduct::GetInstance()
	{
		if (_instance == NULL) {
			_instance = new CopyProduct();
		}

		return _instance;
	}

	CopyProduct::CopyProduct(QWidget* parent) : MVDialog(parent)
	{
		setupUi(this);
		Logger::Debug("Copy Copy constructor() start....");

		QString windowTitle = PLM_NAME  + " PLM Copy " + QString::fromStdString(Configuration::GetInstance()->GetLocalizedStyleClassName());
		this->setWindowTitle(windowTitle);

#ifdef __APPLE__
		this->setWindowFlags(Qt::Dialog | Qt::CustomizeWindowHint | Qt::WindowMinMaxButtonsHint | Qt::WindowCloseButtonHint);
#else
		this->setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint | Qt::CustomizeWindowHint);
		m_pTitleBar = new MVTitleBar(windowTitle, this);
		layout()->setMenuBar(m_pTitleBar);
#endif // !__APPLE__

		m_CopyProductTreeWidget_1 = nullptr;
		m_CopyProductTreeWidget_2 = nullptr;
		m_cancelButton = nullptr;
		m_publishButton = nullptr;
		ui_ColorwayTable = nullptr;
		m_searchStyleButton = nullptr;
		m_CopyButton = nullptr;
		m_multipartFilesParams = "";
		m_copyCreated = false;
		m_CopyProductTreeWidget_1 = CVWidgetGenerator::CreatePublishTreeWidget("QTreeWidget { background-color: #262628; padding-left: 20px; min-width: 400px; }""QTreeWidget::item {font-size: 10px; font-face: ArialMT; height: 20px; width: 200px; margin-right: 20px; margin-top: 5px; margin-bottom: 5px; border: none; }""QTreeWidget::item:hover{ background-color: #262628; } QTreeView{outline: 0;} ", true);
		m_CopyProductTreeWidget_1->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
		m_CopyProductTreeWidget_2 = CVWidgetGenerator::CreatePublishTreeWidget("QTreeWidget { background-color: #262628; padding-left: 20px; min-width: 400px; }""QTreeWidget::item {font-size: 10px; font-face: ArialMT; height: 20px; width: 200px; margin-right: 20px; margin-top: 5px; margin-bottom: 5px; border: none; }""QTreeWidget::item:hover{ background-color: #262628; } QTreeView{outline: 0;} ", true);
		m_CopyProductTreeWidget_2->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

		m_HeirarchyTreeWidget = CVWidgetGenerator::CreatePublishTreeWidget("QTreeWidget { background-color: #262628;  padding-left: 20px; min-width: 400px; }""QTreeWidget::item {font-size: 10px; font-face: ArialMT; height: 20px; width: 200px; margin-right: 20px; margin-top: 5px; margin-bottom: 5px; border: none; }""QTreeWidget::item:hover{ background-color: #262628; } ", true);
		m_HeirarchyTreeWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
		m_HeirarchyTreeWidget->setMaximumHeight(125);
		m_HeirarchyTreeWidget->hide();

		m_searchStyleButton = CVWidgetGenerator::CreatePushButton("Search Golden Style", ADD_HOVER_ICON_PATH, "Search Golden Style", PUSH_BUTTON_STYLE, 30, true);
		m_cancelButton = CVWidgetGenerator::CreatePushButton("Back", BACK_HOVER_PATH, "Back", PUSH_BUTTON_STYLE, 30, true);
		m_CopyButton = CVWidgetGenerator::CreatePushButton("Copy", SAVE_HOVER_ICON_PATH, "Copy", PUSH_BUTTON_STYLE, 30, true);


		QSpacerItem *horizontalSpacer = new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Fixed);
		m_colorwayRowcount = -1;
		m_isSaveClicked = false;
		m_addClicked = false;

		QFrame *frame;
		QGridLayout *gridLayout;
		QSplitter *splitter;

		frame = new QFrame();
		frame->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
		gridLayout = new QGridLayout(frame);
		splitter = new QSplitter(frame);
		splitter->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
		splitter->setOrientation(Qt::Horizontal);
		splitter->insertWidget(0, m_CopyProductTreeWidget_1);
		splitter->insertWidget(1, m_CopyProductTreeWidget_2);
		gridLayout->setContentsMargins(QMargins(0, 0, 0, 0));
		gridLayout->addWidget(splitter, 0, 0, 1, 1);

		ui_copyTreeLayout_2->addWidget(frame);
		ui_heirarchyLayout->addWidget(m_HeirarchyTreeWidget);
		ui_tabWidget->setTabText(0, "Overview");
		ui_tabWidget->hide();
		ui_overviewTab->hide();
		QFont font;
		font.setBold(false);
		font.setFamily("ArialMT");
		ui_overviewTab->setFont(font);

		ui_tabWidget->setFont(font);
		ui_tabWidget->setCurrentIndex(0);
		ui_tabWidget->setStyleSheet(" QTabWidget::pane { border: none; color: #FFFFFF; font-size: 10px; background-color: #262628; }""QTabBar::tab { width: 100px; padding: 2px; }""QTabBar::tab:selected { border: none; color: #FFFFFF; background-color: \"" + DEFAULT_TAB_BG_COLOR + "\"; }""QTabBar::tab:!selected { color:#FFFFFF; background-color:\"" + SELECTED_TAB_BG_COLOR + "\"; }");

		//ui_treeWidgetLayout->addWidget(ui_tabWidget);

		/*ui_SearchButtonLayout->insertWidget(0, m_searchStyleButton);
		ui_SearchButtonLayout->insertSpacerItem(1, horizontalSpacer);*/

		ui_buttonsLayout->insertWidget(0, m_cancelButton);
		ui_buttonsLayout->insertSpacerItem(1, horizontalSpacer);
		ui_buttonsLayout->insertWidget(4, m_CopyButton);
		connectSignalSlots(true);

		addCopyProductDetailsWidgetData();
		Logger::Debug("Copy product constructor() end....");
		UTILITY_API->DeleteProgressBar(true);
	}

	CopyProduct::~CopyProduct()
	{
		Logger::Debug("Copy product Destructor() Start....");
		connectSignalSlots(false);
		UIHelper::DeletePointer(m_CopyProductTreeWidget_1);
		UIHelper::DeletePointer(m_CopyProductTreeWidget_2);
		UIHelper::DeletePointer(m_cancelButton);
		UIHelper::DeletePointer(m_CopyButton);
		Logger::Debug("Copy product Destructor() end....");
	}
	
	/*
	* Description - connectSignalSlots() method used to connect and disconnect signal and slots.
	* Parameter -  bool.
	* Exception -
	* Return -
	*/
	void CopyProduct::connectSignalSlots(bool _b)
	{
		if (_b)
		{
			QObject::connect(m_cancelButton, SIGNAL(clicked()), this, SLOT(onBackButtonClicked()));
			QObject::connect(m_CopyButton, SIGNAL(clicked()), this, SLOT(onPublishToPLMClicked()));
			QObject::connect(m_searchStyleButton, SIGNAL(clicked()), this, SLOT(onSearchStyleClicked()));
		}
		else
		{
			QObject::disconnect(m_cancelButton, SIGNAL(clicked()), this, SLOT(onBackButtonClicked()));
			QObject::disconnect(m_CopyButton, SIGNAL(clicked()), this, SLOT(onPublishToPLMClicked()));
			QObject::disconnect(m_searchStyleButton, SIGNAL(clicked()), this, SLOT(onSearchStyleClicked()));
		}
	}

	/*
	* Description - cancelWindowClicked() method is a slot for cancel button click and close the publish to plm.
	* Parameter -
	* Exception -
	* Return -
	*/
	void CopyProduct::onBackButtonClicked()
	{
		Logger::Debug("CopyProduct onBackButtonClicked() Start....");
		this->close();
		ClearAllFields(m_CopyProductTreeWidget_1);
		ClearAllFields(m_CopyProductTreeWidget_2);
		ClearAllFields(m_HeirarchyTreeWidget);
		ui_tabWidget->hide();
		ui_overviewTab->hide();
		UTILITY_API->CreateProgressBar();
		RESTAPI::SetProgressBarData(10, "Loading" + Configuration::GetInstance()->GetLocalizedStyleClassName() + " Search", true);
		ProductConfig::GetInstance()->GetProductFieldsJSON();
		UIHelper::ClearAllFieldsForSearch(PLMProductSearch::GetInstance()->GetTreewidget(0));
		UIHelper::ClearAllFieldsForSearch(PLMProductSearch::GetInstance()->GetTreewidget(1));
		UTILITY_API->SetProgress("Loading " + Configuration::GetInstance()->GetLocalizedStyleClassName() + " Search", (qrand() % 101));
		CLOVise::PLMProductSearch::GetInstance()->setModal(true);
		CLOVise::PLMProductSearch::GetInstance()->show();
		UTILITY_API->DeleteProgressBar(true);
		Logger::Debug("CopyProduct onBackButtonClicked() Start....");
	}

	/*
	* Description - Add3DModelDetailsWidgetData() method used to Add 3D Model Details Widget Data.
	* Parameter -
	* Exception -
	* Return -
	*/
	void CopyProduct::addCopyProductDetailsWidgetData()
	{
		Logger::Debug("Copy product addCopyProductDetailsWidgetData() start....");
		try
		{
			json fieldsJsonArray = json::array();
			json mergedJsonArray_1 = json::array();
			json mergedJsonArray_2 = json::array();
			fieldsJsonArray = PublishToPLMData::GetInstance()->GetDocumentFieldsJSON();
			//UTILITY_API->DisplayMessageBox("fieldsJsonArray:: " + to_string(fieldsJsonArray));
			Logger::Debug("Copy product addCopyProductDetailsWidgetData() 1....");
			//QStringList emptylist;
			//json feildsJson = json::object();
			json copyStyleJSON = json::array();
			json copyStyleHierarchyJSON = json::array();
			//string attRestApiExposed = "";
			string copyStylePath = DirectoryUtil::GetPLMPluginDirectory() + "Centric_Copy_Style.json";//Reading Columns from json
			copyStyleJSON = Helper::ReadJSONFile(copyStylePath);
			//UTILITY_API->DisplayMessageBox("copyStyleJSON:: " + to_string(copyStyleJSON));
			string copyStyleHierarchyPath = DirectoryUtil::GetPLMPluginDirectory() + "CentricCopyStyleHierarchy.json";//Reading Columns from json
			copyStyleHierarchyJSON = Helper::ReadJSONFile(copyStyleHierarchyPath);
			//UTILITY_API->DisplayMessageBox("copyStyleHierarchyJSON:: " + to_string(copyStyleHierarchyJSON));

			int sizeOfFieldsJsonArray = fieldsJsonArray.size();
			int sizeOfCopyStyleJSON = copyStyleJSON.size();
			int sizeOfCopyStyleHierarchyJSON = copyStyleHierarchyJSON.size();

			for (int i = 0; i < sizeOfCopyStyleJSON + sizeOfCopyStyleHierarchyJSON; i++)
			{
				if (i < sizeOfCopyStyleHierarchyJSON)
					mergedJsonArray_1[i] = copyStyleHierarchyJSON[i];
				else
					mergedJsonArray_1[i] = copyStyleJSON[i - sizeOfCopyStyleHierarchyJSON];
			}
			//UTILITY_API->DisplayMessageBox("mergedJsonArray_1:: " + to_string(mergedJsonArray_1));

			int sizeOfTwoJSONs = mergedJsonArray_1.size();

			for (int i = 0; i < sizeOfFieldsJsonArray + sizeOfTwoJSONs; i++)
			{
				if (i < sizeOfTwoJSONs)
					mergedJsonArray_2[i] = mergedJsonArray_1[i];
				else
					mergedJsonArray_2[i] = fieldsJsonArray[i - sizeOfTwoJSONs];
			}
			//UTILITY_API->DisplayMessageBox("mergedJsonArray_2:: " + to_string(mergedJsonArray_2));

			drawCriteriaWidget(mergedJsonArray_2, m_CopyProductTreeWidget_1, m_CopyProductTreeWidget_2, PublishToPLMData::GetInstance()->GetActive3DModelMetaData());
			//QTreeWidget *treeWidget = new QTreeWidget();
			//drawCriteriaWidget(copyStyleHierarchyJSON, m_HeirarchyTreeWidget, treeWidget, PublishToPLMData::GetInstance()->GetActive3DModelMetaData());

			m_CopyProductTreeWidget_1->setColumnCount(2);
			m_CopyProductTreeWidget_1->setHeaderHidden(true);
			m_CopyProductTreeWidget_1->setWordWrap(true);
			m_CopyProductTreeWidget_1->setDropIndicatorShown(false);
			m_CopyProductTreeWidget_1->setRootIsDecorated(false);
			m_CopyProductTreeWidget_1->setSelectionMode(QAbstractItemView::NoSelection);
			m_CopyProductTreeWidget_1->setStyleSheet("QTreeWidget { background-color: #262628; border: 1px solid #000; padding-left: 20px; min-width: 400px; outline: 0;}""QTreeWidget::item {height: 20px; width: 200px; margin-right: 20px; margin-top: 5px; margin-bottom: 5px; border: none; }""QTreeWidget::item:hover{ background-color: #262628; }""QTreeView{outline: 0;}");
			m_CopyProductTreeWidget_2->setColumnCount(2);
			m_CopyProductTreeWidget_2->setHeaderHidden(true);
			m_CopyProductTreeWidget_2->setWordWrap(true);
			m_CopyProductTreeWidget_2->setDropIndicatorShown(false);
			m_CopyProductTreeWidget_2->setRootIsDecorated(false);
			m_CopyProductTreeWidget_2->setSelectionMode(QAbstractItemView::NoSelection);
			m_CopyProductTreeWidget_2->setStyleSheet("QTreeWidget { background-color: #262628; border: 1px solid #000; padding-left: 20px; min-width: 400px; outline: 0;}""QTreeWidget::item {height: 20px; width: 200px; margin-right: 20px; margin-top: 5px; margin-bottom: 5px; border: none; }""QTreeWidget::item:hover{ background-color: #262628; }""QTreeView{outline: 0;}");

		}
		catch (string msg)
		{
			Logger::Error("Copy product addCopyProductDetailsWidgetData()  Exception - " + msg);
		}
		catch (exception & e)
		{
			Logger::Error("Copy product addCopyProductDetailsWidgetData()  Exception - " + string(e.what()));
		}
		catch (const char* msg)
		{
			wstring wstr(msg, msg + strlen(msg));
			Logger::Error("Copy product addCopyProductDetailsWidgetData()  Exception - " + string(msg));
		}
		/*for (int feildsCount = 0; feildsCount < jsonvalue.size(); feildsCount++)
		{
			feildsJson = Helper::GetJSONParsedValue<int>(jsonvalue, feildsCount, false);
			attRestApiExposed = Helper::GetJSONValue<string>(feildsJson, ATTRIBUTE_REST_API_EXPOSED, true);
			if (attRestApiExposed == "false")
				continue;
			drawWidget(feildsJson, m_CopyProductTreeWidget);
		}
		for (int feildsCount = 0; feildsCount < fieldsJsonArray.size(); feildsCount++)
		{
			feildsJson = Helper::GetJSONParsedValue<int>(fieldsJsonArray, feildsCount, false);
			attRestApiExposed = Helper::GetJSONValue<string>(feildsJson, ATTRIBUTE_REST_API_EXPOSED, true);
			if (attRestApiExposed == "false")
				continue;
			drawWidget(feildsJson, m_CopyProductTreeWidget);
		}*/
		//for (int feildsCount = 0; feildsCount < jsonvalue.size(); feildsCount++)
		//{
		//	feildsJson = Helper::GetJSONParsedValue<int>(jsonvalue, feildsCount, false);
		//	attRestApiExposed = Helper::GetJSONValue<string>(feildsJson, ATTRIBUTE_REST_API_EXPOSED, true);
		//	if (attRestApiExposed == "false")
		//		continue;
		//	drawWidget(feildsJson, m_HeirarchyTreeWidget);
		//}
		Logger::Debug("Copy product addCopyProductDetailsWidgetData() end....");
	}

	/*
	* Description - DrawCriteriaWidget() method used to create fields in UI.
	* Parameter -  json, QTreeWidget, json.
	* Exception -
	* Return -
	*/
	void CopyProduct::drawCriteriaWidget(json _attributesJsonArray, QTreeWidget* _documentPushToPLMTree_1, QTreeWidget* _documentPushToPLMTree_2, json _metadataJson)
	{
		Logger::Debug("Copy product drawCriteriaWidget() start....");

		if (_attributesJsonArray.size() < 10)
		{
			drawWidget(_attributesJsonArray, _documentPushToPLMTree_1, STARTING_INDEX, _attributesJsonArray.size());
			_documentPushToPLMTree_2->hide();
		}
		else
		{
			drawWidget(_attributesJsonArray, _documentPushToPLMTree_1, STARTING_INDEX, _attributesJsonArray.size() / DIVISION_FACTOR);
			drawWidget(_attributesJsonArray, _documentPushToPLMTree_2, _attributesJsonArray.size() / DIVISION_FACTOR, _attributesJsonArray.size());

		}

		Logger::Debug("Create product drawCriteriaWidget() end....");
		Logger::Debug("Copy product drawCriteriaWidget() end....");
	}

	/*
	* Description - DrawCriteriaWidget() method used to create input fields.
	* Parameter -  json, QTreeWidget, string, string, bool.
	* Exception -
	* Return -
	*/
	void CopyProduct::drawWidget(json _feildsJson, QTreeWidget* m_TreeWidget, int _start, int _end)
	{
		Logger::Debug("Copy product drawCriteriaWidget() start....");
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
		string attRestApiExposed = "";
		QString attributeInitiaValue = "";
		string attCreateOnly = "";
		//bool isDisable = false;		

		for (int i = _start; i < _end; i++)
		{

			bool isDisable = false;
			bool required = true;
			responseJson.clear();
			attJson = Helper::GetJSONParsedValue<int>(_feildsJson, i, false);
			attRestApiExposed = Helper::GetJSONValue<string>(attJson, ATTRIBUTE_REST_API_EXPOSED, true);
			if (attRestApiExposed == "false")
				continue;

			attributeType = Helper::GetJSONValue<string>(attJson, ATTRIBUTE_TYPE, true);
			internalName = Helper::GetJSONValue<string>(attJson, ATTRIBUTE_INTERNAL_NAME, true);
			Logger::Debug("Copy product drawCriteriaWidget() internalName....");
			attributeName = Helper::GetJSONValue<string>(attJson, ATTRIBUTE_NAME, true);
			Logger::Debug("Copy product drawCriteriaWidget() 2...." + attributeName);
			attributeDisplayName = Helper::GetJSONValue<string>(attJson, ATTRIBUTE_DISPLAY_NAME, true);
			Logger::Debug("Copy product drawCriteriaWidget() 3....");
			attributeRequired = Helper::GetJSONValue<string>(attJson, ATTRIBUTE_REQUIRED, true);
			Logger::Debug("Copy product drawCriteriaWidget() 4....");
			restrictEdit = Helper::GetJSONValue<string>(attJson, ATTRIBUTE_EDITABLE, true);
			attributeInitiaValue = QString::fromStdString(Helper::GetJSONValue<string>(attJson, ATTRIBUTE_INITIAL_VALUE, true));
			Logger::Debug("Copy product drawCriteriaWidget() attributeEditable...." + restrictEdit);
			Logger::Debug("Copy product drawCriteriaWidget() internalName...." + internalName);
			Logger::Debug("Copy product drawCriteriaWidget() attributeDisplayName...." + attributeDisplayName);
			attCreateOnly = Helper::GetJSONValue<string>(attJson, ATTRIBUTE_CREATE_ONLY, true);
			if (!m_localAttributesList.contains(QString::fromStdString(internalName)))
			{
				m_localAttributesList.append(QString::fromStdString(internalName));
			}
			else
				continue;


			if (!FormatHelper::HasContent(attributeInitiaValue.toStdString()) || attributeInitiaValue.contains("ref(centric:)"))
			{
				attributeInitiaValue = "";
			}
			else
			{
				int found = attributeInitiaValue.toStdString().find_first_of(":");
				if (found != -1)
				{

					attributeInitiaValue = QString::fromStdString(attributeInitiaValue.toStdString().substr(found + 1));
					//UTILITY_API->DisplayMessageBox(attributeInitiaValue.toStdString());
						
				}
			}

			if (attributeRequired == "false")
				required = false;

			if (restrictEdit == "false" && attCreateOnly == "false")
			{
				isDisable = true;
				Logger::Debug("Copy product drawCriteriaWidget() isEditable...." + to_string(isDisable));
			}

			if (attributeType == INTEGER_ATT_TYPE_KEY || attributeType == CONSTANT_ATT_TYPE_KEY)
			{
				QTreeWidgetItem* topLevel = new QTreeWidgetItem();  // Creating new TreeWidgetItem
				m_TreeWidget->addTopLevelItem(topLevel);
				m_TreeWidget->setItemWidget(topLevel, 0, CVWidgetGenerator::CreateLabelWidget(attributeDisplayName, internalName, "", required, !isDisable));
				m_TreeWidget->setItemWidget(topLevel, 1, CVWidgetGenerator::CreateSpinBoxWidget(attributeInitiaValue, isDisable, ""));
			}
			else if (attributeType == DATE_ATT_TYPE_KEY || attributeType == TIME_ATT_TYPE_KEY)
			{
				QStringList dateList;
				QTreeWidgetItem* topLevel = new QTreeWidgetItem();  // Creating new TreeWidgetItem
				{
					m_TreeWidget->addTopLevelItem(topLevel);			// Adding ToplevelItem
					m_TreeWidget->setItemWidget(topLevel, 0, CVWidgetGenerator::CreateLabelWidget(attributeDisplayName, internalName, "", required, !isDisable));	// Adding label at column 1
					m_TreeWidget->setItemWidget(topLevel, 1, CVWidgetGenerator::CreateDateWidget(attributeInitiaValue.toStdString(), isDisable));
				}
			}
			else if (attributeType == BOOLEAN_ATT_TYPE_KEY)
			{
				QTreeWidgetItem* topLevel = new QTreeWidgetItem();  // Creating new TreeWidgetItem
				m_TreeWidget->addTopLevelItem(topLevel);			// Adding ToplevelItem
				m_TreeWidget->setItemWidget(topLevel, 0, CVWidgetGenerator::CreateLabelWidget(attributeDisplayName, internalName, "", required, !isDisable));	// Adding label at column 1
				m_TreeWidget->setItemWidget(topLevel, 1, CVWidgetGenerator::CreateBooleanWidget(attributeInitiaValue, isDisable, topLevel, 1));	// Adding label at column 2

			}
			else if (attributeType == REF_ATT_TYPE_KEY || attributeType == ENUM_ATT_TYPE_KEY)
			{
				
				Logger::Debug("Copy product drawWidget() ref  6....");
				QString selectedValue = "";
				QTreeWidgetItem* topLevel = new QTreeWidgetItem();
				ComboBoxItem* comboBox = new ComboBoxItem();
				comboBox->setFocusPolicy(Qt::StrongFocus);

#ifdef __APPLE__	// Settinng Windows OS style to QComboBox on MAC OS

				comboBox->setStyle(QStyleFactory::create("Windows"));
				comboBox->setItemDelegate(new QStyledItemDelegate());

#endif

				QStringList valueList;
				valueList.append(QString::fromStdString(BLANK));
	
				if (attributeType == ENUM_ATT_TYPE_KEY)
				{
					string format = Helper::GetJSONValue<string>(attJson, "format", true);
				    UIHelper::GetEnumValues(format, valueList, comboBox);
				}
				else
				{
					if (attributeName == "Season")
					{
						
						responseJson = Helper::makeRestcallGet(RESTAPI::SEASON_SEARCH_API, "?limit=" + Configuration::GetInstance()->GetMaximumLimitForRefAttValue() + "&sort=node_name", "", "Loading season details..");
						Logger::RestAPIDebug("seasonJson::" + to_string(responseJson));
						
					}
					else if (attributeName == "Style Type")
					{
						responseJson = Helper::makeRestcallGet(RESTAPI::STYLE_TYPE_API, "?available=true&limit=100", "", "Loading style type details..");
						Logger::RestAPIDebug("styleTypeJson::" + to_string(responseJson));
					}
					for (int i = 0; i < responseJson.size(); i++)
					{
						attJson = Helper::GetJSONParsedValue<int>(responseJson, i, false);;///use new method
						attName = Helper::GetJSONValue<string>(attJson, ATTRIBUTE_NAME, true);
						attId = Helper::GetJSONValue<string>(attJson, ATTRIBUTE_ID, true);
						valueList.append(QString::fromStdString(attName));
						if (attributeName == "Season")
						{
							m_seasonNameIdMap.insert(make_pair(attName, attId));
						}
						comboBox->setProperty(attName.c_str(), QString::fromStdString(attId));
						comboBox->setProperty(attId.c_str(), QString::fromStdString(attName));
					}
				}

				comboBox->setProperty("attName", QString::fromStdString(attributeName));
				comboBox->setStyleSheet(COMBOBOX_STYLE);
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

				if (comboBox->isEnabled())
				{
					comboBox->setEditable(true);
					comboBox->fillItemListAndDefaultValue(valueList, selectedValue);
					QCompleter* m_nameCompleter = new QCompleter(valueList, comboBox); // using QCompleter class
					m_nameCompleter->setCompletionMode(QCompleter::PopupCompletion);
					m_nameCompleter->setCaseSensitivity(Qt::CaseInsensitive);
					comboBox->setCompleter(m_nameCompleter);
				}
				comboBox->setStyleSheet(COMBOBOX_STYLE);
				
				if (!isDisable)
				{
					comboBox->setDisabled(true);
					comboBox->setStyleSheet(DISABLED_COMBOBOX_STYLE); 
				}

				m_TreeWidget->addTopLevelItem(topLevel);
				m_TreeWidget->setItemWidget(topLevel, 0, CVWidgetGenerator::CreateLabelWidget(attributeDisplayName, internalName, "", required, !isDisable));
				m_TreeWidget->setItemWidget(topLevel, 1, comboBox);
				if (attributeName != "Style/Shape" &&  attributeName != "Style/Theme")
				{
					valueList.sort();
					comboBox->addItems(valueList);
				}
				if (attributeName == "Copy Template")
				{
					comboBox->setCurrentText("FR Replica");
					selectedValue = "FR Replica";
				}
				comboBox->setProperty("LabelName", QString::fromStdString(attributeName));
				if (comboBox->isEnabled())
				{
					comboBox->setEditable(true);
					comboBox->fillItemListAndDefaultValue(valueList, selectedValue);
					QCompleter* m_nameCompleter = new QCompleter(valueList, comboBox); // using QCompleter class
					m_nameCompleter->setCompletionMode(QCompleter::PopupCompletion);
					m_nameCompleter->setCaseSensitivity(Qt::CaseInsensitive);
					comboBox->setCompleter(m_nameCompleter);
				}
				QObject::connect(comboBox, SIGNAL(currentIndexChanged(const QString&)), this,
					SLOT(OnHandleDropDownValue(const QString&)));
				Logger::Debug("Copy product drawWidget() ref  End....");

			}
			else if (attributeType == STRING_ATT_TYPE_KEY || attributeType == TEXT_ATT_TYPE_KEY)
			{
				Logger::Debug("Copy product drawWidget() string  Start....");
				QTreeWidgetItem* topLevel = new QTreeWidgetItem();  // Creating new TreeWidgetItem
				QLineEdit* LineEdit = CVWidgetGenerator::CreateLineEdit("", "", true);
				LineEdit->setStyleSheet(LINEEDIT_STYLE);
				if (!isDisable)
				{
					LineEdit->setEnabled(false);
					LineEdit->setStyleSheet(DISABLED_LINEEDIT_STYLE);
				}
				LineEdit->setAttribute(Qt::WA_MacShowFocusRect, false);

				LineEdit->setText("");

				m_TreeWidget->addTopLevelItem(topLevel);						// Adding ToplevelItem
				m_TreeWidget->setItemWidget(topLevel, 0, CVWidgetGenerator::CreateLabelWidget(attributeDisplayName, internalName, "", required, !isDisable));	// Adding label at column 1
				m_TreeWidget->setItemWidget(topLevel, 1, LineEdit);
				Logger::Debug("Copy product drawWidget() string  End....");
			}
		}

		Logger::Debug("Copy product drawWidget() end....");
	}

	/*
	* Description - PublishToPLMClicked() method is a slot for publish 3D model to plm click.
	* Parameter -
	* Exception - exception, Char *
	* Return -
	*/
	void CopyProduct::onPublishToPLMClicked()
	{
		Logger::Debug("Copy product onPublishToPLMClicked() start....");
		QDir dir;
		map<string, string>::iterator it;
		try
		{
			this->hide();

			collectCopyProductFieldsData();
			UIHelper::ValidateRquired3DModelData(m_CopyProductTreeWidget_1);
			UIHelper::ValidateRquired3DModelData(m_CopyProductTreeWidget_2);
			UTILITY_API->DeleteProgressBar(true);
			UTILITY_API->CreateProgressBar();		
			UTILITY_API->SetProgress("Copying "+ Configuration::GetInstance()->GetLocalizedStyleClassName() +"..", (qrand() % 101));
			vector<pair<string, string>> headerNameAndValueList;
			headerNameAndValueList.push_back(make_pair("content-Type", "application/json"));
			headerNameAndValueList.push_back(make_pair("Cookie", Configuration::GetInstance()->GetBearerToken()));
			Logger::RestAPIDebug("Copy product onPublishToPLMClicked() m_copydataRequest:: "+ m_copydataRequest);

			string response = RESTAPI::PostRestCall(m_copydataRequest, Configuration::GetInstance()->GetPLMServerURL() + RESTAPI::CREATE_STYLE_COPY_API, "content-type: application/json; boundary=----WebKitFormBoundary7MA4YWxkTrZu0gW");
			Logger::RestAPIDebug("Copy product onPublishToPLMClicked() response:: " + response);
			RESTAPI::SetProgressBarData(30, "Copying "+ Configuration::GetInstance()->GetLocalizedStyleClassName() +"..", true);
			if (FormatHelper::HasError(response))
			{
				Helper::GetCentricErrorMessage(response);
				throw runtime_error(response);
			}
			else
			{
				Logger::Debug("PublishToPLMData -> onPublishToPLMClicked 1");
				json detailJson = Helper::GetJsonFromResponse(response, "{");
				Logger::Debug("PublishToPLMData -> onPublishToPLMClicked 2");
				string productId = Helper::GetJSONValue<string>(detailJson, ATTRIBUTE_ID, true);
				Logger::Debug("PublishToPLMData -> onPublishToPLMClicked productId ----------- "+ productId);
				Logger::RestAPIDebug("Copy product onPublishToPLMClicked() m_ProductMetaData:: " + m_ProductMetaData);
				if (m_ProductMetaData != "") 
				{
					response = RESTAPI::PutRestCall(m_ProductMetaData, Configuration::GetInstance()->GetPLMServerURL() + RESTAPI::UPDATE_STYLE_API + "/" + productId, "content-type: application/json; boundary=----WebKitFormBoundary7MA4YWxkTrZu0gW");
					Logger::RestAPIDebug("Copy product onPublishToPLMClicked() m_ProductMetaData response 1:: " + response);
					
						//UTILITY_API->DisplayMessageBox(response);
					if (FormatHelper::HasError(response)) {
						Helper::GetCentricErrorMessage(response);
						throw runtime_error(response);
					}
					UTILITY_API->SetProgress("Copying " + Configuration::GetInstance()->GetLocalizedStyleClassName() + "..", (qrand() % 101));
					detailJson = Helper::GetJsonFromResponse(response, "{");
					productId = Helper::GetJSONValue<string>(detailJson, ATTRIBUTE_ID, true);
				}
		
				UTILITY_API->NewProject();
				//Clearing cached data post successful publish to plm
				PublishToPLMData::GetInstance()->SetActiveProductMetaData(json::object());
				PublishToPLMData::GetInstance()->SetActive3DModelMetaData(json::object());
				PublishToPLMData::GetInstance()->SetIsProductOverridden(false);
				PublishToPLMData::GetInstance()->SetActiveProductId(BLANK);
				PublishToPLMData::GetInstance()->SetActiveProductObjectId(BLANK);
				PublishToPLMData::GetInstance()->SetActiveProductName(BLANK);
				PublishToPLMData::GetInstance()->SetActiveProductStatus(BLANK);
				PublishToPLMData::GetInstance()->Set3DModelObjectId(BLANK);
				CVDisplayMessageBox DownloadDialogObject;
				DownloadDialogObject.DisplyMessage(Configuration::GetInstance()->GetLocalizedStyleClassName() + " copied successfully");
				DownloadDialogObject.setModal(true);
				DownloadDialogObject.exec();
				ClearAllFields(m_CopyProductTreeWidget_1);
				ClearAllFields(m_CopyProductTreeWidget_2);
				ClearAllFields(m_HeirarchyTreeWidget);
				UTILITY_API->DeleteProgressBar(true);
				ui_tabWidget->hide();
				ui_overviewTab->hide();
				this->hide();
				UTILITY_API->CreateProgressBar();

				UTILITY_API->SetProgress("Loading Result..", (qrand() % 101));
				m_copyCreated = true;
				ProductConfig::GetInstance()->m_isShow3DAttWidget = true;
				ProductConfig::GetInstance()->SetCopyStyleResult(response);
				PLMProductResults::GetInstance()->m_downloadButton->setText("Download");
				PLMProductResults::GetInstance()->DrawResultWidget(false);
				UTILITY_API->DeleteProgressBar(true);
				PLMProductResults::GetInstance()->setModal(true);
				PLMProductResults::GetInstance()->exec();

				this->close();
			}

		}
		catch (exception& e)
		{
			UTILITY_API->DeleteProgressBar(true);
			Logger::Error("Copy product-> Copy product Exception - " + string(e.what()));
			UTILITY_API->DisplayMessageBox(e.what());

			this->show();
		}
		catch (const char* msg)
		{
			Logger::Error("Copy product-> Copy product Exception - " + string(msg));
			wstring wstr(msg, msg + strlen(msg));
			UTILITY_API->DeleteProgressBar(true);
			UTILITY_API->DisplayMessageBoxW(wstr);

			this->show();
		}
		catch (string str)
		{
			UTILITY_API->DeleteProgressBar(true);
			Logger::Error("Copy product-> Copy product Exception - " + str);
			UTILITY_API->DisplayMessageBox(str);
			this->show();
		}

		Logger::Debug("Copy product onPublishToPLMClicked() end....");
	}

	/*
	* Description - CollectPublishToPLMFieldsData() method used to collect publish to plm fields data.
	* Parameter -
	* Exception -
	* Return -
	*/
	void CopyProduct::collectPublishToPLMFieldsData()
	{
		Logger::Debug("Copy product collectPublishToPLMFieldsData() start....");
		//Collects all Product Section related data into map
		m_parameterJson[DOCUMENT_DETAIL_KEY] = m_documentDetailsJson;
		Logger::Debug("Copy product collectPublishToPLMFieldsData() end....");
	}

	/*
	* Description - CollectProductFieldsData() method used to collect product fields data.
	* Parameter -
	* Exception -
	* Return -
	*/
	void CopyProduct::collectProductFieldsData()
	{
		Logger::Debug("Copy product collectProductFieldsData() start....");
		m_parameterJson[PRODUCT_OBJ_ID] = PublishToPLMData::GetInstance()->GetActiveProductObjectId();
		m_parameterJson[PRODUCTID_KEY] = PublishToPLMData::GetInstance()->GetActiveProductId();
		m_parameterJson[PRODUCT_NAME_KEY] = PublishToPLMData::GetInstance()->GetActiveProductName();
		m_parameterJson[PRODUCT_STATUS_KEY] = PublishToPLMData::GetInstance()->GetActiveProductStatus();

		m_productDetailsJson[PRODUCTID_KEY] = PublishToPLMData::GetInstance()->GetActiveProductId();
		m_productDetailsJson[PRODUCT_NAME_KEY] = PublishToPLMData::GetInstance()->GetActiveProductName();
		m_productDetailsJson[PRODUCT_STATUS_KEY] = PublishToPLMData::GetInstance()->GetActiveProductStatus();

		m_productDetailsJson[ATTRIBUTES_KEY] = json::array();

		json thumbNailDetails = json::object();
		thumbNailDetails[THUMBNAIL_NAME_KEY] = m_3DModelThumbnailName;

		m_productDetailsJson[THUMBNAIL_DETAIL_KEY] = thumbNailDetails;
		Logger::Debug("Copy product collectProductFieldsData() end....");
	}

	/*
	* Description - Collect3DModelFieldsData() method used to collect 3d model fields data.
	* Parameter -
	* Exception -
	* Return -
	*/
	void CopyProduct::collectCopyProductFieldsData()
	{
		Logger::Debug("Copy product collectCopyProductFieldsData() start....");
		m_ProductMetaData = collectCriteriaFields(m_CopyProductTreeWidget_1, m_CopyProductTreeWidget_2);
		Logger::Debug("Copy product collectCopyProductFieldsData() end....");
	}

	/*
	* Description - PreparePublishRequestParameter() method used to prepare request parameter.
	* Parameter -
	* Exception -
	* Return -
	*/
	string CopyProduct::getPublishRequestParameter(string _path, string _imageName)
	{
		Logger::Debug("Copy product getPublishRequestParameter() start....");

		string contentType = Helper::GetFileContetType(_path);
		string fileStream = Helper::GetFilestream(_path);
		string contentLength = Helper::getFileLength(_path);
		string postField = "------WebKitFormBoundary7MA4YWxkTrZu0gW\r\nContent-Disposition: form-data; name=\"file\"; filename=" + _imageName + "\r\nContent-Type: " + contentType + "\r\n" + contentLength + "\r\n\r\n" + fileStream + "\r\n------WebKitFormBoundary7MA4YWxkTrZu0gW--";
		return postField;
	}

	string CopyProduct::uploadDocument(string _productId)
	{
		vector<pair<string, string>> headerNameAndValueList;
		string latestRevisionId;
		try
		{
			headerNameAndValueList.push_back(make_pair("content-Type", "application/json"));
			headerNameAndValueList.push_back(make_pair("Cookie", Configuration::GetInstance()->GetBearerToken()));

			json EmptyJson = json::object();
			string emptyString = to_string(EmptyJson);
			string resultJsonString = REST_API->CallRESTPost(Configuration::GetInstance()->GetPLMServerURL() + RESTAPI::DOCUMENT_CREATE_API + "/" + _productId, &emptyString, headerNameAndValueList, "Loading");
			if (!FormatHelper::HasContent(resultJsonString))
			{
				throw "Unable to initiliaze Document Configuration. Please try again or Contact your System Administrator.";
			}
			json detailJson = Helper::GetJsonFromResponse(resultJsonString, "{");
			string latestRevisionId = Helper::GetJSONValue<string>(detailJson, LATEST_REVISION_KEY, true);
		}
		catch (string msg)
		{
			Logger::Error("CopyProduct::uploadDocument() Exception - " + msg);
		}
		catch (exception & e)
		{
			Logger::Error("CopyProduct::uploadDocument() Exception - " + string(e.what()));
		}
		catch (const char* msg)
		{
			wstring wstr(msg, msg + strlen(msg));
			Logger::Error("CopyProduct::uploadDocument() Exception - " + string(msg));
		}
		return latestRevisionId;
	}

	/*
	* Description - exportZPRJ() method used to export a 3D model and visual images.
	* Parameter -
	* Exception - using throw if anything gets fail.
	* Return -
	*/
	void CopyProduct::exportZPRJ(string _revisionId)
	{
		Logger::Debug("Copy product exportZPRJ() start....");
		QDir dir;
		string _3DModelFilePath = UTILITY_API->GetProjectFilePath();

		m_3DModelFileName = UTILITY_API->GetProjectName() + ".zprj";
		EXPORT_API->ExportZPrj(_3DModelFilePath, true);
		string postField = getPublishRequestParameter(_3DModelFilePath, m_3DModelFileName);
		string resultJsonString = RESTAPI::PutRestCall(postField, Configuration::GetInstance()->GetPLMServerURL() + RESTAPI::DOCUMENT_UPLOAD_API + "/" + _revisionId + "/upload", "content-type: multipart/form-data; boundary=----WebKitFormBoundary7MA4YWxkTrZu0gW");
		UTILITY_API->DisplayMessageBox("Upload zprj completed" + resultJsonString);
		Logger::Debug("Copy product exportZPRJ() end....");
	}

	/*
	* Description - exportGLB() method used to export a 3D model and visual images.
	* Parameter - bool
	* Exception - using throw if anything gets fail.
	* Return -
	*/
	void CopyProduct::exportGLB(bool _isGLBExportable)
	{
		if (_isGLBExportable)
		{
			Marvelous::ImportExportOption option;

			string _3DModelFilePath = UTILITY_API->GetProjectFilePath();
			Helper::EraseSubString(_3DModelFilePath, UTILITY_API->GetProjectName());

			string _3DModelFileName = "";
			string _3DModelFilePathWithExtension = "";

			_3DModelFileName = UTILITY_API->GetProjectName() + GLB;

			_3DModelFilePathWithExtension = _3DModelFilePath + _3DModelFileName;

			long int uploadjFilesize = Helper::GetFileSize(_3DModelFilePathWithExtension);
			int uploadFilesizeInMB = Helper::ConvertBytesToMB(uploadjFilesize);
			if (uploadFilesizeInMB > PublishToPLMData::GetInstance()->GetMaxUploadFileSize())
				throw "Unable to publish to PLM. Maximum file upload size exceeded.";

			EXPORT_API->ExportGLTF(_3DModelFilePathWithExtension, option, true);

			string contentType = Helper::GetFileContetType(_3DModelFilePathWithExtension);
			string fileStream = Helper::GetFilestream(_3DModelFilePathWithExtension);
			string contentLength = Helper::getFileLength(_3DModelFilePathWithExtension);
			m_multipartFilesParams = Helper::AddMultiPartFilesToRequest(m_multipartFilesParams, m_3DModelFileName, contentType, fileStream, contentLength);

		}
	}

	/*
	* Description - CollectCriteriaFields() method used to collect data in Publish to plm UI.
	* Parameter -  QTreeWidget.
	* Exception -
	* Return - json.
	*/
	string CopyProduct::collectCriteriaFields(QTreeWidget* _documentPushToPLMTree_1, QTreeWidget* _documentPushToPLMTree_2)
	{
		Logger::Debug("Copy product collectCriteriaFields() start....");
		json attsJson = json::array();
		json attJson = json::object();
		string fieldAndValues = "{";
		m_copydataRequest = "{";
		for (int i = 0; i < _documentPushToPLMTree_1->topLevelItemCount(); i++)
		{
			ReadVisualUIFieldValue(_documentPushToPLMTree_1, i, fieldAndValues);
		}
		for (int i = 0; i < _documentPushToPLMTree_2->topLevelItemCount(); i++)
		{
			ReadVisualUIFieldValue(_documentPushToPLMTree_2, i, fieldAndValues);
		}
		m_copydataRequest = m_copydataRequest.substr(0, m_copydataRequest.length() - 1);
		m_copydataRequest += "\n}"; 

		//UTILITY_API->DisplayMessageBox("m_copydataRequest" + m_copydataRequest);
		if (fieldAndValues == "{") {
			fieldAndValues = "";
			return fieldAndValues;
		}
		fieldAndValues = fieldAndValues.substr(0, fieldAndValues.length() - 1);
		fieldAndValues += "\n}";
		return fieldAndValues;
	}

	void CopyProduct::onAddColorwaysClicked()
	{
		Logger::Debug("CopyProduct -> onAddColorwaysClicked() -> Start");
		this->hide();
		//PLMColorSearch::Destroy();
		//ColorConfig::GetInstance()->InitializeColorData();
		//ColorConfig::GetInstance()->GetColorConfigJSON();
		PLMColorSearch::GetInstance()->setModal(true);
		PLMColorSearch::GetInstance()->DrawSearchWidget(false);
		PLMColorSearch::GetInstance()->exec();
		Logger::Debug("CopyProduct -> onAddColorwaysClicked() -> End");
	}



	void CopyProduct::AddColorwayDetails(json _downloadJson)
	{

		Logger::Debug("CopyProduct -> AddColorwayDetails() -> Start");
		try
		{
			ui_ColorwayTable->show();
			QStringList headerlist;
			json attachmentsJson = json::object();
			string objectName = "";
			string pantone = "";
			string objectCode = "";
			string attachmentName = "";
			string rgbValue = "";
			string attId = "";
			json jsonvalue = json::object();
			json columnNamejson = json::array();
			int colorResultColumnCount = 0;
			int count = 0;
			bool Isduplicate = false;
			int appendedRowCount = 0;

			string columnsNames = DirectoryUtil::GetPLMPluginDirectory() + "ColorwayTableColumns.json";//Reading Columns from json
			jsonvalue = Helper::ReadJSONFile(columnsNames);
			string colorwayTableColumns = Helper::GetJSONValue<string>(jsonvalue, "colorwayTableFieldList", false);
			columnNamejson = json::parse(colorwayTableColumns);
			json downloadJsonArray = _downloadJson;
			m_colorwayRowcount = ui_ColorwayTable->rowCount();
			string columndatafield = "";
			for (int index = 0; index < columnNamejson.size(); index++)
			{
				string columnname = Helper::GetJSONParsedValue<int>(columnNamejson, index, false);
				headerlist.insert(index, QString::fromStdString(columnname));
			}
			ui_ColorwayTable->setColumnCount(headerlist.size());
			ui_ColorwayTable->setHorizontalHeaderLabels(headerlist);

			if (!m_addClicked)
				ui_ColorwayTable->setRowCount(downloadJsonArray.size());

			QStringList colorwayNamesList;
			int colorwayCount = UTILITY_API->GetColorwayCount();
			colorwayNamesList.append(QString::fromStdString(BLANK));
			for (int count = 0; count < colorwayCount; count++)
			{
				string colorwayName = UTILITY_API->GetColorwayName(count);
				colorwayNamesList.append(QString::fromStdString(colorwayName));
			}
			for (int rowCount = 0; rowCount < downloadJsonArray.size(); rowCount++)
			{
				attachmentsJson = Helper::GetJSONParsedValue<int>(downloadJsonArray, rowCount, false);
				objectName = Helper::GetJSONValue<string>(attachmentsJson, ATTRIBUTE_NAME, true);
				pantone = Helper::GetJSONValue<string>(attachmentsJson, PANTONE_KEY, true);
				rgbValue = Helper::GetJSONValue<string>(attachmentsJson, RGB_VALUE_KEY, true);
				objectCode = Helper::GetJSONValue<string>(attachmentsJson, CODE_KEY, true);
				attId = Helper::GetJSONValue<string>(attachmentsJson, ATTRIBUTE_ID, true);
				if (rgbValue.empty())
					continue;

				QString UniqueObjectId;
				QTableWidgetItem* item;

				if (m_addClicked)
				{
					for (int i = 0; i < ui_ColorwayTable->rowCount(); i++)
					{
						QWidget* qWidget = ui_ColorwayTable->cellWidget(i, 0);
						if (!qWidget)
						{
							continue;
						}
						QCheckBox *temp = qobject_cast <QCheckBox*> (qWidget->layout()->itemAt(0)->widget());
						if (temp != nullptr)
						{
							UniqueObjectId = temp->property("objectId").toString();
							Logger::Debug("CopyProduct -> AddColorwayDetails() -> UniqueObjectId" + UniqueObjectId.toStdString());
							Logger::Debug("CopyProduct -> AddColorwayDetails() -> objectId" + attId);
							if (QString::compare(QString::fromStdString(attId), UniqueObjectId, Qt::CaseInsensitive) == 0)
							{
								Isduplicate = true;
								break;
							}
						}
					}
				}
				if (Isduplicate)
				{
					Isduplicate = false;
					Logger::Debug("CopyProduct -> AddColorwayDetails() -> Continue");
					continue;
				}
				else
					ui_ColorwayTable->setRowCount(m_colorwayRowcount + count + 1);

				Logger::Debug("CopyProduct -> AddColorwayDetails() -> m_colorwayRowcount" + to_string(m_colorwayRowcount));
				Logger::Debug("CopyProduct -> AddColorwayDetails() -> count" + to_string(count));
				Logger::Debug("CopyProduct -> AddColorwayDetails() -> row number" + to_string(m_colorwayRowcount + count));
				AddRows(count, attId, objectName, rgbValue, objectCode, pantone, colorwayNamesList);
				count++;
			}

			m_addClicked = true;

			for (int index = 0; index < headerlist.count(); index++)
			{
				if (index == 0)
					ui_ColorwayTable->setColumnWidth(index, 30);
				else if (index == 1)
				{
					ui_ColorwayTable->resizeColumnToContents(1);
				}

				else
					ui_ColorwayTable->setColumnWidth(index, 60);
			}
		}
		catch (string msg)
		{
			Logger::Error("CopyProduct -> AddColorwayDetails() Exception - " + msg);
		}
		catch (exception & e)
		{
			Logger::Error("CopyProduct -> AddColorwayDetails() Exception - " + string(e.what()));
		}
		catch (const char* msg)
		{
			wstring wstr(msg, msg + strlen(msg));
			Logger::Error("CopyProduct -> AddColorwayDetails() Exception - " + string(msg));
		}
		Logger::Debug("CopyProduct -> AddColorwayDetails() -> End");
	}

	void CopyProduct::onContextMenuClicked(const QPoint & _pos)
	{
		Logger::Debug("CopyProduct -> onContextMenu_Clicked() -> Start");
		QMenu menu;
		QAction* removeAction = menu.addAction(QString::fromStdString("Delete"));
		QAction* rightClickAction = menu.exec(ui_ColorwayTable->viewport()->mapToGlobal(_pos));
		if (removeAction == rightClickAction)
		{
			QTableWidgetItem* item = ui_ColorwayTable->itemAt(_pos);
			if (item)
			{
				int rowIndex = ui_ColorwayTable->row(item);
				ui_ColorwayTable->removeRow(rowIndex);
			}
			else
			{
				UTILITY_API->DisplayMessageBox("SelectImageToDelete");
			}
		}
		Logger::Debug("CopyProduct -> onContextMenu_Clicked() -> End");
	}

	bool CopyProduct::ValidateColorwayNameField()
	{
		Logger::Debug("CopyProduct -> ValidateColorwayNameField() -> Start");
		m_modifiedColorwayNames.clear();
		bool iscolorwayNameValidated = true;
		for (int rowIndex = 0; rowIndex < ui_ColorwayTable->rowCount(); rowIndex++)
		{
			QComboBox* colorwayNameCombo = qobject_cast<QComboBox*>(ui_ColorwayTable->cellWidget(rowIndex, 1));
			m_modifiedColorwayNames << colorwayNameCombo->currentText();
		}
		for (int nameIndex = 0; nameIndex < m_modifiedColorwayNames.count(); nameIndex++)
		{
			if (m_modifiedColorwayNames[nameIndex] != "Empty")
			{
				int count = m_modifiedColorwayNames.count(m_modifiedColorwayNames[nameIndex]);
				if (count > 1)
				{
					iscolorwayNameValidated = false;
				}
			}

		}
		if (!iscolorwayNameValidated)
		{
			UTILITY_API->DisplayMessageBox("CanNotRepeatColorwayNames");
		}

		return iscolorwayNameValidated;
		Logger::Debug("CopyProduct -> ValidateColorwayNameField() -> End");
	}

	void CopyProduct::GetUpdatedColorwayNames()
	{
		Logger::Debug("CopyProduct -> GetUpdatedColorwayNames() -> Start");
		m_modifiedColorwayNames.clear();
		int comboSize;
		int comboBoxIndex;;
		vector<int> comboBoxIndexList;
		int noOfRows = ui_ColorwayTable->rowCount();

		for (int rowIndex = 0; rowIndex < noOfRows; rowIndex++)
		{
			QComboBox* colorwayNameCombo = qobject_cast<QComboBox*>(ui_ColorwayTable->cellWidget(rowIndex, 1));

			if (colorwayNameCombo)
			{
				comboSize = colorwayNameCombo->count();
				m_modifiedColorwayNames << colorwayNameCombo->currentText();
				comboBoxIndex = colorwayNameCombo->currentIndex();
				comboBoxIndexList.push_back(comboBoxIndex);
			}
		}
		m_modifiedColorwayNames.clear();
		QStringList colorwayNamesList;
		int colorwayCount = UTILITY_API->GetColorwayCount();
		colorwayNamesList.append("Empty");
		for (int count = 0; count < colorwayCount; count++)
		{
			string colorwayName = UTILITY_API->GetColorwayName(count);
			colorwayNamesList.append(QString::fromStdString(colorwayName));
		}

		for (int rowIndex = 0; rowIndex < ui_ColorwayTable->rowCount(); rowIndex++)
		{
			QComboBox* colorwayNameCombo = qobject_cast<QComboBox*>(ui_ColorwayTable->cellWidget(rowIndex, 1));
			if (colorwayNameCombo)
			{
				comboSize = colorwayNameCombo->count();
				colorwayNameCombo->setCurrentIndex(comboBoxIndexList[rowIndex]);
				colorwayNameCombo->clear();
				colorwayNameCombo->addItems(colorwayNamesList);
				colorwayNameCombo->setCurrentIndex(comboBoxIndexList[rowIndex]);
				if (colorwayNameCombo->currentText() != colorwayNamesList[comboBoxIndexList[rowIndex]])
				{
					colorwayNameCombo->currentText() == colorwayNamesList[comboBoxIndexList[rowIndex]];
					m_modifiedColorwayNames << colorwayNamesList[comboBoxIndexList[rowIndex]];
				}
				else
				{
					m_modifiedColorwayNames << colorwayNameCombo->currentText();
				}
			}
		}
		Logger::Debug("CopyProduct -> GetUpdatedColorwayNames() -> End");
	}

	void CopyProduct::AddRows(int _count, string _objectId, string _objectName, string _rgbValue, string _code, string _pantone, QStringList _colorwayNamesList)
	{

		Logger::Debug("CopyProduct -> AddRows() -> Start");
		QWidget* selectItem = CVWidgetGenerator::CreateCheckBoxWidget(false, _objectId, "", _objectName);
		ui_ColorwayTable->setCellWidget(_count + m_colorwayRowcount, 0, selectItem);

		QComboBox* comboColorwayItem = new QComboBox();
		comboColorwayItem->addItems(_colorwayNamesList);
		comboColorwayItem->setProperty("Id", QString::fromStdString(_objectId));
		if (m_modifiedColorwayNames.count() == 0)
		{
			ui_ColorwayTable->setCellWidget(_count + m_colorwayRowcount, 1, comboColorwayItem);
		}
		else if (m_modifiedColorwayNames.count() <= m_colorwayRowcount)
		{
			if (_count < m_modifiedColorwayNames.count())
			{
				int indexOfSelectedString = comboColorwayItem->findText(m_modifiedColorwayNames[_count]);
				comboColorwayItem->setCurrentIndex(indexOfSelectedString);
			}
			else
			{
				comboColorwayItem->clear();
				comboColorwayItem->addItems(_colorwayNamesList);
			}
			ui_ColorwayTable->setCellWidget(_count + m_colorwayRowcount, 1, comboColorwayItem);
		}
		QObject::connect(comboColorwayItem, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(OnHandleColorwayNameComboBox(const QString&)));
		QSize iconSize(40, 40);
		ui_ColorwayTable->setIconSize(iconSize);
		ui_ColorwayTable->setWordWrap(true);

		QTableWidgetItem* iconItem = new QTableWidgetItem;
		iconItem->setSizeHint(iconSize);


		Logger::Debug("colorRGB:: " + _rgbValue);

		QStringList listRGB;

		int length = _rgbValue.length();
		int index = _rgbValue.find("[");
		string FinalresultJsonString = _rgbValue.substr(index + 1, length - 1);

		QString colorRGB = QString::fromStdString(FinalresultJsonString);
		listRGB = colorRGB.split(',');


		int red = listRGB.at(0).toInt();
		int green = listRGB.at(1).toInt();
		int blue = listRGB.at(2).toInt();
		QColor color(red, green, blue);
		QImage image(40, 40, QImage::Format_ARGB32);
		image.fill(color);

		iconItem->setIcon(QIcon(QPixmap::fromImage(image)));
		iconItem->setToolTip(colorRGB);
		ui_ColorwayTable->setItem(_count + m_colorwayRowcount, 2, iconItem);


		QTableWidgetItem* ColorNameWidget = new QTableWidgetItem(QString::fromStdString(_objectName));
		ColorNameWidget->setTextAlignment(Qt::AlignCenter);
		ColorNameWidget->setToolTip(QString::fromStdString(_objectName));
		ui_ColorwayTable->setItem(_count + m_colorwayRowcount, 3, ColorNameWidget);

		QTableWidgetItem* ColorCodeWidget = new QTableWidgetItem(QString::fromStdString(_code));
		ColorCodeWidget->setTextAlignment(Qt::AlignCenter);
		ColorCodeWidget->setToolTip(QString::fromStdString(_objectId));
		ui_ColorwayTable->setItem(_count + m_colorwayRowcount, 4, ColorCodeWidget);


		QTableWidgetItem* ColorStatusWidget = new QTableWidgetItem(QString::fromStdString(_pantone));
		ColorStatusWidget->setTextAlignment(Qt::AlignCenter);
		ColorStatusWidget->setToolTip(QString::fromStdString(_pantone));
		ui_ColorwayTable->setItem(_count + m_colorwayRowcount, 5, ColorStatusWidget);

		QLineEdit* lineEditItem = new QLineEdit();
		lineEditItem->setStyleSheet(LINEEDIT_STYLE);
		lineEditItem->setAttribute(Qt::WA_MacShowFocusRect, false);
		lineEditItem->setText("");
		ui_ColorwayTable->setCellWidget(_count + m_colorwayRowcount, 6, lineEditItem);

		QTextEdit* textEditItem = new QTextEdit();
		textEditItem->setStyleSheet(TEXTEDIT_STYLE);
		textEditItem->setText("");
		ui_ColorwayTable->setCellWidget(_count + m_colorwayRowcount, 7, textEditItem);
		Logger::Debug("CopyProduct -> AddRows() -> End");
	}

	void CopyProduct::onSearchStyleClicked()
	{
		Logger::Debug("CopyProduct -> onSearchStyleClicked() -> Start");
		this->hide();  
		UTILITY_API->CreateProgressBar();
		RESTAPI::SetProgressBarData(10, "Loading Style Search", true);
		ProductConfig::GetInstance()->GetProductFieldsJSON();
		Configuration::GetInstance()->SetCurrentScreen(COPY_PRODUCT_CLICKED);
		ProductConfig::GetInstance()->m_isShow3DAttWidget = false;
		PLMProductSearch::GetInstance()->setModal(true);
		PLMProductSearch::GetInstance()->exec();
		UTILITY_API->DeleteProgressBar(true);
		Logger::Debug("CopyProduct -> onSearchStyleClicked() -> End");
	}

	void CopyProduct::OnHandleDropDownValue(const QString& _item)
	{

		Logger::Debug("CopyProduct OnHandleDropDownValue() Start");
		map<string, string>::iterator it;
		map < string, string> nameIdMap;
		string id;

		QString labelName = sender()->property("LabelName").toString();
		string apiUrl = "";
		QString comboboxtofill = "";
		string progressbarText;

		if (!_item.isEmpty())
		{
			QStringList dependentFields;

			if (labelName == "Season")
			{
				apiUrl = RESTAPI::SEASON_SEARCH_API;
				nameIdMap = m_seasonNameIdMap;
				comboboxtofill = "Brand";
				dependentFields.append("category_1");
				dependentFields.append("shape");
				dependentFields.append("theme");
				clearDependentComboBox(labelName, m_CopyProductTreeWidget_1);
				clearDependentComboBox(labelName, m_CopyProductTreeWidget_2);
				progressbarText = "Loading Brand details..";
			}
			else if (labelName == "Brand")
			{
				apiUrl = RESTAPI::DEPARTMENT_DETAIL_API;
				Logger::Debug("CopyProduct -> OnHandleDropDownValue() m_brandNameIdMap " + to_string(m_brandNameIdMap.size()));
				nameIdMap = m_brandNameIdMap;
				dependentFields.append("category_2");
				clearDependentComboBox(labelName, m_CopyProductTreeWidget_1);
				clearDependentComboBox(labelName, m_CopyProductTreeWidget_2);
				progressbarText = "Loading Department details..";
			}
			else if (labelName == "Department")
			{
				apiUrl = RESTAPI::COLLECTION_DETAIL_API;
				Logger::Debug("CopyProduct -> OnHandleDropDownValue() m_brandNameIdMap " + to_string(m_dipartmentNameIdMap.size()));
				nameIdMap = m_brandNameIdMap;
				dependentFields.append("collection");
				clearDependentComboBox(labelName, m_CopyProductTreeWidget_1);
				clearDependentComboBox(labelName, m_CopyProductTreeWidget_2);
				progressbarText = "Loading Division details..";
			}
			if (labelName == "Season" || labelName == "Brand" || labelName == "Department")
			{
				Configuration::GetInstance()->SetProgressBarProgress(0);
				RESTAPI::SetProgressBarData(40, progressbarText, true);
				UTILITY_API->SetProgress(progressbarText, (qrand() % 101));
			}

			it = nameIdMap.find(_item.toStdString());//
			if (it != nameIdMap.end())
			{
				id = it->second;
				string attName, attId;

				Logger::Debug("CopyProduct -> SetDocumentConfigJSON Season Id: " + id);
				Logger::Debug("CopyProduct -> SetDocumentConfigJSON valueList size: " + to_string(dependentFields.size()));
				for (int itemIndex = 0; itemIndex < m_CopyProductTreeWidget_1->topLevelItemCount(); ++itemIndex)
				{
					QTreeWidgetItem* topItem = m_CopyProductTreeWidget_1->topLevelItem(itemIndex);
					QWidget* qWidgetColumn_0 = m_CopyProductTreeWidget_1->itemWidget(topItem, 0);
					QWidget* qWidgetColumn_1 = m_CopyProductTreeWidget_1->itemWidget(topItem, 1);
					if (!qWidgetColumn_0 || !qWidgetColumn_1)
					{
						continue;
					}
					QLabel* qlabel = qobject_cast<QLabel*>(qWidgetColumn_0);
					QString lableText = qlabel->property(ATTRIBUTE_KEY.c_str()).toString();

					Logger::Debug("CopyProduct -> OnHandleDropDownValue() Label text: " + (lableText.toStdString()));
					if (dependentFields.contains(lableText))
					{
						QStringList valueList;
						valueList.append(QString::fromStdString(BLANK));
						json dependentFieldJson = json::object();
						ComboBoxItem* qComboBox = qobject_cast<ComboBoxItem*>(qWidgetColumn_1);
						if ((qComboBox))
						{
							if (lableText == "shape")
							{
								progressbarText = "Loading Shape details..";
								dependentFieldJson = Helper::makeRestcallGet(RESTAPI::SHAPE_API, "&skip=0&limit=100", "?shape_seasons=" + id, progressbarText);

							}
							else if (lableText == "theme")
							{
								progressbarText = "Loading Theme details..";
								dependentFieldJson = Helper::makeRestcallGet(RESTAPI::THEME_API, "&skip=0&limit=100", "?theme_seasons=" + id, progressbarText);
							}
							else
							{
								dependentFieldJson = Helper::makeRestcallGet(apiUrl, "/hierarchy", "/" + id, progressbarText);
							}
							json attJson = json::object();

							for (int i = 0; i < dependentFieldJson.size(); i++)
							{
								attJson = Helper::GetJSONParsedValue<int>(dependentFieldJson, i, false);;///use new method
								attName = Helper::GetJSONValue<string>(attJson, ATTRIBUTE_NAME, true);
								Logger::Debug("CopyProduct -> OnHandleDropDownValue() attName: " + attName);
								attId = Helper::GetJSONValue<string>(attJson, ATTRIBUTE_ID, true);
								Logger::Debug("CopyProduct -> OnHandleDropDownValue() attId: " + attId);
								valueList.append(QString::fromStdString(attName));
								qComboBox->setProperty(attName.c_str(), QString::fromStdString(attId));
								m_brandNameIdMap.insert(make_pair(attName, attId));
							}
							qComboBox->clear();
							if (qComboBox->isEnabled())
							{
								qComboBox->setEditable(true);
								qComboBox->fillItemListAndDefaultValue(valueList, qComboBox->currentText());
								QCompleter* m_nameCompleter = new QCompleter(valueList, qComboBox); // using QCompleter class
								m_nameCompleter->setCompletionMode(QCompleter::PopupCompletion);
								m_nameCompleter->setCaseSensitivity(Qt::CaseInsensitive);
								qComboBox->setCompleter(m_nameCompleter);
							}
							valueList.sort();
							qComboBox->addItems(valueList);
							valueList.clear();
						}
					}
				}
				for (int itemIndex = 0; itemIndex < m_CopyProductTreeWidget_2->topLevelItemCount(); ++itemIndex)
				{
					QTreeWidgetItem* topItem = m_CopyProductTreeWidget_2->topLevelItem(itemIndex);
					QWidget* qWidgetColumn_0 = m_CopyProductTreeWidget_2->itemWidget(topItem, 0);
					QWidget* qWidgetColumn_1 = m_CopyProductTreeWidget_2->itemWidget(topItem, 1);
					if (!qWidgetColumn_0 || !qWidgetColumn_1)
					{
						continue;
					}
					QLabel* qlabel = qobject_cast<QLabel*>(qWidgetColumn_0);
					QString lableText = qlabel->property(ATTRIBUTE_KEY.c_str()).toString();

					Logger::Debug("CopyProduct -> OnHandleDropDownValue() Label text: " + (lableText.toStdString()));
					if (dependentFields.contains(lableText))
					{
						QStringList valueList;
						valueList.append(QString::fromStdString(BLANK));
						json dependentFieldJson = json::object();
						ComboBoxItem* qComboBox = qobject_cast<ComboBoxItem*>(qWidgetColumn_1);
						if ((qComboBox))
						{
							if (lableText == "shape")
							{
								progressbarText = "Loading Shape details..";
								dependentFieldJson = Helper::makeRestcallGet(RESTAPI::SHAPE_API, "&skip=0&limit=100", "?shape_seasons=" + id, progressbarText);

							}
							else if (lableText == "theme")
							{
								progressbarText = "Loading Theme details..";
								dependentFieldJson = Helper::makeRestcallGet(RESTAPI::THEME_API, "&skip=0&limit=100", "?theme_seasons=" + id, progressbarText);

							}
							else
							{
								dependentFieldJson = Helper::makeRestcallGet(apiUrl, "/hierarchy", "/" + id, progressbarText);
							}

							json attJson = json::object();

							for (int i = 0; i < dependentFieldJson.size(); i++)
							{
								attJson = Helper::GetJSONParsedValue<int>(dependentFieldJson, i, false);;///use new method
								attName = Helper::GetJSONValue<string>(attJson, ATTRIBUTE_NAME, true);
								Logger::Debug("CopyProduct -> OnHandleDropDownValue() attName: " + attName);
								attId = Helper::GetJSONValue<string>(attJson, ATTRIBUTE_ID, true);
								Logger::Debug("CopyProduct -> OnHandleDropDownValue() attId: " + attId);
								valueList.append(QString::fromStdString(attName));
								qComboBox->setProperty(attName.c_str(), QString::fromStdString(attId));
								m_brandNameIdMap.insert(make_pair(attName, attId));
							}
							qComboBox->clear();
							if (qComboBox->isEnabled())
							{
								qComboBox->setEditable(true);
								qComboBox->fillItemListAndDefaultValue(valueList, qComboBox->currentText());
								QCompleter* m_nameCompleter = new QCompleter(valueList, qComboBox); // using QCompleter class
								m_nameCompleter->setCompletionMode(QCompleter::PopupCompletion);
								m_nameCompleter->setCaseSensitivity(Qt::CaseInsensitive);
								qComboBox->setCompleter(m_nameCompleter);
							}
							valueList.sort();
							qComboBox->addItems(valueList);
							valueList.clear();
						}
					}

				}
			}

		}
		else
		{
			clearDependentComboBox(labelName, m_CopyProductTreeWidget_1);
			clearDependentComboBox(labelName, m_CopyProductTreeWidget_2);
		}

		UTILITY_API->DeleteProgressBar(true);
		Logger::Debug("CopyProduct OnHandleDropDownValue() End");
	}

	void CopyProduct::ReadVisualUIFieldValue(QTreeWidget* _searchTree, int _rowIndex, string& _data)
	{
		Logger::Debug("Copy product ReadVisualUIFieldValue() Start");
		string fieldValue = "";
		string fieldLabel = "";
		string labelText = "";
		string attKey = ATTRIBUTE_KEY;
		string attType = "";
		string id;

		//string data;
		map<string, string>::iterator it;
		map < string, string> nameIdMap;
		QTreeWidgetItem* topItem = _searchTree->topLevelItem(_rowIndex);
		string fieldVal;
		QWidget* qWidgetColumn0 = _searchTree->itemWidget(topItem, 0);
		QLabel* qlabel = qobject_cast<QLabel*>(qWidgetColumn0);
		labelText = qlabel->text().toStdString();
		fieldLabel = qlabel->property(attKey.c_str()).toString().toStdString();
		attType = qlabel->property(ATTRIBUTE_TYPE_KEY.c_str()).toString().toStdString();
		QWidget* qWidgetColumn1 = _searchTree->itemWidget(topItem, 1);
		bool isEnabled = qWidgetColumn1->isEnabled();
		if (QLineEdit* qLineEditC1 = qobject_cast<QLineEdit*>(qWidgetColumn1))
		{
			fieldValue = qLineEditC1->text().toStdString();
			Logger::Debug("Copy product ReadVisualUIFieldValue() QLineEdit->fieldValue" + fieldValue);
			Logger::Debug("Copy product ReadVisualUIFieldValue() QLineEdit->fieldLabel" + fieldLabel);
			Logger::Debug("Copy product ReadVisualUIFieldValue() QLineEdit->labelText" + labelText);

			fieldVal = qLineEditC1->property(fieldValue.c_str()).toString().toStdString();
			Logger::Debug("Copy product ReadVisualUIFieldValue() QLineEdit->fieldVal" + fieldVal);
			if (!fieldVal.empty())
				fieldValue = fieldVal;
		}
		else if (QTextEdit* qTextC1 = qobject_cast<QTextEdit*>(qWidgetColumn1))
		{
			fieldValue = qTextC1->toPlainText().toStdString();
		}
		else if (QDateEdit* qDateC1 = qobject_cast<QDateEdit*>(qWidgetColumn1))
		{
			fieldValue = FormatHelper::RetrieveDate(qDateC1);
			if (fieldValue.find(DATE_FORMAT_TEXT.toStdString()) != string::npos)
			{
				fieldValue = "";
			}
			else
			{
				fieldValue = fieldValue + "T00:00:00Z";
			}
		}
		else if (QListWidget* listC1 = qobject_cast<QListWidget*>(qWidgetColumn1))
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
		else if (QDoubleSpinBox* doubleSpinC1 = qobject_cast<QDoubleSpinBox*>(qWidgetColumn1))
		{
			if (doubleSpinC1->value() != 0.00000)
			{
				fieldValue = to_string(doubleSpinC1->value());
			}
		}
		else if (QSpinBox* SpinC1 = qobject_cast<QSpinBox*>(qWidgetColumn1))
		{
			if (SpinC1->value() != 0)
			{
				fieldValue = to_string(SpinC1->value());
			}
		}
		else if (QComboBox* qComboBoxC1 = qobject_cast<QComboBox*>(qWidgetColumn1))
		{
			fieldValue = qComboBoxC1->currentText().toStdString();
			Logger::Debug("Copy product ReadVisualUIFieldValue() QComboBox->fieldValue" + fieldValue);
			Logger::Debug("Copy product ReadVisualUIFieldValue() QComboBox->fieldLabel" + fieldLabel);
			Logger::Debug("Copy product ReadVisualUIFieldValue() QComboBox->labelText" + labelText);
			fieldVal = qComboBoxC1->property(fieldValue.c_str()).toString().toStdString();
			Logger::Debug("Copy product ReadVisualUIFieldValue() QComboBox->fieldVal" + fieldVal);
			if (!fieldVal.empty())
			{
				fieldValue = fieldVal;
			}
			/*if (labelText == "Style Type")
				fieldValue = BLANK;*/
		}
		if (!fieldLabel.empty())
		{
			if (fieldLabel == "from_style_id" || fieldLabel == "style_copy_options_id" || fieldLabel == "node_name" || fieldLabel == "collection")
			{
				if (fieldLabel == "collection")
				{
					fieldLabel = "parent_id";
					if (fieldValue.empty())
						fieldValue = m_collectionId;
				}
				m_copydataRequest += "\n\"" + fieldLabel + "\":\"" + fieldValue + "\",";
			}
			else
			{
				if (!fieldValue.empty() && isEnabled)
				      _data += "\n\"" + fieldLabel + "\":\"" + fieldValue + "\",";
			}
		}
		Logger::Debug("Copy product ReadVisualUIFieldValue() End");
	}

	void CopyProduct::ClearAllFields(QTreeWidget* _treeWidget)
	{
		Logger::Debug("CopyProduct -> ClearAllFields() -> Start");
		for (int itemIndex = 0; itemIndex < _treeWidget->topLevelItemCount(); ++itemIndex)
		{
			QTreeWidgetItem* topItem = _treeWidget->topLevelItem(itemIndex);
			QWidget* qWidgetColumn0 = _treeWidget->itemWidget(topItem, 0);
			QWidget* qWidgetColumn1 = _treeWidget->itemWidget(topItem, 1);
			if (!qWidgetColumn0 || !qWidgetColumn1)
			{
				continue;
			}
			QLabel* qlabel = qobject_cast<QLabel*>(qWidgetColumn0);
			string label = qlabel->text().toStdString();
			QLineEdit* qlineedit = qobject_cast<QLineEdit*>(qWidgetColumn1);
			QTextEdit* qtextEdit = qobject_cast<QTextEdit*>(qWidgetColumn1);
			QComboBox* qComboBox = qobject_cast<QComboBox*>(qWidgetColumn1);
			QDateEdit* qDateEdit = qobject_cast<QDateEdit*>(qWidgetColumn1);
			QSpinBox* qspinBox = qobject_cast<QSpinBox*>(qWidgetColumn1);
			ComboBoxItem* qComboBoxItem = qobject_cast<ComboBoxItem*>(qWidgetColumn1);
			QString labelId = qlabel->property("Id").toString();
			/*if (qlineedit)
			{
				qlineedit->setText("");
			}
			else
			{
				QComboBox* qComboBox = qobject_cast<QComboBox*>(qWidgetColumn1);
				if (qComboBox)
				{
					int indexOfEmptyString = qComboBox->findText(QString::fromStdString(BLANK));
					qComboBox->setCurrentIndex(indexOfEmptyString);

				}
				else
				{
					QTextEdit* qtextEdit = qobject_cast<QTextEdit*>(qWidgetColumn1);
					if (qtextEdit)
					{
						qtextEdit->setText("");
					}
				}
			}*/

			if (qlineedit)
			{
				qlineedit->setText("");
			}
			else if (qComboBox)
			{
				int indexOfEmptyString = qComboBox->findText(QString::fromStdString(BLANK));
				qComboBox->setCurrentIndex(indexOfEmptyString);
			}
			else if (qDateEdit)
			{
				QLineEdit *edit = qDateEdit->findChild<QLineEdit*>("qt_spinbox_lineedit");
				if (!edit->text().isEmpty())
				{
					edit->clear();
					qDateEdit->setMinimumDate(QDate(1899, 01, 01));
					qDateEdit->setSpecialValueText("YYYY-MM-DD");
					qDateEdit->setDate(QDate(0001, 01, 01));
				}
			}
			else if (qComboBoxItem)
			{
				int indexOfEmptyString = qComboBoxItem->findText(QString::fromStdString(BLANK));
				qComboBox->setCurrentIndex(indexOfEmptyString);
			}
			else if (qspinBox)
			{
				if (qspinBox->value() != 0)
				{
					qspinBox->setValue(0);
				}
			}
			else if (QDoubleSpinBox* SpinC1 = qobject_cast<QDoubleSpinBox*>(qWidgetColumn1))
			{
				if (SpinC1->value() != 0)
				{
					SpinC1->setValue(0);
				}
			}
		}
		Logger::Debug("CopyProduct -> ClearAllFields() -> End");
	}

	void CopyProduct::SetDownloadedColorwayDetails()
	{
		Logger::Debug("CopyProduct -> SetDownloadedColorwayDetails() -> Start");
		try
		{
			json jsonvalue = json::object();
			string columnsNames = DirectoryUtil::GetPLMPluginDirectory() + "CreateColorwayDetail.json";//Reading Columns from json
			jsonvalue = Helper::ReadJSONFile(columnsNames);
			AddColorwayDetails(jsonvalue);
		}
		catch (string msg)
		{
			Logger::Error("CopyProduct -> SetDownloadedColorwayDetails() Exception - " + msg);
		}
		catch (exception & e)
		{
			Logger::Error("CopyProduct -> SetDownloadedColorwayDetails() Exception - " + string(e.what()));
		}
		catch (const char* msg)
		{
			wstring wstr(msg, msg + strlen(msg));
			Logger::Error("CopyProduct -> SetDownloadedColorwayDetails() Exception - " + string(msg));
		}
		Logger::Debug("CopyProduct -> SetDownloadedColorwayDetails() -> End");
	}

	void CopyProduct::clearDependentComboBox(QString _lable, QTreeWidget* _searchTree)
	{
		for (int itemIndex = 0; itemIndex < _searchTree->topLevelItemCount(); ++itemIndex)
		{

			QTreeWidgetItem* topItem = _searchTree->topLevelItem(itemIndex);
			QWidget* qWidgetColumn_0 = _searchTree->itemWidget(topItem, 0);
			QWidget* qWidgetColumn_1 = _searchTree->itemWidget(topItem, 1);
			QString lableText;
			if (!qWidgetColumn_0 || !qWidgetColumn_1)
			{
				continue;
			}
			QLabel* qlabel = qobject_cast<QLabel*>(qWidgetColumn_0);
			if(qlabel)
				lableText = qlabel->property("attName").toString();
			QComboBox* qComboBox = qobject_cast<QComboBox*>(qWidgetColumn_1);
			Logger::Debug("Copy Product -> clearDependentComboBox () _lable_lablevim: " + _lable.toStdString());
			if (_lable == "Season")
			{
				//if (lableText == "Division")
				if (lableText == "Brand" || lableText == "Division" || lableText == "Department" || lableText == "Item Code" || lableText == "Planning Sum")
				{
					Logger::Debug("Copy Product -> clearDependentComboBox () lableText: " + lableText.toStdString());
					if (qComboBox)
					{
						qComboBox->clear();
						qComboBox->addItem(QString::fromStdString(BLANK));
						qComboBox->setCurrentText(QString::fromStdString(BLANK));
					}
				}
			}
			else if (_lable == "Brand")
			{
				//if (lableText == "Division")
				if (lableText == "Division" || lableText == "Department")
				{
					if (qComboBox)
					{
						qComboBox->clear();
						qComboBox->addItem(QString::fromStdString(BLANK));
						qComboBox->setCurrentText(QString::fromStdString(BLANK));
					}
				}
			}
			else if (_lable == "Department")
			{
				if (lableText == "Division")
				{
					if (qComboBox)
					{
						qComboBox->clear();
						qComboBox->addItem(QString::fromStdString(BLANK));
						qComboBox->setCurrentText(QString::fromStdString(BLANK));
					}
				}
			}

		}
	}


	void CopyProduct::UploadStyleThumbnail(string _productId)
	{

		string path;
		string _3DModelFilePath = UTILITY_API->GetProjectFilePath();
		path = _3DModelFilePath;
		Helper::EraseSubString(path, UTILITY_API->GetProjectName());
		m_3DModelThumbnailName = UTILITY_API->GetProjectName() + ".png";
		UTILITY_API->DisplayMessageBox("m_3DModelThumbnailName: " + m_3DModelThumbnailName);
		m_3DModelThumbnailPath = path + m_3DModelThumbnailName;
		UTILITY_API->DisplayMessageBox("m_3DModelThumbnailPath: " + m_3DModelThumbnailPath);
		string postField = getPublishRequestParameter(m_3DModelThumbnailPath, m_3DModelThumbnailName);
		string resultJsonString = RESTAPI::PostRestCall(postField, Configuration::GetInstance()->GetPLMServerURL() + RESTAPI::UPLOAD_IMAGE_API, "content-type: multipart/form-data; boundary=----WebKitFormBoundary7MA4YWxkTrZu0gW");
		UTILITY_API->DisplayMessageBox("thumbnail uploaded:  " + resultJsonString);
		json detailJson = Helper::GetJsonFromResponse(resultJsonString, "{");
		string imageId = Helper::GetJSONValue<string>(detailJson, ATTRIBUTE_ID, true);
		string data = "{\n\"images\":{\n\"\":\"" + imageId + "\",\n\"1\":\"" + imageId + "\"\n}\n}";
		UTILITY_API->DisplayMessageBox(data);
		resultJsonString = RESTAPI::PutRestCall(data, Configuration::GetInstance()->GetPLMServerURL() + RESTAPI::STYLE_ENDPOINT_API + "/" + _productId, "content-type: application/json");
		UTILITY_API->DisplayMessageBox("ImageLinked " + resultJsonString);

	}

	void CopyProduct::AddStyleDetails(json _downloadJSON)
	{
		string styleId = Helper::GetJSONParsedValue<string>(_downloadJSON, ATTRIBUTE_ID, false);
		string styleName = Helper::GetJSONParsedValue<string>(_downloadJSON, ATTRIBUTE_NAME, false);
		m_collectionId = Helper::GetJSONParsedValue<string>(_downloadJSON, "collection", false);
		Logger::Debug("CopyProduct -> AddStyleDetails () styleId" + styleId);
		Logger::Debug("CopyProduct -> AddStyleDetails () Start");
		Logger::Debug("_downloadJSON::" + to_string(_downloadJSON));
		for (int itemIndex = 0; itemIndex < m_CopyProductTreeWidget_1->topLevelItemCount(); ++itemIndex)
		{

			QTreeWidgetItem* topItem = m_CopyProductTreeWidget_1->topLevelItem(itemIndex);
			QWidget* qWidgetColumn_0 = m_CopyProductTreeWidget_1->itemWidget(topItem, 0);
			QWidget* qWidgetColumn_1 = m_CopyProductTreeWidget_1->itemWidget(topItem, 1);

			if (!qWidgetColumn_0 || !qWidgetColumn_1)
			{
				continue;
			}
			QLabel* qlabel = qobject_cast<QLabel*>(qWidgetColumn_0);
			QString lableText = qlabel->property(ATTRIBUTE_NAME_KEY.c_str()).toString();
			QString lableTextKey = qlabel->property(ATTRIBUTE_KEY.c_str()).toString();
			ComboBoxItem* qComboBox = qobject_cast<ComboBoxItem*>(qWidgetColumn_1);
			QLineEdit* qLineEdit = qobject_cast<QLineEdit*>(qWidgetColumn_1);
			json attJson;
			string attName;
			string attId;
			QStringList valueList;
			if (qComboBox)
			{
				if (lableText == "Copy Template")
				{
					string enumListResponse = RESTAPI::CentricRestCallGet(Configuration::GetInstance()->GetPLMServerURL() + RESTAPI::STYLE_COPY_OPTION_API + "?decode-true&root_sub_type_name=Apparel", APPLICATION_JSON_TYPE, "");
					Logger::RestAPIDebug("enumListResponse::" + enumListResponse);

					json response = json::parse(enumListResponse);
					valueList.append(QString::fromStdString(BLANK));

					for (int i = 0; i < response.size(); i++)
					{
						attJson = Helper::GetJSONParsedValue<int>(response, i, false);///use new method
						attName = Helper::GetJSONValue<string>(attJson, ATTRIBUTE_NAME, true);
						Logger::Debug("PublishToPLMData -> AddStyleDetails attName: " + attName);
						attId = Helper::GetJSONValue<string>(attJson, ATTRIBUTE_ID, true);
						Logger::Debug("PublishToPLMData -> AddStyleDetails attId: " + attId);
						valueList.append(QString::fromStdString(attName));
						qComboBox->setProperty(attName.c_str(), QString::fromStdString(attId));
					}
					qComboBox->clear();
					if (qComboBox->isEnabled())
					{
						qComboBox->setEditable(true);
						qComboBox->fillItemListAndDefaultValue(valueList, qComboBox->currentText());
						QCompleter* m_nameCompleter = new QCompleter(valueList, qComboBox); // using QCompleter class
						m_nameCompleter->setCompletionMode(QCompleter::PopupCompletion);
						m_nameCompleter->setCaseSensitivity(Qt::CaseInsensitive);
						qComboBox->setCompleter(m_nameCompleter);
					}
					valueList.sort();
					qComboBox->addItems(valueList);
				}
				else
				{
					string defaultValueStr = Helper::GetJSONValue<string>(_downloadJSON, lableTextKey.toStdString(), false);
					if (FormatHelper::HasContent(defaultValueStr))
					{
						defaultValueStr = qComboBox->property(defaultValueStr.c_str()).toString().toStdString();
						if (!qComboBox->findText(QString::fromStdString(defaultValueStr), Qt::MatchExactly | Qt::MatchFixedString))
							defaultValueStr = qComboBox->property(defaultValueStr.c_str()).toString().toStdString();
						qComboBox->setCurrentText(QString::fromStdString(defaultValueStr));
						}
				}

			}
			else if (qLineEdit)
			{
				if (lableText == "From Style")
				{
					qLineEdit->setText(QString::fromStdString(styleName));
					qLineEdit->setProperty(styleName.c_str(), QString::fromStdString(styleId));
				}
				else
				{
					string defaultValueStr = Helper::GetJSONParsedValue<string>(_downloadJSON, lableTextKey.toStdString(), false);
					if (FormatHelper::HasContent(defaultValueStr))
					{
						qLineEdit->setText(QString::fromStdString(defaultValueStr));
					}
				}
			}
		}
		for (int itemIndex = 0; itemIndex < m_CopyProductTreeWidget_2->topLevelItemCount(); ++itemIndex)
		{

			QTreeWidgetItem* topItem = m_CopyProductTreeWidget_2->topLevelItem(itemIndex);
			QWidget* qWidgetColumn_0 = m_CopyProductTreeWidget_2->itemWidget(topItem, 0);
			QWidget* qWidgetColumn_1 = m_CopyProductTreeWidget_2->itemWidget(topItem, 1);

			if (!qWidgetColumn_0 || !qWidgetColumn_1)
			{
				continue;
			}
			QLabel* qlabel = qobject_cast<QLabel*>(qWidgetColumn_0);
			QString lableText = qlabel->property(ATTRIBUTE_NAME_KEY.c_str()).toString();
			QString lableTextKey = qlabel->property(ATTRIBUTE_KEY.c_str()).toString();
			ComboBoxItem* qComboBox = qobject_cast<ComboBoxItem*>(qWidgetColumn_1);
			QLineEdit* qLineEdit = qobject_cast<QLineEdit*>(qWidgetColumn_1);
			json attJson;
			string attName;
			string attId;
			QStringList valueList;
			if (qComboBox)
			{
				if (lableText == "Copy Template")
				{
					string enumListResponse = RESTAPI::CentricRestCallGet(Configuration::GetInstance()->GetPLMServerURL() + RESTAPI::STYLE_COPY_OPTION_API + "?decode=true&root_sub_type_name=Apparel", APPLICATION_JSON_TYPE, "");
					Logger::RestAPIDebug("enumListResponse::" + enumListResponse);

					json response = json::parse(enumListResponse);
					valueList.append(QString::fromStdString(BLANK));

					for (int i = 0; i < response.size(); i++)
					{
						attJson = Helper::GetJSONParsedValue<int>(response, i, false);///use new method
						attName = Helper::GetJSONValue<string>(attJson, ATTRIBUTE_NAME, true);
						Logger::Debug("PublishToPLMData -> AddStyleDetails attName: " + attName);
						attId = Helper::GetJSONValue<string>(attJson, ATTRIBUTE_ID, true);
						Logger::Debug("PublishToPLMData -> AddStyleDetails attId: " + attId);
						valueList.append(QString::fromStdString(attName));
						qComboBox->setProperty(attName.c_str(), QString::fromStdString(attId));
					}
					qComboBox->clear();
					if (qComboBox->isEnabled())
					{
						qComboBox->setEditable(true);
						qComboBox->fillItemListAndDefaultValue(valueList, qComboBox->currentText());
						QCompleter* m_nameCompleter = new QCompleter(valueList, qComboBox); // using QCompleter class
						m_nameCompleter->setCompletionMode(QCompleter::PopupCompletion);
						m_nameCompleter->setCaseSensitivity(Qt::CaseInsensitive);
						qComboBox->setCompleter(m_nameCompleter);
					}
					valueList.sort();
					qComboBox->addItems(valueList);
				}
				else
				{
					string defaultValueStr = Helper::GetJSONValue<string>(_downloadJSON, lableTextKey.toStdString(), false);
					if (FormatHelper::HasContent(defaultValueStr))
					{
						defaultValueStr = qComboBox->property(defaultValueStr.c_str()).toString().toStdString();
						if (!qComboBox->findText(QString::fromStdString(defaultValueStr), Qt::MatchExactly | Qt::MatchFixedString))
							defaultValueStr = qComboBox->property(defaultValueStr.c_str()).toString().toStdString();
						qComboBox->setCurrentText(QString::fromStdString(defaultValueStr));
					}
				}

			}
			else if (qLineEdit)
			{
				if (lableText == "From Style")
				{
					qLineEdit->setText(QString::fromStdString(styleName));
					qLineEdit->setProperty(styleName.c_str(), QString::fromStdString(styleId));
				}
				else
				{
					string defaultValueStr = Helper::GetJSONParsedValue<string>(_downloadJSON, lableTextKey.toStdString(), false);
					if (FormatHelper::HasContent(defaultValueStr))
					{
						qLineEdit->setText(QString::fromStdString(defaultValueStr));
					}
				}
			}
		}
		ui_tabWidget->show();
		ui_overviewTab->show();
		Logger::Debug("CopyProduct -> AddStyleDetails () End");
	}

	bool CopyProduct::IsCopyCreated()
	{
		return m_copyCreated;
	}

	void CopyProduct::SetCopyCreated(bool _copy)
	{
	 m_copyCreated = _copy;
	}
}