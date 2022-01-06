/*
* Copyright 2020-2021 CLO-Vise. All rights reserved
*
* @file UpdateMaterial.cpp
*
* @brief Class implementation for Update Material and Document from CLO to PLM.
* This class has all the variable and methods implementation which are used to PLM Update Material instance data update and Publish from CLO to PLM.
*
* @author GoVise
*
* @date 14-AUG-2020
*/
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
#include "UpdateMaterial.h"
#include "CLOVise/PLM/Helper/Util/Helper.h"
#include "CLOVise/PLM/Helper/Util/Configuration.h"
#include "CLOVise/PLM/Inbound/Material/MaterialConfig.h"
#include "CLOVise/PLM/Helper/Util/FlexTypeHelper.h"
#include "CLOVise/PLM/Helper/UIHelper/UIHelper.h"
#include "CLOVise/PLM/Helper/Util/Logger.h"
#include "CLOVise/PLM/CLOViseSuite/CLOViseSuite.h"
#include "CLOVise/PLM/Inbound/Material/PLMMaterialSearch.h"

using namespace std;

namespace CLOVise
{
	UpdateMaterial* UpdateMaterial::_instance = NULL;

	UpdateMaterial* UpdateMaterial::GetInstance()
	{
		if (_instance == NULL) {
			_instance = new UpdateMaterial();
		}

		return _instance;
	}

	UpdateMaterial::UpdateMaterial(QWidget* parent) : MVDialog(parent)
	{
		setupUi(this);
		Logger::Debug("UpdateMaterial -> constructor() start....");

		QString windowTitle = PLM_NAME + " PLM Update "+ QString::fromStdString(Configuration::GetInstance()->GetLocalizedMaterialClassName());
		this->setWindowTitle(windowTitle);

#ifdef __APPLE__
		this->setWindowFlags(Qt::Dialog | Qt::CustomizeWindowHint | Qt::WindowMinMaxButtonsHint | Qt::WindowCloseButtonHint);
#else
		this->setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint | Qt::CustomizeWindowHint);
		m_pTitleBar = new MVTitleBar(windowTitle, this);
		layout()->setMenuBar(m_pTitleBar);
#endif // !__APPLE__
		m_UpdateMaterialTreeWidget_1 = nullptr;
		m_UpdateMaterialTreeWidget_2 = nullptr;
		m_cancelButton = nullptr;
		m_publishButton = nullptr;
		m_addAttachmentButton = nullptr;
		m_tabWidget = nullptr;
		m_overviewTab = nullptr;
		m_attachmentTab = nullptr;
		m_attachmentNameLabel = nullptr;
		m_SaveAndCloseButton = nullptr;

		m_UpdateMaterialTreeWidget_1 = CVWidgetGenerator::CreatePublishTreeWidget("QTreeWidget { background-color: #262628; border: 1px solid #000;}""QTreeWidget::item { padding :5px; height: 20px; color: #FFFFFF; font-face: ArialMT; font-size: 10px; width: 90px; margin-left: 5px; margin-right: 5px; margin-top: 5px; margin-bottom: 5px; border: none;}""QTreeWidget::item:hover{background-color: #262628;}", true);
		m_UpdateMaterialTreeWidget_1->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
		m_UpdateMaterialTreeWidget_2 = CVWidgetGenerator::CreatePublishTreeWidget("QTreeWidget { background-color: #262628; border: 1px solid #000;}""QTreeWidget::item { padding :5px; height: 20px; color: #FFFFFF; font-face: ArialMT; font-size: 10px; width: 90px; margin-left: 5px; margin-right: 5px; margin-top: 5px; margin-bottom: 5px; border: none;}""QTreeWidget::item:hover{background-color: #262628;}", true);
		m_UpdateMaterialTreeWidget_2->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

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

		QVBoxLayout *treeWidgetLayout = new QVBoxLayout;
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
		splitter->insertWidget(0, m_UpdateMaterialTreeWidget_1);
		splitter->insertWidget(1, m_UpdateMaterialTreeWidget_2);
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

		connectSignalSlots(true);
		UpdateMaterialWidget(true);

