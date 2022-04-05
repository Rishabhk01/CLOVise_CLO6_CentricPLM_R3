/*
* Copyright 2020-2021 CLO-Vise. All rights reserved
*
* @file PLMShapeSearch.cpp
*
* @brief Class implementation for accessing Shape in CLO from PLM.
* This class has all the variable and methods implementation which are used in PLM Shape Search.
*
* @author GoVise
*
* @date 28-JUL-2020
*/
#include "PLMShapeSearch.h"

#include <iostream>
#include <string>

#include <qglobal.h>

#include "CLOVise/PLM/Inbound/Shape/ShapeConfig.h"
#include "CLOVise/PLM/Inbound/Shape/PLMShapeResults.h"
#include "CLOVise/PLM/Helper/Util/Helper.h"
#include "CLOVise/PLM/Helper/UIHelper/UIHelper.h"
#include "CLOVise/PLM/Helper/Util/RestAPIUtility.h"
#include "CLOVise/PLM/Helper/Util/FormatHelper.h"
#include "CLOVise/PLM/Helper/Util/FlexTypeHelper.h"
#include "CLOVise/PLM/CLOViseSuite/CLOViseSuite.h"
#include "CLOVise/PLM/Helper/Util/Logger.h"
#include "CLOVise/PLM/Helper/UIHelper/CVWidgetGenerator.h"
#include"classes/widgets/MVDialog.h"
//#include "CLOVise/PLM/Outbound/PublishToPLM/CopyShape.h"

using namespace std;

namespace CLOVise
{
	PLMShapeSearch* PLMShapeSearch::_instance = NULL;

	PLMShapeSearch* PLMShapeSearch::GetInstance()
	{
		if (_instance == NULL)
			_instance = new PLMShapeSearch();
		return _instance;
	}



	PLMShapeSearch::PLMShapeSearch(QWidget* parent)
		: MVDialog(parent)
	{
		Logger::Info("PLMShapeSearch -> Constructor() -> Start");

		setupUi(this);

		QString windowTitle = PLM_NAME + " PLM " + QString::fromStdString(Configuration::GetInstance()->GetLocalizedShapeClassName()) + " Search";
		this->setWindowTitle(windowTitle);
		//Logger::Info(" PLMShapeSearch -> Constructor() -> Start::" + string(windowTitle));
#ifdef __APPLE__
		this->setWindowFlags(Qt::Dialog | Qt::CustomizeWindowHint | Qt::WindowMinMaxButtonsHint | Qt::WindowCloseButtonHint);
#else
		this->setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint | Qt::CustomizeWindowHint);
		m_pTitleBar = new MVTitleBar(windowTitle, this);
		layout()->setMenuBar(m_pTitleBar);
#endif // !__APPLE__

		m_hierarchyTreeWidget = CVWidgetGenerator::CreateHirarchyTreeWidget("QTreeWidget{min-height: 230px; min-width: 50px; max-height: 1500px; max-width: 230px; border: 1px solid #000; font-family: ArialMT; font-size: 10px; background-color: #262628; margin-bottom: 20px;} QTreeWidget:item:selected{ color: #46C8FF; }", false);
		m_hierarchyTreeWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
		m_searchTreeWidget_1 = CVWidgetGenerator::CreateSearchCriteriaTreeWidget(/*"QTreeWidget { background-color: #262628; border: 1px solid #000; padding-left: 20px; }""QTreeWidget::item {height: 20px; width: 200px; margin-right: 20px; margin-top: 5px; margin-bottom: 5px; border: none; }""QTreeWidget::item:hover{ background-color: #262628; }"*/true);
		m_searchTreeWidget_1->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
		m_searchTreeWidget_2 = CVWidgetGenerator::CreateSearchCriteriaTreeWidget(/*"QTreeWidget { background-color: #262628; border: 1px solid #000; padding-left: 20px; }""QTreeWidget::item {height: 20px; width: 200px; margin-right: 20px; margin-top: 5px; margin-bottom: 5px; border: none; }""QTreeWidget::item:hover{ background-color: #262628; }"*/true);
		m_searchTreeWidget_2->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

		m_viewComboBox = CVWidgetGenerator::CreateComboBox(QString::fromStdString(BLANK), VIEW_COMBOBOX_STYLE + SCROLLBAR_STYLE, false);
		m_filterComboBox = CVWidgetGenerator::CreateComboBox(QString::fromStdString(BLANK), VIEW_COMBOBOX_STYLE + SCROLLBAR_STYLE, false);
		m_seasoncomboBox = CVWidgetGenerator::CreateComboBox(QString::fromStdString(BLANK), FONT_STYLE + SCROLLBAR_STYLE, true);
		m_seasoncomboBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

		ui_paletteSearchWidget->hide();
		ui_hideAndShowButton->hide();
		ui_verticalSeparator->hide();

