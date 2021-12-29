/*
* Copyright 2020-2021 CLO-Vise. All rights reserved
*
* @file PublishToPLM.cpp
*
* @brief Class implementation for Create Material and Document from CLO to PLM.
* This class has all the variable and methods implementation which are used to PLM Create Material instance data update and Publish from CLO to PLM.
*
* @author GoVise
*
* @date 14-AUG-2020
*/
#include "CreateMaterial.h"

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
#include "CLOVise/PLM/Inbound/Material/MaterialConfig.h"
#include "CLOVise/PLM/Helper/Util/FlexTypeHelper.h"
#include "CLOVise/PLM/Helper/UIHelper/UIHelper.h"
#include "CLOVise/PLM/Helper/Util/Logger.h"
#include "CLOVise/PLM/CLOViseSuite/CLOViseSuite.h"
#include "CLOVise/PLM/Outbound/CraeteMaterial/CreateMaterialConfig.h"

using namespace std;

namespace CLOVise
{
	CreateMaterial* CreateMaterial::_instance = NULL;

	CreateMaterial* CreateMaterial::GetInstance()
	{
		if (_instance == NULL) {
			_instance = new CreateMaterial();
		}

		return _instance;
	}
	
	CreateMaterial::CreateMaterial(QWidget* parent) : MVDialog(parent)
	{
		setupUi(this);
		Logger::Debug("Create Material constructor() start....");
		//if (!CreateMaterialConfig::GetInstance()->isModelExecuted)
		//	RESTAPI::SetProgressBarData(15, "Loading Create Material", true);

		QString windowTitle =  PLM_NAME + " PLM Create "+ QString::fromStdString(Configuration::GetInstance()->GetLocalizedMaterialClassName());
		this->setWindowTitle(windowTitle);

#ifdef __APPLE__
		this->setWindowFlags(Qt::Dialog | Qt::CustomizeWindowHint | Qt::WindowMinMaxButtonsHint | Qt::WindowCloseButtonHint);
#else
		this->setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint | Qt::CustomizeWindowHint);
		m_pTitleBar = new MVTitleBar(windowTitle, this);
		layout()->setMenuBar(m_pTitleBar);
#endif // !__APPLE__
		m_CreateMaterialTreeWidget_1 = nullptr;
		m_CreateMaterialTreeWidget_2 = nullptr;
		m_cancelButton = nullptr;
		m_publishButton = nullptr;
		m_addAttachmentButton = nullptr;
		m_tabWidget = nullptr;
		m_overviewTab = nullptr;
		m_attachmentTab = nullptr;
		m_attachmentNameLabel = nullptr;
		m_SaveAndCloseButton = nullptr;

		m_isSaveClicked = false;

