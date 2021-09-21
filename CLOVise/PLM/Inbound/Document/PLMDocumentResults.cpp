/*
* Copyright 2020-2021 CLO-Vise. All rights reserved
*
* @file PLMDocumentResults.cpp
*
* brief Class implementation for cach Document data in CLO from PLM.
* This class has all the variable and methods implementation which are used in storing PLM Document instance data to view and download in CLO.
*
* @author GoVise
*
* @date 24-JUL-2020
*/
#include "PLMDocumentResults.h"

#include <iostream>
#include <string>

#include "CLOVise/PLM/Helper/UIHelper/CVWidgetGenerator.h"
#include "CLOVise/PLM/Inbound/Document/PLMDocumentSearch.h"
#include "CLOVise/PLM/Inbound/Document/DocumentConfig.h"
#include "CLOVise/PLM/Helper/Util/FormatHelper.h"
#include "CLOVise/PLM/Helper/Util/RestAPIUtility.h"
#include "CLOVise/PLM/Helper/UIHelper/UIHelper.h"
#include "CLOVise/PLM/Helper/UIHelper/CVHoverDelegate.h"
#include "CLOVise/PLM/Helper/UIHelper/CVDisplayMessageBox.h"
#include "CLOVise/PLM/Helper/Util/Logger.h"

using namespace std;

namespace CLOVise
{
	PLMDocumentResults* PLMDocumentResults::_instance = NULL;

	PLMDocumentResults* PLMDocumentResults::GetInstance()
	{
		if (_instance == NULL)
			_instance = new PLMDocumentResults();
		return _instance;
	}

	void PLMDocumentResults::Destroy()
	{
		if (_instance)
		{
			delete _instance;
			_instance = NULL;
		}
	}

	PLMDocumentResults::PLMDocumentResults(QWidget* parent)
		: MVDialog(parent)
	{
		Logger::Info("PLMDocumentResults -> Constructor() -> Start");
		setupUi(this);
		try
		{
			QString windowTitle = PLM_NAME + " PLM 3D Model Search Results " ;
			this->setWindowTitle("windowTitle");

#ifdef __APPLE__
			this->setWindowFlags(Qt::Dialog | Qt::CustomizeWindowHint | Qt::WindowMinMaxButtonsHint | Qt::WindowCloseButtonHint);
#else
			this->setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint | Qt::CustomizeWindowHint);
			m_pTitleBar = new MVTitleBar("windowTitle", this);
			layout()->setMenuBar(m_pTitleBar);
#endif // !__APPLE__

			label_3->setStyleSheet(HEADER_STYLE);
			numOfResults->setStyleSheet(HEADER_STYLE);
			label->setStyleSheet(HEADER_STYLE);
			label_4->setStyleSheet(HEADER_STYLE);
			comboBox->setStyleSheet(VIEW_COMBOBOX_STYLE);
			resultPerPageComboBox->setStyleSheet(COMBOBOX_STYLE);

			DeSelectAll->setIcon(QIcon(":/CLOVise/FlexPLM/Images/icon_ok_none.svg"));

			back->setIcon(QIcon(":/CLOVise/FlexPLM/Images/icon_back_over.svg"));

			download->setIcon(QIcon(":/CLOVise/FlexPLM/Images/icon_down_over.svg"));
			back->setStyleSheet(BUTTON_STYLE);
			DeSelectAll->setStyleSheet(BUTTON_STYLE);
			download->setStyleSheet(BUTTON_STYLE);

			previousButton->setToolTip(PREVIOUS);
			previousButton->setStyleSheet(PREVIOUS_BUTTON_STYLE);
			nextButton->setToolTip(NEXT);
			nextButton->setStyleSheet(NEXT_BUTTON_STYLE);

			iconView->setToolTip(ICON_VIEW);

			tabView->setToolTip(TABULAR_VIEW);

			resultTable = new MVTableWidget();
			CVWidgetGenerator::InitializeTableView(resultTable);
			CVHoverDelegate* customHoverDelegate = new CVHoverDelegate(resultTable);
			resultTable->setItemDelegate(customHoverDelegate);

			QPalette pal;
			pal.setColor(QPalette::Highlight, highlightColor);
			pal.setColor(QPalette::HighlightedText, highlightedTextColor);
			resultTable->setPalette(pal);

			iconTable = new QListWidget();
			myLayout->addWidget(resultTable);
			//setDataFromResponse(PLMDocumentData::GetInstance()->GetSearchCriteriaJSON());
			if (m_resultsCount > m_maxResultsCount)
				throw "Maximum results limit exceeded. Please refine your search.";
			CVWidgetGenerator::PopulateValuesOnResultsUI(nextButton, numOfResults, totalPageLabel, resultPerPageComboBox, Configuration::GetInstance()->GetResultsPerPage(), m_resultsCount);
			resultTable->setEnabled(false);
			tabView->setEnabled(false);
			CVWidgetGenerator::DrawViewAndResultsWidget(comboBox, resultTable, iconTable, false, m_documentResults, PLMDocumentData::GetInstance()->GetDocumentViewJSON(), "Document Image", m_typename, true, PLMDocumentData::GetInstance()->GetSelectedViewIdx(), PLMDocumentData::GetInstance()->GetAttScopes(), true, PLMDocumentData::GetInstance()->GetProductIdKeyExists(), PLMDocumentData::GetInstance()->GetDocumentViewJSON());
			CVWidgetGenerator::UpdateTableAndIconRows(resultTable, currPageLabel, resultPerPageComboBox, m_resultsCount, false);
			//CVWidgetGenerator::UpdateTableAndIconRows(iconTable, currPageLabel, resultPerPageComboBox, m_resultsCount);
			setHeaderToolTip();
			DeSelectAll->hide();
			resultTable->setEnabled(true);
			connectSignalSlots(true);
			AddConnectorForRadioButton();

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
		Logger::Info("PLMDocumentResults -> Constructor() -> End");
	}

