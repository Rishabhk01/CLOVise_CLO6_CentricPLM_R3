/*
* Copyright 2020-2021 CLO-Vise. All rights reserved
*
* @file PLMPrintSearch.cpp
*
* @brief Class implementation for accessing Prints in CLO from PLM.
* This class has all the variable and methods implementation which are used in PLM Print Search.
*
* @author GoVise
*
* @date  27-MAY-2020
*/
#include "PLMPrintSearch.h"

#include <iostream>
#include <string>

#include "CLOVise/PLM/Inbound/Print/PLMPrintResults.h"
#include "CLOVise/PLM/Inbound/Print/PLMPrintSearch.h"
#include "CLOVise/PLM/Helper/Util/Helper.h"
#include "CLOVise/PLM/Helper/UIHelper/UIHelper.h"
#include "CLOVise/PLM/Helper/Util/RestAPIUtility.h"
#include "CLOVise/PLM/Helper/Util/FormatHelper.h"
#include "CLOVise/PLM/Helper/Util/FlexTypeHelper.h"
#include "CLOVise/PLM/Helper/UIHelper/CVWidgetGenerator.h"
#include "CLOVise/PLM/CLOViseSuite/CLOViseSuite.h"
#include "CLOVise/PLM/Inbound/Print/PrintConfig.h"
#include "classes/widgets/MVDialog.h"
#include "CLOVise/PLM/Helper/Util/Logger.h"
#include "CLOVise/PLM/Outbound/PublishToPLM/CreateProduct.h"
#include "CLOVise/PLM/Outbound/PublishToPLM/UpdateProduct.h"
using namespace std;

namespace CLOVise
{
	PLMPrintSearch* PLMPrintSearch::_instance = NULL;
	
	PLMPrintSearch* PLMPrintSearch::GetInstance()
	{
		if (_instance == NULL)
			_instance = new PLMPrintSearch();
		return _instance;
	}


	PLMPrintSearch::PLMPrintSearch(QWidget* parent)
		: MVDialog(parent)
	{
		Logger::Info("PLMPrintSearch -> Constructor() -> Start");

		
		setupUi(this);
		QString windowTitle = PLM_NAME + " Print Design Color" + " Search Criteria ";
		this->setWindowTitle(windowTitle);

#ifdef __APPLE__
		this->setWindowFlags(Qt::Dialog | Qt::CustomizeWindowHint | Qt::WindowMinMaxButtonsHint | Qt::WindowCloseButtonHint);
#else
		this->setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint | Qt::CustomizeWindowHint);
        m_pTitleBar = new MVTitleBar(windowTitle, this);
        layout()->setMenuBar(m_pTitleBar);
#endif // !__APPLE__
		m_hierarchyTreeWidget = nullptr;
		m_searchTreeWidget_1  = nullptr;
		m_searchTreeWidget_2  = nullptr;
		m_viewComboBox = nullptr;
		m_seasoncomboBox = nullptr;
		m_paletteComboBox = nullptr;
		m_filterComboBox = nullptr;
		m_paletteCheckBox = nullptr;
		m_seasonLabel = nullptr;
		m_paletteLabel = nullptr;
		m_quickSearchLineEdit = nullptr;
		m_dateResetButton = nullptr;
		m_dateResetButton_1 = nullptr;
		m_backButton = nullptr;
		m_unselectAllButton = nullptr;
		m_searchButton = nullptr;
		m_hierarchyTreeWidget = CVWidgetGenerator::CreateHirarchyTreeWidget("QTreeWidget{min-height: 230px; min-width: 50px; max-height: 1500px; max-width: 230px; border: 1px solid #000; font-family: ArialMT; font-size: 10px; background-color: #262628; margin-bottom: 20px;} QTreeWidget:item:selected{ color: #46C8FF; }", false);
		m_hierarchyTreeWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
		m_hierarchyTreeWidget->hide();
		m_searchTreeWidget_1 = CVWidgetGenerator::CreateSearchCriteriaTreeWidget(/*"QTreeWidget { background-color: #262628; border: 1px solid #000; padding-left: 20px; }""QTreeWidget::item {height: 20px; width: 200px; margin-right: 20px; margin-top: 5px; margin-bottom: 5px; border: none; }""QTreeWidget::item:hover{ background-color: #262628; }"*/true);
		m_searchTreeWidget_1->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
		m_searchTreeWidget_2 = CVWidgetGenerator::CreateSearchCriteriaTreeWidget(/*"QTreeWidget { background-color: #262628; border: 1px solid #000; padding-left: 20px; }""QTreeWidget::item {height: 20px; width: 200px; margin-right: 20px; margin-top: 5px; margin-bottom: 5px; border: none; }""QTreeWidget::item:hover{ background-color: #262628; }"*/true);
		m_searchTreeWidget_2->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

		m_viewComboBox = CVWidgetGenerator::CreateComboBox(QString::fromStdString(BLANK), VIEW_COMBOBOX_STYLE + SCROLLBAR_STYLE, false);
		m_filterComboBox = CVWidgetGenerator::CreateComboBox(QString::fromStdString(BLANK), VIEW_COMBOBOX_STYLE + SCROLLBAR_STYLE, false);
		m_seasoncomboBox = CVWidgetGenerator::CreateComboBox(QString::fromStdString(BLANK), FONT_STYLE + SCROLLBAR_STYLE, true);
		m_seasoncomboBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
		m_paletteComboBox = CVWidgetGenerator::CreateComboBox(QString::fromStdString(BLANK), FONT_STYLE + SCROLLBAR_STYLE, true);
		m_paletteComboBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

		ui_paletteSearchWidget->hide();
		ui_hideAndShowButton->hide();
		ui_verticalSeparator->hide();

