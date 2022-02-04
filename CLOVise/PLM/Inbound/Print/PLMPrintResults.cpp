/*
* Copyright 2020-2021 CLO-Vise. All rights reserved
*
* @file PLMPrintResults.cpp
*
* @brief Class implementation for cach Prints data in CLO from PLM.
* This class has all the variable and methods implementation which are used in storing PLM Print instance data to view and download in CLO.
*
* @author GoVise
*
* @date  30-MAY-2020
*/
#include "PLMPrintResults.h"

#include <iostream>
#include <string>

#include "CLOVise/PLM/Helper/UIHelper/CVWidgetGenerator.h"
#include "CLOVise/PLM/Inbound/Print/PLMPrintSearch.h"
#include "CLOVise/PLM/Inbound/Print/PrintConfig.h"
#include "CLOVise/PLM/Helper/Util/FormatHelper.h"
#include "CLOVise/PLM/Helper/Util/RestAPIUtility.h"
#include "CLOVise/PLM/Helper/UIHelper/UIHelper.h"
#include "CLOVise/PLM/Helper/UIHelper/CVDisplayMessageBox.h"
#include "CLOVise/PLM/Helper/UIHelper/CVHoverDelegate.h"
#include "CLOVise/PLM/Helper/Util/Logger.h"
#include "CLOVise/PLM/Helper/UIHelper/CVWidgetGenerator.h"
#include "CLOVise/PLM/Outbound/PublishToPLM/CreateProduct.h"
#include "CLOVise/PLM/Outbound/PublishToPLM/UpdateProduct.h"

using namespace std;

namespace CLOVise
{
	PLMPrintResults* PLMPrintResults::_instance = NULL;


	PLMPrintResults* PLMPrintResults::GetInstance()
	{
		if (_instance == NULL)
			_instance = new PLMPrintResults();
		return _instance;
	}
	
	PLMPrintResults::PLMPrintResults(QWidget* parent)
		: MVDialog(parent)
	{
		Logger::Info("PLMPrintResults -> Constructor() -> Start");
		setupUi(this);
		try
		{
			QString windowTitle = PLM_NAME + " PLM Print Design Color Search Results ";
			this->setWindowTitle(windowTitle);

#ifdef __APPLE__
			this->setWindowFlags(Qt::Dialog | Qt::CustomizeWindowHint | Qt::WindowMinMaxButtonsHint | Qt::WindowCloseButtonHint);
#else
			this->setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint | Qt::CustomizeWindowHint);
			m_pTitleBar = new MVTitleBar(windowTitle, this);
			layout()->setMenuBar(m_pTitleBar);
#endif // !__APPLE__

			 resultTable = nullptr;
			 iconTable = nullptr;
			 m_viewComboBox = nullptr;
			 m_perPageResultComboBox = nullptr;
			 m_noOfResultLabel = nullptr;
			 m_tabViewButton = nullptr;
			 m_iconViewButton = nullptr;
			 m_backButton = nullptr;
			 m_downloadButton = nullptr;
			 m_deSelectAllButton = nullptr;
			 m_selectAllButton = nullptr;

			QSpacerItem *horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Fixed);
			QSpacerItem *horizontalSpacer_3 = new QSpacerItem(20, 20, QSizePolicy::Fixed, QSizePolicy::Fixed);
			//QSpacerItem *horizontalSpacer_4 = new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Fixed);

			ui_toolLayout->insertSpacerItem(2, horizontalSpacer_2);
			ui_toolLayout->insertSpacerItem(5, horizontalSpacer_3);
			ui_toolLayout->insertSpacerItem(8, horizontalSpacer_3);

			//bool permission;
			//string permission = Helper::GetJSONValue<string>(m_widgetListJson, "tabularView", true);

			//permission = Helper::GetJSONValue<string>(m_widgetListJson, "resultCount", true);
			QLabel *resultCountLabel = CVWidgetGenerator::CreateLabel("Result Count", "", HEADER_STYLE, true);
			ui_toolLayout->insertWidget(0, resultCountLabel);

			m_noOfResultLabel = CVWidgetGenerator::CreateLabel("", "", HEADER_STYLE, true);
			m_noOfResultLabel->setFocusPolicy(Qt::NoFocus);
			ui_toolLayout->insertWidget(1, m_noOfResultLabel);

			QLabel *viewLabel = CVWidgetGenerator::CreateLabel("Views", "", HEADER_STYLE, false);
			ui_toolLayout->insertWidget(3, viewLabel);

			m_viewComboBox = CVWidgetGenerator::CreateComboBox("", VIEW_COMBOBOX_STYLE, false);
			m_viewComboBox->setFocusPolicy(Qt::NoFocus);
			ui_toolLayout->insertWidget(4, m_viewComboBox);

			QLabel *perPageLabel = CVWidgetGenerator::CreateLabel("Per Page", "", HEADER_STYLE, false);
			ui_toolLayout->insertWidget(6, perPageLabel);


			m_perPageResultComboBox = CVWidgetGenerator::CreateComboBox("", COMBOBOX_STYLE, false);
			m_perPageResultComboBox->setFocusPolicy(Qt::NoFocus);
			ui_toolLayout->insertWidget(7, m_perPageResultComboBox);

			m_tabViewButton = CVWidgetGenerator::CreateToolButton(VIEW_LIST_HOVER_ICON_PATH, TABULAR_VIEW, TOOLTIP_STYLE, false);
			m_tabViewButton->setFocusPolicy(Qt::NoFocus);
			ui_toolLayout->insertWidget(8, m_tabViewButton);