	PLMDocumentResults::~PLMDocumentResults()
	{

	}

	/*
	* Description - SetDataFromResponse() method used to cache the configured json from rest.
	* Parameter -  json.
	* Exception - exception, Char *
	* Return -
	*/
	void PLMDocumentResults::setDataFromResponse(json _param)
	{
		Logger::Info("SetDataFromResponse -> Constructor() -> Start");
		try
		{
			string resultJsonString = RESTAPI::RESTMethodSubmit(RESTAPI::SEARCH_RESULTS_API, _param);
			if (!FormatHelper::HasContent(resultJsonString))
			{
				throw "Unable to fetch results. Please try again or Contact your System Administrator.";
			}

			if (FormatHelper::HasError(resultJsonString))
			{
				Logger::Error("SetDataFromResponse -> Constructor() -> restErrorMsg - " + string(resultJsonString));
				throw runtime_error(resultJsonString);
			}
			string error = RESTAPI::CheckForErrorMsg(resultJsonString);
			if (FormatHelper::HasContent(error))
			{
				throw std::logic_error(error);
			}
			m_documentResults = json::parse(resultJsonString);
			string resultsCount = Helper::GetJSONValue<string>(m_documentResults, "resultFound", true);
			if (FormatHelper::HasContent(resultsCount))
			{
				m_resultsCount = stoi(resultsCount);
			}
			else
			{
				m_resultsCount = 0;
			}

			string maxResultsLimit = Helper::GetJSONValue<string>(m_documentResults, "maxResultsLimit", true);
			if (FormatHelper::HasContent(maxResultsLimit))
			{
				m_maxResultsCount = stoi(maxResultsLimit);
			}
			else
			{
				m_maxResultsCount = 50;
			}
			m_typename = Helper::GetJSONValue<string>(m_documentResults, TYPENAME_KEY, true);

		}
		catch (exception& e)
		{
			Logger::Error("PLMDocumentData -> SetDataFromResponse Exception :: " + string(e.what()));
			throw e;
		}
		catch (const char* msg)
		{
			Logger::Error("PLMDocumentData -> SetDataFromResponse Exception :: " + string(msg));
			throw msg;
		}
	}