		QLabel* viewLabel = CVWidgetGenerator::CreateLabel("View", QString::fromStdString(BLANK), HEADER_STYLE, false);
		QLabel* filterLabel = CVWidgetGenerator::CreateLabel("Filter", QString::fromStdString(BLANK), HEADER_STYLE, false);
		QLabel* paletteCheckBoxLable = CVWidgetGenerator::CreateLabel("Palette Search Criteria", QString::fromStdString(BLANK), HEADER_STYLE, true);
		paletteCheckBoxLable->hide();
		m_seasonLabel = CVWidgetGenerator::CreateLabel("Season", QString::fromStdString(BLANK), HEADER_STYLE, true);
		m_seasonLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
		m_paletteLabel = CVWidgetGenerator::CreateLabel("Palette", QString::fromStdString(BLANK), HEADER_STYLE, true);
		m_paletteLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
		QLabel* searchCriteriaLabel = CVWidgetGenerator::CreateLabel("Search Criteria", QString::fromStdString(BLANK), HEADER_STYLE, true);
		QLabel* quickSearchHeaderLabel = CVWidgetGenerator::CreateLabel("Quick Search Criteria", QString::fromStdString(BLANK), HEADER_STYLE, true);
		quickSearchHeaderLabel->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
		QLabel* quickSearchLabel = CVWidgetGenerator::CreateLabel("Quick Search", QString::fromStdString(BLANK), HEADER_STYLE, true);
		quickSearchLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

		m_paletteCheckBox = CVWidgetGenerator::CreateCheckBox("", "", true);
		m_paletteCheckBox->hide();

		m_quickSearchLineEdit = CVWidgetGenerator::CreateLineEdit("", LINEEDIT_STYLE, true);

		m_dateResetButton = CVWidgetGenerator::CreateToolButton("", "Reset Date", "QToolButton { icon_size: 18px; image: url(:/CLOVise/PLM/Images/DateReset_none.svg); }""QToolButton:hover { image: url(:/CLOVise/PLM/Images/DateReset_over.svg); }""QToolTip{ color: #46C8FF; background-color: #33414D; border: 1px #000000; }", true);

		m_backButton = CVWidgetGenerator::CreatePushButton("Back", BACK_HOVER_PATH, "Back", PUSH_BUTTON_STYLE, 30, true);
		m_backButton->setFocusPolicy(Qt::NoFocus);
		//m_unselectAllButton = CVWidgetGenerator::CreatePushButton("Back", BACK_HOVER_PATH, "Back", BUTTON_STYLE);
		m_searchButton = CVWidgetGenerator::CreatePushButton("Search", SEARCH_HOVER_ICON_PATH, "Search", PUSH_BUTTON_STYLE, 30, true);
		m_searchButton->setFocusPolicy(Qt::StrongFocus);
#ifdef __APPLE__
		ui_paletteLabelLayout->setSpacing(10);
#endif

		this->setStyleSheet("QDialog{height:539px; width: 1050px;}");
		/*ui_paletteSearchWidget->setStyleSheet("#ui_paletteSearchWidget{padding: 10px; width: 920px;  border: 1px solid #000; background-color: #262628; max-height:50px;}");
		ui_quickSearchWidget->setStyleSheet("#ui_quickSearchWidget{padding: 10px; width: 920px;border: 1px solid #000; background-color: #262628; max-height:42px;}");
*/
		//initializeColorData();
		m_flextypeTree.clear();
		/*selectType = FlexTypeHelper::CreateFlexTypeTree(ColorConfig::GetInstance()->GetColorHierarchyJSON(), m_hierarchyTreeWidget, m_flextypeTree);

		m_hierarchyTreeWidget->topLevelItem(0)->setSelected(true);
		m_hierarchyTreeWidget->expandAll();
		m_hierarchyTreeWidget->setExpandsOnDoubleClick(false);
		m_hierarchyTreeWidget->setSortingEnabled(true);
		m_hierarchyTreeWidget->sortByColumn(0, Qt::AscendingOrder);
		m_hierarchyTreeWidget->setFocusPolicy(Qt::NoFocus);*/
		onHideHirarchy(false);
		DrawSearchWidget(true);
		//CVWidgetGenerator::CreateViewComboBoxOnSearch(m_viewComboBox, ColorConfig::GetInstance()->GetColorViewJSON(), selectType.toStdString());
		
		connectSignalSlots(true);

		m_seasonLabel->hide();
		m_paletteLabel->hide();
		m_seasoncomboBox->hide();
		m_paletteComboBox->hide();
		quickSearchHeaderLabel->hide();