			m_iconViewButton = CVWidgetGenerator::CreateToolButton(VIEW_THUMB_HOVER_ICON_PATH, ICON_VIEW, TOOLTIP_STYLE, false);
			m_iconViewButton->setFocusPolicy(Qt::NoFocus);
			ui_toolLayout->insertWidget(9, m_iconViewButton);


			QSpacerItem *horizontalSpacer_4 = new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Fixed);
			QSpacerItem *horizontalSpacer_5 = new QSpacerItem(10, 20, QSizePolicy::Expanding, QSizePolicy::Fixed);

			//ui_buttonLayout->insertSpacerItem(0, horizontalSpacer_4);
			m_backButton = CVWidgetGenerator::CreatePushButton("Back", BACK_HOVER_PATH, "Back", BUTTON_STYLE, 30, true);
			m_backButton->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
			m_backButton->setFocusPolicy(Qt::NoFocus);
			//m_back->setMaximumSize()
			ui_buttonLayout->insertWidget(1, m_backButton);

			ui_buttonLayout->insertSpacerItem(2, horizontalSpacer_5);
			m_selectAllButton = CVWidgetGenerator::CreatePushButton("Select All", OK_NONE_ICON_PATH, "Unselect All", PUSH_BUTTON_STYLE, 30, true);
			m_selectAllButton->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
			m_selectAllButton->setFocusPolicy(Qt::NoFocus);
			ui_buttonLayout->insertWidget(3, m_selectAllButton);

			m_deSelectAllButton = CVWidgetGenerator::CreatePushButton("Unselect All", OK_NONE_ICON_PATH, "Unselect All", PUSH_BUTTON_STYLE, 30, true);
			m_deSelectAllButton->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
			m_deSelectAllButton->setFocusPolicy(Qt::NoFocus);
			ui_buttonLayout->insertWidget(4, m_deSelectAllButton);
			ui_buttonLayout->insertSpacerItem(4, horizontalSpacer_5);

			m_downloadButton = CVWidgetGenerator::CreatePushButton("Download", DOWNLOAD_HOVER_ICON_PATH, "Download", PUSH_BUTTON_STYLE, 30, true);
			m_downloadButton->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
			m_downloadButton->setFocusPolicy(Qt::StrongFocus);
			
			ui_buttonLayout->insertWidget(5, m_downloadButton);

			
			//ui_buttonLayout->insertSpacerItem(6, horizontalSpacer_4);

			//label_3->setStyleSheet(HEADER_STYLE);
			//noOfResultLabel->setStyleSheet(HEADER_STYLE);
			/*label->setStyleSheet(HEADER_STYLE);
			label_4->setStyleSheet(HEADER_STYLE);*/
			//m_viewComboBox->setStyleSheet(VIEW_COMBOBOX_STYLE);
			//m_perPageResultComboBox->setStyleSheet(COMBOBOX_STYLE);

			previousButton->setToolTip(PREVIOUS);
			previousButton->setStyleSheet(PREVIOUS_BUTTON_STYLE);
			nextButton->setToolTip(NEXT);
			nextButton->setStyleSheet(NEXT_BUTTON_STYLE);

			//DeSelectAll->setIcon(QIcon(":/CLOVise/PLM/Images/icon_ok_none.svg"));

			//back->setIcon(QIcon(":/CLOVise/PLM/Images/icon_back_over.svg"));

			//download->setIcon(QIcon(":/CLOVise/PLM/Images/icon_down_over.svg"));
			/*back->setStyleSheet(BUTTON_STYLE);
			DeSelectAll->setStyleSheet(BUTTON_STYLE);
			download->setStyleSheet(BUTTON_STYLE);*/

			//iconView->setToolTip(ICON_VIEW);

			//tabView->setToolTip(TABULAR_VIEW);

			resultTable = new MVTableWidget();

			CVWidgetGenerator::InitializeTableView(resultTable);
			CVHoverDelegate* customHoverDelegate = new CVHoverDelegate(resultTable);
			resultTable->setItemDelegate(customHoverDelegate);

			QPalette pal;
			pal.setColor(QPalette::Highlight, highlightColor);
			pal.setColor(QPalette::HighlightedText, highlightedTextColor);
			resultTable->setPalette(pal);
			iconTable = new QListWidget();
			ui_resultTableLayout->addWidget(resultTable);
			DrawResultWidget(true);
			//setDataFromResponse(ColorConfig::GetInstance()->GetSearchCriteriaJSON());
		
			connectSignalSlots(true);

			RESTAPI::SetProgressBarData(0, "", false);
		}
		catch (string msg)
		{
			
			throw msg;
		}
		catch (exception& e)
		{
			
			throw e;
		}
		catch (const char* msg)
		{
			
			throw msg;
		}
		Logger::Info("PLMPrintResults -> Constructor() -> End");
	}

	PLMPrintResults::~PLMPrintResults()
	{
		Logger::Info("PLMPrintResults -> Destructor() -> Start");
		
	
	
		
		
		Logger::Info("PLMPrintResults -> Destructor() -> End");
	}

	/*
	* Description - SetPrintHierarchyJSON() method used to cache the hierarchy data.
	* Parameter -  json.
	* Exception -
	* Return -
	*/
	void PLMPrintResults::setDataFromResponse(json _param)
	{
		try
		{
			string resultJsonString = RESTAPI::RESTMethodSubmit(RESTAPI::SEARCH_RESULTS_API, _param);
			if (!FormatHelper::HasContent(resultJsonString))
			{
				throw "Unable to fetch results. Please try again or Contact your System Administrator.";
			}

			if (FormatHelper::HasError(resultJsonString))
			{
				throw runtime_error(resultJsonString);
			}
			string error = RESTAPI::CheckForErrorMsg(resultJsonString);
			if (FormatHelper::HasContent(error))
			{
				throw std::logic_error(error);
			}
			m_printResults = json::parse(resultJsonString);
			string resultsCount = Helper::GetJSONValue<string>(m_printResults, "resultFound", true);
			if (FormatHelper::HasContent(resultsCount))
			{
				m_resultsCount = stoi(resultsCount);
			}
			else
			{
				m_resultsCount = 0;
			}
			string maxResultsLimit = Helper::GetJSONValue<string>(m_printResults, "maxResultsLimit", true);
			if (FormatHelper::HasContent(maxResultsLimit))
			{
				m_maxResultsCount = stoi(maxResultsLimit);
			}
			else
			{
				m_maxResultsCount = 50;
			}
			m_typename = Helper::GetJSONValue<string>(m_printResults, TYPENAME_KEY, true);
			//	m_widgetListJson = Helper::GetJSONValue<string>(m_colorResults, "widgetsList", true);
		}
		catch (string msg)
		{
			UTILITY_API->DeleteProgressBar(true);
			Logger::Error("PLMPrintResults::setDataFromResponse() Exception - " + msg);
		}
		catch (exception & e)
		{
			UTILITY_API->DeleteProgressBar(true);
			Logger::Error("PLMPrintResults::setDataFromResponse() Exception - " + string(e.what()));
		}
		catch (const char* msg)
		{
			UTILITY_API->DeleteProgressBar(true);
			wstring wstr(msg, msg + strlen(msg));
			Logger::Error("PLMPrintResults::setDataFromResponse() Exception - " + string(msg));
		}
	}

	/*
	* Description - AddConnectorForCheckbox() method used to create a connection method for checkbox.
	* Parameter -
	* Exception -
	* Return -
	*/
	void PLMPrintResults::AddConnectorForCheckbox()
	{
		Logger::Info("PLMPrintResults -> AddConnectorForCheckbox() -> Start");
		int currentPageValue = currPageLabel->text().toInt();
		int m_resultPerPage = m_perPageResultComboBox->currentText().toInt();
		
		QRadioButton* tempRadioButton;
		QCheckBox* tempCheckBox;
		for (int row = 0; row < resultTable->rowCount(); row++) 
		{
			QWidget* qWidget = resultTable->cellWidget(row, 0); //to select multiple rows
			if (Configuration::GetInstance()->GetIsPrintSearchClicked())
			{
				tempRadioButton = qobject_cast<QRadioButton*>(qWidget);
				QObject::connect(tempRadioButton, SIGNAL(clicked()), this, SLOT(TableRadioButtonSelected()));
			}
			else
			{
				tempCheckBox = qobject_cast <QCheckBox*> (qWidget->layout()->itemAt(0)->widget());
				QObject::connect(tempCheckBox, SIGNAL(clicked()), this, SLOT(onTableCheckBoxSelected()));
			}
		}
		Logger::Info("PLMPrintResult -> AddConnectorForCheckbox() -> End");
	}

	void PLMPrintResults::TableRadioButtonSelected()
	{
		Logger::Info("PLMPrintResults -> TableRadioButtonSelected() -> Start");
		try
		{
			m_rowsSelected.clear();
			m_iconsSelected.clear();
			if (ProductConfig::GetInstance()->m_isShow3DAttWidget)
			{
				this->hide();
				UTILITY_API->CreateProgressBar();
				RESTAPI::SetProgressBarData(10, "Loading 3D Documents..", true);
				m_rowsSelected = CVWidgetGenerator::TableRowSelected(resultTable, iconTable, true, false);
				m_totalSelected = CVWidgetGenerator::SetDownloadCount(m_iconsSelected, m_rowsSelected, m_downloadButton);
				this->show();
			}
			else
			{
				m_rowsSelected = CVWidgetGenerator::TableRowSelected(resultTable, iconTable, true, false);
				m_totalSelected = CVWidgetGenerator::SetDownloadCount(m_iconsSelected, m_rowsSelected, m_downloadButton);
			}
			Logger::Info("PLMPrintResults -> TableRadioButtonSelected() -> end");
		}
		catch (string msg)
		{
			Logger::Error("PLMPrintResults -> TableRadioButtonSelected Exception :: " + msg);
			UTILITY_API->DisplayMessageBox(msg);
		}
		catch (exception& e)
		{
			Logger::Error("PLMPrintResults -> TableRadioButtonSelected Exception :: " + string(e.what()));
			UTILITY_API->DisplayMessageBox(e.what());
		}
		catch (const char* msg)
		{
			Logger::Error("PLMPrintResults -> TableRadioButtonSelected Exception :: " + string(msg));
			wstring wstr(msg, msg + strlen(msg));
			UTILITY_API->DisplayMessageBoxW(wstr);
		}
	}
	/*
	* Description - TableCheckBoxSelected() method is a slot for table check box select.
	* Parameter -
	* Exception - exception, Char *
	* Return -
	*/
	void PLMPrintResults::onTableCheckBoxSelected()
	{
		Logger::Info("PLMPrintResults -> onTableCheckBoxSelected() -> Start");
		TableCheckBoxSelected();
		Logger::Info("PLMPrintResults -> onTableCheckBoxSelected() -> End");
	}

	void PLMPrintResults::TableCheckBoxSelected()
	{
		Logger::Info("PLMPrintResults -> TableCheckBoxSelected() -> Start");
		try
		{
			m_rowsSelected.clear();
			m_iconsSelected.clear();
			m_rowsSelected = CVWidgetGenerator::TableRowSelected(resultTable, iconTable, false, false);
			m_totalSelected = CVWidgetGenerator::SetDownloadCount(m_iconsSelected, m_rowsSelected, m_downloadButton);
		}
		catch (string msg)
		{
			Logger::Error("PLMPrintResults-> TableCheckBoxSelected Exception - " + msg);
			UTILITY_API->DisplayMessageBox(msg);
		}
		catch (exception& e)
		{
			Logger::Error("PLMPrintResults-> TableCheckBoxSelected Exception - " + string(e.what()));
			UTILITY_API->DisplayMessageBox(e.what());

		}
		catch (const char* msg)
		{
			Logger::Error("PLMPrintResults-> TableCheckBoxSelected Exception - " + string(msg));
			wstring wstr(msg, msg + strlen(msg));
			UTILITY_API->DisplayMessageBoxW(wstr);
		}
		Logger::Info("PLMPrintResults -> TableCheckBoxSelected() -> End");
	}

	
	void PLMPrintResults::setHeaderToolTip()
	{
		Logger::Info("PLMPrintResults -> setHeaderToolTip() -> Start");
		for (int i = 0; i < resultTable->columnCount(); i++)
		{
			QString headerValue = resultTable->horizontalHeaderItem(i)->text();
			resultTable->horizontalHeaderItem(i)->setToolTip(headerValue);
		}
		Logger::Info("PLMPrintResults -> setHeaderToolTip() -> Start");
	}

	/*
	* Description - onClickedDeselectAllButton() method is slot for deselect all click to unselect all selected item in the result.
	* Parameter -
	* Exception -
	* Return -
	*/
	void PLMPrintResults::onClickedDeselectAllButton()
	{
		Logger::Info("PLMPrintResults -> onClickedDeselectAllButton() -> Start");
		ClickedDeselectAllButton();
		Logger::Info("PLMPrintResults -> onClickedDeselectAllButton() -> End");
	}

	void PLMPrintResults::ClickedDeselectAllButton()
	{
		Logger::Info("PLMPrintResults -> ClickedDeselectAllButton() -> Start");
		CVWidgetGenerator::DeSelectAllClicked(resultTable, iconTable, m_downloadButton);
		m_rowsSelected.clear();
		m_iconsSelected.clear();
		m_totalSelected.clear();
		Logger::Info("PLMPrintResults -> ClickedDeselectAllButton() -> End");
	}
	
	/*
	* Description - onClickedDeselectAllButton() method is slot for deselect all click to unselect all selected item in the result.
	* Parameter -
	* Exception -
	* Return -
	*/
	void PLMPrintResults::onClickedSelectAllButton()
	{
		Logger::Info("PLMPrintResults -> onClickedSelectAllButton() -> Start");
		ClickedSelectAllButton();
		Logger::Info("PLMPrintResults -> onClickedSelectAllButton() -> End");
	}

	void PLMPrintResults::ClickedSelectAllButton()
	{
		Logger::Info("PLMPrintResults -> ClickedSelectAllButton() -> Start");
		m_rowsSelected.clear();
		m_iconsSelected.clear();
		m_totalSelected.clear();
		resultTable->clearSelection();
		QStringList selectedIds;
		int totalRowCount = resultTable->rowCount();
		resultTable->setSelectionMode(QAbstractItemView::MultiSelection);
		resultTable->setSelectionMode(QAbstractItemView::MultiSelection);
		for (int rowCount = 0; rowCount < totalRowCount; rowCount++)
		{
			QWidget* qWidget = resultTable->cellWidget(rowCount, 0);
			QCheckBox *checkBox = qobject_cast <QCheckBox*> (qWidget->layout()->itemAt(0)->widget());
			QString objectId = checkBox->property(OBJECT_ID_KEY.c_str()).toString();
			if (objectId == "centric%3A" || !FormatHelper::HasContent(objectId.toStdString()))
			{
				checkBox->setCheckState(Qt::Unchecked);
				continue;
			}
			else
			{
				checkBox->setCheckState(Qt::Checked);
				resultTable->selectRow(rowCount);
				if (!selectedIds.contains(objectId))
				{
					selectedIds.push_back(objectId);
				}
			}
		}
		m_rowsSelected = selectedIds;
		m_totalSelected = CVWidgetGenerator::SetDownloadCount(m_iconsSelected, m_rowsSelected, m_downloadButton);

		Logger::Info("PLMPrintResults -> ClickedSelectAllButton() -> End");
	}

	/*
	* Description - onClickedBackButton() method is slot for back button click.
	* Parameter -
	* Exception -
	* Return -
	*/
	void PLMPrintResults::onClickedBackButton()
	{
		Logger::Info("PLMPrintResults -> onClickedBackButton() -> Start");
		backButtonClicked();
		Logger::Info("PLMPrintResults -> onClickedBackButton() -> End");
	}

	void PLMPrintResults::backButtonClicked()
	{
		Logger::Info("PLMPrintResults -> backButtonClicked() -> Start");
		this->close();
		CLOVise::PLMPrintSearch::GetInstance()->setModal(true);
		CLOVise::PLMPrintSearch::GetInstance()->show();
		Logger::Info("PLMPrintResults -> backButtonClicked() -> End");
		
	}

	/*
	* Description - onClickedNextButton() method is a slot for next button click.
	* Parameter -
	* Exception -
	* Return -
	*/
	void PLMPrintResults::onClickedNextButton()
	{
		Logger::Info("PLMPrintResults -> onClickedNextButton() -> Start");
		ClickedNextButton();
		Logger::Info("PLMPrintResults -> onClickedNextButton() -> End");
	}

	void PLMPrintResults::ClickedNextButton()
	{
		Logger::Info("PLMPrintResults -> ClickedNextButton() -> Start");
		int currPage = currPageLabel->text().toInt() + 1;
		currPageLabel->setText(QString::fromStdString(to_string(currPage)));
		if (currPage == totalPageLabel->text().toInt())
			nextButton->setEnabled(false);
		if (m_isTabularView)
			CVWidgetGenerator::UpdateTableAndIconRows(resultTable, currPageLabel, m_perPageResultComboBox, m_resultsCount, false);
		else
			CVWidgetGenerator::UpdateTableAndIconRows(iconTable, currPageLabel, m_perPageResultComboBox, m_resultsCount);
		previousButton->setEnabled(true);
		Logger::Info("PLMPrintResults -> ClickedNextButton() -> End");
	}

	/*
	* Description - onClickedPreviousButton() method is a slot for previous button click.
	* Parameter -
	* Exception -
	* Return -
	*/
	void PLMPrintResults::onClickedPreviousButton()
	{
		Logger::Info("PLMPrintResults -> onClickedPreviousButton() -> Start");
		ClickedPreviousButton();
		Logger::Info("PLMPrintResults -> onClickedPreviousButton() -> End");
	}

	void PLMPrintResults::ClickedPreviousButton()
	{
		Logger::Info("PLMPrintResults -> ClickedPreviousButton() -> Start");
		int currPage = currPageLabel->text().toInt() - 1;
		currPageLabel->setText(QString::fromStdString(to_string(currPage)));
		if (currPage == 1)
			previousButton->setEnabled(false);
		if (m_isTabularView)
			CVWidgetGenerator::UpdateTableAndIconRows(resultTable, currPageLabel, m_perPageResultComboBox, m_resultsCount, false);
		else
			CVWidgetGenerator::UpdateTableAndIconRows(iconTable, currPageLabel, m_perPageResultComboBox, m_resultsCount);
		nextButton->setEnabled(true);
		Logger::Info("PLMPrintResults -> ClickedPreviousButton() -> End");
	}
	/*
	* Description - onResultPerPageCurrentIndexChanged() method is a slot for result per page click.
	* Parameter -  string.
	* Exception -
	* Return -
	*/
	void PLMPrintResults::onResultPerPageCurrentIndexChanged(const QString&)
	{
		Logger::Info("PLMPrintResults -> onResultPerPageCurrentIndexChanged() -> Start");
		int currPage = 1;
		currPageLabel->setText(QString::fromStdString(to_string(currPage)));
		if (currPage == 1)
			previousButton->setEnabled(false);
		CVWidgetGenerator::PopulateValuesOnResultsUI(nextButton, m_noOfResultLabel, totalPageLabel, m_perPageResultComboBox, m_resultsCount);
		CVWidgetGenerator::UpdateTableAndIconRows(resultTable, currPageLabel, m_perPageResultComboBox, m_resultsCount, false);
		CVWidgetGenerator::UpdateTableAndIconRows(iconTable, currPageLabel, m_perPageResultComboBox, m_resultsCount);
		//Caching selected option to remember irrespective of sessions
		Configuration::GetInstance()->SetSelectedResultsPerPage(m_perPageResultComboBox->currentText().toStdString(), true, false);
		Logger::Info("PLMPrintResults -> onResultPerPageCurrentIndexChanged() -> End");
	}

	/*
	* Description - onResultViewIndexChanged() method is a slot for  views click.
	* Parameter -  string.
	* Exception - exception, Char *
	* Return -
	*/

	void PLMPrintResults::onResultViewIndexChanged(const QString& _str)
	{
		Logger::Info("PLMPrintResults -> onResultViewIndexChanged() -> Start");
		ResultViewIndexChanged(_str);
		Logger::Info("PLMPrintResults -> onResultViewIndexChanged() -> End");
	}
	void PLMPrintResults::ResultViewIndexChanged(const QString&)
	{
		Logger::Info("PLMPrintResults -> ResultViewIndexChanged() -> Start");
		try
		{
			UTILITY_API->DeleteProgressBar(true);

			this->hide();

			UTILITY_API->CreateProgressBar();
			UTILITY_API->SetProgress("Loading", (qrand() % 101));
			Logger::Info("PLMPrintResults -> onResultViewIndexChanged() -> Start");
			m_totalSelected.clear();
			m_rowsSelected.clear();
			m_iconsSelected.clear();
			m_downloadButton->setText(DOWNLOAD_TEXT);
			resultTable->setEnabled(false);
			resultTable->clear();
			resultTable->reset();
			resultTable->setColumnCount(0);
			CVWidgetGenerator::InitializeTableView(resultTable);
			iconTable->clear();
			CVWidgetGenerator::DrawViewAndResultsWidget(m_viewComboBox, resultTable, iconTable, false, m_printResults, PrintConfig::GetInstance()->GetPrintViewJSON(), "Color Image", m_typename, false, m_viewComboBox->currentIndex(), PrintConfig::GetInstance()->GetAttScopes(), false, false, PrintConfig::GetInstance()->GetPrintViewJSON());
			onTabViewClicked();
			setHeaderToolTip();
			AddConnectorForCheckbox();
			resultTable->setEnabled(true);
			RESTAPI::SetProgressBarData(0, "", false);
			this->show();

		}
		catch (string msg)
		{
			Logger::Error("PLMPrintResults -> onResultViewIndexChanged Exception :: " + msg);
			UTILITY_API->DisplayMessageBox(msg);
			RESTAPI::SetProgressBarData(0, "", false);
			this->close();
		}
		catch (exception& e)
		{
			Logger::Error("PLMPrintResults -> onResultViewIndexChanged Exception :: " + string(e.what()));
			UTILITY_API->DisplayMessageBox(e.what());
			RESTAPI::SetProgressBarData(0, "", false);
			this->close();
		}
		catch (const char* msg)
		{
			Logger::Error("PLMPrintResults -> onResultViewIndexChanged Exception :: " + string(msg));
			wstring wstr(msg, msg + strlen(msg));
			UTILITY_API->DisplayMessageBoxW(wstr);
			RESTAPI::SetProgressBarData(0, "", false);
			this->close();
		}
		Logger::Info("PLMPrintResults -> onResultViewIndexChanged() -> End");
	}

	/*
	* Description - SetPrintHierarchyJSON() method is a slot for icon view click.
	* Parameter -
	* Exception -
	* Return -
	*/
	void PLMPrintResults::onIconViewClicked()
	{
		Logger::Info("PLMPrintResults -> onIconViewClicked() -> Start");
		updateTableForIconView();
		Logger::Info("PLMPrintResults -> onIconViewClicked() -> End");
	}

	void PLMPrintResults::updateTableForIconView()
	{
		Logger::Info("PLMPrintResults -> updateTableForIconView() -> Start");
		m_isTabularView = false;
		m_iconViewButton->setEnabled(false);
		m_tabViewButton->setEnabled(true);
		resultTable->close();
		if (m_isResultTableSorted)
		{
			this->hide();
			RESTAPI::SetProgressBarData(0, "", false);
			UTILITY_API->CreateProgressBar();
			UTILITY_API->SetProgress("Loading", (qrand() % 101));
			CVWidgetGenerator::SortIconResultTable(m_isResultTableSorted, iconTable, resultTable, m_printResults, false);
			iconTable->show();
			ui_resultTableLayout->addWidget(iconTable);
			CVWidgetGenerator::UpdateTableAndIconRows(iconTable, currPageLabel, m_perPageResultComboBox, m_resultsCount);
			CVWidgetGenerator::SetDownloadCount(m_iconsSelected, m_rowsSelected, m_downloadButton); iconTable->show();
			AddConnectorForCheckbox();
			RESTAPI::SetProgressBarData(0, "", false);
			this->show();
		}
		else
		{
			iconTable->show();
			ui_resultTableLayout->addWidget(iconTable);
			CVWidgetGenerator::UpdateTableAndIconRows(iconTable, currPageLabel, m_perPageResultComboBox, m_resultsCount);
		}
		Logger::Info("PLMPrintResults -> updateTableForIconView() -> End");
	}

	/*
	* Description - SetPrintHierarchyJSON() method is a slot for table view click.
	* Parameter -
	* Exception -
	* Return -
	*/
	void PLMPrintResults::onTabViewClicked()
	{
		Logger::Info("PLMPrintResults -> onTabViewClicked() -> Start");
		updateTableForTabView();
		Logger::Info("PLMPrintResults -> onTabViewClicked() -> End");
	}


	void PLMPrintResults::updateTableForTabView()
	{
		Logger::Info("PLMPrintResults -> updateTableForTabView() -> Start");
		m_isTabularView = true;
		m_tabViewButton->setEnabled(false);
		iconTable->close();
		m_iconViewButton->setEnabled(true);
		CVWidgetGenerator::UpdateTableAndIconRows(resultTable, currPageLabel, m_perPageResultComboBox, m_resultsCount, false);
		resultTable->show();
		ui_resultTableLayout->addWidget(resultTable);
		Logger::Info("PLMPrintResults -> updateTableForTabView() -> End");
	}

	/*
	* Description - onHorizontalHeaderClicked() method is a slot for horizontal header click and sorting the columns.
	* Parameter -  int.
	* Exception -
	* Return -
	*/
	void PLMPrintResults::onHorizontalHeaderClicked(int _column)
	{
		Logger::Info("PLMPrintResults -> onHorizontalHeaderClicked() -> Start");
		horizontalHeaderClicked(_column);
		Logger::Info("PLMPrintResults -> onHorizontalHeaderClicked() -> End");
	}

	void PLMPrintResults::horizontalHeaderClicked(int _column)
	{
		Logger::Info("PLMPrintResults -> horizontalHeaderClicked() -> Start");
		Logger::Debug("Column.." + to_string(_column));
		if (_column == CHECKBOX_COLUMN || _column == IMAGE_COLUMN)
			resultTable->setSortingEnabled(false);
		else
		{
			resultTable->setSortingEnabled(true);
			m_isResultTableSorted = true;
			AddConnectorForCheckbox();
			CVWidgetGenerator::UpdateTableAndIconRows(resultTable, currPageLabel, m_perPageResultComboBox, m_resultsCount, true);
		}
		Logger::Info("PLMPrintResults -> horizontalHeaderClicked() -> End");
	}

	/*
	* Description - connectSignalSlots() method used to connect and disconnect the signals and slots.
	* Parameter -  bool.
	* Exception -
	* Return -
	*/
	void PLMPrintResults::connectSignalSlots(bool _b)
	{
		Logger::Info("PLMPrintResults -> connectSignalSlots() -> Start");
		if (_b)
		{
			QObject::connect(m_backButton, SIGNAL(clicked()), this, SLOT(onClickedBackButton()));
			QObject::connect(nextButton, SIGNAL(clicked()), this, SLOT(onClickedNextButton()));
			QObject::connect(previousButton, SIGNAL(clicked()), this, SLOT(onClickedPreviousButton()));
			QObject::connect(m_perPageResultComboBox, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(onResultPerPageCurrentIndexChanged(const QString&)));
			QObject::connect(m_viewComboBox, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(onResultViewIndexChanged(const QString&)));
			QObject::connect(m_iconViewButton, SIGNAL(clicked()), this, SLOT(onIconViewClicked()));
			QObject::connect(m_tabViewButton, SIGNAL(clicked()), this, SLOT(onTabViewClicked()));
			QObject::connect(m_downloadButton, SIGNAL(clicked()), this, SLOT(onDownloadClicked()));
			QObject::connect(m_deSelectAllButton, SIGNAL(clicked()), this, SLOT(onClickedDeselectAllButton()));
			QObject::connect(m_selectAllButton, SIGNAL(clicked()), this, SLOT(onClickedSelectAllButton()));
			QObject::connect(resultTable->horizontalHeader(), SIGNAL(sectionClicked(int)), this, SLOT(onHorizontalHeaderClicked(int)));
		}
		else
		{
			QObject::disconnect(m_backButton, SIGNAL(clicked()), this, SLOT(onClickedBackButton()));
			QObject::disconnect(nextButton, SIGNAL(clicked()), this, SLOT(onClickedNextButton()));
			QObject::disconnect(previousButton, SIGNAL(clicked()), this, SLOT(onClickedPreviousButton()));
			QObject::disconnect(m_perPageResultComboBox, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(onResultPerPageCurrentIndexChanged(const QString&)));
			QObject::disconnect(m_viewComboBox, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(onResultViewIndexChanged(const QString&)));
			QObject::disconnect(m_iconViewButton, SIGNAL(clicked()), this, SLOT(onIconViewClicked()));
			QObject::disconnect(m_tabViewButton, SIGNAL(clicked()), this, SLOT(onTabViewClicked()));
			QObject::disconnect(m_downloadButton, SIGNAL(clicked()), this, SLOT(onDownloadClicked()));
			QObject::disconnect(m_deSelectAllButton, SIGNAL(clicked()), this, SLOT(onClickedDeselectAllButton()));
			QObject::disconnect(m_selectAllButton, SIGNAL(clicked()), this, SLOT(onClickedSelectAllButton()));
			QObject::disconnect(resultTable->horizontalHeader(), SIGNAL(sectionClicked(int)), this, SLOT(onHorizontalHeaderClicked(int)));
		}
		Logger::Info("PLMPrintResults -> connectSignalSlots() -> End");
	}

	/*
	* Description - DownloadClicked() method used to download the selected items.
	* Parameter -
	* Exception - exception, Char *
	* Return -
	*/
	void PLMPrintResults::onDownloadClicked()
	{
		Logger::Info("PLMPrintResults -> onDownloadClicked() -> Start");
		downloadPrintResult();
		Logger::Info("PLMPrintResults -> onDownloadClicked() -> End");
	}

	void PLMPrintResults::downloadPrintResult()
	{
		Logger::Info("PLMPrintResults -> downloadPrintResult() -> Start");
		bool rowsSelected = true;
		bool returnValue= false;
		try
		{

			if (m_totalSelected.isEmpty())
			{
				rowsSelected = false;
#ifdef __APPLE__
				throw string("Please select Results to download.");
#else
				throw runtime_error("Please select Results to download.");
#endif // __APPLE__
			}
			this->hide();
			UTILITY_API->CreateProgressBar();

			//json downloadJson = DownloadResults(m_totalSelected, COLOR_MODULE, m_colorResults);

			switch (Configuration::GetInstance()->GetCurrentScreen())
			{

			case CREATE_PRODUCT_CLICKED:
			{
				Logger::Debug("PLMPrintResults -> PrintResultTableDownload_clicked() -> CreateProduct::GetInstance()");
				RESTAPI::SetProgressBarData(30, "Adding Print Design Colors", true);
				UTILITY_API->SetProgress("Adding Print Design Colors", (qrand() % 101));

				RESTAPI::SetProgressBarData(14, "Adding Print Design Colors", true);
				if (Configuration::GetInstance()->GetIsPrintSearchClicked())
					returnValue = CreateProduct::GetInstance()->UpdateColorInColorways(m_totalSelected, m_printResults);

				if (returnValue)
				{
					Logger::Debug("PLMPrintResults -> void onDownloadClicked TRue");
					RESTAPI::SetProgressBarData(0, "", false);
					CreateProduct::GetInstance()->setModal(true);
					CreateProduct::GetInstance()->show();
				}
				else
				{
					Logger::Debug("PLMPrintResults -> void onDownloadClicked False");
					RESTAPI::SetProgressBarData(0, "", false);
					UTILITY_API->DisplayMessageBox("Print design color Specification must be unique");
					this->show();
				}
				
			}
			break;
			case UPDATE_PRODUCT_CLICKED:
			{
				Logger::Debug("PLMPrintResults -> colorResultTableDownload_clicked() -> CreateProduct::GetInstance()");
				RESTAPI::SetProgressBarData(30, "Adding Print Design Colors", true);
				UTILITY_API->SetProgress("Adding Print Design Colors", (qrand() % 101));

				RESTAPI::SetProgressBarData(14, "Adding Print Design Colors", true);
				if (Configuration::GetInstance()->GetIsPrintSearchClicked())
					returnValue = UpdateProduct::GetInstance()->UpdateColorInColorways(m_totalSelected, m_printResults);
				else
				{
					UpdateProduct::GetInstance()->AddColorwayDetails(m_totalSelected, m_printResults);
				}
				if (returnValue)
				{
					RESTAPI::SetProgressBarData(0, "", false);
					UpdateProduct::GetInstance()->setModal(true);
					UpdateProduct::GetInstance()->show();
				}
				else
				{
					Logger::Debug("PLMColorResults -> void onDownloadClicked False");
					RESTAPI::SetProgressBarData(0, "", false);
					UTILITY_API->DisplayMessageBox("The print design color Specification must be unique");
					this->show();
				}
			}
			break;
			case PRINT_SEARCH_CLICKED:
			{
				RESTAPI::SetProgressBarData(30, "Downloading Print Design Colors...", true);
				UTILITY_API->SetProgress("Downloading Print Design Colors", (qrand() % 101));

				RESTAPI::SetProgressBarData(14, "Downloading Print Design Colors...", true);
				QString faildesObjectsString = UIHelper::DownloadResults(m_totalSelected, PRINT_MODULE, m_printResults);
				RESTAPI::SetProgressBarData(0, "", false);

				if (FormatHelper::HasContent(faildesObjectsString.toStdString()))
				{
					string message;
					message = "Download Completed.\n\n Failed: ";
					int totalSelectedMaterial = m_totalSelected.size();
					int count = faildesObjectsString.count(QLatin1Char(','));

					if (faildesObjectsString.indexOf(',') == -1 || totalSelectedMaterial == count + 1)
						message = "Download failed.\n\n";

					UTILITY_API->DisplayMessageBox(message + faildesObjectsString.toStdString());
				}
				else
				{
					CVDisplayMessageBox DownloadDialogObject;
					DownloadDialogObject.DisplyMessage("Download Completed.");
					DownloadDialogObject.setModal(true);
					DownloadDialogObject.exec();
				}
				if (Configuration::GetInstance()->GetCloseResultsDialogue())
				{
					this->close();
					
					
				}
				else
				{
					if (!Configuration::GetInstance()->GetIsPrintSearchClicked())
					onClickedDeselectAllButton();
					this->show();
				}
			}
			}
			Logger::Info("PLMPrintResults -> void onDownloadClicked end");
		}
		catch (string msg)
		{
			Logger::Error("PLMPrintResults -> DownloadClicked Exception :: " + msg);
			UTILITY_API->DisplayMessageBox(msg);
			if (Configuration::GetInstance()->GetCloseResultsDialogue() && rowsSelected)
			{
				this->close();
				
			}
			else
			{
				if (!Configuration::GetInstance()->GetIsPrintSearchClicked())
				onClickedDeselectAllButton();
				this->show();
			}
		}
		catch (exception& e)
		{
			Logger::Error("PLMPrintResults -> DownloadClicked Exception :: " + string(e.what()));
			UTILITY_API->DisplayMessageBox(e.what());
			if (Configuration::GetInstance()->GetCloseResultsDialogue() && rowsSelected)
			{
				this->close();
				
			}
			else
			{
				if (!Configuration::GetInstance()->GetIsPrintSearchClicked())
				onClickedDeselectAllButton();
				this->show();
			}
		}
		catch (const char* msg)
		{
			Logger::Error("PLMPrintResults -> DownloadClicked Exception :: " + string(msg));
			wstring wstr(msg, msg + strlen(msg));
			UTILITY_API->DisplayMessageBoxW(wstr);
			if (Configuration::GetInstance()->GetCloseResultsDialogue() && rowsSelected)
			{
				this->close();
				
			}
			else
			{
				if (!Configuration::GetInstance()->GetIsPrintSearchClicked())
				onClickedDeselectAllButton();
				this->show();
			}
		}
		Logger::Info("PLMPrintResults -> downloadPrintResult() -> End");
	}

	void PLMPrintResults::DrawResultWidget(bool _isFromConstructor)
	{
		Logger::Info("PLMPrintResults -> DrawResultWidget() -> Start");		
		m_totalSelected.clear();
		m_printResults = PrintConfig::GetInstance()->GetPrintResultsSON();
		m_typename = PrintConfig::GetInstance()->GetTypename();
		m_maxResultsCount = PrintConfig::GetInstance()->GetMaxResultCount();
		m_resultsCount = PrintConfig::GetInstance()->GetResultsCount();
		resultTable->clearContents();
		CVWidgetGenerator::PopulateValuesOnResultsUI(nextButton, m_noOfResultLabel, totalPageLabel, m_perPageResultComboBox, Configuration::GetInstance()->GetResultsPerPage(), m_resultsCount);
		resultTable->setEnabled(false);
		m_tabViewButton->setEnabled(false);
		if (Configuration::GetInstance()->GetIsPrintSearchClicked())
		{
			CVWidgetGenerator::DrawViewAndResultsWidget(m_viewComboBox, resultTable, iconTable, false, m_printResults, PrintConfig::GetInstance()->GetPrintViewJSON(), "Color Image", m_typename, true, PrintConfig::GetInstance()->GetSelectedViewIdx(), PrintConfig::GetInstance()->GetAttScopes(), true, false, PrintConfig::GetInstance()->GetPrintViewJSON());
			m_selectAllButton->hide();
			m_deSelectAllButton->hide();
		}
		else
			CVWidgetGenerator::DrawViewAndResultsWidget(m_viewComboBox, resultTable, iconTable, false, m_printResults, PrintConfig::GetInstance()->GetPrintViewJSON(), "Color Image", m_typename, true, PrintConfig::GetInstance()->GetSelectedViewIdx(), PrintConfig::GetInstance()->GetAttScopes(), false, false, PrintConfig::GetInstance()->GetPrintViewJSON());

		CVWidgetGenerator::UpdateTableAndIconRows(resultTable, currPageLabel, m_perPageResultComboBox, m_resultsCount, _isFromConstructor);
		setHeaderToolTip();
		resultTable->setEnabled(true);
		AddConnectorForCheckbox();
		if (Configuration::GetInstance()->GetCurrentScreen() == PRINT_SEARCH_CLICKED)
		{
			m_downloadButton->setText("Download");
			m_downloadButton->setIcon(QIcon(QString(DOWNLOAD_HOVER_ICON_PATH)));
		}
		else
		{
			m_downloadButton->setText("Add");
			m_downloadButton->setIcon(QIcon(QString(ADD_HOVER_ICON_PATH)));
		}
		horizontalHeaderClicked(PrintConfig::GetInstance()->m_sortedColumnNumber);
		Logger::Info("PLMPrintResults -> DrawResultWidget() -> End");
	}

	void PLMPrintResults::reject()
	{
		//Configuration::GetInstance()->SetIsPrintSearchClicked(false);
		this->accept();
	}
}
