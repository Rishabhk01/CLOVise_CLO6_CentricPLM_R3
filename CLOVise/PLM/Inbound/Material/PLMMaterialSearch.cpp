/*
* Copyright 2020-2021 CLO-Vise. All rights reserved
*
* @file PLMMaterialSearch.cpp
*
* @brief Class implementation for accessing Material in CLO from PLM.
* This class has all the variable and methods implementation which are used in PLM Material Search.
*
* @author GoVise
*
* @date 16-JUN-2020
*/
#include "PLMMaterialSearch.h"

#include <iostream>
#include <string>
#include <time.h>

#include <qglobal.h>

#include "CLOVise/PLM/Inbound/Material/MaterialConfig.h"
#include "CLOVise/PLM/Helper/Util/Helper.h"
#include "CLOVise/PLM/Helper/UIHelper/UIHelper.h"
#include "CLOVise/PLM/Helper/Util/RestAPIUtility.h"
#include "CLOVise/PLM/Helper/Util/FormatHelper.h"
#include "CLOVise/PLM/Helper/Util/FlexTypeHelper.h"
#include "CLOVise/PLM/Inbound/Material/PLMMaterialResult.h"
#include "CLOVise/PLM/Helper/UIHelper/CVWidgetGenerator.h"
#include "CLOVise/PLM/CLOViseSuite/CLOViseSuite.h"
#include "CLOVise/PLM/Helper/Util/Logger.h"
#include"classes/widgets/MVDialog.h"
#include "CLOVise/PLM/Outbound/PublishToPLM/CreateProduct.h"
#include "CLOVise/PLM/Outbound/PublishToPLM/UpdateProduct.h"

using namespace std;

namespace CLOVise
{
	PLMMaterialSearch* PLMMaterialSearch::_instance = NULL;
	
	PLMMaterialSearch* PLMMaterialSearch::GetInstance()
	{
		if (_instance == NULL)
			_instance = new PLMMaterialSearch();
		return _instance;
	}
	
	PLMMaterialSearch::PLMMaterialSearch(QWidget* parent) 
		: MVDialog(parent)
	{
		Logger::Info("PLMMaterialSearch -> Constructor() -> Start");
		
		setupUi(this);

		QString windowTitle = PLM_NAME + " PLM "+ QString::fromStdString(Configuration::GetInstance()->GetLocalizedMaterialClassName()) +" Search";
		this->setWindowTitle(windowTitle);

#ifdef __APPLE__
		this->setWindowFlags(Qt::Dialog | Qt::CustomizeWindowHint | Qt::WindowMinMaxButtonsHint | Qt::WindowCloseButtonHint);
#else
		this->setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint | Qt::CustomizeWindowHint);
		m_pTitleBar = new MVTitleBar(windowTitle, this);
		layout()->setMenuBar(m_pTitleBar);
#endif // !__APPLE__

		m_hierarchyTreeWidget = CVWidgetGenerator::CreateHirarchyTreeWidget("QTreeWidget{min-height: 230px; min-width: 50px; max-height: 1500px; max-width: 230px; border: 1px solid #000; font-family: ArialMT; font-size: 10px; background-color: #262628; margin-bottom: 20px;} QTreeWidget:item:selected{ color: #46C8FF; }", false);
		m_hierarchyTreeWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
		//m_hierarchyTreeWidget->hide();
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
		m_searchButton = CVWidgetGenerator::CreatePushButton("Search", SEARCH_HOVER_ICON_PATH, "Search", PUSH_BUTTON_STYLE, 30, true);
		m_searchButton->setFocusPolicy(Qt::StrongFocus);
#ifdef __APPLE__
		ui_paletteLabelLayout->setSpacing(10);
#endif

		this->setStyleSheet("QDialog{height:539px; width: 1050px;}");
		ui_paletteSearchWidget->setStyleSheet("#ui_paletteSearchWidget{padding: 10px; width: 920px;  border: 1px solid #000; background-color: #262628; max-height:50px;}");
		ui_quickSearchWidget->setStyleSheet("#ui_quickSearchWidget{padding: 10px; width: 920px;border: 1px solid #000; background-color: #262628; max-height:42px;}");