		m_CreateMaterialTreeWidget_1 = CVWidgetGenerator::CreatePublishTreeWidget("QTreeWidget { background-color: #262628; border: 1px solid #000;}""QTreeWidget::item { padding :5px; height: 20px; color: #FFFFFF; font-face: ArialMT; font-size: 10px; width: 90px; margin-left: 5px; margin-right: 5px; margin-top: 5px; margin-bottom: 5px; border: none;}""QTreeWidget::item:hover{background-color: #262628;}", true);
		m_CreateMaterialTreeWidget_1->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);	
		m_CreateMaterialTreeWidget_2 = CVWidgetGenerator::CreatePublishTreeWidget("QTreeWidget { background-color: #262628; border: 1px solid #000;}""QTreeWidget::item { padding :5px; height: 20px; color: #FFFFFF; font-face: ArialMT; font-size: 10px; width: 90px; margin-left: 5px; margin-right: 5px; margin-top: 5px; margin-bottom: 5px; border: none;}""QTreeWidget::item:hover{background-color: #262628;}", true);
		m_CreateMaterialTreeWidget_2->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

		m_cancelButton = CVWidgetGenerator::CreatePushButton("Back", BACK_HOVER_PATH, "Back", PUSH_BUTTON_STYLE, 30, true);
		m_publishButton = CVWidgetGenerator::CreatePushButton("Publish", PUBLISH_HOVER_ICON_PATH, "Publish", PUSH_BUTTON_STYLE, 30, true);
		m_SaveAndCloseButton = CVWidgetGenerator::CreatePushButton("Save", SAVE_HOVER_ICON_PATH, "Save", PUSH_BUTTON_STYLE, 30, true);

		QSpacerItem *horizontalSpacer = new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Fixed);

		m_overviewTab = new QWidget();
		m_attachmentTab = new QWidget();
		m_tabWidget = new QTabWidget();

		m_addAttachmentButton = CVWidgetGenerator::CreatePushButton("Add", ADD_HOVER_ICON_PATH, "Add", PUSH_BUTTON_STYLE, 30, true);
		m_attachmentNameLabel = CVWidgetGenerator::CreateLabel("Attachment Name", "Attachment Name", LABEL_STYLE, true);
		m_attachmentNameLabel->setAlignment(Qt::AlignCenter);
		m_attachmentNameLabel->setWordWrap(true);
		m_attachmentNameLabel->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
		m_zfabFilePath = "";
		m_attachmentName = "";
		m_zfabPath = "";

		QHBoxLayout *treeWidgetLayout = new QHBoxLayout;
		QVBoxLayout *attcahmentButtonLayout = new QVBoxLayout;
		m_attachmentsList = new QListWidget();
		m_attachmentsList->setStyleSheet("QToolTip { color: #ffffff; background-color: #000000; border: 1px #000000; }QListWidget::item{color:#00a2e8;background - color:transparent;}");
		m_attachmentsList->setViewMode(QListView::ListMode);
		m_attachmentsList->setResizeMode(QListView::Fixed);
		m_attachmentsList->setEditTriggers(QAbstractItemView::NoEditTriggers);
		m_attachmentsList->setSelectionMode(QAbstractItemView::NoSelection);
		m_attachmentsList->setDragDropMode(QAbstractItemView::NoDragDrop);
		m_attachmentsList->setWordWrap(true);
		m_attachmentsList->setTextElideMode(Qt::ElideRight);
		m_attachmentsList->setContentsMargins(10, 10, 10, 10);
		m_attachmentsList->setIconSize(QSize(80, 80));
		m_attachmentsList->setSpacing(5);
		m_attachmentsList->setResizeMode(QListView::Fixed);
		m_attachmentsList->setUniformItemSizes(true);

		QFrame *frame;
		QGridLayout *gridLayout;
		QSplitter *splitter;

		frame = new QFrame();
		frame->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
		gridLayout = new QGridLayout(frame);
		splitter = new QSplitter(frame);
		splitter->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
		splitter->setOrientation(Qt::Horizontal);
		splitter->insertWidget(0, m_CreateMaterialTreeWidget_1);
		splitter->insertWidget(1, m_CreateMaterialTreeWidget_2);
		gridLayout->setContentsMargins(QMargins(0, 0, 0, 0));
		gridLayout->addWidget(splitter, 0, 0, 1, 1);

		treeWidgetLayout->addWidget(frame);

		m_overviewTab->setLayout(treeWidgetLayout);
		QHBoxLayout *horizontalLayout = new QHBoxLayout();
		horizontalLayout->insertWidget(0, m_addAttachmentButton);
		horizontalLayout->insertSpacerItem(1, horizontalSpacer);

		attcahmentButtonLayout->insertLayout(0, horizontalLayout);
		attcahmentButtonLayout->insertWidget(1, m_attachmentsList);
		m_attachmentTab->setLayout(attcahmentButtonLayout);

		m_tabWidget->setTabText(0, "Overview");
		m_tabWidget->addTab(m_overviewTab, "Overview");

		m_tabWidget->setTabText(1, "Attachment");
		m_tabWidget->addTab(m_attachmentTab, "Attachment");

		QFont font;
		font.setBold(false);
		font.setFamily("ArialMT");
		m_overviewTab->setFont(font);
		m_tabWidget->setFont(font);
		m_tabWidget->setCurrentIndex(0);
		m_tabWidget->setStyleSheet(" QTabWidget::pane { border: none; color: #FFFFFF; font-size: 10px; background-color: #262628; }""QTabBar::tab { width: 100px; padding: 2px; }""QTabBar::tab:selected { border: none; color: #FFFFFF; background-color: \"" + DEFAULT_TAB_BG_COLOR + "\"; }""QTabBar::tab:!selected { color:#FFFFFF; background-color:\"" + SELECTED_TAB_BG_COLOR + "\"; }");

		ui_tabWidgetLayout->addWidget(m_tabWidget);

		ui_buttonsLayout->insertWidget(0, m_cancelButton);
		ui_buttonsLayout->insertSpacerItem(1, horizontalSpacer);
		ui_buttonsLayout->insertWidget(2, m_SaveAndCloseButton);
		ui_buttonsLayout->insertSpacerItem(3, horizontalSpacer);
		ui_buttonsLayout->insertWidget(4, m_publishButton);
		DrawCriteriaWidget(true);
		connectSignalSlots(true);

		Logger::Debug("Create Material constructor() end....");
		//if (!CreateMaterialConfig::GetInstance()->isModelExecuted)
		UTILITY_API->DeleteProgressBar(true);
	}

	CreateMaterial::~CreateMaterial()
	{
		Logger::Info("CreateMaterial -> Destructor() -> Start");
		DeletePointer(m_CreateMaterialTreeWidget_1);
		DeletePointer(m_CreateMaterialTreeWidget_2);
		DeletePointer(m_cancelButton);
		DeletePointer(m_publishButton);
		DeletePointer(m_addAttachmentButton);
		DeletePointer(m_tabWidget);
		DeletePointer(m_overviewTab);
		DeletePointer(m_attachmentTab);
		DeletePointer(m_attachmentNameLabel);
		Logger::Info("CreateMaterial -> Destructor() -> Start");
	}

	/*
	* Description - ResetDateEditWidget() method used to Reset Date Widget.
	* Parameter -
	* Exception -
	* Return -
	*/
	//void CreateMaterial::onResetDateEditWidget()
	//{
	//	//UIHelper::ResetDate(m_CreateMaterialTreeWidget);
	//}

	/*
	* Description - connectSignalSlots() method used to connect and disconnect signal and slots.
	* Parameter -  bool.
	* Exception -
	* Return -
	*/
	void CreateMaterial::connectSignalSlots(bool _b)
	{
		if (_b)
		{
			QObject::connect(m_publishButton, SIGNAL(clicked()), this, SLOT(onPublishToPLMClicked()));
			QObject::connect(m_cancelButton, SIGNAL(clicked()), this, SLOT(onBackButtonClicked()));
			QObject::connect(m_addAttachmentButton, SIGNAL(clicked()), this, SLOT(onAddAttachmentClicked()));
			QObject::connect(m_SaveAndCloseButton, SIGNAL(clicked()), this, SLOT(onSaveAndCloseClicked()));
			//QObject::connect(m_dateResetButton, SIGNAL(clicked()), this, SLOT(onResetDateEditWidget()));
		}
		else
		{
			QObject::disconnect(m_publishButton, SIGNAL(clicked()), this, SLOT(onPublishToPLMClicked()));
			QObject::disconnect(m_cancelButton, SIGNAL(clicked()), this, SLOT(onBackButtonClicked()));
			QObject::disconnect(m_addAttachmentButton, SIGNAL(clicked()), this, SLOT(onAddAttachmentClicked()));
			QObject::disconnect(m_SaveAndCloseButton, SIGNAL(clicked()), this, SLOT(onSaveAndCloseClicked()));
			//QObject::disconnect(m_dateResetButton, SIGNAL(clicked()), this, SLOT(onResetDateEditWidget()));
		}
	}

	/*
	* Description - cancelWindowClicked() method is a slot for cancel button click and close the publish to plm.
	* Parameter -
	* Exception -
	* Return -
	*/
	void CreateMaterial::onBackButtonClicked()
	{
		Logger::Info("CreateMaterial -> onBackButtonClicked() -> Start");
		backButtonClicked();
		Logger::Info("CreateMaterial -> onBackButtonClicked() -> End");
	}

	void CreateMaterial::backButtonClicked()
	{

		Logger::Info("CreateMaterial -> backButtonClicked() -> Start");
		this->hide();
		CLOVise::CLOViseSuite::GetInstance()->setModal(true);
		CLOViseSuite::GetInstance()->show();
		Logger::Info("CreateMaterial -> backButtonClicked() -> End");
	}
	/*
	* Description - Add3DModelDetailsWidgetData() method used to Add 3D Model Details Widget Data.
	* Parameter -
	* Exception -
	* Return -
	*/
	void CreateMaterial::addCreateMaterialDetailsWidgetData()
	{
		Logger::Debug("Create Material add3DModelDetailsWidgetData() start....");
		try
		{
			json fieldsJsonArray = MaterialConfig::GetInstance()->GetMaterialFieldsJSON();
			//UTILITY_API->DisplayMessageBox("fieldsJsonArray:: " + to_string(fieldsJsonArray));
			QStringList emptylist;
			json feildsJson = json::object();
			json attributesJson = json::object();
			string typeName = "";
			for (int feildsCount = 0; feildsCount < fieldsJsonArray.size(); feildsCount++)
			{
				feildsJson = Helper::GetJSONParsedValue<int>(fieldsJsonArray, feildsCount, false);
				typeName = Helper::GetJSONValue<string>(feildsJson, TYPENAME_JSON_KEY, true);
				attributesJson = Helper::GetJSONParsedValue<string>(feildsJson, FILTER_ATTRIBUTES_KEY, false);
				drawCriteriaWidget(attributesJson, m_CreateMaterialTreeWidget_1, m_CreateMaterialTreeWidget_2, PublishToPLMData::GetInstance()->GetActive3DModelMetaData());
				break;
			}

			m_CreateMaterialTreeWidget_1->setColumnCount(2);
			m_CreateMaterialTreeWidget_1->setHeaderHidden(true);
			m_CreateMaterialTreeWidget_1->setWordWrap(true);
			m_CreateMaterialTreeWidget_1->setDropIndicatorShown(false);
			m_CreateMaterialTreeWidget_1->setRootIsDecorated(false);
			m_CreateMaterialTreeWidget_1->setSelectionMode(QAbstractItemView::NoSelection);
			m_CreateMaterialTreeWidget_1->setStyleSheet("QTreeWidget { background-color: #262628; border: 1px solid #000; padding-left: 5px; min-width: 80px; outline: 0;}""QTreeWidget::item {height: 20px; width: 80px; margin-right: 20px; margin-top: 5px; margin-bottom: 5px; border: none; }""QTreeWidget::item:hover{ background-color: #262628; }""QTreeView{outline: 0;}");
			m_CreateMaterialTreeWidget_2->setColumnCount(2);
			m_CreateMaterialTreeWidget_2->setHeaderHidden(true);
			m_CreateMaterialTreeWidget_2->setWordWrap(true);
			m_CreateMaterialTreeWidget_2->setDropIndicatorShown(false);
			m_CreateMaterialTreeWidget_2->setRootIsDecorated(false);
			m_CreateMaterialTreeWidget_2->setSelectionMode(QAbstractItemView::NoSelection);
			m_CreateMaterialTreeWidget_2->setStyleSheet("QTreeWidget { background-color: #262628; border: 1px solid #000; padding-left: 5px; min-width: 80px; outline: 0;}""QTreeWidget::item {height: 20px; width: 80px; margin-right: 20px; margin-top: 5px; margin-bottom: 5px; border: none; }""QTreeWidget::item:hover{ background-color: #262628; }""QTreeView{outline: 0;}");

			/*	if (!PublishToPLMData::GetInstance()->GetDateFlag())
					m_dateResetButton->hide();
				else
					m_dateResetButton->show();*/
		}
		catch (string msg)
		{
			Logger::Error("Create Material add3DModelDetailsWidgetData() Exception - " + msg);
			this->close();
			CLOViseSuite::GetInstance()->show();
		}
		catch (exception & e)
		{
			Logger::Error("Create Material add3DModelDetailsWidgetData() Exception - " + string(e.what()));
			this->close();
			CLOViseSuite::GetInstance()->show();
		}
		catch (const char* msg)
		{
			wstring wstr(msg, msg + strlen(msg));
			Logger::Error("Create Material add3DModelDetailsWidgetData() Exception - " + string(msg));
			this->close();
			CLOViseSuite::GetInstance()->show();
		}

		Logger::Debug("Create Material add3DModelDetailsWidgetData() end....");
	}

	/*
	* Description - DrawCriteriaWidget() method used to create fields in UI.
	* Parameter -  json, QTreeWidget, json.
	* Exception -
	* Return -
	*/
	void CreateMaterial::drawCriteriaWidget(json _attributesJsonArray, QTreeWidget* _documentPushToPLMTree_1, QTreeWidget* _documentPushToPLMTree_2, json _metadataJson)
	{
		Logger::Debug("Create Material drawCriteriaWidget() start....");

		if (_attributesJsonArray.size() < 10)
		{
			UIHelper::CreateWidgetsOnOutBoundTreeWidget(_documentPushToPLMTree_1, STARTING_INDEX, _attributesJsonArray.size(), _attributesJsonArray, "CREATE");
			_documentPushToPLMTree_2->hide();
			connectOnHandleDropDownWidget(_documentPushToPLMTree_1);
		}
		else
		{
			UIHelper::CreateWidgetsOnOutBoundTreeWidget(_documentPushToPLMTree_1, STARTING_INDEX, ceil(_attributesJsonArray.size() / DIVISION_FACTOR), _attributesJsonArray, "CREATE");
			UIHelper::CreateWidgetsOnOutBoundTreeWidget(_documentPushToPLMTree_2, ceil(_attributesJsonArray.size() / DIVISION_FACTOR), _attributesJsonArray.size(), _attributesJsonArray, "CREATE");
			connectOnHandleDropDownWidget(_documentPushToPLMTree_1);
			connectOnHandleDropDownWidget(_documentPushToPLMTree_2);
		}
//		json attJson = json::object();
//		QStringList attScopes;
//		string attScope = "";
//		bool isAttSettable = false;
//		bool isAttUpdatable = false;
//		string isAttEnable = "";
//		string attKey = "";
//		string attValue = "";
//		for (int i = 0; i < _attributesJsonArray.size(); i++)
//		{
//			bool isAttSettable = false;
//			
//			attJson = Helper::GetJSONParsedValue<int>(_attributesJsonArray, i, false);
//			attScope = Helper::GetJSONValue<string>(attJson, ATTRIBUTE_ATTSCOPE_KEY, true);
//			/*if (!attScopes.isEmpty() && !attScopes.contains(QString::fromStdString(attScope)))
//			{
//				continue;
//			}
//*/
//			string attType = Helper::GetJSONValue<string>(attJson, ATTRIBUTE_TYPE_KEY, true);
//			/*if (!Configuration::GetInstance()->GetModifySupportedAttsList().contains(QString::fromStdString(attType)))
//			{
//				Logger::Error("CVWidgetGenerator::CreateSearchCriteriaWidget() - attributeType:: not supported");
//				continue;
//			}*/
//
//			isAttEnable = Helper::IsValueTrue(Helper::GetJSONValue<string>(attJson, ATTRIBUTE_ENABLED_KEY, true));
//			if ("false" == isAttEnable)
//			{
//				continue;
//			}
//			attKey = Helper::GetJSONValue<string>(attJson, ATTRIBUTE_KEY, true);
//			attValue = Helper::GetJSONValue<string>(attJson, ATTRIBUTE_DEFAULT_VALUE_KEY, true);
//			string attSettable = Helper::GetJSONValue<string>(attJson, ATTRIBUTE_SETTABLE_KEY, true);
//			string attRestApiExposed = Helper::GetJSONValue<string>(attJson, ATTRIBUTE_REST_API_EXPOSED, true);
//			if (attRestApiExposed == "false")
//				continue;
//			if (attValue.find("ref(centric:)") != -1)
//			{
//				attValue = "";
//			}
//			if (attSettable.compare("0") == 0)
//			{
//				isAttSettable = true;
//			}
//			drawWidget(attJson, _documentPushToPLMTree, attValue, attKey, isAttSettable);
//		}

		Logger::Debug("Create Material drawCriteriaWidget() end....");
	}

	/*
	* Description - DrawCriteriaWidget() method used to create input fields.
	* Parameter -  json, QTreeWidget, string, string, bool.
	* Exception -
	* Return -
	*/
	//void CreateMaterial::drawWidget(json _attJson, QTreeWidget* _documentPushToPLMTree, string _attValue, string _attKey, bool _attEditable)
	//{
	//	Logger::Debug("Create Material drawCriteriaWidget() start....");
	//	string attName = Helper::GetJSONValue<string>(_attJson, ATTRIBUTE_NAME_KEY, true);
	//	string attType = Helper::GetJSONValue<string>(_attJson, ATTRIBUTE_TYPE_KEY, true);
	//	bool attRequired = Helper::IsValueTrue((Helper::GetJSONValue<string>(_attJson, ATTRIBUTE_REQUIRED_KEY, true)));
	//	if (!FormatHelper::HasContent(_attValue))
	//		_attValue = BLANK;
	//	json enumJson = json::object();
	//	json attEnumsJson = json::array();
	//	string attEnumVale = BLANK;
	//	string attEnumKey = BLANK;
	//	if (attType == MOA_LIST_ATT_TYPE_KEY)
	//	{
	//		QTreeWidgetItem* topLevel = new QTreeWidgetItem();
	//		QListWidget* listWidget = new QListWidget();

	//		attEnumsJson = Helper::GetJSONParsedValue<string>(_attJson, ENUMLIST_JSON_KEY, false);
	//		for (int i = 0; i < attEnumsJson.size(); i++)
	//		{
	//			enumJson = Helper::GetJSONParsedValue<int>(attEnumsJson, i, false);
	//			attEnumVale = Helper::GetJSONValue<string>(enumJson, ENUM_VALUE_KEY, true);
	//			attEnumKey = Helper::GetJSONValue<string>(enumJson, ENUM_KEY, true);
	//			CVWidgetGenerator::CreateCVMultiListWidget(listWidget, QString::fromStdString(attEnumVale), QString::fromStdString(attEnumKey), QString::fromStdString(_attValue));
	//		}

	//		listWidget->setFocusPolicy(Qt::NoFocus);
	//		listWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

	//		listWidget->setStyleSheet("QListWidget{border-bottom: 1px inset #262626; font-size: 10px; font-face: ArialMT; border-left: 1px inset #0D0D0D; max-height:70px; border-top: 1px inset #0D0D0D; border-right: 1px inset #262626; background-color: #222224; } QListWidget::item{ background-color: #222224; margin: 2px;} QListWidget::indicator { width: 10px; height:10px; } QListWidget::indicator:unchecked {background-color: #343434 ; border: 1px solid #3B3B3B;} QListWidget::indicator:checked {image: url(:/CLOVise/PLM/Images/icon_ok_over.svg);background-color: #343434; border: 1px solid #3B3B3B;}");

	//		_documentPushToPLMTree->addTopLevelItem(topLevel);
	//		_documentPushToPLMTree->setItemWidget(topLevel, 0, CVWidgetGenerator::CreateLabelWidget(attName, _attKey, attType, attRequired, false));	// Adding label at column 1
	//		if (!_attEditable)
	//		{
	//			listWidget->setDisabled(true);
	//			listWidget->setStyleSheet("QListWidget{ color: #808080; border-bottom: 1px inset #262626; font-size: 10px; font-face: ArialMT; border-left: 1px inset #0D0D0D; max-height:70px; border-top: 1px inset #0D0D0D; border-right: 1px inset #262626; background-color: #222224; } QListWidget::item{ color: #808080; background-color: #222224; margin: 2px;} QListWidget::indicator { width: 10px; height:10px; } QListWidget::indicator:unchecked {background-color: #343434 ; border: 1px solid #3B3B3B;} QListWidget::indicator:checked {image: url(:/CLOVise/PLM/Images/icon_ok_over.svg);background-color: #343434; border: 1px solid #3B3B3B;}");
	//		}
	//		_documentPushToPLMTree->setItemWidget(topLevel, 1, listWidget);

	//	}
	//	else if (attType == TEXT_AREA_ATT_TYPE_KEY)
	//	{
	//		QTreeWidgetItem* topLevel = new QTreeWidgetItem();  // Creating new TreeWidgetItem		

	//		_documentPushToPLMTree->addTopLevelItem(topLevel);				// Adding ToplevelItem
	//		_documentPushToPLMTree->setItemWidget(topLevel, 0, CVWidgetGenerator::CreateLabelWidget(attName, _attKey, attType, attRequired, false));	// Adding label at column 1
	//		_documentPushToPLMTree->setItemWidget(topLevel, 1, CVWidgetGenerator::CreateTextAreaWidget(_attValue, _attEditable));

	//	}
	//	else if (attType == DATE_ATT_TYPE_KEY || attType == TIME_ATT_TYPE_KEY)
	//	{
	//		PublishToPLMData::GetInstance()->SetDateFlag(true);
	//		QStringList dateList;
	//		QTreeWidgetItem* topLevel = new QTreeWidgetItem();  // Creating new TreeWidgetItem
	//		_documentPushToPLMTree->addTopLevelItem(topLevel);			// Adding ToplevelItem
	//		_documentPushToPLMTree->setItemWidget(topLevel, 0, CVWidgetGenerator::CreateLabelWidget(attName, _attKey, attType, attRequired, false));	// Adding label at column 1
	//		_documentPushToPLMTree->setItemWidget(topLevel, 1, CVWidgetGenerator::CreateDateWidget(_attValue, _attEditable));

	//	}
	//	else if (attType == INTEGER_ATT_TYPE_KEY || attType == CONSTANT_ATT_TYPE_KEY)
	//	{
	//		QTreeWidgetItem* topLevel = new QTreeWidgetItem();  // Creating new TreeWidgetItem
	//		QSpinBox* spinBixWidget = new QSpinBox();
	//		spinBixWidget->setMaximumHeight(SPINBOX_HEIGHT);
	//		spinBixWidget->setStyleSheet(SPINBOX_STYLE);
	//		_documentPushToPLMTree->addTopLevelItem(topLevel);						// Adding ToplevelItem
	//		_documentPushToPLMTree->setItemWidget(topLevel, 0, CVWidgetGenerator::CreateLabelWidget(attName, _attKey, attType, attRequired, false));	// Adding label at column 1
	//		spinBixWidget = CVWidgetGenerator::CreateSpinBoxWidget(QString::fromStdString(_attValue), BLANK);
	//		_documentPushToPLMTree->setItemWidget(topLevel, 1, spinBixWidget);
	//		if (!_attEditable)
	//		{
	//			// for desable 
	//			spinBixWidget->setDisabled(true);
	//			spinBixWidget->setStyleSheet(DISABLED_SPINBOX_STYLE);
	//		}

	//	}
	//	else if (attType == FLOAT_ATT_TYPE_KEY || attType == DOUBLE_ATT_TYPE_KEY)
	//	{
	//		QTreeWidgetItem* topLevel = new QTreeWidgetItem();  // Creating new TreeWidgetItem
	//		QDoubleSpinBox* doubleSpinBoxWidget = new QDoubleSpinBox();
	//		doubleSpinBoxWidget->setMaximumHeight(SPINBOX_HEIGHT);
	//		string attDecimals = Helper::GetJSONValue<string>(_attJson, DECIMAP_FIGURES, true);
	//		_documentPushToPLMTree->addTopLevelItem(topLevel);						// Adding ToplevelItem
	//		_documentPushToPLMTree->setItemWidget(topLevel, 0, CVWidgetGenerator::CreateLabelWidget(attName, _attKey, attType, attRequired,false));	// Adding label at column 1
	//		doubleSpinBoxWidget = CVWidgetGenerator::CreateDoubleSpinBoxWidget(QString::fromStdString(_attValue), BLANK, "0", 0.0000, 10000000000000.0000);
	//		_documentPushToPLMTree->setItemWidget(topLevel, 1, doubleSpinBoxWidget);
	//		if (!_attEditable)
	//		{
	//			// for desable 
	//			doubleSpinBoxWidget->setDisabled(true);
	//			doubleSpinBoxWidget->setStyleSheet(DISABLED_DOUBLE_SPINBOX_STYLE);
	//		}

	//	}
	//	else if (attType == CURRENCY_ATT_TYPE_KEY)
	//	{
	//		QTreeWidgetItem* topLevel = new QTreeWidgetItem();  // Creating new TreeWidgetItem
	//		QDoubleSpinBox* doubleSpinBoxWidget = new QDoubleSpinBox();

	//		string attDecimals = Helper::GetJSONValue<string>(_attJson, DECIMAP_FIGURES, true);
	//		string attCurrencyCode = Helper::GetJSONValue<string>(_attJson, CURRENCY_CODE, true) + " ";
	//		_documentPushToPLMTree->addTopLevelItem(topLevel);						// Adding ToplevelItem
	//		_documentPushToPLMTree->setItemWidget(topLevel, 0, CVWidgetGenerator::CreateLabelWidget(attName, _attKey, attType, attRequired, false));	// Adding label at column 1
	//		doubleSpinBoxWidget = CVWidgetGenerator::CreateDoubleSpinBoxWidget(QString::fromStdString(_attValue), BLANK + attCurrencyCode, attDecimals, 0.00, 10000000000000.00);
	//		_documentPushToPLMTree->setItemWidget(topLevel, 1, doubleSpinBoxWidget);
	//		if (!_attEditable)
	//		{
	//			// for desable 
	//			doubleSpinBoxWidget->setDisabled(true);
	//			doubleSpinBoxWidget->setStyleSheet(DISABLED_DOUBLE_SPINBOX_STYLE);
	//		}

	//	}
	//	else if (attType == BOOLEAN_ATT_TYPE_KEY)
	//	{
	//		QTreeWidgetItem* topLevel = new QTreeWidgetItem();  // Creating new TreeWidgetItem

	//		_documentPushToPLMTree->addTopLevelItem(topLevel);						// Adding ToplevelItem
	//		_documentPushToPLMTree->setItemWidget(topLevel, 0, CVWidgetGenerator::CreateLabelWidget(attName, _attKey, attType, attRequired, false));	// Adding label at column 1
	//		_documentPushToPLMTree->setItemWidget(topLevel, 1, CVWidgetGenerator::CreateBooleanWidget(_attValue, _attEditable, topLevel, 1));	// Adding label at column 2

	//	}
	//	else if (attType == USER_LIST_ATT_TYPE_KEY || attType == CHOICE_ATT_TYPE_KEY || attType == REF_ATT_TYPE_KEY || attType == ENUM_ATT_TYPE_KEY)
	//	{
	//		QTreeWidgetItem* topLevel = new QTreeWidgetItem();  // Creating new TreeWidgetItem
	//		if (attType == REF_ATT_TYPE_KEY || attType == ENUM_ATT_TYPE_KEY)
	//		{
	//			attEnumsJson = Helper::GetJSONParsedValue<string>(_attJson, ENUMLIST_JSON_KEY, false);
	//		}
	//		else
	//		{
	//			attEnumsJson = Helper::GetJSONParsedValue<string>(_attJson, USER_LIST_ATT_TYPE_KEY, false);
	//		}
	//		QStringList valueList;
	//		ComboBoxItem* comboBox = new ComboBoxItem();
	//		comboBox->setFocusPolicy(Qt::StrongFocus);
	//		comboBox->setStyleSheet(COMBOBOX_STYLE);
	//		comboBox->addItem(QString::fromStdString(BLANK));
	//		for (int i = 0; i < attEnumsJson.size(); i++)
	//		{
	//			_attJson = Helper::GetJSONParsedValue<int>(attEnumsJson, i, false);///use new method
	//			attEnumVale = Helper::GetJSONValue<string>(_attJson, ENUM_VALUE_KEY, true);
	//			attEnumKey = Helper::GetJSONValue<string>(_attJson, ENUM_KEY, true);
	//			valueList.push_back(QString::fromStdString(attEnumVale));
	//			comboBox->setProperty(attEnumVale.c_str(), QString::fromStdString(attEnumKey));
	//		}

	//		comboBox->addItems(valueList);
	//		int index = comboBox->findText(QString::fromStdString(_attValue));
	//		if (index != -1) { // -1 for not found
	//			comboBox->setCurrentIndex(index);
	//		}
	//		if (!_attEditable)
	//		{
	//			comboBox->setDisabled(true);
	//			comboBox->setStyleSheet(DISABLED_COMBOBOX_STYLE);
	//		}
	//		_documentPushToPLMTree->addTopLevelItem(topLevel);						// Adding ToplevelItem
	//		_documentPushToPLMTree->setItemWidget(topLevel, 0, CVWidgetGenerator::CreateLabelWidget(attName, _attKey, attType, attRequired, false));	// Adding label at column 1
	//		_documentPushToPLMTree->setItemWidget(topLevel, 1, comboBox);
	//	}
	//	else if (attType == STRING_ATT_TYPE_KEY || attType == TEXT_ATT_TYPE_KEY)
	//	{
	//		QTreeWidgetItem* topLevel = new QTreeWidgetItem();  // Creating new TreeWidgetItem
	//		QLineEdit* LineEdit = new QLineEdit();						// Creating new LineEdit Widget
	//		LineEdit->setAttribute(Qt::WA_MacShowFocusRect, false);
	//		LineEdit->setStyleSheet(LINEEDIT_STYLE);
	//		LineEdit->setText(QString::fromStdString(_attValue));
	//		_documentPushToPLMTree->addTopLevelItem(topLevel);						// Adding ToplevelItem
	//		_documentPushToPLMTree->setItemWidget(topLevel, 0, CVWidgetGenerator::CreateLabelWidget(attName, _attKey, attType, attRequired, false));	// Adding label at column 1
	//		if (!_attEditable)
	//		{
	//			LineEdit->setDisabled(true);
	//			LineEdit->setStyleSheet(DISABLED_LINEEDIT_STYLE);
	//		}
	//		_documentPushToPLMTree->setItemWidget(topLevel, 1, LineEdit);

	//	}

	//	Logger::Debug("Create Material drawWidget() end....");
	//}

	void CreateMaterial::onPublishToPLMClicked()
	{
		Logger::Debug("Create Material onPublishToPLMClicked() Start....");

		if (!m_zfabFilePath.toStdString().empty())
		{
			publishToPLMClicked(); 
		}
		else
		{
			UTILITY_API->DisplayMessageBox("Add an attachment before creating in PLM.");
			this->show();
		}
		
		Logger::Debug("Create Material onPublishToPLMClicked() End....");
	}

	/*
	* Description - PublishToPLMClicked() method is a slot for publish 3D model to plm click.
	* Parameter -
	* Exception - exception, Char *
	* Return -
	*/
	void CreateMaterial::publishToPLMClicked()
	{
		Logger::Debug("Create Material publishToPLMClicked() start....");
		QDir dir;
		try
		{
			this->hide();
			UTILITY_API->DeleteProgressBar(true);			
			collectCreateMaterialFieldsData();
			//UIHelper::ValidateForValidParams(m_documentDetailsJson, PRODUCT_MODULE);
			UIHelper::ValidateRquired3DModelData(m_CreateMaterialTreeWidget_1);
			UIHelper::ValidateRquired3DModelData(m_CreateMaterialTreeWidget_2);
			Configuration::GetInstance()->SetProgressBarProgress(0);
			UTILITY_API->CreateProgressBar();
			UTILITY_API->SetProgress("Creating "+ Configuration::GetInstance()->GetLocalizedMaterialClassName() +"...", (qrand() % 101));
			//UTILITY_API->DisplayMessageBox(m_MaterialMetaData);
			Logger::Logger("m_MaterialMetaData=========== "+ m_MaterialMetaData);
			string response = RESTAPI::PostRestCall(m_MaterialMetaData, Configuration::GetInstance()->GetPLMServerURL() + RESTAPI::CREATE_MATERIAL_API, "content-type: application/json; boundary=----WebKitFormBoundary7MA4YWxkTrZu0gW");
			//UTILITY_API->DisplayMessageBox(response);
			Logger::Logger("m_MaterialMetaData response=========== " + response);
			Logger::RestAPIDebug("Create Material response" + response);
			//RESTAPI::SetProgressBarData(20, "Creating Material", true);
			Configuration::GetInstance()->SetProgressBarProgress(RESTAPI::SetProgressBarProgress(Configuration::GetInstance()->GetProgressBarProgress(), 5, "Creating " + Configuration::GetInstance()->GetLocalizedMaterialClassName()+ "..."));
			if (!FormatHelper::HasContent(response))
			{
				throw "Unable to publish to PLM. Please try again or Contact your System Administrator.";
			}

			if (FormatHelper::HasError(response))
			{
				throw runtime_error(response);
			}
			/*if (response.find("Successful") == string::npos)
			{
				string error = RESTAPI::CheckForErrorMsg(response);
				error = "Failed. Publish to PLM. \n\t" + error;
				if (FormatHelper::HasContent(error))
				{
					throw std::logic_error(error);
				}
				else
				{
					UTILITY_API->DisplayMessageBox("Failed. Publish to PLM.");
					this->show();
				}
			}*/
			else
			{
				json detailJson = Helper::GetJsonFromResponse(response, "{");
				Logger::Debug("PublishToPLMData -> onPublishToPLMClicked 2");
				string productId = Helper::GetJSONValue<string>(detailJson, ATTRIBUTE_ID, true);
				Configuration::GetInstance()->SetProgressBarProgress(RESTAPI::SetProgressBarProgress(Configuration::GetInstance()->GetProgressBarProgress(), 10, "Creating "+ Configuration::GetInstance()->GetLocalizedMaterialClassName() +"..."));
				string fabricFileId = exportFabricFile(productId);
				UploadMaterialThumbnail(productId, fabricFileId);
				UTILITY_API->DeleteProgressBar(true);
				m_attachmentsList->clear();
				m_zfabFilePath.clear();
				m_tabWidget->setCurrentIndex(0);
				CreateMaterialConfig::GetInstance()->SetIsSaveAndCloseClicked(false);
				CVDisplayMessageBox DownloadDialogObject;
				ClearAllFields(m_CreateMaterialTreeWidget_1);
				ClearAllFields(m_CreateMaterialTreeWidget_2);
				DownloadDialogObject.DisplyMessage("Successfully published to PLM.");
				DownloadDialogObject.setModal(true);
				DownloadDialogObject.exec();
				//this->Destroy();
			}

		}
		catch (exception& e)
		{
			UTILITY_API->DeleteProgressBar(true);
			Logger::Error("Create Material-> Create Material Exception - " + string(e.what()));
			UTILITY_API->DisplayMessageBox(e.what());

			this->show();
		}
		catch (const char* msg)
		{
			Logger::Error("Create Material-> Create Material Exception - " + string(msg));
			wstring wstr(msg, msg + strlen(msg));
			UTILITY_API->DeleteProgressBar(true);
			UTILITY_API->DisplayMessageBoxW(wstr);

			this->show();
		}
		catch (string str)
		{
			UTILITY_API->DeleteProgressBar(true);
			Logger::Error("Create Material-> Create Material Exception - " + str);
			UTILITY_API->DisplayMessageBox(str);

			this->show();
		}

		Logger::Debug("Create Material publishToPLMClicked() end....");
	}

	/*
	* Description - collectCreateMaterialFieldsData() method used to collect publish to plm fields data.
	* Parameter -
	* Exception -
	* Return -
	*/
	void CreateMaterial::collectCreateMaterialFieldsData()
	{
		Logger::Debug("Create Material collectCreateMaterialFieldsData() start....");
		//Collects all Product Section related data into map
		collectCreateMaterialTreeData();
		//collectMaterialFieldsData();
		/*m_parameterJson[PRODUCT_DETAIL_KEY] = m_createMaterialDetailsJson;

		string metaData = UTILITY_API->GetMetaDataForCurrentGarment();
		json metadataJSON = json::parse(metaData);
		m_parameterJson[DOCUMENTID_KEY] = Helper::GetJSONValue<string>(metadataJSON, OBJECT_ID, true);
		m_parameterJson[DOCUMENT_DETAIL_KEY] = m_documentDetailsJson;*/
		Logger::Debug("Create Material collectCreateMaterialFieldsData() end....");
	}

	///*
	//* Description - collectMaterialFieldsData() method used to collect product fields data.
	//* Parameter -
	//* Exception -
	//* Return -
	//*/
	//void CreateMaterial::collectMaterialFieldsData()
	//{
	//	Logger::Debug("Create Material collectMaterialFieldsData() start....");
	//	m_parameterJson[PRODUCT_OBJ_ID] = PublishToPLMData::GetInstance()->GetActiveProductObjectId();
	//	m_parameterJson[PRODUCTID_KEY] = PublishToPLMData::GetInstance()->GetActiveProductId();
	//	m_parameterJson[PRODUCT_NAME_KEY] = PublishToPLMData::GetInstance()->GetActiveProductName();
	//	m_parameterJson[PRODUCT_STATUS_KEY] = PublishToPLMData::GetInstance()->GetActiveProductStatus();

	//	m_productDetailsJson[PRODUCTID_KEY] = PublishToPLMData::GetInstance()->GetActiveProductId();
	//	m_productDetailsJson[PRODUCT_NAME_KEY] = PublishToPLMData::GetInstance()->GetActiveProductName();
	//	m_productDetailsJson[PRODUCT_STATUS_KEY] = PublishToPLMData::GetInstance()->GetActiveProductStatus();

	//	m_productDetailsJson[ATTRIBUTES_KEY] = json::array();

	//	json thumbNailDetails = json::object();
	//	thumbNailDetails[THUMBNAIL_NAME_KEY] = m_3DModelThumbnailName;

	//	m_productDetailsJson[THUMBNAIL_DETAIL_KEY] = thumbNailDetails;
	//	Logger::Debug("Create Material collectMaterialFieldsData() end....");
	//}

	/*
	* Description - Collect3DModelFieldsData() method used to collect 3d model fields data.
	* Parameter -
	* Exception -
	* Return -
	*/
	void CreateMaterial::collectCreateMaterialTreeData()
	{
		Logger::Debug("Create Material collectCreateMaterialTreeData() start....");
		m_MaterialMetaData = collectCriteriaFields(m_CreateMaterialTreeWidget_1, m_CreateMaterialTreeWidget_2);
		//UTILITY_API->DisplayMessageBox("m_MaterialMetaData:: " + m_MaterialMetaData);
		Logger::Debug("Create Material collectCreateMaterialTreeData() end....");
	}

	/*
	* Description - PreparePublishRequestParameter() method used to prepare request parameter.
	* Parameter -
	* Exception -
	* Return -
	*/
	string CreateMaterial::getPublishRequestParameter(string _path, string _fileName)
	{
		Logger::Debug("Create product getPublishRequestParameter(string _path, string _fileName) start....");
		string contentType = Helper::GetFileContetType(_path);
		string fileStream = Helper::GetFilestream(_path);
		string contentLength = Helper::getFileLength(_path);
		string postField = "------WebKitFormBoundary7MA4YWxkTrZu0gW\r\nContent-Disposition: form-data; name=\"file\"; filename=" + _fileName + "\r\nContent-Type: " + contentType + "\r\n" + contentLength + "\r\n\r\n" + fileStream + "\r\n------WebKitFormBoundary7MA4YWxkTrZu0gW\r\n";
		if (_fileName.find(".png") != -1)
		{
			postField += "Content-Disposition: form-data; name=\"generated_by\"\r\n\r\n";
			postField += "CLO3D";
			postField += "\r\n";
			postField += "------WebKitFormBoundary7MA4YWxkTrZu0gW--";
			Logger::Debug("Create product getPublishRequestParameter(string _path, string _imageName) end....");
		}
		else
		{
			postField += "Content-Disposition: form-data; name=\"name_name\"\r\n\r\n";
			postField += _fileName;
			postField += "\r\n";
			postField += "------WebKitFormBoundary7MA4YWxkTrZu0gW\r\n";
			postField += "Content-Disposition: form-data; name=\"is_3d_material\"\r\n\r\n";
			postField += "true";
			postField += "\r\n";
			postField += "------WebKitFormBoundary7MA4YWxkTrZu0gW\r\n";
			postField += "Content-Disposition: form-data; name=\"is_3d\"\r\n\r\n";
			postField += "false";
			postField += "\r\n";
			postField += "------WebKitFormBoundary7MA4YWxkTrZu0gW--";
		}
		Logger::Debug("Create product getPublishRequestParameter(string _path, string _fileName) End....");
		return postField;
	}

	/*
	* Description - CollectCriteriaFields() method used to collect data in Publish to plm UI.
	* Parameter -  QTreeWidget.
	* Exception -
	* Return - json.
	*/
	string CreateMaterial::collectCriteriaFields(QTreeWidget* _documentPushToPLMTree_1, QTreeWidget* _documentPushToPLMTree_2)
	{
		Logger::Debug("Create Material collectCriteriaFields() start....");
		json attsJson = json::array();
		json attJson = json::object();
		string attkey;
		string attValue;
		string attType;
		string data = "{";
		try
		{
			for (int i = 0; i < _documentPushToPLMTree_1->topLevelItemCount(); i++)
			{
				QTreeWidgetItem* topItem = _documentPushToPLMTree_1->topLevelItem(i);

				QWidget* qWidgetColumn0 = _documentPushToPLMTree_1->itemWidget(topItem, 0);
				QLabel* qlabel = qobject_cast<QLabel*>(qWidgetColumn0);
				attType = qlabel->property(ATTRIBUTE_TYPE_KEY.c_str()).toString().toStdString();

				attJson = UIHelper::ReadVisualUIFieldValue(_documentPushToPLMTree_1, i);
				if (!attJson.empty())
				{
					attkey = Helper::GetJSONValue<string>(attJson, "attKey", true);
					attValue = Helper::GetJSONValue<string>(attJson, "attValue", true);
					if (!attValue.empty() && attValue != "|@isDisabled@|")
					{
						if (attType == "integer" || attType == "float" || attType == "boolean")
							data += "\n\"" + attkey + "\":" + attValue + ",";
						else
							data += "\n\"" + attkey + "\":\"" + attValue + "\",";
					}
				}
			}
			for (int i = 0; i < _documentPushToPLMTree_2->topLevelItemCount(); i++)
			{
				QTreeWidgetItem* topItem = _documentPushToPLMTree_2->topLevelItem(i);

				QWidget* qWidgetColumn0 = _documentPushToPLMTree_2->itemWidget(topItem, 0);
				QLabel* qlabel = qobject_cast<QLabel*>(qWidgetColumn0);
				attType = qlabel->property(ATTRIBUTE_TYPE_KEY.c_str()).toString().toStdString();

				attJson = UIHelper::ReadVisualUIFieldValue(_documentPushToPLMTree_2, i);
				if (!attJson.empty())
				{
					attkey = Helper::GetJSONValue<string>(attJson, "attKey", true);
					attValue = Helper::GetJSONValue<string>(attJson, "attValue", true);
					if (!attValue.empty() && attValue != "|@isDisabled@|")
					{
						if (attType == "integer" || attType == "float" || attType == "boolean")
							data += "\n\"" + attkey + "\":" + attValue + ",";
						else
							data += "\n\"" + attkey + "\":\"" + attValue + "\",";
					}
				}
			}
		}
		catch (string msg)
		{
			
			Logger::Error("Create Material collectCriteriaFields() Exception - " + msg);
		}
		catch (exception & e)
		{
			Logger::Error("Create Material collectCriteriaFields() Exception - " + string(e.what()));
		}
		catch (const char* msg)
		{
			wstring wstr(msg, msg + strlen(msg));
			Logger::Error("Create Material collectCriteriaFields() Exception - " + string(msg));
		}

		//UTILITY_API->DisplayMessageBox("data_1:: " + data);
		data = data.substr(0, data.length() - 1);

		data += "\n}";
		//UTILITY_API->DisplayMessageBox("data_2:: " + data);
		Logger::Debug("Create Material collectCriteriaFields() end....");
		return data;
	}

	/*
	* Description - addAttachmentClicked() method used to collect data in Publish to plm UI.
	* Parameter -
	* Exception -
	* Return - json.
	*/
	void CreateMaterial::addAttachmentClicked()
	{
		Logger::Debug("CreateFabric -> addAttachmentClicked() -> Start");
		//m_addAttachmentButton->setIcon(QIcon(ADD_NONE_ICON_PATH));
		QString zfabFilePath;
		if (m_zfabFilePath.isEmpty())
		{
			zfabFilePath = QFileDialog::getOpenFileName(this, tr("Add Attachment"), FABRICS_FILE_PATH, "ZFAB Files(*.zfab);; Trim Files(*.trm *.obj *.sst);; Button Files (*.btn *.bth);; Zippers (*.zpac *.zsd *.zip *zpl)");
		}
		else
		{
			zfabFilePath = QFileDialog::getOpenFileName(this, tr("Add Attachment"), QString::fromStdString(m_zfabPath), "ZFAB Files(*.zfab);; Trim Files(*.trm *.obj *.sst);; Button Files (*.btn *.bth);; Zippers (*.zpac *.zsd *.zip *zpl)");
		}
		if (!zfabFilePath.isEmpty()) {
			m_zfabFilePath = zfabFilePath;
		}
		if (!m_zfabFilePath.isEmpty())
		{
			m_attachmentName = m_zfabFilePath.toStdString();
			m_zfabPath = m_zfabFilePath.toStdString();
			const size_t lastSlashIndex = m_attachmentName.find_last_of("/");
			if (std::string::npos != lastSlashIndex)
			{
				m_zfabPath = m_attachmentName.substr(0, lastSlashIndex);
				m_attachmentName.erase(0, lastSlashIndex + 1);
			}

			ExtractThumbnailFromAttachment();
			m_attachmentsList->clear();
			QListWidgetItem* listItem = new QListWidgetItem();
			listItem->setTextAlignment(Qt::AlignCenter | Qt::AlignVCenter);

			QPixmap pixmap(QString::fromStdString(m_MaterialThumbnailPath));
			pixmap.scaled(QSize(80, 80), Qt::KeepAspectRatio);
			QIcon newIcon;
			newIcon.addPixmap(pixmap);

			listItem->setText(QString::fromStdString(m_attachmentName));
			listItem->setToolTip(QString::fromStdString(m_attachmentName));
			listItem->setSizeHint(QSize(100, 100));

			m_attachmentsList->addItem(listItem);
			m_attachmentsList->setIconSize(QSize(80, 80));
			listItem->setData(Qt::DecorationRole, newIcon);

		}

		Logger::Debug("CreateFabric -> addAttachmentClicked() -> End");
	}

	/*
	* Description - onAddAttachmentClicked() method used to collect data in Publish to plm UI.
	* Parameter -
	* Exception -
	* Return - json.
	*/
	void CreateMaterial::onAddAttachmentClicked()
	{
		Logger::Debug("CreateFabric -> onAddAttachmentClicked() -> Start");
		addAttachmentClicked();
		Logger::Debug("CreateFabric -> onAddAttachmentClicked() -> End");
	}

	void CreateMaterial::DeletePointer(QWidget*_ptr)
	{
		Logger::Info("CreateMaterial -> DeletePointer() -> Start");
		if (_ptr != nullptr)
		{
			delete _ptr;
		}
		Logger::Info("CreateMaterial -> DeletePointer() -> End");
	}

	void CreateMaterial::UploadMaterialThumbnail(string _productId, string _fabricFileId)
	{
		
		if (!m_MaterialThumbnailPath.empty())
		{
			//UTILITY_API->DisplayMessageBox(m_MaterialThumbnailPath);

			//string ThumbnailPath = m_zfabPath + "/" + m_MaterialThumbnailName;
			//	UTILITY_API->DisplayMessageBox("ThumbnailPath: " + ThumbnailPath);
			string postField = getPublishRequestParameter(m_MaterialThumbnailPath, m_MaterialThumbnailName);
			//	UTILITY_API->DisplayMessageBox("postField:  " + postField);
			string resultJsonString = RESTAPI::PostRestCall(postField, Configuration::GetInstance()->GetPLMServerURL() + RESTAPI::UPLOAD_IMAGE_API, "content-type: multipart/form-data; boundary=----WebKitFormBoundary7MA4YWxkTrZu0gW");
			//	UTILITY_API->DisplayMessageBox("thumbnail uploaded:  " + resultJsonString);
			json detailJson = Helper::GetJsonFromResponse(resultJsonString, "{");
			//	UTILITY_API->DisplayMessageBox(to_string(detailJson));

			string imageId = Helper::GetJSONValue<string>(detailJson, ATTRIBUTE_ID, true);
			string data = "{\n\"images\":{\n\"\":\"" + imageId + "\",\n\"1\":\"" + imageId + "\"\n},\n\"default_3d_material\":\"" + _fabricFileId + "\"\n }";
			//string data = "{\n\"images\":{\n\"\":\"" + imageId + "\",\n\"1\":\"" + imageId + "\"\n}\n }";
			//	UTILITY_API->DisplayMessageBox(data);
			resultJsonString = RESTAPI::PutRestCall(data, Configuration::GetInstance()->GetPLMServerURL() + RESTAPI::CREATE_MATERIAL_API + "/" + _productId, "content-type: application/json");
			Logger::RestAPIDebug("resultJsonString::" + resultJsonString);
			//	UTILITY_API->DisplayMessageBox("ImageLinked " + resultJsonString);
		}
	}

	void CreateMaterial::ExtractThumbnailFromAttachment()
	{
		Logger::Logger("CreateMaterial -> ExtractThumbnailFromAttachment() -> Start");
		std::string fabricFilePath = m_zfabFilePath.toStdString();
		Logger::Logger("fabricFilePath:: " + fabricFilePath);
		//	UTILITY_API->DisplayMessageBox(fabricFilePath);
		m_MaterialThumbnailPath = "";
		string thumbnailName;
		//	UTILITY_API->DisplayMessageBox("ExtractThumbnailFromAttachment m_attachmentName: " + m_attachmentName);
		const size_t lastDotIndex = m_attachmentName.find(".");
		thumbnailName = m_attachmentName.substr(0, lastDotIndex);
		string iconThumbnail;
		iconThumbnail = thumbnailName + ".png";
		m_MaterialThumbnailName = iconThumbnail;
		//	UTILITY_API->DisplayMessageBox("m_attachmentName: " + iconThumbnail);
			// icon thumbnail
		unsigned int fileSize = 0;
		unsigned char * icoBuffer = UTILITY_API->GetAssetIconInCLOFile(fabricFilePath, fileSize);
		Logger::Logger("CreateMaterial -> ExtractThumbnailFromAttachment() -> 1");
		string filepath;
		filepath = m_zfabPath + "/" + iconThumbnail;
		
		     
		//UTILITY_API->DisplayMessageBox("filepath: " + filepath);
		if (icoBuffer != nullptr && fileSize > 0)
		{
			
			FILE * saveFp = fopen(filepath.c_str(), "wb");
			fwrite((const void *)icoBuffer, 1, fileSize, saveFp);
			fclose(saveFp);
			m_MaterialThumbnailPath = filepath;
		}
		// custom thumbnails +
		unsigned int fileSizel = 0;
		unsigned int fileSize2 = 0;
		unsigned char * bufferl = UTILITY_API->GetThumbnailInCLOFile(fabricFilePath, 0, fileSizel);
		unsigned char * buffer2 = UTILITY_API->GetThumbnailInCLOFile(fabricFilePath, 1, fileSize2);
		iconThumbnail = m_MaterialThumbnailName + "1.png";
		filepath = m_zfabPath + "/" + iconThumbnail;
		//	UTILITY_API->DisplayMessageBox("filepath: " + filepath);

		if (bufferl != nullptr && fileSizel > 0) 
		{
			FILE * saveFp = fopen(filepath.c_str(), "wb");
			fwrite((const void *)bufferl, 1, fileSizel, saveFp);
			fclose(saveFp);
		}

		if (buffer2 != nullptr && fileSize2 > 0) 
		{
			iconThumbnail = m_MaterialThumbnailName + "2.png";
			filepath = m_zfabPath + "/" + iconThumbnail;		
			//	UTILITY_API->DisplayMessageBox("filepath: " + filepath);
			FILE * saveFp = fopen(filepath.c_str(), "wb");
			fwrite((const void *)buffer2, 1, fileSize2, saveFp);
			fclose(saveFp);
			if (m_zfabFilePath.contains(".zfab"))
				m_MaterialThumbnailPath = filepath;
		}
		Logger::Logger("CreateMaterial -> ExtractThumbnailFromAttachment() -> End");
	}
	string CreateMaterial::uploadDocument(string _productId)
	{
		string latestRevisionId;
		try
		{
			vector<pair<string, string>> headerNameAndValueList;
			headerNameAndValueList.push_back(make_pair("content-Type", "application/json"));
			headerNameAndValueList.push_back(make_pair("Cookie", Configuration::GetInstance()->GetBearerToken()));

			json dataJson = json::object();
			const size_t lastDotIndex = m_attachmentName.find(".");

			string thumbnailName = m_attachmentName.substr(0, lastDotIndex);
			dataJson["node_name"] = thumbnailName;

			string dataJSONString = to_string(dataJson);

			string resultJsonString = REST_API->CallRESTPost(Configuration::GetInstance()->GetPLMServerURL() + RESTAPI::DOCUMENT_CREATE_API + "/" + _productId, &dataJSONString, headerNameAndValueList, "Loading");
			//UTILITY_API->DisplayMessageBox(resultJsonString);
			if (!FormatHelper::HasContent(resultJsonString))
			{
				throw "Unable to initiliaze Document Configuration. Please try again or Contact your System Administrator.";
			}
			json detailJson = Helper::GetJsonFromResponse(resultJsonString, "{");
			//UTILITY_API->DisplayMessageBox(to_string(detailJson));

			latestRevisionId = Helper::GetJSONValue<string>(detailJson, LATEST_REVISION_KEY, true);
			//UTILITY_API->DisplayMessageBox("Revision id  " + latestRevisionId);
		}
		catch (string msg)
		{
			UTILITY_API->DeleteProgressBar(true);
			Logger::Error("CreateMaterial->uploadDocument() Exception - " + msg);
		}
		catch (exception & e)
		{
			UTILITY_API->DeleteProgressBar(true);
			Logger::Error("CreateMaterial->uploadDocument() Exception - " + string(e.what()));
		}
		catch (const char* msg)
		{
			UTILITY_API->DeleteProgressBar(true);
			wstring wstr(msg, msg + strlen(msg));
			Logger::Error("CreateMaterial->uploadDocument() Exception - " + string(msg));
		}

		return latestRevisionId;
	}

	/*
	* Description - exportZPRJ() method used to export a 3D model and visual images.
	* Parameter -
	* Exception - using throw if anything gets fail.
	* Return -
	*/
	string CreateMaterial::exportFabricFile(string _productId)
	{
		Logger::Debug("Create product exportFabricFile() start....");
		QDir dir;
		string FabricFileId;
		try 
		{
			string postField = getPublishRequestParameter(m_zfabFilePath.toStdString(), m_attachmentName);
			string resultJsonString = RESTAPI::PostRestCall(postField, Configuration::GetInstance()->GetPLMServerURL() + RESTAPI::DOCUMENT_CREATE_API + "/" + _productId + "/upload", "content-type: multipart/form-data; boundary=----WebKitFormBoundary7MA4YWxkTrZu0gW");
			//	UTILITY_API->DisplayMessageBox("Upload fabric completed" + resultJsonString);
			json detailJson = Helper::GetJsonFromResponse(resultJsonString, "{");
			FabricFileId = Helper::GetJSONValue<string>(detailJson, ATTRIBUTE_ID, true);
		}
		catch (string msg)
		{
			Logger::Error("Create product exportFabricFile() Exception - " + msg);
		}
		catch (exception & e)
		{		
			Logger::Error("Create product exportFabricFile() Exception - " + string(e.what()));
		}
		catch (const char* msg)
		{
			wstring wstr(msg, msg + strlen(msg));
			Logger::Error("Create product exportFabricFile() Exception - " + string(msg));
		}
		Logger::Debug("Create product exportFabricFile() end....");
		return FabricFileId;
	}

	void CreateMaterial::reject()
	{
		Logger::Debug("CreateMaterial -> reject -> Start");
		
		this->accept();
		
		Logger::Debug("CreateMaterial -> reject -> End");
	}


	void CreateMaterial::ClearAllFields(QTreeWidget* _documentPushToPLMTree)
	{
		Logger::Debug("CreateMaterial -> ClearAllFields() -> Start");
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
					string attName = qComboBox->property("LabelName").toString().toStdString();
					if (attName == Configuration::GetInstance()->GetSubTypeInternalName())
					{
						qComboBox->clear();
						qComboBox->addItem(QString::fromStdString(BLANK));
						int indexOfEmptyString = qComboBox->findText(QString::fromStdString(BLANK));
						qComboBox->setCurrentIndex(indexOfEmptyString);
					}
					QString defaultValue = qComboBox->property(ATTRIBUTE_INITIAL_VALUE.c_str()).toString();
					if (defaultValue.isEmpty())
						defaultValue = "";
					qComboBox->setCurrentText(defaultValue);
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
					if (defaultValue.isEmpty())
						defaultValue = "";
					qComboBox->setCurrentText(defaultValue);
				}
				else if (qspinBox)
				{
					QString defaultValue = qspinBox->property(ATTRIBUTE_INITIAL_VALUE.c_str()).toString();
					if (!defaultValue.isEmpty())
					{
						qspinBox->setValue(stoi(defaultValue.toStdString()));
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
		}
		catch (string msg)
		{
			UTILITY_API->DeleteProgressBar(true);
			Logger::Error("ERROR::CreateMaterial: ClearAllFields()-> Exception string:: " + msg);
			this->show();
		}
		catch (exception& e)
		{
			UTILITY_API->DeleteProgressBar(true);
			Logger::Error("ERROR::CreateMaterial: ClearAllFields()-> Exception e:: " + string(e.what()));
			this->show();
		}
		catch (const char* msg)
		{
			UTILITY_API->DeleteProgressBar(true);
			Logger::Error("ERROR::CreateMaterial: ClearAllFields()-> Exception char:: " + string(msg));
			wstring wstr(msg, msg + strlen(msg));
			this->show();
		}
		Logger::Debug("CreateMaterial -> ClearAllFields() -> End");
	}

	void CreateMaterial::onSaveAndCloseClicked()
	{
		Logger::Debug("CreateMaterial -> SaveClicked() -> Start");
		CreateMaterialConfig::GetInstance()->SetIsSaveAndCloseClicked(true);
		{
			m_tabWidget->setCurrentIndex(0);	
			this->hide();
			if (UTILITY_API)
				UTILITY_API->DisplayMessageBox(Configuration::GetInstance()->GetLocalizedMaterialClassName() + " metadata saved");
		}
		Logger::Debug("CreateMaterial -> SaveClicked() -> End");
	}

	/*
	* Description - GetTreewidget() method is for pass a treeWidget.
	* Parameter -
	* Exception -
	* Return -QTreeWidget
	*/
	QTreeWidget* CreateMaterial::GetTreewidget(int _index)
	{
		if (_index == 0)
			return m_CreateMaterialTreeWidget_1;
		if (_index == 1)
			return m_CreateMaterialTreeWidget_2;

		return m_CreateMaterialTreeWidget_1;
	}

	/*
	* Description - OnHandleDropDownValue() method handles the event on changing of the values.
	* Parameter - _item
	* Exception -
	* Return - void
	*/
	void CreateMaterial::OnHandleDropDownValue(const QString& _item)
	{
		Logger::Info("CreateMaterial -> OnHandleDropDownValue() -> Start");
		Logger::Debug("CreateMaterial -> OnHandleDropDownValue() -> _item:: "+ _item.toStdString());
		QString senderCombo = sender()->property("LabelName").toString();
		Logger::Debug("CreateMaterial -> OnHandleDropDownValue() -> senderCombo:: " + senderCombo.toStdString());
		try
		{
			QString dependentField = "";
			if (senderCombo == "product_type")
			{
				dependentField = QString::fromStdString(Configuration::GetInstance()->GetSubTypeInternalName());
				if (FormatHelper::HasContent(_item.toStdString()))
				{
					processTreeWidgetForDependency(_item, senderCombo, m_CreateMaterialTreeWidget_1);
					processTreeWidgetForDependency(_item, senderCombo, m_CreateMaterialTreeWidget_2);
				}
				else
				{
					clearDependentFields(dependentField, m_CreateMaterialTreeWidget_1);
					clearDependentFields(dependentField, m_CreateMaterialTreeWidget_2);
				}
			}
		}
		catch (string msg)
		{
			Logger::Error("CreateMaterial -> OnHandleDropDownValue()-> Exception string:: " + msg);
			UTILITY_API->DisplayMessageBox(msg);
			this->show();
		}
		catch (exception& e)
		{
			Logger::Error("CreateMaterial -> OnHandleDropDownValue()-> Exception e:: " + string(e.what()));
			UTILITY_API->DisplayMessageBox(e.what());
			this->show();
		}
		catch (const char* msg)
		{
			Logger::Error("CreateMaterial -> OnHandleDropDownValue()-> Exception char:: " + string(msg));
			wstring wstr(msg, msg + strlen(msg));
			UTILITY_API->DisplayMessageBoxW(wstr);
			this->show();
		}
		Logger::Info("CreateMaterial -> OnHandleDropDownValue() -> End");
	}

	/*
	* Description - connectOnHandleDropDownWidget() method connects to the onHandleDropDownWidget.
	* Parameter - _documentPushToPLMTree
	* Exception -
	* Return - void
	*/
	void CreateMaterial::connectOnHandleDropDownWidget(QTreeWidget* _documentPushToPLMTree)
	{
		Logger::Info("CreateMaterial::ConnectOnHandleDropDownWidget() -> Start");

		for (int i = 0; i < _documentPushToPLMTree->topLevelItemCount(); i++)
		{
			QTreeWidgetItem* topItem = _documentPushToPLMTree->topLevelItem(i);
			QWidget* qWidgetColumn0 = _documentPushToPLMTree->itemWidget(topItem, 0);
			QWidget* qWidgetColumn_1 = _documentPushToPLMTree->itemWidget(topItem, 1);
			if (!qWidgetColumn0 || !qWidgetColumn_1)
			{
				continue;
			}

			QComboBox* qComboBox = qobject_cast<QComboBox*>(qWidgetColumn_1);
			if (qComboBox)
			{
				string attName = qComboBox->property("LabelName").toString().toStdString();
				if (attName == "product_type")
				{
					QObject::connect(qComboBox, SIGNAL(activated(const QString&)), this, SLOT(OnHandleDropDownValue(const QString&)));
				}
			}
		}
		Logger::Info("CreateMaterial::ConnectOnHandleDropDownWidget() -> End");
	}

	/*
	* Description - processTreeWidgetForDependency() method processes the onHandleDropDownWidget items to handle dependencies.
	* Parameter - _item, _senderCombo, _documentPushToPLMTree
	* Exception -
	* Return - void
	*/
	void CreateMaterial::processTreeWidgetForDependency(QString _item, QString _senderCombo, QTreeWidget* _documentPushToPLMTree)
	{
		string progressbarText;
		string attName, attId;
		QStringList dependentFields;
		if (_senderCombo == "product_type")
		{
			dependentFields.append(QString::fromStdString(Configuration::GetInstance()->GetSubTypeInternalName()));
		}
		for (int itemIndex = 0; itemIndex < _documentPushToPLMTree->topLevelItemCount(); ++itemIndex)
		{
			QTreeWidgetItem* topItem = _documentPushToPLMTree->topLevelItem(itemIndex);
			QWidget* qWidgetColumn_0 = _documentPushToPLMTree->itemWidget(topItem, 0);
			QWidget* qWidgetColumn_1 = _documentPushToPLMTree->itemWidget(topItem, 1);
			if (!qWidgetColumn_0 || !qWidgetColumn_1)
			{
				continue;
			}
			QLabel* qlabel = qobject_cast<QLabel*>(qWidgetColumn_0);
			QString lableText = qlabel->property(ATTRIBUTE_KEY.c_str()).toString();
			Logger::Logger("lableText============= "+ lableText.toStdString());
			if (dependentFields.contains(lableText))
			{
				QStringList valueList;
				valueList.append(QString::fromStdString(BLANK));
				json dependentFieldJson = json::object();

				ComboBoxItem* qComboBox = qobject_cast<ComboBoxItem*>(qWidgetColumn_1);
				QLabel* qlabel = qobject_cast<QLabel*>(qWidgetColumn_0);

				if (qComboBox)
				{
					Logger::Logger("CATEGORY_INT_NAME============= " + Configuration::GetInstance()->GetSubTypeInternalName());
					if (lableText.toStdString() == Configuration::GetInstance()->GetSubTypeInternalName())
					{
						progressbarText = "Loading Category details..";
						string matTypeId = sender()->property(_item.toStdString().c_str()).toString().toStdString();
						string categoryValueId = MaterialConfig::GetInstance()->GetMaterialCategoryEnumId();
						dependentFieldJson = Helper::makeRestcallGet(RESTAPI::SEARCH_ENUM_ATT_API + "/" + categoryValueId + "/values?depends_on_details=true&skip=0&limit=1000", "","", progressbarText);
						Logger::Logger("dependentFieldJson============= " + to_string(dependentFieldJson));
					}
					json attJson = json::object();
					for (int i = 0; i < dependentFieldJson.size(); i++)
					{
						attJson = Helper::GetJSONParsedValue<int>(dependentFieldJson, i, false);;///use new method
						json dependsOnValue = Helper::GetJSONParsedValue<string>(attJson, "depends_on", false);
						Logger::Logger("dependsOnValue============= " + to_string(dependsOnValue));

						if (!dependsOnValue.size())
							continue;

						for (auto it = dependsOnValue.begin(); it != dependsOnValue.end(); ++it)
						{
							json valueJson = it.value();
							string parentName = Helper::GetJSONValue<string>(valueJson, "display_name", true);
							if (parentName == _item.toStdString())
							{
								attName = Helper::GetJSONValue<string>(attJson, ATTRIBUTE_DISPLAY_NAME, true);
								Logger::Debug("PublishToPLMData -> OnHandleDropDownValue() attName: " + attName);
								attId = Helper::GetJSONValue<string>(attJson, ATTRIBUTE_ID, true);
								Logger::Debug("PublishToPLMData -> OnHandleDropDownValue() attId: " + attId);
								string internalName = Helper::GetJSONValue<string>(attJson, "node_name", true);
								Logger::Debug("PublishToPLMData -> OnHandleDropDownValue() internalName: " + internalName);
								valueList.append(QString::fromStdString(attName));
								qComboBox->setProperty(attName.c_str(), QString::fromStdString(internalName));
							}
						}
					}
					if (valueList.size() > 1)
					{
						qComboBox->clear();
						valueList.sort();
						qComboBox->addItems(valueList);
						qComboBox->setDisabled(false);
						qlabel->setStyleSheet(FONT_STYLE);

						if (qComboBox->isEnabled())
						{
							qComboBox->fillItemListAndDefaultValue(valueList, qComboBox->currentText());
							QCompleter* m_nameCompleter = new QCompleter(valueList, qComboBox); // using QCompleter class
							m_nameCompleter->setCompletionMode(QCompleter::PopupCompletion);
							m_nameCompleter->setCaseSensitivity(Qt::CaseInsensitive);
							qComboBox->setCompleter(m_nameCompleter);
						}
					}
					else
					{
						qComboBox->clear();
						qComboBox->setDisabled(true);
						qlabel->setStyleSheet(DISABLED);
					}
					valueList.clear();
				}
			}
		}
	}

	/*
	* Description - clearDependentFields() method clears the dependent fields.
	* Parameter - _lable, _documentPushToPLMTree
	* Exception -
	* Return - void
	*/
	void CreateMaterial::clearDependentFields(QString _lable, QTreeWidget* _documentPushToPLMTree)
	{
		Logger::Info("CreateMaterial:: clearDependentFields() -> Start");
		Logger::Debug("CreateMaterial:: clearDependentFields() -> _lable:: " + _lable.toStdString());
		for (int itemIndex = 0; itemIndex < _documentPushToPLMTree->topLevelItemCount(); ++itemIndex)
		{
			QTreeWidgetItem* topItem = _documentPushToPLMTree->topLevelItem(itemIndex);
			QWidget* qWidgetColumn_0 = _documentPushToPLMTree->itemWidget(topItem, 0);
			QWidget* qWidgetColumn_1 = _documentPushToPLMTree->itemWidget(topItem, 1);
			QString lableText;
			if (!qWidgetColumn_0 || !qWidgetColumn_1)
			{
				continue;
			}

			ComboBoxItem* qComboBox = qobject_cast<ComboBoxItem*>(qWidgetColumn_1);
			if (qComboBox)
			{
				string attName = qComboBox->property("LabelName").toString().toStdString();
				if (attName == _lable.toStdString())
				{
					qComboBox->clear();
					qComboBox->addItem(QString::fromStdString(BLANK));
					int indexOfEmptyString = qComboBox->findText(QString::fromStdString(BLANK));
					qComboBox->setCurrentIndex(indexOfEmptyString);
				}
			}
		}
		Logger::Info("CreateMaterial:: clearDependentFields() -> End");
	}
	
	/*
	* Description - DrawCriteriaWidget() method is create/reset the create widget.
	* Parameter - bool
	* Exception -
	* Return -
	*/
	void CreateMaterial::DrawCriteriaWidget(bool _isFromConstructor)
	{
		Logger::Info("CreateMaterial:: DrawCriteriaWidget() -> Start");
		if (_isFromConstructor)
		{
			m_CreateMaterialTreeWidget_1->clear();
			m_CreateMaterialTreeWidget_2->clear();
			addCreateMaterialDetailsWidgetData();
		}

		if (!CreateMaterialConfig::GetInstance()->GetIsSaveAndCloseClicked())
		{
			m_attachmentsList->clear();
			ClearAllFields(m_CreateMaterialTreeWidget_1);
			ClearAllFields(m_CreateMaterialTreeWidget_2);
			m_tabWidget->setCurrentIndex(0);
			m_zfabFilePath.clear();
		}
		Logger::Info("CreateMaterial:: DrawCriteriaWidget() -> End");
	}
}
