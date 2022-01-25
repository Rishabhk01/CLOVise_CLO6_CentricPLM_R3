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
#include "CreateProduct.h"
#include <string>
#include <cstring>

#include <QTextEdit>
#include <QComboBox>
#include <QLabel>
#include "qtreewidget.h"
#include <QFile>
#include "qdir.h"
#include "CreateImageIntent.h"
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
#include "CLOVise/PLM/Inbound/Print/PLMPrintSearch.h"
#include "CLOVise/PLM/Inbound/Color/ColorConfig.h"
#include "CLOVise/PLM/Inbound/Print/PrintConfig.h"
#include "CLOVise/PLM/Outbound/PublishToPLM/CreateImageIntent.h"
#include "CLOVise/PLM/Outbound/PublishToPLM/BOM/AddNewBom.h"
#include "CLOVise/PLM/Outbound/PublishToPLM/Section.h"
#include "CLOVise/PLM/Outbound/PublishToPLM/BOM/CreateProductBOMHandler.h"

using namespace std;

namespace CLOVise
{
	CreateProduct* CreateProduct::_instance = NULL;

	CreateProduct* CreateProduct::GetInstance()
	{
		if (_instance == NULL) {
			_instance = new CreateProduct();
		}

		return _instance;
	}

	void CreateProduct::Destroy()
	{
		Logger::Debug("Create product Destroy() Start....");
		if (_instance)
		{
			Logger::Debug("Create product Destroy() 1....");
			//_instance->setParent(nullptr);
			//delete _instance;
			_instance = NULL;
			Logger::Debug("Create product Destroy() 2....");
		}
		Logger::Debug("Create product Destroy() End....");
	}

	CreateProduct::CreateProduct(QWidget* parent) : MVDialog(parent)
	{
		setupUi(this);
		Logger::Debug("Create product constructor() start....");/*
		if (!PublishToPLMData::GetInstance()->isModelExecuted)
			RESTAPI::SetProgressBarData(15, "Loading Create product", true);*/

		QString windowTitle = PLM_NAME + " PLM Create " + QString::fromStdString(Configuration::GetInstance()->GetLocalizedStyleClassName());
		this->setWindowTitle(windowTitle);

#ifdef __APPLE__
		this->setWindowFlags(Qt::Dialog | Qt::CustomizeWindowHint | Qt::WindowMinMaxButtonsHint | Qt::WindowCloseButtonHint);
#else
		this->setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint | Qt::CustomizeWindowHint);
		m_pTitleBar = new MVTitleBar(windowTitle, this);
		layout()->setMenuBar(m_pTitleBar);
#endif // !__APPLE__

		m_createProductTreeWidget_1 = nullptr;
		m_createProductTreeWidget_2 = nullptr;
		m_cancelButton = nullptr;
		m_publishButton = nullptr;
		m_SaveAndCloseButton = nullptr;
		ui_colorwayTable = nullptr;
		m_colorwayAddButton = nullptr;
		m_bomAddButton = nullptr;
		m_2DigiCodeActive = false;
		m_updateBomTab = false;
		m_multipartFilesParams = "";
		m_buttonSignalMapper = new QSignalMapper();
		m_createActionSignalMapper = new QSignalMapper();
		m_colorwayDeleteSignalMapper = new QSignalMapper();
		m_printActionSignalMapper = new QSignalMapper();
		m_createProductTreeWidget_1 = CVWidgetGenerator::CreatePublishTreeWidget("QTreeWidget { background-color: #262628; border: 1px solid #000;}""QTreeWidget::item { padding :5px; height: 20px; color: #FFFFFF; font-face: ArialMT; font-size: 10px; width: 90px; margin-left: 5px; margin-right: 5px; margin-top: 5px; margin-bottom: 5px; border: none;}""QTreeWidget::item:hover{background-color: #262628;}", true);
		m_createProductTreeWidget_1->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
		m_createProductTreeWidget_2 = CVWidgetGenerator::CreatePublishTreeWidget("QTreeWidget { background-color: #262628; border: 1px solid #000;}""QTreeWidget::item { padding :5px; height: 20px; color: #FFFFFF; font-face: ArialMT; font-size: 10px; width: 90px; margin-left: 5px; margin-right: 5px; margin-top: 5px; margin-bottom: 5px; border: none;}""QTreeWidget::item:hover{background-color: #262628;}", true);
		m_createProductTreeWidget_2->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

		m_cancelButton = CVWidgetGenerator::CreatePushButton("Back", BACK_HOVER_PATH, "Back", PUSH_BUTTON_STYLE, 30, true);
		m_publishButton = CVWidgetGenerator::CreatePushButton("Publish", PUBLISH_HOVER_ICON_PATH, "Publish", PUSH_BUTTON_STYLE, 30, true);
		m_colorwayAddButton = CVWidgetGenerator::CreatePushButton("New Colorway", ADD_HOVER_ICON_PATH, "New Colorway", PUSH_BUTTON_STYLE, 30, true);
		m_SaveAndCloseButton = CVWidgetGenerator::CreatePushButton("Save", SAVE_HOVER_ICON_PATH, "Save", PUSH_BUTTON_STYLE, 30, true);
		m_addImageIntentButton = CVWidgetGenerator::CreatePushButton("Add Image Intents", ADD_HOVER_ICON_PATH, "Add Image Intents", PUSH_BUTTON_STYLE, 30, true);
		m_totalCountLabel = CVWidgetGenerator::CreateLabel("Total count: 0 ", QString::fromStdString(BLANK), HEADER_STYLE, true);
		m_bomAddButton = CVWidgetGenerator::CreatePushButton("New Style Bom", ADD_HOVER_ICON_PATH, "New Style Bom", PUSH_BUTTON_STYLE, 30, true);
		m_totalCountLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

