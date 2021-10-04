/*
* Copyright 2020-2021 CLO-Vise. All rights reserved
*
* @file PLMProductResults.cpp
*
* @brief Class implementation for cach Product data in CLO from PLM.
* This class has all the variable and methods implementation which are used in storing PLM Product instance data to view and download in CLO.
*
* @author GoVise
*
* @date 28-JUL-2020
*/
#include "PLMProductResults.h"

#include <iostream>
#include <string>

#include "CLOVise/PLM/Helper/UIHelper/CVWidgetGenerator.h"
#include "CLOVise/PLM/Inbound/Product/PLMProductSearch.h"
#include "CLOVise/PLM/Inbound/Product/ProductConfig.h"
#include "CLOVise/PLM/Helper/Util/FormatHelper.h"
#include "CLOVise/PLM/Helper/Util/RestAPIUtility.h"
#include "CLOVise/PLM/Helper/UIHelper/UIHelper.h"
#include "CLOVise/PLM/Helper/UIHelper/CVHoverDelegate.h"
#include "CLOVise/PLM/Helper/UIHelper/CVDisplayMessageBox.h"
#include "CLOVise/PLM/Helper/Util/Logger.h"
#include "CLOVise/PLM/Outbound/PublishToPLM/CopyProduct.h"
#include "CLOVise/PLM/Outbound/PublishToPLM/UpdateProduct.h"
using namespace std;

namespace CLOVise
{
	PLMProductResults* PLMProductResults::_instance = NULL;

	PLMProductResults* PLMProductResults::GetInstance()
	{
		if (_instance == NULL)
			_instance = new PLMProductResults();
		return _instance;
	}
	
	PLMProductResults::PLMProductResults(QWidget* parent)
		: MVDialog(parent)
	{
		//UTILITY_API->DisplayMessageBox("PLMProductResults -> Constructor() -> Start calling");
		Logger::Info("PLMProductResults -> Constructor() -> Start");
		setupUi(this);
		try
		{
			QString windowTitle = PLM_NAME + " PLM "+ QString::fromStdString(Configuration::GetInstance()->GetLocalizedStyleClassName()) +" Search Results";
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
			//m_back->setMaximumSize()
			m_backButton->setFocusPolicy(Qt::NoFocus);
			ui_buttonLayout->insertWidget(1, m_backButton);

			ui_buttonLayout->insertSpacerItem(2, horizontalSpacer_5);
			m_deSelectAllButton = CVWidgetGenerator::CreatePushButton("Unselect All", OK_NONE_ICON_PATH, "Unselect All", PUSH_BUTTON_STYLE, 30, false);
			m_deSelectAllButton->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
			m_deSelectAllButton->setFocusPolicy(Qt::NoFocus);
			ui_buttonLayout->insertWidget(3, m_deSelectAllButton);
			ui_buttonLayout->insertSpacerItem(4, horizontalSpacer_5);

			


			if (Configuration::GetInstance()->GetCurrentScreen() == SEARCH_PRODUCT_CLICKED || Configuration::GetInstance()->GetCurrentScreen() == UPDATE_PRODUCT_CLICKED)
			{
				m_downloadButton = CVWidgetGenerator::CreatePushButton("Download", DOWNLOAD_HOVER_ICON_PATH, "Download", PUSH_BUTTON_STYLE, 30, true);
			}
			else
			{
				m_downloadButton = CVWidgetGenerator::CreatePushButton("Copy", SAVE_HOVER_ICON_PATH, "Copy", PUSH_BUTTON_STYLE, 30, true);
			}

			
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
			previousButton->setFocusPolicy(Qt::NoFocus);
			nextButton->setToolTip(NEXT);
			nextButton->setStyleSheet(NEXT_BUTTON_STYLE);
			nextButton->setFocusPolicy(Qt::NoFocus);
			//DeSelectAll->setIcon(QIcon(":/CLOVise/PLM/Images/icon_ok_none.svg"));

			//m_backButton->setIcon(QIcon(":/CLOVise/PLM/Images/icon_back_over.svg"));

			//m_downloadButton->setIcon(QIcon(":/CLOVise/PLM/Images/icon_down_over.svg"));
			/*m_backButton->setStyleSheet(BUTTON_STYLE);
			DeSelectAll->setStyleSheet(BUTTON_STYLE);
			m_downloadButton->setStyleSheet(BUTTON_STYLE);*/

			//iconView->setToolTip(ICON_VIEW);

			//m_tabViewButton->setToolTip(TABULAR_VIEW);

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
			aditionalResultWidget = new QListWidget();
			ui_3DDesignsLayout->addWidget(aditionalResultWidget);
			CVWidgetGenerator::GetInstance()->InitializeIconView(aditionalResultWidget);
			resultTable->setMinimumWidth(750);			
			//setDataFromResponse(ProductConfig::GetInstance()->GetSearchCriteriaJSON());
			/*if (m_resultsCount > m_maxResultsCount)
				throw "Maximum results limit exceeded. Please refine your search.";
			if (!m_productResults.contains(COMP3D_KEY))
				throw "Compatibility key is not found. Please contact System Administrator.";*/
			DrawResultWidget(true);
			connectSignalSlots(true);
			
			viewLabel->hide();
			m_viewComboBox->hide();
			perPageLabel->hide();
			m_perPageResultComboBox->hide();
			m_tabViewButton->hide();
			m_iconViewButton->hide();
			
			//UTILITY_API->DisplayMessageBox("endddd......");
			RESTAPI::SetProgressBarData(0, "", false);
		}
		catch (exception& e)
		{
			throw e;
		}
		catch (const char* msg)
		{
			throw msg;
		}
		catch (string msg)
		{
			throw msg;
		}
		Logger::Info("PLMProductResults -> Constructor() -> End");
	}

	PLMProductResults::~PLMProductResults()
	{

	}

	/*
	* Description - SetDataFromResponse() method used to cache the configured json from rest.
	* Parameter -  json.
	* Exception - exception, Char *
	* Return -
	*/
	/*void PLMProductResults::setDataFromResponse(json _param)
	{
		Logger::Info("PLMProductResults -> setDataFromResponse() -> Start");
		try
		{
			string resultJsonString = RESTAPI::RESTMethodSubmit(RESTAPI::SEARCH_RESULTS_API, _param);
			if (!FormatHelper::HasContent(resultJsonString))
			{
				throw "Unable to fetch results. Please try again or Contact your System Administrator.";
			}

			if (FormatHelper::HasError(resultJsonString))
			{
				Logger::Debug("SetDataFromResponse -> Constructor() -> restErrorMsg - " + string(resultJsonString));
				throw runtime_error(resultJsonString);
			}
			string error = RESTAPI::CheckForErrorMsg(resultJsonString);
			if (FormatHelper::HasContent(error))
			{
				throw std::logic_error(error);
			}
			m_productResults = json::parse(resultJsonString);
			string resultsCount = Helper::GetJSONValue<string>(m_productResults, "resultFound", true);
			if (FormatHelper::HasContent(resultsCount))
			{
				m_resultsCount = stoi(resultsCount);
			}
			else
			{ 
				m_resultsCount = 0;
			}

			string maxResultsLimit = Helper::GetJSONValue<string>(m_productResults, "maxResultsLimit", true);
			if (FormatHelper::HasContent(maxResultsLimit))
			{
				m_maxResultsCount = stoi(maxResultsLimit);
			}
			else
			{
				m_maxResultsCount = 50;
			}
			m_typename = Helper::GetJSONValue<string>(m_productResults, TYPENAME_KEY, true);

		}
		catch (exception& e)
		{
			Logger::Error("PLMProductResults -> SetDataFromResponse Exception :: " + string(e.what()));
			throw e;
		}
		catch (const char* msg)
		{
			Logger::Error("PLMProductResults -> SetDataFromResponse Exception :: " + string(msg));
			throw msg;
		}
		Logger::Info("PLMProductResults -> setDataFromResponse() -> end");
	}*/