		QLabel* viewLabel = CVWidgetGenerator::CreateLabel("View", QString::fromStdString(BLANK), HEADER_STYLE, false);
		QLabel* filterLabel = CVWidgetGenerator::CreateLabel("Filter", QString::fromStdString(BLANK), HEADER_STYLE, false);
		QLabel* paletteCheckBoxLable = CVWidgetGenerator::CreateLabel("Palette Search Criteria", QString::fromStdString(BLANK), HEADER_STYLE, true);
		paletteCheckBoxLable->hide();
		m_seasonLabel = CVWidgetGenerator::CreateLabel("Season", QString::fromStdString(BLANK), HEADER_STYLE, true);
		m_seasonLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
		//m_paletteLabel = CVWidgetGenerator::CreateLabel("Palette", QString::fromStdString(BLANK), HEADER_STYLE, true);
		//m_paletteLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
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
		this->setStyleSheet("QDialog{height:539px; width: 1050px;}");
		ui_paletteSearchWidget->setStyleSheet("#ui_paletteSearchWidget{padding: 10px; width: 920px;  border: 1px solid #000; background-color: #262628; max-height:50px;}");
		ui_quickSearchWidget->setStyleSheet("#ui_quickSearchWidget{padding: 10px; width: 920px;border: 1px solid #000; background-color: #262628; max-height:42px;}");

		//initializeColorData();
		m_flextypeTree.clear();
		/*selectType = FlexTypeHelper::CreateFlexTypeTree(ShapeConfig::GetInstance()->GetShapeHierarchyJSON(), m_hierarchyTreeWidget, m_flextypeTree);

		m_hierarchyTreeWidget->topLevelItem(0)->setSelected(true);
		m_hierarchyTreeWidget->expandAll();
		m_hierarchyTreeWidget->setExpandsOnDoubleClick(false);
		m_hierarchyTreeWidget->setSortingEnabled(true);
		m_hierarchyTreeWidget->sortByColumn(0, Qt::AscendingOrder);
		m_hierarchyTreeWidget->setFocusPolicy(Qt::NoFocus);*/

		onHideHirarchy(false);
		DrawSearchWidget(true);
		//CVWidgetGenerator::CreateViewComboBoxOnSearch(m_viewComboBox, ShapeConfig::GetInstance()->GetShapeViewJSON(), selectType.toStdString());
		connectSignalSlots(true);

		m_seasonLabel->hide();
		m_seasoncomboBox->hide();

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

		//ui_paletteLayout_2->insertWidget(0, m_paletteLabel);
		//ui_paletteLayout_2->insertWidget(1, m_paletteComboBox);
		//ui_paletteLayout_2->insertSpacerItem(2, horizontalSpacer_2);

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