		QSpacerItem *horizontalSpacer = new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Fixed);
		QSpacerItem *horizontalSpacer_1 = new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Fixed);
		QSpacerItem *horizontalSpacer_2 = new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Fixed);
		QSpacerItem *horizontalSpacer_3 = new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Fixed);
		QSpacerItem *horizontalSpacer_4 = new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Fixed);
		QSpacerItem *horizontalSpacer_5 = new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Fixed);

		QFrame *frame;
		QGridLayout *gridLayout;
		QSplitter *splitter;

		frame = new QFrame();
		frame->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
		gridLayout = new QGridLayout(frame);
		splitter = new QSplitter(frame);
		splitter->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
		splitter->setOrientation(Qt::Horizontal);
		splitter->insertWidget(0, m_searchTreeWidget_1);
		splitter->insertWidget(1, m_searchTreeWidget_2);
		gridLayout->setContentsMargins(QMargins(0, 0, 0, 0));
		gridLayout->addWidget(splitter, 0, 0, 1, 1);

		ui_searchTreeLayout_1->addWidget(frame);

		ui_heirarchyLayout->addWidget(m_hierarchyTreeWidget);

		ui_viewLayout->addWidget(viewLabel);
		ui_viewLayout->addWidget(m_viewComboBox);

		ui_filterLayout->addWidget(filterLabel);
		ui_filterLayout->addWidget(m_filterComboBox);

		ui_paletteLabelLayout->insertWidget(0, m_paletteCheckBox);
		ui_paletteLabelLayout->insertWidget(1, paletteCheckBoxLable);
		ui_paletteLabelLayout->insertSpacerItem(2, horizontalSpacer);
		
		ui_paletteLayout_1->insertWidget(0, m_seasonLabel);
		ui_paletteLayout_1->insertWidget(1, m_seasoncomboBox);
		ui_paletteLayout_1->insertSpacerItem(2, horizontalSpacer_1);		
		
		ui_paletteLayout_2->insertWidget(0, m_paletteLabel);
		ui_paletteLayout_2->insertWidget(1, m_paletteComboBox);
		ui_paletteLayout_2->insertSpacerItem(2, horizontalSpacer_2);

		ui_quickSearchCriteriaLayout->addWidget(quickSearchHeaderLabel);
		ui_quickSearchLayout->insertWidget(0, quickSearchLabel);
		ui_quickSearchLayout->insertWidget(1, m_quickSearchLineEdit);
		ui_quickSearchLayout->insertSpacerItem(2, horizontalSpacer_3);

		ui_searchHeader_1->insertWidget(0, searchCriteriaLabel);
		ui_searchHeader_1->insertSpacerItem(1, horizontalSpacer_4);
		ui_searchHeader_1->insertWidget(2, m_dateResetButton);

		ui_buttonsLayout->insertWidget(0, m_backButton);
		ui_buttonsLayout->insertSpacerItem(1, horizontalSpacer_5);
		ui_buttonsLayout->insertWidget(2, m_searchButton);

		ui_searchHeader_1->setEnabled(false);
		ui_searchHeader_1->setStretchFactor(ui_searchHeader_1, 0);

		ui_searchTreeLayout_1->setEnabled(false);
		ui_searchTreeLayout_1->setStretchFactor(ui_searchTreeLayout_1, 0);
		ui_quickSearchWidget->hide();