		Logger::Debug("UpdateMaterial -> constructor() end....");
		RESTAPI::SetProgressBarData(0, "", false);
	}

	UpdateMaterial::~UpdateMaterial()
	{
		Logger::Info("UpdateMaterial -> Destructor() -> Start");
		DeletePointer(m_UpdateMaterialTreeWidget_1);
		DeletePointer(m_UpdateMaterialTreeWidget_2);
		DeletePointer(m_cancelButton);
		DeletePointer(m_publishButton);
		DeletePointer(m_addAttachmentButton);
		DeletePointer(m_tabWidget);
		DeletePointer(m_overviewTab);
		DeletePointer(m_attachmentTab);
		DeletePointer(m_attachmentNameLabel);
		Logger::Info("UpdateMaterial -> Destructor() -> Start");
	}

	/*
	* Description - connectSignalSlots() method used to connect and disconnect signal and slots.
	* Parameter -  bool.
	* Exception -
	* Return -
	*/
	void UpdateMaterial::connectSignalSlots(bool _b)
	{
		if (_b)
		{
			QObject::connect(m_publishButton, SIGNAL(clicked()), this, SLOT(onPublishToPLMClicked()));
			QObject::connect(m_cancelButton, SIGNAL(clicked()), this, SLOT(onBackButtonClicked()));
			QObject::connect(m_addAttachmentButton, SIGNAL(clicked()), this, SLOT(onAddAttachmentClicked()));
			QObject::connect(m_SaveAndCloseButton, SIGNAL(clicked()), this, SLOT(onSaveAndCloseClicked()));
		}
		else
		{
			QObject::disconnect(m_publishButton, SIGNAL(clicked()), this, SLOT(onPublishToPLMClicked()));
			QObject::disconnect(m_cancelButton, SIGNAL(clicked()), this, SLOT(onBackButtonClicked()));
			QObject::disconnect(m_addAttachmentButton, SIGNAL(clicked()), this, SLOT(onAddAttachmentClicked()));
			QObject::disconnect(m_SaveAndCloseButton, SIGNAL(clicked()), this, SLOT(onSaveAndCloseClicked()));
		}
	}

	/*
	* Description - cancelWindowClicked() method is a slot for cancel button click and close the publish to plm.
	* Parameter -
	* Exception -
	* Return -
	*/
	void UpdateMaterial::onBackButtonClicked()
	{
		Logger::Info("UpdateMaterial -> onBackButtonClicked() -> Start");
		backButtonClicked();
		Logger::Info("UpdateMaterial -> onBackButtonClicked() -> End");
	}

	void UpdateMaterial::backButtonClicked()
	{

		Logger::Info("UpdateMaterial -> backButtonClicked() -> Start");
		this->hide();
		CLOVise::CLOViseSuite::GetInstance()->setModal(true);
		CLOViseSuite::GetInstance()->show();
		Logger::Info("UpdateMaterial -> backButtonClicked() -> End");
	}
	/*
	* Description - addUpdateMaterialDetailsWidgetData() method adds the updated material detaials the widget.
	* Parameter -
	* Exception -
	* Return -
	*/
	void UpdateMaterial::addUpdateMaterialDetailsWidgetData()
	{
		Logger::Info("UpdateMaterial -> add3DModelDetailsWidgetData() start....");
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
				drawCriteriaWidget(attributesJson, m_UpdateMaterialTreeWidget_1, m_UpdateMaterialTreeWidget_2, PublishToPLMData::GetInstance()->GetActive3DModelMetaData());
				break;
			}

			m_UpdateMaterialTreeWidget_1->setColumnCount(2);
			m_UpdateMaterialTreeWidget_1->setHeaderHidden(true);
			m_UpdateMaterialTreeWidget_1->setWordWrap(true);
			m_UpdateMaterialTreeWidget_1->setDropIndicatorShown(false);
			m_UpdateMaterialTreeWidget_1->setRootIsDecorated(false);
			m_UpdateMaterialTreeWidget_1->setSelectionMode(QAbstractItemView::NoSelection);
			m_UpdateMaterialTreeWidget_1->setStyleSheet("QTreeWidget { background-color: #262628; border: 1px solid #000; padding-left: 20px; min-width: 400px; outline: 0;}""QTreeWidget::item {height: 20px; width: 200px; margin-right: 20px; margin-top: 5px; margin-bottom: 5px; border: none; }""QTreeWidget::item:hover{ background-color: #262628; }""QTreeView{outline: 0;}");
			m_UpdateMaterialTreeWidget_2->setColumnCount(2);
			m_UpdateMaterialTreeWidget_2->setHeaderHidden(true);
			m_UpdateMaterialTreeWidget_2->setWordWrap(true);
			m_UpdateMaterialTreeWidget_2->setDropIndicatorShown(false);
			m_UpdateMaterialTreeWidget_2->setRootIsDecorated(false);
			m_UpdateMaterialTreeWidget_2->setSelectionMode(QAbstractItemView::NoSelection);
			m_UpdateMaterialTreeWidget_2->setStyleSheet("QTreeWidget { background-color: #262628; border: 1px solid #000; padding-left: 20px; min-width: 400px; outline: 0;}""QTreeWidget::item {height: 20px; width: 200px; margin-right: 20px; margin-top: 5px; margin-bottom: 5px; border: none; }""QTreeWidget::item:hover{ background-color: #262628; }""QTreeView{outline: 0;}");
		}
		catch (string msg)
		{
			Logger::Error("UpdateMaterial -> add3DModelDetailsWidgetData() Exception - " + msg);
		}
		catch (exception & e)
		{
			Logger::Error("UpdateMaterial -> add3DModelDetailsWidgetData() Exception - " + string(e.what()));
		}
		catch (const char* msg)
		{
			wstring wstr(msg, msg + strlen(msg));
			Logger::Error("UpdateMaterial -> add3DModelDetailsWidgetData() Exception - " + string(msg));
		}
		Logger::Info("UpdateMaterial -> add3DModelDetailsWidgetData() end....");
	}

	/*
	* Description - DrawCriteriaWidget() method used to create fields in UI.
	* Parameter -  json, QTreeWidget, json.
	* Exception -
	* Return -
	*/
	void UpdateMaterial::drawCriteriaWidget(json _attributesJsonArray, QTreeWidget* _documentPushToPLMTree_1, QTreeWidget* _documentPushToPLMTree_2, json _metadataJson)
	{
		Logger::Info("UpdateMaterial -> drawCriteriaWidget() start....");

		if (_attributesJsonArray.size() < 10)
		{
			drawWidget(_attributesJsonArray, _documentPushToPLMTree_1, STARTING_INDEX, _attributesJsonArray.size());
			_documentPushToPLMTree_2->hide();
		}
		else
		{

			drawWidget(_attributesJsonArray, _documentPushToPLMTree_1, STARTING_INDEX, ceil(_attributesJsonArray.size() / DIVISION_FACTOR));
			drawWidget(_attributesJsonArray, _documentPushToPLMTree_2, ceil(_attributesJsonArray.size() / DIVISION_FACTOR), _attributesJsonArray.size());
		}

		//	drawWidget(attJson, _documentPushToPLMTree, attValue, attKey, isAttSettable);
		//}

		Logger::Info("UpdateMaterial -> drawCriteriaWidget() end....");
	}

	/*
	* Description - DrawCriteriaWidget() method used to create input fields.
	* Parameter -  json, QTreeWidget, string, string, bool.
	* Exception -
	* Return -
	*/
	void UpdateMaterial::drawWidget(json _attributesJsonArray, QTreeWidget* _treeWidget, int _start, int _end)
	{
		json attJson = json::object();
		QStringList attScopes;
		string attScope = "";
		string isAttEnable = "";
		//string isAttUpdatable = "";
		string attKey = "";
		string attValue = "";
		try
		{
			for (int i = _start; i < _end; i++)
			{
				//bool isAttSettable = false;

				attJson = Helper::GetJSONParsedValue<int>(_attributesJsonArray, i, false);
				bool isAttSettable = false;
				bool isAttUpdatable = false;

				attScope = Helper::GetJSONValue<string>(attJson, ATTRIBUTE_ATTSCOPE_KEY, true);
				Logger::Debug("UIHelper::CreateWidgetsOnOutBoundTreeWidget() - attScope:: " + attScope);

				string attType = Helper::GetJSONValue<string>(attJson, ATTRIBUTE_TYPE_KEY, true);
				Logger::Debug("UIHelper::CreateWidgetsOnOutBoundTreeWidget() - attType:: " + attType);
				/*if (!Configuration::GetInstance()->GetModifySupportedAttsList().contains(QString::fromStdString(attType)))
				{
					Logger::Error("CVWidgetGenerator::CreateSearchCriteriaWidget() - attributeType:: not supported");
					continue;
				}*/

				isAttEnable = Helper::IsValueTrue(Helper::GetJSONValue<string>(attJson, ATTRIBUTE_ENABLED_KEY, true));
				Logger::Debug("UIHelper::CreateWidgetsOnOutBoundTreeWidget() - isAttEnable:: " + isAttEnable);
				isAttUpdatable = Helper::IsValueTrue(Helper::GetJSONValue<string>(attJson, ATTRIBUTE_UPDATABLE_KEY, true));
				Logger::Debug("UIHelper::CreateWidgetsOnOutBoundTreeWidget() - isAttUpdatable:: " + to_string(isAttUpdatable));
				if ("false" == isAttEnable)
				{
					continue;
				}
				attKey = Helper::GetJSONValue<string>(attJson, ATTRIBUTE_KEY, true);
				Logger::Debug("UIHelper::CreateWidgetsOnOutBoundTreeWidget() - attKey:: " + attKey);
				attValue = Helper::GetJSONValue<string>(attJson, ATTRIBUTE_DEFAULT_VALUE_KEY, true);
				Logger::Debug("UIHelper::CreateWidgetsOnOutBoundTreeWidget() - attValue:: " + attValue);
				string attSettable = Helper::GetJSONValue<string>(attJson, ATTRIBUTE_SETTABLE_KEY, true);
				Logger::Debug("UIHelper::CreateWidgetsOnOutBoundTreeWidget() - attSettable:: " + attSettable);
				string attRestApiExposed = Helper::GetJSONValue<string>(attJson, ATTRIBUTE_REST_API_EXPOSED, true);
				Logger::Debug("UIHelper::CreateWidgetsOnOutBoundTreeWidget() - attRestApiExposed:: " + attRestApiExposed);


				if (attRestApiExposed == "false")
					continue;
				if (attValue.find("ref(centric:)") != -1)
				{
					attValue = "";
				}

				if (attSettable == "false")// || !isAttUpdatable)
					isAttSettable = true;

				if (isAttUpdatable)
					isAttSettable = false;

				string attValue = (Helper::GetJSONValue<string>(m_downloadedMaterialJson, attKey, true));
				if (!FormatHelper::HasContent(attValue)) {
					attValue = "";
				}
				string attName = Helper::GetJSONValue<string>(attJson, ATTRIBUTE_NAME_KEY, true);
				Logger::Debug("UIHelper::CreateWidgetsOnOutBoundTreeWidget() - attName:: " + attName);
				//string attType = Helper::GetJSONValue<string>(attJson, ATTRIBUTE_TYPE_KEY, true);
				bool attRequired = Helper::IsValueTrue((Helper::GetJSONValue<string>(attJson, ATTRIBUTE_REQUIRED_KEY, true)));
				Logger::Debug("UIHelper::CreateWidgetsOnOutBoundTreeWidget() - attRequired:: " + to_string(attRequired));

				json enumJson = json::object();
				json attEnumsJson = json::array();
				string attEnumVale = BLANK;
				string attEnumKey = BLANK;
				if (attType == MOA_LIST_ATT_TYPE_KEY)
				{
					QTreeWidgetItem* topLevel = new QTreeWidgetItem();
					QListWidget* listWidget = new QListWidget();

					attEnumsJson = Helper::GetJSONParsedValue<string>(attJson, ENUMLIST_JSON_KEY, false);
					for (int i = 0; i < attEnumsJson.size(); i++)
					{
						enumJson = Helper::GetJSONParsedValue<int>(attEnumsJson, i, false);
						attEnumVale = Helper::GetJSONValue<string>(enumJson, ENUM_VALUE_KEY, true);
						attEnumKey = Helper::GetJSONValue<string>(enumJson, ENUM_KEY, true);
						CVWidgetGenerator::CreateCVMultiListWidget(listWidget, QString::fromStdString(attEnumVale), QString::fromStdString(attEnumKey), QString::fromStdString(attValue));
					}

					listWidget->setFocusPolicy(Qt::NoFocus);
					listWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
					listWidget->setProperty(ATTRIBUTE_INITIAL_VALUE.c_str(), QString::fromStdString(attValue));

					listWidget->setStyleSheet("QListWidget{border-bottom: 1px inset #262626; font-size: 10px; font-face: ArialMT; border-left: 1px inset #0D0D0D; max-height:70px; border-top: 1px inset #0D0D0D; border-right: 1px inset #262626; background-color: #222224; } QListWidget::item{ background-color: #222224; margin: 2px;} QListWidget::indicator { width: 10px; height:10px; } QListWidget::indicator:unchecked {background-color: #343434 ; border: 1px solid #3B3B3B;} QListWidget::indicator:checked {image: url(:/CLOVise/PLM/Images/icon_ok_over.svg);background-color: #343434; border: 1px solid #3B3B3B;}");

					_treeWidget->addTopLevelItem(topLevel);
					_treeWidget->setItemWidget(topLevel, 0, CVWidgetGenerator::CreateLabelWidget(attName, attKey, attType, attRequired, !isAttSettable));	// Adding label at column 1
					if (!isAttSettable)
					{
						listWidget->setDisabled(true);
						listWidget->setStyleSheet("QListWidget{ color: #808080; border-bottom: 1px inset #262626; font-size: 10px; font-face: ArialMT; border-left: 1px inset #0D0D0D; max-height:70px; border-top: 1px inset #0D0D0D; border-right: 1px inset #262626; background-color: #222224; } QListWidget::item{ color: #808080; background-color: #222224; margin: 2px;} QListWidget::indicator { width: 10px; height:10px; } QListWidget::indicator:unchecked {background-color: #343434 ; border: 1px solid #3B3B3B;} QListWidget::indicator:checked {image: url(:/CLOVise/PLM/Images/icon_ok_over.svg);background-color: #343434; border: 1px solid #3B3B3B;}");
					}
					_treeWidget->setItemWidget(topLevel, 1, listWidget);

				}
				else if (attType == TEXT_AREA_ATT_TYPE_KEY)
				{
					QTreeWidgetItem* topLevel = new QTreeWidgetItem();  // Creating new TreeWidgetItem		

					_treeWidget->addTopLevelItem(topLevel);				// Adding ToplevelItem
					_treeWidget->setItemWidget(topLevel, 0, CVWidgetGenerator::CreateLabelWidget(attName, attKey, attType, attRequired, !isAttSettable));	// Adding label at column 1
					_treeWidget->setItemWidget(topLevel, 1, CVWidgetGenerator::CreateTextAreaWidget(attValue, isAttSettable));

				}
				else if (attType == DATE_ATT_TYPE_KEY || attType == TIME_ATT_TYPE_KEY)
				{
					PublishToPLMData::GetInstance()->SetDateFlag(true);
					QStringList dateList;
					QTreeWidgetItem* topLevel = new QTreeWidgetItem();  // Creating new TreeWidgetItem
					_treeWidget->addTopLevelItem(topLevel);			// Adding ToplevelItem
					_treeWidget->setItemWidget(topLevel, 0, CVWidgetGenerator::CreateLabelWidget(attName, attKey, attType, attRequired, !isAttSettable));	// Adding label at column 1
					_treeWidget->setItemWidget(topLevel, 1, CVWidgetGenerator::CreateDateWidget(attValue, isAttSettable));

				}
				else if (attType == INTEGER_ATT_TYPE_KEY || attType == CONSTANT_ATT_TYPE_KEY)
				{
					QTreeWidgetItem* topLevel = new QTreeWidgetItem();  // Creating new TreeWidgetItem
					QSpinBox* spinBixWidget = new QSpinBox();
					spinBixWidget->setMaximumHeight(SPINBOX_HEIGHT);
					spinBixWidget->setStyleSheet(SPINBOX_STYLE);
					_treeWidget->addTopLevelItem(topLevel);						// Adding ToplevelItem
					_treeWidget->setItemWidget(topLevel, 0, CVWidgetGenerator::CreateLabelWidget(attName, attKey, attType, attRequired, !isAttSettable));	// Adding label at column 1
					spinBixWidget = CVWidgetGenerator::CreateSpinBoxWidget(QString::fromStdString(attValue), BLANK);
					_treeWidget->setItemWidget(topLevel, 1, spinBixWidget);
					if (!isAttSettable)
					{
						// for desable 
						spinBixWidget->setDisabled(true);
						spinBixWidget->setStyleSheet(DISABLED_SPINBOX_STYLE);
					}

				}
				else if (attType == FLOAT_ATT_TYPE_KEY || attType == DOUBLE_ATT_TYPE_KEY)
				{
					QTreeWidgetItem* topLevel = new QTreeWidgetItem();  // Creating new TreeWidgetItem
					QDoubleSpinBox* doubleSpinBoxWidget = new QDoubleSpinBox();
					doubleSpinBoxWidget->setMaximumHeight(SPINBOX_HEIGHT);
					string attDecimals = Helper::GetJSONValue<string>(attJson, DECIMAP_FIGURES, true);
					_treeWidget->addTopLevelItem(topLevel);						// Adding ToplevelItem
					_treeWidget->setItemWidget(topLevel, 0, CVWidgetGenerator::CreateLabelWidget(attName, attKey, attType, attRequired, !isAttSettable));	// Adding label at column 1
					doubleSpinBoxWidget = CVWidgetGenerator::CreateDoubleSpinBoxWidget(QString::fromStdString(attValue), BLANK, "0", 0.0000, 10000000000000.0000);
					_treeWidget->setItemWidget(topLevel, 1, doubleSpinBoxWidget);
					if (!isAttSettable)
					{
						// for desable 
						doubleSpinBoxWidget->setDisabled(true);
						doubleSpinBoxWidget->setStyleSheet(DISABLED_DOUBLE_SPINBOX_STYLE);
					}

				}
				else if (attType == CURRENCY_ATT_TYPE_KEY)
				{
					QTreeWidgetItem* topLevel = new QTreeWidgetItem();  // Creating new TreeWidgetItem
					QDoubleSpinBox* doubleSpinBoxWidget = new QDoubleSpinBox();

					string attDecimals = Helper::GetJSONValue<string>(attJson, DECIMAP_FIGURES, true);
					string attCurrencyCode = Helper::GetJSONValue<string>(attJson, CURRENCY_CODE, true) + " ";
					_treeWidget->addTopLevelItem(topLevel);						// Adding ToplevelItem
					_treeWidget->setItemWidget(topLevel, 0, CVWidgetGenerator::CreateLabelWidget(attName, attKey, attType, attRequired, !isAttSettable));	// Adding label at column 1
					doubleSpinBoxWidget = CVWidgetGenerator::CreateDoubleSpinBoxWidget(QString::fromStdString(attValue), BLANK + attCurrencyCode, attDecimals, 0.00, 10000000000000.00);
					_treeWidget->setItemWidget(topLevel, 1, doubleSpinBoxWidget);
					if (!isAttSettable)
					{
						// for desable 
						doubleSpinBoxWidget->setDisabled(true);
						doubleSpinBoxWidget->setStyleSheet(DISABLED_DOUBLE_SPINBOX_STYLE);
					}

				}
				else if (attType == BOOLEAN_ATT_TYPE_KEY)
				{
					QTreeWidgetItem* topLevel = new QTreeWidgetItem();  // Creating new TreeWidgetItem

					_treeWidget->addTopLevelItem(topLevel);						// Adding ToplevelItem
					_treeWidget->setItemWidget(topLevel, 0, CVWidgetGenerator::CreateLabelWidget(attName, attKey, attType, attRequired, !isAttSettable));	// Adding label at column 1
					_treeWidget->setItemWidget(topLevel, 1, CVWidgetGenerator::CreateBooleanWidget(attValue, isAttSettable, topLevel, 1));	// Adding label at column 2

				}
				else if (attType == USER_LIST_ATT_TYPE_KEY || attType == CHOICE_ATT_TYPE_KEY || attType == REF_ATT_TYPE_KEY || attType == ENUM_ATT_TYPE_KEY)
				{
					QTreeWidgetItem* topLevel = new QTreeWidgetItem();  // Creating new TreeWidgetItem
					if (attType == REF_ATT_TYPE_KEY || attType == ENUM_ATT_TYPE_KEY)
					{
						attEnumsJson = Helper::GetJSONParsedValue<string>(attJson, ENUMLIST_JSON_KEY, false);
					}
					else
					{
						attEnumsJson = Helper::GetJSONParsedValue<string>(attJson, USER_LIST_ATT_TYPE_KEY, false);
					}
					QStringList valueList;
					Logger::Debug("UIHelper::CreateWidgetsOnOutBoundTreeWidget() - ComboBoxItem:: 1");
					ComboBoxItem* comboBox = new ComboBoxItem();

					comboBox->setFocusPolicy(Qt::StrongFocus);
					comboBox->setStyleSheet(COMBOBOX_STYLE);
					comboBox->addItem(QString::fromStdString(BLANK));
					comboBox->setProperty(ATTRIBUTE_INITIAL_VALUE.c_str(), QString::fromStdString(attValue));

#ifdef __APPLE__	// Settinng Windows OS style to QComboBox on MAC OS

					comboBox->setStyle(QStyleFactory::create("Windows"));
					comboBox->setItemDelegate(new QStyledItemDelegate());

#endif

					for (int i = 0; i < attEnumsJson.size(); i++)
					{
						attJson = Helper::GetJSONParsedValue<int>(attEnumsJson, i, false);///use new method
						attEnumVale = Helper::GetJSONValue<string>(attJson, ENUM_VALUE_KEY, true);
						attEnumKey = Helper::GetJSONValue<string>(attJson, ENUM_KEY, true);
						if (attValue == attEnumKey)
						{
							attValue = attEnumVale;
						}
						valueList.push_back(QString::fromStdString(attEnumVale));
						comboBox->setProperty(attEnumVale.c_str(), QString::fromStdString(attEnumKey));
					}
					valueList.sort();
					comboBox->addItems(valueList);

					int index = comboBox->findText(QString::fromStdString(attValue));
					if (index != -1) { // -1 for not found
						comboBox->setCurrentIndex(index);
					}
					if (!isAttSettable)
					{
						comboBox->setDisabled(true);
						comboBox->setStyleSheet(DISABLED_COMBOBOX_STYLE);
					}
					if (comboBox->isEnabled())
					{
						Logger::Debug("UIHelper::CreateWidgetsOnOutBoundTreeWidget() - ComboBoxItem:: 2");
						comboBox->setEditable(true);
						comboBox->fillItemListAndDefaultValue(valueList, comboBox->property(ATTRIBUTE_INITIAL_VALUE.c_str()).toString());
						QCompleter* m_nameCompleter = new QCompleter(valueList, comboBox); // using QCompleter class
						m_nameCompleter->setCompletionMode(QCompleter::PopupCompletion);
						m_nameCompleter->setCaseSensitivity(Qt::CaseInsensitive);
						comboBox->setCompleter(m_nameCompleter);
					}
					Logger::Debug("UIHelper::CreateWidgetsOnOutBoundTreeWidget() - ComboBoxItem:: 3");
					_treeWidget->addTopLevelItem(topLevel);						// Adding ToplevelItem
					_treeWidget->setItemWidget(topLevel, 0, CVWidgetGenerator::CreateLabelWidget(attName, attKey, attType, attRequired, !isAttSettable));	// Adding label at column 1
					_treeWidget->setItemWidget(topLevel, 1, comboBox);
				}
				else if (attType == STRING_ATT_TYPE_KEY || attType == TEXT_ATT_TYPE_KEY)
				{
					QTreeWidgetItem* topLevel = new QTreeWidgetItem();  // Creating new TreeWidgetItem
					QLineEdit* LineEdit = new QLineEdit();						// Creating new LineEdit Widget
					LineEdit->setAttribute(Qt::WA_MacShowFocusRect, false);
					LineEdit->setStyleSheet(LINEEDIT_STYLE);
					LineEdit->setProperty(ATTRIBUTE_INITIAL_VALUE.c_str(), QString::fromStdString(attValue));
					LineEdit->setText(QString::fromStdString(attValue));
					_treeWidget->addTopLevelItem(topLevel);						// Adding ToplevelItem
					_treeWidget->setItemWidget(topLevel, 0, CVWidgetGenerator::CreateLabelWidget(attName, attKey, attType, attRequired, !isAttSettable));	// Adding label at column 1
					if (!isAttSettable)
					{
						LineEdit->setDisabled(true);
						LineEdit->setStyleSheet(DISABLED_LINEEDIT_STYLE);
					}
					_treeWidget->setItemWidget(topLevel, 1, LineEdit);

				}
			}
		}
		catch (string msg)
		{
			Logger::Error("UpdateMaterial->drawWidget() Exception - " + msg);
		}
		catch (exception & e)
		{
			Logger::Error("UpdateMaterial->drawWidget() Exception - " + string(e.what()));
		}
		catch (const char* msg)
		{
			wstring wstr(msg, msg + strlen(msg));
			Logger::Error("UpdateMaterial->drawWidget() Exception - " + string(msg));
		}
	}
	

	void UpdateMaterial::onPublishToPLMClicked()
	{
		Logger::Info("UpdateMaterial -> onPublishToPLMClicked() Start....");

		publishToPLMClicked();
		Logger::Info("UpdateMaterial -> onPublishToPLMClicked() End....");
	}

	/*
	* Description - PublishToPLMClicked() method is a slot for publish 3D model to plm click.
	* Parameter -
	* Exception - exception, Char *
	* Return -
	*/
	void UpdateMaterial::publishToPLMClicked()
	{
		Logger::Info("UpdateMaterial -> publishToPLMClicked() start....");
		QDir dir;
		try
		{
			this->hide();
			UTILITY_API->DeleteProgressBar(true);
			UTILITY_API->CreateProgressBar();
			Configuration::GetInstance()->SetProgressBarProgress(0);
			RESTAPI::SetProgressBarData(Configuration::GetInstance()->GetProgressBarProgress(), "Updating " + Configuration::GetInstance()->GetLocalizedMaterialClassName() + "...", true);
			collectUpdateMaterialFieldsData();
			UIHelper::ValidateRquired3DModelData(m_UpdateMaterialTreeWidget_1);
			UIHelper::ValidateRquired3DModelData(m_UpdateMaterialTreeWidget_2);
			Configuration::GetInstance()->SetProgressBarProgress(RESTAPI::SetProgressBarProgress(Configuration::GetInstance()->GetProgressBarProgress(), 5, "Updating " + Configuration::GetInstance()->GetLocalizedMaterialClassName() + "..."));
			string materialId = Helper::GetJSONValue<string>(m_downloadedMaterialJson, "id", true);
			Logger::Debug("Update Material publishToPLMClicked() m_downloadedMaterialJson:: " + m_MaterialMetaData);
			string response = RESTAPI::PutRestCall(m_MaterialMetaData, Configuration::GetInstance()->GetPLMServerURL() + RESTAPI::CREATE_MATERIAL_API + "/" + materialId, "content-type: application/json; boundary=----WebKitFormBoundary7MA4YWxkTrZu0gW");
			Logger::Debug("Update Material publishToPLMClicked() response:: " + response);
			RESTAPI::SetProgressBarData(20, "Updating " + Configuration::GetInstance()->GetLocalizedMaterialClassName(), true);
			if (!FormatHelper::HasContent(response))
			{
				throw "Unable to publish to PLM. Please try again or Contact your System Administrator.";
			}

			if (FormatHelper::HasError(response))
			{
				throw runtime_error(response);
			}
			else
			{
				json detailJson = Helper::GetJsonFromResponse(response, "{");
				string productId = Helper::GetJSONValue<string>(detailJson, ATTRIBUTE_ID, true);
				UTILITY_API->SetProgress("Updating "+ Configuration::GetInstance()->GetLocalizedMaterialClassName(), (qrand() % 101));
				string revisionId = getLatestVersionOfDocument(materialId);
				string fabricFileId = "";

				if (!m_zfabFilePath.toStdString().empty())
				{
					fabricFileId = exportFabricFile(revisionId, materialId);
				}
				UploadMaterialThumbnail(materialId, fabricFileId);

				UTILITY_API->DeleteProgressBar(true);
				m_UpdateMaterialTreeWidget_1->clear();
				m_UpdateMaterialTreeWidget_2->clear();
				CVDisplayMessageBox DownloadDialogObject;

				DownloadDialogObject.DisplyMessage("Successfully Updated in PLM.");
				DownloadDialogObject.setModal(true);
				DownloadDialogObject.exec();
				MaterialConfig::GetInstance()->SetIsSaveAndCloseClicked(false);
			}
		}
		catch (exception& e)
		{
			UTILITY_API->DeleteProgressBar(true);
			Logger::Error("UpdateMaterial -> Update Material Exception - " + string(e.what()));
			UTILITY_API->DisplayMessageBox(e.what());

			this->show();
		}
		catch (const char* msg)
		{
			Logger::Error("UpdateMaterial -> Update Material Exception - " + string(msg));
			wstring wstr(msg, msg + strlen(msg));
			UTILITY_API->DeleteProgressBar(true);
			UTILITY_API->DisplayMessageBoxW(wstr);

			this->show();
		}
		catch (string str)
		{
			Logger::Error("UpdateMaterial -> Update Material Exception - " + str);
			UTILITY_API->DeleteProgressBar(true);
			UTILITY_API->DisplayMessageBox(str);

			this->show();
		}
		Logger::Info("UpdateMaterial -> publishToPLMClicked() end....");
	}

	/*
	* Description - collectUpdateMaterialFieldsData() method used to collect publish to plm fields data.
	* Parameter -
	* Exception -
	* Return -
	*/
	void UpdateMaterial::collectUpdateMaterialFieldsData()
	{
		Logger::Info("UpdateMaterial -> collectUpdateMaterialFieldsData() start....");
		//Collects all Product Section related data into map
		collectUpdateMaterialTreeData();
		Logger::Info("UpdateMaterial -> collectUpdateMaterialFieldsData() end....");
	}

	/*
	* Description - Collect3DModelFieldsData() method used to collect 3d model fields data.
	* Parameter -
	* Exception -
	* Return -
	*/
	void UpdateMaterial::collectUpdateMaterialTreeData()
	{
		Logger::Info("UpdateMaterial -> collectUpdateMaterialTreeData() start....");
		m_MaterialMetaData = collectCriteriaFields(m_UpdateMaterialTreeWidget_1, m_UpdateMaterialTreeWidget_2);
		Logger::Info("UpdateMaterial -> collectUpdateMaterialTreeData() end....");
	}

	/*
	* Description - PreparePublishRequestParameter() method used to prepare request parameter.
	* Parameter - _path, _imageName
	* Exception -
	* Return - string
	*/
	string UpdateMaterial::getPublishRequestParameter(string _path, string _fileName)
	{
		Logger::Info("UpdateMaterial -> getPublishRequestParameter() start....");
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
			Logger::Debug("Update material getPublishRequestParameter(string _path, string _imageName) end....");
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
		Logger::Info("UpdateMaterial -> getPublishRequestParameter() End....");
		return postField;
	}

	/*
	* Description - CollectCriteriaFields() method used to collect data in Publish to plm UI.
	* Parameter -  QTreeWidget.
	* Exception -
	* Return - string.
	*/
	string UpdateMaterial::collectCriteriaFields(QTreeWidget* _documentPushToPLMTree_1, QTreeWidget* _documentPushToPLMTree_2)
	{
		Logger::Info("UpdateMaterial -> collectCriteriaFields() start....");
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
				attJson = UIHelper::ReadVisualUIFieldValue(_documentPushToPLMTree_1, i);
				if (!attJson.empty())
				{
					attkey = Helper::GetJSONValue<string>(attJson, "attKey", true);
					attValue = Helper::GetJSONValue<string>(attJson, "attValue", true);
					attType = Helper::GetJSONValue<string>(attJson, ATTRIBUTE_TYPE_KEY, true);
					if (!attValue.empty() && attValue != "|@isDisabled@|")
					{
						if (attType == "integer" || attType == "float" || attType == "boolean") {
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
				attJson = UIHelper::ReadVisualUIFieldValue(_documentPushToPLMTree_2, i);
				if (!attJson.empty())
				{
					attkey = Helper::GetJSONValue<string>(attJson, "attKey", true);
					attValue = Helper::GetJSONValue<string>(attJson, "attValue", true);
					attType = Helper::GetJSONValue<string>(attJson, ATTRIBUTE_TYPE_KEY, true);
					if (!attValue.empty() && attValue != "|@isDisabled@|")
					{
						if (attType == "integer" || attType == "float" || attType == "boolean") {
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
			data = data.substr(0, data.length() - 1);
			data += "\n}";
		}
		catch (string msg)
		{
			Logger::Error("UpdateMaterial -> collectCriteriaFields() Exception - " + msg);
		}
		catch (exception & e)
		{
			Logger::Error("UpdateMaterial -> collectCriteriaFields() Exception - " + string(e.what()));
		}
		catch (const char* msg)
		{
			wstring wstr(msg, msg + strlen(msg));
			Logger::Error("UpdateMaterial -> collectCriteriaFields() Exception - " + string(msg));
		}
		Logger::Info("UpdateMaterial -> collectCriteriaFields() end....");
		return data;
	}

	/*
	* Description - addAttachmentClicked() method used to collect data in Publish to plm UI.
	* Parameter -
	* Exception -
	* Return -
	*/
	void UpdateMaterial::addAttachmentClicked()
	{
		Logger::Info("UpdateMaterial -> addAttachmentClicked() -> Start");
		QString zfabFilePath;
		if (m_zfabFilePath.isEmpty())
		{
			zfabFilePath = QFileDialog::getOpenFileName(this, tr("Add Attachment"), FABRICS_FILE_PATH, "ZFAB Files(*.zfab);; Trim Files(*.trm *.obj *.sst);; Button Files (*.btn *.bth);; Zippers (*.zpac *.zsd *.zip *zpl)");
		}
		else
		{
			zfabFilePath = QFileDialog::getOpenFileName(this, tr("Add Attachment"), QString::fromStdString(m_zfabPath), "ZFAB Files(*.zfab);; Trim Files(*.trm *.obj *.sst);; Button Files(*.btn *.bth);; Zippers(*.zpac *.zsd *.zip *zpl)");
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

		Logger::Info("UpdateMaterial -> addAttachmentClicked() -> End");
	}

	/*
	* Description - onAddAttachmentClicked() method used to collect data in Publish to plm UI.
	* Parameter -
	* Exception -
	* Return -
	*/
	void UpdateMaterial::onAddAttachmentClicked()
	{
		Logger::Info("UpdateMaterial -> onAddAttachmentClicked() -> Start");
		addAttachmentClicked();
		Logger::Info("UpdateMaterial -> onAddAttachmentClicked() -> End");
	}

	void UpdateMaterial::DeletePointer(QWidget*_ptr)
	{
		Logger::Info("UpdateMaterial -> DeletePointer() -> Start");
		if (_ptr != nullptr)
		{
			delete _ptr;
		}
		Logger::Info("UpdateMaterial -> DeletePointer() -> End");
	}

	/*
	* Description - UploadMaterialThumbnail() method uploads the thumbnaials for the maaterial.
	* Parameter - _productId, _fabricFileId
	* Exception -
	* Return -
	*/
	void UpdateMaterial::UploadMaterialThumbnail(string _productId, string _fabricId)
	{
		Logger::Info("UpdateMaterial -> UploadMaterialThumbnail() -> Start");
		string imageId = "";
		string materialDocumentId = Helper::GetJSONValue<string>(m_downloadedMaterialJson, "default_3d_material", true);
		if (materialDocumentId.empty() || materialDocumentId == "centric%3A")
			materialDocumentId = _fabricId;
		string data = "{}";
		if (!m_MaterialThumbnailPath.empty())
		{
			string postField = getPublishRequestParameter(m_MaterialThumbnailPath, m_MaterialThumbnailName);
			string resultJsonString = RESTAPI::PostRestCall(postField, Configuration::GetInstance()->GetPLMServerURL() + RESTAPI::UPLOAD_IMAGE_API, "content-type: multipart/form-data; boundary=----WebKitFormBoundary7MA4YWxkTrZu0gW");
			json detailJson = Helper::GetJsonFromResponse(resultJsonString, "{");
			
			imageId = Helper::GetJSONValue<string>(detailJson, ATTRIBUTE_ID, true);
			data = "{\n\"images\":{\n\"\":\"" + imageId + "\",\n\"1\":\"" + imageId + "\"\n},\n\"default_3d_material\":\"" + materialDocumentId + "\"\n }";
		}
		else
		{
			data = "{\"default_3d_material\":\"" + materialDocumentId + "\"}";
		}
		string imageLinkReponse = RESTAPI::PutRestCall(data, Configuration::GetInstance()->GetPLMServerURL() + RESTAPI::CREATE_MATERIAL_API + "/" + _productId, "content-type: application/json");
		Logger::Debug("UpdateMaterial -> UploadMaterialThumbnail() -> imageLinkReponse:: " + imageLinkReponse);
		Logger::Info("UpdateMaterial -> UploadMaterialThumbnail() -> End");
	}

	/*
	* Description - ExtractThumbnailFromAttachment() method extracts the thumbnails(snapshots) from attachhments.
	* Parameter -
	* Exception -
	* Return -
	*/
	void UpdateMaterial::ExtractThumbnailFromAttachment()
	{
		Logger::Info("UpdateMaterial -> ExtractThumbnailFromAttachment() -> Start");
		std::string fabricFilePath = m_zfabFilePath.toStdString();
		m_MaterialThumbnailPath = "";
		string thumbnailName;
		const size_t lastDotIndex = m_attachmentName.find(".");
		thumbnailName = m_attachmentName.substr(0, lastDotIndex);
		string iconThumbnail;
		iconThumbnail = thumbnailName + ".png";
		m_MaterialThumbnailName = iconThumbnail;
		unsigned int fileSize = 0;
		unsigned char * icoBuffer = UTILITY_API->GetAssetIconInCLOFile(fabricFilePath, fileSize);
		string filepath;
		filepath = m_zfabPath + "/" + iconThumbnail;

		if (icoBuffer != nullptr && fileSize > 0)
		{

			FILE * saveFp = fopen(filepath.c_str(), "wb");
			fwrite((const void *)icoBuffer, 1, fileSize, saveFp);
			fclose(saveFp);
			m_MaterialThumbnailPath = filepath;
		}
		unsigned int fileSizel = 0;
		unsigned int fileSize2 = 0;
		unsigned char * bufferl = UTILITY_API->GetThumbnailInCLOFile(fabricFilePath, 0, fileSizel);
		unsigned char * buffer2 = UTILITY_API->GetThumbnailInCLOFile(fabricFilePath, 1, fileSize2);
		iconThumbnail = m_MaterialThumbnailName + "1.png";
		filepath = m_zfabPath + "/" + iconThumbnail;

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
			FILE * saveFp = fopen(filepath.c_str(), "wb");
			fwrite((const void *)buffer2, 1, fileSize2, saveFp);
			fclose(saveFp);
			if (m_zfabFilePath.contains(".zfab"))
				m_MaterialThumbnailPath = filepath;
		}
		Logger::Info("UpdateMaterial -> ExtractThumbnailFromAttachment() -> End");
	}

	/*
	* Description - exportFabricFile() method revise the document and uploads updated document.
	* Parameter - _revisionId
	* Exception - using throw if anything gets fail.
	* Return - string
	*/
	string UpdateMaterial::exportFabricFile(string _revisionId, string _materialId)
	{
		Logger::Info("UpdateMaterial -> exportFabricFile() start....");
		json emptyJson = json::object();
		string resultJsonString = "";
		string postField = getPublishRequestParameter(m_zfabFilePath.toStdString(), m_attachmentName);
		if (!_revisionId.empty())
		{
			string reviseDocumentResponse = RESTAPI::PostRestCall(to_string(emptyJson), Configuration::GetInstance()->GetPLMServerURL() + RESTAPI::DOCUMENT_UPLOAD_API + "/" + _revisionId + "/revise", "content-type: application/json; boundary=----WebKitFormBoundary7MA4YWxkTrZu0gW");
			if (!FormatHelper::HasContent(reviseDocumentResponse))
			{
				throw "Unable to revise document in PLM. Please try again or Contact your System Administrator.";
			}
			if (FormatHelper::HasError(reviseDocumentResponse))
			{
				throw runtime_error(reviseDocumentResponse);
			}
			json revisedResponseJson = json::parse(reviseDocumentResponse);
			string latestRevisionId = Helper::GetJSONValue<string>(revisedResponseJson, ATTRIBUTE_ID, true);
			 resultJsonString = RESTAPI::PutRestCall(postField, Configuration::GetInstance()->GetPLMServerURL() + RESTAPI::DOCUMENT_UPLOAD_API + "/" + latestRevisionId + "/upload", "content-type: multipart/form-data; boundary=----WebKitFormBoundary7MA4YWxkTrZu0gW");
		}
		else
			 resultJsonString = RESTAPI::PostRestCall(postField, Configuration::GetInstance()->GetPLMServerURL() + RESTAPI::DOCUMENT_CREATE_API + "/" + _materialId + "/upload", "content-type: multipart/form-data; boundary=----WebKitFormBoundary7MA4YWxkTrZu0gW");
	
		json detailJson = Helper::GetJsonFromResponse(resultJsonString, "{");
		string FabricFileId = Helper::GetJSONValue<string>(detailJson, ATTRIBUTE_ID, true);

		Logger::Info("UpdateMaterial -> exportFabricFile() end....");
		return FabricFileId;
	}

	void UpdateMaterial::reject()
	{
		Logger::Info("UpdateMaterial -> reject() -> Start");
		this->accept();
		Logger::Info("UpdateMaterial -> reject() -> End");
	}

	void UpdateMaterial::ClearAllFields(QTreeWidget* _documentPushToPLMTree)
	{
		Logger::Info("UpdateMaterial -> ClearAllFields() -> Start");
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
					//UTILITY_API->DisplayMessageBox("qspinBox -> defaultValue:: " + defaultValue.toStdString());
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
			Logger::Error("ERROR::UpdateMaterial: ClearAllFields()-> Exception string:: " + msg);
			this->show();
		}
		catch (exception& e)
		{
			UTILITY_API->DeleteProgressBar(true);
			Logger::Error("ERROR::UpdateMaterial: ClearAllFields()-> Exception e:: " + string(e.what()));
			this->show();

		}
		catch (const char* msg)
		{
			UTILITY_API->DeleteProgressBar(true);
			Logger::Error("ERROR::UpdateMaterial: ClearAllFields()-> Exception char:: " + string(msg));
			wstring wstr(msg, msg + strlen(msg));
			this->show();

		}
		Logger::Info("UpdateMaterial -> ClearAllFields() -> End");
	}

	void UpdateMaterial::onSaveAndCloseClicked()
	{
		Logger::Info("UpdateMaterial -> onSaveAndCloseClicked() -> Start");
		MaterialConfig::GetInstance()->SetIsSaveAndCloseClicked(true);
		{ 
			m_tabWidget->setCurrentIndex(0);
			this->hide();
			if (UTILITY_API)
				UTILITY_API->DisplayMessageBox(Configuration::GetInstance()->GetLocalizedMaterialClassName() + " metadata saved");
		}
		Logger::Info("UpdateMaterial -> onSaveAndCloseClicked() -> End");
	}

	/*
	* Description - getLatestVersionOfDocument() method used to get latest revision of the document
	* Parameter - _materialId
	* Exception - using throw if anything gets fail.
	* Return - string
	*/
	string UpdateMaterial::getLatestVersionOfDocument(string _materialId)
	{
		Logger::Info("UpdateMaterial -> getLatestVersionOfDocument -> start");
		string latestRevision = "";
		try
		{
			string attachmentResponse = RESTAPI::CentricRestCallGet(Configuration::GetInstance()->GetPLMServerURL() + RESTAPI::ATTACHMENTS_LATEST_REVISION_RESULTS_API + _materialId + "?revision_details=true&decode=true", APPLICATION_JSON_TYPE, "");
			if (FormatHelper::HasError(attachmentResponse))
			{
				Helper::GetCentricErrorMessage(attachmentResponse);
				throw runtime_error(attachmentResponse);
			}
			json attachmentjson = json::parse(attachmentResponse);

			string latestVersionAttName = "";
			
			for (int attachmenAarrayCount = 0; attachmenAarrayCount < attachmentjson.size(); attachmenAarrayCount++)
			{
				json attachmentCountJson = Helper::GetJSONParsedValue<int>(attachmentjson, attachmenAarrayCount, false);
				latestRevision = Helper::GetJSONValue<string>(attachmentCountJson, "latest_revision", true);
			}
		}
		catch (string msg)
		{
			Logger::Error("UpdateMaterial -> getLatestVersionOfDocument () Exception - " + msg);
		}
		catch (exception & e)
		{
			Logger::Error("UpdateMaterial -> getLatestVersionOfDocument () Exception - " + string(e.what()));
		}
		catch (const char* msg)
		{
			wstring wstr(msg, msg + strlen(msg));
			Logger::Error("UpdateMaterial -> getLatestVersionOfDocument () Exception - " + string(msg));
		}
		Logger::Info("UpdateMaterial -> getLatestVersionOfDocument -> end");
		return latestRevision;
	}

	/*
	* Description - UpdateMaterialWidget() method used to rset/create update widget.
	* Parameter - bool
	* Exception - 
	* Return - 
	*/
	void UpdateMaterial::UpdateMaterialWidget(bool _isFromConstructor)
	{
		Logger::Info("UpdateMaterial -> UpdateMaterialWidget -> Start");
		if (_isFromConstructor)
		{
			m_downloadedMaterialJson = MaterialConfig::GetInstance()->GetUpdateMaterialCacheData();
			addUpdateMaterialDetailsWidgetData();
		}
		else if (!MaterialConfig::GetInstance()->GetIsSaveAndCloseClicked())
		{
			m_attachmentsList->clear();
			m_UpdateMaterialTreeWidget_1->clear();
			m_UpdateMaterialTreeWidget_2->clear();
			m_tabWidget->setCurrentIndex(0);
			m_zfabFilePath.clear();
			m_downloadedMaterialJson = MaterialConfig::GetInstance()->GetUpdateMaterialCacheData();
			addUpdateMaterialDetailsWidgetData();
		}
		PLMMaterialSearch::GetInstance()->DrawSearchWidget(false);

		Logger::Info("UpdateMaterial -> UpdateMaterialWidget -> End");
	}
}

