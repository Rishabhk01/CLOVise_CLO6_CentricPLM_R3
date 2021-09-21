/*
* Copyright 2020-2021 CLO-Vise. All rights reserved
*
* @file PLMSampleSearch.cpp
*
* @brief Class implementation for accessing Sample in CLO from PLM.
* This class has all the variable and methods implementation which are used in PLM Sample Search.
*
* @author GoVise
*
* @date  27-MAY-2020
*/
#include "PLMSampleSearch.h"

#include <iostream>
#include <string>

#include "CLOVise/PLM/Inbound/Sample/PLMSampleResult.h"
#include "CLOVise/PLM/Helper/Util/Helper.h"
#include "CLOVise/PLM/Helper/UIHelper/UIHelper.h"
#include "CLOVise/PLM/Helper/Util/RestAPIUtility.h"
#include "CLOVise/PLM/Helper/Util/FormatHelper.h"
#include "CLOVise/PLM/Helper/Util/FlexTypeHelper.h"
#include "CLOVise/PLM/Helper/UIHelper/CVWidgetGenerator.h"
#include "CLOVise/PLM/CLOViseSuite/CLOViseSuite.h"
#include "CLOVise/PLM/Inbound/Sample/SampleConfig.h"
#include "classes/widgets/MVDialog.h"
#include "CLOVise/PLM/Helper/Util/Logger.h"

using namespace std;

namespace CLOVise
{
	PLMSampleSearch* PLMSampleSearch::_instance = NULL;

	PLMSampleSearch* PLMSampleSearch::GetInstance()
	{
		if (_instance == NULL)
			_instance = new PLMSampleSearch();
		return _instance;
	}

	void PLMSampleSearch::Destroy()
	{
		if (_instance)
		{
			delete _instance;
			_instance = NULL;
		}
	}

