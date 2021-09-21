/*
* Copyright 2020-2021 CLO-Vise. All rights reserved
*
* @file PLMSampleResult.cpp
*
* @brief Class implementation for cach Sample data in CLO from PLM.
* This class has all the variable and methods implementation which are used in storing PLM Sample instance data to view and download in CLO.
*
* @author GoVise
*
* @date  30-MAY-2020
*/
#include "PLMSampleResult.h"

#include <iostream>
#include <string>

#include "CLOVise/PLM/Helper/UIHelper/CVWidgetGenerator.h"
#include "CLOVise/PLM/Inbound/Sample/PLMSampleSearch.h"
#include "CLOVise/PLM/Inbound/Sample/SampleConfig.h"
#include "CLOVise/PLM/Helper/Util/FormatHelper.h"
#include "CLOVise/PLM/Helper/Util/RestAPIUtility.h"
#include "CLOVise/PLM/Helper/UIHelper/UIHelper.h"
#include "CLOVise/PLM/Helper/UIHelper/CVDisplayMessageBox.h"
#include "CLOVise/PLM/Helper/UIHelper/CVHoverDelegate.h"
#include "CLOVise/PLM/Helper/Util/Logger.h"
#include "CLOVise/PLM/Helper/UIHelper/CVWidgetGenerator.h"

using namespace std;

namespace CLOVise
{
	PLMSampleResult* PLMSampleResult::_instance = NULL;


	PLMSampleResult* PLMSampleResult::GetInstance()
	{
		if (_instance == NULL)
			_instance = new PLMSampleResult();
		return _instance;
	}

	void PLMSampleResult::Destroy()
	{
		if (_instance)
		{
			delete _instance;
			_instance = NULL;
		}
	}

	PLMSampleResult::PLMSampleResult(QWidget* parent) : MVDialog(parent)
	{
		Logger::Info("PLMSampleResult -> Constructor() -> Start");
		setupUi(this);
		try
		{
			QString windowTitle = PLM_NAME + " PLM Sample Search Results ";
			this->setWindowTitle(windowTitle);

#ifdef __APPLE__
			this->setWindowFlags(Qt::Dialog | Qt::CustomizeWindowHint | Qt::WindowMinMaxButtonsHint | Qt::WindowCloseButtonHint);
#else
			this->setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint | Qt::CustomizeWindowHint);
			m_pTitleBar = new MVTitleBar(windowTitle, this);
			layout()->setMenuBar(m_pTitleBar);
#endif // !__APPLE__

			QSpacerItem *horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Fixed);
			QSpacerItem *horizontalSpacer_3 = new QSpacerItem(20, 20, QSizePolicy::Fixed, QSizePolicy::Fixed);

			ui_toolLayout->insertSpacerItem(2, horizontalSpacer_2);
			ui_toolLayout->insertSpacerItem(5, horizontalSpacer_3);
			ui_toolLayout->insertSpacerItem(8, horizontalSpacer_3);

			QLabel *resultCountLabel = CVWidgetGenerator::CreateLabel("Result Count", "", HEADER_STYLE, true);
			ui_toolLayout->insertWidget(0, resultCountLabel);

			m_noOfResultLabel = CVWidgetGenerator::CreateLabel("", "", HEADER_STYLE, true);
			ui_toolLayout->insertWidget(1, m_noOfResultLabel);

			QLabel *viewLabel = CVWidgetGenerator::CreateLabel("Views", "", HEADER_STYLE, false);
			ui_toolLayout->insertWidget(3, viewLabel);

			m_viewComboBox = CVWidgetGenerator::CreateComboBox("", VIEW_COMBOBOX_STYLE, false);
			ui_toolLayout->insertWidget(4, m_viewComboBox);

			QLabel *perPageLabel = CVWidgetGenerator::CreateLabel("Per Page", "", HEADER_STYLE, false);
			ui_toolLayout->insertWidget(6, perPageLabel);


			m_perPageResultComboBox = CVWidgetGenerator::CreateComboBox("", COMBOBOX_STYLE, false);
			ui_toolLayout->insertWidget(7, m_perPageResultComboBox);