	/*
	* Description - AddConnectorForRadioButton() method used connect the ratio button signal and slots.
	* Parameter -
	* Exception -
	* Return -
	*/
	void PLMDocumentResults::AddConnectorForRadioButton()
	{
		int currentPageValue = currPageLabel->text().toInt();
		int m_resultPerPage = resultPerPageComboBox->currentText().toInt();
		QRadioButton* tempRadioButton;
		for (int row = 0; row < resultTable->rowCount(); row++) {
			QWidget* qWidget = resultTable->cellWidget(row, 0); //to select multiple rows
			tempRadioButton = qobject_cast<QRadioButton*>(qWidget);
			QObject::connect(tempRadioButton, SIGNAL(clicked()), this, SLOT(onTableRadioButtonSelected()));

		}

	}

	/*
	* Description - TableRadioButtonSelected() method is a slot for tabular view radio button click.
	* Parameter -
	* Exception - exception, Char *
	* Return -
	*/
	void PLMDocumentResults::onTableRadioButtonSelected()
	{
		try
		{
			m_rowsSelected.clear();
			m_iconsSelected.clear();
			m_rowsSelected = CVWidgetGenerator::TableRowSelected(resultTable, iconTable, true, false);
			m_totalSelected = CVWidgetGenerator::SetDownloadCount(m_iconsSelected, m_rowsSelected, download);
		}
		catch (exception& e)
		{
			Logger::Error("PLMDocumentResults -> TableRadioButton Exception - " + string(e.what()));
			UTILITY_API->DisplayMessageBox(e.what());
		}
		catch (const char* msg)
		{
			Logger::Error("PLMDocumentResults -> TableRadioButton Exception - " + string(msg));
			wstring wstr(msg, msg + strlen(msg));
			UTILITY_API->DisplayMessageBoxW(wstr);
		}
	}

	///*
	//* Description - IconRadioButtonSelected() method is a slot for icon view radio button click.
	//* Parameter -
	//* Exception - exception, Char *
	//* Return -
	//*/
	//void PLMDocumentResults::onIconRadioButtonSelected()
	//{
	//	try
	//	{
	//		m_iconsSelected.clear();
	//		m_rowsSelected.clear();
	//		m_iconsSelected = CVWidgetGenerator::IconRowSelected(iconTable, resultTable, true);
	//		m_totalSelected = CVWidgetGenerator::SetDownloadCount(m_iconsSelected, m_rowsSelected, download);
	//	}
	//	catch (exception& e)
	//	{
	//		Logger::Error("PLMDocumentResults -> IconRadioButton Exception :: " + string(e.what()));
	//		UTILITY_API->DisplayMessageBox(e.what());
	//	}
	//	catch (const char* msg)
	//	{
	//		Logger::Error("PLMDocumentResults -> IconRadioButton Exception :: " + string(msg));
	//		wstring wstr(msg, msg + strlen(msg));
	//		UTILITY_API->DisplayMessageBoxW(wstr);
	//	}
	//}

	/*
	* Description - setHeaderToolTip() method used set a tool tip for the headers.
	* Parameter -
	* Exception -
	* Return -
	*/
	void PLMDocumentResults::setHeaderToolTip()
	{
		for (int i = 0; i < resultTable->columnCount(); i++)
		{
			QString headerValue = resultTable->horizontalHeaderItem(i)->text();
			resultTable->horizontalHeaderItem(i)->setToolTip(headerValue);
		}
	}

	/*
	* Description - onClickedDeselectAllButton() method is a slot for deselect all button click and deselect all the selected.
	* Parameter -
	* Exception -
	* Return -
	*/
	void PLMDocumentResults::onClickedDeselectAllButton()
	{
		CVWidgetGenerator::DeSelectAllClicked(resultTable, iconTable, download);
		m_rowsSelected.clear();
		m_iconsSelected.clear();
		m_totalSelected.clear();
	}

