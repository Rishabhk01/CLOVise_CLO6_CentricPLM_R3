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
#include "UpdateProduct.h"
#include <string>
#include <cstring>

#include <QTextEdit>
#include <QComboBox>
#include <QLabel>
#include "qtreewidget.h"
#include <QFile>
#include "qdir.h"
#include "UpdateImageIntent.h"
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
#include "CLOVise/PLM/Outbound/PublishToPLM/UpdateImageIntent.h"
#include "CLOVise/PLM/Helper/UIHelper/CVQDateEdit.h"
#include "CLOVise/PLM/Helper/Util/CustomSpinBox.h"
#include "CLOVise/PLM/Inbound/Print/PLMPrintSearch.h"
#include "CLOVise/PLM/Inbound/Print/PrintConfig.h"
#include "CLOVise/PLM/Outbound/PublishToPLM/BOM/AddNewBom.h"
#include "CLOVise/PLM/Outbound/PublishToPLM/BOM/UpdateProductBOMHandler.h"

using namespace std;

namespace CLOVise
{
	UpdateProduct* UpdateProduct::_instance = NULL;

	UpdateProduct* UpdateProduct::GetInstance()
	{
		if (_instance == NULL) {
			_instance = new UpdateProduct();
		}

		return _instance;
	}



	UpdateProduct::UpdateProduct(QWidget* parent) : MVDialog(parent)
	{
		setupUi(this);
		Logger::Debug("Update product constructor() start....");/*
		if (!PublishToPLMData::GetInstance()->isModelExecuted)
			RESTAPI::SetProgressBarData(15, "Loading Create product", true);*/

		QString windowTitle = PLM_NAME + " PLM Update " + QString::fromStdString(Configuration::GetInstance()->GetLocalizedStyleClassName());
		this->setWindowTitle(windowTitle);

#ifdef __APPLE__
		this->setWindowFlags(Qt::Dialog | Qt::CustomizeWindowHint | Qt::WindowMinMaxButtonsHint | Qt::WindowCloseButtonHint);
#else
		this->setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint | Qt::CustomizeWindowHint);
		m_pTitleBar = new MVTitleBar(windowTitle, this);
		layout()->setMenuBar(m_pTitleBar);
#endif // !__APPLE__

		m_updateProductTreeWidget_1 = nullptr;
		m_updateProductTreeWidget_2 = nullptr;
		m_cancelButton = nullptr;
		m_publishButton = nullptr;
		m_SaveAndCloseButton = nullptr;
		ui_colorwayTable = nullptr;
		m_colorwayAddButton = nullptr;
		m_updateColorwayDeleteButton = nullptr;
		m_downloadedColorway = false;
		m_multipartFilesParams = "";
		m_perveiouselySelectedId = "";
		m_editButtonClicked = false;
		m_2DigiCodeActive = false;
		m_updateBomTab = false;
		m_updateColorButtonSignalMapper = new QSignalMapper();
		m_editButtonSignalMapper = new QSignalMapper();
		m_deleteSignalMapper = new QSignalMapper();
		m_updateColorwayDeleteSignalMapper = new QSignalMapper();
		m_createActionSignalMapper = new QSignalMapper();
		m_printActionSignalMapper = new QSignalMapper();
		m_updateProductTreeWidget_1 = CVWidgetGenerator::CreatePublishTreeWidget("QTreeWidget { background-color: #262628; border: 1px solid #000;}""QTreeWidget::item { padding :5px; height: 20px; color: #FFFFFF; font-face: ArialMT; font-size: 10px; width: 90px; margin-left: 5px; margin-right: 5px; margin-top: 5px; margin-bottom: 5px; border: none;}""QTreeWidget::item:hover{background-color: #262628;}", true);
		m_updateProductTreeWidget_1->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
		m_updateProductTreeWidget_2 = CVWidgetGenerator::CreatePublishTreeWidget("QTreeWidget { background-color: #262628; border: 1px solid #000;}""QTreeWidget::item { padding :5px; height: 20px; color: #FFFFFF; font-face: ArialMT; font-size: 10px; width: 90px; margin-left: 5px; margin-right: 5px; margin-top: 5px; margin-bottom: 5px; border: none;}""QTreeWidget::item:hover{background-color: #262628;}", true);
		m_updateProductTreeWidget_2->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