		QSpacerItem *horizontalSpacer = new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Fixed);
		m_colorwayRowcount = -1;
		PublishToPLMData::GetInstance()->m_isSaveClicked = false;
		m_addClicked = false;
		m_collectionId = "";

		QFrame *frame;
		QGridLayout *gridLayout;
		QSplitter *splitter;

		frame = new QFrame();
		frame->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
		gridLayout = new QGridLayout(frame);
		splitter = new QSplitter(frame);
		splitter->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
		splitter->setOrientation(Qt::Horizontal);
		splitter->insertWidget(0, m_createProductTreeWidget_1);
		splitter->insertWidget(1, m_createProductTreeWidget_2);
		gridLayout->setContentsMargins(QMargins(0, 0, 0, 0));
		gridLayout->addWidget(splitter, 0, 0, 1, 1);

		ui_treeWidgetLayout_2->addWidget(frame);
		ui_overviewTab->setLayout(ui_treeWidgetLayout_2);

		ui_tabWidget->setTabText(OVERVIEW_TAB, "Overview");
		ui_tabWidget->setTabText(COLORWAY_TAB, "Colorway");
		ui_tabWidget->setTabText(IMAGE_INTENT_TAB, "Image Intent");
		ui_tabWidget->setTabText(BOM_TAB, "BOM");

		ui_colorwayTable = new MVTableWidget();
		CVHoverDelegate* customHoverDelegate = new CVHoverDelegate(ui_colorwayTable);
		ui_colorwayTable->setItemDelegate(customHoverDelegate);
		CVWidgetGenerator::InitializeTableView(ui_colorwayTable);


		ui_colorwayTable->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
		ui_colorwayTable->setContextMenuPolicy(Qt::CustomContextMenu); ui_colorwayTable->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
		ui_colorwayTable->horizontalHeader()->setStyleSheet("QHeaderView { font-face: ArialMT; font-size: 10px;background-color:#2D2D2F; color: #FFFFFF; font-weight: bold;}" "QHeaderView::section:horizontal{border: 1px solid #000000;}" "QHeaderView::down-arrow{ image: url(:/CLOVise/PLM/Images/ui_spin_icon_minus_over.svg); width: 18px; height: 18px; color: #0000FF }" " QHeaderView::up-arrow{ image: url(:/CLOVise/PLM/Images/ui_spin_icon_plus_over.svg); width: 18px; height: 18px; color: #0000FF }");
		ui_colorwayTable->horizontalHeader()->setStretchLastSection(true);
		//ui_colorwayTable->verticalHeader()->setStyleSheet("QHeaderView { color: #FFFFFF;font-weight: bold; background-color:#2D2D2F;}" "QHeaderView::section:vertical{border: 1px solid #000000;}");

		//ui_colorwayTable->verticalHeader()->setStyleSheet("QHeaderView { font-face: ArialMT; font-size: 10px; color: #FFFFFF; min-width: 30px; }""QHeaderView::section:vertical{ background-color:#262628; border-right: 0.5px solid #262628; border-bottom: 0.5px solid #000000; border-top: 0.2px solid #000000; border-left: 0.5px solid #000000; min-width: 30px;}");
		ui_colorwayTable->setStyleSheet("QTableWidget{ background-color: #262628; border-right: 0px solid #000000; border-top: 0px solid #000000; border-left: 0px solid #000000; font-face: ArialMT; font-size: 12px; color: #FFFFFF; }"
			"QTableCornerButton::section{border: 1px solid #000000; background-color: #262628; }""QTableWidget::item{ border-bottom: 2px solid #000000; font-face: ArialMT; font-size: 12px; color: #FFFFFF; }"
			"QToolTip{ color: #46C8FF; background-color: #33414D; border: 1px #000000; }""QTableWidget::item:selected{ background-color: #33414D; color: #46C8FF; }""QScrollBar::add-page:vertical { background: #000000; }"
			"QScrollBar::sub-page:vertical {background: #000000;}""QScrollBar::add-page:horizontal { background: #000000; }""QScrollBar::sub-page:horizontal { background: #000000; }");
		ui_colorwayTable->horizontalHeader()->setMinimumHeight(COLORWAY_HORIZONTAL_HEADER_HEIGHT);
		horizontalLayout_2->addWidget(ui_colorwayTable);

		m_imageIntentTable = new MVTableWidget();
		m_imageIntentTable->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
		m_imageIntentTable->horizontalHeader()->setStyleSheet("QHeaderView { font-face: ArialMT; font-size: 10px;background-color:#2D2D2F; color: #FFFFFF; font-weight: bold;}" "QHeaderView::section:horizontal{border: 1px solid #000000;}" "QHeaderView::down-arrow{ image: url(:/CLOVise/PLM/Images/ui_spin_icon_minus_over.svg); width: 18px; height: 18px; color: #0000FF }" " QHeaderView::up-arrow{ image: url(:/CLOVise/PLM/Images/ui_spin_icon_plus_over.svg); width: 18px; height: 18px; color: #0000FF }");
		m_imageIntentTable->horizontalHeader()->setStretchLastSection(true);
		m_imageIntentTable->verticalHeader()->setStyleSheet("QHeaderView { color: #FFFFFF;font-weight: bold; background-color:#2D2D2F;}" "QHeaderView::section:vertical{border: 1px solid #000000;}");

		m_imageIntentTable->setStyleSheet("QTableWidget{ font-face: ArialMT; font-size: 12px; color: #FFFFFF; gridline-color: #000000; border: 0px solid #000000;}""QTableWidget::item{ font-face: ArialMT;gridline-color: #000000; font-size: 12px; color: #FFFFFF; border: 0px solid #000000; }""QToolTip{ color: #46C8FF; background-color: #33414D; border: 0px #000000; }""QTableWidget::item:selected{ background-color: #33414D; color: #46C8FF; }""QScrollBar::add-page:vertical { background: #000; }""QScrollBar::sub-page:vertical {background: #000;}""QScrollBar::add-page:horizontal { background: #000; }""QScrollBar::sub-page:horizontal {background: #000;}""QTableWidget::indicator:unchecked{ background-color:white; }QTableWidget::indicator:checked{ color:blue; }");	//#46C8FF"QTableWidget::focus {border-color: red;}"
		CVWidgetGenerator::InitializeTableView(m_imageIntentTable);
		m_imageIntentTable->hide();
		horizontalLayout_5->addWidget(m_imageIntentTable);

		QFont font;
		font.setBold(false);
		font.setFamily("ArialMT");
		ui_overviewTab->setFont(font);
		ui_tabWidget->setFont(font);
		ui_tabWidget->setCurrentIndex(OVERVIEW_TAB);
		ui_tabWidget->setStyleSheet(" QTabWidget::pane { border: none; color: #FFFFFF; font-size: 10px; background-color: #262628; }""QTabBar::tab { width: 100px; padding: 2px; }""QTabBar::tab:selected { border: none; color: #FFFFFF; background-color: \"" + DEFAULT_TAB_BG_COLOR + "\"; }""QTabBar::tab:!selected { color:#FFFFFF; background-color:\"" + SELECTED_TAB_BG_COLOR + "\"; }");

		//ui_treeWidgetLayout->addWidget(ui_tabWidget);

		ui_buttonsLayout->insertWidget(0, m_cancelButton);
		ui_buttonsLayout->insertSpacerItem(1, horizontalSpacer);
		ui_buttonsLayout->insertWidget(2, m_SaveAndCloseButton);
		ui_buttonsLayout->insertSpacerItem(3, horizontalSpacer);
		ui_buttonsLayout->insertWidget(4, m_publishButton);


		horizontalLayout->insertWidget(0, m_colorwayAddButton);
		horizontalLayout->insertSpacerItem(1, horizontalSpacer);

		horizontalLayout_4->insertWidget(0, m_addImageIntentButton);
		horizontalLayout_4->insertSpacerItem(1, horizontalSpacer);
		horizontalLayout_4->insertWidget(2, m_totalCountLabel);


		ui_addNewBomButtonLayout->insertWidget(0, m_bomAddButton);
		QLabel *label = new QLabel();
		label->setText("BOM Name: ");
		QLabel *label1 = new QLabel();
		label1->setText("Template: ");
		m_bomName = new QLabel();
		m_bomTemplateName = new QLabel();
		ui_addNewBomButtonLayout->insertSpacerItem(1, horizontalSpacer);
		ui_addNewBomButtonLayout->insertWidget(2, label);
		ui_addNewBomButtonLayout->insertWidget(3, m_bomName);
		ui_addNewBomButtonLayout->insertSpacerItem(4, horizontalSpacer);
		ui_addNewBomButtonLayout->insertWidget(5, label1);
		ui_addNewBomButtonLayout->insertWidget(6, m_bomTemplateName);
		ui_addNewBomButtonLayout->insertSpacerItem(7, horizontalSpacer);


		m_ImageIntentList = new QListWidget();
		m_ImageIntentList->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
		QString listStyleSheet = "QListWidget{ border-bottom: 1px inset #262626; font-size: 10px; font-face: ArialMT; border-left: 1px inset #0D0D0D; border-top: 1px inset #0D0D0D; border-right: 1px inset #262626; background-color: #222224; } QListWidget::item{ background-color: #222224; spacing: 10px; margin: 10px;height: 200px;  border-bottom: 2px solid black;}";//height: 150px;, max-height:100px;
		//m_ImageIntentList->setStyleSheet(listStyleSheet);
		m_ImageIntentList->setSortingEnabled(true);
		horizontalLayout_5->addWidget(m_ImageIntentList);
		m_ImageIntentList->setSpacing(10);
		m_ImageIntentList->setItemAlignment(Qt::AlignCenter);
		connectSignalSlots(true);
		m_ImageIntentList->setMouseTracking(false);
		//DrawCriteriaWidget(true);//impleted for destroy 
		json columnNamejson = json::array();
		string columnsNames = DirectoryUtil::GetPLMPluginDirectory() + "ColorwayTableColumns.json";//Reading Columns from json
		json jsonvalue = Helper::ReadJSONFile(columnsNames);
		string colorwayTableColumns = Helper::GetJSONValue<string>(jsonvalue, "colorwayTableFieldList", false);
		columnNamejson = json::parse(colorwayTableColumns);

		for (int index = 0; index < columnNamejson.size(); index++)
		{
			string columnname = Helper::GetJSONParsedValue<int>(columnNamejson, index, false);
			m_ColorwayTableColumnNames.insert(index, QString::fromStdString(columnname));
		}
		ui_colorwayTable->setColumnCount(m_ColorwayTableColumnNames.size());
		ui_colorwayTable->setHorizontalHeaderLabels(m_ColorwayTableColumnNames);
		//ui_colorwayTable->hideColumn(SELECT_COLUMN);
		//addCreateProductDetailsWidgetData();

		m_colorwayImageLabelsMap = UIHelper::GetImageLabels("Colorway");
		Configuration::GetInstance()->SetColorwayImageLabels(m_colorwayImageLabelsMap);


		m_styleImageLabelsMap = UIHelper::GetImageLabels("Style");
		Configuration::GetInstance()->SetStyleImageLabels(m_styleImageLabelsMap);

		m_clientSpecificJson = Configuration::GetInstance()->GetClientSpecificJson();
		Logger::Debug("Create product constructor() m_clientSpecificJson...." + to_string(m_clientSpecificJson));
		string attTypesStr = "";
		json attTypesArray = Helper::GetJSONParsedValue<string>(m_clientSpecificJson, "colorwayTableAttributes", false);
		string attName = "";
		ui_colorwayTable->hideColumn(UNI_2_DIGIT_CODE_COLUMN);
		for (int i = 0; i < attTypesArray.size(); i++)
		{
			attName = Helper::GetJSONValue<int>(attTypesArray, i, true);
			Logger::Debug("Create product constructor() attName...." + attName);
			m_digiCodeNamesMap = UIHelper::GetEnumValues(attName, m_digiCodeValue, nullptr);
			ui_colorwayTable->showColumn(UNI_2_DIGIT_CODE_COLUMN);
			m_2DigiCodeActive = true;
		}

		Logger::Debug("Create product constructor() end....");
		RESTAPI::SetProgressBarData(0, "", false);
	}

	CreateProduct::~CreateProduct()
	{
		Logger::Debug("Create product Destructor() Start....");
		connectSignalSlots(false);
		UIHelper::DeletePointer(m_createProductTreeWidget_1);
		UIHelper::DeletePointer(m_createProductTreeWidget_2);
		UIHelper::DeletePointer(m_cancelButton);
		UIHelper::DeletePointer(m_publishButton);
		UIHelper::DeletePointer(m_SaveAndCloseButton);
		UIHelper::DeletePointer(ui_colorwayTable);
		UIHelper::DeletePointer(m_colorwayAddButton);
		Logger::Debug("Create product Destructor() end....");
	}

	/*
	* Description - ResetDateEditWidget() method used to Reset Date Widget.
	* Parameter -
	* Exception -
	* Return -
	*/
	//void CreateProduct::onResetDateEditWidget()
	//{
	//	//UIHelper::ResetDate(m_createProductTreeWidget);
	//}

	/*
	* Description - connectSignalSlots() method used to connect and disconnect signal and slots.
	* Parameter -  bool.
	* Exception -
	* Return -
	*/
	void CreateProduct::connectSignalSlots(bool _b)
	{
		if (_b)
		{
			QObject::connect(m_publishButton, SIGNAL(clicked()), this, SLOT(onPublishToPLMClicked()));
			QObject::connect(m_cancelButton, SIGNAL(clicked()), this, SLOT(onBackButtonClicked()));
			QObject::connect(m_colorwayAddButton, SIGNAL(clicked()), this, SLOT(onAddColorwaysClicked()));
			QObject::connect(m_SaveAndCloseButton, SIGNAL(clicked()), this, SLOT(onSaveAndCloseClicked()));
			QObject::connect(m_addImageIntentButton, SIGNAL(clicked()), this, SLOT(onAddImageIntentClicked()));
			QObject::connect(ui_tabWidget, SIGNAL(currentChanged(int)), this, SLOT(onTabClicked(int)));
			QObject::connect(m_colorwayDeleteSignalMapper, SIGNAL(mapped(int)), this, SLOT(OnColorwaysTableDeleteButtonClicked(int)));
			QObject::connect(m_buttonSignalMapper, SIGNAL(mapped(int)), this, SLOT(OnClickAddColorButton(int)));
			QObject::connect(m_createActionSignalMapper, SIGNAL(mapped(int)), this, SLOT(OnCreateColorSpecClicked(int)));
			QObject::connect(m_printActionSignalMapper, SIGNAL(mapped(int)), this, SLOT(OnSearchPrintClicked(int)));
			QObject::connect(ui_colorwayTable->horizontalHeader(), SIGNAL(sectionClicked(int)), this, SLOT(onHorizontalHeaderClicked(int)));
			QObject::connect(m_bomAddButton, SIGNAL(clicked()), this, SLOT(onAddNewBomClicked()));



		}
		else
		{
			QObject::disconnect(m_publishButton, SIGNAL(clicked()), this, SLOT(onPublishToPLMClicked()));
			QObject::disconnect(m_cancelButton, SIGNAL(clicked()), this, SLOT(onBackButtonClicked()));
			QObject::disconnect(m_colorwayAddButton, SIGNAL(clicked()), this, SLOT(onAddColorwaysClicked()));
			QObject::disconnect(m_SaveAndCloseButton, SIGNAL(clicked()), this, SLOT(onSaveAndCloseClicked()));
			QObject::disconnect(m_colorwayDeleteSignalMapper, SIGNAL(mapped(int)), this, SLOT(OnColorwaysTableDeleteButtonClicked(int)));
			QObject::disconnect(m_addImageIntentButton, SIGNAL(clicked()), this, SLOT(onAddImageIntentClicked()));
			QObject::disconnect(ui_tabWidget, SIGNAL(currentChanged(int)), this, SLOT(onTabClicked(int)));
			QObject::disconnect(m_buttonSignalMapper, SIGNAL(mapped(int)), this, SLOT(OnClickAddColorButton(int)));
			QObject::disconnect(m_createActionSignalMapper, SIGNAL(mapped(int)), this, SLOT(OnCreateColorSpecClicked(int)));
			QObject::disconnect(m_printActionSignalMapper, SIGNAL(mapped(int)), this, SLOT(OnSearchPrintClicked(int)));
			QObject::disconnect(ui_colorwayTable->horizontalHeader(), SIGNAL(sectionClicked(int)), this, SLOT(onHorizontalHeaderClicked(int)));
			QObject::disconnect(m_bomAddButton, SIGNAL(clicked()), this, SLOT(onAddNewBomClicked()));
		}
	}

	/*
	* Description - cancelWindowClicked() method is a slot for cancel button click and close the publish to plm.
	* Parameter -
	* Exception -
	* Return -
	*/
	void CreateProduct::onBackButtonClicked()
	{
		Logger::Debug("CreateProduct onBackButtonClicked() Start....");
		ui_tabWidget->setStyleSheet(" QTabWidget::pane { border: none; color: #FFFFFF; font-size: 10px; background-color: #262628; }""QTabBar::tab { width: 100px; padding: 2px; }""QTabBar::tab:selected { border: none; color: #FFFFFF; background-color: \"" + DEFAULT_TAB_BG_COLOR + "\"; }""QTabBar::tab:!selected { color:#FFFFFF; background-color:\"" + SELECTED_TAB_BG_COLOR + "\"; }");
		if (!PublishToPLMData::GetInstance()->m_isSaveClicked)
		{
			ClearAllFields(m_createProductTreeWidget_1);
			ClearAllFields(m_createProductTreeWidget_2);
			ui_colorwayTable->clear();
			ui_colorwayTable->clearContents();
			ui_colorwayTable->setRowCount(0);
			m_colorwayRowcount = 0;
			ui_colorwayTable->hide();
			CreateImageIntent::GetInstance()->ClearAllFields();
			m_ImageIntentList->clear();
			m_colorSpecList.clear();
			ui_tabWidget->setCurrentIndex(OVERVIEW_TAB);
			m_totalCountLabel->setText("Total count: 0");


			/*	QLayoutItem *child;
				while ((child = ui_sectionLayout->takeAt(0)) != 0) {
					Logger::Debug("CreateProduct -> onBackButtonClicked -> 6");
					delete child;
				}*/
			m_CloAndPLMColorwayMap.clear();
			ClearBOMData();
		}


		//SetTotalImageCount();

		Configuration::GetInstance()->SetIsPrintSearchClicked(false);
		this->close();
		CLOVise::CLOViseSuite::GetInstance()->setModal(true);
		CLOViseSuite::GetInstance()->show();
		Logger::Debug("CreateProduct onBackButtonClicked() End....");
	}

	/*
	* Description - Add3DModelDetailsWidgetData() method used to Add 3D Model Details Widget Data.
	* Parameter -
	* Exception -
	* Return -
	*/
	void CreateProduct::addCreateProductDetailsWidgetData()
	{
		Logger::Debug("Create product add3DModelDetailsWidgetData() start....");
		//Configuration::GetInstance()->SetModifySupportedAttsList(PublishToPLMData::GetInstance()->GetDocumentConfigJSON());
		//PublishToPLMData::GetInstance()->GetMaxUploadFileSize();
		json fieldsJsonArray = json::array();
		json mergedJsonArray = json::array();

		fieldsJsonArray = PublishToPLMData::GetInstance()->GetDocumentFieldsJSON();
		Logger::Debug("Create product add3DModelDetailsWidgetData() fieldsJsonArray:: " + to_string(fieldsJsonArray));
		//QStringList emptylist;
		//json feildsJson = json::object();

		//string attRestApiExposed = "";
		//UTILITY_API->DisplayMessageBox(to_string(fieldsJsonArray));
		json jsonvalueArray = json::array();
		string columnsNames = DirectoryUtil::GetPLMPluginDirectory() + "CentricCreateStyleHierarchy.json";//Reading Columns from json
		jsonvalueArray = Helper::ReadJSONFile(columnsNames);
		//UTILITY_API->DisplayMessageBox("jsonvalue:: " + to_string(jsonvalue));
		//createFieldsJson(to_string(fieldsJsonArray), jsonvalue);

		int sizeOfResponseJSON = fieldsJsonArray.size();
		int sizeOfDefaultJSON = jsonvalueArray.size();

		for (int i = 0; i < sizeOfResponseJSON + sizeOfDefaultJSON; i++)
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
				json fieldsJson = Helper::GetJSONParsedValue<int>(fieldsJsonArray, i - sizeOfDefaultJSON, false);
				string internalName = Helper::GetJSONValue<string>(fieldsJson, "rest_api_name", true);
				m_plmAttributesList.push_back(QString::fromStdString(internalName));
				mergedJsonArray[i] = fieldsJsonArray[i - sizeOfDefaultJSON];
			}
		}
		//UTILITY_API->DisplayMessageBox("mergedJsonArray:: " + to_string(mergedJsonArray));

		//UTILITY_API->DisplayMessageBox("m_creatProductFieldsJson:: " + to_string(m_creatProductFieldsJson));
		////QStringList emptylist;
		////json feildsJson = json::object();
		//json attributesJson = json::object();
		//string typeName = "";
		//for (int feildsCount = 0; feildsCount < m_creatProductFieldsJson.size(); feildsCount++)
		//{
		//	feildsJson = Helper::GetJSONParsedValue<int>(m_creatProductFieldsJson, feildsCount, false);
		//	typeName = Helper::GetJSONValue<string>(feildsJson, TYPENAME_JSON_KEY, true);
		//	attributesJson = Helper::GetJSONParsedValue<string>(feildsJson, FILTER_ATTRIBUTES_KEY, false);
		//	UTILITY_API->DisplayMessageBox("attributesJson:: " + to_string(attributesJson));
		drawCriteriaWidget(mergedJsonArray, m_createProductTreeWidget_1, m_createProductTreeWidget_2, PublishToPLMData::GetInstance()->GetActive3DModelMetaData());
		//	break;
		//}

		m_createProductTreeWidget_1->setColumnCount(2);
		m_createProductTreeWidget_1->setHeaderHidden(true);
		m_createProductTreeWidget_1->setWordWrap(true);
		m_createProductTreeWidget_1->setDropIndicatorShown(false);
		m_createProductTreeWidget_1->setRootIsDecorated(false);
		m_createProductTreeWidget_1->setSelectionMode(QAbstractItemView::NoSelection);
		m_createProductTreeWidget_1->setStyleSheet("QTreeWidget { background-color: #262628; border: 1px solid #000; padding-left: 20px; min-width: 400px; outline: 0;}""QTreeWidget::item {height: 20px; width: 200px; margin-right: 20px; margin-top: 5px; margin-bottom: 5px; border: none; }""QTreeWidget::item:hover{ background-color: #262628; }""QTreeView{outline: 0;}");
		m_createProductTreeWidget_2->setColumnCount(2);
		m_createProductTreeWidget_2->setHeaderHidden(true);
		m_createProductTreeWidget_2->setWordWrap(true);
		m_createProductTreeWidget_2->setDropIndicatorShown(false);
		m_createProductTreeWidget_2->setRootIsDecorated(false);
		m_createProductTreeWidget_2->setSelectionMode(QAbstractItemView::NoSelection);
		m_createProductTreeWidget_2->setStyleSheet("QTreeWidget { background-color: #262628; border: 1px solid #000; padding-left: 20px; min-width: 400px; outline: 0;}""QTreeWidget::item {height: 20px; width: 200px; margin-right: 20px; margin-top: 5px; margin-bottom: 5px; border: none; }""QTreeWidget::item:hover{ background-color: #262628; }""QTreeView{outline: 0;}");


		//for (int feildsCount = 0; feildsCount < jsonvalue.size(); feildsCount++)
		//{
		//	feildsJson = Helper::GetJSONParsedValue<int>(jsonvalue, feildsCount, false);
		//	attRestApiExposed = Helper::GetJSONValue<string>(feildsJson, ATTRIBUTE_REST_API_EXPOSED, true);
		//	if (attRestApiExposed == "false")
		//		continue;
		//	drawWidget(feildsJson, m_createProductTreeWidget);

		//	//drawCriteriaWidget(attributesJson, m_createProductTreeWidget, PublishToPLMData::GetInstance()->GetActive3DModelMetaData());
		//	//break;
		//}
		//for (int feildsCount = 0; feildsCount < fieldsJsonArray.size(); feildsCount++)
		//{
		//	feildsJson = Helper::GetJSONParsedValue<int>(fieldsJsonArray, feildsCount, false);
		//	attRestApiExposed = Helper::GetJSONValue<string>(feildsJson, ATTRIBUTE_REST_API_EXPOSED, true);
		//	if (attRestApiExposed == "false")
		//		continue;
		//	drawWidget(feildsJson, m_createProductTreeWidget);

		//	//drawCriteriaWidget(attributesJson, m_createProductTreeWidget, PublishToPLMData::GetInstance()->GetActive3DModelMetaData());
		//	//break;
		//}
		//m_createProductTreeWidget->setStyleSheet("QTreeWidget { background-color: #262628; border: 1px solid #000; padding-left: 20px; min-width: 400px; }""QTreeWidget::item {font-size: 10px; font-face: ArialMT; height: 20px; width: 200px; margin-right: 20px; margin-top: 5px; margin-bottom: 5px; border: none; }""QTreeWidget::item:hover{ background-color: #262628; } QTreeView{outline: 0;}");

		///*	if (!PublishToPLMData::GetInstance()->GetDateFlag())
		//		m_dateResetButton->hide();
		//	else
		//		m_dateResetButton->show();*/

		Logger::Debug("Create product add3DModelDetailsWidgetData() end....");
	}

	/*
	* Description - DrawCriteriaWidget() method used to create fields in UI.
	* Parameter -  json, QTreeWidget, json.
	* Exception -
	* Return -
	*/
	void CreateProduct::drawCriteriaWidget(json _attributesJsonArray, QTreeWidget* _documentPushToPLMTree_1, QTreeWidget* _documentPushToPLMTree_2, json _metadataJson)
	{
		Logger::Debug("Create product drawCriteriaWidget() start....");
		//UTILITY_API->DisplayMessageBox("size/2:: " + to_string(_attributesJsonArray.size() / DIVISION_FACTOR));
		//UTILITY_API->DisplayMessageBox("ceil(size/2):: " + to_string(ceil(_attributesJsonArray.size() / DIVISION_FACTOR)));

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
	}

	/*
	* Description - DrawCriteriaWidget() method used to create input fields.
	* Parameter -  json, QTreeWidget, string, string, bool.
	* Exception -
	* Return -
	*/
	void CreateProduct::drawWidget(json _feildsJson, QTreeWidget* m_TreeWidget, int _start, int _end)
	{
		Logger::Debug("Create product drawCriteriaWidget() start....");
		/*string attName = Helper::GetJSONValue<string>(_attJson, ATTRIBUTE_NAME_KEY, true);
		string attType = Helper::GetJSONValue<string>(_attJson, ATTRIBUTE_TYPE_KEY, true);
		bool attRequired = Helper::IsValueTrue((Helper::GetJSONValue<string>(_attJson, ATTRIBUTE_REQUIRED_KEY, true)));
		if (!FormatHelper::HasContent(_attValue))
			_attValue = BLANK;*/
		try
		{
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
			for (int i = _start; i < _end; i++)
			{
				attJson = Helper::GetJSONParsedValue<int>(_feildsJson, i, false);
				bool isEditable = false;
				bool required = true;
				responseJson.clear();

				attRestApiExposed = Helper::GetJSONValue<string>(attJson, ATTRIBUTE_REST_API_EXPOSED, true);
				if (attRestApiExposed == "false")
					continue;

				attributeType = Helper::GetJSONValue<string>(attJson, ATTRIBUTE_TYPE, true);
				Logger::Debug("Create product drawCriteriaWidget() attributeType...." + attributeType);
				internalName = Helper::GetJSONValue<string>(attJson, ATTRIBUTE_INTERNAL_NAME, true);
				Logger::Debug("Create product drawCriteriaWidget() internalName...." + internalName);

				if (!m_localAttributesList.contains(QString::fromStdString(internalName)))
				{
					m_localAttributesList.append(QString::fromStdString(internalName));
				}
				else
					continue;

				attributeName = Helper::GetJSONValue<string>(attJson, ATTRIBUTE_NAME, true);
				attributeDisplayName = Helper::GetJSONValue<string>(attJson, ATTRIBUTE_DISPLAY_NAME, true);
				Logger::Debug("Create product drawCriteriaWidget() attributeDisplayName...." + attributeDisplayName);

				restrictEdit = Helper::GetJSONValue<string>(attJson, ATTRIBUTE_EDITABLE, true);
				Logger::Debug("Create product drawCriteriaWidget() attributeEditable...." + restrictEdit);

				attributeRequired = Helper::GetJSONValue<string>(attJson, ATTRIBUTE_REQUIRED, true);
				attributeInitiaValue = QString::fromStdString(Helper::GetJSONValue<string>(attJson, ATTRIBUTE_INITIAL_VALUE, true));
				Logger::Debug("Create product drawCriteriaWidget() attributeDefaultValue...." + attributeInitiaValue.toStdString());


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
					Logger::Debug("Create product drawCriteriaWidget() isEditable...." + to_string(isEditable));
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
					Logger::Debug("Create product drawWidget() ref  6....");

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
						if (attributeName == "Season")
						{
							responseJson = Helper::makeRestcallGet(RESTAPI::SEASON_SEARCH_API, "?limit=" + Configuration::GetInstance()->GetMaximumLimitForRefAttValue(), "", "Loading season details..");
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
							if (attributeName == "Style Type")
							{
								string tdsmapString = Helper::GetJSONValue<string>(attJson, "tds_map", false);
								Logger::Debug("PublishToPLMData -> SetDocumentConfigJSON tdsmapString: " + tdsmapString);
								json tdsmapJson = json::parse(tdsmapString);
								Logger::Debug("PublishToPLMData -> SetDocumentConfigJSON tdsmapJson: " + to_string(tdsmapJson));
								string apparelBomFlag = Helper::GetJSONValue<string>(tdsmapJson, "ApparelBOM", true);
								Logger::Debug("PublishToPLMData -> SetDocumentConfigJSON apparelBomFlag: " + apparelBomFlag);
								m_styleTypeBomPermissionMap.insert(make_pair(attName, apparelBomFlag));
							}
							/* "tds_map": {
			"ReviewStyle": true,
			"ApparelBOM": true,
			"ImageDataSheet": true,
			"CareAndComposition": true
		},*/
							valueList.append(QString::fromStdString(attName));
							m_seasonNameIdMap.insert(make_pair(attName, attId));
							//m_styleTypeNameIdMap.insert(make_pair(attName, attId));
							comboBox->setProperty(attName.c_str(), QString::fromStdString(attId));
							string allowCreateColor = Helper::GetJSONValue<string>(attJson, ALLOW_CREATE_COLOR, true);
							comboBox->setProperty((attName + ALLOW_CREATE_COLOR).c_str(), QString::fromStdString(allowCreateColor));
							string defaultImage = Helper::GetJSONValue<string>(attJson, REFER_DEFAULT_IMAGE_ON_COLOR, true);
							comboBox->setProperty((attName + REFER_DEFAULT_IMAGE_ON_COLOR).c_str(), QString::fromStdString(defaultImage));
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
					Logger::Debug("Create product drawWidget() ref  End....");

				}
				else if (attributeType == STRING_ATT_TYPE_KEY || attributeType == TEXT_ATT_TYPE_KEY)
				{
					Logger::Debug("Create product drawWidget() string  Start....");
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
					Logger::Debug("Create product drawWidget() string  End....");
				}
			}
		}
		catch (string msg)
		{
			Logger::Error("Create product drawWidget() Exception - " + msg);
		}
		catch (exception & e)
		{
			Logger::Error("Create product drawWidget() Exception - " + string(e.what()));
		}
		catch (const char* msg)
		{
			wstring wstr(msg, msg + strlen(msg));
			Logger::Error("Create product drawWidget() Exception - " + string(msg));
		}
		Logger::Debug("Create product drawWidget() end....");
	}

	/*
	* Description - PublishToPLMClicked() method is a slot for publish 3D model to plm click.
	* Parameter -
	* Exception - exception, Char *
	* Return -
	*/
	void CreateProduct::onPublishToPLMClicked()
	{
		Logger::Debug("Create product onPublishToPLMClicked() start....");
		QDir dir;
		string response;
		try
		{
			Configuration::GetInstance()->SetIsPrintSearchClicked(false);
			if (ValidateColorwayNameField() && CreateProductBOMHandler::GetInstance()->ValidateBomFields())
			{
				this->hide();
				collectCreateProductFieldsData();
				UIHelper::ValidateRquired3DModelData(m_createProductTreeWidget_1);
				UIHelper::ValidateRquired3DModelData(m_createProductTreeWidget_2);

				UTILITY_API->CreateProgressBar();
				RESTAPI::SetProgressBarData(20, "Creating " + Configuration::GetInstance()->GetLocalizedStyleClassName() + "..", true);
				UTILITY_API->SetProgress("Publishing to PLM", (qrand() % 101));
				vector<pair<string, string>> headerNameAndValueList;
				headerNameAndValueList.push_back(make_pair("content-Type", "application/json"));
				headerNameAndValueList.push_back(make_pair("Accept", "application/json"));
				headerNameAndValueList.push_back(make_pair("Cookie", Configuration::GetInstance()->GetBearerToken()));
				//	UTILITY_API->DisplayMessageBox(m_collectionId);
				//  UTILITY_API->DisplayMessageBox(to_string(m_ProductMetaData));
				string productMetaData = to_string(m_ProductMetaData);
				Logger::RestAPIDebug("CreateProduct:: onPublishToPLMClicked() productMetaData:: " + productMetaData);
				response = REST_API->CallRESTPost(Configuration::GetInstance()->GetPLMServerURL() + RESTAPI::CREATE_STYLE_API + "/" + m_collectionId + "/hierarchy", &productMetaData, headerNameAndValueList, "Loading");
				///string response = RESTAPI::PostRestCall(m_ProductMetaData, Configuration::GetInstance()->GetPLMServerURL() + RESTAPI::CREATE_STYLE_COPY_API, "content-type: application/json; boundary=----WebKitFormBoundary7MA4YWxkTrZu0gW");

				Logger::RestAPIDebug("CreateProduct:: onPublishToPLMClicked() response:: " + response);
				//UTILITY_API->DisplayMessageBox(response);

				if (!FormatHelper::HasContent(response))
				{
					RESTAPI::SetProgressBarData(0, "", false);
					throw "Unable to publish to PLM. Please try again or Contact your System Administrator.";
				}

				if (FormatHelper::HasError(response))
				{
					RESTAPI::SetProgressBarData(0, "", false);
					//Logger::Debug("PublishToPLMData -> onPublishToPLMClicked 1");
					throw runtime_error(response);
				}
				/*if (response.find("Created") == string::npos)
				{
					string error = RESTAPI::CheckForErrorMsg(response);
					error = "Failed. Publish to PLM. \n\t" + error;
					if (FormatHelper::HasContent(error))
					{
						throw std::logic_error(error);
					}
					else
					{
						RESTAPI::SetProgressBarData(0, "", false);
						UTILITY_API->DisplayMessageBox("Failed. Publish to PLM.");
						this->show();
					}
				}*/
				else
				{
					//Logger::Debug("PublishToPLMData -> onPublishToPLMClicked 1");
					json detailJson = Helper::GetJsonFromResponse(response, "{");
					//Logger::Debug("PublishToPLMData -> onPublishToPLMClicked 2");
					string productId = Helper::GetJSONValue<string>(detailJson, ATTRIBUTE_ID, true);
					//Logger::Debug("PublishToPLMData -> onPublishToPLMClicked 3");
					string revisionId = uploadDocument(productId);
					ExtractColorwayDetails(productId);
					exportZPRJ(revisionId);
					uploadGLBFile(productId);
					//UploadStyleThumbnail(productId);
					//exportTurntableImages();
					uploadColorwayImages();
					LinkImagesToColorways(productId);
					if (CreateProductBOMHandler::GetInstance()->IsBomCreated())
							CreateProductBOMHandler::GetInstance()->CreateBom(productId, AddNewBom::GetInstance()->m_BOMMetaData, m_CloAndPLMColorwayMap);
					//Logger::Debug("Create product onPublishToPLMClicked() 1....");
					UTILITY_API->NewProject();
					//Logger::Debug("Create product onPublishToPLMClicked() 2....");

					CVDisplayMessageBox * DownloadDialogObject = new CVDisplayMessageBox(this);
					//Logger::Debug("Create product onPublishToPLMClicked() 3....");
					DownloadDialogObject->DisplyMessage("Successfully published to PLM.");
					//Logger::Debug("Create product onPublishToPLMClicked() 4....");
					DownloadDialogObject->setModal(true);
					DownloadDialogObject->show();
					Helper::RemoveDirectory(QString::fromStdString(Configuration::GetInstance()->TURNTABLE_IMAGES_WITH_AVATAR_TEMP_DIRECTORY));
					Helper::RemoveDirectory(QString::fromStdString(Configuration::GetInstance()->TURNTABLE_IMAGES_WITHOUT_AVATAR_TEMP_DIRECTORY));
					dir.mkpath(QString::fromStdString(Configuration::GetInstance()->TURNTABLE_IMAGES_WITH_AVATAR_TEMP_DIRECTORY));
					dir.mkpath(QString::fromStdString(Configuration::GetInstance()->TURNTABLE_IMAGES_WITHOUT_AVATAR_TEMP_DIRECTORY));
					//Logger::Debug("Create product onPublishToPLMClicked() 5....");
					ClearAllFields(m_createProductTreeWidget_1);
					ClearAllFields(m_createProductTreeWidget_2);
					//Logger::Debug("Create product onPublishToPLMClicked() 6....");
					ui_colorwayTable->clear();
					ui_colorwayTable->clearContents();
					ui_colorwayTable->setRowCount(0);
					//Logger::Debug("Create product onPublishToPLMClicked() 7....");
					m_colorwayRowcount = 0;
					ui_colorwayTable->hide();
					CreateImageIntent::GetInstance()->ClearAllFields();
					//Logger::Debug("Create product onPublishToPLMClicked() 8....");
					m_ImageIntentList->clear();
					m_colorSpecList.clear();
					m_CloAndPLMColorwayMap.clear();
					ClearBOMData();
					PublishToPLMData::GetInstance()->m_isSaveClicked = false;
					ui_tabWidget->setCurrentIndex(OVERVIEW_TAB);
					m_totalCountLabel->setText("Total count: 0");
					RESTAPI::SetProgressBarData(0, "", false);
					//Logger::Debug("Create product onPublishToPLMClicked() 9....");
					this->hide();
					this->close();
					//Logger::Debug("Create product onPublishToPLMClicked() 10....");

				}
			}
		}
		catch (exception& e)
		{
			RESTAPI::SetProgressBarData(0, "", false);
			Logger::Error("Create product-> Create product Exception - " + string(e.what()));
			UTILITY_API->DisplayMessageBox(e.what());
			//Helper::RemoveDirectory(QString::fromStdString(Configuration::GetInstance()->TURNTABLE_IMAGES_TEMP_DIRECTORY));
			//dir.mkpath(QString::fromStdString(Configuration::GetInstance()->TURNTABLE_IMAGES_TEMP_DIRECTORY));

			this->show();
		}
		catch (const char* msg)
		{
			Logger::Error("Create product-> Create product Exception - " + string(msg));
			wstring wstr(msg, msg + strlen(msg));
			RESTAPI::SetProgressBarData(0, "", false);
			UTILITY_API->DisplayMessageBoxW(wstr);
			//Helper::RemoveDirectory(QString::fromStdString(Configuration::GetInstance()->TURNTABLE_IMAGES_TEMP_DIRECTORY));
			//dir.mkpath(QString::fromStdString(Configuration::GetInstance()->TURNTABLE_IMAGES_TEMP_DIRECTORY));

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

		Logger::Debug("Create product onPublishToPLMClicked() end....");
	}

	/*
	* Description - CollectPublishToPLMFieldsData() method used to collect publish to plm fields data.
	* Parameter -
	* Exception -
	* Return -
	*/
	void CreateProduct::collectPublishToPLMFieldsData()
	{
		Logger::Debug("Create product collectPublishToPLMFieldsData() start....");
		//Collects all Product Section related data into map
	/*	collectCreateProductFieldsData();
		collectProductFieldsData();
		m_parameterJson[PRODUCT_DETAIL_KEY] = m_productDetailsJson;

		string metaData = UTILITY_API->GetMetaDataForCurrentGarment();
		json metadataJSON = json::parse(metaData);
		m_parameterJson[DOCUMENTID_KEY] = Helper::GetJSONValue<string>(metadataJSON, OBJECT_ID, true);
		if (PublishToPLMData::GetInstance()->GetIsProductOverridden())
		{
			m_parameterJson[DOCUMENTID_KEY] = "";
		}*/
		m_parameterJson[DOCUMENT_DETAIL_KEY] = m_documentDetailsJson;
		Logger::Debug("Create product collectPublishToPLMFieldsData() end....");
	}

	/*
	* Description - CollectProductFieldsData() method used to collect product fields data.
	* Parameter -
	* Exception -
	* Return -
	*/
	void CreateProduct::collectProductFieldsData()
	{
		Logger::Debug("Create product collectProductFieldsData() start....");
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
		Logger::Debug("Create product collectProductFieldsData() end....");
	}

	/*
	* Description - Collect3DModelFieldsData() method used to collect 3d model fields data.
	* Parameter -
	* Exception -
	* Return -
	*/
	void CreateProduct::collectCreateProductFieldsData()
	{
		Logger::Debug("Create product collectCreateProductFieldsData() start....");
		m_ProductMetaData = collectCriteriaFields(m_createProductTreeWidget_1, m_createProductTreeWidget_2);

		//m_documentDetailsJson[ATTRIBUTES_KEY] = m_ProductMetaData;

		/*json thumbNailDetails = json::object();
		json _3DModelDetails = json::object();
		thumbNailDetails[THUMBNAIL_NAME_KEY] = m_3DModelThumbnailName;
		_3DModelDetails[THREE_D_MODEL_NAME_KEY] = m_3DModelFileName;
		_3DModelDetails[THREE_D_MODEL_TYPE_KEY] = "zprj";
		m_documentDetailsJson[THUMBNAIL_DETAIL_KEY] = thumbNailDetails;

		m_documentDetailsJson[THREE_D_MODEL_DETAIL_KEY] = _3DModelDetails;
		m_documentDetailsJson[GLB_EXPORT_KEY] = false;*/

		Logger::Debug("Create product collectCreateProductFieldsData() end....");
	}

	/*
	* Description - PreparePublishRequestParameter() method used to prepare request parameter.
	* Parameter -
	* Exception -
	* Return -
	*/
	string CreateProduct::getPublishRequestParameter(string _path, string _fileName)
	{
		Logger::Debug("Create product getPublishRequestParameter(string _path, string _imageName) start....");

		string contentType = Helper::GetFileContetType(_path);
		//UTILITY_API->DisplayMessageBox("contentType:" + contentType);
		string fileStream = Helper::GetFilestream(_path);
		string contentLength = Helper::getFileLength(_path);
		string postField = "------WebKitFormBoundary7MA4YWxkTrZu0gW\r\nContent-Disposition: form-data; name=\"file\"; filename=" + _fileName + "\r\nContent-Type: " + contentType + "\r\n" + contentLength + "\r\n\r\n" + fileStream + "\r\n------WebKitFormBoundary7MA4YWxkTrZu0gW";
		if (_fileName.find(".png") != -1)
		{
			postField += "\r\nContent-Disposition: form-data; name=\"generated_by\"\r\n\r\n";
			postField += "CLO3D";
			postField += "\r\n";
			postField += "------WebKitFormBoundary7MA4YWxkTrZu0gW--";
			Logger::Debug("Create product getPublishRequestParameter(string _path, string _imageName) end....");
		}
		else if (_fileName.find(".zip") != -1)
		{
			postField += "\r\nContent-Disposition: form-data; name=\"is_3d\"\r\n\r\n";
			postField += "true";
			postField += "\r\n";
			postField += "------WebKitFormBoundary7MA4YWxkTrZu0gW--";
		}
		else
			postField += "--";

		return postField;
	}

	string CreateProduct::uploadDocument(string _productId)
	{
		Logger::Debug("Create product uploadDocument() start....");
		vector<pair<string, string>> headerNameAndValueList;
		headerNameAndValueList.push_back(make_pair("content-Type", "application/json"));
		headerNameAndValueList.push_back(make_pair("Cookie", Configuration::GetInstance()->GetBearerToken()));

		json bodyJson = json::object();
		string zprjFileName = UTILITY_API->GetProjectName();
		bodyJson["node_name"] = zprjFileName;
		string bodyJsonString = to_string(bodyJson);
		string resultJsonString = REST_API->CallRESTPost(Configuration::GetInstance()->GetPLMServerURL() + RESTAPI::DOCUMENT_CREATE_API + "/" + _productId, &bodyJsonString, headerNameAndValueList, "Loading");
		if (!FormatHelper::HasContent(resultJsonString))
		{
			throw "Unable to initiliaze Document Configuration. Please try again or Contact your System Administrator.";
		}
		json detailJson = Helper::GetJsonFromResponse(resultJsonString, "{");
		//UTILITY_API->DisplayMessageBox(to_string(detailJson));

		string latestRevisionId = Helper::GetJSONValue<string>(detailJson, LATEST_REVISION_KEY, true);
		//UTILITY_API->DisplayMessageBox("Revision id  " + latestRevisionId);
		Logger::Debug("Create product uploadDocument() end....");
		return latestRevisionId;
	}
	/*
		* Description - uploadGLBFile() method used to Upload GLb file into PLM with .zip format
		* Parameter -
		* Exception -
		* Return -
		*/
	void CreateProduct::uploadGLBFile(string _productId)
	{
		Logger::Debug("CreateProduct uploadGLBFile() start....");
		try {
			string _3DModelFilePath = UTILITY_API->GetProjectFilePath();
			Helper::EraseSubString(_3DModelFilePath, UTILITY_API->GetProjectName());
			string m_GLBFilePath = _3DModelFilePath + UTILITY_API->GetProjectName();
			string FileName = UTILITY_API->GetProjectName() + ".zip";

			Marvelous::ImportExportOption option;
			option.bSaveInZip = true;
			option.bSaveColorWaysSingleFile = true;
			option.bSaveColorWays = true;
			EXPORT_API->ExportGLTF(m_GLBFilePath + GLB, option, true);
			Logger::Debug("m_GLBFilePath::" + m_GLBFilePath);
			RESTAPI::SetProgressBarData(20, "Uploading GLB file to PLM...", true);
			string postField = getPublishRequestParameter(m_GLBFilePath + ".zip", FileName);

			string resultJsonString;

			resultJsonString = RESTAPI::PostRestCall(postField, Configuration::GetInstance()->GetPLMServerURL() + RESTAPI::DOCUMENT_CREATE_API + "/" + _productId + "/upload", "content-type: multipart/form-data; boundary=----WebKitFormBoundary7MA4YWxkTrZu0gW");

			if (!FormatHelper::HasContent(resultJsonString))
			{
				throw "Unable to initiliaze Document Configuration. Please try again or Contact your System Administrator.";
			}
		}
		catch (string msg)
		{
			Logger::Error("CreateProduct uploadGLBFile() Exception - " + msg);
		}
		catch (exception & e)
		{
			Logger::Error("CreateProduct uploadGLBFile() Exception - " + string(e.what()));
		}
		catch (const char* msg)
		{
			wstring wstr(msg, msg + strlen(msg));
			Logger::Error("CreateProduct uploadGLBFile() Exception - " + string(msg));
		}
		RESTAPI::SetProgressBarData(0, "", false);
		Logger::Debug("CreateProduct uploadZip() end....");
	}
	/*
	* Description - exportZPRJ() method used to export a 3D model and visual images.
	* Parameter -
	* Exception - using throw if anything gets fail.
	* Return -
	*/
	void CreateProduct::exportZPRJ(string _revisionId)
	{
		Logger::Debug("Create product exportZPRJ() start....");
		QDir dir;
		string _3DModelFilePath = UTILITY_API->GetProjectFilePath();

		m_3DModelFileName = UTILITY_API->GetProjectName() + ".zprj";
		//UTILITY_API->DisplayMessageBox("m_3DModelFileName" + m_3DModelFileName);
		EXPORT_API->ExportZPrj(_3DModelFilePath, true);
		string postField = getPublishRequestParameter(_3DModelFilePath, m_3DModelFileName);
		string resultJsonString = RESTAPI::PutRestCall(postField, Configuration::GetInstance()->GetPLMServerURL() + RESTAPI::DOCUMENT_UPLOAD_API + "/" + _revisionId + "/upload", "content-type: multipart/form-data; boundary=----WebKitFormBoundary7MA4YWxkTrZu0gW");
		//UTILITY_API->DisplayMessageBox("Upload zprj completed"+ resultJsonString);
		Logger::Debug("Create product exportZPRJ() end....");
	}

	/*
	* Description - exportGLB() method used to export a 3D model and visual images.
	* Parameter - bool
	* Exception - using throw if anything gets fail.
	* Return -
	*/
	void CreateProduct::exportGLB(bool _isGLBExportable)
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
	json CreateProduct::collectCriteriaFields(QTreeWidget* _documentPushToPLMTree_1, QTreeWidget* _documentPushToPLMTree_2)
	{
		Logger::Debug("Create product collectCriteriaFields() start....");
		json attsJson = json::object();
		json attJson = json::object();
		string fieldAndValues = "{";
		string attkey;
		string attValue;
		for (int i = 0; i < _documentPushToPLMTree_1->topLevelItemCount(); i++)
		{
			attJson = ReadVisualUIFieldValue(_documentPushToPLMTree_1, i);

			if (!attJson.empty())
			{
				attkey = Helper::GetJSONValue<string>(attJson, "attKey", true);
				attValue = Helper::GetJSONValue<string>(attJson, "attValue", true);
				if (!attValue.empty())
					attsJson[attkey] = attValue;
			}
		}
		Logger::Debug("Create product collectCriteriaFields() attsJson...." + to_string(attsJson));
		Logger::Debug("Create product collectCriteriaFields() 1....");
		for (int i = 0; i < _documentPushToPLMTree_2->topLevelItemCount(); i++)
		{
			attJson = ReadVisualUIFieldValue(_documentPushToPLMTree_2, i);

			if (!attJson.empty())
			{
				attkey = Helper::GetJSONValue<string>(attJson, "attKey", true);
				attValue = Helper::GetJSONValue<string>(attJson, "attValue", true);
				if (!attValue.empty())
					attsJson[attkey] = attValue;
			}
		}
		//attJson["cus_cost_factors"] = "18";
		Logger::Debug("Create product collectCriteriaFields() attsJson:: " + to_string(attsJson));
		//UTILITY_API->DisplayMessageBox("Create product collectCriteriaFields() attsJson:: " + to_string(attsJson));
		Logger::Debug("Create product collectCriteriaFields() end....");
		//UTILITY_API->DisplayMessageBox(fieldAndValues);
		return attsJson;
	}

	void CreateProduct::onAddColorwaysClicked()
	{
		Logger::Debug("createProduct -> onAddColorwaysClicked() -> Start");

		AddColorwayDetails();

		Logger::Debug("createProduct -> onAddColorwaysClicked() -> End");
	}



	void CreateProduct::AddColorwayDetails()
	{

		Logger::Debug("CreateProduct -> AddColorwayDetails() -> Start");
		//SetProgressBarData(15, m_translationMap["pbAddColor"][m_languageIndex], true);

		ui_colorwayTable->show();
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


		QStringList colorwayNamesList;

		int colorwayCount = UTILITY_API->GetColorwayCount();
		colorwayNamesList.append(QString::fromStdString(BLANK));
		for (int count = 0; count < colorwayCount; count++)
		{
			string colorwayName = UTILITY_API->GetColorwayName(count);
			colorwayNamesList.append(QString::fromStdString(colorwayName));
		}


		AddRows(count, attId, objectName, rgbValue, objectCode, pantone, colorwayNamesList);

		for (int index = 0; index < headerlist.count(); index++)
		{
			if (index == CHECKBOX_COLUMN)
				ui_colorwayTable->setColumnWidth(index, COLUMN_SIZE);
			//ui_colorwayTable->resizeColumnToContents(1);
			else if (index == CLO_COLORWAY_COLUMN || index == UNI_2_DIGIT_CODE_COLUMN)
			{
				ui_colorwayTable->resizeColumnToContents(1);
			}

			else
				ui_colorwayTable->setColumnWidth(index, 80);
		}
		//SetProgressBarData(0, "", false);
		Logger::Debug("CreateProduct -> AddColorwayDetails() -> End");
	}

	bool CreateProduct::ValidateColorwayNameField()
	{
		Logger::Debug("createProduct -> ValidateColorwayNameField() -> Start");

		QStringList list;
		bool duplicateColrwayName = false;

		for (int row = 0; row < ui_colorwayTable->rowCount(); row++)
		{
			//QComboBox* colorwayNameCombo = qobject_cast<QComboBox*>(ui_colorwayTable->cellWidget(row, 1)->layout()->widget());
			QComboBox *colorwayNameCombo = static_cast<QComboBox*>(ui_colorwayTable->cellWidget(row, CLO_COLORWAY_COLUMN)->children().last());
			QComboBox *plmColorwayCombo = static_cast<QComboBox*>(ui_colorwayTable->cellWidget(row, UNI_2_DIGIT_CODE_COLUMN)->children().last());
			//QComboBox* colorwayNameCombo;
			//colorwayNameCombo = ui_colorwayTable->findChild<QComboBox*>();
			QLineEdit *plmColorwayName = static_cast<QLineEdit*>(ui_colorwayTable->cellWidget(row, PLM_COLORWAY_COLUMN)->children().last());
			if (plmColorwayCombo)
			{
				QString colorwayName = plmColorwayCombo->currentText();
				if (colorwayName.isEmpty() && m_2DigiCodeActive)
				{
					UTILITY_API->DisplayMessageBox("2 Digi Code cannot be blank");
					ui_tabWidget->setCurrentIndex(COLORWAY_TAB);
					duplicateColrwayName = true;
					break;
				}
			}
			if (colorwayNameCombo)
			{
				QString colorwayName = colorwayNameCombo->currentText();
				if (!colorwayName.isEmpty())
				{
					if (list.contains(colorwayName))
					{
						UTILITY_API->DisplayMessageBox("CLO Colorway names cannot be repeated");
						ui_tabWidget->setCurrentIndex(COLORWAY_TAB);
						duplicateColrwayName = true;
						break;
					}
					else
						list.append(colorwayName);
				}
			}
			if (plmColorwayName)
			{
				QString userEnteredPLMColorwayName = plmColorwayName->text();
				if (userEnteredPLMColorwayName.isEmpty())
				{
					UTILITY_API->DisplayMessageBox("Centric Color name cannot be blank");
					ui_tabWidget->setCurrentIndex(COLORWAY_TAB);
					duplicateColrwayName = true;
					break;
				}
			}
		}
		if (!duplicateColrwayName)
			CreateImageIntent::GetInstance()->fillSelectedList(list);
		Logger::Debug("createProduct -> ValidateColorwayNameField() -> End");
		return	!duplicateColrwayName;
	}

	void CreateProduct::GetUpdatedColorwayNames()
	{
		Logger::Debug("createProduct -> GetUpdatedColorwayNames() -> Start");
		m_modifiedColorwayNames.clear();
		int comboSize;
		int comboBoxIndex;
		vector<int> comboBoxIndexList;
		int noOfRows = ui_colorwayTable->rowCount();

		for (int rowIndex = 0; rowIndex < noOfRows; rowIndex++)
		{
			QComboBox *colorwayNameCombo = static_cast<QComboBox*>(ui_colorwayTable->cellWidget(rowIndex, CLO_COLORWAY_COLUMN)->children().last());

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
		colorwayNamesList.append(QString::fromStdString(BLANK));
		for (int count = 0; count < colorwayCount; count++)
		{
			string colorwayName = UTILITY_API->GetColorwayName(count);
			colorwayNamesList.append(QString::fromStdString(colorwayName));
		}

		for (int rowIndex = 0; rowIndex < ui_colorwayTable->rowCount(); rowIndex++)
		{
			QComboBox *colorwayNameCombo = static_cast<QComboBox*>(ui_colorwayTable->cellWidget(rowIndex, CLO_COLORWAY_COLUMN)->children().last());

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
		Logger::Debug("createProduct -> GetUpdatedColorwayNames() -> End");
	}

	void CreateProduct::AddRows(int _count, string _objectId, string _objectName, string _rgbValue, string _code, string _pantone, QStringList _colorwayNamesList)
	{

		Logger::Debug("createProduct -> AddRows() -> Start");
		m_colorwayRowcount = ui_colorwayTable->rowCount();
		ui_colorwayTable->setRowCount(m_colorwayRowcount);
		ui_colorwayTable->insertRow(m_colorwayRowcount);
		QWidget *pWidget = nullptr;
		QWidget *newQWidget = new QWidget();
		QPushButton* updateColorButton = CVWidgetGenerator::CreatePushButton("Search Color", "", "Search Color", PUSH_BUTTON_STYLE, 30, true);
		QPushButton* ColorCreateButton = new QPushButton();
		ColorCreateButton->setStyleSheet("QPushButton{max-height: 20px; max-width: 10px;} ::menu-indicator{ image: none; }");
		ColorCreateButton->setIcon(QIcon(":/CLOVise/PLM/Images/ui_spin_icon_minus_none.svg"));
		ColorCreateButton->setIconSize(QSize(25, 22));
		ColorCreateButton->setMaximumWidth(10);
		ColorCreateButton->setProperty(("row"), _count);
		QAction* colorSpecAction = new QAction(tr("Create Color Specification"), this);
		colorSpecAction->setEnabled(m_isCreateColorSpec);
		QAction* printAction = new QAction(tr("Search Prints"), this);
		printAction->setEnabled(true);
		QMenu* menu = new QMenu(ColorCreateButton);
		menu->addAction(colorSpecAction);
		menu->addAction(printAction);
		ColorCreateButton->setMenu(menu);
		QHBoxLayout* pLayout = new QHBoxLayout(newQWidget);
		pLayout->insertWidget(0, updateColorButton);
		pLayout->insertWidget(1, ColorCreateButton);
		pLayout->setSpacing(0);
		newQWidget->setLayout(pLayout);
		pWidget = CVWidgetGenerator::InsertWidgetInCenter(newQWidget);
		if (m_buttonSignalMapper != nullptr)
		{
			connect(updateColorButton, SIGNAL(clicked()), m_buttonSignalMapper, SLOT(map()));
			m_buttonSignalMapper->setMapping(updateColorButton, m_colorwayRowcount);
		}
		if (m_createActionSignalMapper != nullptr)
		{
			connect(colorSpecAction, SIGNAL(triggered()), m_createActionSignalMapper, SLOT(map()));
			m_createActionSignalMapper->setMapping(colorSpecAction, m_colorwayRowcount);
		}
		if (m_printActionSignalMapper != nullptr)
		{
			connect(printAction, SIGNAL(triggered()), m_printActionSignalMapper, SLOT(map()));
			m_printActionSignalMapper->setMapping(printAction, m_colorwayRowcount);
		}

		QPushButton *deleteButton = CVWidgetGenerator::CreatePushButton("", ":/CLOVise/PLM/Images/icon_delete_over.svg", "Delete", PUSH_BUTTON_STYLE, 30, true);
		QWidget *pdeleteWidget = CVWidgetGenerator::InsertWidgetInCenter(deleteButton);

		if (m_colorwayDeleteSignalMapper != nullptr)
		{
			connect(deleteButton, SIGNAL(clicked()), m_colorwayDeleteSignalMapper, SLOT(map()));
			m_colorwayDeleteSignalMapper->setMapping(deleteButton, m_colorwayRowcount);
		}
		ui_colorwayTable->setCellWidget(m_colorwayRowcount, COLORWAY_DELETE_COLUMN, pdeleteWidget);
		ui_colorwayTable->setColumnWidth(COLORWAY_DELETE_COLUMN, 50);
		ui_colorwayTable->setWordWrap(true);

		ui_colorwayTable->setCellWidget(m_colorwayRowcount, UPDATE_BTN_COLUMN, pWidget);
		ui_colorwayTable->setColumnWidth(UPDATE_BTN_COLUMN, COLUMN_SIZE);
		ComboBoxItem* comboColorwayItem = new ComboBoxItem();
		comboColorwayItem->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
		comboColorwayItem->setFocusPolicy(Qt::StrongFocus);
		comboColorwayItem->addItems(_colorwayNamesList);
		comboColorwayItem->setProperty("Id", QString::fromStdString(_objectId));

		ui_colorwayTable->setColumnWidth(CLO_COLORWAY_COLUMN, 150);
		pWidget = CVWidgetGenerator::InsertWidgetInCenter(comboColorwayItem);
		ui_colorwayTable->setColumnWidth(CLO_COLORWAY_COLUMN, 150);

		ui_colorwayTable->setCellWidget(m_colorwayRowcount, CLO_COLORWAY_COLUMN, pWidget);

		QObject::connect(comboColorwayItem, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(OnHandleColorwayNameComboBox(const QString&)));

		m_plmColowayName = new QLineEdit();
		m_plmColowayName->setStyleSheet(LINEEDIT_STYLE);
		m_plmColowayName->setAttribute(Qt::WA_MacShowFocusRect, false);
		m_plmColowayName->setText("");
		QWidget *pLineEditWidget = CVWidgetGenerator::InsertWidgetInCenter(m_plmColowayName);
		m_plmColowayName->setProperty("row", _count + m_colorwayRowcount);
		m_plmColowayName->setProperty("Edited", false);
		connect(m_plmColowayName, SIGNAL(editingFinished()), this, SLOT(OnplmColorwayNameEntered()));
		ui_colorwayTable->setCellWidget(_count + m_colorwayRowcount, PLM_COLORWAY_COLUMN, pLineEditWidget);

		QSize iconSize(40, 40);
		ui_colorwayTable->setIconSize(iconSize);
		ui_colorwayTable->setWordWrap(true);

		QTableWidgetItem* iconItem = new QTableWidgetItem;
		iconItem->setSizeHint(iconSize);

		Logger::Debug("colorRGB:: " + _rgbValue);

		//UTILITY_API->GetCurrentColorwayIndex();
		QStringList listRGB;
		QImage Icon;
		string rgbValue = _rgbValue;
		rgbValue = Helper::FindAndReplace(_rgbValue, "(", "");
		rgbValue = Helper::FindAndReplace(rgbValue, ")", "");
		rgbValue = Helper::FindAndReplace(rgbValue, " ", "");
		if (FormatHelper::HasContent(rgbValue))
		{

			QStringList listRGB;
			QString colorRGB = QString::fromStdString(rgbValue);
			listRGB = colorRGB.split(',');
			int red = listRGB.at(0).toInt();
			int green = listRGB.at(1).toInt();
			int blue = listRGB.at(2).toInt();
			QColor color(red, green, blue);
			QImage image(80, 80, QImage::Format_ARGB32);
			image.fill(color);
			iconItem->setIcon(QIcon(QPixmap::fromImage(image)));
			iconItem->setToolTip(QString::fromStdString(rgbValue));
			QPixmap pixmap;
			QLabel* label = new QLabel();
			pixmap = QPixmap::fromImage(image);
			label->setPixmap(QPixmap(pixmap));
			pWidget = CVWidgetGenerator::InsertWidgetInCenter(label);
			ui_colorwayTable->setCellWidget(m_colorwayRowcount, COLOR_CHIP_COLUMN, pWidget);
		}


		if (_objectName == "null")
			_objectName = " ";
		QTableWidgetItem* ColorNameWidget = new QTableWidgetItem(QString::fromStdString(_objectName));
		ColorNameWidget->setTextAlignment(Qt::AlignCenter);
		ColorNameWidget->setToolTip(QString::fromStdString(_objectName));
		ui_colorwayTable->setItem(m_colorwayRowcount, COLOR_NAME_COLUMN, ColorNameWidget);

		if (_code == "null")
			_code = " ";
		QTableWidgetItem* ColorCodeWidget = new QTableWidgetItem(QString::fromStdString(_code));
		ColorCodeWidget->setTextAlignment(Qt::AlignCenter);
		ColorCodeWidget->setToolTip(QString::fromStdString(_objectId));
		ui_colorwayTable->setItem(m_colorwayRowcount, COLOR_CODE_COLUMN, ColorCodeWidget);

		if (_pantone == "null")
			_pantone = " ";
		QTableWidgetItem* ColorStatusWidget = new QTableWidgetItem(QString::fromStdString(_pantone));
		ColorStatusWidget->setTextAlignment(Qt::AlignCenter);
		ColorStatusWidget->setToolTip(QString::fromStdString(_pantone));
		ui_colorwayTable->setItem(m_colorwayRowcount, PANTONE_CODE_COLUMN, ColorStatusWidget);

		ComboBoxItem* uni2digiCodes = new ComboBoxItem();
		uni2digiCodes->setFocusPolicy(Qt::StrongFocus);

		QStringList valueList;
		for (auto it = m_digiCodeNamesMap.begin(); it != m_digiCodeNamesMap.end(); it++)
		{
			uni2digiCodes->setProperty(it->second.c_str(), it->first.c_str());
			uni2digiCodes->setProperty(it->first.c_str(), it->second.c_str());
		}
		uni2digiCodes->addItems(m_digiCodeValue);

		int indexOfSelectedString = uni2digiCodes->findText(QString::fromStdString(BLANK));
		uni2digiCodes->setCurrentIndex(indexOfSelectedString);
		uni2digiCodes->setProperty("row", m_colorwayRowcount);
		pWidget = CVWidgetGenerator::InsertWidgetInCenter(uni2digiCodes);
		if (m_2DigiCodeActive)
		{
			connect(uni2digiCodes, SIGNAL(activated(const QString&)), this, SLOT(OnUni2CodeSelected(const QString&)));
		}
		ui_colorwayTable->setCellWidget(m_colorwayRowcount, UNI_2_DIGIT_CODE_COLUMN, pWidget);

		QTextEdit* textEditItem = new QTextEdit();
		textEditItem->setStyleSheet(TEXTEDIT_STYLE);
		textEditItem->setText("");
		pWidget = CVWidgetGenerator::InsertWidgetInCenter(textEditItem);
		ui_colorwayTable->setCellWidget(m_colorwayRowcount, DESCRIPTION_COLUMN, pWidget);
		Logger::Debug("createProduct -> AddRows() -> End");
	}

	/*
	* Description - OnCreateColorSpecClicked() method is the slot for calling the search print widget.
	* Parameter - int
	* Exception -
	* Return -
	*/
	void CreateProduct::OnCreateColorSpecClicked(int _SelectedRow)
	{
		Logger::Info("INFO::createProduct -> OnCreateColorSpecClicked() -> Start");
		Configuration::GetInstance()->SetIsUpdateColorClicked(true);
		m_selectedRow = _SelectedRow;
		m_currentColorSpec = BLANK;
		this->hide();

		if (ui_colorwayTable->rowCount() != 0)
		{
			GetUpdatedColorwayNames();
		}
		QComboBox *colorwayNameCombo = static_cast<QComboBox*>(ui_colorwayTable->cellWidget(m_selectedRow, CLO_COLORWAY_COLUMN)->children().last());
		string colorSpecId = colorwayNameCombo->property("Id").toString().toStdString();
		if (!colorSpecId.empty())
			m_currentColorSpec = colorSpecId;

		this->hide();
		ColorConfig::GetInstance()->m_mode = "Create";
		ColorConfig::GetInstance()->InitializeColorData();
		ColorConfig::GetInstance()->m_isSearchColor = false;
		PLMColorSearch::GetInstance()->setModal(true);
		PLMColorSearch::GetInstance()->DrawSearchWidget(true);
		UTILITY_API->DeleteProgressBar(true);
		PLMColorSearch::GetInstance()->exec();
		RESTAPI::SetProgressBarData(0, "", false);
		Logger::Info("INFO::createProduct -> OnCreateColorSpecClicked() -> Start");
	}

	/*
	* Description - OnSearchPrintClicked() method is the slot for calling the search print widget.
	* Parameter - int
	* Exception -
	* Return -
	*/
	void CreateProduct::OnSearchPrintClicked(int _SelectedRow)
	{
		Logger::Info("INFO::createProduct -> OnSearchPrintClicked() -> Start");
		Configuration::GetInstance()->SetIsPrintSearchClicked(true);
		m_selectedRow = _SelectedRow;
		m_currentColorSpec = BLANK;
		this->hide();

		if (ui_colorwayTable->rowCount() != 0)
		{
			GetUpdatedColorwayNames();
		}
		QComboBox *colorwayNameCombo = static_cast<QComboBox*>(ui_colorwayTable->cellWidget(m_selectedRow, CLO_COLORWAY_COLUMN)->children().last());
		string colorSpecId = colorwayNameCombo->property("Id").toString().toStdString();
		if (!colorSpecId.empty())
			m_currentColorSpec = colorSpecId;

		this->hide();
		if (!PrintConfig::GetInstance()->GetIsModelExecuted())
			PrintConfig::GetInstance()->InitializePrintData();
		PLMPrintSearch::GetInstance()->setModal(true);
		if (!PrintConfig::GetInstance()->GetIsModelExecuted())
			PLMPrintSearch::GetInstance()->DrawSearchWidget(true);
		else
			PLMPrintSearch::GetInstance()->DrawSearchWidget(false);
		UTILITY_API->DeleteProgressBar(true);
		PLMPrintSearch::GetInstance()->exec();
		RESTAPI::SetProgressBarData(0, "", false);
		PrintConfig::GetInstance()->SetIsModelExecuted(true);
		Logger::Info("INFO::createProduct -> OnSearchPrintClicked() -> Start");
	}

	void CreateProduct::OnplmColorwayNameEntered()
	{
		sender()->setProperty("Edited", true);
	}

	void CreateProduct::OnUni2CodeSelected(const QString& _str)
	{
		int nRow = sender()->property("row").toInt();
		QLineEdit *plmColorwayName = static_cast<QLineEdit*>(ui_colorwayTable->cellWidget(nRow, PLM_COLORWAY_COLUMN)->children().last());
		if (!_str.isEmpty())
		{
			if (plmColorwayName->text().isEmpty() || !plmColorwayName->property("Edited").toBool())
			{
				string nodeName = m_digiCodeNamesMap[_str.toStdString()];
				plmColorwayName->setText(QString::fromStdString(nodeName));
				plmColorwayName->setProperty("Edited", false);
				//UTILITY_API->DisplayMessageBox("node name is " + nodeName);
			}
		}


	}
	void CreateProduct::onSaveAndCloseClicked()
	{
		Logger::Debug("createProduct -> SaveClicked() -> Start");



		PublishToPLMData::GetInstance()->m_isSaveClicked = true;

		Configuration::GetInstance()->SetIsPrintSearchClicked(false);

		GetUpdatedColorwayNames();
		//if (ExtractAllUIValues())
		{
			//CreateProductCreateMap();
			//m_isUserInputEmpty = true;
			ui_tabWidget->setCurrentIndex(OVERVIEW_TAB);
			ui_tabWidget->setStyleSheet(" QTabWidget::pane { border: none; color: #FFFFFF; font-size: 10px; background-color: #262628; }""QTabBar::tab { width: 100px; padding: 2px; }""QTabBar::tab:selected { border: none; color: #FFFFFF; background-color: \"" + DEFAULT_TAB_BG_COLOR + "\"; }""QTabBar::tab:!selected { color:#FFFFFF; background-color:\"" + SELECTED_TAB_BG_COLOR + "\"; }");
			this->hide();
			if (UTILITY_API)
				UTILITY_API->DisplayMessageBox(Configuration::GetInstance()->GetLocalizedStyleClassName() + " Metadata Saved");
		}
		CreateProductBOMHandler::GetInstance()->BackupBomDetails();
		Logger::Debug("createProduct -> SaveClicked() -> End");
	}

	void CreateProduct::onTabClicked(int _index)
	{
		Logger::Debug("Create product onTabClicked() Start");
		QStringList list;
		bool duplicateColrwayName = false;
		if (_index == COLORWAY_TAB)
		{
			if (m_selectedStyleTypeIndex <= 0)
			{
				UTILITY_API->DisplayMessageBox("Please select the Style Type in the Overview tab to add colorways");
				ui_tabWidget->setCurrentIndex(0);
			}
			else
			{
				m_prevSelectedStyleTypeIndex = m_selectedStyleTypeIndex;
				ui_colorwayTable->setColumnCount(m_ColorwayTableColumnNames.size());
				ui_colorwayTable->setHorizontalHeaderLabels(m_ColorwayTableColumnNames);
				ui_colorwayTable->show();
			}
		}
		if (_index == IMAGE_INTENT_TAB /*&& m_colorwayRowcount > 0*/)//Image Intent tab
		{
			ValidateColorwayNameField();
		}
		//	int imageRowCount = m_ImageIntentList->count();
		//	if (imageRowCount != 0)
		//	{
		//		UTILITY_API->CreateProgressBar();
		//		RESTAPI::SetProgressBarData(20, "Loading Latest Image Intents... ", true);
		//		UTILITY_API->SetProgress("Loading Latest Image Intents...", (qrand() % 101));
		//	}

		//	if (ValidateColorwayNameField() && imageRowCount != 0)
		//	{
		//		exportTurntableImages();

		//		string colorwayName;
		//		string includeAvatar;
		//		string viewName;
		//		int view;
		//		Logger::Debug("CreateProduct -> onTabClicked() -> Item" + to_string(imageRowCount));

		//		string temporaryPath = UTILITY_API->GetCLOTemporaryFolderPath();

		//		for (int index = 0; index < imageRowCount; index++)
		//		{
		//			QListWidgetItem* listImageItem = new QListWidgetItem();
		//			QListWidgetItem* item = m_ImageIntentList->item(index);
		//			QListWidget *listItem = qobject_cast<QListWidget*>(m_ImageIntentList->itemWidget(item));
		//			for (int itemIndex = 0; itemIndex < listItem->count(); itemIndex++)
		//			{
		//				string text = listItem->item(itemIndex)->text().toStdString();
		//				Logger::Debug("CreateProduct -> onTabClicked() -> Item" + text);
		//				if (itemIndex == 0)
		//				{
		//					int length = text.length();
		//					int indexOfColon = text.find(":");
		//					colorwayName = text.substr(indexOfColon + 2, length);
		//					Logger::Debug("CreateProduct -> onTabClicked() -> clorwayname" + colorwayName);

		//				}
		//				if (itemIndex == 1)
		//				{
		//					int length = text.length();
		//					int indexOfColon = text.find(":");
		//					viewName = text.substr(indexOfColon + 1, length);
		//					Logger::Debug("CreateProduct -> onTabClicked() -> viewName" + viewName);

		//					if (viewName == " Back")
		//						view = BACK_VIEW;
		//					else if (viewName == " Front")
		//						view = FRONT_VIEW;
		//					else if (viewName == " Left")
		//						view = LEFT_VIEW;
		//					else
		//						view = RIGHT_VIEW;
		//				}
		//				if (itemIndex == 4)
		//				{
		//					int length = text.length();
		//					int indexOfColon = text.find(":");
		//					includeAvatar = text.substr(indexOfColon + 2, length);
		//					Logger::Debug("CreateImageIntent -> onTabClicked() -> includeAvatar" + includeAvatar);

		//				}
		//			}

		//			QString filepath;
		//			if (includeAvatar == "Yes")
		//				filepath = QString::fromStdString(temporaryPath) + "CLOViseTurntableImages/WithAvatar/" + QString::fromStdString(colorwayName) + "_" + QString::fromStdString(to_string(view)) + ".png";
		//			else
		//				filepath = QString::fromStdString(temporaryPath) + "CLOViseTurntableImages/WithoutAvatar/" + QString::fromStdString(colorwayName) + "_" + QString::fromStdString(to_string(view)) + ".png";

		//			Logger::Debug("CreateImageIntent -> onTabClicked() -> filepath" + filepath.toStdString());

		//			item->setTextAlignment(Qt::AlignLeft);
		//			QPixmap pix(filepath);
		//			pix.scaled(QSize(80, 80), Qt::KeepAspectRatio);
		//			QIcon newIcon;
		//			newIcon.addPixmap(pix);
		//			Logger::Debug("CreateProduct -> onTabClicked() -> 1");
		//			item->setIcon(newIcon);
		//			item->setSizeHint(QSize(80, 80));
		//			m_ImageIntentList->addItem(item);
		//			Logger::Debug("CreateProduct -> onTabClicked() -> 3");
		//			CreateProduct::GetInstance()->m_ImageIntentList->setIconSize(QSize(80, 80));
		//			//m_ImageIntentList->takeItem(index);
		//			m_ImageIntentList->addItem(item);
		//			CreateProduct::GetInstance()->m_ImageIntentList->setItemWidget(item, listItem);
		//		}
		//	}

		//	RESTAPI::SetProgressBarData(0, "", false);
		//}

		if (_index == BOM_TAB)
		{
			if (m_selectedStyleTypeIndex <= 0)
			{
				UTILITY_API->DisplayMessageBox("Please select the Style Type in the Overview tab to create BOM ");
				ui_tabWidget->setCurrentIndex(0);
			}
			else
			{

				if (PublishToPLMData::GetInstance()->m_isSaveClicked && m_updateBomTab && CreateProductBOMHandler::GetInstance()->IsBomCreated())
				{

					CreateProductBOMHandler::GetInstance()->RestoreBomDetails();
					m_updateBomTab = false;
				}
				GetMappedColorway();
				UpdateColorwayColumnsInBom();

			}
		}

		Logger::Debug("Create product onTabClicked() End");
	}

	void CreateProduct::OnHandleDropDownValue(const QString& _item)
	{

		try
		{
			Logger::Debug("Create product OnHandleDropDownValue() Start");
			map<string, string>::iterator it;
			map < string, string> nameIdMap;
			string id;

			QString labelName = sender()->property("LabelName").toString();
			string apiUrl = "";
			QString comboboxtofill = "";

			Logger::Debug("CreateProduct -> OnHandleDropDownValue() LabelName: " + labelName.toStdString());
			string progressbarText;
			Logger::Debug("PublishToPLMData -> OnHandleDropDownValue() _item: " + _item.toStdString());
			if (labelName == "Style Type")
			{
				QComboBox * comboBox = qobject_cast<QComboBox *>(sender());
				Logger::Debug("CreateProduct -> OnHandleDropDownValue() senderSignalIndex: " + to_string(comboBox->currentIndex()));
				m_selectedStyleTypeIndex = comboBox->currentIndex();
				Logger::Debug("CreateProduct -> OnHandleDropDownValue() Getcurrent text: " + comboBox->currentText().toStdString());
				m_currentlySelectedStyleTypeId = sender()->property(_item.toStdString().c_str()).toString().toStdString();
				Logger::Debug("PublishToPLMData -> OnHandleDropDownValue() m_currentlySelectedStyleTypeId: " + m_currentlySelectedStyleTypeId);

				string selectedStyleType = comboBox->currentText().toStdString();
				auto it = m_styleTypeBomPermissionMap.find(selectedStyleType);
				if (it != m_styleTypeBomPermissionMap.end())
				{
					if (FormatHelper::HasContent(it->second) && it->second == "true")
						m_bomAddButton->show();
					else
						m_bomAddButton->hide();

				}

				string isAllowCreatColor = sender()->property((_item.toStdString() + ALLOW_CREATE_COLOR).c_str()).toString().toStdString();
				if (isAllowCreatColor == "true")
					m_isCreateColorSpec = true;
				else
					m_isCreateColorSpec = false;

				if ((m_selectedStyleTypeIndex != m_prevSelectedStyleTypeIndex && ui_colorwayTable->rowCount() > 0) || (m_selectedStyleTypeIndex != m_prevSelectedStyleTypeIndex && CreateProductBOMHandler::GetInstance()->IsBomCreated()))
				{
					if (m_selectedStyleTypeIndex != 0)
					{
						QString message;
						QString bomMessage = "";
						QMessageBox* deleteMessage = new QMessageBox(this);
#ifdef __APPLE__
						deleteMessage->setModal(true);
						deleteMessage->setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint | Qt::CustomizeWindowHint | Qt::WindowStaysOnTopHint);
						deleteMessage->setWindowModality(Qt::WindowModal);
#endif // __APPLE__
						deleteMessage->setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint | Qt::CustomizeWindowHint);
						deleteMessage->setStandardButtons(QMessageBox::Yes | QMessageBox::No);
						deleteMessage->setIcon(QMessageBox::Question);
						if (CreateProductBOMHandler::GetInstance()->IsBomCreated())
							bomMessage = " and BOM tab";
						message = "Changing the Style Type will delete the existing information on the Colorway" + bomMessage + ". Are you sure you want to proceed? ";
						deleteMessage->setText(message);

						if (deleteMessage->exec() == QMessageBox::Yes)
						{
							m_prevSelectedStyleTypeIndex = m_selectedStyleTypeIndex;
							for (int rowCount = 0; rowCount < ui_colorwayTable->rowCount(); rowCount++)
							{
								ui_colorwayTable->removeRow(rowCount);
							}
							if (CreateProductBOMHandler::GetInstance()->IsBomCreated())
							{
								m_CloAndPLMColorwayMap.clear();
								ClearBOMData();
							}
						}
						else
						{
							m_bomAddButton->hide();
							comboBox->setCurrentIndex(m_prevSelectedStyleTypeIndex);
							m_selectedStyleTypeIndex = m_prevSelectedStyleTypeIndex;
						}
					}
				}
				string isreferDefauleImage = sender()->property(REFER_DEFAULT_IMAGE_ON_COLOR.c_str()).toString().toStdString();
			}
			else if (!_item.isEmpty())
			{
				QStringList dependentFields;

				if (labelName == "Season")
				{
					apiUrl = RESTAPI::SEASON_SEARCH_API;
					nameIdMap = m_seasonNameIdMap;
					m_brandNameIdMap.clear();
					m_shapeNameIdMap.clear();
					m_themeNameIdMap.clear();
					comboboxtofill = "Brand";
					dependentFields.append("category_1");
					dependentFields.append("shape");
					dependentFields.append("theme");
					clearDependentComboBox(labelName, m_createProductTreeWidget_1);
					clearDependentComboBox(labelName, m_createProductTreeWidget_2);
					progressbarText = "Loading Brand details..";
				}
				else if (labelName == "Brand")
				{
					Logger::Debug("CreateProduct -> OnHandleDropDownValue() Brand combobox Start ");
					apiUrl = RESTAPI::DEPARTMENT_DETAIL_API;
					Logger::Debug("CreateProduct -> OnHandleDropDownValue() m_brandNameIdMap " + to_string(m_brandNameIdMap.size()));
					nameIdMap = m_brandNameIdMap;
					m_dipartmentNameIdMap.clear();
					dependentFields.append("category_2");
					clearDependentComboBox(labelName, m_createProductTreeWidget_1);
					clearDependentComboBox(labelName, m_createProductTreeWidget_2);
					progressbarText = "Loading Department details..";
					Logger::Debug("CreateProduct -> OnHandleDropDownValue() Brand combobox  End");
				}
				else if (labelName == "Department")
				{
					Logger::Debug("CreateProduct -> OnHandleDropDownValue() Department combobox Start ");
					apiUrl = RESTAPI::COLLECTION_DETAIL_API;
					Logger::Debug("CreateProduct -> OnHandleDropDownValue() m_brandNameIdMap " + to_string(m_dipartmentNameIdMap.size()));
					nameIdMap = m_dipartmentNameIdMap;
					m_collectionNameIdMap.clear();
					dependentFields.append("collection");
					clearDependentComboBox(labelName, m_createProductTreeWidget_1);
					clearDependentComboBox(labelName, m_createProductTreeWidget_2);
					progressbarText = "Loading Division details..";
					Logger::Debug("CreateProduct -> OnHandleDropDownValue() Department combobox  End");
				}
				else if (labelName == "Collection")
				{
					m_collectionId = sender()->property(_item.toStdString().c_str()).toString().toStdString();
					Logger::Debug("CreateProduct -> OnHandleDropDownValue() Department Collection  End");
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

					Logger::Debug("PublishToPLMData -> SetDocumentConfigJSON Season Id: " + id);
					Logger::Debug("PublishToPLMData -> SetDocumentConfigJSON valueList size: " + to_string(dependentFields.size()));
					for (int itemIndex = 0; itemIndex < m_createProductTreeWidget_1->topLevelItemCount(); ++itemIndex)
					{
						QTreeWidgetItem* topItem = m_createProductTreeWidget_1->topLevelItem(itemIndex);
						QWidget* qWidgetColumn_0 = m_createProductTreeWidget_1->itemWidget(topItem, 0);
						QWidget* qWidgetColumn_1 = m_createProductTreeWidget_1->itemWidget(topItem, 1);
						if (!qWidgetColumn_0 || !qWidgetColumn_1)
						{
							continue;
						}
						QLabel* qlabel = qobject_cast<QLabel*>(qWidgetColumn_0);
						QString lableText = qlabel->property(ATTRIBUTE_KEY.c_str()).toString();

						Logger::Debug("PublishToPLMData -> OnHandleDropDownValue() Label text: " + (lableText.toStdString()));
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
									dependentFieldJson = Helper::makeRestcallGet(RESTAPI::SHAPE_API, "?skip=0&limit=100", "?shape_seasons=" + id, progressbarText);

								}
								else if (lableText == "theme")
								{
									progressbarText = "Loading Theme details..";
									dependentFieldJson = Helper::makeRestcallGet(RESTAPI::THEME_API, "?skip=0&limit=100", "?theme_seasons=" + id, progressbarText);
								}
								else
								{
									dependentFieldJson = Helper::makeRestcallGet(apiUrl, "/hierarchy?skip=0&limit=1000", "/" + id, progressbarText);
								}
								json attJson = json::object();

								for (int i = 0; i < dependentFieldJson.size(); i++)
								{
									attJson = Helper::GetJSONParsedValue<int>(dependentFieldJson, i, false);;///use new method
									attName = Helper::GetJSONValue<string>(attJson, ATTRIBUTE_NAME, true);
									Logger::Debug("PublishToPLMData -> OnHandleDropDownValue() attName: " + attName);
									attId = Helper::GetJSONValue<string>(attJson, ATTRIBUTE_ID, true);
									Logger::Debug("PublishToPLMData -> OnHandleDropDownValue() attId: " + attId);
									valueList.append(QString::fromStdString(attName));
									qComboBox->setProperty(attName.c_str(), QString::fromStdString(attId));
									if (labelName == "Season")
										m_brandNameIdMap.insert(make_pair(attName, attId));
									if (labelName == "Brand")
										m_dipartmentNameIdMap.insert(make_pair(attName, attId));
									if (labelName == "Department")
										m_collectionNameIdMap.insert(make_pair(attName, attId));
									if (lableText == "shape")
										m_shapeNameIdMap.insert(make_pair(attName, attId));
									if (lableText == "theme")
										m_themeNameIdMap.insert(make_pair(attName, attId));

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
					for (int itemIndex = 0; itemIndex < m_createProductTreeWidget_2->topLevelItemCount(); ++itemIndex)
					{
						QTreeWidgetItem* topItem = m_createProductTreeWidget_2->topLevelItem(itemIndex);
						QWidget* qWidgetColumn_0 = m_createProductTreeWidget_2->itemWidget(topItem, 0);
						QWidget* qWidgetColumn_1 = m_createProductTreeWidget_2->itemWidget(topItem, 1);
						if (!qWidgetColumn_0 || !qWidgetColumn_1)
						{
							continue;
						}
						QLabel* qlabel = qobject_cast<QLabel*>(qWidgetColumn_0);
						QString lableText = qlabel->property(ATTRIBUTE_KEY.c_str()).toString();

						Logger::Debug("PublishToPLMData -> OnHandleDropDownValue() Label text: " + (lableText.toStdString()));
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
									dependentFieldJson = Helper::makeRestcallGet(RESTAPI::SHAPE_API, "?skip=0&limit=100", "?shape_seasons=" + id, progressbarText);

								}
								else if (lableText == "theme")
								{
									progressbarText = "Loading Theme details..";
									dependentFieldJson = Helper::makeRestcallGet(RESTAPI::THEME_API, "?skip=0&limit=100", "?theme_seasons=" + id, progressbarText);

								}
								else
								{
									dependentFieldJson = Helper::makeRestcallGet(apiUrl, "/hierarchy?skip=0&limit=100", "/" + id, progressbarText);
								}

								json attJson = json::object();

								for (int i = 0; i < dependentFieldJson.size(); i++)
								{
									attJson = Helper::GetJSONParsedValue<int>(dependentFieldJson, i, false);;///use new method
									attName = Helper::GetJSONValue<string>(attJson, ATTRIBUTE_NAME, true);
									Logger::Debug("PublishToPLMData -> OnHandleDropDownValue() attName: " + attName);
									attId = Helper::GetJSONValue<string>(attJson, ATTRIBUTE_ID, true);
									Logger::Debug("PublishToPLMData -> OnHandleDropDownValue() attId: " + attId);
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
				clearDependentComboBox(labelName, m_createProductTreeWidget_1);
				clearDependentComboBox(labelName, m_createProductTreeWidget_2);
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

	json CreateProduct::ReadVisualUIFieldValue(QTreeWidget* _searchTree, int _rowIndex)
	{
		Logger::Debug("Create product ReadVisualUIFieldValue() Start");
		string fieldValue = "";
		string attInternalName = "";
		string labelText = "";
		string attKey = ATTRIBUTE_KEY;
		string attType = "";
		string id;

		//string data;
		map<string, string>::iterator it;
		map < string, string> nameIdMap;
		json attJson = json::object();
		QTreeWidgetItem* topItem = _searchTree->topLevelItem(_rowIndex);
		string fieldVal;
		QWidget* qWidgetColumn0 = _searchTree->itemWidget(topItem, 0);
		QLabel* qlabel = qobject_cast<QLabel*>(qWidgetColumn0);
		labelText = qlabel->text().toStdString();
		attInternalName = qlabel->property(attKey.c_str()).toString().toStdString();
		attType = qlabel->property(ATTRIBUTE_TYPE_KEY.c_str()).toString().toStdString();
		QWidget* qWidgetColumn1 = _searchTree->itemWidget(topItem, 1);

		if (QLineEdit* qLineEditC1 = qobject_cast<QLineEdit*>(qWidgetColumn1))
		{
			fieldValue = qLineEditC1->text().toStdString();
			//fieldLabel = qLineEditC1->property("InternalName").toString().toStdString();
			Logger::Debug("Create product ReadVisualUIFieldValue() QLineEdit->fieldValue" + fieldValue);
			Logger::Debug("Create product ReadVisualUIFieldValue() QLineEdit->fieldLabel" + attInternalName);
			Logger::Debug("Create product ReadVisualUIFieldValue() QLineEdit->labelText" + labelText);

			fieldVal = qLineEditC1->property(fieldValue.c_str()).toString().toStdString();
			Logger::Debug("Create product ReadVisualUIFieldValue() QLineEdit->fieldVal" + fieldVal);
			if (!fieldVal.empty())
				fieldValue = fieldVal;
			/*if (labelText == "Style")
				_data += "\n\"" + fieldLabel + "\":\"" + fieldValue + "\",";*/
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
				//UTILITY_API->DisplayMessageBox("fieldValue::" + fieldValue);
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

			Logger::Debug("Create product ReadVisualUIFieldValue() QComboBox->fieldLabel" + attInternalName);
			Logger::Debug("Create product ReadVisualUIFieldValue() QComboBox->labelText" + labelText);
			if (attInternalName == "parent_season")
				nameIdMap = m_seasonNameIdMap;
			else if (attInternalName == "category_1")
				nameIdMap = m_brandNameIdMap;
			else if (attInternalName == "category_2")
				nameIdMap = m_dipartmentNameIdMap;
			else if (attInternalName == "collection")
				nameIdMap = m_collectionNameIdMap;
			else if (attInternalName == "shape")
				nameIdMap = m_shapeNameIdMap;
			else if (attInternalName == "theme")
				nameIdMap = m_themeNameIdMap;
			else if (attInternalName == "bom_template")
				m_bomTemplateName->setText(QString::fromStdString(fieldValue));

			if (attInternalName == "parent_season" || attInternalName == "category_1" || attInternalName == "category_2" || attInternalName == "collection" || attInternalName == "shape" || attInternalName == "theme")
			{
				it = nameIdMap.find(fieldValue);//
				if (it != nameIdMap.end())
				{
					fieldValue = it->second;
				}
			}
			else
			{
				fieldVal = qComboBoxC1->property(fieldValue.c_str()).toString().toStdString();
				Logger::Debug("Create product ReadVisualUIFieldValue() QComboBox->fieldVal" + fieldVal);
				if (!fieldVal.empty())
				{
					fieldValue = fieldVal;
				}
			}
			Logger::Debug("Create product ReadVisualUIFieldValue() QComboBox->fieldValue" + fieldValue);
		}

		if (!FormatHelper::HasContent(fieldValue))
		{
			fieldValue = BLANK;
		}
		attJson[ATTRIBUTE_KEY] = attInternalName;
		attJson[ATTRIBUTE_VALUE_KEY] = fieldValue;
		//data += "\n\"" + fieldLabel + "\":\"" + fieldValue + "\",";
		//_attJson[fieldLabel] = fieldValue;
		//	attJson[ATTRIBUTE_VALUE_KEY] = fieldValue;

		Logger::Debug("Create product ReadVisualUIFieldValue() End");
		return attJson;
	}

	void CreateProduct::ClearAllFields(QTreeWidget* _documentPushToPLMTree)
	{
		Logger::Debug("CreateProduct -> ClearAllFields() -> Start");
		try
		{
			for (int itemIndex = 0; itemIndex < _documentPushToPLMTree->topLevelItemCount(); ++itemIndex)
			{
				QTreeWidgetItem* topItem = _documentPushToPLMTree->topLevelItem(itemIndex);
				QWidget* qWidgetColumn0 = _documentPushToPLMTree->itemWidget(topItem, 0);
				QWidget* qWidgetColumn1 = _documentPushToPLMTree->itemWidget(topItem, 1);
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

				if (qlabel)
				{
					QString labelId = qlabel->property("Id").toString();
				}
				if (qlineedit)
				{
					QString defaultValue = qlineedit->property(ATTRIBUTE_INITIAL_VALUE.c_str()).toString();
					if (defaultValue.isEmpty())
						defaultValue = "";
					qlineedit->setText(defaultValue);
				}
				else if (qComboBoxItem)
				{

					QString defaultValue = qComboBoxItem->property(ATTRIBUTE_INITIAL_VALUE.c_str()).toString();
					int valueIndex = qComboBoxItem->findText(defaultValue);

					if (valueIndex == -1) // -1 for not found
					{
						valueIndex = qComboBoxItem->findText(QString::fromStdString(BLANK));
					}

					qComboBoxItem->setCurrentIndex(valueIndex);
				}
				else if (qComboBox)
				{
					QString defaultValue = qComboBox->property(ATTRIBUTE_INITIAL_VALUE.c_str()).toString();
					int valueIndex = qComboBox->findText(defaultValue);

					if (valueIndex == -1) // -1 for not found
					{
						valueIndex = qComboBox->findText(QString::fromStdString(BLANK));
					}

					qComboBox->setCurrentIndex(valueIndex);


				}
				else if (qDateEdit)
				{

					string defaultValue = qDateEdit->property(ATTRIBUTE_INITIAL_VALUE.c_str()).toString().toStdString();
					if (defaultValue.find(DATE_FORMAT_TEXT.toStdString()) != string::npos)
						defaultValue = "";

					if (FormatHelper::HasContent(defaultValue))
					{
						int day, month, year;
						sscanf(defaultValue.c_str(), "%4d-%2d-%2d", &year, &month, &day);
						QDate defaultQDate;
						defaultQDate.setDate(year, month, day);
						qDateEdit->setDate(defaultQDate);
					}
					else
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
				}
				else if (qspinBox)
				{

					QString defaultValue = qspinBox->property(ATTRIBUTE_INITIAL_VALUE.c_str()).toString();
					if (!defaultValue.isEmpty())
					{
						qspinBox->setValue(stoi(defaultValue.toStdString()));
					}
				}
			}
		}
		catch (string msg)
		{
			UTILITY_API->DeleteProgressBar(true);
			Logger::Error("ERROR::CreateProduct: ClearAllFields()-> Exception string:: " + msg);
			this->show();
		}
		catch (exception& e)
		{
			UTILITY_API->DeleteProgressBar(true);
			Logger::Error("ERROR::CreateProduct: ClearAllFields()-> Exception e:: " + string(e.what()));
			this->show();
		}
		catch (const char* msg)
		{
			UTILITY_API->DeleteProgressBar(true);
			Logger::Error("ERROR::CreateProduct: ClearAllFields()-> Exception char:: " + string(msg));
			wstring wstr(msg, msg + strlen(msg));
			this->show();
		}
		Logger::Debug("CreateProduct -> ClearAllFields() -> End");
	}

	//void CreateProduct::SetColorwayDetails(QStringList _downloadIdList, json _jsonarray);
	//{
	//	Logger::Debug("CreateProduct -> SetDownloadedColorwayDetails() -> Start");
	//	json jsonvalue = json::object();
	//	string columnsNames = DirectoryUtil::GetPLMPluginDirectory() + "CreateColorwayDetail.json";//Reading Columns from json
	//	jsonvalue = Helper::ReadJSONFile(columnsNames);
	//	AddColorwayDetails(jsonvalue, _downloadIdList);
	//	Logger::Debug("CreateProduct -> SetDownloadedColorwayDetails() -> End");
	//}

	void CreateProduct::ExtractColorwayDetails(string _productId)
	{
		Logger::Debug("CreateProduct -> ExtractColorwayDetails () Start");

		string twoDigiCode;
		int rowCount = ui_colorwayTable->rowCount();
		try
		{
			Logger::Debug("CreateProduct -> ExtractColorwayDetails () 1");
			for (int row = 0; row < rowCount; row++)
			{
				//QComboBox* colorwayNameCombo = qobject_cast<QComboBox*>(ui_colorwayTable->cellWidget(row, 1));
				QComboBox *colorwayNameCombo = static_cast<QComboBox*>(ui_colorwayTable->cellWidget(row, CLO_COLORWAY_COLUMN)->children().last());
				//QLineEdit* colorwayName1 = qobject_cast<QLineEdit*>(ui_colorwayTable->cellWidget(row, 6));
				QComboBox *colorwayName1 = static_cast<QComboBox*>(ui_colorwayTable->cellWidget(row, UNI_2_DIGIT_CODE_COLUMN)->children().last());
				//QTextEdit* description = qobject_cast<QTextEdit*>(ui_colorwayTable->cellWidget(row, 7));
				QTextEdit *description = static_cast<QTextEdit*>(ui_colorwayTable->cellWidget(row, DESCRIPTION_COLUMN)->children().last());
				QLineEdit *plmColorwayName = static_cast<QLineEdit*>(ui_colorwayTable->cellWidget(row, PLM_COLORWAY_COLUMN)->children().last());
				if (colorwayNameCombo)
				{
					Logger::Debug("CreateProduct -> ExtractColorwayDetails () 2");
					QString id = colorwayNameCombo->property("Id").toString();
					vector<pair<string, string>> headerNameAndValueList;
					json request = json::object();

					/*if (!colorwayName1->currentText().toStdString().empty())
					{*/
					if (m_2DigiCodeActive)
					{
						if (!colorwayName1->currentText().toStdString().empty())
						{
							twoDigiCode = colorwayName1->property(colorwayName1->currentText().toStdString().c_str()).toString().toStdString();
							request["uni_2_digit_code"] = twoDigiCode;
						}
					}
					request["color_specification"] = id.toStdString();
					request["description"] = description->toPlainText().toStdString();
					request["node_name"] = plmColorwayName->text().toStdString();

					//UTILITY_API->DisplayMessageBox("colorwayName" + colorwayName);
					Logger::Debug("CreateProduct -> ExtractColorwayDetails () 3");
					headerNameAndValueList.push_back(make_pair("content-Type", "application/json"));
					headerNameAndValueList.push_back(make_pair("Cookie", Configuration::GetInstance()->GetBearerToken()));

					string requestString = to_string(request);
					//UTILITY_API->DisplayMessageBox("requestString" + requestString);
					string response = REST_API->CallRESTPost(Configuration::GetInstance()->GetPLMServerURL() + RESTAPI::STYLE_ENDPOINT_API + "/" + _productId + "/product_colors", &requestString, headerNameAndValueList, "Creating Colorways");
					//UTILITY_API->DisplayMessageBox("response" + response);
					if (FormatHelper::HasError(response))
					{
						RESTAPI::SetProgressBarData(0, "", false);
						//Logger::Debug("PublishToPLMData -> onPublishToPLMClicked 1");
						throw runtime_error(response);
					}
					int colorwayCount = UTILITY_API->GetColorwayCount();
					QString selectedCLOColorwayName = colorwayNameCombo->currentText();
					Logger::Debug("CreateProduct -> ExtractColorwayDetails () 4");
					for (int count = 0; count < colorwayCount; count++)
					{
						string colorwayName = UTILITY_API->GetColorwayName(count);
						if (selectedCLOColorwayName.toStdString() == colorwayName)
						{
							UTILITY_API->SetColorwayName(count, plmColorwayName->text().toStdString());
						}
					}

					json detailJson = Helper::GetJsonFromResponse(response, "{");
					string colorwayId = Helper::GetJSONValue<string>(detailJson, ATTRIBUTE_ID, true);
					Logger::Debug("CreateProduct -> ExtractColorwayDetails () 5");
					//ColorwayViews colorwayView;
					QString selectedColorway = colorwayNameCombo->currentText();

					if (!selectedColorway.isEmpty())
					{
						auto it = CreateImageIntent::GetInstance()->m_ColorwayViewMap.find(selectedColorway.toStdString());// Searching unit in map

						if (it != CreateImageIntent::GetInstance()->m_ColorwayViewMap.end())
						{
							it->second.colorwayId = colorwayId;
							//UTILITY_API->DisplayMessageBox("ExtractColorwayDetails it->first: " + it->first + "  Colorway id: " + colorwayId);
						}
					}
				}
			}
		}

		catch (string msg)
		{
			Logger::Error("CreateProduct -> ExtractColorwayDetails() Exception - " + msg);
		}
		catch (exception & e)
		{
			Logger::Error("CreateProduct -> ExtractColorwayDetails() Exception - " + string(e.what()));
		}
		catch (const char* msg)
		{
			wstring wstr(msg, msg + strlen(msg));
			Logger::Error("CreateProduct -> ExtractColorwayDetails() Exception - " + string(msg));
		}
		Logger::Debug("CreateProduct -> ExtractColorwayDetails() End");

	}

	void CreateProduct::onAddImageIntentClicked()
	{
		this->hide();
		CreateImageIntent::GetInstance()->m_imageQueueTableRowCount = 0;
		CreateImageIntent::GetInstance()->m_imageQueueTable->setRowCount(0);
		CreateImageIntent::GetInstance()->m_imageQueueTable->setColumnCount(0);
		CreateImageIntent::GetInstance()->m_imageQueueTable->clear();

		CreateImageIntent::GetInstance()->setModal(true);
		CreateImageIntent::GetInstance()->exec();

	}

	void  CreateProduct::SetTotalImageCount()
	{
		Logger::Debug("CreateProduct -> SetTotalImageCount () Start");
		QString label = "Total Count: " + QString::fromStdString(to_string((m_ImageIntentList->count())));
		m_totalCountLabel->setText(label);
		m_ImageIntentList->sortItems(Qt::SortOrder::AscendingOrder);
		Logger::Debug("CreateProduct -> SetTotalImageCount () End");
	}

	void CreateProduct::uploadColorwayImages()
	{
		Logger::Debug("CreateProduct -> uploadColorwayImages () Start");
		string temporaryPath = UTILITY_API->GetCLOTemporaryFolderPath();
		if (CreateImageIntent::GetInstance()->m_ColorwayViewMap.size() > 0)
		{
			for (auto colorwayIterator = CreateImageIntent::GetInstance()->m_ColorwayViewMap.begin(); colorwayIterator != CreateImageIntent::GetInstance()->m_ColorwayViewMap.end(); colorwayIterator++)
			{
				string str = colorwayIterator->first;
				//UTILITY_API->DisplayMessageBox("colorwayIterator->second.viewUploadId size" + to_string(colorwayIterator->second.viewUploadId->size()));
				for (int i = 0; i < 4; i++)
				{
					if (colorwayIterator->second.viewUploadId[i].compare("") != 0)
					{
						string filepath;
						string postField;
							if (colorwayIterator->second.includeAvatar[i] == 1)
							{
								filepath = temporaryPath + "CLOViseTurntableImages/WithAvatar/Avatar_" + str + "_" + to_string(i) + ".png";
								 postField = getPublishRequestParameter(filepath, "Avatar_" + str + "_" + to_string(i) + ".png");
							}
							else
							{
								filepath = temporaryPath + "CLOViseTurntableImages/WithoutAvatar/" + str + "_" + to_string(i) + ".png";
								 postField = getPublishRequestParameter(filepath, str + "_" + to_string(i) + ".png");
							}

						string resultJsonString;


						resultJsonString = RESTAPI::PostRestCall(postField, Configuration::GetInstance()->GetPLMServerURL() + RESTAPI::UPLOAD_IMAGE_API, "content-type: multipart/form-data; boundary=----WebKitFormBoundary7MA4YWxkTrZu0gW");


						Logger::Debug("resultJsonString" + resultJsonString);
						//Logger::RestAPIDebug("uploadColorwayImages" + resultJsonString);
						//UTILITY_API->DisplayMessageBox(resultJsonString);
						json detailJson = Helper::GetJsonFromResponse(resultJsonString, "{");
						//UTILITY_API->DisplayMessageBox(to_string(detailJson));
						Logger::Debug("detailJson" + to_string(detailJson));

						string imageId = Helper::GetJSONValue<string>(detailJson, ATTRIBUTE_ID, true);

						colorwayIterator->second.viewUploadId[i] = imageId;
						//UTILITY_API->DisplayMessageBox("imageId: " + imageId);
					}
				}

			}
		}
		Logger::Debug("CreateProduct -> uploadColorwayImages () End");
	}

	void CreateProduct::LinkImagesToColorways(string _productId)
	{
		Logger::Debug("CreateProduct -> LinkImagesToColorways () Start");
		if (CreateImageIntent::GetInstance()->m_ColorwayViewMap.size() > 0)
		{
			for (auto colorwayIterator = CreateImageIntent::GetInstance()->m_ColorwayViewMap.begin(); colorwayIterator != CreateImageIntent::GetInstance()->m_ColorwayViewMap.end(); colorwayIterator++)
			{
				string str = colorwayIterator->first;
				Logger::Debug("CreateProduct -> LinkImagesToColorways() -> str" + str);

				//string data = "{\n\"images\":{\n\"\":\"C397197\",\n\"1\":\"C397197\",\n\"2\":\"C397222\",\n\"3\":\"C397226\"\n}\n}";
				string data = "{\n\"images\":{";

				if (colorwayIterator->second.defaultImage != -1)
				{
					if (!colorwayIterator->second.viewUploadId[colorwayIterator->second.defaultImage].empty())
						data += "\n\"\":\"" + colorwayIterator->second.viewUploadId[colorwayIterator->second.defaultImage] + "\",";
				}

				for (int i = 0; i < 4; i++)
				{
					if (!colorwayIterator->second.viewUploadId[i].empty())
					{
						Logger::Debug("CreateProduct -> LinkImagesToColorways () viewUploadId[i]" + colorwayIterator->second.viewUploadId[i]);
						data += "\n\"" + to_string(i + 1) + "\":\"" + colorwayIterator->second.viewUploadId[i] + "\",";
						for (int labelIterator = 0; labelIterator < colorwayIterator->second.viewLabelMap[i].size(); labelIterator++)
						{

							auto searchColorwayImageLabels = m_colorwayImageLabelsMap.find(colorwayIterator->second.viewLabelMap[i][labelIterator]);
							if (searchColorwayImageLabels != m_colorwayImageLabelsMap.end())
							{
								data += "\n\"" + searchColorwayImageLabels->second.toStdString() + "\":\"" + colorwayIterator->second.viewUploadId[i] + "\",";
							}
							else
							{
								auto  searchStyleImageLabels = m_styleImageLabelsMap.find(colorwayIterator->second.viewLabelMap[i][labelIterator]);
								if (searchStyleImageLabels != m_styleImageLabelsMap.end())
								{
									data += "\n\"" + searchStyleImageLabels->second.toStdString() + "\":\"" + colorwayIterator->second.viewUploadId[i] + "\",";
								}
							}
						}

					}
				}
				data = data.substr(0, data.length() - 1);

				data += "\n}\n}";

				Logger::Debug("CreateProduct -> LinkImagesToColorways () data" + data);
				if (str.compare("No_Colorway_Default") == 0)
				{
					//Link images to style .....
					string resultJsonString = RESTAPI::PutRestCall(data, Configuration::GetInstance()->GetPLMServerURL() + RESTAPI::STYLE_ENDPOINT_API + "/" + _productId, "content-type: application/json");

				}
				else
				{
					if (!colorwayIterator->second.colorwayId.empty())
					{
						Logger::Debug("CreateProduct -> LinkImagesToColorways () colorwayIterator->second.colorwayId" + colorwayIterator->second.colorwayId);
						//Link images to colorways ....
						string resultJsonString = RESTAPI::PutRestCall(data, Configuration::GetInstance()->GetPLMServerURL() + RESTAPI::COLORWAY_API + "/" + colorwayIterator->second.colorwayId, "content-type: application/json");

					}
				}
			}
		}
		Logger::Debug("CreateProduct -> LinkImagesToColorways () End");
	}

	void CreateProduct::clearDependentComboBox(QString _lable, QTreeWidget* _searchTree)
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
			if (qlabel)
				lableText = qlabel->property("attName").toString();
			Logger::Debug("CreateProduct -> clearDependentComboBox () lableText: " + lableText.toStdString());
			QComboBox* qComboBox = qobject_cast<QComboBox*>(qWidgetColumn_1);
			if (_lable == "Season")
			{
				if (lableText == "Brand" || lableText == "Division" || lableText == "Department" || lableText == "Item Code" || lableText == "Planning Sum")
				{
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


	void CreateProduct::UploadStyleThumbnail(string _productId)
	{

		string path;
		string _3DModelFilePath = UTILITY_API->GetProjectFilePath();
		path = _3DModelFilePath;
		//UTILITY_API->DisplayMessageBox("_3DModelFilePath" + _3DModelFilePath);
		Helper::EraseSubString(path, UTILITY_API->GetProjectName());
		m_3DModelThumbnailName = UTILITY_API->GetProjectName() + ".png";
		//UTILITY_API->DisplayMessageBox("m_3DModelThumbnailName: " + m_3DModelThumbnailName);
		m_3DModelThumbnailPath = path + m_3DModelThumbnailName;
		//UTILITY_API->DisplayMessageBox("m_3DModelThumbnailPath: " + m_3DModelThumbnailPath);
		string postField = getPublishRequestParameter(m_3DModelThumbnailPath, m_3DModelThumbnailName);
		//UTILITY_API->DisplayMessageBox("postField:  " + postField);
		string resultJsonString = RESTAPI::PostRestCall(postField, Configuration::GetInstance()->GetPLMServerURL() + RESTAPI::UPLOAD_IMAGE_API, "content-type: multipart/form-data; boundary=----WebKitFormBoundary7MA4YWxkTrZu0gW");
		//UTILITY_API->DisplayMessageBox("thumbnail uploaded:  " + resultJsonString);
		json detailJson = Helper::GetJsonFromResponse(resultJsonString, "{");
		//UTILITY_API->DisplayMessageBox(to_string(detailJson));

		string imageId = Helper::GetJSONValue<string>(detailJson, ATTRIBUTE_ID, true);
		string data = "{\n\"images\":{\n\"\":\"" + imageId + "\",\n\"1\":\"" + imageId + "\"\n}\n}";
		//UTILITY_API->DisplayMessageBox(data);
		resultJsonString = RESTAPI::PutRestCall(data, Configuration::GetInstance()->GetPLMServerURL() + RESTAPI::STYLE_ENDPOINT_API + "/" + _productId, "content-type: application/json");
		//UTILITY_API->DisplayMessageBox("ImageLinked " + resultJsonString);
	}

	void CreateProduct::reject()
	{
		Logger::Debug("CreateProduct -> reject -> Start");

		ui_tabWidget->setStyleSheet(" QTabWidget::pane { border: none; color: #FFFFFF; font-size: 10px; background-color: #262628; }""QTabBar::tab { width: 100px; padding: 2px; }""QTabBar::tab:selected { border: none; color: #FFFFFF; background-color: \"" + DEFAULT_TAB_BG_COLOR + "\"; }""QTabBar::tab:!selected { color:#FFFFFF; background-color:\"" + SELECTED_TAB_BG_COLOR + "\"; }");
		if (!PublishToPLMData::GetInstance()->m_isSaveClicked)
		{
			ClearAllFields(m_createProductTreeWidget_1);
			ClearAllFields(m_createProductTreeWidget_2);
			ui_colorwayTable->clear();
			ui_colorwayTable->clearContents();
			ui_colorwayTable->setRowCount(0);
			m_colorwayRowcount = 0;
			ui_colorwayTable->hide();
			CreateImageIntent::GetInstance()->ClearAllFields();
			m_ImageIntentList->clear();
			m_colorSpecList.clear();
			ui_tabWidget->setCurrentIndex(OVERVIEW_TAB);
			m_totalCountLabel->setText("Total count: 0");
			m_CloAndPLMColorwayMap.clear();
			ClearBOMData();

			//ui_sectionLayout->removeWidget();


		}
		Configuration::GetInstance()->SetIsPrintSearchClicked(false);
		this->accept();
		/*if (!m_isSaveClicked)*/
			//CreateProduct::Destroy();
		Logger::Debug("CreateProduct -> reject -> End");
	}

	void CreateProduct::OnClickAddColorButton(int i)
	{
		Logger::Debug("CreateProduct -> OnClickUpdateColorButton () Start");
		Configuration::GetInstance()->SetIsUpdateColorClicked(true);
		m_selectedRow = i;
		m_currentColorSpec = BLANK;
		this->hide();

		if (ui_colorwayTable->rowCount() != 0)
		{
			GetUpdatedColorwayNames();
		}

		//QWidget* widget = ui_colorwayTable->cellWidget(m_selectedRow, 1);
		QComboBox *colorwayNameCombo = static_cast<QComboBox*>(ui_colorwayTable->cellWidget(m_selectedRow, CLO_COLORWAY_COLUMN)->children().last());
		string colorSpecId = colorwayNameCombo->property("Id").toString().toStdString();
		if (!colorSpecId.empty())
			m_currentColorSpec = colorSpecId;
		ColorConfig::GetInstance()->m_mode = "Search";

		bool isFromConstructor = false;
		if (!ColorConfig::GetInstance()->GetIsModelExecuted())
		{
			ColorConfig::GetInstance()->InitializeColorData();
			isFromConstructor = true;
		}

		ColorConfig::GetInstance()->InitializeColorData();
		ColorConfig::GetInstance()->m_isSearchColor = true;
		PLMColorSearch::GetInstance()->setModal(true);
		PLMColorSearch::GetInstance()->DrawSearchWidget(true);
		UTILITY_API->DeleteProgressBar(true);
		ColorConfig::GetInstance()->SetIsModelExecuted(true);

		PLMColorSearch::GetInstance()->exec();
		RESTAPI::SetProgressBarData(0, "", false);

		Logger::Debug("CreateProduct -> OnClickUpdateColorButton () End");

	}

	bool CreateProduct::UpdateColorInColorways(QStringList _downloadIdList, json _jsonarray)
	{
		Logger::Debug("CreateProduct -> UpdateColorInColorways () Start");

		string objectName;
		string pantone;
		string rgbValue;
		string objectCode;
		string attId;
		json attachmentsJson;

		QPixmap pixmap;
		QImage styleIcon;

		int tabIndex = ui_tabWidget->currentIndex();

		if (tabIndex == COLORWAY_TAB)
		{
			if (!m_currentColorSpec.empty())
				m_colorSpecList.removeOne(QString::fromStdString(m_currentColorSpec));
		}
		
		for (int rowCount = 0; rowCount < _jsonarray.size(); rowCount++)
		{
			attachmentsJson = Helper::GetJSONParsedValue<int>(_jsonarray, rowCount, false);
			
			attId = Helper::GetJSONValue<string>(attachmentsJson, ATTRIBUTE_ID, true);
			if (_downloadIdList.contains(QString::fromStdString(attId)))
			{
				if (tabIndex == COLORWAY_TAB)
				{
					if (m_colorSpecList.contains(QString::fromStdString(attId)))
						return false;
					else
						m_colorSpecList.append(QString::fromStdString(attId));
				}
				attId = Helper::GetJSONValue<string>(attachmentsJson, ATTRIBUTE_ID, true);
				objectName = Helper::GetJSONValue<string>(attachmentsJson, ATTRIBUTE_NAME, true);
				pantone = Helper::GetJSONValue<string>(attachmentsJson, PANTONE_KEY, true);
				rgbValue = Helper::GetJSONValue<string>(attachmentsJson, RGB_VALUE_KEY, true);
				objectCode = Helper::GetJSONValue<string>(attachmentsJson, CODE_KEY, true);

				break;
			}
		}

		rgbValue = Helper::FindAndReplace(rgbValue, "(", "");
		rgbValue = Helper::FindAndReplace(rgbValue, ")", "");
		rgbValue = Helper::FindAndReplace(rgbValue, " ", "");

		QLabel* label = new QLabel();
		label->setToolTip(QString::fromStdString(objectName));
		QWidget *pWidget = nullptr;

		if (FormatHelper::HasContent(rgbValue))
		{
			QStringList listRGB;
			QString colorRGB = QString::fromStdString(rgbValue);
			listRGB = colorRGB.split(',');
			int red = listRGB.at(0).toInt();
			int green = listRGB.at(1).toInt();
			int blue = listRGB.at(2).toInt();
			QColor color(red, green, blue);
			QSize size;
			if (tabIndex == COLORWAY_TAB)
			{
				size.setHeight(60);
				size.setWidth(60);
			}
			if (tabIndex == BOM_TAB)
			{
				size.setHeight(20);
				size.setWidth(20);
			}
			QImage image(size, QImage::Format_ARGB32);
			image.fill(color);

			pixmap = QPixmap::fromImage(image);

		}
		else
		{
			string images = Helper::GetJSONValue<string>(attachmentsJson, "images", false);
			json imageIDJson = json::parse(images);
			string defaultImageID = Helper::GetJSONValue<string>(imageIDJson, "", true);

			string thumbnail = UIHelper::GetPrintThumbnailUrl(defaultImageID);
			
			if (FormatHelper::HasContent(thumbnail))
			{
				QByteArray imageBytes;
				auto startTime = std::chrono::high_resolution_clock::now();
				imageBytes = Helper::DownloadImageFromURL(thumbnail);
				auto finishTime = std::chrono::high_resolution_clock::now();
				std::chrono::duration<double> totalDuration = finishTime - startTime;
				Logger::perfomance(PERFOMANCE_KEY + "Download Thumnail API :: " + to_string(totalDuration.count()));

				QBuffer buffer(&imageBytes);
				buffer.open(QIODevice::ReadOnly);
				QImageReader imageReader(&buffer);
				imageReader.setDecideFormatFromContent(true);
				styleIcon = imageReader.read();
				if (styleIcon.isNull())
				{
					Logger::Logger("FabricsSearchDialog -> CreateResultTable() Image is not loaded.  ");
				}
				else
				{
					pixmap = QPixmap::fromImage(styleIcon);
				}
			}

			//if (pixmap.isNull())
			//{
			//	QImageReader imageReader(":/CLOVise/PLM/Images/NoImage.png");
			//	imageReader.setDecideFormatFromContent(true);
			//	styleIcon = imageReader.read();
			//	pixmap = QPixmap::fromImage(styleIcon);
			//}
		}

		if (tabIndex == COLORWAY_TAB)
			label->setPixmap(QPixmap(pixmap.scaled(60, 60, Qt::KeepAspectRatio)));
		if (tabIndex == BOM_TAB)
			label->setPixmap(QPixmap(pixmap.scaled(20, 20, Qt::KeepAspectRatio)));

		pWidget = CVWidgetGenerator::InsertWidgetInCenter(label);
		pWidget->setProperty("colorId", attId.c_str());

		if (tabIndex == COLORWAY_TAB)
		{
			if (!FormatHelper::HasContent(pantone))
				pantone = BLANK;
			ui_colorwayTable->item(m_selectedRow, COLOR_NAME_COLUMN)->setText(QString::fromStdString(objectName));
			ui_colorwayTable->item(m_selectedRow, COLOR_CODE_COLUMN)->setText(QString::fromStdString(objectCode));
			if (FormatHelper::HasContent(pantone))
				ui_colorwayTable->item(m_selectedRow, PANTONE_CODE_COLUMN)->setText(QString::fromStdString(pantone));
			else
				ui_colorwayTable->item(m_selectedRow, PANTONE_CODE_COLUMN)->setText(QString::fromStdString(BLANK));
			QComboBox *colorwayName1 = static_cast<QComboBox*>(ui_colorwayTable->cellWidget(m_selectedRow, CLO_COLORWAY_COLUMN)->children().last());
			colorwayName1->setProperty("Id", attId.c_str());
			QTableWidgetItem* iconItem = new QTableWidgetItem;
			QSize iconSize(40, 40);
			iconItem->setSizeHint(iconSize);
			QStringList listRGB;

			ui_colorwayTable->setCellWidget(m_selectedRow, COLOR_CHIP_COLUMN, pWidget);
		}
		if (tabIndex == BOM_TAB)
		{
			Logger::Debug("CreateProduct -> UpdateColorInColorways () 6");
			Logger::Debug("CreateProduct -> UpdateColorInColorways () AddNewBom::GetInstance()->m_currentTableName" + CreateProductBOMHandler::GetInstance()->m_currentTableName);
			CreateProductBOMHandler::GetInstance()->m_currentTableName;
			auto itr = CreateProductBOMHandler::GetInstance()->m_bomSectionTableInfoMap.find(CreateProductBOMHandler::GetInstance()->m_currentTableName);
			if (itr != CreateProductBOMHandler::GetInstance()->m_bomSectionTableInfoMap.end())
			{
				Logger::Debug("CreateProduct -> UpdateColorInColorways () AddNewBom::GetInstance()->m_currentRow" + to_string(CreateProductBOMHandler::GetInstance()->m_currentRow));
				QTableWidget* sectionTable = itr->second;
				if (QWidget* widget = sectionTable->cellWidget(CreateProductBOMHandler::GetInstance()->m_currentRow, CreateProductBOMHandler::GetInstance()->m_currentColumn))
				{

					QString columnName = sectionTable->horizontalHeaderItem(CreateProductBOMHandler::GetInstance()->m_currentColumn)->text();
					if (columnName == "Common Color")
					{
						QPushButton* pushButton = static_cast<QPushButton*>(sectionTable->cellWidget(CreateProductBOMHandler::GetInstance()->m_currentRow, 0)->children().last());;

						if (pushButton != nullptr)
						{

							Logger::Debug("CreateProduct -> UpdateColorInColorways () colorId" + attId);
							pushButton->setProperty("commonColorId", attId.c_str());
						}
					}
					else
					{
						widget->setProperty("colorId", attId.c_str());
					}
					Logger::Debug("CreateProduct -> UpdateColorInColorways () 8");
					if (QLayout* layout = widget->layout())
					{
						Logger::Debug("CreateProduct -> UpdateColorInColorways () 9");
						{
							auto gridLayout = dynamic_cast<QGridLayout*>(widget->layout());
							if (gridLayout != nullptr)
							{
								pWidget->setProperty("colorId", attId.c_str());
								gridLayout->addWidget(pWidget, 0, 0, 1, 1, Qt::AlignHCenter);
								Logger::Debug("CreateProduct -> UpdateColorInColorways () 10");
							}

						}
					}
				}

			}
		}


		Configuration::GetInstance()->SetIsUpdateColorClicked(false);
		Logger::Debug("CreateProduct -> UpdateColorInColorways () End");
		return true;
	}

	void CreateProduct::exportTurntableImages()
	{
		Logger::Debug("CreateProduct -> exportTurntableImages() -> Start ");

		int cloColorwaySelectedIndex = UTILITY_API->GetCurrentColorwayIndex();
		int colorwayCount = UTILITY_API->GetColorwayCount();
		Logger::Debug("CreateProduct -> exportTurntableImages() colorwayCount" + to_string(colorwayCount));

		int colorwayIndex = 0;

		string temporaryPath = UTILITY_API->GetCLOTemporaryFolderPath();

		Logger::Debug("CreateProduct->exportTurntableImages() temporaryPath" + temporaryPath);
		QStringList imageNameList;
		string filepath;
		for (colorwayIndex = 0; colorwayIndex < colorwayCount; colorwayIndex++)
		{
			string colorwayName = UTILITY_API->GetColorwayName(colorwayIndex);

			UTILITY_API->SetShowHideAvatar(true);
			UTILITY_API->SetCurrentColorwayIndex(colorwayIndex);
			filepath = temporaryPath + "CLOViseTurntableImages/WithAvatar/Avatar_" + colorwayName + ".png";
			EXPORT_API->ExportTurntableImages(filepath, 4, 480, 640);
			filepath.clear();
			UTILITY_API->SetShowHideAvatar(false);
			filepath = temporaryPath + "CLOViseTurntableImages/WithoutAvatar/" + colorwayName + ".png";
			EXPORT_API->ExportTurntableImages(filepath, 4, 480, 640);
		}

		filepath.clear();
		UTILITY_API->SetCurrentColorwayIndex(cloColorwaySelectedIndex);
		UTILITY_API->SetShowHideAvatar(true);
		filepath = temporaryPath + "CLOViseTurntableImages/WithAvatar/Avatar_No_Colorway_Default.png";
		EXPORT_API->ExportTurntableImages(filepath, 4, 480, 640);
		filepath.clear();
		UTILITY_API->SetShowHideAvatar(false);
		filepath = temporaryPath + "CLOViseTurntableImages/WithoutAvatar/No_Colorway_Default.png";
		EXPORT_API->ExportTurntableImages(filepath, 4, 480, 640);

		Logger::Debug("CreateProduct -> exportTurntableImages() -> End ");

	}
	void CreateProduct::onHorizontalHeaderClicked(int _column)
	{
		Logger::Info("CreateProduct -> horizontalHeaderClicked() -> Start");
		Logger::Debug("Column.." + to_string(_column));
		if (_column == DELETE_BUTTON_COLUMN || _column == ASSOCIATE_COLOR_COLUMN || _column == PLM_COLORWAY_COLUMN || _column == CLO_COLORWAY_COLUMN || _column == COLOR_CHIP_COLUMN || _column == UNI_2_DIGIT_CODE_COLUMN || _column == DESCRIPTION_COLUMN)
			ui_colorwayTable->setSortingEnabled(false);
		else
		{
			ui_colorwayTable->setSortingEnabled(true);
		}
		Logger::Info("CreateProduct -> horizontalHeaderClicked() -> End");
	}


	void CreateProduct::onAddNewBomClicked()
	{
		this->hide();
		AddNewBom::GetInstance()->setModal(true);
		AddNewBom::GetInstance()->exec();

	}

	void CreateProduct::AddMaterialInBom()
	{
		Logger::Debug("CreateProduct -> AddMaterialInBom() -> Start");
		CreateProductBOMHandler::GetInstance()->AddMaterialInBom();
		Logger::Debug("CreateProduct -> AddMaterialInBom() -> End");
		
	}


	void CreateProduct::GetMappedColorway()
	{
		Logger::Debug("CreateProduct -> GetMappedColorway() -> Start");
		m_mappedColorways.clear();
		for (int count = 0; count < ui_colorwayTable->rowCount(); count++)
		{
			QLineEdit * plmColorwayCombo = static_cast<QLineEdit*>(ui_colorwayTable->cellWidget(count, PLM_COLORWAY_COLUMN)->children().last());
			QComboBox * cloColorwayCombo = static_cast<QComboBox*>(ui_colorwayTable->cellWidget(count, CLO_COLORWAY_COLUMN)->children().last());
			Logger::Debug("CreateProduct -> GetMappedColorway() -> plmColorway:" + plmColorwayCombo->text().toStdString());
			Logger::Debug("CreateProduct -> GetMappedColorway() ->cloColorway:" + cloColorwayCombo->currentText().toStdString());
			QString plmColorwayName = plmColorwayCombo->text();
			QString cloColorwayName = cloColorwayCombo->currentText();
			if (!plmColorwayName.isEmpty() && !cloColorwayName.isEmpty())
			{
				m_CloAndPLMColorwayMap.insert(make_pair(plmColorwayName.toStdString(), cloColorwayName.toStdString()));
				m_mappedColorways.append(cloColorwayCombo->currentText());
				Logger::Debug("CreateProduct -> GetMappedColorway() -> mappedColorway:" + cloColorwayCombo->currentText().toStdString());
			}
		}
		Logger::Debug("CreateProduct -> GetMappedColorway() -> End");
	}

	void CreateProduct::UpdateColorwayColumnsInBom()
	{
		CreateProductBOMHandler::GetInstance()->UpdateColorwayColumnsInBom();
	}


	void CreateProduct::CreateBom(string _productId)
	{
		Logger::Debug("CreateProduct -> CreateBom() -> Start");
		vector<pair<string, string>> headerNameAndValueList;
		headerNameAndValueList.push_back(make_pair("content-Type", "application/json"));
		headerNameAndValueList.push_back(make_pair("Accept", "application/json"));
		headerNameAndValueList.push_back(make_pair("Cookie", Configuration::GetInstance()->GetBearerToken()));

		Logger::Debug("CreateProduct -> CreateBom() -> AddNewBom::GetInstance()->m_BomMetaData" + to_string(AddNewBom::GetInstance()->m_BOMMetaData));
		json bomData = AddNewBom::GetInstance()->m_BOMMetaData;
		string bomTemplateId = Helper::GetJSONValue<string>(bomData, "bom_template", true);
		bomData.erase("bom_template");
		bomData["style_id"] = _productId;
		string bomMetaData = to_string(bomData);
		//UTILITY_API->DisplayMessageBox(to_string(bomData));
		string response = REST_API->CallRESTPost(Configuration::GetInstance()->GetPLMServerURL() + RESTAPI::CREATE_BOM_API + "/" + bomTemplateId, &bomMetaData, headerNameAndValueList, "Loading");
		if (!FormatHelper::HasContent(response))
		{
			RESTAPI::SetProgressBarData(0, "", false);
			throw "Unable to publish to PLM. Please try again or Contact your System Administrator.";
		}

		if (FormatHelper::HasError(response))
		{
			RESTAPI::SetProgressBarData(0, "", false);
			//Logger::Debug("PublishToPLMData -> onPublishToPLMClicked 1");
			throw runtime_error(response);
		}
		Logger::Debug("CreateProduct -> CreateBom() -> response" + response);
		json bomJson = Helper::GetJsonFromResponse(response, "{");
		string bomLatestRevision = Helper::GetJSONValue<string>(bomJson, "latest_revision", true);

		if (FormatHelper::HasContent(bomLatestRevision))
		{
			string modifiedbyFlag = "{\"modified_by_application\":\"CLO3D\"}";
			Logger::Debug("AddNewBom -> CreateBom() -> modifiedbyFlag" + modifiedbyFlag);
			string resultJsonString = RESTAPI::PutRestCall(modifiedbyFlag, Configuration::GetInstance()->GetPLMServerURL() + RESTAPI::BOM_REVISION_API + "/" + bomLatestRevision, "content-type: application/json");
			Logger::Debug("CreateProduct -> CreateBom() -> resultJsonStringModifiedBy" + resultJsonString);
		}
		for (auto itr = CreateProductBOMHandler::GetInstance()->m_bomSectionTableInfoMap.begin(); itr != CreateProductBOMHandler::GetInstance()->m_bomSectionTableInfoMap.end(); itr++)
		{
			//AddNewBom::sectionInfo sectionInfoObj = itr->second;
			QTableWidget* sectionTable = itr->second;
			string sectionId = sectionTable->property("SectionId").toString().toStdString();
			for (int rowCount = 0; rowCount < sectionTable->rowCount(); rowCount++)
			{
				json attJson = json::object();
				string commonColorId;
				map<string, string> partMaterialColorsMap;
				for (int columnCount = 0; columnCount < sectionTable->columnCount(); columnCount++)
				{
					string fieldValue;

					QWidget* qcolumnWidget = (QWidget*)sectionTable->cellWidget(rowCount, columnCount)->children().last();
					string attInternalName = qcolumnWidget->property("rest_api_name").toString().toStdString();
					Logger::Debug("Create product CreateBom() attInternalName" + attInternalName);
					QString columnName = sectionTable->horizontalHeaderItem(columnCount)->text();
					Logger::Debug("Create product CreateBom() columnName" + columnName.toStdString());

					if (CreateProduct::GetInstance()->m_mappedColorways.contains(columnName))
					{
						Logger::Debug("Create product CreateBom() colorways1");

						//QWidget *widget = gridLayout->itemAt(i)->widget();
						//auto gridLayout = dynamic_cast<QGridLayout*>(qcolumnWidget->layout());
						//QWidget *widget = gridLayout->itemAtPosition(0,0)->widget();
						//attInternalName = columnName.toStdString();
						//fieldValue = widget->property("colorId").toString().toStdString();
						//Logger::Debug("Create product CreateBom() colorId" + fieldValue);

						if (QWidget* widget = sectionTable->cellWidget(rowCount, columnCount))// Half cooked code for part material color
						{

							string colorId, colorId2;
							string colorId1 = widget->property("colorId").toString().toStdString();


							Logger::Debug("Create product CreateBom() colorId1" + colorId1);
							Logger::Debug("CreateProduct -> CreateBom () 8");
							if (QLayout* layout = widget->layout())
							{
								Logger::Debug("CreateProduct -> CreateBom () 9");
								{
									auto gridLayout = dynamic_cast<QGridLayout*>(widget->layout());
									QWidget *childwidget = gridLayout->itemAtPosition(0, 0)->widget();
									//attInternalName = columnName.toStdString();
									colorId2 = childwidget->property("colorId").toString().toStdString();
									Logger::Debug("Create product CreateBom() colorId2" + colorId2);

								}
							}

							if (FormatHelper::HasContent(colorId1))
								colorId = colorId1;
							else
								colorId = colorId2;

							if (FormatHelper::HasContent(colorId))
								partMaterialColorsMap.insert(make_pair(columnName.toStdString(), colorId));
						}

					}
					else if (QLineEdit* qLineEditC1 = qobject_cast<QLineEdit*>(qcolumnWidget))
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
							commonColorId = pushButton->property("commonColorId").toString().toStdString();
							Logger::Debug("Create product CreateBom() QComboBox->materialId" + fieldValue);
							Logger::Debug("Create product CreateBom() QComboBox->commonColorId" + commonColorId);
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

						Logger::Debug("Create product CreateBom() QComboBox->fieldLabel" + attInternalName);
						//Logger::Debug("Create product ReadVisualUIFieldValue() QComboBox->labelText" + labelText);

						string fieldVal = qComboBoxC1->property(fieldValue.c_str()).toString().toStdString();
						Logger::Debug("Create product CreateBom() QComboBox->fieldVal" + fieldVal);
						if (!fieldVal.empty())
						{
							fieldValue = fieldVal;
						}
						Logger::Debug("Create product CreateBom() QComboBox->fieldValue" + fieldValue);
					}
					if (!attInternalName.empty() && !fieldValue.empty())
					{
						if (attInternalName == "qty_default")
							attJson[attInternalName] = atof(fieldValue.c_str());
						else
							attJson[attInternalName] = fieldValue;
					}
					Logger::Debug("Create product CreateBom() fieldValue" + fieldValue);
				}
				attJson["ds_section"] = sectionId;
				if (FormatHelper::HasContent(commonColorId))
					attJson["common_color"] = commonColorId;

				Logger::Debug("Create product CreateBom() attJson" + to_string(attJson));
				string materialId = Helper::GetJSONValue<string>(attJson, "actual", true);
				Logger::Debug("Create product CreateBom() materialId" + materialId);
				string materialType = Helper::GetJSONValue<string>(attJson, "Type", true);
				Logger::Debug("Create product CreateBom() materialType" + materialType);
				string materialName = Helper::GetJSONValue<string>(attJson, "material_name", true);
				Logger::Debug("Create product CreateBom() mayerialName" + materialName);
				if (FormatHelper::HasContent(bomLatestRevision))
				{
					string partMaterialResponse;
					if (FormatHelper::HasContent(materialId))
					{

						attJson.erase("Type");
						attJson.erase("material_name");
						attJson.erase("uom");
						string placementData = to_string(attJson);
						Logger::Debug("Create product CreateBom() placementData" + placementData);
						partMaterialResponse = RESTAPI::PostRestCall(placementData, Configuration::GetInstance()->GetPLMServerURL() + RESTAPI::BOM_REVISION_API_V3 + "/" + bomLatestRevision + "/items/part_materials", "content-type: application/json; boundary=----WebKitFormBoundary7MA4YWxkTrZu0gW");
						//response = REST_API->CallRESTPost(Configuration::GetInstance()->GetPLMServerURL() + RESTAPI::BOM_REVISION_API_V3 + "/" + bomLatestRevision + "/items/part_materials", &placementData, headerNameAndValueList, "Loading");
						Logger::Debug("Create product CreateBom() response material" + partMaterialResponse);

					}
					else
					{
						attJson.erase("Type");
						attJson.erase("material_name");
						attJson.erase("actual");
						attJson.erase("uom");
						string placementData = to_string(attJson);

						string queryParam;
						if (FormatHelper::HasContent(materialType))
							queryParam = queryParam + "material_type=" + materialType;
						if (FormatHelper::HasContent(queryParam))
							queryParam = "?" + queryParam;
						if (FormatHelper::HasContent(materialName))
						{
							materialName = QString::fromStdString(materialName).replace(" ", "%20").toStdString();
							queryParam = queryParam + "&material_name=" + materialName;
						}
						string api = Configuration::GetInstance()->GetPLMServerURL() + RESTAPI::BOM_REVISION_API_V3 + "/" + bomLatestRevision + "/items/special_part_materials" + queryParam;
						Logger::Debug("Create product CreateBom() queryParam" + queryParam);
						Logger::Debug("Create product CreateBom() placementData" + placementData);
						Logger::Debug("Create product CreateBom() queryParam" + api);

						partMaterialResponse = RESTAPI::PostRestCall(placementData, api, "content-type: application/json; boundary=----WebKitFormBoundary7MA4YWxkTrZu0gW");
						//response = REST_API->CallRESTPost(api, &placementData, headerNameAndValueList, "Loading");
						Logger::Debug("Create product CreateBom() response Special" + partMaterialResponse);

					}

					json detailJson = Helper::GetJsonFromResponse(partMaterialResponse, "{");

					Logger::Debug("Create product CreateBom() detailJson" + to_string(detailJson));

					string partMaterialId = Helper::GetJSONValue<string>(detailJson, ATTRIBUTE_ID, true);
					Logger::Debug("Create product CreateBom() partMaterialId" + partMaterialId);
					if (FormatHelper::HasContent(partMaterialId) && partMaterialColorsMap.size())
					{
						json partMaterailColorJson = Helper::GetJSONParsedValue<string>(detailJson, "part_material_colors", false);
						//	json partMaterailColorJson = Helper::GetJSONParsedValue<string>(partMaterialResponse, "part_material_colors", false);
						string partMaterailColorIds;
						for (int partMaterailColorCount = 0; partMaterailColorCount < partMaterailColorJson.size(); partMaterailColorCount++)
						{
							string partMaterailColorId = Helper::GetJSONValue<int>(partMaterailColorJson, partMaterailColorCount, true);
							Logger::Debug("CreateProduct -> CreateBom() -> apiMetadataStr" + sectionId);
							partMaterailColorIds += "id=" + partMaterailColorId + "&";
						}
						partMaterailColorIds = partMaterailColorIds.substr(0, partMaterailColorIds.length() - 1);

						string partMaterialDefinitions = RESTAPI::CentricRestCallGet(Configuration::GetInstance()->GetPLMServerURL() + RESTAPI::PART_MATERIAL_COLOR_API + "?" + partMaterailColorIds + "&limit=1000", APPLICATION_JSON_TYPE, "");
						Logger::Debug("CreateProduct -> CreateBom() -> partMaterialDefinitions" + partMaterialDefinitions);

						json partMaterialDefinitionsJson = json::parse(partMaterialDefinitions);

						for (int partMaterailColorCount = 0; partMaterailColorCount < partMaterialDefinitionsJson.size(); partMaterailColorCount++)
						{
							Logger::Debug("CreateProduct -> CreateBom() -> 1");
							json partMaterailColor = Helper::GetJSONParsedValue<int>(partMaterialDefinitionsJson, partMaterailColorCount, false);;///use new method
							Logger::Debug("CreateProduct -> CreateBom() -> 1");
							Logger::Debug("CreateProduct -> CreateBom() -> sectionCountJson" + to_string(partMaterailColor));
							string partMaterailColorId = Helper::GetJSONValue<string>(partMaterailColor, ATTRIBUTE_ID, true);
							Logger::Debug("CreateProduct -> CreateBom() -> sectionId" + partMaterailColorId);
							string partMaterailColorName = Helper::GetJSONValue<string>(partMaterailColor, "node_name", true);
							Logger::Debug("CreateProduct -> CreateBom()-> partMaterailColorName" + partMaterailColorName);
							Logger::Debug("CreateProduct -> CreateBom()-> partMaterialColorsMap.size()" + partMaterialColorsMap.size());
							auto it = m_CloAndPLMColorwayMap.find(partMaterailColorName);
							if (it != m_CloAndPLMColorwayMap.end())
							{

								string cloColorwayName = it->second;
								auto itr = partMaterialColorsMap.find(cloColorwayName);
								if (itr != partMaterialColorsMap.end())
								{
									string colorid = itr->second;
									Logger::Debug("CreateProduct -> CreateBom()-> colorid" + colorid);
									if (FormatHelper::HasContent(colorid))
									{
										string data = "{\"pmc_color\":\"" + colorid + "\"}";
										Logger::Debug("AddNewBom -> CreateTableforEachSection() -> data" + data);
										string resultJsonString = RESTAPI::PutRestCall(data, Configuration::GetInstance()->GetPLMServerURL() + RESTAPI::PART_MATERIAL_COLOR_API + "/" + partMaterailColorId, "content-type: application/json");

									}
								}
							}

						}

					}

				}

				//UTILITY_API->DisplayMessageBox("attJson" + to_string(attJson));
			}



		}
		Logger::Debug("CreateProduct -> CreateBom() -> End");
	}
	
	void CreateProduct::SetUpdateBomFlag(bool _flag)
	{
		m_updateBomTab = _flag;
	}
	void CreateProduct::OnColorwaysTableDeleteButtonClicked(int _row)
	{
		Logger::Debug("CreateProduct -> onContextMenu_Clicked() -> Start");

		map<string, CreateImageIntent::ColorwayViews>::iterator it;

		bool isConfirmed = false;

		QMessageBox* deleteMessage = new QMessageBox(this);
#ifdef __APPLE__
		deleteMessage->setModal(true);
		deleteMessage->setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint | Qt::CustomizeWindowHint | Qt::WindowStaysOnTopHint);
		deleteMessage->setWindowModality(Qt::WindowModal);
#endif // __APPLE__
		deleteMessage->setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint | Qt::CustomizeWindowHint);
		deleteMessage->setStandardButtons(QMessageBox::Yes | QMessageBox::No);
		deleteMessage->setIcon(QMessageBox::Question);
		deleteMessage->setText("Deleting the colorway and associated Image Intents. Are you sure you want to proceed? ");

		if (deleteMessage->exec() != QMessageBox::Yes)
		{
			isConfirmed = false;
		}
		else
		{
			//QTableWidgetItem* item = ui_colorwayTable->itemAt(_row);
			//if (item)
			//{
				//int rowIndex = ui_colorwayTable->row(item);
			Logger::Debug("1" + to_string(_row));
			QComboBox *colorwayNameCombo = static_cast<QComboBox*>(ui_colorwayTable->cellWidget(_row, CLO_COLORWAY_COLUMN)->children().last());
			Logger::Debug("1");
			string colorSpecId = colorwayNameCombo->property("Id").toString().toStdString();
			Logger::Debug("1");
			QString plmColorwayName = colorwayNameCombo->currentText();
			Logger::Debug("1");
			if (!colorSpecId.empty())
			{
				m_colorSpecList.removeOne(QString::fromStdString(colorSpecId));
				it = CreateImageIntent::GetInstance()->m_ColorwayViewMap.find(plmColorwayName.toStdString());
				if (it != CreateImageIntent::GetInstance()->m_ColorwayViewMap.end())
					CreateImageIntent::GetInstance()->m_ColorwayViewMap.erase(plmColorwayName.toStdString());
			}
			ui_colorwayTable->removeRow(_row);
			m_colorwayRowcount--;
			if (m_buttonSignalMapper != nullptr)
			{
				for (int i = 0; i < ui_colorwayTable->rowCount(); i++)
				{
					QPushButton *colorButton = UIHelper::GetButtonWidgetFromCell(ui_colorwayTable, i, UPDATE_BTN_COLUMN, STARTING_INDEX);
					if (colorButton)
						m_buttonSignalMapper->setMapping(colorButton, i);
					QPushButton *CreateButton = UIHelper::GetButtonWidgetFromCell(ui_colorwayTable, i, UPDATE_BTN_COLUMN, FIRST_INDEX);
					if (m_createActionSignalMapper != nullptr)
					{
						if (CreateButton)
						{
							QList<QAction*> actions;
							actions = CreateButton->menu()->actions();
							m_createActionSignalMapper->setMapping(actions.at(STARTING_INDEX), i);
						}
					}
					if (m_printActionSignalMapper != nullptr)
					{
						if (CreateButton)
						{
							QList<QAction*> actions;
							actions = CreateButton->menu()->actions();
							m_createActionSignalMapper->setMapping(actions.at(FIRST_INDEX), i);
						}
					}
				}
			}

				if (m_colorwayDeleteSignalMapper != nullptr)
				{
					for (int index = 0; index < ui_colorwayTable->rowCount(); index++)
					{
						QPushButton *deleteButton = static_cast<QPushButton*>(ui_colorwayTable->cellWidget(index, COLORWAY_DELETE_COLUMN)->children().last());
						m_colorwayDeleteSignalMapper->setMapping(deleteButton, index);
						//QPushButton *editButton = static_cast<QPushButton*>(ui_colorwayTab->cellWidget(index, COLORWAYDELETE_COLUMN)->children().last());
						//m_colorwayDeleteSignalMapper->setMapping(editButton, index);
					}
				}

			QStringList selectedIndexs;
			int rowCount = m_ImageIntentList->count();
			for (int index = 0; index < rowCount; index++)
			{
				QListWidgetItem* listImageItem = new QListWidgetItem();
				QListWidgetItem* item = m_ImageIntentList->item(index);
				QListWidget *listItem = qobject_cast<QListWidget*>(m_ImageIntentList->itemWidget(item));
				for (int itemIndex = 0; itemIndex < listItem->count(); itemIndex++)
				{
					string text = listItem->item(itemIndex)->text().toStdString();
					Logger::Debug("CreateProduct -> onContextMenu_Clicked() -> Item" + text);
					if (itemIndex == 0)
					{
						int length = text.length();
						int indexOfColon = text.find(":");
						string colorwayName = text.substr(indexOfColon + 2, length);
						Logger::Debug("CreateProduct -> onContextMenu_Clicked() -> clorwayname" + colorwayName);
						Logger::Debug("CreateProduct -> onContextMenu_Clicked() -> clorwayname" + plmColorwayName.toStdString());
						if (colorwayName == plmColorwayName.toStdString())
						{
							Logger::Debug("CreateProduct -> onContextMenu_Clicked() Matched");
							selectedIndexs.append(QString::fromStdString(to_string(index)));
						}

					}
				}
			}
			for (int index = selectedIndexs.size() - 1; index >= 0; index--)
			{
				m_ImageIntentList->takeItem(selectedIndexs[index].toInt()); //deleting row from table, in reverse order

			}
			string currentViewName;
			for (int index = 0; index < 4; index++)
			{
				switch (index)
				{
				case BACK_VIEW:
					currentViewName = "Back";
					break;
				case FRONT_VIEW:
					currentViewName = "Front";
					break;
				case LEFT_VIEW:
					currentViewName = "Left";
					break;
				case RIGHT_VIEW:
					currentViewName = "Right";
					break;
				}
				if (CreateImageIntent::GetInstance()->m_colorwayViewQueue.contains(plmColorwayName + QString::fromStdString(currentViewName))) //deleting row from table, in reverse order
					CreateImageIntent::GetInstance()->m_colorwayViewQueue.removeAll(plmColorwayName + QString::fromStdString(currentViewName));
			}
			//}

			//else
			//{
				//UTILITY_API->DisplayMessageBox("Select Image To Delete");
			//}

		}
		SetTotalImageCount();
		for (int count = 0; count < ui_colorwayTable->rowCount(); count++)
		{
			QComboBox * digiCodeCombo = static_cast<QComboBox*>(ui_colorwayTable->cellWidget(count, UNI_2_DIGIT_CODE_COLUMN)->children().last());
			digiCodeCombo->setProperty("row", count);
		}

		Logger::Debug("CreateProduct -> onContextMenu_Clicked() -> End");
	}

	void CreateProduct::refreshImageIntents()
	{
		int imageRowCount = m_ImageIntentList->count();
		if (imageRowCount != 0)
		{
			UTILITY_API->CreateProgressBar();
			RESTAPI::SetProgressBarData(20, "Loading Latest Image Intents... ", true);
			UTILITY_API->SetProgress("Loading Latest Image Intents...", (qrand() % 101));
		}

		if (/*ValidateColorwayNameField() && */imageRowCount != 0)
		{
			exportTurntableImages();

			string colorwayName;
			string includeAvatar;
			string viewName;
			int view;
			Logger::Debug("CreateProduct -> onTabClicked() -> Item" + to_string(imageRowCount));

			string temporaryPath = UTILITY_API->GetCLOTemporaryFolderPath();

			for (int index = 0; index < imageRowCount; index++)
			{
				QListWidgetItem* listImageItem = new QListWidgetItem();
				QListWidgetItem* item = m_ImageIntentList->item(index);
				QListWidget *listItem = qobject_cast<QListWidget*>(m_ImageIntentList->itemWidget(item));
				for (int itemIndex = 0; itemIndex < listItem->count(); itemIndex++)
				{
					string text = listItem->item(itemIndex)->text().toStdString();
					Logger::Debug("CreateProduct -> onTabClicked() -> Item" + text);
					if (itemIndex == 0)
					{
						int length = text.length();
						int indexOfColon = text.find(":");
						colorwayName = text.substr(indexOfColon + 2, length);
						Logger::Debug("CreateProduct -> onTabClicked() -> clorwayname" + colorwayName);

					}
					if (itemIndex == 1)
					{
						int length = text.length();
						int indexOfColon = text.find(":");
						viewName = text.substr(indexOfColon + 1, length);
						Logger::Debug("CreateProduct -> onTabClicked() -> viewName" + viewName);

						if (viewName == " Back")
							view = BACK_VIEW;
						else if (viewName == " Front")
							view = FRONT_VIEW;
						else if (viewName == " Left")
							view = LEFT_VIEW;
						else
							view = RIGHT_VIEW;
					}
					if (itemIndex == 4)
					{
						int length = text.length();
						int indexOfColon = text.find(":");
						includeAvatar = text.substr(indexOfColon + 2, length);
						Logger::Debug("CreateImageIntent -> onTabClicked() -> includeAvatar" + includeAvatar);

					}
				}

					QString filepath;
					if (includeAvatar == "Yes")
						filepath = QString::fromStdString(temporaryPath) + "CLOViseTurntableImages/WithAvatar/Avatar_" + QString::fromStdString(colorwayName) + "_" + QString::fromStdString(to_string(view)) + ".png";
					else
						filepath = QString::fromStdString(temporaryPath) + "CLOViseTurntableImages/WithoutAvatar/" + QString::fromStdString(colorwayName) + "_" + QString::fromStdString(to_string(view)) + ".png";

				Logger::Debug("CreateImageIntent -> onTabClicked() -> filepath" + filepath.toStdString());

				item->setTextAlignment(Qt::AlignLeft);
				QPixmap pix(filepath);
				pix.scaled(QSize(80, 80), Qt::KeepAspectRatio);
				QIcon newIcon;
				newIcon.addPixmap(pix);
				Logger::Debug("CreateProduct -> onTabClicked() -> 1");
				item->setIcon(newIcon);
				item->setSizeHint(QSize(80, 80));
				m_ImageIntentList->addItem(item);
				Logger::Debug("CreateProduct -> onTabClicked() -> 3");
				CreateProduct::GetInstance()->m_ImageIntentList->setIconSize(QSize(80, 80));
				//m_ImageIntentList->takeItem(index);
				m_ImageIntentList->addItem(item);
				CreateProduct::GetInstance()->m_ImageIntentList->setItemWidget(item, listItem);
			}
		}

		RESTAPI::SetProgressBarData(0, "", false);
	}

	/*
Description - ClearBOMData() used to clear bom tab data and UI
	* Parameter -
	* Exception -
	*Return -
	*/
	void CreateProduct::ClearBOMData()
	{
		CreateProductBOMHandler::GetInstance()->ClearBomData();
		ClearAllFields(AddNewBom::GetInstance()->m_createBOMTreeWidget);
		BOMUtility::ClearBomSectionLayout(ui_sectionLayout);
		m_bomAddButton->show();
		m_bomAddButton->setEnabled(true);
	}
}