	/*
	* Description - onClickedBackButton() method is a slot for back button click and navigate to design suite.
	* Parameter -
	* Exception -
	* Return -
	*/
	void PLMDocumentResults::onClickedBackButton()
	{
		this->close();
		CLOVise::PLMDocumentSearch::GetInstance()->setModal(true);
		CLOVise::PLMDocumentSearch::GetInstance()->show();
		Destroy();
	}

	/*
	* Description - onClickedNextButton() method is a slot for next button click and navigate to next slide.
	* Parameter -
	* Exception -
	* Return -
	*/
	void PLMDocumentResults::onClickedNextButton()
	{
		int currPage = currPageLabel->text().toInt() + 1;
		currPageLabel->setText(QString::fromStdString(to_string(currPage)));
		if (currPage == totalPageLabel->text().toInt())
			nextButton->setEnabled(false);
		if (m_isTabularView)
			CVWidgetGenerator::UpdateTableAndIconRows(resultTable, currPageLabel, resultPerPageComboBox, m_resultsCount, false);
		else
			CVWidgetGenerator::UpdateTableAndIconRows(iconTable, currPageLabel, resultPerPageComboBox, m_resultsCount);
		previousButton->setEnabled(true);
	}

	/*
	* Description - onClickedPreviousButton() method is a slot for previous button click and navigate to previous slide.
	* Parameter -
	* Exception -
	* Return -
	*/
	void PLMDocumentResults::onClickedPreviousButton()
	{
		int currPage = currPageLabel->text().toInt() - 1;
		currPageLabel->setText(QString::fromStdString(to_string(currPage)));
		if (currPage == 1)
			previousButton->setEnabled(false);
		if (m_isTabularView)
			CVWidgetGenerator::UpdateTableAndIconRows(resultTable, currPageLabel, resultPerPageComboBox, m_resultsCount, false);
		else
			CVWidgetGenerator::UpdateTableAndIconRows(iconTable, currPageLabel, resultPerPageComboBox, m_resultsCount);
		nextButton->setEnabled(true);
	}

	/*
	* Description - onResultPerPageCurrentIndexChanged() method is a slot for result per page option and cache the latest selected option.
	* Parameter -  QString.
	* Exception -
	* Return -
	*/
	void PLMDocumentResults::onResultPerPageCurrentIndexChanged(const QString&)
	{
		int currPage = 1;
		currPageLabel->setText(QString::fromStdString(to_string(currPage)));
		if (currPage == 1)
			previousButton->setEnabled(false);
		CVWidgetGenerator::PopulateValuesOnResultsUI(nextButton, numOfResults, totalPageLabel, resultPerPageComboBox, m_resultsCount);
		CVWidgetGenerator::UpdateTableAndIconRows(resultTable, currPageLabel, resultPerPageComboBox, m_resultsCount, false);
		CVWidgetGenerator::UpdateTableAndIconRows(iconTable, currPageLabel, resultPerPageComboBox, m_resultsCount);
		//Caching selected option to remember irrespective of sessions
		Configuration::GetInstance()->SetSelectedResultsPerPage(resultPerPageComboBox->currentText().toStdString(), true, false);
	}