		m_cancelButton = CVWidgetGenerator::CreatePushButton("Back", BACK_HOVER_PATH, "Back", PUSH_BUTTON_STYLE, 30, true);
		m_publishButton = CVWidgetGenerator::CreatePushButton("Publish", PUBLISH_HOVER_ICON_PATH, "Publish", PUSH_BUTTON_STYLE, 30, true);
		m_colorwayAddButton = CVWidgetGenerator::CreatePushButton("New Colorway", ADD_HOVER_ICON_PATH, "New Colorway", PUSH_BUTTON_STYLE, 30, true);
		m_SaveAndCloseButton = CVWidgetGenerator::CreatePushButton("Save", SAVE_HOVER_ICON_PATH, "Save", PUSH_BUTTON_STYLE, 30, true);
		m_addImageIntentButton = CVWidgetGenerator::CreatePushButton("Add Image Intents", ADD_HOVER_ICON_PATH, "Add Image Intents", PUSH_BUTTON_STYLE, 30, true);
		m_totalCountLabel = CVWidgetGenerator::CreateLabel("Total count: 0 ", QString::fromStdString(BLANK), HEADER_STYLE, true);
		m_totalCountLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
		m_bomAddButton = CVWidgetGenerator::CreatePushButton("New Style Bom", ADD_HOVER_ICON_PATH, "New Style Bom", PUSH_BUTTON_STYLE, 30, true);
		QSpacerItem *horizontalSpacer = new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Fixed);
		m_colorwayRowcount = -1;
		m_imageIntentRowcount = -1;

		m_isSaveClicked = false;
		m_addClicked = false;
		m_collectionId = "";
		m_selectedRow = -1;
		m_isColorwayImageListHidden = false;
		QFrame *frame;
		QGridLayout *gridLayout;
		QSplitter *splitter;

		frame = new QFrame();
		frame->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
		gridLayout = new QGridLayout(frame);
		splitter = new QSplitter(frame);
		splitter->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
		splitter->setOrientation(Qt::Horizontal);
		splitter->insertWidget(0, m_updateProductTreeWidget_1);
		splitter->insertWidget(1, m_updateProductTreeWidget_2);
		gridLayout->setContentsMargins(QMargins(0, 0, 0, 0));
		gridLayout->addWidget(splitter, 0, 0, 1, 1);

		ui_treeWidgetLayout_2->addWidget(frame);
		ui_overviewTab->setLayout(ui_treeWidgetLayout_2);

		ui_tabWidget->setTabText(OVERVIEW_TAB, "Overview");
		ui_tabWidget->setTabText(COLORWAY_TAB, "Colorway");
		ui_tabWidget->setTabText(IMAGE_INTENT_TAB, "Image Intent");
		ui_tabWidget->setTabText(BOM_TAB, "BOM");

		ui_colorwayTable = new MVTableWidget();
		ui_colorwayTable->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
		CVWidgetGenerator::InitializeTableView(ui_colorwayTable);
		CVHoverDelegate* customHoverDelegate = new CVHoverDelegate(ui_colorwayTable);
		ui_colorwayTable->setItemDelegate(customHoverDelegate);
		ui_colorwayTable->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
		ui_colorwayTable->horizontalHeader()->setStyleSheet("QHeaderView { font-face: ArialMT; font-size: 10px;background-color:#2D2D2F; color: #FFFFFF; font-weight: bold;}" "QHeaderView::section:horizontal{border: 1px solid #000000;}" "QHeaderView::down-arrow{ image: url(:/CLOVise/PLM/Images/ui_spin_icon_minus_over.svg); width: 18px; height: 18px; color: #0000FF }" " QHeaderView::up-arrow{ image: url(:/CLOVise/PLM/Images/ui_spin_icon_plus_over.svg); width: 18px; height: 18px; color: #0000FF }");
		ui_colorwayTable->horizontalHeader()->setStretchLastSection(true);
		//ui_colorwayTable->verticalHeader()->setStyleSheet("QHeaderView { color: #FFFFFF;font-weight: bold; background-color:#2D2D2F;}" "QHeaderView::section:vertical{border: 1px solid #000000;}");
		ui_colorwayTable->setStyleSheet("QTableWidget{ background-color: #262628; border-right: 0px solid #000000; border-top: 0px solid #000000; border-left: 0px solid #000000; font-face: ArialMT; font-size: 12px; color: #FFFFFF; }"
			"QTableCornerButton::section{border: 1px solid #000000; background-color: #262628; }""QTableWidget::item{ border-bottom: 2px solid #000000; font-face: ArialMT; font-size: 12px; color: #FFFFFF; }"
			"QToolTip{ color: #46C8FF; background-color: #33414D; border: 1px #000000; }""QTableWidget::item:selected{ background-color: #33414D; color: #46C8FF; }""QScrollBar::add-page:vertical { background: #000000; }"
			"QScrollBar::sub-page:vertical {background: #000000;}""QScrollBar::add-page:horizontal { background: #000000; }""QScrollBar::sub-page:horizontal { background: #000000; }");
		ui_colorwayTable->setContextMenuPolicy(Qt::CustomContextMenu);
		ui_colorwayTableLayout->addWidget(ui_colorwayTable);
		ui_colorwayTable->horizontalHeader()->setMinimumHeight(COLORWAY_HORIZONTAL_HEADER_HEIGHT);

		m_imageIntentTable = new MVTableWidget();
		m_imageIntentTable->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
		m_imageIntentTable->horizontalHeader()->setStyleSheet("QHeaderView { font-face: ArialMT; font-size: 10px;background-color:#2D2D2F; color: #FFFFFF; font-weight: bold;}" "QHeaderView::section:horizontal{border: 1px solid #000000;}" "QHeaderView::down-arrow{ image: url(:/CLOVise/PLM/Images/ui_spin_icon_minus_over.svg); width: 18px; height: 18px; color: #0000FF }" " QHeaderView::up-arrow{ image: url(:/CLOVise/PLM/Images/ui_spin_icon_plus_over.svg); width: 18px; height: 18px; color: #0000FF }");
		m_imageIntentTable->horizontalHeader()->setStretchLastSection(true);
		m_imageIntentTable->verticalHeader()->setStyleSheet("QHeaderView { color: #FFFFFF;font-weight: bold; background-color:#2D2D2F;}" "QHeaderView::section:vertical{border: 1px solid #000000;height: 200px;}");
		m_imageIntentTable->setSortingEnabled(false);
		m_imageIntentTable->setStyleSheet("QTableWidget{ font-face: ArialMT; font-size: 12px; color: #FFFFFF; gridline-color: #000000; border: 0px solid #000000;}""QTableWidget::item{ font-face: ArialMT;gridline-color: #000000; font-size: 12px; color: #FFFFFF; border: 0px solid #000000; }""QToolTip{ color: #46C8FF; background-color: #33414D; border: 0px #000000; }""QTableWidget::item:selected{ background-color: #33414D; color: #46C8FF; }""QScrollBar::add-page:vertical { background: #000; }""QScrollBar::sub-page:vertical {background: #000;}""QScrollBar::add-page:horizontal { background: #000; }""QScrollBar::sub-page:horizontal {background: #000;}""QTableWidget::indicator:unchecked{ background-color:white; }QTableWidget::indicator:checked{ color:blue; }");	//#46C8FF"QTableWidget::focus {border-color: red;}"
		CVWidgetGenerator::InitializeTableView(m_imageIntentTable);
		m_imageIntentTable->verticalHeader()->hide();
		m_imageIntentTable->setShowGrid(false);
		m_imageIntentTable->setWordWrap(true);
		//m_imageIntentTable->show();
		CVHoverDelegate* imageIntentsHoverDelegate = new CVHoverDelegate(m_imageIntentTable);
		m_imageIntentTable->setItemDelegate(imageIntentsHoverDelegate);




		QFont font;
		font.setBold(false);
		font.setFamily("ArialMT");
		ui_overviewTab->setFont(font);
		ui_tabWidget->setFont(font);
		ui_tabWidget->setCurrentIndex(0);
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

		m_colorwayImageList = new QListWidget();
		ui_colorwayImageLayout->addWidget(m_colorwayImageList);
		CVWidgetGenerator::GetInstance()->InitializeIconView(m_colorwayImageList);
		m_colorwayImageList->hide();
		//ui_hideButton->setStyleSheet("#ui_hideButton {\n" "qproperty-icon: none; \n""image: url(:/CLOVise/PLM/Images/icon_close_tree_none.svg);\n""}\n"
			//"#ui_hideButton:hover{\n""image: url(:/CLOVise/PLM/Images/icon_close_tree_over.svg);\n""}""QToolTip{ color: #46C8FF; background-color: #33414D; border: 1px #000000; }");
		//ui_hideButton->hide();
		//line->hide();

		connectSignalSlots(true);
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
		m_digiCodeValue.append(QString::fromStdString(BLANK));
		//json colorwayAttributes = json::array();
		//colorwayAttributes = m_clientSpecificJson["colorwayTableAttributes"];
		//for (int index = 0; index = colorwayAttributes.size(); index++)
		//{
		//	string attributeName = Helper::GetJSONParsedValue<int>(colorwayAttributes, index, false);
		//	m_digiCodeNamesMap = UIHelper::GetEnumValues("uni2DigitCode", m_digiCodeValue, nullptr);
		//}

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

		json imageIntentsColumnNamejson = json::array();
		string imageIntentsColumnsNames = DirectoryUtil::GetPLMPluginDirectory() + "ImageIntentsTableColumns.json";//Reading Columns from json
		json jValue = Helper::ReadJSONFile(imageIntentsColumnsNames);
		string imageIntentTableColumns = Helper::GetJSONValue<string>(jValue, "ImageIntentsTableFieldList", false);
		imageIntentsColumnNamejson = json::parse(imageIntentTableColumns);

		for (int index = 0; index < imageIntentsColumnNamejson.size(); index++)
		{
			string columnname = Helper::GetJSONParsedValue<int>(imageIntentsColumnNamejson, index, false);
			m_ImageIntentsColumnsNames.insert(index, QString::fromStdString(columnname));
		}
		horizontalLayout_5->addWidget(m_imageIntentTable);
		m_imageIntentTable->setColumnCount(m_ImageIntentsColumnsNames.size());
		m_imageIntentTable->setHorizontalHeaderLabels(m_ImageIntentsColumnsNames);
		m_imageIntentTable->horizontalHeader()->setStretchLastSection(true);


		//addUpdateProductDetailsWidgetData();

		m_colorwayImageLabelsMap = UIHelper::GetImageLabels("Colorway");
		Logger::Debug("Update product constructor() m_colorwayImageLabelsMap...." + to_string(m_colorwayImageLabelsMap.size()));
		Configuration::GetInstance()->SetColorwayImageLabels(m_colorwayImageLabelsMap);


		m_styleImageLabelsMap = UIHelper::GetImageLabels("Style");
		Logger::Debug("Update product constructor() m_styleImageLabelsMap...." + to_string(m_styleImageLabelsMap.size()));
		Configuration::GetInstance()->SetStyleImageLabels(m_styleImageLabelsMap);
		Logger::Debug("Update product constructor() end....");
		//if (!PublishToPLMData::GetInstance()->isModelExecuted)
		//RESTAPI::SetProgressBarData(0, "", false);
	}

	UpdateProduct::~UpdateProduct()
	{
		Logger::Debug("Update product Destructor() Start....");
		connectSignalSlots(false);
		UIHelper::DeletePointer(m_updateProductTreeWidget_1);
		UIHelper::DeletePointer(m_updateProductTreeWidget_2);
		UIHelper::DeletePointer(m_cancelButton);
		UIHelper::DeletePointer(m_publishButton);
		UIHelper::DeletePointer(m_SaveAndCloseButton);
		UIHelper::DeletePointer(ui_colorwayTable);
		UIHelper::DeletePointer(m_colorwayAddButton);
		Logger::Debug("Update product Destructor() end....");
	}

	/*
	* Description - ResetDateEditWidget() method used to Reset Date Widget.
	* Parameter -
	* Exception -
	* Return -
	*/
	//void UpdateProduct::onResetDateEditWidget()
	//{
	//	//UIHelper::ResetDate(m_createProductTreeWidget);
	//}

	/*
	* Description - connectSignalSlots() method used to connect and disconnect signal and slots.
	* Parameter -  bool.
	* Exception -
	* Return -
	*/
	void UpdateProduct::connectSignalSlots(bool _b)
	{
		if (_b)
		{
			QObject::connect(m_publishButton, SIGNAL(clicked()), this, SLOT(onPublishToPLMClicked()));
			QObject::connect(m_cancelButton, SIGNAL(clicked()), this, SLOT(onBackButtonClicked()));
			QObject::connect(m_colorwayAddButton, SIGNAL(clicked()), this, SLOT(OnAddColorwayClicked()));
			QObject::connect(m_SaveAndCloseButton, SIGNAL(clicked()), this, SLOT(onSaveAndCloseClicked()));
			QObject::connect(m_addImageIntentButton, SIGNAL(clicked()), this, SLOT(onAddImageIntentClicked()));
			QObject::connect(ui_tabWidget, SIGNAL(currentChanged(int)), this, SLOT(onTabClicked(int)));
			QObject::connect(m_updateColorButtonSignalMapper, SIGNAL(mapped(int)), this, SLOT(OnClickUpdateColorButton(int)));
			QObject::connect(m_updateColorwayDeleteSignalMapper, SIGNAL(mapped(int)), this, SLOT(OnColorwaysTableDeleteButtonClicked(int)));
			QObject::connect(m_createActionSignalMapper, SIGNAL(mapped(int)), this, SLOT(OnCreateColorSpecClicked(int)));
			QObject::connect(m_printActionSignalMapper, SIGNAL(mapped(int)), this, SLOT(OnSearchPrintClicked(int)));
			QObject::connect(m_editButtonSignalMapper, SIGNAL(mapped(int)), this, SLOT(onImageIntentsTableEditButtonClicked(int)));
			QObject::connect(m_deleteSignalMapper, SIGNAL(mapped(int)), this, SLOT(onImageIntentsTableDeleteButtonClicked(int)));
			//QObject::connect(ui_hideButton, SIGNAL(clicked(bool)), this, SLOT(onHideButtonClicked(bool)));
			QObject::connect(m_imageIntentTable->horizontalHeader(), SIGNAL(sectionClicked(int)), this, SLOT(onImageIntentsTableHorizontalHeaderClicked(int)));
			QObject::connect(ui_colorwayTable->horizontalHeader(), SIGNAL(sectionClicked(int)), this, SLOT(onColorwayTableHorizontalHeaderClicked(int)));
			QObject::connect(m_bomAddButton, SIGNAL(clicked()), this, SLOT(onAddNewBomClicked()));


			//QObject::connect(m_dateResetButton, SIGNAL(clicked()), this, SLOT(onResetDateEditWidget()));
		}
		else
		{
			QObject::disconnect(m_publishButton, SIGNAL(clicked()), this, SLOT(onPublishToPLMClicked()));
			QObject::disconnect(m_cancelButton, SIGNAL(clicked()), this, SLOT(onBackButtonClicked()));
			QObject::disconnect(m_colorwayAddButton, SIGNAL(clicked()), this, SLOT(OnAddColorwayClicked()));
			QObject::disconnect(m_SaveAndCloseButton, SIGNAL(clicked()), this, SLOT(onSaveAndCloseClicked()));
			QObject::disconnect(m_addImageIntentButton, SIGNAL(clicked()), this, SLOT(onAddImageIntentClicked()));
			QObject::disconnect(ui_tabWidget, SIGNAL(currentChanged(int)), this, SLOT(onTabClicked(int)));
			QObject::disconnect(m_updateColorButtonSignalMapper, SIGNAL(mapped(int)), this, SLOT(OnClickUpdateColorButton(int)));
			QObject::disconnect(m_updateColorwayDeleteSignalMapper, SIGNAL(mapped(int)), this, SLOT(OnColorwaysTableDeleteButtonClicked(int)));
			QObject::disconnect(m_printActionSignalMapper, SIGNAL(mapped(int)), this, SLOT(OnSearchPrintClicked(int)));
			QObject::disconnect(m_createActionSignalMapper, SIGNAL(mapped(int)), this, SLOT(OnCreateColorSpecClicked(int)));
			QObject::disconnect(m_editButtonSignalMapper, SIGNAL(mapped(int)), this, SLOT(onImageIntentsTableEditButtonClicked(int)));
			QObject::disconnect(m_deleteSignalMapper, SIGNAL(mapped(int)), this, SLOT(onImageIntentsTableDeleteButtonClicked(int)));
			//QObject::disconnect(ui_hideButton, SIGNAL(clicked(bool)), this, SLOT(onHideButtonClicked(bool)));
			//QObject::disconnect(m_dateResetButton, SIGNAL(clicked()), this, SLOT(onResetDateEditWidget()));
			QObject::disconnect(m_imageIntentTable->horizontalHeader(), SIGNAL(sectionClicked(int)), this, SLOT(onImageIntentsTableHorizontalHeaderClicked(int)));
			QObject::disconnect(ui_colorwayTable->horizontalHeader(), SIGNAL(sectionClicked(int)), this, SLOT(onColorwayTableHorizontalHeaderClicked(int)));
			QObject::disconnect(m_bomAddButton, SIGNAL(clicked()), this, SLOT(onAddNewBomClicked()));


		}
	}

	/*
	* Description - cancelWindowClicked() method is a slot for cancel button click and close the publish to plm.
	* Parameter -
	* Exception -
	* Return -
	*/
	void UpdateProduct::onBackButtonClicked()
	{
		Logger::Debug("UpdateProduct onBackButtonClicked() Start....");
		if (!m_isSaveClicked)
		{
			ClearAllFields(m_updateProductTreeWidget_1);
			ClearAllFields(m_updateProductTreeWidget_2);
			ClearColorwayTable();
			UpdateImageIntent::GetInstance()->ClearAllFields();			
			m_imageIntentTable->clear();
			ui_tabWidget->setCurrentIndex(0);
			m_colorSpecList.clear();
			ClearBOMData();
			m_totalCountLabel->setText("Total count: 0");
		}
		SetTotalImageCount();
		ui_tabWidget->setCurrentIndex(0);
		
		ui_tabWidget->setStyleSheet(" QTabWidget::pane { border: none; color: #FFFFFF; font-size: 10px; background-color: #262628; }""QTabBar::tab { width: 100px; padding: 2px; }""QTabBar::tab:selected { border: none; color: #FFFFFF; background-color: \"" + DEFAULT_TAB_BG_COLOR + "\"; }""QTabBar::tab:!selected { color:#FFFFFF; background-color:\"" + SELECTED_TAB_BG_COLOR + "\"; }");

		Configuration::GetInstance()->SetIsPrintSearchClicked(false);

		this->close();
		CLOVise::CLOViseSuite::GetInstance()->setModal(true);
		CLOViseSuite::GetInstance()->show();
		Logger::Debug("UpdateProduct onBackButtonClicked() End....");
	}

	/*
	* Description - Add3DModelDetailsWidgetData() method used to Add 3D Model Details Widget Data.
	* Parameter -
	* Exception -
	* Return -
	*/
	void UpdateProduct::addCreateProductDetailsWidgetData()
	{
		Logger::Debug("UpdateProduct add3DModelDetailsWidgetData() start....");
		//Configuration::GetInstance()->SetModifySupportedAttsList(PublishToPLMData::GetInstance()->GetDocumentConfigJSON());
		//PublishToPLMData::GetInstance()->GetMaxUploadFileSize();
		json fieldsJsonArray = json::array();
		json mergedJsonArray = json::array();
		m_updateProductTreeWidget_1->clear();
		m_updateProductTreeWidget_2->clear();
		m_DeletedColorwayList.clear();
		m_NewlyAddedColorway.clear();
		m_localAttributesList.clear();
		m_colorwayRowcount = 0;
		ui_colorwayTable->clear();
		ui_colorwayTable->clearContents();
		ui_colorwayTable->setRowCount(0);
		m_colorSpecList.clear();

		m_isSaveClicked = false;
		m_downloadedColorwayIdMap.clear();
		m_downloadedStyleJson = PublishToPLMData::GetInstance()->GetUpdateStyleCacheData();
		Logger::Debug("Update product m_downloadedStyleJson.." + to_string(m_downloadedStyleJson));		
		fieldsJsonArray = PublishToPLMData::GetInstance()->GetDocumentFieldsJSON();
		Logger::Debug("UpdateProduct add3DModelDetailsWidgetData() 1....");
		//QStringList emptylist;
		//json feildsJson = json::object();
		//string attRestApiExposed = "";
		//UTILITY_API->DisplayMessageBox(to_string(fieldsJsonArray));
		json jsonvalueArray = json::array();
		string columnsNames = DirectoryUtil::GetPLMPluginDirectory() + "CentricUpdateStyleHierarchy.json";//Reading Columns from json
		jsonvalueArray = Helper::ReadJSONFile(columnsNames);

		int sizeOfResponseJSON = fieldsJsonArray.size();
		int sizeOfDefaultJSON = jsonvalueArray.size();
		Logger::Debug("UpdateProduct add3DModelDetailsWidgetData() fieldsJsonArray...." + to_string(fieldsJsonArray.size()));
		Logger::Debug("UpdateProduct add3DModelDetailsWidgetData() jsonvalueArray...." + to_string(jsonvalueArray.size()));
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
		drawCriteriaWidget(mergedJsonArray, m_updateProductTreeWidget_1, m_updateProductTreeWidget_2, PublishToPLMData::GetInstance()->GetActive3DModelMetaData());
		GetcolorwayDetails();
		m_updateProductTreeWidget_1->setColumnCount(2);
		m_updateProductTreeWidget_1->setHeaderHidden(true);
		m_updateProductTreeWidget_1->setWordWrap(true);
		m_updateProductTreeWidget_1->setDropIndicatorShown(false);
		m_updateProductTreeWidget_1->setRootIsDecorated(false);
		m_updateProductTreeWidget_1->setSelectionMode(QAbstractItemView::NoSelection);
		m_updateProductTreeWidget_1->setStyleSheet("QTreeWidget { background-color: #262628; border: 1px solid #000; padding-left: 20px; min-width: 400px; outline: 0;}""QTreeWidget::item {height: 20px; width: 200px; margin-right: 20px; margin-top: 5px; margin-bottom: 5px; border: none; }""QTreeWidget::item:hover{ background-color: #262628; }""QTreeView{outline: 0;}");
		m_updateProductTreeWidget_2->setColumnCount(2);
		m_updateProductTreeWidget_2->setHeaderHidden(true);
		m_updateProductTreeWidget_2->setWordWrap(true);
		m_updateProductTreeWidget_2->setDropIndicatorShown(false);
		m_updateProductTreeWidget_2->setRootIsDecorated(false);
		m_updateProductTreeWidget_2->setSelectionMode(QAbstractItemView::NoSelection);
		m_updateProductTreeWidget_2->setStyleSheet("QTreeWidget { background-color: #262628; border: 1px solid #000; padding-left: 20px; min-width: 400px; outline: 0;}""QTreeWidget::item {height: 20px; width: 200px; margin-right: 20px; margin-top: 5px; margin-bottom: 5px; border: none; }""QTreeWidget::item:hover{ background-color: #262628; }""QTreeView{outline: 0;}");

		//UTILITY_API->DisplayMessageBox(to_string(jsonvalue));
		//for (int feildsCount = 0; feildsCount < jsonvalue.size(); feildsCount++)
		//{
		//	feildsJson = Helper::GetJSONParsedValue<int>(jsonvalue, feildsCount, false);
		//	attRestApiExposed = Helper::GetJSONValue<string>(feildsJson, ATTRIBUTE_REST_API_EXPOSED, true);
		//	if (attRestApiExposed == "false")
		//		continue;
		//	drawWidget(feildsJson, m_updateProductTreeWidget);

		//	//drawCriteriaWidget(attributesJson, m_createProductTreeWidget, PublishToPLMData::GetInstance()->GetActive3DModelMetaData());
		//	//break;
		//}
		//for (int feildsCount = 0; feildsCount < fieldsJsonArray.size(); feildsCount++)
		//{
		//	feildsJson = Helper::GetJSONParsedValue<int>(fieldsJsonArray, feildsCount, false);
		//	attRestApiExposed = Helper::GetJSONValue<string>(feildsJson, ATTRIBUTE_REST_API_EXPOSED, true);
		//	if (attRestApiExposed == "false")
		//		continue;
		//	drawWidget(feildsJson, m_updateProductTreeWidget);

		//	//drawCriteriaWidget(attributesJson, m_createProductTreeWidget, PublishToPLMData::GetInstance()->GetActive3DModelMetaData());
		//	//break;
		//}
		//m_updateProductTreeWidget->setStyleSheet("QTreeWidget { background-color: #262628; border: 1px solid #000; padding-left: 20px; }""QTreeWidget::item {height: 20px;" "width: 200px;""margin-right: 20px;""margin-top: 5px;""margin-bottom: 5px;""border: none;}""QTreeWidget::item:hover{background-color: #262628;} QTreeView{outline: 0;}");
		//
	/*	if (!PublishToPLMData::GetInstance()->GetDateFlag())
			m_dateResetButton->hide();
		else
			m_dateResetButton->show();*/

		Logger::Debug("UpdateProduct add3DModelDetailsWidgetData() end....");
	}

	/*
	* Description - DrawCriteriaWidget() method used to create fields in UI.
	* Parameter -  json, QTreeWidget, json.
	* Exception -
	* Return -
	*/
	void UpdateProduct::drawCriteriaWidget(json _attributesJsonArray, QTreeWidget* _documentPushToPLMTree_1, QTreeWidget* _documentPushToPLMTree_2, json _metadataJson)
	{
		Logger::Debug("UpdateProduct drawCriteriaWidget() start....");

		if (_attributesJsonArray.size() < 10)
		{
			drawWidget(_attributesJsonArray, _documentPushToPLMTree_1, STARTING_INDEX, _attributesJsonArray.size());
			_documentPushToPLMTree_2->hide();
		}
		else
		{
			Logger::Debug("UpdateProduct drawCriteriaWidget() _attributesJsonArray.size()...." + to_string(_attributesJsonArray.size()));
			drawWidget(_attributesJsonArray, _documentPushToPLMTree_1, STARTING_INDEX, _attributesJsonArray.size() / DIVISION_FACTOR);
			drawWidget(_attributesJsonArray, _documentPushToPLMTree_2, _attributesJsonArray.size() / DIVISION_FACTOR, _attributesJsonArray.size());
		}

		Logger::Debug("Update product drawCriteriaWidget() end....");
	}

	/*
	* Description - DrawCriteriaWidget() method used to create input fields.
	* Parameter -  json, QTreeWidget, string, string, bool.
	* Exception -
	* Return -
	*/
	void UpdateProduct::drawWidget(json _feildsJson, QTreeWidget* m_TreeWidget, int _start, int _end)
	{
		Logger::Debug("Update product drawWidget() start....");
		/*string attName = Helper::GetJSONValue<string>(_attJson, ATTRIBUTE_NAME_KEY, true);
		string attType = Helper::GetJSONValue<string>(_attJson, ATTRIBUTE_TYPE_KEY, true);
		bool attRequired = Helper::IsValueTrue((Helper::GetJSONValue<string>(_attJson, ATTRIBUTE_REQUIRED_KEY, true)));
		if (!FormatHelper::HasContent(_attValue))
			_attValue = BLANK;*/
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
		string attRestApiExposed = "";
		string attCreateOnly = "";
		string internalNameKey = "";
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
			internalName = Helper::GetJSONValue<string>(attJson, ATTRIBUTE_INTERNAL_NAME, true);

			if (!m_localAttributesList.contains(QString::fromStdString(internalName)))
			{
				m_localAttributesList.append(QString::fromStdString(internalName));
			}
			else
				continue;

			attributeName = Helper::GetJSONValue<string>(attJson, ATTRIBUTE_NAME, true);
			attributeDisplayName = Helper::GetJSONValue<string>(attJson, ATTRIBUTE_DISPLAY_NAME, true);
			attributeRequired = Helper::GetJSONValue<string>(attJson, ATTRIBUTE_REQUIRED, true);
			restrictEdit = Helper::GetJSONValue<string>(attJson, ATTRIBUTE_EDITABLE, true);

			attCreateOnly = Helper::GetJSONValue<string>(attJson, ATTRIBUTE_CREATE_ONLY, true);

			if (attributeRequired == "false")
				required = false;

			if (restrictEdit == "false" && attCreateOnly == "false")
				isEditable = true;

			internalNameKey = internalName;
			if (internalNameKey == "parent_season")
				internalNameKey = "parent_season_Id";


			QString attDefaultValue = QString::fromStdString(Helper::GetJSONValue<string>(m_downloadedStyleJson, internalNameKey, true));
			if (!FormatHelper::HasContent(attDefaultValue.toStdString())) {
				attDefaultValue = "";
			}
			Logger::Debug("Update product drawCriteriaWidget() attDefaultValue....:" + attDefaultValue.toStdString());
			if (attributeType == INTEGER_ATT_TYPE_KEY || attributeType == CONSTANT_ATT_TYPE_KEY)
			{
				QTreeWidgetItem* topLevel = new QTreeWidgetItem();  // Creating new TreeWidgetItem
				m_TreeWidget->addTopLevelItem(topLevel);
				// Adding ToplevelItem
				//UTILITY_API->DisplayMessageBox("isInteger::" + to_string(isInteger));
				QSpinBox* spinBox;
				spinBox = CVWidgetGenerator::CreateSpinBoxWidget(attDefaultValue, isEditable, "");
				spinBox->setProperty(ATTRIBUTE_TYPE.c_str(), QString::fromStdString(attributeType));
				spinBox->setProperty(ATTRIBUTE_INITIAL_VALUE.c_str(), attDefaultValue);
				m_TreeWidget->setItemWidget(topLevel, 0, CVWidgetGenerator::CreateLabelWidget(attributeDisplayName, internalName, "", required, !isEditable));
				m_TreeWidget->setItemWidget(topLevel, 1, spinBox);

			}
			else if (attributeType == DATE_ATT_TYPE_KEY || attributeType == TIME_ATT_TYPE_KEY)
			{
				ColorConfig::GetInstance()->SetDateFlag(true);
				MaterialConfig::GetInstance()->SetDateFlag(true);
				ProductConfig::GetInstance()->SetDateFlag(true);
				QStringList dateList;
				QTreeWidgetItem* topLevel = new QTreeWidgetItem();  // Creating new TreeWidgetItem

				/*if (stoi(attributeEditable) > 0)
				{
					isEditable = false;
				}*/
				QDateEdit* DateTimeEdit;
				DateTimeEdit = CVWidgetGenerator::CreateDateWidget(attDefaultValue.toStdString(), isEditable);
				DateTimeEdit->setProperty(ATTRIBUTE_TYPE.c_str(), QString::fromStdString(attributeType));
				DateTimeEdit->setProperty(ATTRIBUTE_INITIAL_VALUE.c_str(), attDefaultValue);
				m_TreeWidget->addTopLevelItem(topLevel);			// Adding ToplevelItem
				m_TreeWidget->setItemWidget(topLevel, 0, CVWidgetGenerator::CreateLabelWidget(attributeDisplayName, internalName, "", required, !isEditable));	// Adding label at column 1
				m_TreeWidget->setItemWidget(topLevel, 1, DateTimeEdit);

			}
			else if (attributeType == BOOLEAN_ATT_TYPE_KEY)
			{
				QTreeWidgetItem* topLevel = new QTreeWidgetItem();  // Creating new TreeWidgetItem
				/*if (stoi(attributeEditable) > 0)
				{
					isEditable = false;
				}*/
				ComboBoxItem* comboBox;
				comboBox = CVWidgetGenerator::CreateBooleanWidget(attDefaultValue, isEditable, topLevel, 1);
				comboBox->setProperty(ATTRIBUTE_TYPE.c_str(), QString::fromStdString(attributeType));
				comboBox->setProperty(ATTRIBUTE_INITIAL_VALUE.c_str(), attDefaultValue);
				m_TreeWidget->addTopLevelItem(topLevel);						// Adding ToplevelItem
				m_TreeWidget->setItemWidget(topLevel, 0, CVWidgetGenerator::CreateLabelWidget(attributeDisplayName, internalName, "", required, !isEditable));	// Adding label at column 1
				m_TreeWidget->setItemWidget(topLevel, 1, comboBox);	// Adding label at column 2

			}
			else if (attributeType == REF_ATT_TYPE_KEY || attributeType == ENUM_ATT_TYPE_KEY)
			{
				Logger::Debug("UpdateProduct drawWidget() ref  6....");

				QTreeWidgetItem* topLevel = new QTreeWidgetItem();
				ComboBoxItem* comboBox = new ComboBoxItem();

				QStringList valueList;
				valueList.append(QString::fromStdString(BLANK));
				//json attJson = json::object();
				if (attributeType == ENUM_ATT_TYPE_KEY)
				{
					string format = Helper::GetJSONValue<string>(attJson, "format", true);
					UIHelper::GetEnumValues(format, valueList, comboBox);
				}
				else
				{
					if (attributeName == "Season")
					{

						responseJson = RESTAPI::makeRestcallGet(RESTAPI::SEASON_SEARCH_API, "?limit=" + Configuration::GetInstance()->GetMaximumLimitForRefAttValue(), "", "Loading season details..");
					}
					else if (attributeName == "Brand")
					{
						string seasonId = Helper::GetJSONValue<string>(m_downloadedStyleJson, "parent_season_Id", true);
						responseJson = RESTAPI::makeRestcallGet(RESTAPI::SEASON_SEARCH_API, "/hierarchy?limit=" + Configuration::GetInstance()->GetMaximumLimitForRefAttValue(), "/" + seasonId, "Loading brand details..");
					}
					else if (attributeName == "Department")
					{

						string brandId = Helper::GetJSONValue<string>(m_downloadedStyleJson, "category_1", true);
						responseJson = RESTAPI::makeRestcallGet(RESTAPI::DEPARTMENT_DETAIL_API, "/hierarchy?limit=" + Configuration::GetInstance()->GetMaximumLimitForRefAttValue(), "/" + brandId, "Loading Department details..");
					}
					else if (attributeName == "Collection")
					{
						string departmentId = Helper::GetJSONValue<string>(m_downloadedStyleJson, "category_2", true);
						responseJson = RESTAPI::makeRestcallGet(RESTAPI::COLLECTION_DETAIL_API, "/hierarchy?limit=" + Configuration::GetInstance()->GetMaximumLimitForRefAttValue(), "/" + departmentId, "Loading collection details..");
					}

					else if (attributeName == "Style Type")
					{
						responseJson = RESTAPI::makeRestcallGet(RESTAPI::STYLE_TYPE_API, "?available=true&limit=100", "", "Loading style type details..");
						m_currentlySelectedStyleTypeId = attDefaultValue.toStdString();
					}
					else if (attributeName == "Style/Shape")
					{
						string seasonId = Helper::GetJSONValue<string>(m_downloadedStyleJson, "parent_season_Id", true);
						responseJson = RESTAPI::makeRestcallGet(RESTAPI::SHAPE_API, "&skip=0&limit=100", "?shape_seasons=" + seasonId, "Loading style type details..");
					}
					else if (attributeName == "Style/Theme")
					{
						string seasonId = Helper::GetJSONValue<string>(m_downloadedStyleJson, "parent_season_Id", true);
						responseJson = RESTAPI::makeRestcallGet(RESTAPI::THEME_API, "&skip=0&limit=100", "?theme_seasons=" + seasonId, "Loading theme type details..");
					}

					Logger::Debug("UpdateProduct drawWidget() attributeName:**************************** " + attributeName);
					

					for (int i = 0; i < responseJson.size(); i++)
					{
						attJson = Helper::GetJSONParsedValue<int>(responseJson, i, false);;///use new method
						attName = Helper::GetJSONValue<string>(attJson, ATTRIBUTE_NAME, true);
						Logger::Debug("PublishToPLMData -> SetDocumentConfigJSON attName: " + attName);
						attId = Helper::GetJSONValue<string>(attJson, ATTRIBUTE_ID, true);
						Logger::Debug("PublishToPLMData -> SetDocumentConfigJSON attId: " + attId);
						valueList.append(QString::fromStdString(attName));
						m_seasonNameIdMap.insert(make_pair(attName, attId));
						//m_styleTypeNameIdMap.insert(make_pair(attName, attId));
						if (attributeName == "Style Type" && attId == attDefaultValue.toStdString())
						{
							string tdsmapString = Helper::GetJSONValue<string>(attJson, "tds_map", false);
							Logger::Debug("PublishToPLMData -> SetDocumentConfigJSON tdsmapString: " + tdsmapString);
							json tdsmapJson = json::parse(tdsmapString);
							Logger::Debug("PublishToPLMData -> SetDocumentConfigJSON tdsmapJson: " + to_string(tdsmapJson));
							string apparelBomFlag = Helper::GetJSONValue<string>(tdsmapJson, "ApparelBOM", true);
							Logger::Debug("PublishToPLMData -> SetDocumentConfigJSON apparelBomFlag: " + apparelBomFlag);
							if (apparelBomFlag == "true")
								m_bomAddButton->show();
							else
								m_bomAddButton->hide();
						}
						if (FormatHelper::HasContent(attDefaultValue.toStdString()))
						{
							
							if (attributeName == "Style Type" && attDefaultValue == QString::fromStdString(attId))
							{
								string allowCreateColor = Helper::GetJSONValue<string>(attJson, ALLOW_CREATE_COLOR, true);
								if (allowCreateColor == "true")
									m_isCreateColorSpec = true;
								else
									m_isCreateColorSpec = false;
							}
						}
						comboBox->setProperty(attName.c_str(), QString::fromStdString(attId));
						comboBox->setProperty(attId.c_str(), QString::fromStdString(attName));
					}
				}
				comboBox->setProperty("attName", QString::fromStdString(attributeName));
				comboBox->setProperty(ATTRIBUTE_TYPE.c_str(), QString::fromStdString(attributeType));

				m_TreeWidget->addTopLevelItem(topLevel);
				m_TreeWidget->setItemWidget(topLevel, 0, CVWidgetGenerator::CreateLabelWidget(attributeDisplayName, internalName, "", required, !isEditable));
				m_TreeWidget->setItemWidget(topLevel, 1, comboBox);
				valueList.sort();
				comboBox->addItems(valueList);
				string fieldVal = attDefaultValue.toStdString();
				string defaultValue;
				Logger::Debug("Updateproduct drawWidget()attDefaultValue..." + fieldVal);
				//if (internalName == "parent_season")
				//	defaultValue = fieldVal;
				//else
				defaultValue = comboBox->property(fieldVal.c_str()).toString().toStdString();
				Logger::Debug("Updateproduct drawWidget()defaultValue..." + defaultValue);
				comboBox->setProperty(ATTRIBUTE_INITIAL_VALUE.c_str(), QString::fromStdString(defaultValue));
				int valueIndex = comboBox->findText(QString::fromUtf8(QString::fromStdString(defaultValue).toStdString().c_str()));

				if (valueIndex < 0)
				{
					defaultValue = "";
					valueIndex = comboBox->findText(QString::fromUtf8(QString::fromStdString(defaultValue).toStdString().c_str()));
				}
				else if (attributeName == "Style Type")
				{
					Logger::Debug("Updateproduct drawWidget()defaultValue..valueIndex." + to_string(valueIndex));
					m_selectedStyleTypeIndex = valueIndex;
				}
				comboBox->setCurrentIndex(valueIndex);
				comboBox->setProperty("LabelName", QString::fromStdString(attributeName));
				comboBox->setStyleSheet(COMBOBOX_STYLE);
				if (comboBox->isEnabled())
				{
					comboBox->setEditable(true);
					comboBox->fillItemListAndDefaultValue(valueList, comboBox->currentText());
					QCompleter* m_nameCompleter = new QCompleter(valueList, comboBox); // using QCompleter class
					m_nameCompleter->setCompletionMode(QCompleter::PopupCompletion);
					m_nameCompleter->setCaseSensitivity(Qt::CaseInsensitive);
					comboBox->setCompleter(m_nameCompleter);
				}
				if (!isEditable)
				{
					comboBox->setDisabled(true);
					comboBox->setFocusPolicy(Qt::NoFocus);
					comboBox->setStyleSheet(DISABLED_COMBOBOX_STYLE);
				}
				QObject::connect(comboBox, SIGNAL(activated(const QString&)), this, SLOT(OnHandleDropDownValue(const QString&)));
				Logger::Debug("Update product drawWidget() ref  End....");

			}
			else if (attributeType == STRING_ATT_TYPE_KEY || attributeType == TEXT_AREA_ATT_TYPE_KEY)
			{
				Logger::Debug("Update product drawWidget() string  Start....");
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
				Logger::Debug("Update product drawWidget() string  attDefaultValue...." + attDefaultValue.toStdString());
				LineEdit->setText(attDefaultValue);
				LineEdit->setProperty(ATTRIBUTE_TYPE.c_str(), QString::fromStdString(attributeType));
				LineEdit->setProperty(ATTRIBUTE_INITIAL_VALUE.c_str(), attDefaultValue);
				m_TreeWidget->addTopLevelItem(topLevel);						// Adding ToplevelItem
				m_TreeWidget->setItemWidget(topLevel, 0, CVWidgetGenerator::CreateLabelWidget(attributeDisplayName, internalName, "", required, !isEditable));	// Adding label at column 1
				m_TreeWidget->setItemWidget(topLevel, 1, LineEdit);
				Logger::Debug("Update product drawWidget() string  End....");
			}
		}
		this->update();
		Logger::Debug("Update product drawWidget() end....");
	}

	/*
	* Description - PublishToPLMClicked() method is a slot for publish 3D model to plm click.
	* Parameter -
	* Exception - exception, Char *
	* Return -
	*/
	void UpdateProduct::onPublishToPLMClicked()
	{
		Logger::Debug("Update product onPublishToPLMClicked() start....");
		QDir dir;
		string response;
		try
		{
			Configuration::GetInstance()->SetIsPrintSearchClicked(false);		
			
			if (ValidateColorwayNameField() && UpdateProductBOMHandler::GetInstance()->ValidateBomFields())
			{
				this->hide();
				collectCreateProductFieldsData();
				UIHelper::ValidateRquired3DModelData(m_updateProductTreeWidget_1);
				UIHelper::ValidateRquired3DModelData(m_updateProductTreeWidget_2);

				UTILITY_API->CreateProgressBar();
				RESTAPI::SetProgressBarData(20, "Updating " + Configuration::GetInstance()->GetLocalizedStyleClassName() + "..", true);
				UTILITY_API->SetProgress("Publishing to PLM", (qrand() % 101));
				vector<pair<string, string>> headerNameAndValueList;
				headerNameAndValueList.push_back(make_pair("content-Type", "application/json"));
				headerNameAndValueList.push_back(make_pair("Accept", "application/json"));
				headerNameAndValueList.push_back(make_pair("Cookie", Configuration::GetInstance()->GetBearerToken()));
				//	UTILITY_API->DisplayMessageBox(m_collectionId);
				//UTILITY_API->DisplayMessageBox(m_ProductMetaData);
				string productId = Helper::GetJSONValue<string>(m_downloadedStyleJson, ATTRIBUTE_ID, true);
				GetLatestRevisionStyleID(productId);
				if (FormatHelper::HasContent(productId))
				{
					response = RESTAPI::PutRestCall(m_productMetaData, Configuration::GetInstance()->GetPLMServerURL() + RESTAPI::STYLE_ENDPOINT_API + "/" + productId, "content-type: application/json");
					//UTILITY_API->DisplayMessageBox(response);
				}


				if (!FormatHelper::HasContent(response))
				{
					RESTAPI::SetProgressBarData(0, "", false);
					throw "Unable to publish to PLM. Please try again or Contact your System Administrator.";
				}

				if (FormatHelper::HasError(response))
				{
					RESTAPI::SetProgressBarData(0, "", false);
					Logger::Debug("PublishToPLMData -> onPublishToPLMClicked 1");
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
					string documentId;
					string glbDocumentId;
					Logger::Debug("PublishToPLMData -> onPublishToPLMClicked 1");
					json detailJson = Helper::GetJsonFromResponse(response, "{");
					Logger::Debug("PublishToPLMData -> onPublishToPLMClicked 2");
					string productId = Helper::GetJSONValue<string>(detailJson, ATTRIBUTE_ID, true);
					Logger::Debug("PublishToPLMData -> onPublishToPLMClicked 3");
					
					if (PublishToPLMData::GetInstance()->GetIsCreateNewDocument())
					{
						documentId = uploadDocument(productId);
						glbDocumentId=uploadGLBFile(productId);
					}
				   
					else
					{
						Logger::Debug("PublishToPLMData -> onPublishToPLMClicked 5");
						string latestRevisionId = PublishToPLMData::GetInstance()->GetLatestRevision();
						documentId = reviseDocument(latestRevisionId);
						if (PublishToPLMData::GetInstance()->GetIsCreateNewGLBDocument())
						{
							Logger::Debug("PublishToPLMData -> onPublishToPLMClicked 6======");
							glbDocumentId=uploadGLBFile(productId);
						}
						else
						{
							Logger::Debug("PublishToPLMData -> onPublishToPLMClicked 7======");
							string latestGLBRevisionId = PublishToPLMData::GetInstance()->GetGLBLatestRevision();
							glbDocumentId=reviseDocument(latestGLBRevisionId);
						}					
					}
					

					Logger::Debug("PublishToPLMData -> onPublishToPLMClicked 6");
					UTILITY_API->SetProgress("Publishing to PLM", (qrand() % 101));
					CreateAndUpdateColorways(productId);
					exportZPRJ(documentId);
					exportGLBFile(glbDocumentId);
					//UploadStyleThumbnail(productId);
					exportTurntableImages();
					DeleteColorwayFromPLM();
					uploadColorwayImages();
					LinkImagesToColorways(productId);
					if (UpdateProductBOMHandler::GetInstance()->IsBomCreated())
						UpdateProductBOMHandler::GetInstance()->CreateBom(productId, AddNewBom::GetInstance()->m_BOMMetaData, m_CloAndPLMColorwayMap);
					m_colorSpecList.clear();
					UTILITY_API->NewProject();
					//Clearing cached data post successful publish toii plm
					PublishToPLMData::GetInstance()->SetActiveProductMetaData(json::object());
					PublishToPLMData::GetInstance()->SetActive3DModelMetaData(json::object());
					PublishToPLMData::GetInstance()->SetIsProductOverridden(false);
					PublishToPLMData::GetInstance()->SetActiveProductId(BLANK);
					PublishToPLMData::GetInstance()->SetActiveProductObjectId(BLANK);
					PublishToPLMData::GetInstance()->SetActiveProductName(BLANK);
					PublishToPLMData::GetInstance()->SetActiveProductStatus(BLANK);
					PublishToPLMData::GetInstance()->Set3DModelObjectId(BLANK);
					//m_createProductTreeWidget->clear();
					CVDisplayMessageBox* DownloadDialogObject = new CVDisplayMessageBox(this);
					DownloadDialogObject->DisplyMessage("Successfully published to PLM.");
					DownloadDialogObject->setModal(true);
					DownloadDialogObject->show();
					Helper::RemoveDirectory(QString::fromStdString(Configuration::GetInstance()->TURNTABLE_IMAGES_WITH_AVATAR_TEMP_DIRECTORY));
					dir.mkpath(QString::fromStdString(Configuration::GetInstance()->TURNTABLE_IMAGES_WITH_AVATAR_TEMP_DIRECTORY));
					Helper::RemoveDirectory(QString::fromStdString(Configuration::GetInstance()->TURNTABLE_IMAGES_WITHOUT_AVATAR_TEMP_DIRECTORY));
					dir.mkpath(QString::fromStdString(Configuration::GetInstance()->TURNTABLE_IMAGES_WITHOUT_AVATAR_TEMP_DIRECTORY));
					ClearAllFields(m_updateProductTreeWidget_1);
					ClearAllFields(m_updateProductTreeWidget_2);
					ui_colorwayTable->clear();
					ui_colorwayTable->clearContents();
					ui_colorwayTable->setRowCount(0);
					m_colorwayRowcount = 0;
					ui_colorwayTable->hide();
					UpdateImageIntent::GetInstance()->ClearAllFields();
					m_imageIntentTable->clear();
					m_colorSpecList.clear();
					m_downloadedStyleJson.clear();
					UpdateImageIntent::GetInstance()->m_ColorwayViewMap.clear();
					m_nonCloStyleImageLabelsMap.clear();
					m_imageIntentIdAndLabeMap.clear();
					m_nonCloColorwayImagesMap.clear();
					m_nonCloColorWayImageLabelsMap.clear();
					m_isSaveClicked = false;
					PublishToPLMData::GetInstance()->SetUpdateStyleCacheData(m_downloadedStyleJson);
					ui_tabWidget->setCurrentIndex(0);
					PublishToPLMData::GetInstance()->SetIsCreateNewGLBDocument(false);
					m_totalCountLabel->setText("Total count: 0");
					ClearBOMData();
					RESTAPI::SetProgressBarData(0, "", false);
					//this->hide();
					this->close();

				}
			}

		}
		catch (exception& e)
		{
			RESTAPI::SetProgressBarData(0, "", false);
			Logger::Error("Update product-> Update product Exception - " + string(e.what()));
			UTILITY_API->DisplayMessageBox(e.what());


			this->show();
		}
		catch (const char* msg)
		{
			Logger::Error("Update product-> Update product Exception - " + string(msg));
			wstring wstr(msg, msg + strlen(msg));
			RESTAPI::SetProgressBarData(0, "", false);
			UTILITY_API->DisplayMessageBoxW(wstr);


			this->show();
		}
		catch (string str)
		{
			RESTAPI::SetProgressBarData(0, "", false);
			Logger::Error("Update product-> Update product Exception - " + str);
			UTILITY_API->DisplayMessageBox(str);
			this->show();
		}

		Logger::Debug("Update product onPublishToPLMClicked() end....");
	}

	/*
	* Description - CollectPublishToPLMFieldsData() method used to collect publish to plm fields data.
	* Parameter -
	* Exception -
	* Return -
	*/
	void UpdateProduct::collectPublishToPLMFieldsData()
	{
		Logger::Debug("Update product collectPublishToPLMFieldsData() start....");
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
		Logger::Debug("Update product collectPublishToPLMFieldsData() end....");
	}

	/*
	* Description - CollectProductFieldsData() method used to collect product fields data.
	* Parameter -
	* Exception -
	* Return -
	*/
	void UpdateProduct::collectProductFieldsData()
	{
		Logger::Debug("Update product collectProductFieldsData() start....");
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
		Logger::Debug("Update product collectProductFieldsData() end....");
	}

	/*
	* Description - Collect3DModelFieldsData() method used to collect 3d model fields data.
	* Parameter -
	* Exception -
	* Return -
	*/
	void UpdateProduct::collectCreateProductFieldsData()
	{
		Logger::Debug("Update product collectCreateProductFieldsData() start....");
		m_productMetaData = collectCriteriaFields(m_updateProductTreeWidget_1, m_updateProductTreeWidget_2);
		//m_documentDetailsJson[ATTRIBUTES_KEY] = m_ProductMetaData;

		/*json thumbNailDetails = json::object();
		json _3DModelDetails = json::object();
		thumbNailDetails[THUMBNAIL_NAME_KEY] = m_3DModelThumbnailName;
		_3DModelDetails[THREE_D_MODEL_NAME_KEY] = m_3DModelFileName;
		_3DModelDetails[THREE_D_MODEL_TYPE_KEY] = "zprj";
		m_documentDetailsJson[THUMBNAIL_DETAIL_KEY] = thumbNailDetails;

		m_documentDetailsJson[THREE_D_MODEL_DETAIL_KEY] = _3DModelDetails;
		m_documentDetailsJson[GLB_EXPORT_KEY] = false;*/

		Logger::Debug("Update product collectCreateProductFieldsData() end....");
	}

	/*
	* Description - PreparePublishRequestParameter() method used to prepare request parameter.
	* Parameter -
	* Exception -
	* Return -
	*/
	string UpdateProduct::getPublishRequestParameter(string _path, string _fileName)
	{
		Logger::Debug("Update product getPublishRequestParameter() start....");
		Logger::Debug("Update product getPublishRequestParameter_path" + _path);
		Logger::Debug("Create product getPublishRequestParameter_fileName" + _fileName);
		string contentType = Helper::GetFileContetType(_path);
		//UTILITY_API->DisplayMessageBox("contentType:" + contentType);
		string fileStream = Helper::GetFilestream(_path);
		string contentLength = Helper::getFileLength(_path);
		//UTILITY_API->DisplayMessageBox("contentLength:" + contentLength);
		//UTILITY_API->DisplayMessageBox("getPublishRequestParameter path:" + _path);
		//UTILITY_API->DisplayMessageBox("getPublishRequestParameter _imageName:" + _fileName);
		string postField = "------WebKitFormBoundary7MA4YWxkTrZu0gW\r\nContent-Disposition: form-data; name=\"file\"; filename=" + _fileName + "\r\nContent-Type: " + contentType + "\r\n" + contentLength + "\r\n\r\n" + fileStream + "\r\n------WebKitFormBoundary7MA4YWxkTrZu0gW";
		if (_fileName.find(".png") != -1)
		{
			postField += "\r\nContent-Disposition: form-data; name=\"generated_by\"\r\n\r\n";
			postField += "CLO3D";
			postField += "\r\n";
			postField += "------WebKitFormBoundary7MA4YWxkTrZu0gW--";
			Logger::Debug("Update product getPublishRequestParameter(string _path, string _imageName) end....");
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

	string UpdateProduct::uploadDocument(string _productId)
	{
		Logger::Debug("UpdateProduct uploadDocument() start....");
		string latestRevisionId;
		try
		{
			vector<pair<string, string>> headerNameAndValueList;
			headerNameAndValueList.push_back(make_pair("content-Type", "application/json"));
			headerNameAndValueList.push_back(make_pair("Cookie", Configuration::GetInstance()->GetBearerToken()));

			json bodyJson = json::object();
			string zprjFileName = UTILITY_API->GetProjectName();
			bodyJson["node_name"] = zprjFileName;
			string bodyJsonString = to_string(bodyJson);
			string resultJsonString = RESTAPI::PostRestCall(bodyJsonString, Configuration::GetInstance()->GetPLMServerURL() + RESTAPI::DOCUMENT_CREATE_API + "/" + _productId, "content-type: application/json; boundary=----WebKitFormBoundary7MA4YWxkTrZu0gW");
			if (!FormatHelper::HasContent(resultJsonString))
			{
				throw "Unable to initiliaze Document Configuration. Please try again or Contact your System Administrator.";
			}
			json detailJson = Helper::GetJsonFromResponse(resultJsonString, "{");
			//UTILITY_API->DisplayMessageBox(to_string(detailJson));

			latestRevisionId = Helper::GetJSONValue<string>(detailJson, LATEST_REVISION_KEY, true);
		}
		catch (string msg)
		{
			Logger::Error("UpdateProduct uploadDocument() Exception - " + msg);
		}
		catch (exception & e)
		{
			Logger::Error("UpdateProduct uploadDocument() Exception - " + string(e.what()));
		}
		catch (const char* msg)
		{
			wstring wstr(msg, msg + strlen(msg));
			Logger::Error("UpdateProduct uploadDocument() Exception - " + string(msg));
		}
		//UTILITY_API->DisplayMessageBox("Revision id  " + latestRevisionId);
		Logger::Debug("UpdateProduct uploadDocument() End....");
		return latestRevisionId;
	}

	/*
		* Description - uploadGLBFile() method used to Upload GLb file into PLM with .zip format
		* Parameter -
		* Exception -
		* Return -
		*/
	string UpdateProduct::uploadGLBFile(string _productId)
	{
		Logger::Debug("UpdateProduct uploadGLBFile() start....");
		string latestRevisionId;
		try
		{

			vector<pair<string, string>> headerNameAndValueList;
			headerNameAndValueList.push_back(make_pair("content-Type", "application/json"));
			headerNameAndValueList.push_back(make_pair("Cookie", Configuration::GetInstance()->GetBearerToken()));

			json bodyJson = json::object();
			string _3DModelFilePath = UTILITY_API->GetProjectFilePath();
			Helper::EraseSubString(_3DModelFilePath, UTILITY_API->GetProjectName());
			string m_GLBFilePath = _3DModelFilePath + UTILITY_API->GetProjectName();
			string FileName = UTILITY_API->GetProjectName() + ".zip";
			bodyJson["node_name"] = FileName;
			string bodyJsonString = to_string(bodyJson);
			string resultJsonString = RESTAPI::PostRestCall(bodyJsonString, Configuration::GetInstance()->GetPLMServerURL() + RESTAPI::DOCUMENT_CREATE_API + "/" + _productId, "content-type: application/json; boundary=----WebKitFormBoundary7MA4YWxkTrZu0gW");
			if (!FormatHelper::HasContent(resultJsonString))
			{
				throw "Unable to initiliaze Document Configuration. Please try again or Contact your System Administrator.";
			}
			json detailJson = Helper::GetJsonFromResponse(resultJsonString, "{");
			latestRevisionId = Helper::GetJSONValue<string>(detailJson, LATEST_REVISION_KEY, true);
		}
	
		catch (string msg)
		{
			Logger::Error("UpdateProduct uploadGLBFile() Exception - " + msg);
		}
		catch (exception & e)
		{
			Logger::Error("UpdateProduct uploadGLBFile() Exception - " + string(e.what()));
		}
		catch (const char* msg)
		{
			wstring wstr(msg, msg + strlen(msg));
			Logger::Error("UpdateProduct uploadGLBFile() Exception - " + string(msg));
		}
		
		return latestRevisionId;
		RESTAPI::SetProgressBarData(0, "", false);
		Logger::Debug("UpdateProduct uploadDocument() End....");
	}

	/*
	* Description - exportZPRJ() method used to export a 3D model and visual images.
	* Parameter -
	* Exception - using throw if anything gets fail.
	* Return -
	*/
	void UpdateProduct::exportZPRJ(string _revisedDocId)
	{
		Logger::Debug("Update product exportZPRJ() start....");
		QDir dir;
		string _3DModelFilePath = UTILITY_API->GetProjectFilePath();

		m_3DModelFileName = UTILITY_API->GetProjectName() + ".zprj";
		//UTILITY_API->DisplayMessageBox("m_3DModelFileName" + m_3DModelFileName);
		EXPORT_API->ExportZPrj(_3DModelFilePath, true);
		string postField = getPublishRequestParameter(_3DModelFilePath, m_3DModelFileName);
		//string resultJsonString = RESTAPI::PutRestCall(postField, Configuration::GetInstance()->GetPLMServerURL() + RESTAPI::DOCUMENT_UPLOAD_API + "/" + _revisionId + "/upload", "content-type: multipart/form-data; boundary=----WebKitFormBoundary7MA4YWxkTrZu0gW");
		string resultJsonString = RESTAPI::PutRestCall(postField, Configuration::GetInstance()->GetPLMServerURL() + RESTAPI::DOCUMENT_UPLOAD_API + "/" + _revisedDocId + "/upload", "content-type: multipart/form-data; boundary=----WebKitFormBoundary7MA4YWxkTrZu0gW");
		//UTILITY_API->DisplayMessageBox("Upload zprj completed"+ resultJsonString);
		Logger::Debug("Update product exportZPRJ() end....");
	}

	/*
	* Description - exportGLBFile() method used to export a GLB file.
	* Parameter -
	* Exception - using throw if anything gets fail.
	* Return -
	*/
	void UpdateProduct::exportGLBFile(string _revisedDocId)
	{
		Logger::Debug("Update product exportGLBFile() Start....");

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
		string resultJsonString = RESTAPI::PutRestCall(postField, Configuration::GetInstance()->GetPLMServerURL() + RESTAPI::DOCUMENT_UPLOAD_API + "/" + _revisedDocId + "/upload", "content-type: multipart/form-data; boundary=----WebKitFormBoundary7MA4YWxkTrZu0gW");
		
		Logger::Debug("Update product exportGLBFile() end....");
	}

	/*
	* Description - exportGLB() method used to export a 3D model and visual images.
	* Parameter - bool
	* Exception - using throw if anything gets fail.
	* Return -
	*/
	void UpdateProduct::exportGLB(bool _isGLBExportable)
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
	string UpdateProduct::collectCriteriaFields(QTreeWidget* _documentPushToPLMTree_1, QTreeWidget* _documentPushToPLMTree_2)
	{
		Logger::Debug("Update product collectCriteriaFields() start....");
		json attsJson = json::object();
		json attJson = json::object();
		string fieldAndValues = "{";
		string attkey;
		string attValue;
		string attType;
		string data = "{";
		for (int i = 0; i < _documentPushToPLMTree_1->topLevelItemCount(); i++)
		{
			attJson = ReadVisualUIFieldValue(_documentPushToPLMTree_1, i);

			if (!attJson.empty())
			{
				attkey = Helper::GetJSONValue<string>(attJson, "attKey", true);
				attValue = Helper::GetJSONValue<string>(attJson, "attValue", true);
				attType = Helper::GetJSONValue<string>(attJson, ATTRIBUTE_TYPE_KEY, true);
				if (!attValue.empty() && attValue != "|@isDisabled@|")
				{
					if (attType == "integer" || attType == "float") {
						data += "\n\"" + attkey + "\":" + attValue + ",";
					}
					//For RefList type of attributes, need to make it generic
					else if (attType == "choice" && attkey == "material_security_groups") {
						attsJson.push_back(attValue);
						Logger::Logger("attsJson================ " + to_string(attsJson));
						data += "\n\"" + attkey + "\":" + to_string(attsJson) + ",";
					}
					else {
						data += "\n\"" + attkey + "\":\"" + attValue + "\",";
					}
				}

			}
		}
		for (int i = 0; i < _documentPushToPLMTree_2->topLevelItemCount(); i++)
		{
			attJson = ReadVisualUIFieldValue(_documentPushToPLMTree_2, i);

			if (!attJson.empty())
			{
				attkey = Helper::GetJSONValue<string>(attJson, "attKey", true);
				attValue = Helper::GetJSONValue<string>(attJson, "attValue", true);
				attType = Helper::GetJSONValue<string>(attJson, ATTRIBUTE_TYPE_KEY, true);
				if (!attValue.empty() && attValue != "|@isDisabled@|")
				{
					if (attType == "integer" || attType == "float") {
						data += "\n\"" + attkey + "\":" + attValue + ",";
					}
					//For RefList type of attributes, need to make it generic
					else if (attType == "choice" && attkey == "material_security_groups") {
						attsJson.push_back(attValue);
						Logger::Logger("attsJson================ " + to_string(attsJson));
						data += "\n\"" + attkey + "\":" + to_string(attsJson) + ",";
					}
					else {
						data += "\n\"" + attkey + "\":\"" + attValue + "\",";
					}
				}

			}
		}
		//attJson["cus_cost_factors"] = "18";
		data = data.substr(0, data.length() - 1);
		data += "\n}";
		Logger::Debug("Update product collectCriteriaFields() end....");
		//UTILITY_API->DisplayMessageBox(fieldAndValues);
		return data;
	}


	/*
	* Description - onAddColorwaysClicked() method used to add color in colorway table
	* Parameter -
	* Exception -
	* Return - void.
	*/
	void UpdateProduct::onAddColorwaysClicked()
	{
		Logger::Debug("UpdateProduct -> onAddColorwaysClicked() -> Start");
		//ui_colorwayAddButton->setIcon(QIcon(":/CLOVISE_PLUGIN/INFOR/Images/icon_add_none.svg"));
		this->hide();

		if (ui_colorwayTable->rowCount() != 0)
		{
			GetUpdatedColorwayNames();
		}
		//SetProgressBarData(15, m_translationMap["pbLoadingColorSearch"][m_languageIndex], true);
		//PLMColorSearch::Destroy();
		bool isFromConstructor = false;
		if (!ColorConfig::GetInstance()->GetIsModelExecuted())
		{
			ColorConfig::GetInstance()->InitializeColorData();
			isFromConstructor = true;
		}
		//ColorConfig::GetInstance()->InitializeColorData();

		//ColorConfig::GetInstance()->GetColorConfigJSON();
		PLMColorSearch::GetInstance()->setModal(true);
		//m_isColorwayHasAccess = true;
		//PLMColorSearch::GetInstance()->ClearAllFields();
		PLMColorSearch::GetInstance()->DrawSearchWidget(true);
		UTILITY_API->DeleteProgressBar(true);
		ColorConfig::GetInstance()->SetIsModelExecuted(true);
		PLMColorSearch::GetInstance()->exec();
		RESTAPI::SetProgressBarData(0, "", false);
		//SetProgressBarData(0, "", false);
		Logger::Debug("UpdateProduct -> onAddColorwaysClicked() -> End");
	}


	/*
	* Description - AddColorwayDetails() method used to add color in colorway table
	* Parameter -QStringList _downloadIdList, json _jsonarray
	* Exception -
	* Return - void.
	*/
	void UpdateProduct::AddColorwayDetails(QStringList _downloadIdList, json _jsonarray)
	{

		Logger::Debug("UpdateProduct -> AddColorwayDetails() -> Start");
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
		string colorwayName = "";
		json jsonvalue = json::object();
		json columnNamejson = json::array();
		int colorResultColumnCount = 0;
		int count = 0;
		bool Isduplicate = false;
		int appendedRowCount = 0;
		string colorSpecification;
		string columnsNames = DirectoryUtil::GetPLMPluginDirectory() + "ColorwayTableColumns.json";//Reading Columns from json
		jsonvalue = Helper::ReadJSONFile(columnsNames);
		string colorwayTableColumns = Helper::GetJSONValue<string>(jsonvalue, "colorwayTableFieldList", false);
		columnNamejson = json::parse(colorwayTableColumns);
		//UTILITY_API->DisplayMessageBox(to_string(_downloadJson));
		//	json downloadJsonArray = Helper::GetJSONParsedValue<string>(_downloadJson, ATTACHMENTS_KEY, false);
		json downloadJsonArray = _jsonarray;
		m_colorwayRowcount = ui_colorwayTable->rowCount();
		string columndatafield = "";
		try
		{
			for (int index = 0; index < columnNamejson.size(); index++)
			{
				string columnname = Helper::GetJSONParsedValue<int>(columnNamejson, index, false);
				headerlist.insert(index, QString::fromStdString(columnname));
			}


			ui_colorwayTable->setColumnCount(headerlist.size());
			ui_colorwayTable->setHorizontalHeaderLabels(headerlist);
			if (!m_addClicked)
				ui_colorwayTable->setRowCount(downloadJsonArray.size());

			QStringList colorwayNamesList;
			string colorwayId = "";
			int colorwayCount = UTILITY_API->GetColorwayCount();
			colorwayNamesList.append(QString::fromStdString(BLANK));
			for (int count = 0; count < colorwayCount; count++)
			{
				string colorwayName = UTILITY_API->GetColorwayName(count);
				colorwayNamesList.append(QString::fromStdString(colorwayName));
			}
			//UTILITY_API->DisplayMessageBox(to_string(downloadJsonArray));
			m_colorwayNamesList = colorwayNamesList;
			for (int rowCount = 0; rowCount < downloadJsonArray.size(); rowCount++)
			{

				attachmentsJson = Helper::GetJSONParsedValue<int>(downloadJsonArray, rowCount, false);
				attId = Helper::GetJSONValue<string>(attachmentsJson, ATTRIBUTE_ID, true);
				if (_downloadIdList.contains(QString::fromStdString(attId)))
				{
					objectName = Helper::GetJSONValue<string>(attachmentsJson, ATTRIBUTE_NAME, true);
					//	objectName = Helper::Trim(objectName);
					pantone = Helper::GetJSONValue<string>(attachmentsJson, PANTONE_KEY, true);
					rgbValue = Helper::GetJSONValue<string>(attachmentsJson, RGB_VALUE_KEY, true);
					objectCode = Helper::GetJSONValue<string>(attachmentsJson, CODE_KEY, true);

					if (m_downloadedColorway)
					{
						objectName = Helper::GetJSONValue<string>(attachmentsJson, "color_name_name", true);
						attId = Helper::GetJSONValue<string>(attachmentsJson, COLOR_SPEC_ID, true);
						colorwayId = Helper::GetJSONValue<string>(attachmentsJson, ATTRIBUTE_ID, true);
						colorwayName = Helper::GetJSONValue<string>(attachmentsJson, ATTRIBUTE_NAME, true);
						m_downloadedColorwayIdMap.insert(make_pair(colorwayId, colorwayName));
					}
					Logger::Debug("UpdateProduct -> AddColorwayDetails() -> pantone: " + pantone);
					/*if (rgbValue.empty())
						continue;*/

					QString UniqueObjectId;
					QTableWidgetItem* item;

					/*if (m_addClicked)
					{

						for (int i = 0; i < ui_colorwayTable->rowCount(); i++)
						{
							QWidget* qWidget = ui_colorwayTable->cellWidget(i, 1);
							if (!qWidget)
							{
								continue;
							}
							QComboBox *temp = qobject_cast <QComboBox*> (qWidget->layout()->itemAt(0)->widget());
							if (temp != nullptr)
							{

								UniqueObjectId = temp->property("Id").toString();
								Logger::Debug("UpdateProduct -> AddColorwayDetails() -> UniqueObjectId" + UniqueObjectId.toStdString());
								Logger::Debug("UpdateProduct -> AddColorwayDetails() -> objectId" + attId);
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
						Logger::Debug("UpdateProduct -> AddColorwayDetails() -> Continue");
						continue;
					}
					else*/
					ui_colorwayTable->setRowCount(m_colorwayRowcount + count + 1);



					Logger::Debug("UpdateProduct -> AddColorwayDetails() -> m_colorwayRowcount" + to_string(m_colorwayRowcount));
					Logger::Debug("UpdateProduct -> AddColorwayDetails() -> count" + to_string(count));
					Logger::Debug("UpdateProduct -> AddColorwayDetails() -> row number" + to_string(m_colorwayRowcount + count));

					AddRows(count, attId, objectName, rgbValue, objectCode, pantone, colorwayNamesList, attachmentsJson, colorwayId);
					count++;
				}
			}
			m_addClicked = true;

			//ui_colorwayTable->hideColumn(CHECKBOX_COLUMN);
			m_colorwayRowcount = ui_colorwayTable->rowCount();

			for (int index = 0; index < headerlist.count(); index++)
			{
				if (index == CHECKBOX_COLUMN)
				{
					ui_colorwayTable->setColumnWidth(index, COLUMN_SIZE);
				}
				else if (index == CLO_COLORWAY_COLUMN || index == UNI_2_DIGIT_CODE_COLUMN)
				{
					ui_colorwayTable->resizeColumnToContents(1);
				}
				if (index == COLORWAY_DELETE_COLUMN)
				{
					Logger::Debug("entering eleseif COLORWAY_DELETE_COLUMN-");
					ui_colorwayTable->setColumnWidth(index, 50);
				}
				else
				{
					ui_colorwayTable->setColumnWidth(index, 80);
				}
			}
		}
		catch (string msg)
		{
			Logger::Error("UpdateProduct -> AddColorwayDetails() Exception - " + msg);
		}
		catch (exception & e)
		{
			Logger::Error("UpdateProduct -> AddColorwayDetails() Exception - " + string(e.what()));
		}
		catch (const char* msg)
		{
			wstring wstr(msg, msg + strlen(msg));
			Logger::Error("UpdateProduct -> AddColorwayDetails() Exception - " + string(msg));
		}
		//SetProgressBarData(0, "", false);
		Logger::Debug("UpdateProduct -> AddColorwayDetails() -> End");
	}


	bool UpdateProduct::ValidateColorwayNameField()
	{
		Logger::Debug("UpdateProduct -> ValidateColorwayNameField() -> Start");

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
					ui_tabWidget->setCurrentIndex(1);
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
						ui_tabWidget->setCurrentIndex(1);
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
					ui_tabWidget->setCurrentIndex(1);
					duplicateColrwayName = true;
					break;
				}
			}
		}

		list.removeDuplicates();
		UpdateImageIntent::GetInstance()->fillSelectedList(list);

		Logger::Debug("UpdateProduct -> ValidateColorwayNameField() -> End");
		return	!duplicateColrwayName;
	}

	void UpdateProduct::GetUpdatedColorwayNames()
	{
		Logger::Debug("UpdateProduct -> GetUpdatedColorwayNames() -> Start");
		m_modifiedColorwayNames.clear();
		int comboSize;
		int comboBoxIndex;;
		vector<int> comboBoxIndexList;
		Logger::Debug("UpdateProduct -> GetUpdatedColorwayNames() -> Start");
		int noOfRows = ui_colorwayTable->rowCount();
		Logger::Debug("UpdateProduct -> GetUpdatedColorwayNames() -> 1");
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
		Logger::Debug("UpdateProduct -> GetUpdatedColorwayNames() -> 2");
		m_modifiedColorwayNames.clear();
		QStringList colorwayNamesList;
		int colorwayCount = UTILITY_API->GetColorwayCount();
		colorwayNamesList.append(QString::fromStdString(BLANK));
		for (int count = 0; count < colorwayCount; count++)
		{
			string colorwayName = UTILITY_API->GetColorwayName(count);
			colorwayNamesList.append(QString::fromStdString(colorwayName));
		}
		Logger::Debug("UpdateProduct -> GetUpdatedColorwayNames() -> 3");
		for (int rowIndex = 0; rowIndex < ui_colorwayTable->rowCount(); rowIndex++)
		{
			Logger::Debug("UpdateProduct -> GetUpdatedColorwayNames() -> 4");
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
		Logger::Debug("UpdateProduct -> GetUpdatedColorwayNames() -> End");
	}

	void UpdateProduct::AddRows(int _count, string _objectId, string _objectName, string _rgbValue, string _code, string _pantone, QStringList _colorwayNamesList, json _colorwayJson, string _colorwayId)
	{

		Logger::Debug("UpdateProduct -> AddRows() -> Start");
		QWidget *pWidget = nullptr;
		QWidget *newQWidget = new QWidget();
		if (!m_downloadedColorway)
		{
			m_colorwayRowcount = ui_colorwayTable->rowCount();
			ui_colorwayTable->setRowCount(m_colorwayRowcount);
			ui_colorwayTable->insertRow(m_colorwayRowcount);
		}
		QPushButton* updateColorButton = CVWidgetGenerator::CreatePushButton("Update Color", "", "Update Color", PUSH_BUTTON_STYLE, 30, true);
		QPushButton* ColorCreateButton = new QPushButton();
		ColorCreateButton->setStyleSheet("QPushButton{max-height: 20px; max-width: 10px;} ::menu-indicator{ image: none; }");
		ColorCreateButton->setIcon(QIcon(":/CLOVise/PLM/Images/ui_spin_icon_minus_none.svg"));
		ColorCreateButton->setIconSize(QSize(25, 22));
		ColorCreateButton->setMaximumWidth(10);
		ColorCreateButton->setProperty(("row"), _count);
		QAction* colorSpecAction = new QAction(tr("Create Color Specification"), this);
		colorSpecAction->setProperty(("row"), _count);
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
		if (m_updateColorButtonSignalMapper != nullptr)
		{
			connect(updateColorButton, SIGNAL(clicked()), m_updateColorButtonSignalMapper, SLOT(map()));
			m_updateColorButtonSignalMapper->setMapping(updateColorButton, _count + m_colorwayRowcount);
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

		if (m_updateColorwayDeleteSignalMapper != nullptr)
		{
			connect(deleteButton, SIGNAL(clicked()), m_updateColorwayDeleteSignalMapper, SLOT(map()));
			//QPushButton *deleteButton = static_cast<QPushButton*>(ui_colorwayTable->cellWidget(index, COLORWAY_DELETE_COLUMN)->children().last());
			m_updateColorwayDeleteSignalMapper->setMapping(deleteButton, m_colorwayRowcount);
			for (int index = 0; index < ui_colorwayTable->rowCount(); index++)
			{
				m_updateColorwayDeleteSignalMapper->setMapping(deleteButton, index);
			}
		}

		ui_colorwayTable->setCellWidget(_count + m_colorwayRowcount, COLORWAY_DELETE_COLUMN, pdeleteWidget);
		ui_colorwayTable->setColumnWidth(COLORWAY_DELETE_COLUMN, 50);
		ui_colorwayTable->setWordWrap(true);

		ui_colorwayTable->setCellWidget(_count + m_colorwayRowcount, UPDATE_BTN_COLUMN, pWidget);
		ui_colorwayTable->setColumnWidth(UPDATE_BTN_COLUMN, COLUMN_SIZE);
		string defaultDescption;
		string defaultPLMColorwayName;
		string colorSpecId;
		string downloadedPLMColorwayName;
		string DefaultImageId;
		ComboBoxItem* comboColorwayItem = new ComboBoxItem();
		comboColorwayItem->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
		comboColorwayItem->setFocusPolicy(Qt::StrongFocus);
		comboColorwayItem->addItems(_colorwayNamesList);
		comboColorwayItem->setProperty("Id", QString::fromStdString(_objectId));

		ui_colorwayTable->setColumnWidth(CLO_COLORWAY_COLUMN, 150);
		if (m_downloadedColorway)
		{
			comboColorwayItem->setProperty("IsDownloadedColorway", "1");
			comboColorwayItem->setProperty("colorwayId", QString::fromStdString(_colorwayId));
			colorSpecId = Helper::GetJSONValue<string>(_colorwayJson, "color_specification", true);
			comboColorwayItem->setProperty("Id", QString::fromStdString(colorSpecId));
			defaultPLMColorwayName = Helper::GetJSONValue<string>(_colorwayJson, "uni_2_digit_code", true);
			defaultDescption = Helper::GetJSONValue<string>(_colorwayJson, "description", true);
			downloadedPLMColorwayName = Helper::GetJSONValue<string>(_colorwayJson, "node_name", true);
			json imageJson = Helper::GetJSONParsedValue<string>(_colorwayJson, IMAGES_JSON, false);
			Logger::Logger("imageJson ::" + to_string(imageJson));
			DefaultImageId = Helper::GetJSONValue<string>(imageJson, BLANK, true);
			Logger::Logger("DefaultImageId ::" + DefaultImageId);
			int valueIndex = comboColorwayItem->findText(QString::fromUtf8(QString::fromStdString(downloadedPLMColorwayName).toStdString().c_str()));
			if (valueIndex < 0)
			{
				valueIndex = comboColorwayItem->findText(QString::fromUtf8(QString::fromStdString(BLANK).toStdString().c_str()));
			}
			comboColorwayItem->setCurrentIndex(valueIndex);
		}
		else
		{
			comboColorwayItem->setProperty("IsDownloadedColorway", "0");
			comboColorwayItem->setProperty("NewColorway", QString::fromStdString(to_string(m_colorwayRowcount)));
			m_NewlyAddedColorway.append(QString::fromStdString(to_string(m_colorwayRowcount)));
		}
		pWidget = CVWidgetGenerator::InsertWidgetInCenter(comboColorwayItem);
		ui_colorwayTable->setCellWidget(_count + m_colorwayRowcount, CLO_COLORWAY_COLUMN, pWidget);
		QObject::connect(comboColorwayItem, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(OnHandleColorwayNameComboBox(const QString&)));
		QSize iconSize(40, 40);
		ui_colorwayTable->setIconSize(iconSize);
		ui_colorwayTable->setWordWrap(true);

		QLineEdit* lineEditItem = new QLineEdit();
		lineEditItem->setStyleSheet(LINEEDIT_STYLE);
		lineEditItem->setAttribute(Qt::WA_MacShowFocusRect, false);
		lineEditItem->setText("");
		QWidget *pLineEditWidget = CVWidgetGenerator::InsertWidgetInCenter(lineEditItem);
		ui_colorwayTable->setCellWidget(_count + m_colorwayRowcount, PLM_COLORWAY_COLUMN, pLineEditWidget);
		lineEditItem->setProperty("row", _count + m_colorwayRowcount);
		lineEditItem->setProperty("Edited", true);
		connect(lineEditItem, SIGNAL(editingFinished()), this, SLOT(OnplmColorwayNameEntered()));
		
		QTableWidgetItem* iconItem = new QTableWidgetItem;
		iconItem->setSizeHint(iconSize);

		Logger::Debug("colorRGB:: " + _rgbValue);

		QStringList listRGB;
		QImage Icon;
		QPixmap pixmap;
		string rgbValue = _rgbValue;
		rgbValue = Helper::FindAndReplace(_rgbValue, "(", "");
		rgbValue = Helper::FindAndReplace(rgbValue, ")", "");
		rgbValue = Helper::FindAndReplace(rgbValue, " ", "");
		QWidget *pColorWidget = nullptr;
		if (FormatHelper::HasContent(rgbValue))
		{
			QStringList listRGB;
			QString colorRGB = QString::fromStdString(rgbValue);
			listRGB = colorRGB.split(',');
			int red = listRGB.at(0).toInt();
			int green = listRGB.at(1).toInt();
			int blue = listRGB.at(2).toInt();
			QColor color(red, green, blue);
			QImage image(60, 60, QImage::Format_ARGB32);
			image.fill(color);			
			QLabel* label = new QLabel();
			pixmap = QPixmap::fromImage(image);
			label->setPixmap(QPixmap(pixmap));

			pColorWidget = CVWidgetGenerator::InsertWidgetInCenter(label);

		}
		else if (FormatHelper::HasContent(DefaultImageId))
		{
			Logger::Logger("else DefaultImageId ::" + DefaultImageId);
			QImage thumnail;
			string imageResponse = RESTAPI::CentricRestCallGet(Configuration::GetInstance()->GetPLMServerURL() + RESTAPI::PRINT_IMAGE_API + "/" + DefaultImageId, APPLICATION_JSON_TYPE, BLANK);
			json imageResponseJson = json::parse(imageResponse);

			string thumbnailUrl = Helper::GetJSONValue<string>(imageResponseJson, THUMBNAIL_KEY, true);
			Logger::Logger("thumbnailUrl ::" + thumbnailUrl);
			string latestVersionAttUrl = Helper::GetJSONValue<string>(imageResponseJson, "_url_base_template", true);

			latestVersionAttUrl = Helper::FindAndReplace(latestVersionAttUrl, "%s", thumbnailUrl);
			Logger::Logger("latestVersionAttUrl ::" + latestVersionAttUrl);
			if (FormatHelper::HasContent(latestVersionAttUrl))
			{
				QByteArray imageBytes;
				auto startTime = std::chrono::high_resolution_clock::now();
				imageBytes = Helper::DownloadImageFromURL(latestVersionAttUrl);
				auto finishTime = std::chrono::high_resolution_clock::now();
				std::chrono::duration<double> totalDuration = finishTime - startTime;
				Logger::perfomance(PERFOMANCE_KEY + "download Thambnail API :: " + to_string(totalDuration.count()));
				QBuffer buffer(&imageBytes);
				buffer.open(QIODevice::ReadOnly);
				QImageReader imageReader(&buffer);
				imageReader.setDecideFormatFromContent(true);
				thumnail = imageReader.read();
				if (thumnail.isNull())
				{
					Logger::Error("LOGGER::CVWidgetGenerator: CreateIconWidget() -> Image is not loaded.  ");
					QImageReader imageReader(":/CLOVise/PLM/Images/NoImage.png");
					imageReader.setDecideFormatFromContent(true);
					thumnail = imageReader.read();
				}
				pixmap = pixmap.fromImage(thumnail);
				pColorWidget = CVWidgetGenerator::GetInstance()->CreateThumbnailWidget(DefaultImageId, pixmap, PRINT_MODULE);
			}
		}
		ui_colorwayTable->setCellWidget(_count + m_colorwayRowcount, COLOR_CHIP_COLUMN, pColorWidget);


		if (_objectName == "null")
			_objectName = " ";
		QTableWidgetItem* ColorNameWidget = new QTableWidgetItem(QString::fromStdString(_objectName));
		ColorNameWidget->setTextAlignment(Qt::AlignCenter);
		ColorNameWidget->setToolTip(QString::fromStdString(_objectName));
		ui_colorwayTable->setItem(_count + m_colorwayRowcount, COLOR_NAME_COLUMN, ColorNameWidget);

		if (_code == "null")
			_code = " ";
		QTableWidgetItem* ColorCodeWidget = new QTableWidgetItem(QString::fromStdString(_code));
		ColorCodeWidget->setTextAlignment(Qt::AlignCenter);
		ColorCodeWidget->setToolTip(QString::fromStdString(_code));
		ui_colorwayTable->setItem(_count + m_colorwayRowcount, COLOR_CODE_COLUMN, ColorCodeWidget);

		if (_pantone == "null")
			_pantone = " ";
		QTableWidgetItem* ColorStatusWidget = new QTableWidgetItem(QString::fromStdString(_pantone));
		ColorStatusWidget->setTextAlignment(Qt::AlignCenter);
		ColorStatusWidget->setToolTip(QString::fromStdString(_pantone));
		ui_colorwayTable->setItem(_count + m_colorwayRowcount, PANTONE_CODE_COLUMN, ColorStatusWidget);

		ComboBoxItem* uni2digiCodes = new ComboBoxItem();
		uni2digiCodes->setFocusPolicy(Qt::StrongFocus);

		QStringList valueList;
		int indexOfSelectedString;

		for (auto it = m_digiCodeNamesMap.begin(); it != m_digiCodeNamesMap.end(); it++)
		{
			uni2digiCodes->setProperty(it->second.c_str(), it->first.c_str());
			uni2digiCodes->setProperty(it->first.c_str(), it->second.c_str());
		}
		uni2digiCodes->addItems(m_digiCodeValue);
		if (m_downloadedColorway)
		{
			string defaultDisplayname = uni2digiCodes->property(defaultPLMColorwayName.c_str()).toString().toStdString();
			indexOfSelectedString = uni2digiCodes->findText(QString::fromStdString(defaultDisplayname));
		}
		else
			indexOfSelectedString = uni2digiCodes->findText(QString::fromStdString(BLANK));

		uni2digiCodes->setCurrentIndex(indexOfSelectedString);
		pWidget = CVWidgetGenerator::InsertWidgetInCenter(uni2digiCodes);
		uni2digiCodes->setProperty("row", _count + m_colorwayRowcount);
		if (m_2DigiCodeActive)
		{
			connect(uni2digiCodes, SIGNAL(activated(const QString&)), this, SLOT(OnUni2CodeSelected(const QString&)));
		}
		ui_colorwayTable->setCellWidget(_count + m_colorwayRowcount, UNI_2_DIGIT_CODE_COLUMN, pWidget);

		QTextEdit* textEditItem = new QTextEdit();
		textEditItem->setStyleSheet(TEXTEDIT_STYLE);
		if (m_downloadedColorway)
			textEditItem->setText(QString::fromStdString(defaultDescption));
		else
			textEditItem->setText("");
		pWidget = CVWidgetGenerator::InsertWidgetInCenter(textEditItem);
		ui_colorwayTable->setCellWidget(_count + m_colorwayRowcount, DESCRIPTION_COLUMN, pWidget);

		lineEditItem->setStyleSheet(LINEEDIT_STYLE);
		if (m_downloadedColorway)
			lineEditItem->setText(QString::fromStdString(downloadedPLMColorwayName));
		else
			lineEditItem->setText("");
		pWidget = CVWidgetGenerator::InsertWidgetInCenter(lineEditItem);
		ui_colorwayTable->setCellWidget(_count + m_colorwayRowcount, PLM_COLORWAY_COLUMN, pWidget);


		Logger::Debug("UpdateProduct -> AddRows() -> End");
	}

	/*
	* Description - OnCreateColorSpecClicked() method is the slot for calling the create widget.
	* Parameter - int
	* Exception -
	* Return -
	*/
	void UpdateProduct::OnCreateColorSpecClicked(int _selectedRow)
	{
		Logger::Info("INFO::UpdateProduct -> OnCreateColorSpecClicked() -> Start");
		Configuration::GetInstance()->SetIsUpdateColorClicked(true);
		m_currentColorSpec = BLANK;
		m_selectedRow = _selectedRow;
		QComboBox *colorwayNameCombo = static_cast<QComboBox*>(ui_colorwayTable->cellWidget(m_selectedRow, CLO_COLORWAY_COLUMN)->children().last());
		string colorSpecId = colorwayNameCombo->property("Id").toString().toStdString();
		if (!colorSpecId.empty())
			m_currentColorSpec = colorSpecId;
		ColorConfig::GetInstance()->m_mode = "Create";
		ColorConfig::GetInstance()->m_isSearchColor = false;
		onAddColorwaysClicked();
		Logger::Info("INFO::UpdateProduct -> OnCreateColorSpecClicked() -> Start");
	}

	void UpdateProduct::OnplmColorwayNameEntered()
	{
		sender()->setProperty("Edited", true);
	}

	void UpdateProduct::OnUni2CodeSelected(const QString& _str)
	{
		int nRow = sender()->property("row").toInt();
		//UTILITY_API->DisplayMessageBox("nRow is " + to_string(nRow));
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

	/*
	* Description - OnSearchPrintClicked() method is the slot for calling the search print widget.
	* Parameter - int
	* Exception -
	* Return -
	*/
	void UpdateProduct::OnSearchPrintClicked(int _SelectedRow)
	{
		Logger::Info("INFO::UpdateProduct -> OnSearchPrintClicked() -> Start");
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
		Logger::Info("INFO::UpdateProduct -> OnSearchPrintClicked() -> Start");
	}
	void UpdateProduct::onSaveAndCloseClicked()
	{
		Logger::Debug("UpdateProduct -> SaveClicked() -> Start");
		m_isSaveClicked = true;		
		
		Configuration::GetInstance()->SetIsPrintSearchClicked(false);

		GetUpdatedColorwayNames();
		for (int row = 0; row < ui_colorwayTable->rowCount(); row++)
		{
			QComboBox *colorwayNameCombo = static_cast<QComboBox*>(ui_colorwayTable->cellWidget(row, CLO_COLORWAY_COLUMN)->children().last());
			QLineEdit *plmColorwayName = static_cast<QLineEdit*>(ui_colorwayTable->cellWidget(row, PLM_COLORWAY_COLUMN)->children().last());
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
		}
		//if (ExtractAllUIValues())
		{
			//CreateProductCreateMap();
			//m_isUserInputEmpty = true;
			ui_tabWidget->setCurrentIndex(0);
			ui_tabWidget->setStyleSheet(" QTabWidget::pane { border: none; color: #FFFFFF; font-size: 10px; background-color: #262628; }""QTabBar::tab { width: 100px; padding: 2px; }""QTabBar::tab:selected { border: none; color: #FFFFFF; background-color: \"" + DEFAULT_TAB_BG_COLOR + "\"; }""QTabBar::tab:!selected { color:#FFFFFF; background-color:\"" + SELECTED_TAB_BG_COLOR + "\"; }");
			this->hide();
			if (UTILITY_API)
				UTILITY_API->DisplayMessageBox(Configuration::GetInstance()->GetLocalizedStyleClassName() + " Metadata Saved");
		}
		QDir dir;
		Helper::RemoveDirectory(QString::fromStdString(Configuration::GetInstance()->TURNTABLE_IMAGES_WITH_AVATAR_TEMP_DIRECTORY));
		dir.mkpath(QString::fromStdString(Configuration::GetInstance()->TURNTABLE_IMAGES_WITH_AVATAR_TEMP_DIRECTORY));
		Helper::RemoveDirectory(QString::fromStdString(Configuration::GetInstance()->TURNTABLE_IMAGES_WITHOUT_AVATAR_TEMP_DIRECTORY));
		dir.mkpath(QString::fromStdString(Configuration::GetInstance()->TURNTABLE_IMAGES_WITHOUT_AVATAR_TEMP_DIRECTORY));
		UpdateProductBOMHandler::GetInstance()->BackupBomDetails();
		Logger::Debug("UpdateProduct -> SaveClicked() -> End");
	}

	void UpdateProduct::onTabClicked(int _index)
	{
		Logger::Debug("UpdateProduct onTabClicked() Start");
		QStringList list;
		bool duplicateColrwayName = false;
		if (_index == COLORWAY_TAB)
		{
			ui_colorwayTable->setColumnCount(m_ColorwayTableColumnNames.size());
			ui_colorwayTable->setHorizontalHeaderLabels(m_ColorwayTableColumnNames);
			ui_colorwayTable->show();
		}
		if (_index == IMAGE_INTENT_TAB /*&& m_colorwayRowcount > 0*/)//Image Intent tab
		{
			m_imageIntentTable->setColumnCount(m_ImageIntentsColumnsNames.size());
			m_imageIntentTable->setHorizontalHeaderLabels(m_ImageIntentsColumnsNames);
			ValidateColorwayNameField();
			
		}

		if (_index == BOM_TAB)
		{
			
			Logger::Debug("UpdateProduct onTabClicked() 1");
				if (m_isSaveClicked && m_updateBomTab && UpdateProductBOMHandler::GetInstance()->IsBomCreated())
				{
					Logger::Debug("UpdateProduct onTabClicked() 2");
					UpdateProductBOMHandler::GetInstance()->RestoreBomDetails();
					m_updateBomTab = false;
				}
				GetMappedColorway();
				UpdateProductBOMHandler::GetInstance()->UpdateColorwayColumnsInBom();

		}
		RESTAPI::SetProgressBarData(0, "", false);
		Logger::Debug("UpdateProduct onTabClicked() End");

	}

	void UpdateProduct::RefreshImageIntents()
	{
		int imageRowCount = m_imageIntentTable->rowCount();
		m_imageIntentTable->setColumnCount(m_ImageIntentsColumnsNames.size());
		m_imageIntentTable->setHorizontalHeaderLabels(m_ImageIntentsColumnsNames);

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
			string viewName;
			int view;
			int rowCount = m_imageIntentTable->rowCount();
			string temporaryPath = UTILITY_API->GetCLOTemporaryFolderPath();

			for (int index = 0; index < rowCount; index++)
			{

				QTableWidgetItem* item = m_imageIntentTable->item(index, COLORWAY_COLUMN);
				Logger::Debug("UpdateProduct -> onTabClicked() -> Item" + item->text().toStdString());
				colorwayName = item->text().toStdString();

				Logger::Debug("UpdateProduct -> onTabClicked() -> clorwayname" + colorwayName);


				QTableWidgetItem* viewItem = m_imageIntentTable->item(index, IMAGE_VIEW_COLUMN);
				Logger::Debug("UpdateProduct -> onTabClicked() -> Item" + viewItem->text().toStdString());
				string ViewText;
				ViewText = viewItem->text().toStdString();
				int length = ViewText.length();
				int indexOfColon = ViewText.find(":");
				viewName = ViewText.substr(indexOfColon + 1, length);
				Logger::Debug("UpdateProduct -> onTabClicked() -> viewName" + viewName);

				if (viewName.find("Back") != -1)
					view = BACK_VIEW;
				else if (viewName.find("Front") != -1)
					view = FRONT_VIEW;
				else if (viewName.find("Left") != -1)
					view = LEFT_VIEW;
				else
					view = RIGHT_VIEW;

				QPushButton *deleteButton = static_cast<QPushButton*>(m_imageIntentTable->cellWidget(index, DELETE_COLUMN)->children().last());
				string includeAvatar = deleteButton->property("includeAvatar").toString().toStdString();				
				Logger::Debug("UpdateProduct -> refreshImageIntents() -> includeAvatar" + includeAvatar);

				QString filepath;
				if (includeAvatar == "Yes")
					filepath = QString::fromStdString(temporaryPath) + "CLOViseTurntableImages/WithAvatar/Avatar_" + QString::fromStdString(colorwayName) + "_" + QString::fromStdString(to_string(view)) + ".png";
				else
					filepath = QString::fromStdString(temporaryPath) + "CLOViseTurntableImages/WithoutAvatar/" + QString::fromStdString(colorwayName) + "_" + QString::fromStdString(to_string(view)) + ".png";

				Logger::Debug("UpdateProduct -> refreshImageIntents() -> filepath" + filepath.toStdString());

				QPixmap pix(filepath);
				pix.scaled(QSize(80, 80), Qt::KeepAspectRatio);
				QWidget *pColorWidget = nullptr;
				QLabel* label = new QLabel();
				label->setMaximumSize(QSize(80, 80));
				int w = label->width();
				int h = label->height();
				label->setPixmap(QPixmap(pix.scaled(w, h, Qt::KeepAspectRatio)));
				pColorWidget = CVWidgetGenerator::InsertWidgetInCenter(label);
				m_imageIntentTable->setCellWidget(index, IMAGE_INTENT_COLUMN, pColorWidget);

			}
		}
	}

	void UpdateProduct::OnHandleDropDownValue(const QString& _item)
	{

		Logger::Debug("Update product OnHandleDropDownValue() Start");
		map<string, string>::iterator it;
		map < string, string> nameIdMap;
		string id;

		QString labelName = sender()->property("LabelName").toString();
		string apiUrl = "";
		QString comboboxtofill = "";

		Logger::Debug("UpdateProduct -> OnHandleDropDownValue() LabelName: " + labelName.toStdString());
		string progressbarText;
		Logger::Debug("PublishToPLMData -> OnHandleDropDownValue() _item: " + _item.toStdString());
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
				clearDependentComboBox(labelName, m_updateProductTreeWidget_1);
				clearDependentComboBox(labelName, m_updateProductTreeWidget_2);
				progressbarText = "Loading Brand details..";
			}
			else if (labelName == "Brand")
			{
				Logger::Debug("UpdateProduct -> OnHandleDropDownValue() Brand combobox Start ");
				apiUrl = RESTAPI::DEPARTMENT_DETAIL_API;
				Logger::Debug("UpdateProduct -> OnHandleDropDownValue() m_brandNameIdMap " + to_string(m_brandNameIdMap.size()));
				nameIdMap = m_brandNameIdMap;
				dependentFields.append("category_2");
				clearDependentComboBox(labelName, m_updateProductTreeWidget_1);
				clearDependentComboBox(labelName, m_updateProductTreeWidget_2);
				progressbarText = "Loading Department details..";
				Logger::Debug("UpdateProduct -> OnHandleDropDownValue() Brand combobox  End");
			}
			else if (labelName == "Department")
			{
				Logger::Debug("UpdateProduct -> OnHandleDropDownValue() Department combobox Start ");
				apiUrl = RESTAPI::COLLECTION_DETAIL_API;
				Logger::Debug("UpdateProduct -> OnHandleDropDownValue() m_brandNameIdMap " + to_string(m_dipartmentNameIdMap.size()));
				nameIdMap = m_brandNameIdMap;
				dependentFields.append("collection");
				clearDependentComboBox(labelName, m_updateProductTreeWidget_1);
				clearDependentComboBox(labelName, m_updateProductTreeWidget_2);
				progressbarText = "Loading Collection details..";
				Logger::Debug("UpdateProduct -> OnHandleDropDownValue() Department combobox  End");
			}
			else if (labelName == "Collection")
			{
				m_collectionId = sender()->property(_item.toStdString().c_str()).toString().toStdString();
				Logger::Debug("UpdateProduct -> OnHandleDropDownValue() Department Collection  End");
			}


			it = nameIdMap.find(_item.toStdString());//
			if (it != nameIdMap.end())
			{
				id = it->second;


				string attName, attId;

				Logger::Debug("PublishToPLMData -> SetDocumentConfigJSON Season Id: " + id);
				Logger::Debug("PublishToPLMData -> SetDocumentConfigJSON valueList size: " + to_string(dependentFields.size()));
				for (int itemIndex = 0; itemIndex < m_updateProductTreeWidget_1->topLevelItemCount(); ++itemIndex)
				{
					QTreeWidgetItem* topItem = m_updateProductTreeWidget_1->topLevelItem(itemIndex);
					QWidget* qWidgetColumn_0 = m_updateProductTreeWidget_1->itemWidget(topItem, 0);
					QWidget* qWidgetColumn_1 = m_updateProductTreeWidget_1->itemWidget(topItem, 1);
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
								dependentFieldJson = RESTAPI::makeRestcallGet(RESTAPI::SHAPE_API, "&skip=0&limit=100", "?shape_seasons=" + id, progressbarText);

							}
							else if (lableText == "theme")
							{
								progressbarText = "Loading Theme details..";
								dependentFieldJson = RESTAPI::makeRestcallGet(RESTAPI::THEME_API, "&skip=0&limit=100", "?theme_seasons=" + id, progressbarText);

							}
							else
							{
								dependentFieldJson = RESTAPI::makeRestcallGet(apiUrl, "/hierarchy", "/" + id, progressbarText);
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
				for (int itemIndex = 0; itemIndex < m_updateProductTreeWidget_2->topLevelItemCount(); ++itemIndex)
				{
					QTreeWidgetItem* topItem = m_updateProductTreeWidget_2->topLevelItem(itemIndex);
					QWidget* qWidgetColumn_0 = m_updateProductTreeWidget_2->itemWidget(topItem, 0);
					QWidget* qWidgetColumn_1 = m_updateProductTreeWidget_2->itemWidget(topItem, 1);
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
								dependentFieldJson = RESTAPI::makeRestcallGet(RESTAPI::SHAPE_API, "&skip=0&limit=100", "?shape_seasons=" + id, progressbarText);

							}
							else if (lableText == "theme")
							{
								progressbarText = "Loading Theme details..";
								dependentFieldJson = RESTAPI::makeRestcallGet(RESTAPI::THEME_API, "&skip=0&limit=100", "?theme_seasons=" + id, progressbarText);

							}
							else
							{
								dependentFieldJson = RESTAPI::makeRestcallGet(apiUrl, "/hierarchy", "/" + id, progressbarText);
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


		Logger::Debug("Update product OnHandleDropDownValue() End");
	}

	json UpdateProduct::ReadVisualUIFieldValue(QTreeWidget* _searchTree, int _rowIndex)
	{
		Logger::Debug("Update product ReadVisualUIFieldValue() Start");
		string fieldValue = "";
		string fieldLabel = "";
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
		bool isFieldDisabled = false;
		QWidget* qWidgetColumn0 = _searchTree->itemWidget(topItem, 0);
		QLabel* qlabel = qobject_cast<QLabel*>(qWidgetColumn0);
		labelText = qlabel->text().toStdString();
		fieldLabel = qlabel->property(attKey.c_str()).toString().toStdString();
		attType = qlabel->property(ATTRIBUTE_TYPE_KEY.c_str()).toString().toStdString();
		QWidget* qWidgetColumn1 = _searchTree->itemWidget(topItem, 1);

		if (!qWidgetColumn1->isEnabled())
		{
			isFieldDisabled = true;
			fieldValue = "|@isDisabled@|";
		}
		if (!isFieldDisabled)
		{

			if (QLineEdit* qLineEditC1 = qobject_cast<QLineEdit*>(qWidgetColumn1))
			{
				fieldValue = qLineEditC1->text().toStdString();
				//fieldLabel = qLineEditC1->property("InternalName").toString().toStdString();
				Logger::Debug("Update product ReadVisualUIFieldValue() QLineEdit->fieldValue" + fieldValue);
				Logger::Debug("Update product ReadVisualUIFieldValue() QLineEdit->fieldLabel" + fieldLabel);
				Logger::Debug("Update product ReadVisualUIFieldValue() QLineEdit->labelText" + labelText);

				fieldVal = qLineEditC1->property(fieldValue.c_str()).toString().toStdString();
				Logger::Debug("Update product ReadVisualUIFieldValue() QLineEdit->fieldVal" + fieldVal);
				if (!fieldVal.empty())
					fieldValue = fieldVal;
				//if (labelText == "Style")
				//	_data += "\n\"" + fieldLabel + "\":\"" + fieldValue + "\",";
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
				Logger::Debug("Update product ReadVisualUIFieldValue() QComboBox->fieldValue" + fieldValue);
				Logger::Debug("Update product ReadVisualUIFieldValue() QComboBox->fieldLabel" + fieldLabel);
				Logger::Debug("Update product ReadVisualUIFieldValue() QComboBox->labelText" + labelText);

				fieldVal = qComboBoxC1->property(fieldValue.c_str()).toString().toStdString();
				Logger::Debug("Update product ReadVisualUIFieldValue() QComboBox->fieldVal" + fieldVal);
			if (fieldLabel == "bom_template")
				m_bomTemplateName->setText(QString::fromStdString(fieldValue));
				if (!fieldVal.empty())
				{
					fieldValue = fieldVal;
				}
			}
		}

		if (!FormatHelper::HasContent(fieldValue))
		{
			fieldValue = BLANK;
		}
		attJson[ATTRIBUTE_KEY] = fieldLabel;
		attJson[ATTRIBUTE_VALUE_KEY] = fieldValue;
		//data += "\n\"" + fieldLabel + "\":\"" + fieldValue + "\",";
		//_attJson[fieldLabel] = fieldValue;
		//	attJson[ATTRIBUTE_VALUE_KEY] = fieldValue;

		Logger::Debug("Update product ReadVisualUIFieldValue() End");
		return attJson;
	}

	void UpdateProduct::ClearAllFields(QTreeWidget* _documentPushToPLMTree)
	{
		Logger::Debug("UpdateProduct -> ClearAllFields() -> Start");
		try {
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

				QString labelId = qlabel->property("Id").toString();
				if (qlineedit)
				{
					QString defaultValue = qlineedit->property(ATTRIBUTE_INITIAL_VALUE.c_str()).toString();
					if (defaultValue.isEmpty())
						defaultValue = "";
					qlineedit->setText(defaultValue);
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
				else if (qComboBoxItem)
				{

					QString defaultValue = qComboBoxItem->property(ATTRIBUTE_INITIAL_VALUE.c_str()).toString();
					int valueIndex = qComboBox->findText(defaultValue);

					if (valueIndex == -1) // -1 for not found
					{
						valueIndex = qComboBox->findText(QString::fromStdString(BLANK));
					}

					qComboBox->setCurrentIndex(valueIndex);
				}
				else if (qspinBox)
				{

					QString defaultValue = qspinBox->property(ATTRIBUTE_INITIAL_VALUE.c_str()).toString();
					if (!defaultValue.isEmpty())
						qspinBox->setValue(stoi(defaultValue.toStdString()));
				}
			}
		}
		catch (string msg)
		{
			UTILITY_API->DeleteProgressBar(true);
			Logger::Error("ERROR::UpdateProduct: ClearAllFields()-> Exception string:: " + msg);
			this->show();
		}
		catch (exception& e)
		{
			UTILITY_API->DeleteProgressBar(true);
			Logger::Error("ERROR::UpdateProduct: ClearAllFields()-> Exception e:: " + string(e.what()));
			this->show();
		}
		catch (const char* msg)
		{
			UTILITY_API->DeleteProgressBar(true);
			Logger::Error("ERROR::UpdateProduct: ClearAllFields()-> Exception char:: " + string(msg));
			wstring wstr(msg, msg + strlen(msg));
			this->show();
		}

		Logger::Debug("UpdateProduct -> ClearAllFields() -> End");
	}

	//void UpdateProduct::SetColorwayDetails(QStringList _downloadIdList, json _jsonarray);
	//{
	//	Logger::Debug("UpdateProduct -> SetDownloadedColorwayDetails() -> Start");
	//	json jsonvalue = json::object();
	//	string columnsNames = DirectoryUtil::GetPLMPluginDirectory() + "CreateColorwayDetail.json";//Reading Columns from json
	//	jsonvalue = Helper::ReadJSONFile(columnsNames);
	//	AddColorwayDetails(jsonvalue, _downloadIdList);
	//	Logger::Debug("UpdateProduct -> SetDownloadedColorwayDetails() -> End");
	//}

	void UpdateProduct::CreateAndUpdateColorways(string _productId)
	{
		Logger::Debug("UpdateProduct -> CreateAndUpdateColorways () Start");

		string twoDigitCode;
		string response;

		int rowCount = ui_colorwayTable->rowCount();
		for (int row = 0; row < rowCount; row++)
		{

			string data = "{";
			//QComboBox* colorwayNameCombo = qobject_cast<QComboBox*>(ui_colorwayTable->cellWidget(row, 1));
			QComboBox *colorwayNameCombo = static_cast<QComboBox*>(ui_colorwayTable->cellWidget(row, CLO_COLORWAY_COLUMN)->children().last());
			//QLineEdit* colorwayName1 = qobject_cast<QLineEdit*>(ui_colorwayTable->cellWidget(row, 6));
			QComboBox *colorwayName1 = static_cast<QComboBox*>(ui_colorwayTable->cellWidget(row, UNI_2_DIGIT_CODE_COLUMN)->children().last());
			//QTextEdit* description = qobject_cast<QTextEdit*>(ui_colorwayTable->cellWidget(row, 7));
			QTextEdit *description = static_cast<QTextEdit*>(ui_colorwayTable->cellWidget(row, DESCRIPTION_COLUMN)->children().last());
			QLineEdit *plmColorwayName = static_cast<QLineEdit*>(ui_colorwayTable->cellWidget(row, PLM_COLORWAY_COLUMN)->children().last());

			if (colorwayNameCombo)
			{
				QString id = colorwayNameCombo->property("Id").toString();
				QString isDownloadedColorway = colorwayNameCombo->property("IsDownloadedColorway").toString();
				vector<pair<string, string>> headerNameAndValueList;
				json request = json::object();



				if (m_2DigiCodeActive)
				{
					if (!colorwayName1->currentText().toStdString().empty())
						twoDigitCode = colorwayName1->property(colorwayName1->currentText().toStdString().c_str()).toString().toStdString();
				}


				//UTILITY_API->DisplayMessageBox(to_string(request));

				headerNameAndValueList.push_back(make_pair("content-Type", "application/json"));
				headerNameAndValueList.push_back(make_pair("Cookie", Configuration::GetInstance()->GetBearerToken()));

				if (isDownloadedColorway == "1")
				{

					Logger::Debug("UpdateProduct -> CreateAndUpdateColorways () id::" + id.toStdString());

					data += "\n\"color_specification\":\"" + id.toStdString() + "\",";
					if (m_2DigiCodeActive)
						data += "\n\"uni_2_digit_code\":\"" + twoDigitCode + "\",";
					data += "\n\"description\":\"" + description->toPlainText().toStdString() + "\",";
					data += "\n\"node_name\":\"" + plmColorwayName->text().toStdString();
					data += "\"\n}";
					//UTILITY_API->DisplayMessageBox("data..." + data);
					string colorwayId = colorwayNameCombo->property("colorwayId").toString().toStdString();
					//UTILITY_API->DisplayMessageBox("colorwayId..." + colorwayId);
					response = RESTAPI::PutRestCall(data, Configuration::GetInstance()->GetPLMServerURL() + RESTAPI::COLORWAY_API + "/" + colorwayId, "content-type: application/json");
					// UTILITY_API->DisplayMessageBox("response..." + response);
				}
				else
				{
					request["color_specification"] = id.toStdString();
					if (m_2DigiCodeActive)
						request["uni_2_digit_code"] = twoDigitCode;
					request["description"] = description->toPlainText().toStdString();
					request["node_name"] = plmColorwayName->text().toStdString();
					string requestString = to_string(request);
				response = RESTAPI::PostRestCall(requestString, Configuration::GetInstance()->GetPLMServerURL() + RESTAPI::STYLE_ENDPOINT_API + "/" + _productId + "/product_colors", "content-type: application/json; boundary=----WebKitFormBoundary7MA4YWxkTrZu0gW");
				}
				int colorwayCount = UTILITY_API->GetColorwayCount();
				QString selectedCLOColorwayName = colorwayNameCombo->currentText();

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

				ColorwayViews colorwayView;
				QString selectedColorway = colorwayNameCombo->currentText();
				auto it = UpdateImageIntent::GetInstance()->m_ColorwayViewMap.find(selectedColorway.toStdString());// Searching unit in map

				if (it != UpdateImageIntent::GetInstance()->m_ColorwayViewMap.end())
				{
					it->second.colorwayId = colorwayId;
				}

			}
		}
		Logger::Debug("UpdateProduct -> CreateAndUpdateColorways () End");

	}

	void UpdateProduct::onAddImageIntentClicked()
	{
		this->hide();
		UpdateImageIntent::GetInstance()->m_imageQueueTableRowCount = 0;
		UpdateImageIntent::GetInstance()->m_imageQueueTable->setRowCount(0);
		UpdateImageIntent::GetInstance()->m_imageQueueTable->setColumnCount(0);
		UpdateImageIntent::GetInstance()->m_imageQueueTable->clear();
		UpdateImageIntent::GetInstance()->m_colorwayComboBox->setDisabled(false);
		UpdateImageIntent::GetInstance()->m_colorwayComboBox->setStyleSheet(COMBOBOX_STYLE);
		if (UpdateImageIntent::GetInstance()->m_includeAvatarCheckBox->isChecked())
		{
			UpdateImageIntent::GetInstance()->m_includeAvatarCheckBox->setAutoExclusive(false);
			UpdateImageIntent::GetInstance()->m_includeAvatarCheckBox->setChecked(false);
			UpdateImageIntent::GetInstance()->m_includeAvatarCheckBox->setAutoExclusive(true);
		}
		UpdateImageIntent::GetInstance()->setModal(true);
		UpdateImageIntent::GetInstance()->exec();

	}


	//@ToDo - fix the count based on the m_imageIntentTable here
	void  UpdateProduct::SetTotalImageCount()
	{
		Logger::Debug("UpdateProduct -> SetTotalImageCount () Start");
		QString label = "Total Count: " + QString::fromStdString(to_string((m_imageIntentTable->rowCount())));
		m_totalCountLabel->setText(label);
		//m_ImageIntentList->sortItems(Qt::SortOrder::AscendingOrder); @ToDo is sorting needed here?
		Logger::Debug("UpdateProduct -> SetTotalImageCount () End");
	}

	void UpdateProduct::uploadColorwayImages()
	{
		Logger::Debug("UpdateProduct -> uploadColorwayImages () Start");
		map<string, ColorwayViews>::iterator colorwayIterator;
		string temporaryPath = UTILITY_API->GetCLOTemporaryFolderPath();
		ColorwayViews colorwayViewObj;
		if (UpdateImageIntent::GetInstance()->m_ColorwayViewMap.size() > 0)
		{
			Logger::Debug("UpdateProduct -> uploadColorwayImages () Start");
			for (auto colorwayIterator = UpdateImageIntent::GetInstance()->m_ColorwayViewMap.begin(); colorwayIterator != UpdateImageIntent::GetInstance()->m_ColorwayViewMap.end(); colorwayIterator++)
			{
				string str = colorwayIterator->first;
				/*if (str.compare("No Colorway(Default)") == 0)
				{
					int currentColorwayIndex = UTILITY_API->GetCurrentColorwayIndex();
					str = UTILITY_API->GetColorwayName(currentColorwayIndex);
				}*/

				Logger::Debug("UpdateProduct -> uploadColorwayImages () str" + str);
				for (int i = 0; i < 4; i++)
				{
					Logger::Debug("UpdateProduct -> uploadColorwayImages () colorwayIterator->second.viewUploadId[i]" + colorwayIterator->second.viewUploadId[i]);
					if (colorwayIterator->second.viewUploadId[i].compare("") != 0)
					{
						string filepath;
						string postField;
						if (colorwayIterator->second.includeAvatar[i] == 1)
						{
							filepath = temporaryPath + "CLOViseTurntableImages/WithAvatar/Avatar_" + str + "_" + to_string(i) + ".png";
							postField = postField = getPublishRequestParameter(filepath, "Avatar_" + str + "_" + to_string(i) + ".png");
						}
						else
						{
							filepath = temporaryPath + "CLOViseTurntableImages/WithoutAvatar/" + str + "_" + to_string(i) + ".png";
							postField = getPublishRequestParameter(filepath, str + "_" + to_string(i) + ".png");
						}

						string resultJsonString;


						resultJsonString = RESTAPI::PostRestCall(postField, Configuration::GetInstance()->GetPLMServerURL() + RESTAPI::UPLOAD_IMAGE_API, "content-type: multipart/form-data; boundary=----WebKitFormBoundary7MA4YWxkTrZu0gW");
						json detailJson = Helper::GetJsonFromResponse(resultJsonString, "{");
						//UTILITY_API->DisplayMessageBox(to_string(detailJson));
						Logger::Debug("UpdateProduct -> uploadColorwayImages () 4");
						string imageId = Helper::GetJSONValue<string>(detailJson, ATTRIBUTE_ID, true);

						colorwayIterator->second.viewUploadId[i] = imageId;
						Logger::Debug("UpdateProduct -> uploadColorwayImages () imageId" + imageId);
						//UTILITY_API->DisplayMessageBox("imageId: " + imageId);
					}
				}

			}
		}
		Logger::Debug("UpdateProduct -> uploadColorwayImages () End");
	}
	void UpdateProduct::LinkImagesToColorways(string _productId)
	{
		Logger::Debug("UpdateProduct -> LinkImagesToColorways () Start");
		Logger::Debug("UpdateProduct -> LinkImagesToColorways () m_ColorwayViewMap.size()" + to_string(UpdateImageIntent::GetInstance()->m_ColorwayViewMap.size()));
		map<QString, QStringList>::iterator itr;
		if (UpdateImageIntent::GetInstance()->m_ColorwayViewMap.size() > 0)
		{
			map<QString, QString>::iterator it;
			for (auto colorwayIterator = UpdateImageIntent::GetInstance()->m_ColorwayViewMap.begin(); colorwayIterator != UpdateImageIntent::GetInstance()->m_ColorwayViewMap.end(); colorwayIterator++)
			{
				bool colorwayImageAdded = false;
				bool defaultImageAdded = false;
				string str = colorwayIterator->first;
				Logger::Debug("UpdateProduct -> LinkImagesToColorways() -> str" + str);

				//string data = "{\n\"images\":{\n\"\":\"C397197\",\n\"1\":\"C397197\",\n\"2\":\"C397222\",\n\"3\":\"C397226\"\n}\n}";
				string data = "{\n\"images\":{";
				Logger::Debug("UpdateProduct -> LinkImagesToColorways () 1");
				if (colorwayIterator->second.defaultImage != -1)
				{

					Logger::Debug("UpdateProduct -> LinkImagesToColorways () 2");
					if (!colorwayIterator->second.viewUploadId[colorwayIterator->second.defaultImage].empty())
					{
						colorwayImageAdded = true;
						defaultImageAdded = true;
						data += "\n\"\":\"" + colorwayIterator->second.viewUploadId[colorwayIterator->second.defaultImage] + "\",";
						Logger::Debug("UpdateProduct -> LinkImagesToColorways () ===========-----1   " + data);
					}
				}
				Logger::Debug("UpdateProduct -> LinkImagesToColorways () 3");
				for (int i = 0; i < 4; i++)
				{
					Logger::Debug("UpdateProduct -> LinkImagesToColorways () colorwayIterator->second.viewUploadId[i]" + colorwayIterator->second.viewUploadId[i]);
					if (!colorwayIterator->second.viewUploadId[i].empty())
					{
						Logger::Debug("UpdateProduct -> LinkImagesToColorways () 5");
						colorwayImageAdded = true;
						data += "\n\"" + to_string(i + 1) + "\":\"" + colorwayIterator->second.viewUploadId[i] + "\",";

						for (int labelIterator = 0; labelIterator < colorwayIterator->second.viewLabelMap[i].size(); labelIterator++)
						{
							if (colorwayIterator->second.viewLabelMap[i][labelIterator].toStdString() == "default")
							{
								defaultImageAdded = true;
								data += "\n\"\":\"" + colorwayIterator->second.viewUploadId[i] + "\",";
							}
							else
							{
								string labelId;
								if (colorwayIterator->first == "No_Colorway_Default")
								{
									Logger::Debug("UpdateProduct -> LinkImagesToColorways () labels:" + colorwayIterator->second.viewLabelMap[i][labelIterator].toStdString());
									it = m_styleImageLabelsMap.find(colorwayIterator->second.viewLabelMap[i][labelIterator]);
									if (it != m_styleImageLabelsMap.end())
										labelId = it->second.toStdString();

								}
								else
								{
									Logger::Debug("UpdateProduct -> LinkImagesToColorways () labels:" + colorwayIterator->second.viewLabelMap[i][labelIterator].toStdString());
									it = m_colorwayImageLabelsMap.find(colorwayIterator->second.viewLabelMap[i][labelIterator]);
									if (it != m_colorwayImageLabelsMap.end())
										labelId = it->second.toStdString();
								}


								data += "\n\"" + labelId + "\":\"" + colorwayIterator->second.viewUploadId[i] + "\",";

							}
						}

					}
				}
				Logger::Debug("UpdateProduct -> LinkImagesToColorways () ============ Data    " + data);

				if (str.compare("No_Colorway_Default") == 0)
				{
					for (auto styleIterator = m_nonCloStyleImageLabelsMap.begin(); styleIterator != m_nonCloStyleImageLabelsMap.end(); styleIterator++)
					{
						colorwayImageAdded = true;
						QString imageID = styleIterator->first;
						QStringList labelList = styleIterator->second;
						for (auto i = 0; i < labelList.size(); i++)
						{
							if (labelList[i].toStdString() == "default")
							{
								if(!defaultImageAdded)
								data += "\n\"\":\"" + imageID.toStdString() + "\",";
								//Logger::Debug("UpdateProduct -> LinkImagesToColorways () ============1    " + data);
							}
							else
							{
								data += "\n\"" + labelList[i].toStdString() + "\":\"" + imageID.toStdString() + "\",";
							}
						}
					}
					if (colorwayImageAdded)
						data = data.substr(0, data.length() - 1);

					data += "\n}\n}";

					if (!colorwayImageAdded)
						data = "{\n\"images\":{\n}\n}";
					Logger::Debug("UpdateProduct -> LinkImagesToColorways () ============1    " + data);
					string resultJsonString = RESTAPI::PutRestCall(data, Configuration::GetInstance()->GetPLMServerURL() + RESTAPI::STYLE_ENDPOINT_API + "/" + _productId, "content-type: application/json");
				}
				else
				{
					if (!colorwayIterator->second.colorwayId.empty())
					{
						string newdata = data;
						
						QString colowayImageID;
						QStringList labelList;

						auto iterator = m_nonCloColorWayImageLabelsMap.find(QString::fromStdString(colorwayIterator->second.colorwayId));
						if (iterator != m_nonCloColorWayImageLabelsMap.end())
						{
							colorwayImageAdded = true;
							map<QString, QStringList> nonCloColorwayImageLabelsMap;
							nonCloColorwayImageLabelsMap = iterator->second;
							for (auto itr = nonCloColorwayImageLabelsMap.begin(); itr!= nonCloColorwayImageLabelsMap.end(); itr++)
							{
								string imageId = itr->first.toStdString();
								Logger::Debug("UpdateProduct -> LinkImagesToColorways () imageId    " + imageId);
								labelList = itr->second;
								for (int j = 0; j < labelList.size(); j++)
								{
									Logger::Debug("UpdateProduct -> LinkImagesToColorways () labelList    " + labelList[j].toStdString());
									if (labelList[j].toStdString() == "default" )
									{
										if(!defaultImageAdded)
										newdata += "\n\"\":\"" + imageId + "\",";
									}
									else
									{

										newdata += "\n\"" + labelList[j].toStdString() + "\":\"" + imageId + "\",";
									}
								}
							}
							
							

							
							//Link images to colorways ....
							
							//}
						}
						if (colorwayImageAdded)
							newdata = newdata.substr(0, newdata.length() - 1);
						newdata += "\n}\n}";

						Logger::Debug("UpdateProduct -> LinkImagesToColorways () newdata:" + newdata);
						//if (!colorwayIterator->second.colorwayId.empty())
						//{
						Logger::Debug("UpdateProduct -> LinkImagesToColorways () colorwayIterator->second.colorwayId" + colorwayIterator->second.colorwayId);
						string resultJsonString = RESTAPI::PutRestCall(newdata, Configuration::GetInstance()->GetPLMServerURL() + RESTAPI::COLORWAY_API + "/" + colorwayIterator->second.colorwayId, "content-type: application/json");
					}

				}
			}
		}
		Logger::Debug("UpdateProduct -> LinkImagesToColorways () End");
	}

	void UpdateProduct::clearDependentComboBox(QString _lable, QTreeWidget* _searchTree)
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
			QComboBox* qComboBox = qobject_cast<QComboBox*>(qWidgetColumn_1);
			if (_lable == "Season")
			{
				if (lableText == "Brand" || lableText == "Collection" || lableText == "Department")
				{
					if (qComboBox)
					{
						qComboBox->clear();
						qComboBox->setCurrentText(QString::fromStdString(BLANK));
					}
				}
			}
			else if (_lable == "Brand")
			{
				if (lableText == "Collection" || lableText == "Department")
				{
					if (qComboBox)
					{
						qComboBox->clear();
						qComboBox->setCurrentText(QString::fromStdString(BLANK));
					}
				}
			}
			else if (_lable == "Department")
			{
				if (lableText == "Collection")
				{
					if (qComboBox)
					{
						qComboBox->clear();
						qComboBox->setCurrentText(QString::fromStdString(BLANK));
					}
				}
			}

		}
	}


	void UpdateProduct::UploadStyleThumbnail(string _productId)
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
	void UpdateProduct::reject()
	{
		Logger::Debug("UpdateProduct -> reject -> Start");

		if (!m_isSaveClicked)
		{
			ClearAllFields(m_updateProductTreeWidget_1);
			ClearAllFields(m_updateProductTreeWidget_2);
			ClearColorwayTable();
			UpdateImageIntent::GetInstance()->ClearAllFields();
			m_imageIntentTable->clear();
			ui_tabWidget->setCurrentIndex(0);
			m_colorSpecList.clear();
			m_totalCountLabel->setText("Total count: 0");
			ClearBOMData();
			
			
		}
		Configuration::GetInstance()->SetIsPrintSearchClicked(false);
		SetTotalImageCount();
		ui_tabWidget->setCurrentIndex(0);
		ui_tabWidget->setStyleSheet(" QTabWidget::pane { border: none; color: #FFFFFF; font-size: 10px; background-color: #262628; }""QTabBar::tab { width: 100px; padding: 2px; }""QTabBar::tab:selected { border: none; color: #FFFFFF; background-color: \"" + DEFAULT_TAB_BG_COLOR + "\"; }""QTabBar::tab:!selected { color:#FFFFFF; background-color:\"" + SELECTED_TAB_BG_COLOR + "\"; }");
		this->accept();

		Logger::Debug("UpdateProduct -> reject -> End");
	}

	string UpdateProduct::reviseDocument(string _revisionId)
	{
		Logger::Debug("UpdateProduct -> reviseDocument() start....");
		string revisedDocId;
		try
		{
			json emptyJson = json::object();
			string reviseDocumentResponse = RESTAPI::PostRestCall(to_string(emptyJson), Configuration::GetInstance()->GetPLMServerURL() + RESTAPI::DOCUMENT_UPLOAD_API + "/" + _revisionId + "/revise", "content-type: application/json; boundary=----WebKitFormBoundary7MA4YWxkTrZu0gW");
			//UTILITY_API->DisplayMessageBox("reviseDocumentResponse" + reviseDocumentResponse);
			if (!FormatHelper::HasContent(reviseDocumentResponse))
			{
				throw "Unable to revise document in PLM. Please try again or Contact your System Administrator.";
			}
			if (FormatHelper::HasError(reviseDocumentResponse))
			{
				throw runtime_error(reviseDocumentResponse);
			}
			json revisedResponseJson = json::parse(reviseDocumentResponse);
			revisedDocId = Helper::GetJSONValue<string>(revisedResponseJson, ATTRIBUTE_ID, true);
		}
		catch (string msg)
		{
			Logger::Error("UpdateProduct -> reviseDocument() Exception - " + msg);
		}
		catch (exception & e)
		{
			Logger::Error("UpdateProduct -> reviseDocument() Exception - " + string(e.what()));
		}
		catch (const char* msg)
		{
			wstring wstr(msg, msg + strlen(msg));
			Logger::Error("UpdateProduct -> reviseDocument() Exception - " + string(msg));
		}
		Logger::Debug("UpdateProduct -> reviseDocument() end....");
		return revisedDocId;
	}

	void UpdateProduct::GetcolorwayDetails()
	{
		Logger::Debug("UpdateProduct -> GetcolorwayDetails() start....");
		string styleId = Helper::GetJSONValue<string>(m_downloadedStyleJson, ATTRIBUTE_ID, true);
		json colorwayJson;
		string colorSpecsId;
		m_colorwayRowcount = 0;
		ui_colorwayTable->clear();
		ui_colorwayTable->clearContents();
		ui_colorwayTable->setRowCount(0);
		m_colorSpecList.clear();
		UpdateImageIntent::GetInstance()->m_ColorwayViewMap.clear();
		json downloadedColorwayjson = json::array();
		json colorSpecCountJson = json::object();
		QStringList selectedIds;
		m_imageIntentRowcount = -1;
		m_nonCloStyleImageLabelsMap.clear();
		m_imageIntentIdAndLabeMap.clear();
		m_nonCloColorWayImageLabelsMap.clear();
		m_nonCloColorwayImagesMap.clear();
		m_imageIntentTable->clear();
		m_imageIntentTable->clearContents();
		m_imageIntentTable->setRowCount(0);
		SetTotalImageCount();
		try
		{
			if (!styleId.empty())
			{
				colorwayJson = RESTAPI::makeRestcallGet(RESTAPI::UPDATE_STYLE_API + "/" + styleId + "/product_colors?limit=" + Configuration::GetInstance()->GetMaximumLimitForRefAttValue(), "", "", "Loading colorway details..");
				Logger::RestAPIDebug("responseJson->GetcolorwayDetails() ::responseJson::" + to_string(colorwayJson));
				//colorwayJson = Helper::makeRestcallGet(RESTAPI::UPDATE_STYLE_API + "/" + styleId + "/product_colors?limit=" + Configuration::GetInstance()->GetMaximumLimitForRefAttValue(), "", "", "Loading colorway details..");
				//m_downloadedColorwayJson = colorwayJson;
                string colorwayJSONString = to_string(colorwayJson);
				if (FormatHelper::HasError(colorwayJSONString))
				{
					Helper::GetCentricErrorMessage(colorwayJSONString);
					throw runtime_error(colorwayJSONString);
				}
				m_colorwayJson = colorwayJson;
				for (int colorwayAarrayCount = 0; colorwayAarrayCount < colorwayJson.size(); colorwayAarrayCount++)
				{
					json colorwayCountJson = Helper::GetJSONParsedValue<int>(colorwayJson, colorwayAarrayCount, false);
					string colorwayId = Helper::GetJSONValue<string>(colorwayCountJson, ATTRIBUTE_ID, true);
					string colorSpec = Helper::GetJSONValue<string>(colorwayCountJson, "color_specification", true);
					Logger::RestAPIDebug("responseJson->GetcolorwayDetails() ::colorwayId::" + colorwayId);
					Logger::RestAPIDebug("responseJson->GetcolorwayDetails() ::colorSpec::" + colorSpec);
					selectedIds.append(QString::fromStdString(colorwayId));
					colorSpecsId += "id=" + colorSpec + "&";
				}
				//	UTILITY_API->DisplayMessageBox("colorSpecsId" + colorSpecsId);
				colorSpecsId = colorSpecsId.substr(0, colorSpecsId.length() - 1);
				json colorSpecJson = RESTAPI::makeRestcallGet(RESTAPI::COLOR_SPEC_API + "?" + colorSpecsId + "&limit=" + Configuration::GetInstance()->GetMaximumLimitForRefAttValue(), "", "", "Loading color spec details..");

				//	UTILITY_API->DisplayMessageBox("colorSpecsId" + to_string(colorSpecJson));
				for (int colorwayAarrayCount = 0; colorwayAarrayCount < colorwayJson.size(); colorwayAarrayCount++)
				{
					json colorwayCountJson = Helper::GetJSONParsedValue<int>(colorwayJson, colorwayAarrayCount, false);
					string colorwayId = Helper::GetJSONValue<string>(colorwayCountJson, ATTRIBUTE_ID, true);
					string colorSpec = Helper::GetJSONValue<string>(colorwayCountJson, "color_specification", true);


					for (int colorSpecJsonAarrayCount = 0; colorSpecJsonAarrayCount < colorSpecJson.size(); colorSpecJsonAarrayCount++)
					{
						colorSpecCountJson = Helper::GetJSONParsedValue<int>(colorSpecJson, colorSpecJsonAarrayCount, false);
						string colorSpecId = Helper::GetJSONValue<string>(colorSpecCountJson, ATTRIBUTE_ID, true);
						if (colorSpec == colorSpecId)
						{
							Logger::Debug("UpdateProduct -> GetcolorwayDetails() colorSpec : " + colorSpec);
							if (!colorSpec.empty() || colorSpec != "centric%3A")
								m_colorSpecList.append(QString::fromStdString(colorSpec));

							string nodeName = Helper::GetJSONValue<string>(colorSpecCountJson, NODE_NAME_KEY, true);;
							string code = Helper::GetJSONValue<string>(colorSpecCountJson, CODE_KEY, true);;
							string pantone = Helper::GetJSONValue<string>(colorSpecCountJson, PANTONE_KEY, true);;
							string rgbValue = Helper::GetJSONValue<string>(colorSpecCountJson, RGB_VALUE_KEY, true);;

							colorwayCountJson["color_name_name"] = nodeName;
							colorwayCountJson["code"] = code;
							colorwayCountJson["pantone"] = pantone;
							colorwayCountJson["rgb_triple"] = rgbValue;
							//	UTILITY_API->DisplayMessageBox("colorwayCountJson:: " + to_string(colorwayCountJson));
							break;
						}
					}
					downloadedColorwayjson[colorwayAarrayCount] = colorwayCountJson;
				}
				//UTILITY_API->DisplayMessageBox("colorwayCountJson:: " + to_string(downloadedColorwayjson));
				m_downloadedColorway = true;
				m_downloadedColorwayJson = downloadedColorwayjson;
				AddColorwayDetails(selectedIds, downloadedColorwayjson);			
				ShowImageIntent();
				m_downloadedColorway = false;
			}
		}
		catch (string msg)
		{
			Logger::Error("UpdateProduct -> GetcolorwayDetails() Exception - " + msg);
		}
		catch (exception & e)
		{
			Logger::Error("UpdateProduct -> GetcolorwayDetails() Exception - " + string(e.what()));
		}
		catch (const char* msg)
		{
			wstring wstr(msg, msg + strlen(msg));
			Logger::Error("UpdateProduct -> GetcolorwayDetails() Exception - " + string(msg));
		}
		Logger::Debug("UpdateProduct -> GetcolorwayDetails() End....");
	}
	void UpdateProduct::DeleteColorwayFromPLM()
	{
		Logger::Debug("UpdateProduct -> DeleteColorwayFromPLM () Start");

		for (int index = 0; index < m_DeletedColorwayList.size(); index++)
		{
			string response = RESTAPI::DeleteRestCall("", Configuration::GetInstance()->GetPLMServerURL() + RESTAPI::COLORWAY_API + "/" + m_DeletedColorwayList[index].toStdString(), "content-type: application/json");
			//UTILITY_API->DisplayMessageBox("DELETE Response::" + response);
		}

		m_DeletedColorwayList.clear();
		Logger::Debug("UpdateProduct -> DeleteColorwayFromPLM () End");

	}

	void UpdateProduct::OnClickUpdateColorButton(int i)
	{
		Logger::Debug("UpdateProduct -> OnClickUpdateColorButton () Start");
		Configuration::GetInstance()->SetIsUpdateColorClicked(true);
		m_currentColorSpec = BLANK;
		m_selectedRow = i;
		//UTILITY_API->DisplayMessageBox(to_string(m_SelectedRow));
		QComboBox *colorwayNameCombo = static_cast<QComboBox*>(ui_colorwayTable->cellWidget(m_selectedRow, CLO_COLORWAY_COLUMN)->children().last());
		string colorSpecId = colorwayNameCombo->property("Id").toString().toStdString();
		if (!colorSpecId.empty())
			m_currentColorSpec = colorSpecId;
		ColorConfig::GetInstance()->m_mode = "Search";
		ColorConfig::GetInstance()->m_isSearchColor = true;
		onAddColorwaysClicked();


		Logger::Debug("UpdateProduct -> OnClickUpdateColorButton () End");

	}

	void UpdateProduct::ClearColorwayTable()
	{

		Logger::Debug("UpdateProduct -> ClearColorwayTable () Start");
		for (int index = 0; index < ui_colorwayTable->rowCount(); index++)
		{
			//QTableWidgetItem* item;

			QComboBox *colorwayNameCombo = static_cast<QComboBox*>(ui_colorwayTable->cellWidget(index, CLO_COLORWAY_COLUMN)->children().last());
			string newColorwayId = colorwayNameCombo->property("NewColorway").toString().toStdString();
			//QString colorcode = item->text();
			if (m_NewlyAddedColorway.contains(QString::fromStdString(newColorwayId)))
			{
				ui_colorwayTable->removeRow(index);
			}
		}
		m_NewlyAddedColorway.clear();
		Logger::Debug("UpdateProduct -> ClearColorwayTable () End");
	}
	bool UpdateProduct::UpdateColorInColorways(QStringList _downloadIdList, json _jsonarray)
	{
		Logger::Debug("UpdateProduct -> UpdateColorInColorways () Start");
		//UTILITY_API->DisplayMessageBox(to_string(m_SelectedRow));
		int column = ui_colorwayTable->colorCount();
		//UTILITY_API->DisplayMessageBox(to_string(_jsonarray));
		string objectName;
		string pantone;
		string rgbValue;
		string objectCode;
		string attId;
		string DefaultImageId;
		int tabIndex = ui_tabWidget->currentIndex();

		if (!m_currentColorSpec.empty())
			m_colorSpecList.removeOne(QString::fromStdString(m_currentColorSpec));

		for (int rowCount = 0; rowCount < _jsonarray.size(); rowCount++)
		{
			Logger::Debug("UpdateProduct -> UpdateColorInColorways () 1");
			json attachmentsJson = Helper::GetJSONParsedValue<int>(_jsonarray, rowCount, false);
			Logger::Debug("UpdateProduct -> UpdateColorInColorways () 2");
			attId = Helper::GetJSONValue<string>(attachmentsJson, ATTRIBUTE_ID, true);


			if (_downloadIdList.contains(QString::fromStdString(attId)))
			{
				Logger::Debug("UpdateProduct -> UpdateColorInColorways () attId: " + attId);
				Logger::Debug("UpdateProduct -> UpdateColorInColorways () attId: " + to_string(m_colorSpecList.size()));
				if (m_colorSpecList.contains(QString::fromStdString(attId)))
					return false;
				else
					m_colorSpecList.append(QString::fromStdString(attId));

				attId = Helper::GetJSONValue<string>(attachmentsJson, ATTRIBUTE_ID, true);
				objectName = Helper::GetJSONValue<string>(attachmentsJson, ATTRIBUTE_NAME, true);
				pantone = Helper::GetJSONValue<string>(attachmentsJson, PANTONE_KEY, true);
				rgbValue = Helper::GetJSONValue<string>(attachmentsJson, RGB_VALUE_KEY, true);
				objectCode = Helper::GetJSONValue<string>(attachmentsJson, CODE_KEY, true);
				json imageJson = Helper::GetJSONParsedValue<string>(attachmentsJson, IMAGES_JSON, false);
				Logger::Logger("imageJson ::" + to_string(imageJson));
				DefaultImageId = Helper::GetJSONValue<string>(imageJson, BLANK, true);
				Logger::Logger("DefaultImageId ::" + DefaultImageId);
				break;
			}
		}

		//QTableWidgetItem* item = new QTableWidgetItem();
		//ui_colorwayTable->clea
		if (tabIndex == COLORWAY_TAB)
		{
			if (!FormatHelper::HasContent(pantone))
				pantone = BLANK;
			ui_colorwayTable->item(m_selectedRow, COLOR_NAME_COLUMN)->setText(QString::fromStdString(objectName));
			ui_colorwayTable->item(m_selectedRow, COLOR_CODE_COLUMN)->setText(QString::fromStdString(objectCode));
			ui_colorwayTable->item(m_selectedRow, PANTONE_CODE_COLUMN)->setText(QString::fromStdString(pantone));
			QComboBox *colorwayName1 = static_cast<QComboBox*>(ui_colorwayTable->cellWidget(m_selectedRow, CLO_COLORWAY_COLUMN)->children().last());
			colorwayName1->setProperty("Id", attId.c_str());
		}
		QTableWidgetItem* iconItem = new QTableWidgetItem;
		QSize iconSize(40, 40);
		iconItem->setSizeHint(iconSize);
		QStringList listRGB;
		QImage Icon;
		QPixmap pixmap;
		QWidget *pWidget = nullptr;
		//string rgbValue = rgbValue;
		rgbValue = Helper::FindAndReplace(rgbValue, "(", "");
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
			QImage image(60, 60, QImage::Format_ARGB32);
			image.fill(color);
			QLabel* label = new QLabel();
			pixmap = QPixmap::fromImage(image);
			if (tabIndex == COLORWAY_TAB)
				label->setPixmap(QPixmap(pixmap.scaled(60, 60, Qt::KeepAspectRatio)));
			if (tabIndex == BOM_TAB)
				label->setPixmap(QPixmap(pixmap.scaled(20, 20, Qt::KeepAspectRatio)));
			
			pWidget = CVWidgetGenerator::InsertWidgetInCenter(label);
		}
		else if (FormatHelper::HasContent(DefaultImageId))
		{
			Logger::Logger("else DefaultImageId ::" + DefaultImageId);
			QImage thumnail;
			string imageResponse = RESTAPI::CentricRestCallGet(Configuration::GetInstance()->GetPLMServerURL() + RESTAPI::PRINT_IMAGE_API + "/" + DefaultImageId, APPLICATION_JSON_TYPE, BLANK);
			json imageResponseJson = json::parse(imageResponse);

			string thumbnailUrl = Helper::GetJSONValue<string>(imageResponseJson, THUMBNAIL_KEY, true);
			Logger::Logger("thumbnailUrl ::" + thumbnailUrl);
			string latestVersionAttUrl = Helper::GetJSONValue<string>(imageResponseJson, "_url_base_template", true);

			latestVersionAttUrl = Helper::FindAndReplace(latestVersionAttUrl, "%s", thumbnailUrl);
			Logger::Logger("latestVersionAttUrl ::" + latestVersionAttUrl);
			if (FormatHelper::HasContent(latestVersionAttUrl))
			{
				QByteArray imageBytes;
				auto startTime = std::chrono::high_resolution_clock::now();
				imageBytes = Helper::DownloadImageFromURL(latestVersionAttUrl);
				auto finishTime = std::chrono::high_resolution_clock::now();
				std::chrono::duration<double> totalDuration = finishTime - startTime;
				Logger::perfomance(PERFOMANCE_KEY + "download Thambnail API :: " + to_string(totalDuration.count()));
				QBuffer buffer(&imageBytes);
				buffer.open(QIODevice::ReadOnly);
				QImageReader imageReader(&buffer);
				imageReader.setDecideFormatFromContent(true);
				thumnail = imageReader.read();
				if (thumnail.isNull())
				{
					Logger::Error("LOGGER::CVWidgetGenerator: CreateIconWidget() -> Image is not loaded.  ");
					QImageReader imageReader(":/CLOVise/PLM/Images/NoImage.png");
					imageReader.setDecideFormatFromContent(true);
					thumnail = imageReader.read();
				}
				pixmap = pixmap.fromImage(thumnail);
				pWidget = CVWidgetGenerator::GetInstance()->CreateThumbnailWidget(_jsonarray, pixmap, PRINT_MODULE);
			}
		}
		if (tabIndex == COLORWAY_TAB)
		{
			ui_colorwayTable->setCellWidget(m_selectedRow, COLOR_CHIP_COLUMN, pWidget);
			Configuration::GetInstance()->SetIsUpdateColorClicked(false);
		}
		if (tabIndex == BOM_TAB)
		{
			Logger::Debug("CreateProduct -> UpdateColorInColorways () 6");
			Logger::Debug("CreateProduct -> UpdateColorInColorways () AddNewBom::GetInstance()->m_currentTableName" + UpdateProductBOMHandler::GetInstance()->m_currentTableName);
			UpdateProductBOMHandler::GetInstance()->m_currentTableName;
			auto itr = UpdateProductBOMHandler::GetInstance()->m_bomSectionTableInfoMap.find(UpdateProductBOMHandler::GetInstance()->m_currentTableName);
			if (itr != UpdateProductBOMHandler::GetInstance()->m_bomSectionTableInfoMap.end())
			{
				Logger::Debug("CreateProduct -> UpdateColorInColorways () AddNewBom::GetInstance()->m_currentRow" + to_string(UpdateProductBOMHandler::GetInstance()->m_currentRow));
				QTableWidget* sectionTable = itr->second;
				if (QWidget* widget = sectionTable->cellWidget(UpdateProductBOMHandler::GetInstance()->m_currentRow, UpdateProductBOMHandler::GetInstance()->m_currentColumn))
				{

					QString columnName = sectionTable->horizontalHeaderItem(UpdateProductBOMHandler::GetInstance()->m_currentColumn)->text();
					if (columnName == "Common Color")
					{
						QPushButton* pushButton = static_cast<QPushButton*>(sectionTable->cellWidget(UpdateProductBOMHandler::GetInstance()->m_currentRow, 0)->children().last());;

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
		Logger::Debug("UpdateProduct -> UpdateColorInColorways () End");
		return true;
	}

	void UpdateProduct::OnAddColorwayClicked()
	{

		Logger::Debug("UpdateProduct -> AddColorwayDetails() -> Start");

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
			else if (index == CLO_COLORWAY_COLUMN || index == UNI_2_DIGIT_CODE_COLUMN)
			{
				ui_colorwayTable->resizeColumnToContents(1);
			}

			else
				ui_colorwayTable->setColumnWidth(index, 80);
		}
		Logger::Debug("UpdateProduct ->AddColorwayDetails() -> End");
	}

	/*
* Description - ShowImageIntent() used to show existing image intents of the downloaded style
* Parameter int _row
* Exception -
* Return - void.
*/
	void UpdateProduct::ShowImageIntent()
	{
		Logger::Debug("UpdateProduct -> ShowImageIntent() -> Start");
		UTILITY_API->CreateProgressBar();
		RESTAPI::SetProgressBarData(10, "Loading colorway images..", true);
		string styleId;
		styleId = Helper::GetJSONValue<string>(m_downloadedStyleJson, "id", true);
		drawColorwayImageList(styleId, "No_Colorway_Default", "style");//Adding image intent for style
		for (auto colorwayIterator = m_downloadedColorwayIdMap.begin(); colorwayIterator != m_downloadedColorwayIdMap.end(); colorwayIterator++)
			drawColorwayImageList(colorwayIterator->first, colorwayIterator->second, "colorway");
		Logger::Debug("UpdateProduct -> ShowImageIntent() -> End");
	}

	/*
* Description - drawColorwayImageList() used to draw list in colorway table to show colorway images
* Parameter string _selectedColorwayId
* Exception -
* Return - void.
*/
	void UpdateProduct::drawColorwayImageList(string _id, string _colorwayName, string _module)
	{
		Logger::Debug("UpdateProduct -> drawColorwayImageList() -> Start");
		try
		{
			m_colorwayImageList->clear();
			Logger::Debug("_selectedIdList.contains(QString::fromStdString(_selectedColorwayId)" + _id);
			string attachmentResponse;
			if (_module == "colorway")
				attachmentResponse = RESTAPI::CentricRestCallGet(Configuration::GetInstance()->GetPLMServerURL() + RESTAPI::COLORWAY_API + "/" + _id + "/images?skip=0&limit=1000&decode=true", APPLICATION_JSON_TYPE, "");
			if (_module == "style")
				attachmentResponse = RESTAPI::CentricRestCallGet(Configuration::GetInstance()->GetPLMServerURL() + RESTAPI::STYLE_ENDPOINT_API + "/" + _id + "/images?skip=0&limit=1000&decode=true", APPLICATION_JSON_TYPE, "");
			Logger::Debug("UpdateProduct -> drawColorwayImageList() -> attachmentResponse" + attachmentResponse);

			if (FormatHelper::HasError(attachmentResponse))
			{
				Helper::GetCentricErrorMessage(attachmentResponse);
				throw runtime_error(attachmentResponse);
			}
			m_colorwayImageList->setEnabled(true);
			map<string, QStringList>::iterator it;
			std::map<QString, QString>::iterator imageLabelMapIterator;
			map<string, UpdateImageIntent::ColorwayViews> ::iterator mapIterator;

			m_colorwayImageLabelsMap = UIHelper::GetImageLabels("Colorway");
			Configuration::GetInstance()->SetColorwayImageLabels(m_colorwayImageLabelsMap);

			m_styleImageLabelsMap = UIHelper::GetImageLabels("Style");
			Configuration::GetInstance()->SetStyleImageLabels(m_styleImageLabelsMap);

			json attachmentjson = json::parse(attachmentResponse);
			UTILITY_API->SetProgress("Loading colorway images.", (qrand() % 101));
			if (_module == "style")
			{
				string imageJsonStr = Helper::GetJSONValue<string>(m_downloadedStyleJson, "images", false);
				json imageJson = json::parse(imageJsonStr);
				for (int attachmenJsonCount = 0; attachmenJsonCount < attachmentjson.size(); attachmenJsonCount++)
				{
					json attachment = Helper::GetJSONParsedValue<int>(attachmentjson, attachmenJsonCount, false);
					Logger::Debug("UpdateProduct -> attachmentCountJson  " + to_string(attachment));
					string generatedBy = Helper::GetJSONValue<string>(attachment, "generated_by", true);
					string id = Helper::GetJSONValue<string>(attachment, "id", true);

					Logger::Debug("UpdateProduct -> drawColorwayImageList()============" + generatedBy);
					if (generatedBy != "CLO3D")
					{
						FillNonCloImageMap(imageJson, id);
					}
				}
				FillImageIntentIdAndLabeMap(imageJson, _module);
			}
			Logger::Debug("UpdateProduct -> drawColorwayImageList() -> attachmentjson" + to_string(attachmentjson));
			json imageJson;
			for (int attachmenAarrayCount = 0; attachmenAarrayCount < m_colorwayJson.size(); attachmenAarrayCount++)
			{
				json attachmentCountJson = Helper::GetJSONParsedValue<int>(m_colorwayJson, attachmenAarrayCount, false);
				string imageJsonStr = Helper::GetJSONValue<string>(attachmentCountJson, "images", false);
				 imageJson = json::parse(imageJsonStr);
				Logger::Debug("UpdateProduct -> attachmentCountJson imageJson " + to_string(imageJson));
				QString colorwayId = QString::fromStdString(Helper::GetJSONValue<string>(attachmentCountJson, "id", true));

				if (colorwayId.compare(QString::fromStdString(_id)) == 0)
				{
					FillImageIntentIdAndLabeMap(imageJson, _module);
					break;
				}
			}
			
			UpdateImageIntent::ColorwayViews colorwayView;
			QStringList nonCloImageIdsList;
			for (int attachmenAarrayCount = 0; attachmenAarrayCount < attachmentjson.size(); attachmenAarrayCount++)
			{
				json attachmentCountJson = Helper::GetJSONParsedValue<int>(attachmentjson, attachmenAarrayCount, false);
				Logger::Debug("UpdateProduct -> attachmentCountJson Vim " + to_string(attachmentCountJson));
				string thumbnail = Helper::GetJSONValue<string>(attachmentCountJson, "thumbnail", true);
				string imageName = Helper::GetJSONValue<string>(attachmentCountJson, "node_name", true);
				string generatedBy = Helper::GetJSONValue<string>(attachmentCountJson, "generated_by", true);
				string imageId = Helper::GetJSONValue<string>(attachmentCountJson, "id", true);
				Logger::Debug("UpdateProduct -> attachmentCountJson generatedBy " + generatedBy);
				if (generatedBy != "CLO3D" && _module != "style")
				{
					nonCloImageIdsList.append(QString::fromStdString(imageId));
					FillNonCloColorwayImageMap(imageJson, imageId, QString::fromStdString(_id));
					continue;
				}

				if (imageName.empty())
					continue;

				string imageUrl = Helper::GetJSONValue<string>(attachmentCountJson, "_url_base_template", true);

				imageUrl = Helper::FindAndReplace(imageUrl, "%s", thumbnail);

				string filePath = UTILITY_API->GetCLOTemporaryFolderPath();
				if (imageName.find("Avatar_") != -1)
					filePath = filePath + "CLOViseTurntableImages/WithAvatar/";
				else
					filePath = filePath + "CLOViseTurntableImages/WithoutAvatar/";
				
				filePath = filePath + imageName;

				Logger::Debug("UpdateProduct -> drawColorwayImageList() -> filePath " + filePath);
				Helper::DownloadFilesFromURL(imageUrl, filePath);// Downloading Image

				it = m_imageIntentIdAndLabeMap.find(imageId);
				QStringList labelsList;

				QStringList imageLabelList;
				QString defaultImage;
				bool validImageLabel = false;
				QStringList tempimageLabelList;
				if (it != m_imageIntentIdAndLabeMap.end())
				{
					validImageLabel = false;
					imageLabelList = it->second;

					defaultImage = "No";
					for (int i = 0; i < imageLabelList.size(); i++)
					{
						if (imageLabelList[i] == "default")
							defaultImage = "Yes";
						else
						{
							//labels = labels + imageLabelList[i] + "\n";
							imageLabelMapIterator = m_colorwayImageLabelsMap.find(imageLabelList[i]);
							if (imageLabelMapIterator != m_colorwayImageLabelsMap.end())
							{
								Logger::Debug("UpdateProduct -> drawColorwayImageList() -> validImageLabel 1");
								validImageLabel = true;
							}
							imageLabelMapIterator = m_styleImageLabelsMap.find(imageLabelList[i]);
							if (imageLabelMapIterator != m_styleImageLabelsMap.end())
							{
								Logger::Debug("UpdateProduct -> drawColorwayImageList() -> validImageLabel 2");
								validImageLabel = true;
							}

							if (validImageLabel)
							{
								Logger::Debug("UpdateProduct -> drawColorwayImageList() -> validImageLabel 3");
								labelsList.append(imageLabelList[i]);
							}


						}
					}


				}
				labelsList.removeDuplicates();
				Logger::Debug("UpdateProduct -> drawColorwayImageList  ImageName" + imageName);
				Logger::Debug("UpdateProduct -> drawColorwayImageList  ImageId" + imageId);
				int index = imageName.rfind('_');
				if (index == -1)
					continue;
				string str = imageName.substr(index + 1, 1);
				//UTILITY_API->DisplayMessageBox("str :  " + str);
				int view = stoi(str);
				string viewName;
				switch (view)
				{
				case BACK_VIEW:
					viewName = "Back";
					break;
				case FRONT_VIEW:
					viewName = "Front";
					break;
				case LEFT_VIEW:
					viewName = "Left";
					break;
				case RIGHT_VIEW:
					viewName = "Right";
					break;
				}
				Logger::Debug("UpdateProduct -> drawColorwayImageList() ->  _colorwayName + viewName:   " + _colorwayName + viewName);
				//UpdateImageIntent::GetInstance()->m_colorwayViewQueue.append(QString::fromStdString(_colorwayName) + QString::fromStdString(viewName));//using list for 
				ImageIntentsDetails imageIntentsDetails;
				imageIntentsDetails.colorwayName = QString::fromStdString(_colorwayName);
				imageIntentsDetails.view = QString::fromStdString(str);
				imageIntentsDetails.viewName = QString::fromStdString(viewName);
				imageIntentsDetails.labels = labelsList.join(',');
				imageIntentsDetails.isdefault = defaultImage;

				colorwayView.colorwayId = _id;
				if (imageLabelList.contains("default"))
				{
					colorwayView.defaultImage = view;
					labelsList.append("default");

				}
				else
					colorwayView.defaultImage = -1;
				colorwayView.viewUploadId[view] = imageId;

				int includeAvatar=-1;
				QString includeAvaterStr = "No";
				size_t found = imageName.find("Avatar_");
				if (found != string::npos)
				{
					includeAvatar = 1;
					includeAvaterStr = "Yes";
					Logger::Debug("UpdateProduct -> drawColorwayImageList() -> includeAvaterStr" + includeAvaterStr.toStdString());
				}
				imageIntentsDetails.isAvatar = includeAvaterStr;
				colorwayView.includeAvatar[view]= includeAvatar;
				colorwayView.viewLabelMap.insert(make_pair(view, labelsList));

				//QString displayDetails = "Colorway : " + QString::fromStdString(_colorwayName) + "\nView : " + QString::fromStdString(viewName) + "\n Image Labels: " + labels + "\n Default: " + defaultImage;




				QPixmap pix(QString::fromStdString(filePath));
				pix.scaled(QSize(80, 80), Qt::KeepAspectRatio);
				QIcon newIcon;
				newIcon.addPixmap(pix);
				if (_colorwayName == "No_Colorway_Default")
				{
					AddRowInImageIntentTab(pix, imageIntentsDetails, imageId);
				}
				for (auto i = 0; i < m_colorwayNamesList.count(); i++)
				{					
					
					if (m_colorwayNamesList.at(i).toStdString()== _colorwayName )
					{
						AddRowInImageIntentTab(pix, imageIntentsDetails, imageId);
					}
				}
			}

			UpdateImageIntent::GetInstance()->m_ColorwayViewMap.insert(make_pair(_colorwayName, colorwayView));
			Logger::Debug("UpdateProduct -> drawColorwayImageList() -> colorwayView.viewUploadId[0]" + colorwayView.viewUploadId[0]);
			Logger::Debug("UpdateProduct -> drawColorwayImageList() -> colorwayView.viewUploadId[1]" + colorwayView.viewUploadId[1]);
			Logger::Debug("UpdateProduct -> drawColorwayImageList() -> colorwayView.viewUploadId[2]" + colorwayView.viewUploadId[2]);
			Logger::Debug("UpdateProduct -> drawColorwayImageList() -> colorwayView.viewUploadId[3]" + colorwayView.viewUploadId[3]);
			//RESTAPI::SetProgressBarData(0, "", false);
		}
		catch (string msg)
		{
			UTILITY_API->DeleteProgressBar(true);
			Logger::Error("UpdateProduct -> drawColorwayImageList() Exception - " + msg);
		}
		catch (exception & e)
		{
			UTILITY_API->DeleteProgressBar(true);
			Logger::Error("UpdateProduct -> drawColorwayImageList() Exception - " + string(e.what()));
		}
		catch (const char* msg)
		{
			UTILITY_API->DeleteProgressBar(true);
			wstring wstr(msg, msg + strlen(msg));
			Logger::Error("UpdateProduct -> drawColorwayImageList() Exception - " + string(msg));
		}
		Logger::Debug("UpdateProduct -> drawColorwayImageList() -> End");
	}


	/*
* Description - onHideButtonClicked() invoke when hide button clicked on colorway table
* Parameter bool _hide
* Exception -
* Return - void.
*/
/*void UpdateProduct::onHideButtonClicked(bool _hide)
{
	Logger::Debug("UpdateProduct -> onHideButtonClicked() -> Start");
	hideButtonClicked(_hide);
	Logger::Debug("UpdateProduct -> onHideButtonClicked() -> End");
}

void UpdateProduct::hideButtonClicked(bool _hide)
{
	Logger::Debug("UpdateProduct -> hidebuttonClicked() -> Start");
	if (!_hide)
	{
		if (m_isColorwayImageListHidden)
		{
			ui_hideButton->setToolTip("Hide");
			ui_hideButton->setFocusPolicy(Qt::NoFocus);
			m_colorwayImageList->show();
			ui_hideButton->setStyleSheet("#ui_hideButton {\n" "qproperty-icon: none; \n""image: url(:/CLOVise/PLM/Images/icon_open_tree_none.svg);\n""}\n"
				"#ui_hideButton:hover{\n""image: url(:/CLOVise/PLM/Images/icon_open_tree_over.svg);\n""}""QToolTip{ color: #46C8FF; background-color: #33414D; border: 1px #000000; }");
			m_isColorwayImageListHidden = false;
		}
		else
		{
			ui_hideButton->setToolTip("Show");
			ui_hideButton->setStyleSheet("#ui_hideButton {\n" "qproperty-icon: none; \n""image: url(:/CLOVise/PLM/Images/icon_close_tree_none.svg);\n""}\n"
				"#ui_hideButton:hover{\n""image: url(:/CLOVise/PLM/Images/icon_close_tree_over.svg);\n""}""QToolTip{ color: #46C8FF; background-color: #33414D; border: 1px #000000; }");
			m_colorwayImageList->hide();
			m_isColorwayImageListHidden = true;
		}
		RESTAPI::SetProgressBarData(0, "", false);
	}
	Logger::Debug("UpdateProduct -> hidebuttonClicked() -> End");
}*/

/*
* Description - showNoImageAvailable() used to show information when no colorway images are there
* Parameter
* Exception -
* Return - void.
*/
	void  UpdateProduct::showNoImageAvailable()
	{
		Logger::Debug("UpdateProduct -> showNoImageAvailable() -> Start");

		QImageReader imageReader(":/CLOVise/PLM/Images/infoIcon.png");
		imageReader.setDecideFormatFromContent(true);
		QImage styleIcon = imageReader.read();
		QPixmap pixmap = QPixmap::fromImage(styleIcon);

		pixmap = QPixmap::fromImage(styleIcon).scaled(QSize(20, 20), Qt::KeepAspectRatio);
		QIcon icon;
		icon.addPixmap(pixmap, QIcon::Normal);
		icon.addPixmap(pixmap, QIcon::Selected);
		icon.addPixmap(pixmap, QIcon::Active);

		QListWidgetItem* item = new QListWidgetItem(icon, ("No images available for this colorway"));
		m_colorwayImageList->addItem(item);
		m_colorwayImageList->setDisabled(true);
		ui_colorwayImageLayout->addWidget(m_colorwayImageList);
		Logger::Debug("UpdateProduct -> showNoImageAvailable() -> End");
	}

	bool UpdateProduct::GetIsSaveClicked()
	{
		return m_isSaveClicked;
	}

	void UpdateProduct::FillImageIntentsStruct(string _colorway, string _view, string _viewName, string _labels, string _default, string _includeAvatar)
	{
		m_imageIntentsDetails.colorwayName = QString::fromStdString(_colorway);
		m_imageIntentsDetails.view = QString::fromStdString(_view);
		m_imageIntentsDetails.viewName = QString::fromStdString(_viewName);
		m_imageIntentsDetails.labels = QString::fromStdString(_labels);
		m_imageIntentsDetails.isdefault = QString::fromStdString(_default);
		m_imageIntentsDetails.isAvatar = QString::fromStdString(_includeAvatar);
	}

	void UpdateProduct::AddRowInImageIntentTab(QPixmap _pixMap, ImageIntentsDetails _imageIntentsDetails, string _imageId)
	{
		Logger::Debug("UpdateProduct -> AddRowInImageIntentTab() -> Start");
		m_imageIntentRowcount++;
		m_imageIntentTable->insertRow(m_imageIntentRowcount);
		SetTotalImageCount();

		QWidget *pColorWidget = nullptr;
		QLabel* label = new QLabel();
		label->setMaximumSize(QSize(70, 70));
		int w = label->width();
		int h = label->height();
		label->setPixmap(QPixmap(_pixMap.scaled(w, h, Qt::KeepAspectRatio)));
		pColorWidget = CVWidgetGenerator::InsertWidgetInCenter(label);
		m_imageIntentTable->setCellWidget(m_imageIntentRowcount, IMAGE_INTENT_COLUMN, pColorWidget);

		QTableWidgetItem* colorwayWidget = new QTableWidgetItem(_imageIntentsDetails.colorwayName);
		colorwayWidget->setTextAlignment(Qt::AlignCenter);
		colorwayWidget->setToolTip(QString::fromStdString(""));
		m_imageIntentTable->setItem(m_imageIntentRowcount, COLORWAY_COLUMN, colorwayWidget);
		m_imageIntentTable->setColumnWidth(COLORWAY_COLUMN, 150);

		QTableWidgetItem* viewWidget = new QTableWidgetItem(_imageIntentsDetails.viewName);
		viewWidget->setTextAlignment(Qt::AlignCenter);
		viewWidget->setToolTip(QString::fromStdString(""));
		m_imageIntentTable->setItem(m_imageIntentRowcount, IMAGE_VIEW_COLUMN, viewWidget);
		m_imageIntentTable->setColumnWidth(IMAGE_VIEW_COLUMN, 90);

		QTableWidgetItem* labelsWidget = new QTableWidgetItem(_imageIntentsDetails.labels);
		labelsWidget->setTextAlignment(Qt::AlignCenter);
		labelsWidget->setToolTip(QString::fromStdString(""));
		m_imageIntentTable->setItem(m_imageIntentRowcount, LABELS_COLUMN, labelsWidget);
		m_imageIntentTable->setColumnWidth(LABELS_COLUMN, 300);

		QTableWidgetItem* defaultWidget = new QTableWidgetItem(_imageIntentsDetails.isdefault);
		defaultWidget->setTextAlignment(Qt::AlignCenter);
		defaultWidget->setToolTip(QString::fromStdString(""));
		m_imageIntentTable->setItem(m_imageIntentRowcount, DEFAULT_COLUMN, defaultWidget);
		m_imageIntentTable->setColumnWidth(DEFAULT_COLUMN, 90);

		QPushButton *editButton = CVWidgetGenerator::CreatePushButton("", ":/CLOVise/PLM/Images/icon_draw_over.svg", "Edit", PUSH_BUTTON_STYLE, 30, true);
		editButton->setProperty("colorwayName", _imageIntentsDetails.colorwayName);
		editButton->setProperty("view", _imageIntentsDetails.view);
		editButton->setProperty("imageId", QString::fromStdString(_imageId));
		editButton->setProperty("defaultImage", _imageIntentsDetails.isdefault);
		QWidget *pEditWidget = CVWidgetGenerator::InsertWidgetInCenter(editButton);
		if (m_editButtonSignalMapper != nullptr)
		{
			connect(editButton, SIGNAL(clicked()), m_editButtonSignalMapper, SLOT(map()));
			m_editButtonSignalMapper->setMapping(editButton, m_imageIntentRowcount);
		}

		m_imageIntentTable->setCellWidget(m_imageIntentRowcount, EDIT_COLUMN, pEditWidget);
		m_imageIntentTable->setColumnWidth(EDIT_COLUMN, 50);

		QPushButton *deleteButton = CVWidgetGenerator::CreatePushButton("", ":/CLOVise/PLM/Images/icon_delete_over.svg", "Delete", PUSH_BUTTON_STYLE, 30, true);
		deleteButton->setProperty("colorwayName", _imageIntentsDetails.colorwayName);
		deleteButton->setProperty("view", _imageIntentsDetails.view);
		deleteButton->setProperty("imageId", QString::fromStdString(_imageId));
		deleteButton->setProperty("defaultImage", _imageIntentsDetails.isdefault);
		deleteButton->setProperty("includeAvatar", _imageIntentsDetails.isAvatar);
		QWidget *pDeleteWidget = CVWidgetGenerator::InsertWidgetInCenter(deleteButton);
		if (m_deleteSignalMapper != nullptr)
		{
			connect(deleteButton, SIGNAL(clicked()), m_deleteSignalMapper, SLOT(map()));
			m_deleteSignalMapper->setMapping(deleteButton, m_imageIntentRowcount);
		}
		m_imageIntentTable->setCellWidget(m_imageIntentRowcount, DELETE_COLUMN, pDeleteWidget);
		m_imageIntentTable->setColumnWidth(DELETE_COLUMN, 50);
		m_imageIntentTable->setWordWrap(true);

		Logger::Debug("UpdateProduct -> AddRowInImageIntentTab() -> End");
	}

	void UpdateProduct::onImageIntentsTableEditButtonClicked(int _rowIndex)
	{
		Logger::Debug("UpdateProduct -> onImageIntentsTableEditButtonClicked() Start");
		//@ToDo
		try
		{
			m_editButtonClicked = true;
			m_ImageIntentRowIndexForEdit = _rowIndex;
			Logger::Debug("UpdateProduct -> onImageIntentsTableEditButtonClicked() Start 1 _rowIndex  " + to_string(_rowIndex));
			QPushButton *editButoon = static_cast<QPushButton*>(m_imageIntentTable->cellWidget(_rowIndex, EDIT_COLUMN)->children().last());
			QPushButton *deleteButton = static_cast<QPushButton*>(m_imageIntentTable->cellWidget(_rowIndex, DELETE_COLUMN)->children().last());
			string includeAvatar = deleteButton->property("includeAvatar").toString().toStdString();
			string colorwayName = editButoon->property("colorwayName").toString().toStdString();
			string colorwayView = editButoon->property("view").toString().toStdString();
			string imageId = editButoon->property("imageId").toString().toStdString();
			string defaultImage = editButoon->property("defaultImage").toString().toStdString();
			if (defaultImage == "Yes")
			{
				UpdateImageIntent::GetInstance()->m_setDefaultCheckBox->setChecked(false);
				UpdateImageIntent::GetInstance()->m_setDefaultCheckBox->setChecked(true);
			}
			if (includeAvatar == "Yes")
			{
				Logger::Debug("UpdateProduct -> onImageIntentsTableEditButtonClicked()includeAvatar:" + includeAvatar);
				if (!UpdateImageIntent::GetInstance()->m_includeAvatarCheckBox->isChecked())
				{
					UpdateImageIntent::GetInstance()->m_includeAvatarCheckBox->setAutoExclusive(false);
					UpdateImageIntent::GetInstance()->m_includeAvatarCheckBox->setChecked(false);
					UpdateImageIntent::GetInstance()->m_includeAvatarCheckBox->setChecked(true);
					UpdateImageIntent::GetInstance()->m_includeAvatarCheckBox->setAutoExclusive(true);
				}
			}
			else
			{
				if (UpdateImageIntent::GetInstance()->m_includeAvatarCheckBox->isChecked())
				{
					UpdateImageIntent::GetInstance()->m_includeAvatarCheckBox->setAutoExclusive(false);
					UpdateImageIntent::GetInstance()->m_includeAvatarCheckBox->setChecked(false);
					UpdateImageIntent::GetInstance()->m_includeAvatarCheckBox->setAutoExclusive(true);
				}
			}

			Logger::Debug("UpdateProduct -> onImageIntentsTableEditButtonClicked() Start 2 imageId" + imageId);
			if (colorwayName.find("No_Colorway_Default") != -1)
			{
				UpdateImageIntent::GetInstance()->StyleOrColorwayDropdownClicked("No_Colorway_Default");
			}
			else
				UpdateImageIntent::GetInstance()->StyleOrColorwayDropdownClicked("Colorway");

			int view = stoi(colorwayView);
			Logger::Debug("UpdateProduct -> onImageIntentsTableEditButtonClicked() Start 3");
			m_currentViewSelected = view;
			if (view < 4)
			{
				auto radioButon = static_cast<QRadioButton*>(UpdateImageIntent::GetInstance()->m_viewList->itemWidget(UpdateImageIntent::GetInstance()->m_viewList->item(view)));
				if (radioButon != nullptr)
					radioButon->setChecked(true);
			}
			map<string, QStringList>::iterator it;
			QString labels;
			it = m_imageIntentIdAndLabeMap.find(imageId);
			QStringList imageLabelList;
			if (it != m_imageIntentIdAndLabeMap.end())
			{
				imageLabelList = it->second;
			}
			QString str = imageLabelList.join(',');
			Logger::Debug("UpdateProduct -> onImageIntentsTableEditButtonClicked() str" + str.toStdString());
			if (UpdateImageIntent::GetInstance()->m_labelList->count() > 0)
			{
				for (int index = 0; index < UpdateImageIntent::GetInstance()->m_labelList->count(); index++)
				{
					auto checkbox = static_cast<QCheckBox*>(UpdateImageIntent::GetInstance()->m_labelList->itemWidget(UpdateImageIntent::GetInstance()->m_labelList->item(index)));
					checkbox->setChecked(false);
					if (imageLabelList.contains(checkbox->text()))
					{
						checkbox->setChecked(false);
						checkbox->setChecked(true);
					}

				}
			}
			UpdateImageIntent::GetInstance()->m_addToQueueButton->hide();
			UpdateImageIntent::GetInstance()->m_colorwayComboBox->setCurrentText(QString::fromStdString(colorwayName));
			UpdateImageIntent::GetInstance()->m_colorwayComboBox->setDisabled(true);
			UpdateImageIntent::GetInstance()->m_colorwayComboBox->setFocusPolicy(Qt::NoFocus);
			UpdateImageIntent::GetInstance()->m_colorwayComboBox->setStyleSheet(DISABLED_COMBOBOX_STYLE);

			this->hide();
			UpdateImageIntent::GetInstance()->m_imageQueueTableRowCount = 0;
			UpdateImageIntent::GetInstance()->m_imageQueueTable->setRowCount(0);
			UpdateImageIntent::GetInstance()->m_imageQueueTable->setColumnCount(0);
			UpdateImageIntent::GetInstance()->m_imageQueueTable->clear();

			UpdateImageIntent::GetInstance()->setModal(true);
			UpdateImageIntent::GetInstance()->exec();
		}
		catch (string str)
		{
			RESTAPI::SetProgressBarData(0, "", false);
			Logger::Error("UpdateProduct -> UpdateProduct  Exception - " + str);
			UTILITY_API->DisplayMessageBox(str);
			this->show();
		}
		Logger::Debug("UpdateProduct -> onImageIntentsTableEditButtonClicked() End");

	}

	void UpdateProduct::onImageIntentsTableDeleteButtonClicked(int _rowIndex)
	{
		Logger::Debug("UpdateProduct -> onImageIntentsTableDeleteButtonClicked() Start");
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
		deleteMessage->setText("Are you sure you want to Delete the Image Intent ? ");

		if (deleteMessage->exec() != QMessageBox::Yes)
		{
			isConfirmed = false;
		}
		else
		{
			map<string, UpdateImageIntent::ColorwayViews>::iterator it;
			QPushButton *deleteButoon = static_cast<QPushButton*>(m_imageIntentTable->cellWidget(_rowIndex, DELETE_COLUMN)->children().last());
			string colorwayName = deleteButoon->property("colorwayName").toString().toStdString();
			string colorwayView = deleteButoon->property("view").toString().toStdString();
			int view = stoi(colorwayView);
			m_imageIntentTable->removeRow(_rowIndex);
			m_imageIntentRowcount--;
			it = UpdateImageIntent::GetInstance()->m_ColorwayViewMap.find(colorwayName);
			QString labels;
			if (it != UpdateImageIntent::GetInstance()->m_ColorwayViewMap.end())
			{
				it->second.viewUploadId[view] = "";
				if (it->second.defaultImage == view)
					it->second.defaultImage = -1;// The image intent what we are going to delete setted as default image, here we are removing that from map
			}
			Logger::Debug("UpdateProduct -> onImageIntentsTableDeleteButtonClicked() Start 3");
			QString viewName;
			switch (view)
			{
			case BACK_VIEW:
				viewName = "Back";
				break;
			case FRONT_VIEW:
				viewName = "Front";
				break;
			case LEFT_VIEW:
				viewName = "Left";
				break;
			case RIGHT_VIEW:
				viewName = "Right";
				break;
			}

			//UpdateImageIntent::GetInstance()->m_colorwayViewQueue.removeAll(QString::fromStdString(colorwayName)+ viewName);

			if (m_deleteSignalMapper != nullptr)
			{
				for (int index = 0; index < m_imageIntentTable->rowCount(); index++)
				{
					QPushButton *deleteButoon = static_cast<QPushButton*>(m_imageIntentTable->cellWidget(index, DELETE_COLUMN)->children().last());
					m_deleteSignalMapper->setMapping(deleteButoon, index);
					QPushButton *editButoon = static_cast<QPushButton*>(m_imageIntentTable->cellWidget(index, EDIT_COLUMN)->children().last());
					m_editButtonSignalMapper->setMapping(editButoon, index);
				}
			}
			Logger::Debug("UpdateProduct -> onImageIntentsTableDeleteButtonClicked() Start  5");
			SetTotalImageCount();
		}
		Logger::Debug("UpdateProduct -> onImageIntentsTableDeleteButtonClicked() End");

	}

	void UpdateProduct::FillImageIntentIdAndLabeMap(json _imageJson, string _module)
	{
		Logger::Debug("UpdateProduct -> FillImageIntentIdAndLabeMap() -> Start ");
		Logger::Debug("UpdateProduct -> FillImageIntentIdAndLabeMap()  " + to_string(_imageJson));
		QStringList ImageIdlist;
		map<string, QStringList>::iterator it;
		map<QString, QString>::iterator itr;
		std::map<QString, QString> imageLabelsMap;
		if (_module == "style")
			imageLabelsMap = m_styleImageLabelsMap;
		else
			imageLabelsMap = m_colorwayImageLabelsMap;
		for (auto& itrValues : _imageJson.items())
		{
			QString value = QString::fromStdString(itrValues.value());
			QString key = QString::fromStdString(itrValues.key());
			Logger::Debug("UpdateProduct -> FillImageIntentIdAndLabeMap() -> value: " + value.toStdString());
			Logger::Debug("UpdateProduct -> FillImageIntentIdAndLabeMap() -> Key: " + key.toStdString());

			QStringList list;
			if (key.isEmpty())
				key = "default";

			if (ImageIdlist.contains(value))
			{
				it = m_imageIntentIdAndLabeMap.find(value.toStdString());
				if (it != m_imageIntentIdAndLabeMap.end())
				{
					list = it->second;
					m_imageIntentIdAndLabeMap.erase(value.toStdString());
				}
			}
			else
				ImageIdlist.append(value);

#ifdef __APPLE__
			auto result = std::find_if(std::begin(imageLabelsMap), std::end(imageLabelsMap), [&](const std::pair<QString, QString> &pair) { return pair.second == key; });
#else
			auto result = std::find_if(imageLabelsMap.begin(), imageLabelsMap.end(),[key](const auto& mo) {return mo.second == key; });
#endif

			//getting key based on value 
			if (result != imageLabelsMap.end())
			{
				Logger::Debug("UpdateProduct -> FillImageIntentIdAndLabeMap() -> label: " + result->first.toStdString());
				list.append(result->first);
			}	
			if (key == "default")
			{
				list.append(key);
			}

			m_imageIntentIdAndLabeMap.insert(make_pair(value.toStdString(), list));

		}
		Logger::Debug("UpdateProduct -> FillImageIntentIdAndLabeMap() -> End ");
	}

	void UpdateProduct::FillNonCloImageMap(json _imageJson, string _id)
	{
		Logger::Debug("UpdateProduct -> FillNonCloImageMap() -> Start ");
		Logger::Debug("UpdateProduct -> FillNonCloImageMap()  " + to_string(_imageJson));
		QStringList ImageIdlist;
		map<QString, QStringList>::iterator it;

		QStringList imageLablist;
		QString labelListKey;
		for (auto& itrValues : _imageJson.items())
		{
			QString imageId = QString::fromStdString(itrValues.value());
			QString key = QString::fromStdString(itrValues.key());
			Logger::Debug("UpdateProduct -> FillNonCloImageMap() -> value: " + imageId.toStdString());
			Logger::Debug("UpdateProduct -> FillNonCloImageMap() -> Key: " + key.toStdString());


			Logger::Debug("UpdateProduct -> FillNonCloImageMap() -> ======1 " + _id);
			if (imageId.toStdString() == _id)
			{
				Logger::Debug("UpdateProduct -> FillNonCloImageMap() ->  true=======");

				if (key.isEmpty())
					key = "default";

				if (ImageIdlist.contains(imageId))
				{
					it = m_nonCloStyleImageLabelsMap.find(imageId);
					if (it != m_nonCloStyleImageLabelsMap.end())
					{
						imageLablist = it->second;
						m_nonCloStyleImageLabelsMap.erase(imageId);
					}
				}
				else
					ImageIdlist.append(QString::fromStdString(_id));
				labelListKey = key;
				if (key.length() < 2)
				{
					Logger::Debug("UpdateProduct -> FillNonCloImageMap() -> ======1 " + to_string(key.length()));
					int labelKey = stoi(key.toStdString());
					Logger::Debug("UpdateProduct -> FillNonCloImageMap() -> ======2 " + to_string(labelKey));
					if (labelKey <= 4)
					{
						labelKey = labelKey + 4;
						Logger::Debug("UpdateProduct -> FillNonCloImageMap() -> ======4 " + to_string(labelKey));
						string str = to_string(labelKey);
						Logger::Debug("UpdateProduct -> FillNonCloImageMap() -> ======5 " + str);
						labelListKey = QString::fromStdString(str);
						//imageLablist.append(labelListKey);
						Logger::Debug("UpdateProduct -> FillNonCloImageMap() -> ======6 " + labelListKey.toStdString());
					}
				}
				imageLablist.append(labelListKey);

				for (auto i = 0; i < imageLablist.size(); i++)
				{
					Logger::Debug("UpdateProduct -> FillNonCloImageMap() -> ======7 " + imageLablist.at(i).toStdString());
				}

				m_nonCloStyleImageLabelsMap.insert(make_pair(QString::fromStdString(_id), imageLablist));
			}
		}
		Logger::Debug("UpdateProduct -> FillNonCloImageMap() -> End ");
		Logger::Debug("UpdateProduct -> FillNonCloImageMap() -> End " + to_string(m_nonCloStyleImageLabelsMap.size()));

	}




	void UpdateProduct::exportTurntableImages()
	{
		Logger::Debug("UpdateProduct -> exportTurntableImages() -> Start ");

		int cloColorwaySelectedIndex = UTILITY_API->GetCurrentColorwayIndex();
		int colorwayCount = UTILITY_API->GetColorwayCount();
		Logger::Debug("UpdateProduct -> exportTurntableImages() colorwayCount" + to_string(colorwayCount));

		int colorwayIndex = 0;

		string temporaryPath = UTILITY_API->GetCLOTemporaryFolderPath();

		Logger::Debug("UpdateProduct->exportTurntableImages() temporaryPath" + temporaryPath);
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

		Logger::Debug("UpdateProduct -> exportTurntableImages() -> End ");
	}

	void UpdateProduct::onImageIntentsTableHorizontalHeaderClicked(int _column)
	{
		Logger::Info("UpdateProduct -> onImageIntentsTableHorizontalHeaderClicked() -> Start");
		Logger::Debug("Column.." + to_string(_column));
		if (_column == IMAGE_INTENT_COLUMN || _column == EDIT_COLUMN || _column == DELETE_COLUMN)
			m_imageIntentTable->setSortingEnabled(false);
		else
		{
			m_imageIntentTable->setSortingEnabled(true);
		}
		Logger::Info("UpdateProduct -> onImageIntentsTableHorizontalHeaderClicked() -> End");
	}

	void UpdateProduct::onColorwayTableHorizontalHeaderClicked(int _column)
	{
		Logger::Info("UpdateProduct -> onColorwayTableHorizontalHeaderClicked() -> Start");

		Logger::Debug("Column.." + to_string(_column));
		if (_column == ASSOCIATE_COLOR_COLUMN || _column == PLM_COLORWAY_COLUMN || _column == CLO_COLORWAY_COLUMN || _column == COLOR_CHIP_COLUMN || _column == UNI_2_DIGIT_CODE_COLUMN || _column == DESCRIPTION_COLUMN)
			ui_colorwayTable->setSortingEnabled(false);
		else
		{
			ui_colorwayTable->setSortingEnabled(true);
		}
		Logger::Info("UpdateProduct -> onColorwayTableHorizontalHeaderClicked() -> End");
	}

	void UpdateProduct::FillNonCloColorwayImageMap(json _imageJson, string _imageId, QString _colorwayId)
	{
		Logger::Debug("UpdateProduct -> FillNonCloColorwayImageMap() -> Start ");
		Logger::Debug("UpdateProduct -> FillNonCloColorwayImageMap()  " + to_string(_imageJson));
		QStringList ImageIdlist;
		map<QString, QStringList>::iterator it;
		map<QString, QStringList> nonCloColorwayImageLabelsMap;
		QStringList imageLablist;
		QString labelListKey;
		for (auto& itrValues : _imageJson.items())
		{
			QString value = QString::fromStdString(itrValues.value());
			QString key = QString::fromStdString(itrValues.key());
			Logger::Debug("UpdateProduct -> FillImageIntentIdAndLabeMap() -> value: " + value.toStdString());
			Logger::Debug("UpdateProduct -> FillImageIntentIdAndLabeMap() -> Key: " + key.toStdString());

			QStringList list;
			if (key.isEmpty())
				key = "default";
			if (_imageId == value.toStdString())
			{
				if (ImageIdlist.contains(value))
				{
					it = nonCloColorwayImageLabelsMap.find(value);
					if (it != nonCloColorwayImageLabelsMap.end())
					{
						list = it->second;
						nonCloColorwayImageLabelsMap.erase(value);
					}
				}
				else
					ImageIdlist.append(value);

				list.append(key);

				labelListKey = key;
				if (key.length() < 2)
				{
					Logger::Debug("UpdateProduct -> FillNonCloColorwayImageMap() -> ======1 " + to_string(key.length()));
					int labelKey = stoi(key.toStdString());
					Logger::Debug("UpdateProduct -> FillNonCloColorwayImageMap() -> ======2 " + to_string(labelKey));
					if (labelKey <= 4)
					{
						labelKey = labelKey + 4;
						Logger::Debug("UpdateProduct -> FillNonCloColorwayImageMap() -> ======4 " + to_string(labelKey));
						string str = to_string(labelKey);
						Logger::Debug("UpdateProduct -> FillNonCloColorwayImageMap() -> ======5 " + str);
						labelListKey = QString::fromStdString(str);
						//imageLablist.append(labelListKey);
						Logger::Debug("UpdateProduct -> FillNonCloColorwayImageMap() -> ======6 " + labelListKey.toStdString());
					}
				}
				imageLablist.append(labelListKey);

				for (auto i = 0; i < imageLablist.size(); i++)
				{
					Logger::Debug("UpdateProduct -> FillNonCloImageMap() -> ======7 " + imageLablist.at(i).toStdString());
				}
			}
		}
	
	nonCloColorwayImageLabelsMap.insert(make_pair(QString::fromStdString(_imageId), imageLablist));
	m_nonCloColorWayImageLabelsMap.insert(make_pair(_colorwayId, nonCloColorwayImageLabelsMap));
	Logger::Debug("UpdateProduct -> FillNonCloImageMap() -> End ");
	Logger::Debug("UpdateProduct -> FillNonCloImageMap() -> End " + to_string(m_nonCloStyleImageLabelsMap.size()));
}
	void UpdateProduct::OnColorwaysTableDeleteButtonClicked(int _row)
	{
		Logger::Debug("UpdateProduct -> OnColorwaysTableDeleteButtonClicked() -> Start");
		//QModelIndex index = ui_colorwayTable->indexAt(_pos);
		//if ((index.column() != CHECKBOX_COLUMN) && (index.column() != CLO_COLORWAY_COLUMN) && (index.column() != UNI_2_DIGIT_CODE_COLUMN) && (index.column() != COLOR_CHIP_COLUMN) && (index.column() != PLM_COLORWAY_COLUMN)) // Disabling right click context menu for 0th and 1st colum in the colorway result table
		//{
			//QMenu menu;
		map<string, UpdateImageIntent::ColorwayViews>::iterator it;
		//QAction* removeAction = menu.addAction(QString::fromStdString("Delete"));
		//QAction* rightClickAction = menu.exec(ui_colorwayTable->viewport()->mapToGlobal(_pos));
		//if (removeAction == rightClickAction)
		//{
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
			//QTableWidgetItem* item = ui_colorwayTable->itemAt(_pos);
			//if (item)
			//{

				//int rowIndex = ui_colorwayTable->row(item);
			QWidget* widget = ui_colorwayTable->cellWidget(_row, 1);
			QComboBox *colorwayNameCombo = static_cast<QComboBox*>(ui_colorwayTable->cellWidget(_row, CLO_COLORWAY_COLUMN)->children().last());
			QLineEdit *plmColorwayTextBox = static_cast<QLineEdit*>(ui_colorwayTable->cellWidget(_row, PLM_COLORWAY_COLUMN)->children().last());
			QString plmColorwayName = colorwayNameCombo->currentText();

			string colorwayId = colorwayNameCombo->property("colorwayId").toString().toStdString();
			string colorSpecId = colorwayNameCombo->property("Id").toString().toStdString();

			if (!colorwayId.empty())
			{
				Logger::Debug("UpdateProduct -> OnColorwaysTableDeleteButtonClicked() -> size 1:" + to_string(UpdateImageIntent::GetInstance()->m_ColorwayViewMap.size()));
					m_DeletedColorwayList.append(QString::fromStdString(colorwayId));
							
				it = UpdateImageIntent::GetInstance()->m_ColorwayViewMap.find(plmColorwayName.toStdString());
				if (it != UpdateImageIntent::GetInstance()->m_ColorwayViewMap.end())
					UpdateImageIntent::GetInstance()->m_ColorwayViewMap.erase(plmColorwayName.toStdString());

				Logger::Debug("UpdateProduct -> OnColorwaysTableDeleteButtonClicked() -> size2:" + to_string(UpdateImageIntent::GetInstance()->m_ColorwayViewMap.size()));
			}
			else
			{
				it = UpdateImageIntent::GetInstance()->m_ColorwayViewMap.find(plmColorwayName.toStdString());
				if (it != UpdateImageIntent::GetInstance()->m_ColorwayViewMap.end())
					UpdateImageIntent::GetInstance()->m_ColorwayViewMap.erase(plmColorwayName.toStdString());
			}			
			if (!colorSpecId.empty())
			{
				m_colorSpecList.removeOne(QString::fromStdString(colorSpecId));
			}
			ui_colorwayTable->removeRow(_row);
			int count = 0;
			int size = m_imageIntentTable->rowCount();
			QStringList selectedIndexs;
			for (int index = 0; index < size; index++)
			{
				QPushButton *deleteButoon = static_cast<QPushButton*>(m_imageIntentTable->cellWidget(index, DELETE_COLUMN)->children().last());
				QString colorwayName = deleteButoon->property("colorwayName").toString();
				Logger::Debug("UpdateProduct -> OnColorwaysTableDeleteButtonClicked() -> plmColorwayName:" + plmColorwayName.toStdString());
				Logger::Debug("UpdateProduct -> OnColorwaysTableDeleteButtonClicked() -> colorwayName:" + colorwayName.toStdString());

				if (plmColorwayName == colorwayName)
				{
					selectedIndexs.append(QString::fromStdString(to_string(index)));	//Storing row index corresponding to colorway which user want to delete							
				}
			}
			for (int index = selectedIndexs.size() - 1; index >= 0; index--)
			{
				m_imageIntentTable->removeRow(selectedIndexs[index].toInt()); //deleting row from table, in reverse order
				m_imageIntentRowcount--;
			}

			SetTotalImageCount();
			if (m_updateColorButtonSignalMapper != nullptr)
			{
				for (int i = 0; i < ui_colorwayTable->rowCount(); i++)
				{
					QPushButton *colorButoon = UIHelper::GetButtonWidgetFromCell(ui_colorwayTable, i, UPDATE_BTN_COLUMN, 0);
					if (colorButoon)
						m_updateColorButtonSignalMapper->setMapping(colorButoon, i);
					if (m_updateColorButtonSignalMapper != nullptr)
					{
						QPushButton *CreateButoon = UIHelper::GetButtonWidgetFromCell(ui_colorwayTable, i, UPDATE_BTN_COLUMN, 1);
						if (CreateButoon)
						{
							QList<QAction*> actions;
							actions = CreateButoon->menu()->actions();
							m_createActionSignalMapper->setMapping(actions.at(0), i);
						}
						if (m_printActionSignalMapper != nullptr)
						{
							if (CreateButoon)
							{
								QList<QAction*> actions;
								actions = CreateButoon->menu()->actions();
								m_createActionSignalMapper->setMapping(actions.at(FIRST_INDEX), i);
							}
						}
					}
				}
				if (m_updateColorwayDeleteSignalMapper != nullptr)
				{
					for (int index = 0; index < ui_colorwayTable->rowCount(); index++)
					{
						QPushButton *deleteButton = static_cast<QPushButton*>(ui_colorwayTable->cellWidget(index, COLORWAY_DELETE_COLUMN)->children().last());
						m_updateColorwayDeleteSignalMapper->setMapping(deleteButton, index);
						//QPushButton *editButton = static_cast<QPushButton*>(ui_colorwayTab->cellWidget(index, COLORWAY_DELETE_COLUMN)->children().last());
						//m_colorwayDeleteSignalMapper->setMapping(editButton, index);
					}
				}
			}
			if (m_deleteSignalMapper != nullptr)
			{
				for (int index = 0; index < m_imageIntentTable->rowCount(); index++)
				{
					QPushButton *deleteButoon = static_cast<QPushButton*>(m_imageIntentTable->cellWidget(index, DELETE_COLUMN)->children().last());
					m_deleteSignalMapper->setMapping(deleteButoon, index);
					QPushButton *editButoon = static_cast<QPushButton*>(m_imageIntentTable->cellWidget(index, EDIT_COLUMN)->children().last());
					m_editButtonSignalMapper->setMapping(editButoon, index);
				}
			}
			//}
			//else
			//{
			//	UTILITY_API->DisplayMessageBox("Select Image To Delete");
			//}
		//}
	//}
			for (int count = 0; count < ui_colorwayTable->rowCount(); count++)
			{
				QComboBox * digiCodeCombo = static_cast<QComboBox*>(ui_colorwayTable->cellWidget(count, UNI_2_DIGIT_CODE_COLUMN)->children().last());
				digiCodeCombo->setProperty("row", count);
			}
			//}
			Logger::Debug("UpdateProduct -> OnColorwaysTableDeleteButtonClicked() -> End");
		}
	}

	/*
Description - onAddNewBomClicked() called when add new bom btton clicked 
	* Parameter -
	* Exception -
	*Return -
	*/
	void UpdateProduct::onAddNewBomClicked()
	{
		this->hide();
		AddNewBom::GetInstance()->setModal(true);
		AddNewBom::GetInstance()->exec();

	}
	/*
Description - GetMappedColorway() used read and set mapped colorway from colorway tab
	* Parameter -
	* Exception -
	*Return -
	*/
	void UpdateProduct::GetMappedColorway()
	{
		Logger::Debug("UpdateProduct -> GetMappedColorway() -> Start");
		m_mappedColorways.clear();
		for (int count = 0; count < ui_colorwayTable->rowCount(); count++)
		{
			QLineEdit * plmColorwayCombo = static_cast<QLineEdit*>(ui_colorwayTable->cellWidget(count, PLM_COLORWAY_COLUMN)->children().last());
			QComboBox * cloColorwayCombo = static_cast<QComboBox*>(ui_colorwayTable->cellWidget(count, CLO_COLORWAY_COLUMN)->children().last());
			Logger::Debug("UpdateProduct -> GetMappedColorway() -> plmColorway:" + plmColorwayCombo->text().toStdString());
			Logger::Debug("UpdateProduct -> GetMappedColorway() ->cloColorway:" + cloColorwayCombo->currentText().toStdString());
			QString plmColorwayName = plmColorwayCombo->text();
			QString cloColorwayName = cloColorwayCombo->currentText();
			if (!plmColorwayName.isEmpty() && !cloColorwayName.isEmpty())
			{
				m_CloAndPLMColorwayMap.insert(make_pair(plmColorwayName.toStdString(), cloColorwayName.toStdString()));
				m_mappedColorways.append(cloColorwayCombo->currentText());
				Logger::Debug("UpdateProduct -> GetMappedColorway() -> mappedColorway:" + cloColorwayCombo->currentText().toStdString());
			}
		}
		Logger::Debug("UpdateProduct -> GetMappedColorway() -> End");
	}

	/*
Description - SetUpdateBomFlag() used set bom need to be updated
	* Parameter -
	* Exception -
	*Return -
	*/

	void UpdateProduct::SetUpdateBomFlag(bool _flag)
	{
		m_updateBomTab = _flag;
	}
	/*
Description - AddMaterialInBom() used to add material in bom table
	* Parameter -
	* Exception -
	*Return -
	*/
	void UpdateProduct::AddMaterialInBom()
	{
		Logger::Debug("UpdateProduct -> AddMaterialInBom() -> Start");
		UpdateProductBOMHandler::GetInstance()->AddMaterialInBom();
		Logger::Debug("UpdateProduct -> AddMaterialInBom() -> End");
	}

	/*
Description - ClearBOMData() used to clear bom tab data and UI
	* Parameter -
	* Exception -
	*Return -
	*/
	void UpdateProduct::ClearBOMData()
	{
		Logger::Debug("UpdateProduct -> ClearBOMData() -> Start");
		UpdateProductBOMHandler::GetInstance()->ClearBomData();
		ClearAllFields(AddNewBom::GetInstance()->m_createBOMTreeWidget);
		BOMUtility::ClearBomSectionLayout(ui_sectionLayout);
		m_bomAddButton->show();
		m_bomAddButton->setEnabled(true);
		m_CloAndPLMColorwayMap.clear();
		Logger::Debug("UpdateProduct -> ClearBOMData() -> End");
	}

	void UpdateProduct::refreshImageIntents()
	{

			int imageRowCount = m_imageIntentTable->rowCount();
			m_imageIntentTable->setColumnCount(m_ImageIntentsColumnsNames.size());
			m_imageIntentTable->setHorizontalHeaderLabels(m_ImageIntentsColumnsNames);
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
				string viewName;
				int view;
				int rowCount = m_imageIntentTable->rowCount();
				string temporaryPath = UTILITY_API->GetCLOTemporaryFolderPath();

				for (int index = 0; index < rowCount; index++)
				{

					QTableWidgetItem* item = m_imageIntentTable->item(index, COLORWAY_COLUMN);
					Logger::Debug("UpdateProduct -> onTabClicked() -> Item" + item->text().toStdString());
					colorwayName = item->text().toStdString();

					Logger::Debug("UpdateProduct -> onTabClicked() -> clorwayname" + colorwayName);


					QTableWidgetItem* viewItem = m_imageIntentTable->item(index, IMAGE_VIEW_COLUMN);
					Logger::Debug("UpdateProduct -> onTabClicked() -> Item" + viewItem->text().toStdString());
					string ViewText;
					ViewText = viewItem->text().toStdString();
					int length = ViewText.length();
					int indexOfColon = ViewText.find(":");
					viewName = ViewText.substr(indexOfColon + 1, length);
					Logger::Debug("UpdateProduct -> onTabClicked() -> viewName" + viewName);

					if (viewName.find("Back") != -1)
						view = BACK_VIEW;
					else if (viewName.find("Front") != -1)
						view = FRONT_VIEW;
					else if (viewName.find("Left") != -1)
						view = LEFT_VIEW;
					else
						view = RIGHT_VIEW;

					QPushButton *deleteButton = static_cast<QPushButton*>(m_imageIntentTable->cellWidget(index, DELETE_COLUMN)->children().last());
					string includeAvatar = deleteButton->property("includeAvatar").toString().toStdString();
					Logger::Debug("UpdateProduct -> refreshImageIntents() -> includeAvatar" + includeAvatar);
					QString filepath;
					if (includeAvatar == "Yes")
						filepath = QString::fromStdString(temporaryPath) + "CLOViseTurntableImages/WithAvatar/Avatar_" + QString::fromStdString(colorwayName) + "_" + QString::fromStdString(to_string(view)) + ".png";
					else
						filepath = QString::fromStdString(temporaryPath) + "CLOViseTurntableImages/WithoutAvatar/" + QString::fromStdString(colorwayName) + "_" + QString::fromStdString(to_string(view)) + ".png";

					Logger::Debug("UpdateProduct -> refreshImageIntents() -> filepath" + filepath.toStdString());


					QPixmap pix(filepath);
					pix.scaled(QSize(80, 80), Qt::KeepAspectRatio);
					QWidget *pColorWidget = nullptr;
					QLabel* label = new QLabel();
					label->setMaximumSize(QSize(80, 80));
					int w = label->width();
					int h = label->height();
					label->setPixmap(QPixmap(pix.scaled(w, h, Qt::KeepAspectRatio)));
					pColorWidget = CVWidgetGenerator::InsertWidgetInCenter(label);
					m_imageIntentTable->setCellWidget(index, IMAGE_INTENT_COLUMN, pColorWidget);

				}
			}
     }

	void UpdateProduct::GetLatestRevisionStyleID(string _styleID)
	{
		string attachmentResponse = RESTAPI::CentricRestCallGet(Configuration::GetInstance()->GetPLMServerURL() + RESTAPI::ATTACHMENTS_LATEST_REVISION_RESULTS_API + _styleID + "?revision_details=true&limit=100&decode=true&file_ext=" + ZPRJ, APPLICATION_JSON_TYPE, "");
		string glbAttachmentResponse = RESTAPI::CentricRestCallGet(Configuration::GetInstance()->GetPLMServerURL() + RESTAPI::ATTACHMENTS_LATEST_REVISION_RESULTS_API + _styleID + "?revision_details=true&limit=100&decode=true&file_ext=" + ZIP, APPLICATION_JSON_TYPE, "");
	
		Logger::RestAPIDebug("_selectedIdList.contains(QString::fromStdString(styleId)::documentjson::" + attachmentResponse);
		if (FormatHelper::HasError(attachmentResponse))
		{
			Helper::GetCentricErrorMessage(attachmentResponse);
			throw runtime_error(attachmentResponse);
		}
		if (FormatHelper::HasError(glbAttachmentResponse))
		{
			Helper::GetCentricErrorMessage(glbAttachmentResponse);
			throw runtime_error(glbAttachmentResponse);
		}

		json attachmentjson = json::parse(attachmentResponse);

		for (int attachmenAarrayCount = 0; attachmenAarrayCount < attachmentjson.size(); attachmenAarrayCount++)
		{
			json attachmentCountJson = Helper::GetJSONParsedValue<int>(attachmentjson, attachmenAarrayCount, false);
			string documentName = Helper::GetJSONValue<string>(attachmentCountJson, NODE_NAME_KEY, true);

			string documentId = Helper::GetJSONValue<string>(attachmentCountJson, "id", true);

			if (!FormatHelper::HasContent(documentName))
				documentName = "(unnamed)";
			json revisionDetailsJson = Helper::GetJSONParsedValue<string>(attachmentCountJson, "revision_details", false);
			string modifiedAt = Helper::GetJSONValue<string>(attachmentCountJson, "_modified_at", true);
			string latestVersionAttName = "";
			string latestRevisionId = "";
			//string modifiedAt = "";
			for (int attachmenAarrayCount = 0; attachmenAarrayCount < revisionDetailsJson.size(); attachmenAarrayCount++)
			{
				json attachmentCountJson = Helper::GetJSONParsedValue<int>(revisionDetailsJson, attachmenAarrayCount, false);
				latestVersionAttName = Helper::GetJSONValue<string>(attachmentCountJson, "file_name", true);
				latestRevisionId = Helper::GetJSONValue<string>(attachmentCountJson, ATTRIBUTE_ID, true);
				Logger::Logger("product latestRevisionId==================" + latestRevisionId);
			}
			Logger::Logger("product latestRevisionId==================" + latestRevisionId);
			PublishToPLMData::GetInstance()->SetLatestRevision(latestRevisionId);
		}
		json glbAttachmentjson = json::parse(glbAttachmentResponse);
		if (glbAttachmentjson.size() == 0)
		{
			PublishToPLMData::GetInstance()->SetIsCreateNewGLBDocument(true);
		}
		else
		{
			PublishToPLMData::GetInstance()->SetIsCreateNewGLBDocument(false);
			for (int attachmenAarrayCount = 0; attachmenAarrayCount < glbAttachmentjson.size(); attachmenAarrayCount++)
			{
				json attachmentCountJson = Helper::GetJSONParsedValue<int>(glbAttachmentjson, attachmenAarrayCount, false);
				string documentName = Helper::GetJSONValue<string>(attachmentCountJson, NODE_NAME_KEY, true);

				string documentId = Helper::GetJSONValue<string>(attachmentCountJson, "id", true);

				if (!FormatHelper::HasContent(documentName))
					documentName = "(unnamed)";
				json revisionDetailsJson = Helper::GetJSONParsedValue<string>(attachmentCountJson, "revision_details", false);
				Logger::Logger("revisionDetailsJson===========" + to_string(revisionDetailsJson));
				string modifiedAt = Helper::GetJSONValue<string>(attachmentCountJson, "_modified_at", true);
				string latestVersionAttName = "";
				string latestRevisionId = "";
				//string modifiedAt = "";
				for (int attachmenAarrayCount = 0; attachmenAarrayCount < revisionDetailsJson.size(); attachmenAarrayCount++)
				{
					json attachmentCountJson = Helper::GetJSONParsedValue<int>(revisionDetailsJson, attachmenAarrayCount, false);
					latestVersionAttName = Helper::GetJSONValue<string>(attachmentCountJson, "file_name", true);
					Logger::Logger("latestVersionAttName===========" + latestVersionAttName);

					latestRevisionId = Helper::GetJSONValue<string>(attachmentCountJson, ATTRIBUTE_ID, true);
					Logger::Logger("GLB latestRevisionId===========" + latestRevisionId);

				}
				Logger::Logger(" GLB latestRevisionId===========1" + latestRevisionId);
				PublishToPLMData::GetInstance()->SetGLBLatestRevision(latestRevisionId);
			}
		}

	}

}