//		/*if (!ColorConfig::GetInstance()->isModelExecuted)*/
//			RESTAPI::SetProgressBarData(0, "", false);
//#ifdef __APPLE__
//		RESTAPI::SetProgressBarData(0, "", false);
//#endif

		m_dateResetButton->hide();
		UTILITY_API->DeleteProgressBar(true);
		Logger::Info("PLMPrintSearch -> Constructor() -> End");
	}

	/*
	* Description - HideHirarchy() method used to show/hide the hierarchy.
	* Parameter -  bool.
	* Exception -
	* Return -
	*/
	void PLMPrintSearch::HideHirarchy(bool _hide)
	{
		Logger::Info("PLMPrintSearch -> HideHirarchy() -> Start");
		if (!_hide)
		{
			if (!m_isHidden)
			{
				//ui_hideAndShowButton->setToolTip("Show");
				m_hierarchyTreeWidget->hide();
				//ui_hideAndShowButton->setStyleSheet("#ui_hideAndShowButton {\n" "qproperty-icon: none; \n""image: url(:/CLOVise/PLM/Images/icon_open_tree_none.svg);\n""}\n"
					//"#ui_hideAndShowButton:hover{\n""image: url(:/CLOVise/PLM/Images/icon_open_tree_over.svg);\n""}""QToolTip{ color: #46C8FF; background-color: #33414D; border: 1px #000000; }");
				m_isHidden = true;
			}
			else
			{
				//ui_hideAndShowButton->setToolTip("Hide");
				m_hierarchyTreeWidget->show();
				//ui_hideAndShowButton->setStyleSheet("#ui_hideAndShowButton {\n" "qproperty-icon: none; \n""image: url(:/CLOVise/PLM/Images/icon_close_tree_none.svg);\n""}\n"
					//"#ui_hideAndShowButton:hover{\n""image: url(:/CLOVise/PLM/Images/icon_close_tree_over.svg);\n""}""QToolTip{ color: #46C8FF; background-color: #33414D; border: 1px #000000; }");
				m_isHidden = false;
			}
		}
		Logger::Info("PLMPrintSearch -> HideHirarchy() -> End");
	}

	/*
	* Description - HideHirarchy() method used to show/hide the hierarchy.
	* Parameter -  bool.
	* Exception - 
	* Return -
	*/
	void PLMPrintSearch::onHideHirarchy(bool _hide)
	{
		Logger::Info("PLMPrintSearch -> onHideHirarchy() -> Start");
		HideHirarchy(_hide);
		Logger::Info("PLMPrintSearch -> onHideHirarchy() -> End");
	}

	PLMPrintSearch::~PLMPrintSearch()
	{
		Logger::Info("PLMPrintSearch -> Destructor() -> Start");
		
		DisconnectSignalSlots();
		UIHelper::DeletePointer(m_hierarchyTreeWidget);
		UIHelper::DeletePointer(m_searchTreeWidget_1);
		UIHelper::DeletePointer(m_searchTreeWidget_2);
		UIHelper::DeletePointer(m_viewComboBox);
		UIHelper::DeletePointer(m_seasoncomboBox);
		UIHelper::DeletePointer(m_paletteComboBox);
		UIHelper::DeletePointer(m_filterComboBox);
		UIHelper::DeletePointer(m_paletteCheckBox);
		UIHelper::DeletePointer(m_seasonLabel);
		UIHelper::DeletePointer(m_paletteLabel);
		UIHelper::DeletePointer(m_quickSearchLineEdit);
		UIHelper::DeletePointer(m_dateResetButton);
		UIHelper::DeletePointer(m_dateResetButton_1);
		UIHelper::DeletePointer(m_backButton);
		UIHelper::DeletePointer(m_unselectAllButton);
		UIHelper::DeletePointer(m_searchButton);
		Logger::Info("PLMPrintSearch -> Destructor() -> End");
	}

	/*
	* Description - ResetDateEditWidget() method is a slot for reset date button click and used to reset the date fields.
	* Parameter -
	* Exception -
	* Return -
	*/
	void PLMPrintSearch::ResetDateEditWidget()
	{
		Logger::Info("PLMPrintSearch -> ResetDateEditWidget() -> Start");
		UIHelper::ResetDate(m_searchTreeWidget_1);
		UIHelper::ResetDate(m_searchTreeWidget_2);
		Logger::Info("PLMPrintSearch -> ResetDateEditWidget() -> End");
	}

	/*
	* Description - ResetDateEditWidget() method is a slot for reset date button click and used to reset the date fields.
	* Parameter - 
	* Exception - 
	* Return -
	*/
	void PLMPrintSearch::onResetDateEditWidget()
	{
		Logger::Info("PLMPrintSearch -> onResetDateEditWidget() -> Start");
		ResetDateEditWidget();
		Logger::Info("PLMPrintSearch -> onResetDateEditWidget() -> End");
	}

	/*
	* Description - connectSignalSlots() method used to connect and disconnect the signal and slot.
	* Parameter -   bool.
	* Exception - 
	* Return -
	*/
	void PLMPrintSearch::connectSignalSlots(bool _b)
	{
		Logger::Info("PLMPrintSearch -> connectSignalSlots() -> Start");
		if (_b)
		{
			//QObject::connect(ui_hideAndShowButton, SIGNAL(clicked(bool)), this, SLOT(onHideHirarchy(bool)));
			QObject::connect(m_hierarchyTreeWidget, SIGNAL(itemClicked(QTreeWidgetItem*, int)), this, SLOT(onTreeNodeClicked(QTreeWidgetItem*, int)));
			QObject::connect(m_searchButton, SIGNAL(clicked()), this, SLOT(onClickedSubmitButton())); // should be used
			QObject::connect(m_backButton, SIGNAL(clicked()), this, SLOT(onClickedBackButton()));
			QObject::connect(m_filterComboBox, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(onFilterChanged(const QString&)));
			QObject::connect(m_paletteCheckBox, SIGNAL(toggled(bool)), this, SLOT(onCheckBoxChecked(bool)));
			QObject::connect(m_seasoncomboBox, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(onSeasonChanged(const QString&)));
			QObject::connect(m_dateResetButton, SIGNAL(clicked()), this, SLOT(onResetDateEditWidget()));
		}
		else
		{
			//QObject::disconnect(ui_hideAndShowButton, SIGNAL(clicked()), this, SLOT(onHideHirarchy(bool)));
			QObject::disconnect(m_hierarchyTreeWidget, SIGNAL(itemClicked(QTreeWidgetItem*, int)), this, SLOT(onTreeNodeClicked(QTreeWidgetItem*, int)));
			QObject::disconnect(m_searchButton, SIGNAL(clicked()), this, SLOT(onClickedSubmitButton())); // should be used
			QObject::disconnect(m_backButton, SIGNAL(clicked()), this, SLOT(onClickedBackButton()));
			QObject::disconnect(m_paletteCheckBox, SIGNAL(toggled(bool)), this, SLOT(onCheckBoxChecked(bool)));
			QObject::disconnect(m_filterComboBox, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(onFilterChanged(const QString&)));
			QObject::disconnect(m_seasoncomboBox, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(onSeasonChanged(const QString&)));
			QObject::disconnect(m_dateResetButton, SIGNAL(clicked()), this, SLOT(onResetDateEditWidget()));
		}
		Logger::Info("PLMPrintSearch -> connectSignalSlots() -> End");
	}

	/*
	* Description - ClickedBackButton() method is a slot for back button click and used to go back design suite.
	* Parameter -
	* Exception -
	* Return -
	*/
	void PLMPrintSearch::ClickedBackButton()
	{

		Logger::Info("PLMPrintSearch -> ClickedBackButton() -> Start");
		this->hide();
		switch (Configuration::GetInstance()->GetCurrentScreen())
		{

			case CREATE_PRODUCT_CLICKED:
			{			
				CreateProduct::GetInstance()->setModal(true);
				CreateProduct::GetInstance()->show();
			}
			break;
			case UPDATE_PRODUCT_CLICKED:
			{
				UpdateProduct::GetInstance()->setModal(true);
				UpdateProduct::GetInstance()->show();
			}
			break;
			case PRINT_SEARCH_CLICKED:
			{
				CLOVise::CLOViseSuite::GetInstance()->setModal(true);
				CLOViseSuite::GetInstance()->show();
			}
		}

		//PLMColorSearch::Destroy();
		Logger::Info("PLMPrintSearch -> ClickedBackButton() -> End");
	}

	/*
	* Description - OnClickedBackButton() method is a slot for back button click and used to go back design suite.
	* Parameter - 
	* Exception - 
	* Return -
	*/
	void PLMPrintSearch::onClickedBackButton()
	{		
		Logger::Info("PLMPrintSearch -> onClickedBackButton() -> Start");
		ClickedBackButton();
		Logger::Info("PLMPrintSearch -> onClickedBackButton() -> End");
	}

	/*
	* Description - ClickedSubmitButton() method is a slot for submit button click, validate the input and calling the result table module.
	* Parameter -
	* Exception - exception, Char *
	* Return -
	*/
	void PLMPrintSearch::ClickedSubmitButton()
	{
		Logger::Info("PLMPrintSearch -> ClickedSubmitButton() -> Start");
		try
		{
			collectSearchUIFields();
			Logger::Info("PLMPrintSearch=================m_searchCriteriaJson "+to_string( m_searchCriteriaJson));

			UIHelper::ValidateForValidParams(m_searchCriteriaJson, PRINT_MODULE);
			Logger::Info("PLMPrintSearch=================(m_viewComboBox->currentIndex() " + to_string(m_viewComboBox->currentIndex()));
			PrintConfig::GetInstance()->SetSelectedViewIdx(m_viewComboBox->currentIndex());
			PrintConfig::GetInstance()->SetSearchCriteriaJSON(m_searchCriteriaJson);
			this->hide();
			//tmporarilry clearing Results object
			//..... need to implement result related funtionalities.
			UTILITY_API->CreateProgressBar();
			RESTAPI::SetProgressBarData(15, "Searching Print Design Color...", true);
			UTILITY_API->SetProgress("Searching Print Design Color...", (qrand() % 101));			
			PrintConfig::GetInstance()->SetDataFromResponse(PrintConfig::GetInstance()->GetSearchCriteriaJSON());
			PLMPrintResults::GetInstance()->setModal(true);
			if (PrintConfig::GetInstance()->GetIsModelExecuted() || Configuration::GetInstance()->GetCurrentScreen() == CREATE_PRODUCT_CLICKED || Configuration::GetInstance()->GetCurrentScreen() == UPDATE_PRODUCT_CLICKED)
			{
				PLMPrintResults::GetInstance()->currPageLabel->setText("1");
				PLMPrintResults::GetInstance()->DrawResultWidget(false);
			}			
			RESTAPI::SetProgressBarData(0, "", false);
			PLMPrintResults::GetInstance()->exec();
		}
		catch (string msg)
		{
			Logger::Error("PLMPrintSearch -> OnClickedSubmitButton Exception :: " + msg);
			RESTAPI::SetProgressBarData(0, "", false);
			UTILITY_API->DisplayMessageBox(msg);
			this->show();
		}
		catch (exception& e)
		{
			RESTAPI::SetProgressBarData(0, "", false);
			Logger::Error("PLMPrintSearch -> OnClickedSubmitButton Exception :: " + string(e.what()));
			UTILITY_API->DisplayMessageBox(e.what());
			this->show();
		}
		catch (const char* msg)
		{
			Logger::Error("PLMPrintSearch -> OnClickedSubmitButton Exception :: " + string(msg));
			wstring wstr(msg, msg + strlen(msg));
			RESTAPI::SetProgressBarData(0, "", false);
			UTILITY_API->DisplayMessageBoxW(wstr);
			this->show();
		}
		Logger::Info("PLMPrintSearch -> ClickedSubmitButton() -> End");
	}

	/*
	* Description - onClickedSubmitButton() method is a slot for submit button click, validate the input and calling the result table module.
	* Parameter - 
	* Exception - exception, Char *
	* Return -
	*/
	void PLMPrintSearch::onClickedSubmitButton()
	{
		Logger::Info("PLMPrintSearch -> onClickedSubmitButton() -> Start");
		ClickedSubmitButton();
		Logger::Info("PLMPrintSearch -> onClickedSubmitButton() -> End");
	}

	/*
	* Description - FilterChanged() method is a slot for filter item changed and update/create search UI fields.
	* Parameter -  QString.
	* Exception - exception, Char *
	* Return -
	*/
	void PLMPrintSearch::FilterChanged(const QString& _item)
	{
		Logger::Info("PLMPrintSearch -> FilterChanged() -> Start");
		try
		{
			if (!m_hierarchyLoading)
			{
				UTILITY_API->DeleteProgressBar(true);
				this->hide();
				UTILITY_API->CreateProgressBar();
				UTILITY_API->SetProgress("Loading", (qrand() % 101));
			}
			drawSearchUI(selectType, false, _item.toStdString());
			if (!m_hierarchyLoading) {
				UTILITY_API->DeleteProgressBar(false);
				this->show();
			}
		}
		catch (string msg)
		{
			Logger::Error("PLMPrintSearch -> FilterChanged Exception :: " + msg);
			UTILITY_API->DisplayMessageBox(msg);
			this->close();
		}
		catch (exception& e)
		{
			Logger::Error("PLMPrintSearch -> FilterChanged Exception :: " + string(e.what()));
			UTILITY_API->DisplayMessageBox(e.what());
			this->close();
		}
		catch (const char* msg)
		{
			Logger::Error("PLMPrintSearch -> FilterChanged Exception :: " + string(msg));
			wstring wstr(msg, msg + strlen(msg));
			UTILITY_API->DisplayMessageBoxW(wstr);
			this->close();
		}
		Logger::Info("PLMPrintSearch -> FilterChanged() -> End");
	}

	/*
	* Description - onFilterChanged() method is a slot for filter item changed and update/create search UI fields.
	* Parameter -  QString.
	* Exception - exception, Char *
	* Return -
	*/
	void PLMPrintSearch::onFilterChanged(const QString& _item)
	{
		Logger::Info("PLMPrintSearch -> onFilterChanged() -> Start");
		FilterChanged(_item);
		Logger::Info("PLMPrintSearch -> onFilterChanged() -> End");
	}

	/*
	* Description - CheckBoxChecked() method is a slot for check box  for search through season and palette.
	* Parameter -  bool.
	* Exception - exception, Char *
	* Return -
	*/
	void PLMPrintSearch::CheckBoxChecked(bool _checked)
	{
		Logger::Info("PLMPrintSearch -> CheckBoxChecked() -> Start");
		
		try
		{
			if (_checked)
			{
				UTILITY_API->DeleteProgressBar(true);
				this->hide();
				UTILITY_API->CreateProgressBar();
				Configuration::GetInstance()->SetProgressBarProgress(qrand() % 101);
				Configuration::GetInstance()->SetProgressBarProgress(RESTAPI::SetProgressBarProgress(Configuration::GetInstance()->GetProgressBarProgress(), 5, "Loading"));

				DataUtility::GetInstance()->SetProgressBarMsg("Loading");
				UIHelper::CheckBoxChecked(_checked, PrintConfig::GetInstance()->GetSeasonPaletteJSON(true), m_seasoncomboBox, m_paletteComboBox, m_seasonLabel, m_paletteLabel, PRINT_MODULE);
				this->show();
				UTILITY_API->DeleteProgressBar(true);
			}
			else
			{
				UIHelper::CheckBoxChecked(_checked, PrintConfig::GetInstance()->GetSeasonPaletteJSON(false), m_seasoncomboBox, m_paletteComboBox, m_seasonLabel, m_paletteLabel, PRINT_MODULE);
			}
		}
		catch (string msg)
		{
			Logger::Error("PLMPrintSearch -> CheckBoxChecked Exception :: " + msg);
			UTILITY_API->DisplayMessageBox(msg);
			m_paletteCheckBox->setCheckState(Qt::Unchecked);
			this->show();
			UTILITY_API->DeleteProgressBar(true);
		}
		catch (exception& e)
		{
			Logger::Error("PLMPrintSearch -> CheckBoxChecked Exception :: " + string(e.what()));
			UTILITY_API->DisplayMessageBox(e.what());
			m_paletteCheckBox->setCheckState(Qt::Unchecked);
			this->show();
			UTILITY_API->DeleteProgressBar(true);
		}
		catch (const char* msg)
		{
			Logger::Error("PLMPrintSearch -> CheckBoxChecked Exception :: " + string(msg));
			wstring wstr(msg, msg + strlen(msg));
			UTILITY_API->DisplayMessageBoxW(wstr);
			m_paletteCheckBox->setCheckState(Qt::Unchecked);
			this->show();
			UTILITY_API->DeleteProgressBar(true);
		}
		Logger::Info("PLMPrintSearch -> CheckBoxChecked() -> End");
	}

	/*
	* Description - onCheckBoxChecked() method is a slot for check box  for search through season and palette.
	* Parameter -  bool.
	* Exception - exception, Char *
	* Return -
	*/
	void PLMPrintSearch::onCheckBoxChecked(bool _checked)
	{
		Logger::Info("PLMPrintSearch -> onCheckBoxChecked() -> Start");
		CheckBoxChecked(_checked);
		Logger::Info("PLMPrintSearch -> onCheckBoxChecked() -> End");
	}

	/*
	* Description - SeasonChanged() method is a slot for season drop down is changed.
	* Parameter -  Qstring.
	* Exception - exception, Char *
	* Return -
	*/
	void PLMPrintSearch::SeasonChanged(const QString& _selectedSeason)
	{
		Logger::Info("PLMPrintSearch -> SeasonChanged() -> Start");
		try
		{
			UIHelper::SeasonChanged(_selectedSeason.toStdString(), PrintConfig::GetInstance()->GetSeasonPaletteJSON(false), m_paletteComboBox, m_paletteLabel);
		}
		catch (string msg)
		{
			Logger::Error("PLMPrintSearch -> SeasonChanged Exception :: " + msg);
			UTILITY_API->DisplayMessageBox(msg);
			m_paletteCheckBox->setCheckState(Qt::Unchecked);
		}
		catch (exception& e)
		{
			Logger::Error("PLMPrintSearch -> SeasonChanged Exception :: " + string(e.what()));
			UTILITY_API->DisplayMessageBox(e.what());
			m_paletteCheckBox->setCheckState(Qt::Unchecked);
		}
		catch (const char* msg)
		{
			Logger::Error("PLMPrintSearch -> SeasonChanged Exception :: " + string(msg));
			wstring wstr(msg, msg + strlen(msg));
			UTILITY_API->DisplayMessageBoxW(wstr);
			m_paletteCheckBox->setCheckState(Qt::Unchecked);
		}
		Logger::Info("PLMPrintSearch -> SeasonChanged() -> End");
	}

	/*
	* Description - SeasonChanged() method is a slot for season drop down is changed.
	* Parameter -  Qstring.
	* Exception - exception, Char *
	* Return -
	*/
	void PLMPrintSearch::onSeasonChanged(const QString& _selectedSeason)
	{
		Logger::Info("PLMPrintSearch -> onSeasonChanged() -> Start");
		SeasonChanged(_selectedSeason);
		Logger::Info("PLMPrintSearch -> onSeasonChanged() -> End");
	}

	/*
	* Description - TreeNodeClicked() method is a slot for hierarchy is click.
	* Parameter -  QTreeWidgetItem, int.
	* Exception - exception, Char *
	* Return -
	*/
	void PLMPrintSearch::TreeNodeClicked(QTreeWidgetItem* _item, int _column)
	{
		Logger::Info("PLMPrintSearch -> TreeNodeClicked() -> Start");
		m_hierarchyLoading = true;
		try
		{
			QString selectedRootDisplayName = _item->text(_column);
			if (!_item->isSelected())
			{
				UTILITY_API->DeleteProgressBar(true);
				this->hide();
				UTILITY_API->CreateProgressBar();
				m_hierarchyTreeWidget->clearSelection();
				_item->setSelected(true);
				selectType = _item->data(1, Qt::UserRole).toString();
				UTILITY_API->SetProgress("Loading", (qrand() % 101));
				CVWidgetGenerator::CreateViewComboBoxOnSearch(m_viewComboBox, PrintConfig::GetInstance()->GetPrintViewJSON(), selectType.toStdString());
				drawSearchUI(selectType, false, BLANK);
				m_hierarchyLoading = false;
				UTILITY_API->DeleteProgressBar(true);
				this->show();
			}
		}
		catch (string msg)
		{
			Logger::Error("PLMPrintSearch -> TreeNodeClicked Exception :: " + msg);
			UTILITY_API->DisplayMessageBox(msg);
			this->close();
		}
		catch (exception& e)
		{
			Logger::Error("PLMPrintSearch -> TreeNodeClicked Exception :: " + string(e.what()));
			UTILITY_API->DisplayMessageBox(e.what());
			this->close();
		}
		catch (const char* msg)
		{
			Logger::Error("PLMPrintSearch -> TreeNodeClicked Exception :: " + string(msg));
			wstring wstr(msg, msg + strlen(msg));
			UTILITY_API->DisplayMessageBoxW(wstr);
			this->close();
		}
		Logger::Info("PLMPrintSearch -> TreeNodeClicked() -> End");
	}

	/*
	* Description - TreeNodeClicked() method is a slot for hierarchy is click.
	* Parameter -  QTreeWidgetItem, int.
	* Exception - exception, Char *
	* Return -
	*/
	void PLMPrintSearch::onTreeNodeClicked(QTreeWidgetItem* _item, int _column)
	{
		Logger::Info("PLMPrintSearch -> onTreeNodeClicked() -> Start");
		TreeNodeClicked(_item, _column);
		Logger::Info("PLMPrintSearch -> onTreeNodeClicked() -> End");
	}

	/*
	* Description - DrawSearchUI() method used to create search fields in search UI.
	* Parameter -  QString, bool, string.
	* Exception - exception, Char *
	* Return -
	*/
	void PLMPrintSearch::drawSearchUI(QString _selectType, bool _drawFilter, string _selectedFilter)
	{		
		Logger::Info("PLMPrintSearch -> drawSearchUI() -> Start");
		try
		{
			PrintConfig::GetInstance()->SetDateFlag(false);
			QStringList attScops = PrintConfig::GetInstance()->GetAttScopes();
			json attributesJson = json::object();

			FlexTypeHelper::DrawDefaultSearchCriteriaWidget(PrintConfig::GetInstance()->GetPrintFieldsJSON(), _selectType.toStdString(), m_searchTreeWidget_1, m_searchTreeWidget_2, attScops);
		//}
			m_searchTreeWidget_1->setColumnCount(2);
			m_searchTreeWidget_1->setHeaderHidden(true);
			m_searchTreeWidget_1->setWordWrap(true);
			m_searchTreeWidget_1->setDropIndicatorShown(false);
			m_searchTreeWidget_1->setRootIsDecorated(false);
			m_searchTreeWidget_1->setSelectionMode(QAbstractItemView::NoSelection);
			m_searchTreeWidget_1->setStyleSheet("QTreeWidget { background-color: #262628; border: 1px solid #000; padding-left: 20px; padding-top: 10px; outline: 0; }""QTreeWidget::item {height: 20px; width: 200px; margin-right: 20px; margin-top: 5px; margin-bottom: 5px; border: none; }""QTreeWidget::item:hover{ background-color: #262628; }""QTreeView{outline: 0;}");
			m_searchTreeWidget_2->setColumnCount(2);
			m_searchTreeWidget_2->setHeaderHidden(true);
			m_searchTreeWidget_2->setWordWrap(true);
			m_searchTreeWidget_2->setDropIndicatorShown(false);
			m_searchTreeWidget_2->setRootIsDecorated(false);
			m_searchTreeWidget_2->setSelectionMode(QAbstractItemView::NoSelection);
			m_searchTreeWidget_2->setStyleSheet("QTreeWidget { background-color: #262628; border: 1px solid #000; padding-left: 20px; padding-top: 10px; outline: 0; }""QTreeWidget::item {height: 20px; width: 200px; margin-right: 20px; margin-top: 5px; margin-bottom: 5px; border: none; }""QTreeWidget::item:hover{ background-color: #262628; }""QTreeView{outline: 0;}");

			for (int searchFeildsCount = 0; searchFeildsCount < PrintConfig::GetInstance()->GetPrintFieldsJSON().size(); searchFeildsCount++)
			{
				json feildsJson = Helper::GetJSONParsedValue<int>(PrintConfig::GetInstance()->GetPrintFieldsJSON(), searchFeildsCount, false);
				for (int searchFeildsCount = 0; searchFeildsCount < feildsJson.size(); searchFeildsCount++)
					attributesJson = Helper::GetJSONParsedValue<string>(feildsJson, FILTER_ATTRIBUTES_KEY, false);
			}

			if (attributesJson.size() < 10)
				m_searchTreeWidget_2->setVisible(false);
			else
				m_searchTreeWidget_2->setVisible(true);

			if (m_searchTreeWidget_2->isVisible())
				this->setMinimumSize(850, 650); // this commented because, when two widget apear on the searchtable one widget overon anothere widget.

			//if (!MaterialConfig::GetInstance()->GetDateFlag())
			m_dateResetButton->hide();
			/*else
				m_dateResetButton->show();*/

		}
		catch (string msg)
		{
			Logger::Error("PLMPrintSearch -> drawSearchUI() Exception :: " + msg);		
			this->close();
			CLOViseSuite::GetInstance()->show();
		
		}
		catch (exception& e)
		{
			Logger::Error("PLMPrintSearch -> drawSearchUI() Exception :: " + string(e.what()));
			this->close();
			CLOViseSuite::GetInstance()->show();
			//throw e;
		}
		catch (const char* msg)
		{
			Logger::Error("PLMPrintSearch -> drawSearchUI() Exception :: " + string(msg));
			this->close();
			CLOViseSuite::GetInstance()->show();
			//throw msg;
		}
		Logger::Info("PLMPrintSearch -> drawSearchUI() -> End");
	}

	/*
	* Description - CollectSearchUIFields() method used collect all the user input from search UI.
	* Parameter - 
	* Exception - 
	* Return -
	*/
	void  PLMPrintSearch::collectSearchUIFields()
	{
		Logger::Info("PLMPrintSearch -> collectSearchUIFields() -> Start");
		//UTILITY_API->DisplayMessageBox("collectSearchUIFields()::" + to_string(m_searchCriteriaJson));
		try
		{
			m_searchCriteriaJson.clear();
			string seasonValue = "";
			string paletteValue = "";
			string quickSearchValue = "";
			string filterValue = "";
			if (m_paletteCheckBox->checkState())
			{
				seasonValue = m_seasoncomboBox->currentText().toStdString();
				paletteValue = m_paletteComboBox->currentText().toStdString();
				UIHelper::SeasonPaletteValidation(seasonValue, paletteValue);

				seasonValue = m_seasoncomboBox->property(seasonValue.c_str()).toString().toStdString();
				paletteValue = m_paletteComboBox->property(paletteValue.c_str()).toString().toStdString();
				m_searchCriteriaJson[SEASONID_KEY] = seasonValue;
				m_searchCriteriaJson[PALETTEID_KEY] = paletteValue;
			}
			else
			{
				m_searchCriteriaJson[SEASONID_KEY] = "";
				m_searchCriteriaJson[PALETTEID_KEY] = "";
			}
			quickSearchValue = m_quickSearchLineEdit->text().toStdString();
			if (FormatHelper::HasContent(quickSearchValue))
			{
				m_searchCriteriaJson[QUICKSEARCH_KEY] = quickSearchValue;
			}
			else
			{
				m_searchCriteriaJson[QUICKSEARCH_KEY] = "";
			}

			filterValue = m_filterComboBox->currentText().toStdString();
			m_searchCriteriaJson[FILTER_ID_KEY] = m_filterComboBox->property(filterValue.c_str()).toString().toStdString();
			string tempType = selectType.toStdString();
			tempType = FormatHelper::ReplaceString(tempType, "\\\\", "\\");
			m_searchCriteriaJson[FLEX_TYPE_STRING_KEY] = tempType;
			m_searchCriteriaJson[MODULE_KEY] = PRINT_MODULE;
			m_searchCriteriaJson[INCLUDE_SUPPLIER_STRING_KEY] = "";
			m_searchCriteriaJson[SEARCHBY_SUPPLIER_STRING_KEY] = "";
			m_searchCriteriaJson[REQ_FROM_CLO_KEY] = "true";
			m_searchCriteriaJson[ATTRIBUTES_KEY] = UIHelper::CollectSearchCriteriaFields(m_searchTreeWidget_1, m_searchTreeWidget_2);
			Logger::Info("PLMPrintSearch -> collectSearchUIFields() -> End");
		}
		catch (string msg)
		{
			Logger::Error("PLMPrintSearch::collectSearchUIFields() Exception - " + msg);
		}
		catch (exception & e)
		{
			Logger::Error("PLMPrintSearch::collectSearchUIFields() Exception - " + string(e.what()));
		}
		catch (const char* msg)
		{
			wstring wstr(msg, msg + strlen(msg));
			Logger::Error("PLMPrintSearch::collectSearchUIFields() Exception - " + string(msg));
		}
	}

	/*
	* Description - reject() method is a slot for close button.
	* Parameter - 
	* Exception - 
	* Return -
	*/
	void PLMPrintSearch::reject()
	{
		Configuration::GetInstance()->SetIsPrintSearchClicked(false);
		this->accept();
	}

	/*
	* Description - GetTreewidget() method is for pass a treeWidget.
	* Parameter - int 
	* Exception -
	* Return -QTreeWidget
	*/
	QTreeWidget* PLMPrintSearch::GetTreewidget(int _index)
	{
		if (_index == 0)
			return m_searchTreeWidget_1;
		if (_index == 1)
			return m_searchTreeWidget_2;

		return m_searchTreeWidget_1;
	}

	/*
	* Description - DrawSearchWidget() method is create/reset the search widget.
	* Parameter -
	* Exception -
	* Return -
	*/
	void PLMPrintSearch::DrawSearchWidget(bool _isFromConstructor)
	{
		Logger::Info("PLMPrintSearch -> DrawSearchWidget() -> Start");
		if (!_isFromConstructor)
		{
			UIHelper::ClearAllFieldsForSearch(m_searchTreeWidget_1);
			UIHelper::ClearAllFieldsForSearch(m_searchTreeWidget_2);
		}
		else
		{
			selectType = QString::fromStdString(PRINT_ROOT_TYPE);
			if (PrintConfig::GetInstance()->GetIsModelExecuted())
			{
				Configuration::GetInstance()->SetProgressBarProgress(RESTAPI::SetProgressBarProgress(Configuration::GetInstance()->GetProgressBarProgress(), 8, "Loading Print Design Color Search..."));
			}
			drawSearchUI(selectType, false, BLANK);
		}
		Logger::Info("PLMPrintSearch -> DrawSearchWidget() -> End");
	}
}