			m_tabViewButton = CVWidgetGenerator::CreateToolButton(VIEW_LIST_HOVER_ICON_PATH, TABULAR_VIEW, TOOLTIP_STYLE, false);
			ui_toolLayout->insertWidget(8, m_tabViewButton);

			m_iconViewButton = CVWidgetGenerator::CreateToolButton(VIEW_THUMB_HOVER_ICON_PATH, ICON_VIEW, TOOLTIP_STYLE, false);
			ui_toolLayout->insertWidget(9, m_iconViewButton);


			QSpacerItem *horizontalSpacer_4 = new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Fixed);
			QSpacerItem *horizontalSpacer_5 = new QSpacerItem(10, 20, QSizePolicy::Expanding, QSizePolicy::Fixed);

			m_backButton = CVWidgetGenerator::CreatePushButton("Back", BACK_HOVER_PATH, "Back", BUTTON_STYLE, 30, true);
			m_backButton->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
			ui_buttonLayout->insertWidget(1, m_backButton);

			ui_buttonLayout->insertSpacerItem(2, horizontalSpacer_5);

			ui_buttonLayout->insertSpacerItem(4, horizontalSpacer_5);

			m_downloadButton = CVWidgetGenerator::CreatePushButton("Download", DOWNLOAD_HOVER_ICON_PATH, "Download", PUSH_BUTTON_STYLE, 30, true);
			m_downloadButton->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
			ui_buttonLayout->insertWidget(5, m_downloadButton);

			previousButton->setToolTip(PREVIOUS);
			previousButton->setStyleSheet(PREVIOUS_BUTTON_STYLE);
			nextButton->setToolTip(NEXT);
			nextButton->setStyleSheet(NEXT_BUTTON_STYLE);

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
			setDataFromResponse(SampleConfig::GetInstance()->GetSearchCriteriaJSON());
			if (m_resultsCount > m_maxResultsCount)
				throw "Maximum results limit exceeded. Please refine your search.";
			CVWidgetGenerator::PopulateValuesOnResultsUI(nextButton, m_noOfResultLabel, totalPageLabel, m_perPageResultComboBox, Configuration::GetInstance()->GetResultsPerPage(), m_resultsCount);
			resultTable->setEnabled(false);
			m_tabViewButton->setEnabled(false);
			CVWidgetGenerator::DrawViewAndResultsWidget(m_viewComboBox, resultTable, iconTable, false, m_sampleResults, SampleConfig::GetInstance()->GetSampleViewJSON(), IMAGE_DISPLAY_NAME, m_typename, true, SampleConfig::GetInstance()->GetSelectedViewIdx(), SampleConfig::GetInstance()->GetAttScopes(), true, false, SampleConfig::GetInstance()->GetSampleViewJSON());
			CVWidgetGenerator::UpdateTableAndIconRows(resultTable, currPageLabel, m_perPageResultComboBox, m_resultsCount, false);
			//CVWidgetGenerator::UpdateTableAndIconRows(iconTable, currPageLabel, m_perPageResultComboBox, m_resultsCount);
			setHeaderToolTip();
			resultTable->setEnabled(true);
			connectSignalSlots(true);
			addConnectorForRadioButton();

			RESTAPI::SetProgressBarData(0, "", false);
		}
		catch (exception& e)
		{
			Destroy();
			throw e;
		}
		catch (const char* msg)
		{
			Destroy();
			throw msg;
		}
		Logger::Info("PLMSampleResult -> Constructor() -> End");
	}

	PLMSampleResult::~PLMSampleResult()
	{
		Destroy();
	}

	/*
	* Description - setDataFromResponse() method used to cache the hierarchy data.
	* Parameter -  json.
	* Exception -
	* Return -
	*/
	void PLMSampleResult::setDataFromResponse(json _param)
	{
		Logger::Info("PLMSampleResult -> setDataFromResponse() -> Start");
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
			m_sampleResults = json::parse(resultJsonString);
			string resultsCount = Helper::GetJSONValue<string>(m_sampleResults, "resultFound", true);
			if (FormatHelper::HasContent(resultsCount))
			{
				m_resultsCount = stoi(resultsCount);
			}
			else
			{
				m_resultsCount = 0;
			}
			string maxResultsLimit = Helper::GetJSONValue<string>(m_sampleResults, "maxResultsLimit", true);
			if (FormatHelper::HasContent(maxResultsLimit))
			{
				m_maxResultsCount = stoi(maxResultsLimit);
			}
			else
			{
				m_maxResultsCount = 50;
			}
			m_typename = Helper::GetJSONValue<string>(m_sampleResults, TYPENAME_KEY, true);
		}
		catch (exception& e)
		{
			Logger::Error("PLMSampleResult -> SetDataFromResponse Exception :: " + string(e.what()));
			throw e;
		}
		catch (const char* msg)
		{
			Logger::Error("PLMSampleResult -> SetDataFromResponse Exception :: " + string(msg));
			throw msg;
		}
		Logger::Info("PLMSampleResult -> setDataFromResponse() -> End");
	}

	/*
	* Description - addConnectorForRadioButton() method used to create a connection method for radiobutton.
	* Parameter -
	* Exception -
	* Return -
	*/
	void PLMSampleResult::addConnectorForRadioButton()
	{
		Logger::Info("PLMSampleResult -> addConnectorForRadioButton() -> Start");
		QRadioButton* tempRadioButton;
		for (int row = 0; row < resultTable->rowCount(); row++) 
		{
			QWidget* qWidget = resultTable->cellWidget(row, 0); //to select multiple rows
			tempRadioButton = qobject_cast <QRadioButton*>(qWidget);
			QObject::connect(tempRadioButton, SIGNAL(clicked()), this, SLOT(onTableRadioButtonSelected()));

		}
		Logger::Info("PLMSampleResult -> addConnectorForRadioButton() -> End");
	}

	/*
	* Description - onTableRadioButtonSelected() method is a slot for table radio box select.
	* Parameter -
	* Exception - exception, Char *
	* Return -
	*/
	void PLMSampleResult::onTableRadioButtonSelected()
	{
		Logger::Info("PLMSampleResult -> onTableRadioButtonSelected() -> Start");
		TableRadioButtonSelected();
		Logger::Info("PLMSampleResult -> onTableRadioButtonSelected() -> End");
	}

	void PLMSampleResult::TableRadioButtonSelected()
	{
		Logger::Info("PLMSampleResult -> TableRadioButtonSelected() -> Start");
		try
		{
			m_rowsSelected.clear();
			m_iconsSelected.clear();
			m_rowsSelected = CVWidgetGenerator::TableRowSelected(resultTable, iconTable, true, false);
			m_totalSelected = CVWidgetGenerator::SetDownloadCount(m_iconsSelected, m_rowsSelected, m_downloadButton);
		}
		catch (exception& e)
		{
			Logger::Error("PLMSampleResult-> TableRadioButtonSelected Exception - " + string(e.what()));
			UTILITY_API->DisplayMessageBox(e.what());

		}
		catch (const char* msg)
		{
			Logger::Error("PLMSampleResult-> TableRadioButtonSelected Exception - " + string(msg));
			wstring wstr(msg, msg + strlen(msg));
			UTILITY_API->DisplayMessageBoxW(wstr);
		}
		Logger::Info("PLMSampleResult -> TableRadioButtonSelected() -> End");
	}
	
	/*
	* Description - setHeaderToolTip() method used to set header tool tip.
	* Parameter -
	* Exception -
	* Return -
	*/
	void PLMSampleResult::setHeaderToolTip()
	{
		Logger::Info("PLMSampleResult -> setHeaderToolTip() -> Start");
		for (int i = 0; i < resultTable->columnCount(); i++)
		{
			QString headerValue = resultTable->horizontalHeaderItem(i)->text();
			resultTable->horizontalHeaderItem(i)->setToolTip(headerValue);
		}
		Logger::Info("PLMSampleResult -> setHeaderToolTip() -> End");
	}

	/*
	* Description - onClickedBackButton() method is slot for back button click.
	* Parameter -
	* Exception -
	* Return -
	*/
	void PLMSampleResult::onClickedBackButton()
	{
		Logger::Info("PLMSampleResult -> onClickedBackButton() -> Start");
		backButtonClicked();
		Logger::Info("PLMSampleResult -> onClickedBackButton() -> End");

	}

	void PLMSampleResult::backButtonClicked()
	{
		Logger::Info("PLMSampleResult -> backButtonClicked() -> Start");
		this->close();
		CLOVise::PLMSampleSearch::GetInstance()->setModal(true);
		CLOVise::PLMSampleSearch::GetInstance()->show();
		//Destroy();
		Logger::Info("PLMSampleResult -> backButtonClicked() -> End");
	}

	/*
	* Description - onClickedNextButton() method is a slot for next button click.
	* Parameter -
	* Exception -
	* Return -
	*/
	void PLMSampleResult::onClickedNextButton()
	{
		Logger::Info("PLMSampleResult -> onClickedNextButton() -> Start");
		ClickedNextButton();
		Logger::Info("PLMSampleResult -> onClickedNextButton() -> End");
	}

	void PLMSampleResult::ClickedNextButton()
	{
		Logger::Info("PLMSampleResult -> ClickedNextButton() -> Start");
		int currPage = currPageLabel->text().toInt() + 1;
		currPageLabel->setText(QString::fromStdString(to_string(currPage)));
		if (currPage == totalPageLabel->text().toInt())
			nextButton->setEnabled(false);
		if (m_isTabularView)
			CVWidgetGenerator::UpdateTableAndIconRows(resultTable, currPageLabel, m_perPageResultComboBox, m_resultsCount, false);
		else
			CVWidgetGenerator::UpdateTableAndIconRows(iconTable, currPageLabel, m_perPageResultComboBox, m_resultsCount);
		previousButton->setEnabled(true);
		Logger::Info("PLMSampleResult -> ClickedNextButton() -> End");
	}

	/*
	* Description - onClickedPreviousButton() method is a slot for previous button click.
	* Parameter -
	* Exception -
	* Return -
	*/
	void PLMSampleResult::onClickedPreviousButton()
	{
		Logger::Info("PLMSampleResult -> onClickedPreviousButton() -> Start");
		ClickedPreviousButton();
		Logger::Info("PLMSampleResult -> onClickedPreviousButton() -> End");
	}

	void PLMSampleResult::ClickedPreviousButton()
	{
		Logger::Info("PLMSampleResult -> ClickedPreviousButton() -> Start");
		int currPage = currPageLabel->text().toInt() - 1;
		currPageLabel->setText(QString::fromStdString(to_string(currPage)));
		if (currPage == 1)
			previousButton->setEnabled(false);
		if (m_isTabularView)
			CVWidgetGenerator::UpdateTableAndIconRows(resultTable, currPageLabel, m_perPageResultComboBox, m_resultsCount, false);
		else
			CVWidgetGenerator::UpdateTableAndIconRows(iconTable, currPageLabel, m_perPageResultComboBox, m_resultsCount);
		nextButton->setEnabled(true);
		Logger::Info("PLMSampleResult -> ClickedPreviousButton() -> End");

	}
	/*
	* Description - onResultPerPageCurrentIndexChanged() method is a slot for result per page click.
	* Parameter -  string.
	* Exception -
	* Return -
	*/
	void PLMSampleResult::onResultPerPageCurrentIndexChanged(const QString&)
	{
		Logger::Info("PLMSampleResult -> onResultPerPageCurrentIndexChanged() -> Start");
		try
		{
			int currPage = 1;
			currPageLabel->setText(QString::fromStdString(to_string(currPage)));
			if (currPage == 1)
				previousButton->setEnabled(false);
			CVWidgetGenerator::PopulateValuesOnResultsUI(nextButton, m_noOfResultLabel, totalPageLabel, m_perPageResultComboBox, m_resultsCount);
			CVWidgetGenerator::UpdateTableAndIconRows(resultTable, currPageLabel, m_perPageResultComboBox, m_resultsCount, false);
			CVWidgetGenerator::UpdateTableAndIconRows(iconTable, currPageLabel, m_perPageResultComboBox, m_resultsCount);
			//Caching selected option to remember irrespective of sessions
			Configuration::GetInstance()->SetSelectedResultsPerPage(m_perPageResultComboBox->currentText().toStdString(), true, false);
		}
		catch (string msg)
		{
			Logger::Error("PLMSampleResult -> onResultPerPageCurrentIndexChanged() Exception - " + msg);
		}
		catch (exception & e)
		{
			Logger::Error("PLMSampleResult -> onResultPerPageCurrentIndexChanged() Exception - " + string(e.what()));
		}
		catch (const char* msg)
		{
			wstring wstr(msg, msg + strlen(msg));
			Logger::Error("PLMSampleResult -> onResultPerPageCurrentIndexChanged() Exception - " + string(msg));
		}
		Logger::Info("PLMSampleResult -> onResultPerPageCurrentIndexChanged() -> End");
	}

	/*
	* Description - onResultViewIndexChanged() method is a slot for  views click.
	* Parameter -  string.
	* Exception - exception, Char *
	* Return -
	*/

	void PLMSampleResult::onResultViewIndexChanged(const QString& _str)
	{
		Logger::Info("PLMSampleResult -> onResultViewIndexChanged() -> Start");
		ResultViewIndexChanged(_str);
		Logger::Info("PLMSampleResult -> onResultViewIndexChanged() -> End");
	}

	/*
	* Description - onResultViewIndexChanged() method is a slot for  views click.
	* Parameter -  string.
	* Exception - exception, Char *
	* Return -
	*/
	void PLMSampleResult::ResultViewIndexChanged(const QString&)
	{
		Logger::Info("PLMSampleResult -> ResultViewIndexChanged() -> Start");
		try
		{
			UTILITY_API->DeleteProgressBar(true);

			this->hide();

			UTILITY_API->CreateProgressBar();
			UTILITY_API->SetProgress("Loading", (qrand() % 101));
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
			CVWidgetGenerator::DrawViewAndResultsWidget(m_viewComboBox, resultTable, iconTable, false, m_sampleResults, SampleConfig::GetInstance()->GetSampleViewJSON(), IMAGE_DISPLAY_NAME, m_typename, false, m_viewComboBox->currentIndex(), SampleConfig::GetInstance()->GetAttScopes(), true, false, SampleConfig::GetInstance()->GetSampleViewJSON());
			onTabViewClicked();
			setHeaderToolTip();
			addConnectorForRadioButton();
			resultTable->setEnabled(true);
			RESTAPI::SetProgressBarData(0, "", false);
			this->show();

		}
		catch (exception& e)
		{
			Logger::Error("PLMSampleResult -> onResultViewIndexChanged Exception :: " + string(e.what()));
			UTILITY_API->DisplayMessageBox(e.what());
			RESTAPI::SetProgressBarData(0, "", false);
			this->close();
		}
		catch (const char* msg)
		{
			Logger::Error("PLMSampleResult -> onResultViewIndexChanged Exception :: " + string(msg));
			wstring wstr(msg, msg + strlen(msg));
			UTILITY_API->DisplayMessageBoxW(wstr);
			RESTAPI::SetProgressBarData(0, "", false);
			this->close();
		}
		Logger::Info("PLMSampleResult -> onResultViewIndexChanged() -> End");
	}

	/*
	* Description - onIconViewClicked() method is a slot for icon view click.
	* Parameter -
	* Exception -
	* Return -
	*/
	void PLMSampleResult::onIconViewClicked()
	{
		Logger::Info("PLMSampleResult -> onIconViewClicked() -> Start");
		updateTableForIconView();
		Logger::Info("PLMSampleResult -> onIconViewClicked() -> End");
	}

	void PLMSampleResult::updateTableForIconView()
	{
		Logger::Info("PLMSampleResult -> updateTableForIconView() -> Start");
		try
		{

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
				CVWidgetGenerator::SortIconResultTable(m_isResultTableSorted, iconTable, resultTable, m_sampleResults, false);
				iconTable->show();
				ui_resultTableLayout->addWidget(iconTable);
				CVWidgetGenerator::UpdateTableAndIconRows(iconTable, currPageLabel, m_perPageResultComboBox, m_resultsCount);
				CVWidgetGenerator::SetDownloadCount(m_iconsSelected, m_rowsSelected, m_downloadButton); iconTable->show();
				addConnectorForRadioButton();
				RESTAPI::SetProgressBarData(0, "", false);
				this->show();
			}
			else
			{
				iconTable->show();
				ui_resultTableLayout->addWidget(iconTable);
				CVWidgetGenerator::UpdateTableAndIconRows(iconTable, currPageLabel, m_perPageResultComboBox, m_resultsCount);
			}
		}
		catch (string msg)
		{
			UTILITY_API->DeleteProgressBar(true);
			Logger::Error("PLMSampleResult -> updateTableForIconView() Exception - " + msg);
		}
		catch (exception & e)
		{
			UTILITY_API->DeleteProgressBar(true);
			Logger::Error("PLMSampleResult -> updateTableForIconView() Exception - " + string(e.what()));
		}
		catch (const char* msg)
		{
			UTILITY_API->DeleteProgressBar(true);
			wstring wstr(msg, msg + strlen(msg));
			Logger::Error("PLMSampleResult -> updateTableForIconView() Exception - " + string(msg));
		}
		Logger::Info("PLMSampleResult -> updateTableForIconView() -> End");
	}

	/*
	* Description - onTabViewClicked() method is a slot for table view click.
	* Parameter -
	* Exception -
	* Return -
	*/
	void PLMSampleResult::onTabViewClicked()
	{
		Logger::Info("PLMSampleResult -> onTabViewClicked() -> Start");
		updateTableForTabView();
		Logger::Info("PLMSampleResult -> onTabViewClicked() -> End");
	}


	void PLMSampleResult::updateTableForTabView()
	{
		Logger::Info("PLMSampleResult -> updateTableForTabView() -> Start");
		try
		{
			m_isTabularView = true;
			m_tabViewButton->setEnabled(false);
			iconTable->close();
			m_iconViewButton->setEnabled(true);
			CVWidgetGenerator::UpdateTableAndIconRows(resultTable, currPageLabel, m_perPageResultComboBox, m_resultsCount, false);
			resultTable->show();
			ui_resultTableLayout->addWidget(resultTable);
		}
		catch (string msg)
		{
			Logger::Error("PLMSampleResult -> updateTableForTabView() Exception - " + msg);
		}
		catch (exception & e)
		{
			Logger::Error("PLMSampleResult -> updateTableForTabView() Exception - " + string(e.what()));
		}
		catch (const char* msg)
		{
			wstring wstr(msg, msg + strlen(msg));
			Logger::Error("PLMSampleResult -> updateTableForTabView() Exception - " + string(msg));
		}
		Logger::Info("PLMSampleResult -> updateTableForTabView() -> End");
	}

	/*
	* Description - onHorizontalHeaderClicked() method is a slot for horizontal header click and sorting the columns.
	* Parameter -  int.
	* Exception -
	* Return -
	*/
	void PLMSampleResult::onHorizontalHeaderClicked(int column)
	{
		Logger::Info("PLMSampleResult -> onHorizontalHeaderClicked() -> Start");
		horizontalHeaderClicked(column);
		Logger::Info("PLMSampleResult -> onHorizontalHeaderClicked() -> End");
	}

	void PLMSampleResult::horizontalHeaderClicked(int column)
	{
		Logger::Info("PLMSampleResult -> horizontalHeaderClicked() -> Start");
		Logger::Debug("Column.." + to_string(column));
		if (column == RADIOBUTTON_COLUMN || column == IMAGE_COLUMN)
			resultTable->setSortingEnabled(false);
		else
		{
			resultTable->setSortingEnabled(true);
			m_isResultTableSorted = true;
			addConnectorForRadioButton();
			CVWidgetGenerator::UpdateTableAndIconRows(resultTable, currPageLabel, m_perPageResultComboBox, m_resultsCount, true);
		}
		Logger::Info("PLMSampleResult -> horizontalHeaderClicked() -> End");
	}

	/*
	* Description - connectSignalSlots() method used to connect and disconnect the signals and slots.
	* Parameter -  bool.
	* Exception -
	* Return -
	*/
	void PLMSampleResult::connectSignalSlots(bool _b)
	{
		Logger::Info("PLMSampleResult -> connectSignalSlots() -> Start");
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
			QObject::disconnect(resultTable->horizontalHeader(), SIGNAL(sectionClicked(int)), this, SLOT(onHorizontalHeaderClicked(int)));
		}
		Logger::Info("PLMSampleResult -> connectSignalSlots() -> End");
	}

	/*
	* Description - DownloadClicked() method used to download the selected items.
	* Parameter -
	* Exception - exception, Char *
	* Return -
	*/
	void PLMSampleResult::onDownloadClicked()
	{
		Logger::Info("PLMSampleResult -> onDownloadClicked() -> Start");
		downloadSampleResult();
		Logger::Info("PLMSampleResult -> onDownloadClicked() -> End");
	}

	void PLMSampleResult::downloadSampleResult()
	{
		Logger::Info("PLMSampleResult -> downloadSampleResult() -> Start");
		try
		{
			if (m_totalSelected.isEmpty())
			{
				throw "Please select Results to download.";
			}
			//RESTAPI::SetProgressBarData(14, "Downloading Sample", true);
			if (Configuration::GetInstance()->GetCloseResultsDialogue())
				this->close();
			if (!FormatHelper::HasContent(UTILITY_API->GetProjectName()))
			{
				throw logic_error(NO_3D_SAMPLE_MSG);
			}
			if (DEFAULT_3DMODEL_NAME.find(UTILITY_API->GetProjectName()) != string::npos)
			{
				throw logic_error(NO_3D_SAMPLE_MSG);
			}
			UTILITY_API->CreateProgressBar();
			UTILITY_API->SetProgress("Downloading Sample", 10);
			UTILITY_API->SetProgress("Downloading Sample", (qrand() % 101));
			QString faildesObjectsString = downloadSampleMetadata(m_sampleResults, m_totalSelected);
			RESTAPI::SetProgressBarData(0, "", false);
			if (FormatHelper::HasContent(faildesObjectsString.toStdString()))
			{
				UTILITY_API->DisplayMessageBox("Download Completed.\n\n Failed: " + faildesObjectsString.toStdString());
				RESTAPI::SetProgressBarData(0, "", false);
			}
			else
			{
				if (Configuration::GetInstance()->GetCloseResultsDialogue())
					this->close();
				CVDisplayMessageBox DownloadDialogObject;
				DownloadDialogObject.DisplyMessage("Download Completed.");
				DownloadDialogObject.setModal(true);
				DownloadDialogObject.exec();
			}
			//PLMSampleSearch::Destroy();
			Logger::Debug("PLMSampleResult -> void onDownloadClicked end");
		}
		catch (exception& e)
		{
			Logger::Error("PLMSampleResult -> DownloadClicked Exception :: " + string(e.what()));
			UTILITY_API->DisplayMessageBox(e.what());
		}
		catch (const char* msg)
		{
			Logger::Error("PLMSampleResult -> DownloadClicked Exception :: " + string(msg));
			wstring wstr(msg, msg + strlen(msg));
			UTILITY_API->DisplayMessageBoxW(wstr);
		}
		Logger::Info("PLMSampleResult -> downloadSampleResult() -> End");
	}

	QString PLMSampleResult::downloadSampleMetadata(json _resultsJson, QStringList _selectedObjectIds)
	{
		json searchResultsJson = Helper::GetJSONParsedValue<string>(_resultsJson, SEARCHRESULTS_KEY, false);
		string sampleName = "";
		string sampleId = "";
		QStringList failedObjects;
		json resultJson = json::object();

		for (int i = 0; i < searchResultsJson.size(); i++)
		{
			resultJson = Helper::GetJSONParsedValue<int>(searchResultsJson, i, false);
			sampleId = Helper::GetJSONValue<string>(resultJson, OBJECT_ID_KEY, true);
			//For now this considered, selected ids will have only one id
			if (_selectedObjectIds.contains(QString::fromStdString(sampleId)))
			{
				try
				{
					sampleName = Helper::GetJSONValue<string>(resultJson, SAMPLE_NAME_KEY, true);
					//need to update when publish is happening.
					//UTILITY_API->DisplayMessageBox("download results json" + to_string(resultJson));
					//SubmitSampleData::GetInstance()->SetActiveSampleMetaData(resultJson);
					Logger::Debug("resultJson - " + to_string(resultJson));
					break;
				}
				catch (exception e)
				{
					Logger::Error(e.what());
					failedObjects.push_back(QString::fromStdString(sampleName) + " - Doesn't have valid Data");
					break;
				}
			}
		}
		return failedObjects.join(", ");
	}
}