	PLMSampleSearch::PLMSampleSearch(QWidget* parent)
		: MVDialog(parent)
	{
		Logger::Info("PLMSampleSearch -> Constructor() -> Start");

		/*if(!ColorConfig::GetInstance()->isModelExecuted)
			RESTAPI::SetProgressBarData(18, "Loading Sample Search", true);*/
		setupUi(this);

		QString windowTitle = PLM_NAME + " PLM Sample Search";
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
		m_searchTreeWidget = CVWidgetGenerator::CreateSearchCriteriaTreeWidget(/*"QTreeWidget { background-color: #262628; border: 1px solid #000; padding-left: 20px; }""QTreeWidget::item {height: 20px; width: 200px; margin-right: 20px; margin-top: 5px; margin-bottom: 5px; border: none; }""QTreeWidget::item:hover{ background-color: #262628; }"*/true);
		m_searchTreeWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

		m_viewComboBox = CVWidgetGenerator::CreateComboBox(QString::fromStdString(BLANK), VIEW_COMBOBOX_STYLE + SCROLLBAR_STYLE, false);
		m_filterComboBox = CVWidgetGenerator::CreateComboBox(QString::fromStdString(BLANK), VIEW_COMBOBOX_STYLE + SCROLLBAR_STYLE, false);
		m_productNameLineEdit = CVWidgetGenerator::CreateLineEdit("", LINEEDIT_STYLE, true);
		m_productNameLineEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
		m_productIdLineEdit = CVWidgetGenerator::CreateLineEdit("", LINEEDIT_STYLE, true);
		m_productIdLineEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

		ui_paletteSearchWidget->hide();
		ui_hideAndShowButton->hide();
		ui_verticalSeparator->hide();

		QLabel* viewLabel = CVWidgetGenerator::CreateLabel("View", QString::fromStdString(BLANK), HEADER_STYLE, false);
		QLabel* filterLabel = CVWidgetGenerator::CreateLabel("Filter", QString::fromStdString(BLANK), HEADER_STYLE, false);
		QLabel* productCheckBoxLable = CVWidgetGenerator::CreateLabel("Product Search Criteria", QString::fromStdString(BLANK), HEADER_STYLE, true);
		productCheckBoxLable->hide();
		m_productNameLabel= CVWidgetGenerator::CreateLabel("ProductName", QString::fromStdString(BLANK), HEADER_STYLE, true);
		m_productNameLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
		m_productIdLabel = CVWidgetGenerator::CreateLabel("Product Id", QString::fromStdString(BLANK), HEADER_STYLE, true);
		m_productIdLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
		QLabel* searchCriteriaLabel = CVWidgetGenerator::CreateLabel("Search Criteria", QString::fromStdString(BLANK), HEADER_STYLE, true);
		QLabel* quickSearchHeaderLabel = CVWidgetGenerator::CreateLabel("Quick Search Criteria", QString::fromStdString(BLANK), HEADER_STYLE, true);
		quickSearchHeaderLabel->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
		QLabel* quickSearchLabel = CVWidgetGenerator::CreateLabel("Quick Search", QString::fromStdString(BLANK), HEADER_STYLE, true);
		quickSearchLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

		m_productCheckBox = CVWidgetGenerator::CreateCheckBox("", "", true);
		m_productCheckBox->hide();

		m_quickSearchLineEdit = CVWidgetGenerator::CreateLineEdit("", LINEEDIT_STYLE, true);

		m_dateResetButton = CVWidgetGenerator::CreateToolButton("", "Reset Date", "QToolButton { icon_size: 18px; image: url(:/CLOVise/PLM/Images/DateReset_none.svg); }""QToolButton:hover { image: url(:/CLOVise/PLM/Images/DateReset_over.svg); }""QToolTip{ color: #46C8FF; background-color: #33414D; border: 1px #000000; }", true);

		m_backButton = CVWidgetGenerator::CreatePushButton("Back", BACK_HOVER_PATH, "Back", PUSH_BUTTON_STYLE, 30, true);
		m_searchButton = CVWidgetGenerator::CreatePushButton("Search", SEARCH_HOVER_ICON_PATH, "Search", PUSH_BUTTON_STYLE, 30, true);

#ifdef __APPLE__
		ui_paletteLabelLayout->setSpacing(10);
#endif

		this->setStyleSheet("QDialog{height:539px; width: 1050px;}");
		ui_paletteSearchWidget->setStyleSheet("#ui_paletteSearchWidget{padding: 10px; width: 920px;  border: 1px solid #000; background-color: #262628; max-height:50px;}");
		ui_quickSearchWidget->setStyleSheet("#ui_quickSearchWidget{padding: 10px; width: 920px;border: 1px solid #000; background-color: #262628; max-height:42px;}");

		m_flextypeTree.clear();
		selectType = FlexTypeHelper::CreateFlexTypeTree(SampleConfig::GetInstance()->GetSampleHierarchyJSON(), m_hierarchyTreeWidget, m_flextypeTree);

		m_hierarchyTreeWidget->topLevelItem(0)->setSelected(true);
		m_hierarchyTreeWidget->expandAll();
		m_hierarchyTreeWidget->setExpandsOnDoubleClick(false);
		m_hierarchyTreeWidget->setSortingEnabled(true);
		m_hierarchyTreeWidget->sortByColumn(0, Qt::AscendingOrder);
		m_hierarchyTreeWidget->setFocusPolicy(Qt::NoFocus);

		onHideHirarchy(false);
		CVWidgetGenerator::CreateViewComboBoxOnSearch(m_viewComboBox, SampleConfig::GetInstance()->GetSampleViewJSON(), selectType.toStdString());
		drawSearchUI(selectType, false, BLANK);

		connectSignalSlots(true);

		m_productNameLabel->hide();
		m_productNameLineEdit->hide();
		m_productIdLabel->hide();
		m_productIdLineEdit->hide();

		QSpacerItem *horizontalSpacer = new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Fixed);
		QSpacerItem *horizontalSpacer_1 = new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Fixed);
		QSpacerItem *horizontalSpacer_2 = new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Fixed);
		QSpacerItem *horizontalSpacer_3 = new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Fixed);
		QSpacerItem *horizontalSpacer_4 = new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Fixed);
		QSpacerItem *horizontalSpacer_5 = new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Fixed);

		ui_heirarchyLayout->addWidget(m_hierarchyTreeWidget);
		ui_searchTreeLayout_1->addWidget(m_searchTreeWidget);

		ui_viewLayout->addWidget(viewLabel);
		ui_viewLayout->addWidget(m_viewComboBox);

		ui_filterLayout->addWidget(filterLabel);
		ui_filterLayout->addWidget(m_filterComboBox);

		ui_paletteLabelLayout->insertWidget(0, m_productCheckBox);
		ui_paletteLabelLayout->insertWidget(1, productCheckBoxLable);
		ui_paletteLabelLayout->insertSpacerItem(2, horizontalSpacer);

		ui_paletteLayout_1->insertWidget(0, m_productNameLabel);
		ui_paletteLayout_1->insertWidget(1, m_productNameLineEdit);
		ui_paletteLayout_1->insertSpacerItem(2, horizontalSpacer_1);

		ui_paletteLayout_2->insertWidget(0, m_productIdLabel);
		ui_paletteLayout_2->insertWidget(1, m_productIdLineEdit);
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