	/*
	* Description - onResultViewIndexChanged() method is a slot for views changes  and re arranging the column of the table.
	* Parameter -  QString.
	* Exception - exception, Char *
	* Return -
	*/
	void PLMDocumentResults::onResultViewIndexChanged(const QString&)
	{
		try
		{
			UTILITY_API->DeleteProgressBar(true);
			this->hide();
			UTILITY_API->CreateProgressBar();
			UTILITY_API->SetProgress("Loading", (qrand() % 101));
			Logger::Info("PLMDocumentResults -> onResultViewIndexChanged() -> Start");
			m_totalSelected.clear();
			m_rowsSelected.clear();
			m_iconsSelected.clear();
			download->setText(DOWNLOAD_TEXT);
			resultTable->setEnabled(false);
			resultTable->clear();
			resultTable->reset();
			resultTable->setColumnCount(0);
			iconTable->clear();
			CVWidgetGenerator::DrawViewAndResultsWidget(comboBox, resultTable, iconTable, false, m_documentResults, PLMDocumentData::GetInstance()->GetDocumentViewJSON(), "Document Image", m_typename, false, comboBox->currentIndex(), PLMDocumentData::GetInstance()->GetAttScopes(), true, PLMDocumentData::GetInstance()->GetProductIdKeyExists(), PLMDocumentData::GetInstance()->GetDocumentViewJSON());
			onTabViewClicked();
			setHeaderToolTip();
			AddConnectorForRadioButton();
			resultTable->setEnabled(true);
			RESTAPI::SetProgressBarData(0, "", false);
			this->show();
		}
		catch (exception& e)
		{
			Logger::Error("PLMDocumentResults -> onResultViewIndexChanged Exception :: " + string(e.what()));
			UTILITY_API->DisplayMessageBox(e.what());
			RESTAPI::SetProgressBarData(0, "", false);
			this->close();
		}
		catch (const char* msg)
		{
			Logger::Error("PLMDocumentResults -> onResultViewIndexChanged Exception :: " + string(msg));
			wstring wstr(msg, msg + strlen(msg));
			UTILITY_API->DisplayMessageBoxW(wstr);
			RESTAPI::SetProgressBarData(0, "", false);
			this->close();
		}
		Logger::Info("PLMDocumentResults -> onResultViewIndexChanged() -> End");
	}

	/*
	* Description - iconViewClicked() method is a slot for icon click and updated the view in the form of icon.
	* Parameter -
	* Exception -
	* Return -
	*/
	void PLMDocumentResults::onIconViewClicked()
	{
		m_isTabularView = false;
		iconView->setEnabled(false);
		tabView->setEnabled(true);
		resultTable->close();
		try
		{
			if (m_isResultTableSorted)
			{
				this->hide();
				RESTAPI::SetProgressBarData(0, "", false);
				UTILITY_API->CreateProgressBar();
				UTILITY_API->SetProgress("Loading", (qrand() % 101));
				CVWidgetGenerator::SortIconResultTable(m_isResultTableSorted, iconTable, resultTable, m_documentResults, true);
				iconTable->show();
				myLayout->addWidget(iconTable);
				CVWidgetGenerator::UpdateTableAndIconRows(iconTable, currPageLabel, resultPerPageComboBox, m_resultsCount);
				CVWidgetGenerator::SetDownloadCount(m_iconsSelected, m_rowsSelected, download); iconTable->show();
				AddConnectorForRadioButton();
				RESTAPI::SetProgressBarData(0, "", false);
				this->show();
			}
			else
			{
				iconTable->show();
				myLayout->addWidget(iconTable);
				CVWidgetGenerator::UpdateTableAndIconRows(iconTable, currPageLabel, resultPerPageComboBox, m_resultsCount);
			}
		}
		catch (string msg)
		{
			UTILITY_API->DeleteProgressBar(true);
			Logger::Error("PLMDocumentResults::onIconViewClicked() Exception - " + msg);
		}
		catch (exception & e)
		{
			UTILITY_API->DeleteProgressBar(true);
			Logger::Error("PLMDocumentResults::onIconViewClicked() Exception - " + string(e.what()));
		}
		catch (const char* msg)
		{
			UTILITY_API->DeleteProgressBar(true);
			wstring wstr(msg, msg + strlen(msg));
			Logger::Error("PLMDocumentResults::onIconViewClicked() Exception - " + string(msg));
		}
	}

	/*
	* Description - tabViewClicked() method is slot for the table view click and updated the view in the form of tabular.
	* Parameter -
	* Exception -
	* Return -
	*/
	void PLMDocumentResults::onTabViewClicked()
	{
		m_isTabularView = true;
		tabView->setEnabled(false);
		iconTable->close();
		iconView->setEnabled(true);
		CVWidgetGenerator::UpdateTableAndIconRows(resultTable, currPageLabel, resultPerPageComboBox, m_resultsCount, false);
		resultTable->show();
		myLayout->addWidget(resultTable);
	}