		m_flextypeTree.clear();
		//selectType = FlexTypeHelper::CreateFlexTypeTree(MaterialConfig::GetInstance()->GetMaterialHierarchyJSON(), m_hierarchyTreeWidget, m_flextypeTree);
		//RESTAPI::SetProgressBarData(70, "Loading Material configuration", true);
		//m_hierarchyTreeWidget->topLevelItem(0)->setSelected(true);
	//	m_hierarchyTreeWidget->expandAll();
		//m_hierarchyTreeWidget->setExpandsOnDoubleClick(false);
		//m_hierarchyTreeWidget->setSortingEnabled(true);
		//m_hierarchyTreeWidget->sortByColumn(0, Qt::AscendingOrder);
	//	m_hierarchyTreeWidget->setFocusPolicy(Qt::NoFocus);

		onHideHirarchy(false);
	//	CVWidgetGenerator::CreateViewComboBoxOnSearch(m_viewComboBox, MaterialConfig::GetInstance()->GetMaterialViewJSON(), selectType.toStdString());
		DrawSearchWidget(true);

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

		ui_heirarchyLayout->addWidget(m_hierarchyTreeWidget);

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
		ui_quickSearchWidget->hide();

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
		m_dateResetButton->hide();
		Logger::Info("PLMMaterialSearch -> Constructor() -> End");
		UTILITY_API->DeleteProgressBar(true);
	}

	/*
	* Description - HideHirarchy() method used to show/hide the hierarchy.
	* Parameter -  bool.
	* Exception -
	* Return -
	*/
	void PLMMaterialSearch::HideHirarchy(bool _hide)
	{
		Logger::Info("PLMMaterialSearch -> HideHirarchy() -> Start");
		if (!_hide)
		{
			if (!m_isHidden)
			{
				ui_hideAndShowButton->setToolTip("Show");
				m_hierarchyTreeWidget->hide();
				ui_hideAndShowButton->setStyleSheet("#ui_hideAndShowButton {\n" "qproperty-icon: none; \n""image: url(:/CLOVise/PLM/Images/icon_open_tree_none.svg);\n""}\n"
					"#ui_hideAndShowButton:hover{\n""image: url(:/CLOVise/PLM/Images/icon_open_tree_over.svg);\n""}""QToolTip{ color: #46C8FF; background-color: #33414D; border: 1px #000000; }");
				m_isHidden = true;
			}
			else
			{
				ui_hideAndShowButton->setToolTip("Hide");
				m_hierarchyTreeWidget->show();
				ui_hideAndShowButton->setStyleSheet("#ui_hideAndShowButton {\n" "qproperty-icon: none; \n""image: url(:/CLOVise/PLM/Images/icon_close_tree_none.svg);\n""}\n"
					"#ui_hideAndShowButton:hover{\n""image: url(:/CLOVise/PLM/Images/icon_close_tree_over.svg);\n""}""QToolTip{ color: #46C8FF; background-color: #33414D; border: 1px #000000; }");
				m_isHidden = false;
			}
		}
		Logger::Info("PLMMaterialSearch -> HideHirarchy() -> End");
	}
	/*
	* Description - HideHirarchy() method used to show/hide the hierarchy.
	* Parameter -  bool.
	* Exception -
	* Return -
	*/
	void PLMMaterialSearch::onHideHirarchy(bool _hide)
	{
		Logger::Info("PLMMaterialSearch -> onHideHirarchy() -> Start");
		HideHirarchy(_hide);
		Logger::Info("PLMMaterialSearch -> onHideHirarchy() -> End");
	}

	PLMMaterialSearch::~PLMMaterialSearch()
	{
		DisconnectSignalSlots();
		//MaterialConfig::GetInstance()->m_materialFieldsJson.clear();
	}

	/*
	* Description - ResetDateEditWidget() method is a slot for reset date button click and used to reset the date fields.
	* Parameter -
	* Exception -
	* Return -
	*/
	void PLMMaterialSearch::ResetDateEditWidget()
	{
		Logger::Info("PLMMaterialSearch -> ResetDateEditWidget() -> Start");
		UIHelper::ResetDate(m_searchTreeWidget_1);
		UIHelper::ResetDate(m_searchTreeWidget_2);
		Logger::Info("PLMMaterialSearch -> ResetDateEditWidget() -> End");
	}

	/*
	* Description - ResetDateEditWidget() method is a slot for reset date button click and used to reset the date fields.
	* Parameter -
	* Exception -
	* Return -
	*/
	void PLMMaterialSearch::onResetDateEditWidget()
	{
		Logger::Info("PLMMaterialSearch -> onResetDateEditWidget() -> Start");
		ResetDateEditWidget();
		Logger::Info("PLMMaterialSearch -> onResetDateEditWidget() -> End");
	}

	/*
	* Description - connectSignalSlots() method used to connect and disconnect the signal and slot.
	* Parameter -   bool.
	* Exception -
	* Return -
	*/
	void PLMMaterialSearch::connectSignalSlots(bool _b)
	{
		Logger::Info("PLMMaterialSearch -> connectSignalSlots() -> Start");
		if (_b)
		{
			QObject::connect(ui_hideAndShowButton, SIGNAL(clicked(bool)), this, SLOT(onHideHirarchy(bool)));
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
			QObject::disconnect(ui_hideAndShowButton, SIGNAL(clicked()), this, SLOT(onHideHirarchy(bool)));
			QObject::disconnect(m_hierarchyTreeWidget, SIGNAL(itemClicked(QTreeWidgetItem*, int)), this, SLOT(onTreeNodeClicked(QTreeWidgetItem*, int)));
			QObject::disconnect(m_searchButton, SIGNAL(clicked()), this, SLOT(onClickedSubmitButton())); // should be used
			QObject::disconnect(m_backButton, SIGNAL(clicked()), this, SLOT(onClickedBackButton()));
			QObject::disconnect(m_paletteCheckBox, SIGNAL(toggled(bool)), this, SLOT(onCheckBoxChecked(bool)));
			QObject::disconnect(m_filterComboBox, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(onFilterChanged(const QString&)));
			QObject::disconnect(m_seasoncomboBox, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(onSeasonChanged(const QString&)));
			QObject::disconnect(m_dateResetButton, SIGNAL(clicked()), this, SLOT(onResetDateEditWidget()));
		}
		Logger::Info("PLMMaterialSearch -> connectSignalSlots() -> End");
	}

	/*
	* Description - ClickedBackButton() method is a slot for back button click and used to go back design suite.
	* Parameter -
	* Exception -
	* Return -
	*/
	void PLMMaterialSearch::ClickedBackButton()
	{
		this->hide();
		Logger::Info("PLMMaterialSearch -> ClickedBackButton() -> Start");
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
		case SEARCH_MATERIAL_CLICKED:
		{
			CLOVise::CLOViseSuite::GetInstance()->setModal(true);
			CLOViseSuite::GetInstance()->show();
		}
		}
		UIHelper::ClearAllFieldsForSearch(m_searchTreeWidget_1);
		UIHelper::ClearAllFieldsForSearch(m_searchTreeWidget_2);
		Logger::Info("PLMMaterialSearch -> ClickedBackButton() -> End");
	}


	/*
	* Description - OnClickedBackButton() method is a slot for back button click and used to go back design suite.
	* Parameter -
	* Exception -
	* Return -
	*/
	void PLMMaterialSearch::onClickedBackButton()
	{
		Logger::Info("PLMMaterialSearch -> onClickedBackButton() -> Start");
		ClickedBackButton();
		Logger::Info("PLMMaterialSearch -> onClickedBackButton() -> End");
	}

	/*
	* Description - ClickedSubmitButton() method is a slot for submit button click, validate the input and calling the result table module.
	* Parameter -
	* Exception - exception, Char *
	* Return -
	*/
	void PLMMaterialSearch::ClickedSubmitButton()
	{
		auto startTime = std::chrono::high_resolution_clock::now();
		Logger::Info("PLMMaterialSearch -> ClickedSubmitButton() -> Start");
		try
		{
			collectSearchUIFields();
			UIHelper::ValidateForValidParams(m_searchCriteriaJson, MATERIAL_MODULE);
			MaterialConfig::GetInstance()->SetSelectedViewIdx(m_viewComboBox->currentIndex());
			MaterialConfig::GetInstance()->SetSearchCriteriaJSON(m_searchCriteriaJson);
			this->hide();
			//tmporarilry clearing Results object
			//..... need to implement result related funtionalities.
			UTILITY_API->CreateProgressBar();
			RESTAPI::SetProgressBarData(15, "Searching "+ Configuration::GetInstance()->GetLocalizedMaterialClassName(), true);
			UTILITY_API->SetProgress("Searching "+ Configuration::GetInstance()->GetLocalizedMaterialClassName(), (qrand() % 101));
			//RESTAPI::SetProgressBarData(15, "Searching Materials", true);
			//PLMMaterialResult::Destroy();
			Logger::Debug("preparing the response value started...");
			MaterialConfig::GetInstance()->SetDataFromResponse(MaterialConfig::GetInstance()->GetSearchCriteriaJSON());
			Logger::Debug("preparing the response value ended..."); 
			
			PLMMaterialResult::GetInstance()->setModal(true);
			if (MaterialConfig::GetInstance()->GetIsModelExecuted() || Configuration::GetInstance()->GetCurrentScreen() == UPDATE_MATERIAL_CLICKED || MaterialConfig::GetInstance()->m_resultAfterLogout)
			{
				PLMMaterialResult::GetInstance()->currPageLabel->setText("1");
				PLMMaterialResult::GetInstance()->DrawResultWidget(false);
				MaterialConfig::GetInstance()->m_resultAfterLogout = false;
			}
			RESTAPI::SetProgressBarData(0, "", false);
			PLMMaterialResult::GetInstance()->exec();			
			auto finishTime = std::chrono::high_resolution_clock::now();
			std::chrono::duration<double> totalDuration = finishTime - startTime;
			Logger::perfomance(PERFOMANCE_KEY + "Time For Download Material :: " + to_string(totalDuration.count()));
		}
		catch (string msg)
		{
			Logger::Error("PLMMaterialSearch -> OnClickedSubmitButton Exception :: " + msg);
			RESTAPI::SetProgressBarData(0, "", false);
			UTILITY_API->DisplayMessageBox(msg);
			this->show();
		}
		catch (exception& e)
		{
			RESTAPI::SetProgressBarData(0, "", false);
			Logger::Error("PLMMaterialSearch -> OnClickedSubmitButton Exception :: " + string(e.what()));
			UTILITY_API->DisplayMessageBox(e.what());
			this->show();
		}
		catch (const char* msg)
		{
			Logger::Error("PLMMaterialSearch -> OnClickedSubmitButton Exception :: " + string(msg));
			wstring wstr(msg, msg + strlen(msg));
			RESTAPI::SetProgressBarData(0, "", false);
			UTILITY_API->DisplayMessageBoxW(wstr);
			this->show();
		}
		Logger::Info("PLMMaterialSearch -> ClickedSubmitButton() -> End");
	}

	/*
	* Description - OnClickedSubmitButton() method is a slot for submit button click, validate the input and calling the result table module.
	* Parameter -
	* Exception - exception, Char *
	* Return -
	*/
	void PLMMaterialSearch::onClickedSubmitButton()
	{
		Logger::Logger("PLMMaterialSearch -> onClickedSubmitButton() -> Start....");
		ClickedSubmitButton();
		Logger::Logger("PLMMaterialSearch -> onClickedSubmitButton() -> End...");
	}

	/*
	* Description - FilterChanged() method is a slot for filter item changed and update/create search UI fields.
	* Parameter -  QString.
	* Exception - exception, Char *
	* Return -
	*/
	void PLMMaterialSearch::FilterChanged(const QString& _item)
	{
		Logger::Info("PLMMaterialSearch -> FilterChanged() -> Start");
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
			Logger::Error("PLMMaterialSearch -> FilterChanged Exception :: " + msg);
			UTILITY_API->DisplayMessageBox(msg);
			this->close();
		}
		catch (exception& e)
		{
			Logger::Error("PLMMaterialSearch -> FilterChanged Exception :: " + string(e.what()));
			UTILITY_API->DisplayMessageBox(e.what());
			this->close();
		}
		catch (const char* msg)
		{
			Logger::Error("PLMMaterialSearch -> FilterChanged Exception :: " + string(msg));
			wstring wstr(msg, msg + strlen(msg));
			UTILITY_API->DisplayMessageBoxW(wstr);
			this->close();
		}
		Logger::Info("PLMMaterialSearch -> FilterChanged() -> End");
	}

	/*
	* Description - FilterChanged() method is a slot for filter item changed and update/create search UI fields.
	* Parameter -  QString.
	* Exception - exception, Char *
	* Return -
	*/
	void PLMMaterialSearch::onFilterChanged(const QString& _item)
	{
		Logger::Info("PLMMaterialSearch -> onFilterChanged() -> Start");
		FilterChanged(_item);
		Logger::Info("PLMMaterialSearch -> onFilterChanged() -> End");
	}

	/*
	* Description - CheckBoxChecked() method is a slot for check box  for search through season and palette.
	* Parameter -  bool.
	* Exception - exception, Char *
	* Return -
	*/
	void PLMMaterialSearch::CheckBoxChecked(bool _checked)
	{
		Logger::Info("PLMMaterialSearch -> CheckBoxChecked() -> Start");
		try
		{
			if (_checked)
			{
				UIHelper::CheckBoxChecked(_checked, MaterialConfig::GetInstance()->GetSeasonPaletteJSON(true), m_seasoncomboBox, m_paletteComboBox, m_seasonLabel, m_paletteLabel, MATERIAL_MODULE);
			}
			else
			{
				UIHelper::CheckBoxChecked(_checked, MaterialConfig::GetInstance()->GetSeasonPaletteJSON(false), m_seasoncomboBox, m_paletteComboBox, m_seasonLabel, m_paletteLabel, MATERIAL_MODULE);
			}
		}
		catch (string msg)
		{
			Logger::Error("PLMMaterialSearch -> CheckBoxChecked Exception :: " + msg);
			UTILITY_API->DisplayMessageBox(msg);
			m_paletteCheckBox->setCheckState(Qt::Unchecked);
		}
		catch (exception& e)
		{
			Logger::Error("PLMMaterialSearch -> CheckBoxChecked Exception :: " + string(e.what()));
			UTILITY_API->DisplayMessageBox(e.what());
			m_paletteCheckBox->setCheckState(Qt::Unchecked);
		}
		catch (const char* msg)
		{
			Logger::Error("PLMMaterialSearch -> CheckBoxChecked Exception :: " + string(msg));
			wstring wstr(msg, msg + strlen(msg));
			UTILITY_API->DisplayMessageBoxW(wstr);
			m_paletteCheckBox->setCheckState(Qt::Unchecked);
		}
		Logger::Info("PLMMaterialSearch -> CheckBoxChecked() -> End");
	}

	/*
	* Description - CheckBoxChecked() method is a slot for check box  for search through season and palette.
	* Parameter -  bool.
	* Exception - exception, Char *
	* Return -
	*/
	void PLMMaterialSearch::onCheckBoxChecked(bool _checked)
	{
		Logger::Info("PLMMaterialSearch -> onCheckBoxChecked() -> Start");
		CheckBoxChecked(_checked);
		Logger::Info("PLMMaterialSearch -> onCheckBoxChecked() -> End");
	}

	/*
	* Description - SeasonChanged() method is a slot for season drop down is changed.
	* Parameter -  Qstring.
	* Exception - exception, Char *
	* Return -
	*/
	void PLMMaterialSearch::SeasonChanged(const QString& _selectedSeason)
	{
		Logger::Info("PLMMaterialSearch -> SeasonChanged() -> Start");
		try
		{
			UIHelper::SeasonChanged(_selectedSeason.toStdString(), MaterialConfig::GetInstance()->GetSeasonPaletteJSON(false), m_paletteComboBox, m_paletteLabel);
		}
		catch (string msg)
		{
			Logger::Error("PLMMaterialSearch -> SeasonChanged Exception :: " + msg);
			UTILITY_API->DisplayMessageBox(msg);
			m_paletteCheckBox->setCheckState(Qt::Unchecked);
		}
		catch (exception& e)
		{
			Logger::Error("PLMMaterialSearch -> SeasonChanged Exception :: " + string(e.what()));
			UTILITY_API->DisplayMessageBox(e.what());
			m_paletteCheckBox->setCheckState(Qt::Unchecked);
		}
		catch (const char* msg)
		{
			Logger::Error("PLMMaterialSearch -> SeasonChanged Exception :: " + string(msg));
			wstring wstr(msg, msg + strlen(msg));
			UTILITY_API->DisplayMessageBoxW(wstr);
			m_paletteCheckBox->setCheckState(Qt::Unchecked);
		}
		Logger::Info("PLMMaterialSearch -> SeasonChanged() -> End");
	}

	/*
	* Description - SeasonChanged() method is a slot for season drop down is changed.
	* Parameter -  Qstring.
	* Exception - exception, Char *
	* Return -
	*/
	void PLMMaterialSearch::onSeasonChanged(const QString& _selectedSeason)
	{
		Logger::Info("PLMMaterialSearch -> onSeasonChanged() -> Start");
		SeasonChanged(_selectedSeason);
		Logger::Info("PLMMaterialSearch -> onSeasonChanged() -> End");
	}

	/*
	* Description - TreeNodeClicked() method is a slot for hierarchy is click.
	* Parameter -  QTreeWidgetItem, int.
	* Exception - exception, Char *
	* Return -
	*/
	void PLMMaterialSearch::TreeNodeClicked(QTreeWidgetItem* _item, int _column)
	{
		Logger::Info("PLMMaterialSearch -> TreeNodeClicked() -> Start");
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
				CVWidgetGenerator::CreateViewComboBoxOnSearch(m_viewComboBox, MaterialConfig::GetInstance()->GetMaterialViewJSON(), selectType.toStdString());
				drawSearchUI(selectType, false, BLANK);
				m_hierarchyLoading = false;
				UTILITY_API->DeleteProgressBar(true);
				this->show();
			}
		}
		catch (string msg)
		{
			Logger::Error("PLMMaterialSearch -> TreeNodeClicked Exception :: " + msg);
			UTILITY_API->DisplayMessageBox(msg);
			this->close();
		}
		catch (exception& e)
		{
			Logger::Error("PLMMaterialSearch -> TreeNodeClicked Exception :: " + string(e.what()));
			UTILITY_API->DisplayMessageBox(e.what());
			this->close();
		}
		catch (const char* msg)
		{
			Logger::Error("PLMMaterialSearch -> TreeNodeClicked Exception :: " + string(msg));
			wstring wstr(msg, msg + strlen(msg));
			UTILITY_API->DisplayMessageBoxW(wstr);
			this->close();
		}
		Logger::Info("PLMMaterialSearch -> TreeNodeClicked() -> End");
	}

	/*
	* Description - TreeNodeClicked() method is a slot for hierarchy is click.
	* Parameter -  QTreeWidgetItem, int.
	* Exception -
	* Return -
	*/
	void PLMMaterialSearch::onTreeNodeClicked(QTreeWidgetItem* _item, int _column)
	{
		Logger::Info("PLMMaterialSearch -> onTreeNodeClicked() -> Start");
		TreeNodeClicked(_item, _column);
		Logger::Info("PLMMaterialSearch -> onTreeNodeClicked() -> End");
	}

	/*
	* Description - DrawSearchUI() method used to create search fields in search UI.
	* Parameter -  QString, bool, string.
	* Exception - exception, Char *
	* Return -
	*/
	void PLMMaterialSearch::drawSearchUI(QString _selectType, bool _drawFilter, string _selectedFilter)
	{
		Logger::Info("PLMMaterialSearch -> drawSearchUI() -> Start");
		try
		{
			MaterialConfig::GetInstance()->SetDateFlag(false);
			QStringList attScops = MaterialConfig::GetInstance()->GetAttScopes();
			
			if (MaterialConfig::GetInstance()->GetIsModelExecuted())
				Configuration::GetInstance()->SetProgressBarProgress(RESTAPI::SetProgressBarProgress(Configuration::GetInstance()->GetProgressBarProgress(), 10, "Loading Material Search"));
			/*if (!CVWidgetGenerator::DrawFilterAndSearchCriteriaWidget(MaterialConfig::GetInstance()->GetMaterialFilterJSON(), m_filterComboBox, m_searchTreeWidget, _selectType, _selectedFilter, attScops, _drawFilter))
			{*/
				//FlexTypeHelper::DrawDefaultSearchCriteriaWidget(m_materialFieldsJson, _selectType.toStdString(), m_searchTreeWidget, attScops);
				FlexTypeHelper::DrawDefaultSearchCriteriaWidget(MaterialConfig::GetInstance()->GetMaterialFieldsJSON(), _selectType.toStdString(), m_searchTreeWidget_1, m_searchTreeWidget_2, attScops);
				//FlexTypeHelper::DrawDefaultSearchCriteriaWidget(MaterialConfig::GetInstance()->GetMaterialFieldsJSON(), _selectType.toStdString(), m_searchTreeWidget_2, attScops);
			/*}*/
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

			/*if (!m_searchTreeWidget_2->isHidden())
				this->setMinimumSize(850, 650);*/ // this commented because, when two widget apear on the searchtable one widget overon anothere widget.

			//if (!MaterialConfig::GetInstance()->GetDateFlag())
				m_dateResetButton->hide();
			/*else
				m_dateResetButton->show();*/
		}
		catch (string msg)
		{
			Logger::Error("PLMColorSearch -> drawSearchUI() Exception :: " + msg);
			this->close();
			CLOViseSuite::GetInstance()->show();
		}
		catch (exception& e)
		{
			Logger::Error("PLMMaterialSearch -> drawSearchUI() Exception :: " + string(e.what()));
			this->close();
			CLOViseSuite::GetInstance()->show();
		}
		catch (const char* msg)
		{
			Logger::Error("PLMMaterialSearch -> drawSearchUI() Exception :: " + string(msg));
			this->close();
			CLOViseSuite::GetInstance()->show();
		}
		Logger::Info("PLMMaterialSearch -> drawSearchUI() -> End");
	}

	/*
	* Description - CollectSearchUIFields() method used collect all the user input from search UI.
	* Parameter -
	* Exception -
	* Return -
	*/
	void  PLMMaterialSearch::collectSearchUIFields()
	{
		Logger::Info("PLMMaterialSearch -> collectSearchUIFields() -> Start");
		//string seasonValue = "";
		//string paletteValue = "";
		//string quickSearchValue = "";
		//string filterValue = "";
		m_searchCriteriaJson.clear();
		/*if (m_paletteCheckBox->checkState())
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
		m_searchCriteriaJson[MODULE_KEY] = MATERIAL_MODULE;
		m_searchCriteriaJson[INCLUDE_SUPPLIER_STRING_KEY] = "";
		m_searchCriteriaJson[SEARCHBY_SUPPLIER_STRING_KEY] = "";
		m_searchCriteriaJson[REQ_FROM_CLO_KEY] = "true";*/
		m_searchCriteriaJson[ATTRIBUTES_KEY] = UIHelper::CollectSearchCriteriaFields(m_searchTreeWidget_1, m_searchTreeWidget_2);
		//UTILITY_API->DisplayMessageBox("m_searchCriteriaJson:: " + to_string(m_searchCriteriaJson));
		Logger::Info("PLMMaterialSearch -> collectSearchUIFields() -> End");
	}

	/*
	* Description - ClearAllFields() method is used to clear all user entered data on the search screen.
	* Parameter -
	* Exception -
	* Return -
	*/
	void PLMMaterialSearch::ClearAllFields()
	{
		Logger::Info("PLMMaterialSearch -> ClearAllFields() -> Start");
		UIHelper::ClearAllFieldsForSearch(m_searchTreeWidget_1);
		UIHelper::ClearAllFieldsForSearch(m_searchTreeWidget_2);
		Logger::Info("PLMMaterialSearch -> ClearAllFields() -> End");
	}

	/*
	* Description - reject() method is a slot for close button.
	* Parameter - QString
	* Exception - try catch.
	* Return -
	*/
	void PLMMaterialSearch::reject()
	{
		this->accept();
	}

	/*
	* Description - GetTreewidget() method is for pass a treeWidget.
	* Parameter -
	* Exception -
	* Return -QTreeWidget
	*/
	QTreeWidget* PLMMaterialSearch::GetTreewidget(int _index)
	{
		if (_index == 0)
			return m_searchTreeWidget_1;
		if (_index == 1)
			return m_searchTreeWidget_2;

		return m_searchTreeWidget_1;
	}

	/*
	* Description - DrawSearchWidget() method is create/reset the search widget.
	* Parameter - bool
	* Exception -
	* Return -
	*/
	void PLMMaterialSearch::DrawSearchWidget(bool _isFromConstructor)
	{
		Logger::Info("PLMMaterialSearch -> DrawSearchWidget() -> Start");
		if (!_isFromConstructor)
			ClearAllFields();
		else
		{			
			//CVWidgetGenerator::CreateViewComboBoxOnSearch(m_viewComboBox, ColorConfig::GetInstance()->GetColorViewJSON(), selectType.toStdString());
			selectType = QString::fromStdString(COLOR_ROOT_TYPE);
			if (MaterialConfig::GetInstance()->GetIsModelExecuted())
			{
				Configuration::GetInstance()->SetProgressBarProgress(RESTAPI::SetProgressBarProgress(Configuration::GetInstance()->GetProgressBarProgress(), 8, "Loading " + Configuration::GetInstance()->GetLocalizedColorClassName() + " Search"));
			}
			drawSearchUI(selectType, false, BLANK);
		}
		Logger::Info("PLMMaterialSearch -> DrawSearchWidget() -> End");
	}
}