#ifdef __APPLE__
		RESTAPI::SetProgressBarData(0, "", false);
#endif
		Logger::Info("PLMSampleSearch -> Constructor() -> end");
	}

	/*
	* Description - HideHirarchy() method used to show/hide the hierarchy.
	* Parameter -  bool.
	* Exception -
	* Return -
	*/
	void PLMSampleSearch::HideHirarchy(bool _hide)
	{
		Logger::Info("PLMSampleSearch -> HideHirarchy() -> Start");
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
		Logger::Info("PLMSampleSearch -> HideHirarchy() -> End");
	}

	/*
	* Description - HideHirarchy() method used to show/hide the hierarchy.
	* Parameter -  bool.
	* Exception -
	* Return -
	*/
	void PLMSampleSearch::onHideHirarchy(bool _hide)
	{
		Logger::Info("PLMSampleSearch -> onHideHirarchy() -> Start");
		HideHirarchy(_hide);
		Logger::Info("PLMSampleSearch -> onHideHirarchy() -> End");
	}

	PLMSampleSearch::~PLMSampleSearch()
	{
		Logger::Info("PLMSampleSearch -> PLMSampleSearch() -> Start");
		DisconnectSignalSlots();
		Logger::Info("PLMSampleSearch -> PLMSampleSearch() -> End");
	}

	/*
	* Description - ResetDateEditWidget() method is a slot for reset date button click and used to reset the date fields.
	* Parameter -
	* Exception -
	* Return -
	*/
	void PLMSampleSearch::ResetDateEditWidget()
	{
		Logger::Info("PLMSampleSearch -> ResetDateEditWidget() -> Start");
		UIHelper::ResetDate(m_searchTreeWidget);
		Logger::Info("PLMSampleSearch -> ResetDateEditWidget() -> End");
	}

	/*
	* Description - ResetDateEditWidget() method is a slot for reset date button click and used to reset the date fields.
	* Parameter -
	* Exception -
	* Return -
	*/
	void PLMSampleSearch::onResetDateEditWidget()
	{
		Logger::Info("PLMSampleSearch -> onResetDateEditWidget() -> Start");
		ResetDateEditWidget();
		Logger::Info("PLMSampleSearch -> onResetDateEditWidget() -> End");
	}

	/*
	* Description - connectSignalSlots() method used to connect and disconnect the signal and slot.
	* Parameter -   bool.
	* Exception -
	* Return -
	*/
	void PLMSampleSearch::connectSignalSlots(bool _b)
	{
		Logger::Info("PLMSampleSearch -> connectSignalSlots() -> Start");
		if (_b)
		{
			QObject::connect(ui_hideAndShowButton, SIGNAL(clicked(bool)), this, SLOT(onHideHirarchy(bool)));
			QObject::connect(m_hierarchyTreeWidget, SIGNAL(itemClicked(QTreeWidgetItem*, int)), this, SLOT(onTreeNodeClicked(QTreeWidgetItem*, int)));
			QObject::connect(m_searchButton, SIGNAL(clicked()), this, SLOT(onClickedSubmitButton())); // should be used
			QObject::connect(m_backButton, SIGNAL(clicked()), this, SLOT(onClickedBackButton()));
			QObject::connect(m_filterComboBox, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(onFilterChanged(const QString&)));
			QObject::connect(m_productCheckBox, SIGNAL(toggled(bool)), this, SLOT(onCheckBoxChecked(bool)));
			QObject::connect(m_dateResetButton, SIGNAL(clicked()), this, SLOT(onResetDateEditWidget()));
		}
		else
		{
			QObject::disconnect(ui_hideAndShowButton, SIGNAL(clicked()), this, SLOT(onHideHirarchy(bool)));
			QObject::disconnect(m_hierarchyTreeWidget, SIGNAL(itemClicked(QTreeWidgetItem*, int)), this, SLOT(onTreeNodeClicked(QTreeWidgetItem*, int)));
			QObject::disconnect(m_searchButton, SIGNAL(clicked()), this, SLOT(onClickedSubmitButton())); // should be used
			QObject::disconnect(m_backButton, SIGNAL(clicked()), this, SLOT(onClickedBackButton()));
			QObject::disconnect(m_productCheckBox, SIGNAL(toggled(bool)), this, SLOT(onCheckBoxChecked(bool)));
			QObject::disconnect(m_filterComboBox, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(onFilterChanged(const QString&)));
			QObject::disconnect(m_dateResetButton, SIGNAL(clicked()), this, SLOT(onResetDateEditWidget()));
		}
		Logger::Info("PLMSampleSearch -> connectSignalSlots() -> End");
	}

	/*
	* Description - ClickedBackButton() method is a slot for back button click and used to go back design suite.
	* Parameter -
	* Exception -
	* Return -
	*/
	void PLMSampleSearch::ClickedBackButton()
	{
		Logger::Info("PLMSampleSearch -> ClickedBackButton() -> Start");
		CLOVise::CLOViseSuite::GetInstance()->setModal(true);
		CLOViseSuite::GetInstance()->show();
		Destroy();
		Logger::Info("PLMSampleSearch -> ClickedBackButton() -> End");
	}

	/*
	* Description - OnClickedBackButton() method is a slot for back button click and used to go back design suite.
	* Parameter -
	* Exception -
	* Return -
	*/
	void PLMSampleSearch::onClickedBackButton()
	{
		Logger::Info("PLMSampleSearch -> onClickedBackButton() -> Start");
		ClickedBackButton();
		Logger::Info("PLMSampleSearch -> onClickedBackButton() -> End");
	}

	/*
	* Description - ClickedSubmitButton() method is a slot for submit button click, validate the input and calling the result table module.
	* Parameter -
	* Exception - exception, Char *
	* Return -
	*/
	void PLMSampleSearch::ClickedSubmitButton()
	{
		Logger::Info("PLMSampleSearch -> ClickedSubmitButton() -> Start");
		try
		{
			collectSearchUIFields();
			UIHelper::ValidateForValidParams(m_searchCriteriaJson, SAMPLE_MODULE);
			SampleConfig::GetInstance()->SetSelectedViewIdx(m_viewComboBox->currentIndex());
			SampleConfig::GetInstance()->SetSearchCriteriaJSON(m_searchCriteriaJson);
			this->hide();
			//tmporarilry clearing Results object
			//..... need to implement result related funtionalities.
			RESTAPI::SetProgressBarData(15, "Searching Sample", true);
			//PLMSampleResult::Destroy();
			PLMSampleResult::GetInstance()->setModal(true);
			PLMSampleResult::GetInstance()->exec();
			RESTAPI::SetProgressBarData(0, "", false);
		}
		catch (exception& e)
		{
			RESTAPI::SetProgressBarData(0, "", false);
			Logger::Error("PLMSampleSearch -> OnClickedSubmitButton Exception :: " + string(e.what()));
			UTILITY_API->DisplayMessageBox(e.what());
			this->show();
		}
		catch (const char* msg)
		{
			Logger::Error("PLMSampleSearch -> OnClickedSubmitButton Exception :: " + string(msg));
			wstring wstr(msg, msg + strlen(msg));
			RESTAPI::SetProgressBarData(0, "", false);
			UTILITY_API->DisplayMessageBoxW(wstr);
			this->show();
		}
		Logger::Info("PLMSampleSearch -> ClickedSubmitButton() -> End");
	}

	/*
	* Description - onClickedSubmitButton() method is a slot for submit button click, validate the input and calling the result table module.
	* Parameter -
	* Exception - exception, Char *
	* Return -
	*/
	void PLMSampleSearch::onClickedSubmitButton()
	{
		Logger::Info("PLMSampleSearch -> onClickedSubmitButton() -> Start");
		ClickedSubmitButton();
		Logger::Info("PLMSampleSearch -> onClickedSubmitButton() -> End");
	}

	/*
	* Description - FilterChanged() method is a slot for filter item changed and update/create search UI fields.
	* Parameter -  QString.
	* Exception - exception, Char *
	* Return -
	*/
	void PLMSampleSearch::FilterChanged(const QString& _item)
	{
		Logger::Info("PLMSampleSearch -> FilterChanged() -> Start");
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
		catch (exception& e)
		{
			Logger::Error("PLMSampleSearch -> FilterChanged Exception :: " + string(e.what()));
			UTILITY_API->DisplayMessageBox(e.what());
			this->close();
		}
		catch (const char* msg)
		{
			Logger::Error("PLMSampleSearch -> FilterChanged Exception :: " + string(msg));
			wstring wstr(msg, msg + strlen(msg));
			UTILITY_API->DisplayMessageBoxW(wstr);
			this->close();
		}
		Logger::Info("PLMSampleSearch -> FilterChanged() -> End");
	}

	/*
	* Description - onFilterChanged() method is a slot for filter item changed and update/create search UI fields.
	* Parameter -  QString.
	* Exception - exception, Char *
	* Return -
	*/
	void PLMSampleSearch::onFilterChanged(const QString& _item)
	{
		Logger::Info("PLMSampleSearch -> onFilterChanged() -> Start");
		FilterChanged(_item);
		Logger::Info("PLMSampleSearch -> onFilterChanged() -> End");
	}

	/*
	* Description - CheckBoxChecked() method is a slot for check box  for search through season and palette.
	* Parameter -  bool.
	* Exception - exception, Char *
	* Return -
	*/
	void PLMSampleSearch::CheckBoxChecked(bool _checked)
	{
		Logger::Info("PLMSampleSearch -> CheckBoxChecked() -> Start");
		try
		{
			if (_checked)
			{
				m_productNameLabel->show();
				m_productNameLineEdit->show();
			}
			else
			{
				m_productNameLineEdit->setText("");
				m_productNameLineEdit->hide();
				m_productNameLabel->hide();
				m_productIdLineEdit->setText("");
				m_productIdLineEdit->hide();
				m_productIdLabel->hide();
			}
		}
		catch (exception& e)
		{
			Logger::Error("PLMSampleSearch -> CheckBoxChecked Exception :: " + string(e.what()));
			UTILITY_API->DisplayMessageBox(e.what());
			m_productCheckBox->setCheckState(Qt::Unchecked);
		}
		catch (const char* msg)
		{
			Logger::Error("PLMSampleSearch -> CheckBoxChecked Exception :: " + string(msg));
			wstring wstr(msg, msg + strlen(msg));
			UTILITY_API->DisplayMessageBoxW(wstr);
			m_productCheckBox->setCheckState(Qt::Unchecked);
		}
		Logger::Info("PLMSampleSearch -> CheckBoxChecked() -> End");
	}

	/*
	* Description - onCheckBoxChecked() method is a slot for check box  for search through season and palette.
	* Parameter -  bool.
	* Exception - exception, Char *
	* Return -
	*/
	void PLMSampleSearch::onCheckBoxChecked(bool _checked)
	{
		Logger::Info("PLMSampleSearch -> onCheckBoxChecked() -> Start");
		CheckBoxChecked(_checked);
		Logger::Info("PLMSampleSearch -> onCheckBoxChecked() -> End");
	}

	/*
	* Description - TreeNodeClicked() method is a slot for hierarchy is click.
	* Parameter -  QTreeWidgetItem, int.
	* Exception - exception, Char *
	* Return -
	*/
	void PLMSampleSearch::TreeNodeClicked(QTreeWidgetItem* _item, int _column)
	{
		Logger::Info("PLMSampleSearch -> TreeNodeClicked() -> Start");
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
				CVWidgetGenerator::CreateViewComboBoxOnSearch(m_viewComboBox, SampleConfig::GetInstance()->GetSampleViewJSON(), selectType.toStdString());
				drawSearchUI(selectType, false, BLANK);
				m_hierarchyLoading = false;
				UTILITY_API->DeleteProgressBar(true);
				this->show();
			}
		}
		catch (exception& e)
		{
			Logger::Error("PLMSampleSearch -> TreeNodeClicked Exception :: " + string(e.what()));
			UTILITY_API->DisplayMessageBox(e.what());
			this->close();
		}
		catch (const char* msg)
		{
			Logger::Error("PLMSampleSearch -> TreeNodeClicked Exception :: " + string(msg));
			wstring wstr(msg, msg + strlen(msg));
			UTILITY_API->DisplayMessageBoxW(wstr);
			this->close();
		}
		Logger::Info("PLMSampleSearch -> TreeNodeClicked() -> End");
	}

	/*
	* Description - onTreeNodeClicked() method is a slot for hierarchy is click.
	* Parameter -  QTreeWidgetItem, int.
	* Exception - exception, Char *
	* Return -
	*/
	void PLMSampleSearch::onTreeNodeClicked(QTreeWidgetItem* _item, int _column)
	{
		Logger::Info("PLMSampleSearch -> onTreeNodeClicked() -> Start");
		TreeNodeClicked(_item, _column);
		Logger::Info("PLMSampleSearch -> onTreeNodeClicked() -> End");
	}

	/*
	* Description - drawSearchUI() method used to create search fields in search UI.
	* Parameter -  QString, bool, string.
	* Exception - exception, Char *
	* Return -
	*/
	void PLMSampleSearch::drawSearchUI(QString _selectType, bool _drawFilter, string _selectedFilter)
	{
		Logger::Info("PLMSampleSearch -> drawSearchUI() -> Start");
		try
		{
			SampleConfig::GetInstance()->SetDateFlag(false);
			QStringList attScops = SampleConfig::GetInstance()->GetAttScopes();
			//if (!CVWidgetGenerator::DrawFilterAndSearchCriteriaWidget(SampleConfig::GetInstance()->GetSampleFilterJSON(), m_filterComboBox, m_searchTreeWidget, _selectType, _selectedFilter, attScops, _drawFilter))
			//{
			//	//FlexTypeHelper::DrawDefaultSearchCriteriaWidget(SampleConfig::GetInstance()->GetSampleFieldsJSON(), _selectType.toStdString(), m_searchTreeWidget, attScops);
			//}
			m_searchTreeWidget->setColumnCount(3);
			m_searchTreeWidget->setHeaderHidden(true);
			m_searchTreeWidget->setWordWrap(true);
			m_searchTreeWidget->setDropIndicatorShown(false);
			m_searchTreeWidget->setRootIsDecorated(false);
			m_searchTreeWidget->setSelectionMode(QAbstractItemView::NoSelection);
			m_searchTreeWidget->setStyleSheet("QTreeWidget { background-color: #262628; border: 1px solid #000; padding-left: 20px; min-width: 400px; }""QTreeWidget::item {height: 20px; width: 200px; margin-right: 20px; margin-top: 5px; margin-bottom: 5px; border: none; }""QTreeWidget::item:hover{ background-color: #262628; }");

			if (!SampleConfig::GetInstance()->GetDateFlag())
				m_dateResetButton->hide();
			else
				m_dateResetButton->show();

		}
		catch (exception e)
		{
			throw e;
			CLOViseSuite::GetInstance()->show();
			Destroy();
		}
		Logger::Info("PLMSampleSearch -> drawSearchUI() -> End");
	}

	/*
	* Description - collectSearchUIFields() method used collect all the user input from search UI.
	* Parameter -
	* Exception -
	* Return -
	*/
	void  PLMSampleSearch::collectSearchUIFields()
	{
		Logger::Info("PLMSampleSearch -> collectSearchUIFields() -> Start");
		string productNameValue = "";
		string productIdValue = "";
		string quickSearchValue = "";
		string filterValue = "";
		m_searchCriteriaJson.clear();
		if (m_productCheckBox->checkState())
		{
			productNameValue = m_productNameLineEdit->text().toStdString();
			productIdValue = m_productIdLineEdit->text().toStdString();
			m_searchCriteriaJson[PRODUCTNAME_KEY] = productNameValue;
			m_searchCriteriaJson[PRODUCTID_KEY] = productIdValue;
			m_searchCriteriaJson[SEASONID_KEY] = BLANK;
			m_searchCriteriaJson[PALETTEID_KEY] = BLANK;
		}
		else
		{
			m_searchCriteriaJson[SEASONID_KEY] = "";
			m_searchCriteriaJson[PALETTEID_KEY] = "";
			m_searchCriteriaJson[PRODUCTNAME_KEY] = productNameValue;
			m_searchCriteriaJson[PRODUCTID_KEY] = productIdValue;
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
		m_searchCriteriaJson[MODULE_KEY] = SAMPLE_MODULE;
		m_searchCriteriaJson[INCLUDE_SUPPLIER_STRING_KEY] = "";
		m_searchCriteriaJson[SEARCHBY_SUPPLIER_STRING_KEY] = "";
		m_searchCriteriaJson[REQ_FROM_CLO_KEY] = "true";
		m_searchCriteriaJson[ATTRIBUTES_KEY] = UIHelper::CollectSearchCriteriaFields(m_searchTreeWidget);
		Logger::Info("PLMSampleSearch -> collectSearchUIFields() -> End");
	}
}