	/*
	* Description - onHorizontalHeaderClicked() method is a slot for horizontal header click and sorting the columns.
	* Parameter -  int.
	* Exception -
	* Return -
	*/
	void PLMDocumentResults::onHorizontalHeaderClicked(int _column)
	{
		Logger::Info("PLMDocumentResults -> onHorizontalHeaderClicked() -> Start");
		Logger::Debug("Column.." + to_string(_column));
		if (_column == CHECKBOX_COLUMN || _column == IMAGE_COLUMN)
			resultTable->setSortingEnabled(false);
		else
		{
			resultTable->setSortingEnabled(true);
			m_isResultTableSorted = true;
			CVWidgetGenerator::UpdateTableAndIconRows(resultTable, currPageLabel, resultPerPageComboBox, m_resultsCount, true);
		}
		Logger::Info("PLMDocumentResults -> onHorizontalHeaderClicked() -> End");
	}

	/*
	* Description - connectSignalSlots() method used connect and disconnect the slot.
	* Parameter -  bool.
	* Exception -
	* Return -
	*/
	void PLMDocumentResults::connectSignalSlots(bool _b)
	{
		if (_b)
		{
			QObject::connect(back, SIGNAL(clicked()), this, SLOT(onClickedBackButton()));
			QObject::connect(nextButton, SIGNAL(clicked()), this, SLOT(onClickedNextButton()));
			QObject::connect(previousButton, SIGNAL(clicked()), this, SLOT(onClickedPreviousButton()));
			QObject::connect(resultPerPageComboBox, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(onResultPerPageCurrentIndexChanged(const QString&)));
			QObject::connect(comboBox, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(onResultViewIndexChanged(const QString&)));
			QObject::connect(iconView, SIGNAL(clicked()), this, SLOT(onIconViewClicked()));
			QObject::connect(tabView, SIGNAL(clicked()), this, SLOT(onTabViewClicked()));
			QObject::connect(download, SIGNAL(clicked()), this, SLOT(onDownloadClicked()));
			QObject::connect(resultTable->horizontalHeader(), SIGNAL(sectionClicked(int)), this, SLOT(onHorizontalHeaderClicked(int)));
		}
		else
		{
			QObject::disconnect(back, SIGNAL(clicked()), this, SLOT(onClickedBackButton()));
			QObject::disconnect(nextButton, SIGNAL(clicked()), this, SLOT(onClickedNextButton()));
			QObject::disconnect(previousButton, SIGNAL(clicked()), this, SLOT(onClickedPreviousButton()));
			QObject::disconnect(resultPerPageComboBox, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(onResultPerPageCurrentIndexChanged(const QString&)));
			QObject::disconnect(comboBox, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(onResultViewIndexChanged(const QString&)));
			QObject::disconnect(iconView, SIGNAL(clicked()), this, SLOT(onIconViewClicked()));
			QObject::disconnect(tabView, SIGNAL(clicked()), this, SLOT(onTabViewClicked()));
			QObject::disconnect(resultTable->horizontalHeader(), SIGNAL(sectionClicked(int)), this, SLOT(onHorizontalHeaderClicked(int)));
		}
	}