	/*
	* Description - AddConnectorForRadioButton() method used connect the ratio button signal and slots.
	* Parameter -
	* Exception -
	* Return -
	*/
	void PLMProductResults::AddConnectorForRadioButton()
	{
		Logger::Info("PLMProductResults -> AddConnectorForRadioButton() -> Start");
		int currentPageValue = currPageLabel->text().toInt();
		int m_resultPerPage = m_perPageResultComboBox->currentText().toInt();
		QRadioButton* tempRadioButton;
		for (int row = 0; row < resultTable->rowCount(); row++) 
		{
			QWidget* qWidget = resultTable->cellWidget(row, 0); //to select multiple rows
			tempRadioButton = qobject_cast<QRadioButton*>(qWidget);
			QObject::connect(tempRadioButton, SIGNAL(clicked()), this, SLOT(onTableRadioButtonSelected()));
		}
		Logger::Info("PLMProductResults -> AddConnectorForRadioButton() -> end");
	}

	/*
	* Description - TableRadioButtonSelected() method is a slot for tabular view radio button click.
	* Parameter -
	* Exception - exception, Char *
	* Return -
	*/
	void PLMProductResults::TableRadioButtonSelected()
	{
		Logger::Info("PLMProductResults -> TableRadioButtonSelected() -> Start");
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
				drawAddtionalResultTable(m_totalSelected);
				m_isHidden = true;
				hidebuttonClicked(false);
				this->show();
			}
			else
			{
				m_rowsSelected = CVWidgetGenerator::TableRowSelected(resultTable, iconTable, true, false);
				m_totalSelected = CVWidgetGenerator::SetDownloadCount(m_iconsSelected, m_rowsSelected, m_downloadButton);
			}
			Logger::Info("PLMProductResults -> TableRadioButtonSelected() -> end");
		}
		catch (string msg)
		{
			Logger::Error("PLMProductResults -> TableRadioButtonSelected Exception :: " + msg);
			UTILITY_API->DisplayMessageBox(msg);
		}
		catch (exception& e)
		{
			Logger::Error("PLMProductResults -> TableRadioButtonSelected Exception :: " + string(e.what()));
			UTILITY_API->DisplayMessageBox(e.what());
		}
		catch (const char* msg)
		{
			Logger::Error("PLMProductResults -> TableRadioButtonSelected Exception :: " + string(msg));
			wstring wstr(msg, msg + strlen(msg));
			UTILITY_API->DisplayMessageBoxW(wstr);
		}
	}

	/*
	* Description - TableRadioButtonSelected() method is a slot for tabular view radio button click.
	* Parameter -
	* Exception - exception, Char *
	* Return -
	*/
	void PLMProductResults::onTableRadioButtonSelected()
	{
		TableRadioButtonSelected();
	}

	///*
	//* Description - IconRadioButtonSelected() method is a slot for icon view radio button click.
	//* Parameter -
	//* Exception - exception, Char *
	//* Return -
	//*/
	//void PLMProductResults::IconRadioButtonSelected()
	//{
	//	Logger::Info("PLMProductResults -> IconRadioButtonSelected() -> Start");
	//	try
	//	{
	//		m_iconsSelected.clear();
	//		m_rowsSelected.clear();
	//		m_iconsSelected = CVWidgetGenerator::IconRowSelected(iconTable, resultTable, true);
	//		m_totalSelected = CVWidgetGenerator::SetDownloadCount(m_iconsSelected, m_rowsSelected, m_downloadButton);
	//		Logger::Info("PLMProductResults -> IconRadioButtonSelected() -> end");
	//	}
	//	catch (exception& e)
	//	{
	//		Logger::Error("PLMProductResults -> IconRadioButtonSelected Exception :: " + string(e.what()));
	//		UTILITY_API->DisplayMessageBox(e.what());
	//	}
	//	catch (const char* msg)
	//	{
	//		Logger::Error("PLMProductResults -> IconRadioButtonSelected Exception :: " + string(msg));
	//		wstring wstr(msg, msg + strlen(msg));
	//		UTILITY_API->DisplayMessageBoxW(wstr);
	//	}
	//}

	///*
	//* Description - IconRadioButtonSelected() method is a slot for icon view radio button click.
	//* Parameter -
	//* Exception - exception, Char *
	//* Return -
	//*/
	//void PLMProductResults::onIconRadioButtonSelected()
	//{
	//	IconRadioButtonSelected();
	//}

	/*
	* Description - setHeaderToolTip() method used set a tool tip for the headers.
	* Parameter -
	* Exception -
	* Return -
	*/
	void PLMProductResults::setHeaderToolTip()
	{
		Logger::Info("PLMProductResults -> setHeaderToolTip() -> Start");
		for (int i = 0; i < resultTable->columnCount(); i++)
		{
			QString headerValue = resultTable->horizontalHeaderItem(i)->text();
			resultTable->horizontalHeaderItem(i)->setToolTip(headerValue);
		}
		Logger::Info("PLMProductResults -> setHeaderToolTip() -> end");
	}

	/*
	* Description - onClickedDeselectAllButton() method is a slot for deselect all button click and deselect all the selected.
	* Parameter -
	* Exception -
	* Return -
	*/
	void PLMProductResults::ClickedDeselectAllButton()
	{
		Logger::Info("PLMProductResults -> ClickedDeselectAllButton() -> Start");
		CVWidgetGenerator::DeSelectAllClicked(resultTable, iconTable, m_downloadButton);
		m_rowsSelected.clear();
		m_iconsSelected.clear();
		m_totalSelected.clear();
		Logger::Info("PLMProductResults -> ClickedDeselectAllButton() -> end");
	}

	/*
	* Description - onClickedDeselectAllButton() method is a slot for deselect all button click and deselect all the selected.
	* Parameter -
	* Exception -
	* Return -
	*/
	void PLMProductResults::onClickedDeselectAllButton()
	{
		ClickedDeselectAllButton();
	}

	/*
	* Description - onClickedBackButton() method is a slot for back button click and navigate to design suite.
	* Parameter -
	* Exception -
	* Return -
	*/
	void PLMProductResults::ClickedBackButton()
	{
		Logger::Info("PLMProductResults -> ClickedBackButton() -> Start");
/*		switch (Configuration::GetInstance()->GetCurrentScreen())
		{

		case COPY_PRODUCT_CLICKED:
		{
			this->close();
			CopyProduct::GetInstance()->setModal(true);
			CopyProduct::GetInstance()->show();
		}
		break;
		case SEARCH_PRODUCT_CLICKED:
		{
			*/
		this->close();
		UTILITY_API->CreateProgressBar();
		RESTAPI::SetProgressBarData(10, "Loading Style Search", true);
		CopyProduct::GetInstance()->SetCopyCreated(false);
		ProductConfig::GetInstance()->GetProductFieldsJSON();
		UTILITY_API->SetProgress("Loading Style Search", (qrand() % 101));
			CLOVise::PLMProductSearch::GetInstance()->setModal(true);
			RESTAPI::SetProgressBarData(0, "", false);
			CLOVise::PLMProductSearch::GetInstance()->show();
		/*}
		break;
		}*/

		
		Logger::Info("PLMProductResults -> ClickedBackButton() -> end");
	}

	/*
	* Description - onClickedBackButton() method is a slot for back button click and navigate to design suite.
	* Parameter -
	* Exception -
	* Return -
	*/
	void PLMProductResults::onClickedBackButton()
	{
		ClickedBackButton();
	}

	/*
	* Description - onClickedNextButton() method is a slot for next button click and navigate to next slide.
	* Parameter -
	* Exception -
	* Return -
	*/
	void PLMProductResults::ClickedNextButton()
	{
		Logger::Info("PLMProductResults -> ClickedNextButton() -> Start");
		int currPage = currPageLabel->text().toInt() + 1;
		currPageLabel->setText(QString::fromStdString(to_string(currPage)));
		if (currPage == totalPageLabel->text().toInt())
			nextButton->setEnabled(false);
		if (m_isTabularView)
			CVWidgetGenerator::UpdateTableAndIconRows(resultTable, currPageLabel, m_perPageResultComboBox, m_resultsCount, false);
		else
			CVWidgetGenerator::UpdateTableAndIconRows(iconTable, currPageLabel, m_perPageResultComboBox, m_resultsCount);
		previousButton->setEnabled(true);
		Logger::Info("PLMProductResults -> ClickedNextButton() -> end");
	}

	/*
	* Description - onClickedNextButton() method is a slot for next button click and navigate to next slide.
	* Parameter -
	* Exception -
	* Return -
	*/
	void PLMProductResults::onClickedNextButton()
	{
		ClickedNextButton();
	}

	/*
	* Description - onClickedPreviousButton() method is a slot for previous button click and navigate to previous slide.
	* Parameter -
	* Exception -
	* Return -
	*/
	void PLMProductResults::ClickedPreviousButton()
	{
		Logger::Info("PLMProductResults -> ClickedPreviousButton() -> Start");
		int currPage = currPageLabel->text().toInt() - 1;
		currPageLabel->setText(QString::fromStdString(to_string(currPage)));
		if (currPage == 1)
			previousButton->setEnabled(false);
		if (m_isTabularView)
			CVWidgetGenerator::UpdateTableAndIconRows(resultTable, currPageLabel, m_perPageResultComboBox, m_resultsCount, false);
		else
			CVWidgetGenerator::UpdateTableAndIconRows(iconTable, currPageLabel, m_perPageResultComboBox, m_resultsCount);
		nextButton->setEnabled(true);
		Logger::Info("PLMProductResults -> ClickedPreviousButton() -> end");
	}

	/*
	* Description - onClickedPreviousButton() method is a slot for previous button click and navigate to previous slide.
	* Parameter -
	* Exception -
	* Return -
	*/
	void PLMProductResults::onClickedPreviousButton()
	{
		ClickedPreviousButton();
	}

	/*
	* Description - onResultPerPageCurrentIndexChanged() method is a slot for result per page option and cache the latest selected option.
	* Parameter -  QString.
	* Exception -
	* Return -
	*/
	void PLMProductResults::ResultPerPageCurrentIndexChanged(const QString&)
	{
		Logger::Info("PLMProductResults -> ResultPerPageCurrentIndexChanged() -> Start");
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
			Logger::Error("PLMProductResults -> ResultPerPageCurrentIndexChanged() Exception - " + msg);
		}
		catch (exception & e)
		{
			Logger::Error("PLMProductResults -> ResultPerPageCurrentIndexChanged() Exception - " + string(e.what()));
		}
		catch (const char* msg)
		{
			wstring wstr(msg, msg + strlen(msg));
			Logger::Error("PLMProductResults -> ResultPerPageCurrentIndexChanged() Exception - " + string(msg));
		}
		Logger::Info("PLMProductResults -> ResultPerPageCurrentIndexChanged() -> end");
	}

	/*
	* Description - onResultPerPageCurrentIndexChanged() method is a slot for result per page option and cache the latest selected option.
	* Parameter -  QString.
	* Exception -
	* Return -
	*/
	void PLMProductResults::onResultPerPageCurrentIndexChanged(const QString& _str)
	{
		ResultPerPageCurrentIndexChanged(_str);
	}

	/*
	* Description - onResultViewIndexChanged() method is a slot for views changes  and re arranging the column of the table.
	* Parameter -  QString.
	* Exception - exception, Char *
	* Return -
	*/
	void PLMProductResults::ResultViewIndexChanged(const QString&)
	{
		Logger::Info("PLMProductResults -> ResultViewIndexChanged() -> start");
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
			iconTable->clear();
			CVWidgetGenerator::DrawViewAndResultsWidget(m_viewComboBox, resultTable, iconTable, false, m_productResults, ProductConfig::GetInstance()->GetProductViewJSON(), IMAGE_DISPLAY_NAME, m_typename, false, m_viewComboBox->currentIndex(), ProductConfig::GetInstance()->GetAttScopes(), true, false, ProductConfig::GetInstance()->GetProductViewJSON());
			onTabViewClicked();
			setHeaderToolTip();
			AddConnectorForRadioButton();
			resultTable->setEnabled(true);
			RESTAPI::SetProgressBarData(0, "", false);
			this->show();
			Logger::Info("PLMProductResults -> ResultViewIndexChanged() -> end");
		}
		catch (string msg)
		{
			Logger::Error("PLMProductResults -> onResultViewIndexChanged Exception :: " + msg);
			UTILITY_API->DisplayMessageBox(msg);
			RESTAPI::SetProgressBarData(0, "", false);
			this->close();
		}
		catch (exception& e)
		{
			Logger::Error("PLMProductResults -> onResultViewIndexChanged Exception :: " + string(e.what()));
			UTILITY_API->DisplayMessageBox(e.what());
			RESTAPI::SetProgressBarData(0, "", false);
			this->close();
		}
		catch (const char* msg)
		{
			Logger::Error("PLMProductResults -> onResultViewIndexChanged Exception :: " + string(msg));
			wstring wstr(msg, msg + strlen(msg));
			UTILITY_API->DisplayMessageBoxW(wstr);
			RESTAPI::SetProgressBarData(0, "", false);
			this->close();
		}
		Logger::Info("PLMProductResults -> onResultViewIndexChanged() -> End");
	}

	/*
	* Description - onResultViewIndexChanged() method is a slot for views changes  and re arranging the column of the table.
	* Parameter -  QString.
	* Exception - exception, Char *
	* Return -
	*/
	void PLMProductResults::onResultViewIndexChanged(const QString& _str)
	{
		ResultViewIndexChanged(_str);
	}

	/*
	* Description - iconViewClicked() method is a slot for icon click and updated the view in the form of icon.
	* Parameter -
	* Exception -
	* Return -
	*/
	void PLMProductResults::IconViewClicked()
	{
		Logger::Info("PLMProductResults -> IconViewClicked() -> Start");
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
				CVWidgetGenerator::SortIconResultTable(m_isResultTableSorted, iconTable, resultTable, m_productResults, true);
				iconTable->show();
				ui_resultTableLayout->addWidget(iconTable);
				CVWidgetGenerator::UpdateTableAndIconRows(iconTable, currPageLabel, m_perPageResultComboBox, m_resultsCount);
				CVWidgetGenerator::SetDownloadCount(m_iconsSelected, m_rowsSelected, m_downloadButton); iconTable->show();
				AddConnectorForRadioButton();
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
			Logger::Error("PLMProductResults -> IconViewClicked()  Exception - " + msg);
		}
		catch (exception & e)
		{
			UTILITY_API->DeleteProgressBar(true);
			Logger::Error("PLMProductResults -> IconViewClicked()  Exception - " + string(e.what()));
		}
		catch (const char* msg)
		{
			UTILITY_API->DeleteProgressBar(true);
			wstring wstr(msg, msg + strlen(msg));
			Logger::Error("PLMProductResults -> IconViewClicked()  Exception - " + string(msg));
		}
		Logger::Info("PLMProductResults -> IconViewClicked() -> end");
	}

	/*
	* Description - iconViewClicked() method is a slot for icon click and updated the view in the form of icon.
	* Parameter -
	* Exception -
	* Return -
	*/
	void PLMProductResults::onIconViewClicked()
	{
		IconViewClicked();
	}

	/*
	* Description - tabViewClicked() method is slot for the table view click and updated the view in the form of tabular.
	* Parameter -
	* Exception -
	* Return -
	*/
	void PLMProductResults::TabViewClicked()
	{
		Logger::Info("PLMProductResults -> TabViewClicked() -> Start");
		m_isTabularView = true;
		m_tabViewButton->setEnabled(false);
		iconTable->close();
		m_iconViewButton->setEnabled(true);
		CVWidgetGenerator::UpdateTableAndIconRows(resultTable, currPageLabel, m_perPageResultComboBox, m_resultsCount, false);
		resultTable->show();
		ui_resultTableLayout->addWidget(resultTable);
		Logger::Info("PLMProductResults -> TabViewClicked() -> end");
	}

	/*
	* Description - tabViewClicked() method is slot for the table view click and updated the view in the form of tabular.
	* Parameter -
	* Exception -
	* Return -
	*/
	void PLMProductResults::onTabViewClicked()
	{
		TabViewClicked();
	}

	void PLMProductResults::onHideButtonClicked(bool _hide)
	{
		hidebuttonClicked(_hide);
	}

	void PLMProductResults::hidebuttonClicked(bool _hide)
	{
		if (!_hide)
		{
			//UTILITY_API->CreateProgressBar();
			//RESTAPI::SetProgressBarData(10, "Loading 3D Styles..", true);
			if (!m_isHidden)
			{
				ui_hideButton->setToolTip("Show");
				ui_hideButton->setFocusPolicy(Qt::NoFocus);
				//UTILITY_API->DisplayMessageBox("caling hide...");
				aditionalResultWidget->hide();
				ui_hideButton->setStyleSheet("#ui_hideButton {\n" "qproperty-icon: none; \n""image: url(:/CLOVise/PLM/Images/icon_close_tree_none.svg);\n""}\n"
					"#ui_hideButton:hover{\n""image: url(:/CLOVise/PLM/Images/icon_close_tree_over.svg);\n""}""QToolTip{ color: #46C8FF; background-color: #33414D; border: 1px #000000; }");
				m_isHidden = true;
			}
			else
			{
				if (m_totalSelected.size() > 0)
				{
					ui_hideButton->setToolTip("Hide");
					//UTILITY_API->DisplayMessageBox("caling show...");					
					aditionalResultWidget->show();
					ui_hideButton->setStyleSheet("#ui_hideButton {\n" "qproperty-icon: none; \n""image: url(:/CLOVise/PLM/Images/icon_open_tree_none.svg);\n""}\n"
						"#ui_hideButton:hover{\n""image: url(:/CLOVise/PLM/Images/icon_open_tree_over.svg);\n""}""QToolTip{ color: #46C8FF; background-color: #33414D; border: 1px #000000; }");
					m_isHidden = false;
				}
				else
				{
					ui_hideButton->setToolTip("Hide");
					//UTILITY_API->DisplayMessageBox("caling show...");					
					aditionalResultWidget->clear();
					ui_hideButton->setStyleSheet("#ui_hideButton {\n" "qproperty-icon: none; \n""image: url(:/CLOVise/PLM/Images/icon_open_tree_none.svg);\n""}\n"
						"#ui_hideButton:hover{\n""image: url(:/CLOVise/PLM/Images/icon_open_tree_over.svg);\n""}""QToolTip{ color: #46C8FF; background-color: #33414D; border: 1px #000000; }");

					QImageReader imageReader(":/CLOVise/PLM/Images/infoIcon.png");
					imageReader.setDecideFormatFromContent(true);
					QImage styleIcon = imageReader.read();
					QPixmap pixmap = QPixmap::fromImage(styleIcon);
					pixmap = QPixmap::fromImage(styleIcon).scaled(QSize(20, 20), Qt::KeepAspectRatio);
					QIcon icon;
					icon.addPixmap(pixmap, QIcon::Normal);
					icon.addPixmap(pixmap, QIcon::Selected);
					icon.addPixmap(pixmap, QIcon::Active);
					string message = "Please select a " + Configuration::GetInstance()->GetLocalizedStyleClassName() + " to view 3D designs";
					QListWidgetItem* item = new QListWidgetItem(icon, QString::fromStdString(message));//+ "\n" + latestVersionAttName)));
					//aditionalResultWidget->setProperty(documnetName.c_str(), QString::fromStdString(documentId));
					aditionalResultWidget->addItem(item);
					aditionalResultWidget->setSelectionMode(QAbstractItemView::NoSelection);
					aditionalResultWidget->setDisabled(true);
					//_resultTable->setSelectionBehavior(QAbstractItemView::SelectRows);
					ui_3DDesignsLayout->addWidget(aditionalResultWidget);
					//UTILITY_API->DisplayMessageBox("please selcet any one of the listed  result.");
					aditionalResultWidget->show();
					m_isHidden = false;
				}
			}
			//RESTAPI::SetProgressBarData(0, "", false);
		}
	}

	/*
	* Description - onHorizontalHeaderClicked() method is a slot for horizontal header click and sorting the columns.
	* Parameter -  int.
	* Exception -
	* Return -
	*/
	void PLMProductResults::HorizontalHeaderClicked(int _column)
	{
		Logger::Info("PLMProductResults -> HorizontalHeaderClicked() -> Start");
		Logger::Debug("Column.." + to_string(_column));
		if (_column == CHECKBOX_COLUMN || _column == IMAGE_COLUMN)
			resultTable->setSortingEnabled(false);
		else
		{
			resultTable->setSortingEnabled(true);
			m_isResultTableSorted = true;
			CVWidgetGenerator::UpdateTableAndIconRows(resultTable, currPageLabel, m_perPageResultComboBox, m_resultsCount, true);
		}
		Logger::Info("PLMProductResults -> HorizontalHeaderClicked() -> End");
	}

	/*
	* Description - onHorizontalHeaderClicked() method is a slot for horizontal header click and sorting the columns.
	* Parameter -  int.
	* Exception -
	* Return -
	*/
	void PLMProductResults::onHorizontalHeaderClicked(int _column)
	{
		HorizontalHeaderClicked(_column);
	}

	/*
	* Description - connectSignalSlots() method used connect and disconnect the slot.
	* Parameter -  bool.
	* Exception -
	* Return -
	*/
	void PLMProductResults::connectSignalSlots(bool _b)
	{
		if (_b)
		{
			QObject::connect(m_backButton, SIGNAL(clicked()), this, SLOT(onClickedBackButton()));
			QObject::connect(nextButton, SIGNAL(clicked()), this, SLOT(onClickedNextButton()));
			QObject::connect(previousButton, SIGNAL(clicked()), this, SLOT(onClickedPreviousButton()));
			QObject::connect(m_perPageResultComboBox, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(onResultPerPageCurrentIndexChanged(const QString&)));
			QObject::connect(m_viewComboBox, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(onResultViewIndexChanged(const QString&)));
			QObject::connect(m_iconViewButton, SIGNAL(clicked()), this, SLOT(onIconViewClicked()));
			QObject::connect(m_tabViewButton, SIGNAL(clicked()), this, SLOT(onTabViewClicked()));
			QObject::connect(ui_hideButton, SIGNAL(clicked(bool)), this, SLOT(onHideButtonClicked(bool)));
			QObject::connect(m_downloadButton, SIGNAL(clicked()), this, SLOT(onDownloadClicked()));
			QObject::connect(m_deSelectAllButton, SIGNAL(clicked()), this, SLOT(onClickedDeselectAllButton()));
			QObject::connect(resultTable->horizontalHeader(), SIGNAL(sectionClicked(int)), this, SLOT(onHorizontalHeaderClicked(int)));
			QObject::connect(aditionalResultWidget, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(onListItemClicked(QListWidgetItem*)));
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
			QObject::disconnect(resultTable->horizontalHeader(), SIGNAL(sectionClicked(int)), this, SLOT(onHorizontalHeaderClicked(int)));
		}
	}

	/*
	* Description - DownloadClicked() method is slot for download button.
	* Parameter -
	* Exception - exception, Char *
	* Return -
	*/
	void PLMProductResults::DownloadClicked()
	{
		Logger::Info("PLMProductResults -> DownloadClicked() -> Start");
		bool rowsSelected = true;
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
			Configuration::GetInstance()->SetProgressBarProgress(0);
			UTILITY_API->DeleteProgressBar(true);
			UTILITY_API->CreateProgressBar();
			RESTAPI::SetProgressBarData(Configuration::GetInstance()->GetProgressBarProgress(), "Downloading 3D Model", true);
			//UTILITY_API->SetProgress("Downloading 3D Model", (qrand() % 101));
			//Configuration::GetInstance()->SetProgressBarProgress(RESTAPI::SetProgressBarProgress(Configuration::GetInstance()->GetProgressBarProgress(), 5, "Downloading 3D Model..."));
			//json downloadJson = UIHelper::DownloadResults(m_totalSelected, PRODUCT_MODULE, m_productResults);
			//QString faildesObjectsString = UIHelper::DownloadResults(m_totalSelected, STYLE_MODULE, m_productResults);
		
			QString faildesObjectsString = downloadProductMetadata(m_productResults, m_totalSelected);
			//RESTAPI::SetProgressBarData(0, "", false);
			if (FormatHelper::HasContent(faildesObjectsString.toStdString()))
			{
				UTILITY_API->DeleteProgressBar(true);
				UTILITY_API->DisplayMessageBox("Download Completed.\n\n Failed: " + faildesObjectsString.toStdString());
				if (Configuration::GetInstance()->GetCurrentScreen() == COPY_PRODUCT_CLICKED && !CopyProduct::GetInstance()->IsCopyCreated())
				{
					this->close();
					CLOVise::PLMProductSearch::GetInstance()->setModal(true);
					CLOVise::PLMProductSearch::GetInstance()->show();
				}
			}
			else
			{
				if (Configuration::GetInstance()->GetCurrentScreen() == SEARCH_PRODUCT_CLICKED || CopyProduct::GetInstance()->IsCopyCreated()|| Configuration::GetInstance()->GetCurrentScreen() == UPDATE_PRODUCT_CLICKED)
				{
					///////////
					
					if (FormatHelper::HasContent(m_selectedIdnameText))
					{
						UTILITY_API->DeleteProgressBar(true);
						UTILITY_API->CreateProgressBar();
						Configuration::GetInstance()->SetProgressBarProgress(0);
						RESTAPI::SetProgressBarData(Configuration::GetInstance()->GetProgressBarProgress(), "Opening 3D Model...", true);
						Configuration::GetInstance()->SetProgressBarProgress(RESTAPI::SetProgressBarProgress(Configuration::GetInstance()->GetProgressBarProgress(), 5, "Opening 3D Model..."));
						//UTILITY_API->SetProgress("Opening 3D Model", 20);
						UIHelper::OpenDownloadedZPRJ();
						Configuration::GetInstance()->SetProgressBarProgress(RESTAPI::SetProgressBarProgress(Configuration::GetInstance()->GetProgressBarProgress(), 10, "Opening 3D Model..."));
						string metaData = UTILITY_API->GetMetaDataForCurrentGarment();
						Logger::RestAPIDebug("metaData frowm downloaded item::" + metaData);
						json metadataJSON = json::parse(metaData);
						//Logger::Debug("metadataJSON 1 - " + to_string(metadataJSON));
						//Logger::Debug("m_downloadedResultsJSON 1 - " + to_string(m_downloadedResultsJSON));
						//metadataJSON = UIHelper::AddMetaDataIn3DModelFile(metadataJSON, m_downloadedResultsJSON, PLMDocumentData::GetInstance()->Get3DModelFileMetaDataJSONList(), PLMDocumentData::GetInstance()->GetAttScopes());
						//Logger::Debug("metadataJSON 2 - " + to_string(metadataJSON));
						//UTILITY_API->SetMetaDataForCurrentGarment(to_string(metadataJSON));
						//PublishToPLMData::GetInstance()->SetIsProductOverridden(true);
						string currentProjectPath = UTILITY_API->GetProjectFilePath();
						string currentProjectName = UTILITY_API->GetProjectName();
						//string productName = Helper::GetJSONValue<string>(metadataJSON, PRODUCT_NAME_KEY, true);
						string productName = Helper::GetJSONValue<string>(metadataJSON, NODE_NAME_KEY, true);
						UIHelper::Rename3DWindowTitle(currentProjectPath, currentProjectName, productName);
						Configuration::GetInstance()->SetProgressBarProgress(RESTAPI::SetProgressBarProgress(Configuration::GetInstance()->GetProgressBarProgress(), 20, "Opening 3D Model..."));
						UTILITY_API->DeleteProgressBar(true);
					}
					CVDisplayMessageBox DownloadDialogObject;
					DownloadDialogObject.DisplyMessage("Download Completed.");
					DownloadDialogObject.setModal(true);
					DownloadDialogObject.exec();
						if (FormatHelper::HasContent(m_selectedIdnameText))
						     PublishToPLMData::GetInstance()->SetIsCreateNewDocument(false);
						else
							PublishToPLMData::GetInstance()->SetIsCreateNewDocument(true);
				}
			}
			if (Configuration::GetInstance()->GetCloseResultsDialogue())
			{
				UTILITY_API->DeleteProgressBar(true);
				this->close();
			}
			else
			{
				UTILITY_API->DeleteProgressBar(true);
				this->show();
			}
			Logger::Info("PLMProductResults -> DownloadClicked() -> end");
		}
		catch (string msg)
		{
			Logger::Error("PLMProductResults -> DownloadClicked Exception :: " + msg);
			UTILITY_API->DisplayMessageBox(msg);
			if (Configuration::GetInstance()->GetCloseResultsDialogue() && rowsSelected)
			{
				this->close();
			}
			else
			{
				this->show();
			}
		}
		catch (exception& e)
		{
			Logger::Error("PLMProductResults -> DownloadClicked Exception :: " + string(e.what()));
			UTILITY_API->DisplayMessageBox(e.what());
			if (Configuration::GetInstance()->GetCloseResultsDialogue() && rowsSelected)
			{
				this->close();
			}
			else
			{
				this->show();
			}

		}
		catch (const char* msg)
		{
			Logger::Error("PLMProductResults -> DownloadClicked Exception :: " + string(msg));
			wstring wstr(msg, msg + strlen(msg));
			UTILITY_API->DisplayMessageBoxW(wstr);
			if (Configuration::GetInstance()->GetCloseResultsDialogue() && rowsSelected)
			{
				this->close();
			}
			else
			{
				this->show();
			}
		}
	}

	/*
	* Description - DownloadClicked() method is slot for download button.
	* Parameter -
	* Exception - exception, Char *
	* Return -
	*/
	void PLMProductResults::onDownloadClicked()
	{
		DownloadClicked();
	}	

	QString PLMProductResults::downloadProductMetadata(json _resultsJson, QStringList _selectedObjectIds)
	{
		Logger::Info("INFO::PLMProductResults: downloadProductMetadata() -> Start");
		//json searchResultsJson = Helper::GetJSONParsedValue<string>(_resultsJson, SEARCHRESULTS_KEY, false);
		string productObjectId = "";
		string productName = "";
		QStringList failedObjects;
		json resultJson = json::object();
		string documentId = "";
		//UTILITY_API->SetProgress("Downloading Style", (qrand() % 101));
		for (int i = 0; i < _resultsJson.size(); i++)
		{
			resultJson = Helper::GetJSONParsedValue<int>(_resultsJson, i, false);
			productObjectId = Helper::GetJSONValue<string>(resultJson, OBJECT_ID_KEY, true);
			//UTILITY_API->DisplayMessageBox("productObjectId::" + productObjectId);
			//For now this considered, selected ids will have only one id
			if (_selectedObjectIds.contains(QString::fromStdString(productObjectId)))
			{			
				try
				{
					PublishToPLMData::GetInstance()->SetUpdateStyleCacheData(resultJson);
					Configuration::GetInstance()->SetIsNewStyleDownloaded(true);
					//UTILITY_API->DisplayMessageBox("Update style data" + to_string(resultJson));
					
					if(Configuration::GetInstance()->GetCurrentScreen() == COPY_PRODUCT_CLICKED && !CopyProduct::GetInstance()->IsCopyCreated())
					{
						Logger::Info("ColorResultTable -> colorResultTableDownload_clicked() -> CreateProduct::GetInstance()1");
						CopyProduct::GetInstance()->AddStyleDetails(resultJson);
						CopyProduct::GetInstance()->setModal(true);
						CopyProduct::GetInstance()->show();
					}
				
					if (Configuration::GetInstance()->GetCurrentScreen() == SEARCH_PRODUCT_CLICKED || CopyProduct::GetInstance()->IsCopyCreated() || Configuration::GetInstance()->GetCurrentScreen() == UPDATE_PRODUCT_CLICKED)
					{
						//UTILITY_API->DisplayMessageBox("currented item name::" + aditionalResultWidget->currentItem()->text().toStdString());
						if (FormatHelper::HasContent(m_selectedIdnameText))
						{
							//aditionalResultWidget->isItemSelected()
							documentId = aditionalResultWidget->property(m_selectedIdnameText.c_str()).toString().toStdString();
						}
						//UTILITY_API->DisplayMessageBox("documentId::" + documentId);
						productName = Helper::GetJSONValue<string>(resultJson, NODE_NAME_KEY, true);
						//UTILITY_API->DisplayMessageBox("productName::" + productName);
						resultJson[DFAULT_ASSET_KEY] = documentId;
						_resultsJson[i] = resultJson;
						//m_downloadResultJSON = m_styleResults[i];
						PublishToPLMData::GetInstance()->SetActive3DModelMetaData(resultJson);
					
						/*productName = Helper::GetJSONValue<string>(resultJson, "node_name", true);
						PublishToPLMData::GetInstance()->SetActiveProductMetaData(resultJson);
						break;*/
					}
				}
				
				catch (string msg)
				{
					Logger::Error("ERROR::PLMProductResults: downloadProductMetadata() -> Exception string :: " + msg);
					failedObjects.push_back(QString::fromStdString(productName) + " - Doesn't have valid Data");
					break;
				}
				catch (const char* msg)
				{
					Logger::Error("ERROR::PLMProductResults: downloadProductMetadata() -> Exception char :: " + string(msg));
					failedObjects.push_back(QString::fromStdString(productName) + " - Doesn't have valid Data");
					break;
				}
				catch (exception e)
				{
					Logger::Error("ERROR::PLMProductResults: downloadProductMetadata() -> Exception e :: " + string(e.what()));
					failedObjects.push_back(QString::fromStdString(productName) + " - Doesn't have valid Data");
					break;
				}
			}
		}
		Logger::Info("INFO::PLMProductResults: downloadProductMetadata() -> End");
		if (FormatHelper::HasContent(documentId))
			return UIHelper::DownloadResults(_selectedObjectIds, STYLE_MODULE, _resultsJson);
		else return failedObjects.join(", ");
	}

	/*
	* Description - drawAddtionalResultTable() method is slot for download button.
	* Parameter -QStringList
	* Exception -
	* Return -
	*/
	void PLMProductResults::drawAddtionalResultTable(QStringList _selectedIdList)
	{
		Logger::Info("comming inside of drawAddtionalResultTable");
		//aditionalResultWidget = new QListWidget();
		//aditionalResultWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
		//CVWidgetGenerator::GetInstance()->InitializeIconView(aditionalResultWidget);
		try
		{
			for (int selctedIdCount = 0; selctedIdCount < m_productResults.size(); selctedIdCount++)
			{
				string styleId = Helper::GetJSONValue<string>(m_productResults[selctedIdCount], OBJECT_ID_KEY, true);

				if (_selectedIdList.contains(QString::fromStdString(styleId)))
				{
					//aditionalResultWidget->show();
					if (m_perveiouselySelectedId == styleId)
						continue;

					m_perveiouselySelectedId = styleId;
					aditionalResultWidget->clear();

					Logger::Debug("_selectedIdList.contains(QString::fromStdString(styleId)" + styleId);
					string attachmentResponse = RESTAPI::CentricRestCallGet(Configuration::GetInstance()->GetPLMServerURL() + RESTAPI::ATTACHMENTS_LATEST_REVISION_RESULTS_API + styleId + "?revision_details=true&limit=100&file_ext=" + ZPRJ, APPLICATION_JSON_TYPE, "");
					//json documentjson = Helper::GetJSONParsedValue<string>(m_productResults[selctedIdCount], "documents", false);
					Logger::RestAPIDebug("_selectedIdList.contains(QString::fromStdString(styleId)::documentjson::" + attachmentResponse);
					if (FormatHelper::HasError(attachmentResponse))
					{
						Helper::GetCentricErrorMessage(attachmentResponse);
						throw runtime_error(attachmentResponse);
					}
					aditionalResultWidget->setEnabled(true);
					json attachmentjson = json::parse(attachmentResponse);
					if (!attachmentjson.size())
					{
						//UTILITY_API->DisplayMessageBox("Selcted item dons not have any attachment.");
						QImageReader imageReader(":/CLOVise/PLM/Images/infoIcon.png");
						imageReader.setDecideFormatFromContent(true);
						QImage styleIcon = imageReader.read();
						QPixmap pixmap = QPixmap::fromImage(styleIcon);

						pixmap = QPixmap::fromImage(styleIcon).scaled(QSize(20, 20), Qt::KeepAspectRatio);
						QIcon icon;
						icon.addPixmap(pixmap, QIcon::Normal);
						icon.addPixmap(pixmap, QIcon::Selected);
						icon.addPixmap(pixmap, QIcon::Active);
						string infoMessage = "No 3D designs available for this " + Configuration::GetInstance()->GetLocalizedStyleClassName();
						QListWidgetItem* item = new QListWidgetItem(icon, QString::fromStdString(infoMessage));//+ "\n" + latestVersionAttName)));
						//aditionalResultWidget->setProperty(documnetName.c_str(), QString::fromStdString(documentId));
						aditionalResultWidget->addItem(item);
						aditionalResultWidget->setDisabled(true);
						ui_3DDesignsLayout->addWidget(aditionalResultWidget);

						continue;
					}
					UTILITY_API->SetProgress("Loading 3D Documents..", (qrand() % 101));

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
						}

						PublishToPLMData::GetInstance()->SetLatestRevision(latestRevisionId);

						QString displayDetails = QString::fromStdString(documentName) + "\n" + (QString::fromStdString(latestVersionAttName) + "\n" + (QString::fromStdString(modifiedAt)));
						QListWidgetItem* item = new QListWidgetItem(displayDetails);
						//item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
						aditionalResultWidget->setProperty(displayDetails.toStdString().c_str(), QString::fromStdString(documentId));
						aditionalResultWidget->addItem(item);
					}
					//aditionalResultWidget->show();
					ui_3DDesignsLayout->addWidget(aditionalResultWidget);
					break;
				}
				//json documentjson = json::parse(attachmentResponse);
				//for (int docuemntCount = 0; docuemntCount < documentjson.size(); docuemntCount++)
				//{
				//	string documentId = Helper::GetJSONValue<int>(documentjson, docuemntCount, true);
				//	//Logger::Logger("documentId" + documentId);
				//	//auto startTime = std::chrono::high_resolution_clock::now();
				//	//string atttachmentResponse = RESTAPI::CentricRestCallGet(Configuration::GetInstance()->GetPLMServerURL() + RESTAPI::ATTACHMENTS_RESULTS_API + documentId, APPLICATION_JSON_TYPE, "");
				//	//
				//	//auto finishTime = std::chrono::high_resolution_clock::now();
				//	//std::chrono::duration<double> totalDuration = finishTime - startTime;
				//	//Logger::perfomance(PERFOMANCE_KEY + "Documents API :: " + to_string(totalDuration.count()));
				//	////UTILITY_API->DisplayMessageBox("atttachmentResponse::"+atttachmentResponse);
				//	//if (!FormatHelper::HasContent(atttachmentResponse))
				//	//{
				//	//	continue;
				//	//	//throw "Unable to download, please try again or contact your System Administrator.";
				//	//}

				//	//if (FormatHelper::HasError(atttachmentResponse))
				//	//{
				//	//	string response;
				//	//	if (QString::fromStdString(atttachmentResponse).contains("message"))
				//	//	{
				//	//		json errorRsponse = json::parse(atttachmentResponse);
				//	//		response = Helper::GetJSONValue<string>(errorRsponse, "message", true);
				//	//	}
				//	//	throw runtime_error(response);
				//	//}
				//	//json materialAttachmentjson;
				//	//if (FormatHelper::HasContent(atttachmentResponse))
				//	//	materialAttachmentjson = json::parse(atttachmentResponse);
				//	string latestVersionAtt = Helper::GetJSONValue<string>(materialAttachmentjson, "latest_revision", true);
				//	string documnetName = Helper::GetJSONValue<string>(materialAttachmentjson, NODE_NAME_KEY, true);

				//	if (!FormatHelper::HasContent(documnetName))
				//		documnetName = "(unnamed)";
				//	auto latestVersionStartTime = std::chrono::high_resolution_clock::now();
				//	string attachmentVersioValue = RESTAPI::CentricRestCallGet(Configuration::GetInstance()->GetPLMServerURL() + RESTAPI::ATTACHMENTS_VERSION_API + latestVersionAtt, APPLICATION_JSON_TYPE, "");
				//	auto latestVersionApiFinishTime = std::chrono::high_resolution_clock::now();
				//	std::chrono::duration<double> latestVersionApiTotalDuration = latestVersionApiFinishTime - latestVersionStartTime;
				//	Logger::perfomance(PERFOMANCE_KEY + "Documents Revisions API :: " + to_string(latestVersionApiTotalDuration.count()));
				//	//UTILITY_API->DisplayMessageBox("attachmentVersioValue::" + attachmentVersioValue);
				//	if (!FormatHelper::HasContent(attachmentVersioValue))
				//	{
				//		throw "Unable to download, please try again or contact your System Administrator.";
				//	}

				//	if (FormatHelper::HasError(attachmentVersioValue))
				//	{
				//		if (QString::fromStdString(attachmentVersioValue).contains("message"))
				//		{
				//			json errorRsponse = json::parse(attachmentVersioValue);
				//			attachmentVersioValue = Helper::GetJSONValue<string>(errorRsponse, "message", true);
				//		}
				//		throw runtime_error(attachmentVersioValue);
				//	}
				//	json materialAttachmentjson1;
				//	if (FormatHelper::HasContent(attachmentVersioValue))
				//		materialAttachmentjson1 = json::parse(attachmentVersioValue);
				//	//string latestVersionAttId = Helper::GetJSONValue<string>(materialAttachmentjson1, "file", true);
				//	string latestVersionAttName = Helper::GetJSONValue<string>(materialAttachmentjson1, "file_name", true);
				//	if (!FormatHelper::HasContent(latestVersionAttName))
				//		latestVersionAttName = "(unnamed)";
				//	
				//	string modifiedAt = Helper::GetJSONValue<string>(materialAttachmentjson, "_modified_at", true);

				//	/*latestVersionAttUrl = Helper::GetJSONValue<string>(materialAttachmentjson1, "_url_base_template", true);
				//	latestVersionAttUrl = Helper::FindAndReplace(latestVersionAttUrl, "%s", "");
				//	latestVersionAttUrl = latestVersionAttUrl + latestVersionAttId;*/
				//	QString displayDetails = QString::fromStdString(documnetName) + "\n" + (QString::fromStdString(latestVersionAttName) + "\n" + (QString::fromStdString(modifiedAt)));
				//	QListWidgetItem* item = new QListWidgetItem(displayDetails);
				//	//item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
				//	aditionalResultWidget->setProperty(displayDetails.toStdString().c_str(), QString::fromStdString(documentId));
				//	aditionalResultWidget->addItem(item);
				//}
				//aditionalResultWidget->show();
				//ui_3DDesignsLayout->addWidget(aditionalResultWidget);
				//break;
			}
			RESTAPI::SetProgressBarData(0, "", false);
		}
		catch (string msg)
		{
			UTILITY_API->DeleteProgressBar(true);
			Logger::Error("PLMProductResults->drawAddtionalResultTable-> Exception - " + msg);
		}
		catch (exception & e)
		{
			UTILITY_API->DeleteProgressBar(true);
			Logger::Error("PLMProductResults->drawAddtionalResultTable-> Exception - " + string(e.what()));
		}
		catch (const char* msg)
		{
			UTILITY_API->DeleteProgressBar(true);
			wstring wstr(msg, msg + strlen(msg));
			Logger::Error("PLMProductResults->drawAddtionalResultTable-> Exception - " + string(msg));
		}
	}
		//RESTAPI::SetProgressBarData(0, "", false);
		//ui_resultTableLayout->addWidget(aditionalResultWidget);
	//}

	void PLMProductResults::onListItemClicked(QListWidgetItem* _listItem)
	{
		//UTILITY_API->DisplayMessageBox("callign the signal for list..");
		//UTILITY_API->DisplayMessageBox("to_string(_listItem->isSelected())::" + to_string(_listItem->isSelected()));
		string currentSelectedId= aditionalResultWidget->property(_listItem->text().toStdString().c_str()).toString().toStdString();
		if(m_tempId == currentSelectedId)
		//if (_listItem->isSelected())
		{
			m_tempId = "";
			CVWidgetGenerator::GetInstance()->InitializeIconView(aditionalResultWidget);
			//UTILITY_API->DisplayMessageBox("setSelected(false)..");
			_listItem->setSelected(false);
			m_selectedIdnameText = "";
		}
		else
		{
			m_tempId = currentSelectedId;
			//UTILITY_API->DisplayMessageBox("setSelected(true)..");
			_listItem->setSelected(true);
			m_selectedIdnameText = _listItem->text().toStdString();
		}
	}

	void PLMProductResults::DrawResultWidget(bool _isFromConstructor)
	{
		Logger::Info("PLMProductResults -> DrawResultWidget() -> Start");
		m_isHidden = false;
		m_productResults = ProductConfig::GetInstance()->GetStyleResultsSON();
		m_typename = ProductConfig::GetInstance()->GetTypename();
		m_resultsCount = ProductConfig::GetInstance()->GetResultsCount();
		resultTable->clearContents();
		CVWidgetGenerator::PopulateValuesOnResultsUI(nextButton, m_noOfResultLabel, totalPageLabel, m_perPageResultComboBox, Configuration::GetInstance()->GetResultsPerPage(), m_resultsCount);
		resultTable->setEnabled(false);
		m_tabViewButton->setEnabled(false);
		CVWidgetGenerator::DrawViewAndResultsWidget(m_viewComboBox, resultTable, iconTable, false, m_productResults, ProductConfig::GetInstance()->GetProductViewJSON(), IMAGE_DISPLAY_NAME, m_typename, true, ProductConfig::GetInstance()->GetSelectedViewIdx(), ProductConfig::GetInstance()->GetAttScopes(), true, false, ProductConfig::GetInstance()->GetProductViewJSON());
		CVWidgetGenerator::UpdateTableAndIconRows(resultTable, currPageLabel, m_perPageResultComboBox, m_resultsCount, _isFromConstructor);
		setHeaderToolTip();
		resultTable->setEnabled(true);
		AddConnectorForRadioButton();
		hidebuttonClicked(false);
		if (!ProductConfig::GetInstance()->m_isShow3DAttWidget)
		{
			aditionalResultWidget->hide();
			ui_hideButton->hide();
		}
		else {
			ui_hideButton->show();
		}
		m_totalSelected.clear();
		HorizontalHeaderClicked(ProductConfig::GetInstance()->m_sortedColumnNumber);	
		if (Configuration::GetInstance()->GetCurrentScreen() == SEARCH_PRODUCT_CLICKED || Configuration::GetInstance()->GetCurrentScreen() == UPDATE_PRODUCT_CLICKED)
		{
			m_downloadButton->setText("Download");
			m_downloadButton->setToolTip("Download");
			// = CVWidgetGenerator::CreatePushButton("Download", DOWNLOAD_HOVER_ICON_PATH, "Download", PUSH_BUTTON_STYLE, 30, true);
		}
		else
		{
			m_downloadButton->setText("Copy");
			m_downloadButton->setToolTip("Copy");
		}
		UTILITY_API->DeleteProgressBar(true);
	}
}