		m_hierarchyTreeWidget->hide();
		m_seasonLabel->hide();
		m_seasoncomboBox->hide();
		m_paletteCheckBox->hide();
		paletteCheckBoxLable->hide();
		quickSearchHeaderLabel->hide();
		quickSearchLabel->hide();
		m_quickSearchLineEdit->hide();
		Logger::Logger("before hiding the quicksearch widget.");
		ui_quickSearchWidget->hide();
		Logger::Logger("after hiding the quicksearch widget.");
		///*if (!ShapeConfig::GetInstance()->isModelExecuted)
			//RESTAPI::SetProgressBarData(0, "", false);

//#ifdef __APPLE__
//		RESTAPI::SetProgressBarData(0, "", false);
//#endif
		/*addSeasonPalletValue();
		QStringList seasonList;
		seasonList << "summer" << "winter" << "rain";
		CVWidgetGenerator::CreateComboBoxItem(false, m_seasoncomboBox, 0, seasonList);*/
		m_dateResetButton->hide();
		UTILITY_API->DeleteProgressBar(true);
		Logger::Info("PLMShapeSearch -> Constructor() -> end");


	}

	/*
	* Description - HideHirarchy() method used to show/hide the hierarchy.
	* Parameter -  bool.
	* Exception -
	* Return -
	*/
	void PLMShapeSearch::HideHirarchy(bool _hide)
	{
		Logger::Info("PLMShapeSearch -> HideHirarchy() -> Start");
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
		Logger::Info("PLMShapeSearch -> HideHirarchy() -> end");
	}

	/*
	* Description - HideHirarchy() method used to show/hide the hierarchy.
	* Parameter -  bool.
	* Exception -
	* Return -
	*/
	void PLMShapeSearch::onHideHirarchy(bool _hide)
	{
		HideHirarchy(_hide);
	}

	PLMShapeSearch::~PLMShapeSearch()
	{
		DisconnectSignalSlots();
	}

	/*
	* Description - ResetDateEditWidget() method is a slot for reset date button click and used to reset the date fields.
	* Parameter -
	* Exception -
	* Return -
	*/
	void PLMShapeSearch::ResetDateEditWidget()
	{
		Logger::Info("PLMShapeSearch -> ResetDateEditWidget() -> Start");
		UIHelper::ResetDate(m_searchTreeWidget_1);
		UIHelper::ResetDate(m_searchTreeWidget_2);
		Logger::Info("PLMShapeSearch -> ResetDateEditWidget() -> end");
	}

	/*
	* Description - ResetDateEditWidget() method is a slot for reset date button click and used to reset the date fields.
	* Parameter -
	* Exception -
	* Return -
	*/
	void PLMShapeSearch::onResetDateEditWidget()
	{
		ResetDateEditWidget();
	}

	/*
	* Description - connectSignalSlots() method used to connect and disconnect the signal and slot.
	* Parameter -   bool.
	* Exception -
	* Return -
	*/
	void PLMShapeSearch::connectSignalSlots(bool _b)
	{
		Logger::Info("PLMShapeSearch -> connectSignalSlots() -> Start");
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
		Logger::Info("PLMShapeSearch -> connectSignalSlots() -> end");
	}

	/*
	* Description - ClearAllFields() method is used to clear all user entered data on the search screen.
	* Parameter -
	* Exception -
	* Return -
	*/
	void PLMShapeSearch::ClearAllFields()
	{
		Logger::Info("PLMShapeSearch -> ClearAllFields() -> Start");
		UIHelper::ClearAllFieldsForSearch(m_searchTreeWidget_1);
		UIHelper::ClearAllFieldsForSearch(m_searchTreeWidget_2);
		Logger::Info("PLMShapeSearch -> ClearAllFields() -> End");
	}


	/*
	* Description - ClickedBackButton() method is a slot for back button click and used to go back design suite.
	* Parameter -
	* Exception -
	* Return -
	*/
	void PLMShapeSearch::ClickedBackButton()
	{
		Logger::Info("PLMShapeSearch -> ClickedBackButton() -> Start");
		this->hide();
		CLOVise::CLOViseSuite::GetInstance()->setModal(true);
		CLOViseSuite::GetInstance()->show();


		Logger::Info("PLMShapeSearch -> ClickedBackButton() -> end");
	}

	/*
	* Description - OnClickedBackButton() method is a slot for back button click and used to go back design suite.
	* Parameter -
	* Exception -
	* Return -
	*/
	void PLMShapeSearch::onClickedBackButton()
	{
		ClickedBackButton();
	}

	/*
	* Description - OnClickedSubmitButton() method is a slot for submit button click, validate the input and calling the result table module.
	* Parameter -
	* Exception - exception, Char *
	* Return -
	*/
	void PLMShapeSearch::ClickedSubmitButton()
	{
		Logger::Info("PLMShapeSearch -> ClickedSubmitButton() -> Start");
		try
		{
			collectSearchUIFields();
			UIHelper::ValidateForValidParams(m_searchCriteriaJson, SHAPE_MODULE);
			//ShapeConfig::GetInstance()->SetSelectedViewIdx(m_viewComboBox->currentIndex());
			ShapeConfig::GetInstance()->SetSearchCriteriaJSON(m_searchCriteriaJson);
			this->hide();
			//tmporarilry clearing Results object
			UTILITY_API->CreateProgressBar();
			RESTAPI::SetProgressBarData(10, "Searching " + Configuration::GetInstance()->GetLocalizedShapeClassName() + "..", true);
			//UTILITY_API->SetProgress("Searching shape", (qrand() % 101));
			ShapeConfig::GetInstance()->SetDataFromResponse(ShapeConfig::GetInstance()->GetSearchCriteriaJSON());
			PLMShapeResults::GetInstance()->setModal(true);
			if (Configuration::GetInstance()->GetCurrentScreen() == COPY_SHAPE_CLICKED)
			{
				Configuration::GetInstance()->SetCopyStyleDownloadButton(false);
			}
			if (ShapeConfig::GetInstance()->GetIsModelExecuted() || ShapeConfig::GetInstance()->m_resultAfterLogout)
			{
				PLMShapeResults::GetInstance()->currPageLabel->setText("1");
				PLMShapeResults::GetInstance()->DrawResultWidget(false);
				ShapeConfig::GetInstance()->m_resultAfterLogout = false;
			}
			if (Configuration::GetInstance()->GetCurrentScreen() == COPY_SHAPE_CLICKED)
			{
				ShapeConfig::GetInstance()->m_isShow3DAttWidget = false;
			}
			PLMShapeResults::GetInstance()->exec();
			RESTAPI::SetProgressBarData(0, "", false);
			Logger::Info("PLMShapeSearch -> ClickedSubmitButton() -> end");
		}
		catch (string msg)
		{
			Logger::Error("PLMShapeSearch -> OnClickedSubmitButton Exception :: " + msg);
			RESTAPI::SetProgressBarData(0, "", false);
			UTILITY_API->DisplayMessageBox(msg);
			this->show();
		}
		catch (exception& e)
		{
			RESTAPI::SetProgressBarData(0, "", false);
			Logger::Error("PLMShapeSearch -> OnClickedSubmitButton Exception :: " + string(e.what()));
			UTILITY_API->DisplayMessageBox(e.what());
			this->show();
		}
		catch (const char* msg)
		{
			Logger::Error("PLMShapeSearch -> OnClickedSubmitButton Exception :: " + string(msg));
			wstring wstr(msg, msg + strlen(msg));
			RESTAPI::SetProgressBarData(0, "", false);
			UTILITY_API->DisplayMessageBoxW(wstr);
			this->show();
		}
	}

	/*
	* Description - onClickedSubmitButton() method is a slot for submit button click, validate the input and calling the result table module.
	* Parameter -
	* Exception - exception, Char *
	* Return -
	*/
	void PLMShapeSearch::onClickedSubmitButton()
	{
		ClickedSubmitButton();
	}

	/*
	* Description - FilterChanged() method is a slot for filter item changed and update/create search UI fields.
	* Parameter -  QString.
	* Exception - exception, Char *
	* Return -
	*/
	void PLMShapeSearch::FilterChanged(const QString& _item)
	{
		Logger::Info("PLMShapeSearch -> FilterChanged() -> Start");
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
			Logger::Info("PLMShapeSearch -> FilterChanged() -> end");
		}
		catch (string msg)
		{
			Logger::Error("FilterChanged -> CheckBoxChecked Exception :: " + msg);
			UTILITY_API->DisplayMessageBox(msg);
			this->close();
		}
		catch (exception& e)
		{
			Logger::Error("FilterChanged -> CheckBoxChecked Exception :: " + string(e.what()));
			UTILITY_API->DisplayMessageBox(e.what());
			this->close();
		}
		catch (const char* msg)
		{
			Logger::Error("FilterChanged -> CheckBoxChecked Exception :: " + string(msg));
			wstring wstr(msg, msg + strlen(msg));
			UTILITY_API->DisplayMessageBoxW(wstr);
			this->close();
		}
	}

	/*
	* Description - onFilterChanged() method is a slot for filter item changed and update/create search UI fields.
	* Parameter -  QString.
	* Exception - exception, Char *
	* Return -
	*/
	void PLMShapeSearch::onFilterChanged(const QString& _item)
	{
		FilterChanged(_item);
	}

	//void PLMShapeSearch::addSeasonPalletValue()
	//{
	//	json readPaletteJsonValue = json::object();
	//	json activeSeasons = json::object();
	//	json activeSeasonsJson = json::object();
	//	QStringList seasonList;
	//	string seasonName = "", seasonId = "";
	//	try
	//	{
	//	/*	if (_checked)
	//		{
	//			_seasonLabel->show();
	//			_seasoncomboBox->show();*/
	//		//Logger::Info("addSeasonPalletValue start");
	//		//json _seasonPaletteJson = ShapeConfig::GetInstance()->GetSeasonPaletteJSON(true);
	//		//Logger::Info("addSeasonPalletValue enter`1");
	//		//	if (_seasonPaletteJson != Null_Value)
	//		//	{
	//		//		Logger::Info("addSeasonPalletValue enter`2");
	//		//		activeSeasons = Helper::GetJSONParsedValue<string>(_seasonPaletteJson, ACTIVESEASONS_JSON_KEY, false);
	//		//		Logger::Info("addSeasonPalletValue enter`3");
	//		//		for (int asCount = 0; asCount < activeSeasons.size(); asCount++)
	//		//		{
	//		//			Logger::Info("addSeasonPalletValue enter`4");
	//		//			activeSeasonsJson = Helper::GetJSONParsedValue<int>(activeSeasons, asCount, false);
	//		//			Logger::Info("addSeasonPalletValue enter`5");
	//		//			seasonName = Helper::GetJSONValue<string>(activeSeasonsJson, SEASONNAME_KEY, true);
	//		//			Logger::Info("addSeasonPalletValue enter`6");
	//		//			seasonId = Helper::GetJSONValue<string>(activeSeasonsJson, SEASONID_KEY, true);
	//		//			Logger::Info("addSeasonPalletValue enter`7");
	//		//			m_seasoncomboBox->setProperty(seasonName.c_str(), QString::fromStdString(seasonId));
	//		//			////if (_uiModule == PRODUCT_MODULE)
	//		//			//{
	//		//			//	string producttypeValue = Helper::GetJSONValue<string>(activeSeasonsJson, PRODUCT_SEASON_TYPE_KEY, true);
	//		//			//	string productTypeKey = seasonName + "productType";
	//		//			//	m_seasoncomboBox->setProperty(productTypeKey.c_str(), QString::fromStdString(producttypeValue));
	//		//			//}
	//		//			Logger::Info("addSeasonPalletValue enter`8");
	//		//			seasonList << QString::fromStdString(seasonName);
	//		//			Logger::Info("addSeasonPalletValue enter`9");
	//		//		}
	//	
	//				Logger::Info("addSeasonPalletValue enter`10");
	//			
	//		}
	//	/*	else
	//		{
	//			_seasonLabel->hide();
	//			_seasoncomboBox->hide();
	//			if (_uiModule != PRODUCT_MODULE)
	//			{
	//				_paletteLabel->hide();
	//				_paletteComboBox->hide();
	//			}

	//		}*/
	//	
	//	catch (string msg)
	//	{
	//		throw msg;
	//	}
	//	catch (const char* msg)
	//	{
	//		throw msg;
	//	}
	//	catch (exception& e)
	//	{
	//		throw e;
	//	}
	//}


	/*
	* Description - CheckBoxChecked() method is a slot for check box  for search through season.
	* Parameter -  bool.
	* Exception - exception, Char *
	* Return -
	*/
	void PLMShapeSearch::CheckBoxChecked(bool _checked)
	{
		Logger::Info("PLMShapeSearch -> CheckBoxChecked() -> Start");
		try
		{
			QComboBox* emptybox;
			QLabel* emptyLable;
			if (_checked)
			{
				UIHelper::CheckBoxChecked(_checked, ShapeConfig::GetInstance()->GetSeasonPaletteJSON(true), m_seasoncomboBox, emptybox, m_seasonLabel, emptyLable, SHAPE_MODULE);

			}
			else
			{
				UIHelper::CheckBoxChecked(_checked, ShapeConfig::GetInstance()->GetSeasonPaletteJSON(false), m_seasoncomboBox, emptybox, m_seasonLabel, emptyLable, SHAPE_MODULE);
				m_paletteCheckBox->setCheckState(Qt::Unchecked);
			}
			Logger::Info("PLMShapeSearch -> CheckBoxChecked() -> end");
		}
		catch (string msg)
		{
			Logger::Error("PLMShapeSearch -> CheckBoxChecked Exception :: " + msg);
			UTILITY_API->DisplayMessageBox(msg);
			m_paletteCheckBox->setCheckState(Qt::Unchecked);
		}
		catch (exception& e)
		{
			Logger::Error("PLMShapeSearch -> CheckBoxChecked Exception :: " + string(e.what()));
			UTILITY_API->DisplayMessageBox(e.what());
			m_paletteCheckBox->setCheckState(Qt::Unchecked);
		}
		catch (const char* msg)
		{
			Logger::Error("PLMShapeSearch -> CheckBoxChecked Exception :: " + string(msg));
			wstring wstr(msg, msg + strlen(msg));
			UTILITY_API->DisplayMessageBoxW(wstr);
			m_paletteCheckBox->setCheckState(Qt::Unchecked);
		}
	}

	/*
	* Description - onCheckBoxChecked() method is a slot for check box  for search through season and palette.
	* Parameter -  bool.
	* Exception - exception, Char *
	* Return -
	*/
	void PLMShapeSearch::onCheckBoxChecked(bool _checked)
	{
		CheckBoxChecked(_checked);
	}

	/*
	* Description - SeasonChanged() method is a slot for season drop down is changed.
	* Parameter -  Qstring.
	* Exception - exception, Char *
	* Return -
	*/
	void PLMShapeSearch::SeasonChanged(const QString& _selectedSeason)
	{
		Logger::Info("PLMShapeSearch -> SeasonChanged() -> Start");
		this->hide();
		UTILITY_API->CreateProgressBar();
		UTILITY_API->SetProgress("Loading", (qrand() % 101));
		m_hierarchyLoading = true;
		try
		{
			seasonChanged(_selectedSeason.toStdString(), m_seasoncomboBox, selectType.toStdString(), m_hierarchyTreeWidget);
			UTILITY_API->DeleteProgressBar(true);
			this->show();
			m_hierarchyLoading = false;
			Logger::Info("PLMShapeSearch -> SeasonChanged() -> end");
		}
		catch (string msg)
		{
			UTILITY_API->DeleteProgressBar(true);
			Logger::Error("PLMShapeSearch -> SeasonChanged Exception :: " + msg);
			UTILITY_API->DisplayMessageBox(msg);
			m_paletteCheckBox->setCheckState(Qt::Unchecked);
			m_hierarchyLoading = false;
		}
		catch (exception& e)
		{
			UTILITY_API->DeleteProgressBar(true);
			Logger::Error("PLMShapeSearch -> SeasonChanged Exception :: " + string(e.what()));
			UTILITY_API->DisplayMessageBox(e.what());
			m_paletteCheckBox->setCheckState(Qt::Unchecked);
			m_hierarchyLoading = false;
		}
		catch (const char* msg)
		{
			UTILITY_API->DeleteProgressBar(true);
			Logger::Error("PLMShapeSearch -> SeasonChanged Exception :: " + string(msg));
			wstring wstr(msg, msg + strlen(msg));
			UTILITY_API->DisplayMessageBoxW(wstr);
			m_paletteCheckBox->setCheckState(Qt::Unchecked);
			m_hierarchyLoading = false;
		}
	}

	/*
	* Description - SeasonChanged() method is a slot for season drop down is changed.
	* Parameter -  Qstring.
	* Exception - exception, Char *
	* Return -
	*/
	void PLMShapeSearch::onSeasonChanged(const QString& _selectedSeason)
	{
		SeasonChanged(_selectedSeason);
	}

	/*
	* Description - TreeNodeClicked() method is a slot for hierarchy is click.
	* Parameter -  QTreeWidgetItem, int.
	* Exception - exception, Char *
	* Return -
	*/
	void PLMShapeSearch::TreeNodeClicked(QTreeWidgetItem* _item, int _column)
	{
		Logger::Info("PLMShapeSearch -> TreeNodeClicked() -> Start");
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
				CVWidgetGenerator::CreateViewComboBoxOnSearch(m_viewComboBox, ShapeConfig::GetInstance()->GetShapeViewJSON(), selectType.toStdString());
				drawSearchUI(selectType, false, BLANK);
				m_hierarchyLoading = false;
				UTILITY_API->DeleteProgressBar(true);
				this->show();
				onCheckBoxChecked(false);
				Logger::Info("PLMShapeSearch -> TreeNodeClicked() -> end");
			}
		}
		catch (string msg)
		{
			Logger::Error("PLMShapeSearch -> TreeNodeClicked Exception :: " + msg);
			UTILITY_API->DisplayMessageBox(msg);
		}
		catch (exception& e)
		{
			Logger::Error("PLMShapeSearch -> TreeNodeClicked Exception :: " + string(e.what()));
			UTILITY_API->DisplayMessageBox(e.what());
		}
		catch (const char* msg)
		{
			Logger::Error("PLMShapeSearch -> TreeNodeClicked Exception :: " + string(msg));
			wstring wstr(msg, msg + strlen(msg));
			UTILITY_API->DisplayMessageBoxW(wstr);
		}
	}

	/*
	* Description - TreeNodeClicked() method is a slot for hierarchy is click.
	* Parameter -  QTreeWidgetItem, int.
	* Exception - exception, Char *
	* Return -
	*/
	void PLMShapeSearch::onTreeNodeClicked(QTreeWidgetItem* _item, int _column)
	{
		TreeNodeClicked(_item, _column);
	}

	/*
	* Description - DrawSearchUI() method used to create search fields in search UI.
	* Parameter -  QString, bool, string.
	* Exception - exception, Char *
	* Return -
	*/
	void PLMShapeSearch::drawSearchUI(QString _selectType, bool _drawFilter, string _selectedFilter)
	{
		Logger::Info("PLMShapeSearch -> drawSearchUI() -> Start");
		try
		{
			ShapeConfig::GetInstance()->SetDateFlag(false);
			QStringList attScops = ShapeConfig::GetInstance()->GetAttScopes();
			json attributesJson = json::object();

			if (MaterialConfig::GetInstance()->GetIsModelExecuted())
				Configuration::GetInstance()->SetProgressBarProgress(RESTAPI::SetProgressBarProgress(Configuration::GetInstance()->GetProgressBarProgress(), 10, "Loading S Search"));
			/*if (!CVWidgetGenerator::DrawFilterAndSearchCriteriaWidget(ShapeConfig::GetInstance()->GetShapeFilterJSON(), m_filterComboBox, m_searchTreeWidget, _selectType, _selectedFilter, attScops, _drawFilter))
			{*/
			Logger::Info("ShapeConfig::GetInstance()->GetShapeFieldsJSON()=========== "+ to_string(ShapeConfig::GetInstance()->GetShapeFieldsJSON()));
			FlexTypeHelper::DrawDefaultSearchCriteriaWidget(ShapeConfig::GetInstance()->GetShapeFieldsJSON(), _selectType.toStdString(), m_searchTreeWidget_1, m_searchTreeWidget_2, attScops);
			//}
			m_searchTreeWidget_1->setSelectionMode(QAbstractItemView::NoSelection);
			m_searchTreeWidget_1->setColumnCount(2);
			m_searchTreeWidget_1->setHeaderHidden(true);
			m_searchTreeWidget_1->setWordWrap(true);
			m_searchTreeWidget_1->setDropIndicatorShown(false);
			m_searchTreeWidget_1->setRootIsDecorated(false);
			m_searchTreeWidget_1->setSelectionMode(QAbstractItemView::NoSelection);
			m_searchTreeWidget_1->setStyleSheet("QTreeWidget { background-color: #262628; border: 1px solid #000; padding-left: 20px; padding-top: 10px; outline: 0; }""QTreeWidget::item {height: 20px; width: 200px; margin-right: 20px; margin-top: 5px; margin-bottom: 5px; border: none; }""QTreeWidget::item:hover{ background-color: #262628; }""QTreeView{outline: 0;}");
			m_searchTreeWidget_2->setSelectionMode(QAbstractItemView::NoSelection);
			m_searchTreeWidget_2->setColumnCount(2);
			m_searchTreeWidget_2->setHeaderHidden(true);
			m_searchTreeWidget_2->setWordWrap(true);
			m_searchTreeWidget_2->setDropIndicatorShown(false);
			m_searchTreeWidget_2->setRootIsDecorated(false);
			m_searchTreeWidget_2->setSelectionMode(QAbstractItemView::NoSelection);
			m_searchTreeWidget_2->setStyleSheet("QTreeWidget { background-color: #262628; border: 1px solid #000; padding-left: 20px; padding-top: 10px; outline: 0; }""QTreeWidget::item {height: 20px; width: 200px; margin-right: 20px; margin-top: 5px; margin-bottom: 5px; border: none; }""QTreeWidget::item:hover{ background-color: #262628; }""QTreeView{outline: 0;}");

			/*if (!m_searchTreeWidget_2->isHidden())
				this->setMinimumSize(850, 650);*/ // this commented because, when two widget apear on the searchtable one widget overon anothere widget.

			for (int index = 0; index < m_searchTreeWidget_1->topLevelItemCount(); ++index)
			{
				QTreeWidgetItem* topItem = m_searchTreeWidget_1->topLevelItem(index);
				QWidget* qWidgetColumn0 = m_searchTreeWidget_1->itemWidget(topItem, 0);
				QWidget* qWidgetColumn1 = m_searchTreeWidget_1->itemWidget(topItem, 1);
				if (!qWidgetColumn0 || !qWidgetColumn1)
				{
					continue;
				}
				if (QLabel* qlabel = qobject_cast<QLabel*>(qWidgetColumn0))
				{
					if (qlabel->text() == "Season")
					{
						QObject::connect(qWidgetColumn1, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(onSeasonSelected(const QString&)));
					}
				}

			}
			for (int index = 0; index < m_searchTreeWidget_2->topLevelItemCount(); ++index)
			{
				QTreeWidgetItem* topItem = m_searchTreeWidget_2->topLevelItem(index);
				QWidget* qWidgetColumn0 = m_searchTreeWidget_2->itemWidget(topItem, 0);
				QWidget* qWidgetColumn1 = m_searchTreeWidget_2->itemWidget(topItem, 1);
				if (!qWidgetColumn0 || !qWidgetColumn1)
				{
					continue;
				}
				if (QLabel* qlabel = qobject_cast<QLabel*>(qWidgetColumn0))
				{
					if (qlabel->text() == "Season")
					{
						QObject::connect(qWidgetColumn1, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(onSeasonSelected(const QString&)));
					}
				}

			}

			for (int searchFeildsCount = 0; searchFeildsCount < ShapeConfig::GetInstance()->GetShapeFieldsJSON().size(); searchFeildsCount++)
			{
				json feildsJson = Helper::GetJSONParsedValue<int>(ShapeConfig::GetInstance()->GetShapeFieldsJSON(), searchFeildsCount, false);
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
		}
		catch (string msg)
		{
			Logger::Error("PLMShapeSearch -> drawSearchUI() Exception :: " + msg);
			CLOViseSuite::GetInstance()->show();
			throw msg;
		}
		catch (exception& e)
		{
			Logger::Error("PLMShapeSearch -> drawSearchUI() Exception :: " + string(e.what()));
			CLOViseSuite::GetInstance()->show();
			throw e;
		}
		catch (const char* msg)
		{
			Logger::Error("PLMShapeSearch -> drawSearchUI() Exception :: " + string(msg));
			CLOViseSuite::GetInstance()->show();

			throw msg;
		}
		Logger::Info("PLMShapeSearch -> drawSearchUI() -> end");
	}

	/*
	* Description - CollectSearchUIFields() method used collect all the user input from search UI.
	* Parameter -
	* Exception -
	* Return -
	*/
	void  PLMShapeSearch::collectSearchUIFields()
	{
		Logger::Info("PLMShapeSearch -> collectSearchUIFields() -> Start");
		/*string seasonValue = "";
		string paletteValue = "";
		string quickSearchValue = "";
		string filterValue = "";*/
		m_searchCriteriaJson.clear();
		/*if (m_paletteCheckBox->checkState())
		{
			seasonValue = m_seasoncomboBox->currentText().toStdString();
			UIHelper::ShapeSeasonValidation(seasonValue);

			seasonValue = m_seasoncomboBox->property(seasonValue.c_str()).toString().toStdString();
			m_searchCriteriaJson[SEASONID_KEY] = seasonValue;
			m_searchCriteriaJson[SEASONID_VERSION_KEY] = seasonValue;
			m_searchCriteriaJson[PALETTEID_KEY] = paletteValue;
			m_searchCriteriaJson[SHAPENAME_KEY] = BLANK;
			m_searchCriteriaJson[SHAPEID_KEY] = BLANK;
		}
		else
		{
			m_searchCriteriaJson[SEASONID_KEY] = BLANK;
			m_searchCriteriaJson[PALETTEID_KEY] = BLANK;
			m_searchCriteriaJson[SHAPENAME_KEY] = BLANK;
			m_searchCriteriaJson[SHAPEID_KEY] = BLANK;
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
		m_searchCriteriaJson[MODULE_KEY] = SHAPE_MODULE;
		m_searchCriteriaJson[INCLUDE_SUPPLIER_STRING_KEY] = "";
		m_searchCriteriaJson[SEARCHBY_SUPPLIER_STRING_KEY] = "";
		m_searchCriteriaJson[REQ_FROM_CLO_KEY] = "true";*/
		m_searchCriteriaJson[ATTRIBUTES_KEY] = UIHelper::CollectSearchCriteriaFields(m_searchTreeWidget_1, m_searchTreeWidget_2);
		Logger::Info("PLMShapeSearch -> collectSearchUIFields() -> end");
	}

	/*
	* Description - SeasonChanged() method is a slot for season value change and re arranging the search UI fields according to season.
	* Parameter - string, QComboBox, string, QCheckBox, QTreeWidget
	* Exception -
	* Return -
	*/
	void PLMShapeSearch::seasonChanged(string _selectedSeason, QComboBox* _paletteComboBox, string _selectedtype, QTreeWidget* _hirarchyTree)
	{
		Logger::Info("PLMShapeSearch -> seasonChanged() -> Start");
		string shapeTypeKey = _paletteComboBox->currentText().toStdString() + "shapeType";
		string shapeType = _paletteComboBox->property(shapeTypeKey.c_str()).toString().toStdString();
		if (FormatHelper::HasContent(_selectedSeason) && (FormatHelper::HasContent(shapeType) && shapeType != _selectedtype))
		{

			for (int hirarchyCount = 0; hirarchyCount < _hirarchyTree->topLevelItemCount(); hirarchyCount++)
			{
				QTreeWidgetItem* item = _hirarchyTree->topLevelItem(hirarchyCount);
				QString value = item->text(0);
				_hirarchyTree->clearSelection();
				item->data(1, Qt::UserRole).toString().toStdString();
				if (item->data(1, Qt::UserRole).toString().toStdString() == shapeType)
				{
					item->setSelected(true);
					break;
				}
				else
				{
					UIHelper::CheckHirarchyChiltedItem(item, 1, shapeType);
				}
			}
			selectType = QString::fromStdString(shapeType);
			drawSearchUI(selectType, false, BLANK);
		}
		Logger::Info("PLMShapeSearch -> seasonChanged() -> end");
	}

	/*
	* Description - onSeasonSelected() method is a slot for season value change and re collect and the division fields according to season.
	* Parameter - QString
	* Exception - try catch.
	* Return -
	*/
	void PLMShapeSearch::onSeasonSelected(const QString& _item)
	{

		Logger::Info("PLMShapeSearch::onSeasonSelected() Started.." + _item.toStdString());
		try
		{
			string seasonId = BLANK;
			if (!_item.isEmpty())
			{
				seasonId = sender()->property(_item.toStdString().c_str()).toString().toStdString();
			}
			for (int index = 0; index < m_searchTreeWidget_1->topLevelItemCount(); ++index)
			{
				QTreeWidgetItem* topItem = m_searchTreeWidget_1->topLevelItem(index);
				QWidget* qWidgetColumn0 = m_searchTreeWidget_1->itemWidget(topItem, 0);
				QWidget* qWidgetColumn1 = m_searchTreeWidget_1->itemWidget(topItem, 1);
				if (QLabel* qlabel = qobject_cast<QLabel*>(qWidgetColumn0))
				{
					if (qlabel->text() == "Division")
					{
						if (FormatHelper::HasContent(seasonId))
						{
							if (QListWidget* listC1 = qobject_cast<QListWidget*>(qWidgetColumn1))
							{
								listC1->clear();
								UIHelper::UpdateDivisionField(seasonId, qWidgetColumn1);
							}
							else if (QComboBox* qComboBoxC1 = qobject_cast<QComboBox*>(qWidgetColumn1))
							{
								qComboBoxC1->clear();
								UIHelper::UpdateDivisionField(seasonId, qWidgetColumn1);
							}
						}
						else
						{
							if (QListWidget* listC1 = qobject_cast<QListWidget*>(qWidgetColumn1))
							{
								listC1->clear();
							}
							else if (QComboBox* qComboBoxC1 = qobject_cast<QComboBox*>(qWidgetColumn1))
							{
								qComboBoxC1->clear();
							}
						}
						break;
					}
				}
			}

			for (int index = 0; index < m_searchTreeWidget_2->topLevelItemCount(); ++index)
			{
				QTreeWidgetItem* topItem = m_searchTreeWidget_2->topLevelItem(index);
				QWidget* qWidgetColumn0 = m_searchTreeWidget_2->itemWidget(topItem, 0);
				QWidget* qWidgetColumn1 = m_searchTreeWidget_2->itemWidget(topItem, 1);
				if (QLabel* qlabel = qobject_cast<QLabel*>(qWidgetColumn0))
				{
					if (qlabel->text() == "Division")
					{
						if (FormatHelper::HasContent(seasonId))
						{
							if (QListWidget* listC1 = qobject_cast<QListWidget*>(qWidgetColumn1))
							{
								listC1->clear();
								UIHelper::UpdateDivisionField(seasonId, qWidgetColumn1);
							}
							else if (QComboBox* qComboBoxC1 = qobject_cast<QComboBox*>(qWidgetColumn1))
							{
								qComboBoxC1->clear();
								UIHelper::UpdateDivisionField(seasonId, qWidgetColumn1);
							}
						}
						else
						{
							if (QListWidget* listC1 = qobject_cast<QListWidget*>(qWidgetColumn1))
							{
								listC1->clear();
							}
							else if (QComboBox* qComboBoxC1 = qobject_cast<QComboBox*>(qWidgetColumn1))
							{
								qComboBoxC1->clear();
							}
						}
						break;
					}
				}
			}
		}
		catch (string msg)
		{
			UTILITY_API->DeleteProgressBar(true);
			Logger::Error("PLMShapeSearch -> onSeasonSelected() Exception :: " + msg);
			UTILITY_API->DisplayMessageBox(msg);
		}
		catch (exception& e)
		{
			UTILITY_API->DeleteProgressBar(true);
			Logger::Error("PLMShapeSearch -> onSeasonSelected() Exception :: " + string(e.what()));
			UTILITY_API->DisplayMessageBox(e.what());
		}
		catch (const char* msg)
		{
			UTILITY_API->DeleteProgressBar(true);
			Logger::Error("PLMShapeSearch -> onSeasonSelected() Exception :: " + string(msg));
			wstring wstr(msg, msg + strlen(msg));
			UTILITY_API->DisplayMessageBoxW(wstr);
		}
		Logger::Debug("PLMShapeSearch->onSeasonSelected() End");
	}

	/*
	* Description - reject() method is a slot for close button.
	* Parameter - QString
	* Exception - try catch.
	* Return -
	*/
	void PLMShapeSearch::reject()
	{
		this->accept();
	}

	/*
	* Description - GetTreewidget() method is for pass a treeWidget.
	* Parameter -
	* Exception -
	* Return -QTreeWidget
	*/
	QTreeWidget* PLMShapeSearch::GetTreewidget(int _index)
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
	void PLMShapeSearch::DrawSearchWidget(bool _isFromConstructor)
	{
		if (!_isFromConstructor)
		{
			ClearAllFields();
		}
		else
		{
			//onHideHirarchy(false);
			selectType = QString::fromStdString(SHAPE_ROOT_TYPE);
			if (ShapeConfig::GetInstance()->GetIsModelExecuted())
			{
				Configuration::GetInstance()->SetProgressBarProgress(RESTAPI::SetProgressBarProgress(Configuration::GetInstance()->GetProgressBarProgress(), 8, "Loading " + Configuration::GetInstance()->GetLocalizedShapeClassName() + " Search"));
			}
			drawSearchUI(selectType, false, BLANK);
		}
	}
}