	/*
	* Description - DownloadClicked() method is slot for download button.
	* Parameter -
	* Exception - exception, Char *
	* Return -
	*/
	void PLMDocumentResults::onDownloadClicked()
	{
		try
		{
			if (m_totalSelected.isEmpty())
			{
				throw "Please select Results to download.";
			}
			if (Configuration::GetInstance()->GetCloseResultsDialogue())
				this->close();

			RESTAPI::SetProgressBarData(16, "Downloading 3D Model", true);
			QString faildesObjectsString = downloadResults(m_totalSelected, DOCUMENT_MODULE);
			RESTAPI::SetProgressBarData(0, "", false);

			if (FormatHelper::HasContent(faildesObjectsString.toStdString()))
			{
				RESTAPI::SetProgressBarData(0, "", false);
				UTILITY_API->DisplayMessageBox("Download Completed.\n\n Failed: " + faildesObjectsString.toStdString());
			}
			else
			{
				UTILITY_API->CreateProgressBar();
				UTILITY_API->SetProgress("Opening 3D Model", 46);
				UIHelper::OpenDownloadedZPRJ();
				UTILITY_API->SetProgress("Opening 3D Model", 76);
				string metaData = UTILITY_API->GetMetaDataForCurrentGarment();
				json metadataJSON = json::parse(metaData);
				Logger::Debug("metadataJSON 1 - " + to_string(metadataJSON));
				Logger::Debug("m_downloadedResultsJSON 1 - " + to_string(m_downloadedResultsJSON));
				metadataJSON = UIHelper::AddMetaDataIn3DModelFile(metadataJSON, m_downloadedResultsJSON, PLMDocumentData::GetInstance()->Get3DModelFileMetaDataJSONList(), PLMDocumentData::GetInstance()->GetAttScopes());
				Logger::Debug("metadataJSON 2 - " + to_string(metadataJSON));
				UTILITY_API->SetMetaDataForCurrentGarment(to_string(metadataJSON));
				PublishToPLMData::GetInstance()->SetIsProductOverridden(false);
				string currentProjectPath = UTILITY_API->GetProjectFilePath();
				string currentProjectName = UTILITY_API->GetProjectName();
				UIHelper::Rename3DWindowTitle(currentProjectPath, currentProjectName, PublishToPLMData::GetInstance()->GetActiveProductName());
				UTILITY_API->DeleteProgressBar(true);
				CVDisplayMessageBox DownloadDialogObject;
				DownloadDialogObject.DisplyMessage("Download Completed.");
				DownloadDialogObject.setModal(true);
				DownloadDialogObject.exec();
			}
			//PLMDocumentSearch::Destroy();
			this->Destroy();
		}
		catch (exception& e)
		{
			Logger::Error("PLMDocumentResults -> DownloadClicked Exception :: " + string(e.what()));
			UTILITY_API->DisplayMessageBox(e.what());
		}
		catch (const char* msg)
		{
			Logger::Error("PLMDocumentResults -> DownloadClicked Exception :: " + string(msg));
			wstring wstr(msg, msg + strlen(msg));
			UTILITY_API->DisplayMessageBoxW(wstr);
		}
	}

	/*
	* Description - DownloadResults() method used to download all the selected item rest.
	* Parameter -  QStringList, string.
	* Exception - exception, Char *
	* Return -
	*/
	inline QString PLMDocumentResults::downloadResults(QStringList _downloadIdList, string _module)
	{
		json downloadJson = json::object();
		json checkedIds = json::array();
		for (int i = 0; i < _downloadIdList.size(); i++)
		{
			checkedIds.push_back(_downloadIdList[i].toStdString());
		}
		downloadJson[MODULE_KEY] = _module;
		downloadJson[CHECKED_IDS_KEY] = checkedIds;

		string response = RESTAPI::RESTMethodSubmit(RESTAPI::ATTACHMENTS_RESULTS_API, downloadJson);
		if (!FormatHelper::HasContent(response))
		{
			throw "Unable to download, please try again or contact your System Administrator.";
		}

		if (FormatHelper::HasError(response))
		{
			throw runtime_error(response);
		}
		string error = RESTAPI::CheckForErrorMsg(response);
		if (FormatHelper::HasContent(error))
		{
			throw std::logic_error(error);
		}
		Logger::Debug("response response - " + response);
		m_downloadJSON = json::parse(response);
		m_downloadedResultsJSON = Helper::GetJSONParsedValue<string>(m_downloadJSON, ATTACHMENTS_KEY, false);
		m_downloadResultJSON = Helper::GetJSONParsedValue<int>(m_downloadedResultsJSON, 0, false);
		PublishToPLMData::GetInstance()->SetActive3DModelMetaData(m_downloadResultJSON);
		m_metadataJSONList = Helper::GetJSONParsedValue<string>(m_downloadJSON, _3DMODEL_METADATALIST_KEY, false);
		return UIHelper::DownloadResults(m_downloadJSON, _module, m_documentResults);
	}
}
