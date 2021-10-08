/*
* Copyright 2020-2021 CLO-Vise. All rights reserved
*
* @file PLMColorResults.cpp
*
* @brief Class implementation for cach Colors data in CLO from PLM.
* This class has all the variable and methods implementation which are used in storing PLM Color instance data to view and download in CLO.
*
* @author GoVise
*
* @date  30-MAY-2020
*/
#include "PLMColorResults.h"

#include <iostream>
#include <string>

#include "CLOVise/PLM/Helper/UIHelper/CVWidgetGenerator.h"
#include "CLOVise/PLM/Inbound/Color/PLMColorSearch.h"
#include "CLOVise/PLM/Inbound/Color/ColorConfig.h"
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
	PLMColorResults* PLMColorResults::_instance = NULL;


	PLMColorResults* PLMColorResults::GetInstance()
	{
		if (_instance == NULL)
			_instance = new PLMColorResults();
		return _instance;
	}

	PLMColorResults::PLMColorResults(QWidget* parent)
		: MVDialog(parent)
	{
		Logger::Info("PLMColorResults -> Constructor() -> Start");
		setupUi(this);
		try
		{
			QString windowTitle = PLM_NAME + " PLM "+ QString::fromStdString(Configuration::GetInstance()->GetLocalizedColorClassName()) +" Search Results ";
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
			//setDataFromResponse(ColorConfig::GetInstance()->GetSearchCriteriaJSON());
			DrawResultWidget(true);
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
		Logger::Info("PLMColorResults -> Constructor() -> End");
	}

	PLMColorResults::~PLMColorResults()
	{
		Logger::Info("PLMColorResults -> Destructor() -> Start");
		
	
	
		
		
		Logger::Info("PLMColorResults -> Destructor() -> End");
	}

	/*
	* Description - SetColorHierarchyJSON() method used to cache the hierarchy data.
	* Parameter -  json.
	* Exception -
	* Return -
	*/
	void PLMColorResults::setDataFromResponse(json _param)
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
			m_colorResults = json::parse(resultJsonString);
			string resultsCount = Helper::GetJSONValue<string>(m_colorResults, "resultFound", true);
			if (FormatHelper::HasContent(resultsCount))
			{
				m_resultsCount = stoi(resultsCount);
			}
			else
			{
				m_resultsCount = 0;
			}
			string maxResultsLimit = Helper::GetJSONValue<string>(m_colorResults, "maxResultsLimit", true);
			if (FormatHelper::HasContent(maxResultsLimit))
			{
				m_maxResultsCount = stoi(maxResultsLimit);
			}
			else
			{
				m_maxResultsCount = 50;
			}
			m_typename = Helper::GetJSONValue<string>(m_colorResults, TYPENAME_KEY, true);
			//	m_widgetListJson = Helper::GetJSONValue<string>(m_colorResults, "widgetsList", true);
		}
		catch (string msg)
		{
			UTILITY_API->DeleteProgressBar(true);
			Logger::Error("PLMColorResults::setDataFromResponse() Exception - " + msg);
		}
		catch (exception & e)
		{
			UTILITY_API->DeleteProgressBar(true);
			Logger::Error("PLMColorResults::setDataFromResponse() Exception - " + string(e.what()));
		}
		catch (const char* msg)
		{
			UTILITY_API->DeleteProgressBar(true);
			wstring wstr(msg, msg + strlen(msg));
			Logger::Error("PLMColorResults::setDataFromResponse() Exception - " + string(msg));
		}
	}

	/*
	* Description - AddConnectorForCheckbox() method used to create a connection method for checkbox.
	* Parameter -
	* Exception -
	* Return -
	*/
	void PLMColorResults::AddConnectorForCheckbox()
	{
		Logger::Info("PLMColorResult -> AddConnectorForCheckbox() -> Start");
		int currentPageValue = currPageLabel->text().toInt();
		int m_resultPerPage = m_perPageResultComboBox->currentText().toInt();
		
		QRadioButton* tempRadioButton;
		QCheckBox* tempCheckBox;
		for (int row = 0; row < resultTable->rowCount(); row++) 
		{
			QWidget* qWidget = resultTable->cellWidget(row, 0); //to select multiple rows
			if (Configuration::GetInstance()->GetIsUpdateColorClicked())
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
		Logger::Info("PLMColorResult -> AddConnectorForCheckbox() -> End");
	}

	void PLMColorResults::TableRadioButtonSelected()
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
	* Description - TableCheckBoxSelected() method is a slot for table check box select.
	* Parameter -
	* Exception - exception, Char *
	* Return -
	*/
	void PLMColorResults::onTableCheckBoxSelected()
	{
		Logger::Info("PLMColorResult -> onTableCheckBoxSelected() -> Start");
		TableCheckBoxSelected();
		Logger::Info("PLMColorResult -> onTableCheckBoxSelected() -> End");
	}

	void PLMColorResults::TableCheckBoxSelected()
	{
		Logger::Info("PLMColorResult -> TableCheckBoxSelected() -> Start");
		try
		{
			m_rowsSelected.clear();
			m_iconsSelected.clear();
			m_rowsSelected = CVWidgetGenerator::TableRowSelected(resultTable, iconTable, false, false);
			m_totalSelected = CVWidgetGenerator::SetDownloadCount(m_iconsSelected, m_rowsSelected, m_downloadButton);
		}
		catch (string msg)
		{
			Logger::Error("PLMColorResults-> TableCheckBoxSelected Exception - " + msg);
			UTILITY_API->DisplayMessageBox(msg);
		}
		catch (exception& e)
		{
			Logger::Error("PLMColorResults-> TableCheckBoxSelected Exception - " + string(e.what()));
			UTILITY_API->DisplayMessageBox(e.what());

		}
		catch (const char* msg)
		{
			Logger::Error("PLMColorResults-> TableCheckBoxSelected Exception - " + string(msg));
			wstring wstr(msg, msg + strlen(msg));
			UTILITY_API->DisplayMessageBoxW(wstr);
		}
		Logger::Info("PLMColorResults -> TableCheckBoxSelected() -> End");
	}

	///*
	//* Description - IconCheckBoxSelected() method is a slot for icon check box select.
	//* Parameter -
	//* Exception - exception, Char *
	//* Return -
	//*/
	//void PLMColorResults::onIconCheckBoxSelected()
	//{
	//	Logger::Info("PLMColorResults -> onIconCheckBoxSelected() -> Start");
	//	IconCheckBoxSelected();
	//	Logger::Info("PLMColorResults -> onIconCheckBoxSelected() -> End");
	//}
	//void PLMColorResults::IconCheckBoxSelected()
	//{
	//	Logger::Info("PLMColorResults -> IconCheckBoxSelected() -> Start");
	//	try
	//	{
	//		m_iconsSelected.clear();
	//		m_rowsSelected.clear();
	//		m_iconsSelected = CVWidgetGenerator::IconRowSelected(iconTable, resultTable, false);
	//		m_totalSelected = CVWidgetGenerator::SetDownloadCount(m_iconsSelected, m_rowsSelected, m_downloadButton);
	//	}
	//	catch (exception& e)
	//	{
	//		Logger::Error("PLMColorResults-> IconCheckBoxSelected  Exception- " + string(e.what()));
	//		UTILITY_API->DisplayMessageBox(e.what());
	//	}
	//	catch (const char* msg)
	//	{
	//		Logger::Error("PLMColorResults-> IconCheckBoxSelected  Exception- " + string(msg));
	//		wstring wstr(msg, msg + strlen(msg));
	//		UTILITY_API->DisplayMessageBoxW(wstr);
	//	}
	//	Logger::Info("PLMColorResults -> IconCheckBoxSelected() -> End");
	//}

	/*
	* Description - setHeaderToolTip() method used to set header tool tip.
	* Parameter -
	* Exception -
	* Return -
	*/
	void PLMColorResults::setHeaderToolTip()
	{
		Logger::Info("PLMColorResults -> setHeaderToolTip() -> Start");
		for (int i = 0; i < resultTable->columnCount(); i++)
		{
			QString headerValue = resultTable->horizontalHeaderItem(i)->text();
			resultTable->horizontalHeaderItem(i)->setToolTip(headerValue);
		}
		Logger::Info("PLMColorResults -> setHeaderToolTip() -> Start");
	}

	/*
	* Description - onClickedDeselectAllButton() method is slot for deselect all click to unselect all selected item in the result.
	* Parameter -
	* Exception -
	* Return -
	*/
	void PLMColorResults::onClickedDeselectAllButton()
	{
		Logger::Info("PLMColorResults -> onClickedDeselectAllButton() -> Start");
		ClickedDeselectAllButton();
		Logger::Info("PLMColorResults -> onClickedDeselectAllButton() -> End");
	}

	void PLMColorResults::ClickedDeselectAllButton()
	{
		Logger::Info("PLMColorResults -> ClickedDeselectAllButton() -> Start");
		CVWidgetGenerator::DeSelectAllClicked(resultTable, iconTable, m_downloadButton);
		m_rowsSelected.clear();
		m_iconsSelected.clear();
		m_totalSelected.clear();
		Logger::Info("PLMColorResults -> ClickedDeselectAllButton() -> End");
	}
	
	/*
	* Description - onClickedDeselectAllButton() method is slot for deselect all click to unselect all selected item in the result.
	* Parameter -
	* Exception -
	* Return -
	*/
	void PLMColorResults::onClickedSelectAllButton()
	{
		Logger::Info("PLMColorResults -> onClickedSelectAllButton() -> Start");
		ClickedSelectAllButton();
		Logger::Info("PLMColorResults -> onClickedSelectAllButton() -> End");
	}

	void PLMColorResults::ClickedSelectAllButton()
	{
		Logger::Info("PLMColorResults -> ClickedSelectAllButton() -> Start");
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

		Logger::Info("PLMColorResults -> ClickedSelectAllButton() -> End");
	}

	/*
	* Description - onClickedBackButton() method is slot for back button click.
	* Parameter -
	* Exception -
	* Return -
	*/
	void PLMColorResults::onClickedBackButton()
	{
		Logger::Info("PLMColorResults -> onClickedBackButton() -> Start");
		backButtonClicked();
		Logger::Info("PLMColorResults -> onClickedBackButton() -> End");
	}

	void PLMColorResults::backButtonClicked()
	{
		Logger::Info("PLMColorResults -> backButtonClicked() -> Start");
		this->close();
		CLOVise::PLMColorSearch::GetInstance()->setModal(true);
		CLOVise::PLMColorSearch::GetInstance()->show();
		Logger::Info("PLMColorResults -> backButtonClicked() -> End");
		
	}

	/*
	* Description - onClickedNextButton() method is a slot for next button click.
	* Parameter -
	* Exception -
	* Return -
	*/
	void PLMColorResults::onClickedNextButton()
	{
		Logger::Info("PLMColorResults -> onClickedNextButton() -> Start");
		ClickedNextButton();
		Logger::Info("PLMColorResults -> onClickedNextButton() -> End");
	}

	void PLMColorResults::ClickedNextButton()
	{
		Logger::Info("PLMColorResults -> ClickedNextButton() -> Start");
		int currPage = currPageLabel->text().toInt() + 1;
		currPageLabel->setText(QString::fromStdString(to_string(currPage)));
		if (currPage == totalPageLabel->text().toInt())
			nextButton->setEnabled(false);
		if (m_isTabularView)
			CVWidgetGenerator::UpdateTableAndIconRows(resultTable, currPageLabel, m_perPageResultComboBox, m_resultsCount, false);
		else
			CVWidgetGenerator::UpdateTableAndIconRows(iconTable, currPageLabel, m_perPageResultComboBox, m_resultsCount);
		previousButton->setEnabled(true);
		Logger::Info("PLMColorResults -> ClickedNextButton() -> End");
	}

	/*
	* Description - onClickedPreviousButton() method is a slot for previous button click.
	* Parameter -
	* Exception -
	* Return -
	*/
	void PLMColorResults::onClickedPreviousButton()
	{
		Logger::Info("PLMColorResults -> onClickedPreviousButton() -> Start");
		ClickedPreviousButton();
		Logger::Info("PLMColorResults -> onClickedPreviousButton() -> End");
	}

	void PLMColorResults::ClickedPreviousButton()
	{
		Logger::Info("PLMColorResults -> ClickedPreviousButton() -> Start");
		int currPage = currPageLabel->text().toInt() - 1;
		currPageLabel->setText(QString::fromStdString(to_string(currPage)));
		if (currPage == 1)
			previousButton->setEnabled(false);
		if (m_isTabularView)
			CVWidgetGenerator::UpdateTableAndIconRows(resultTable, currPageLabel, m_perPageResultComboBox, m_resultsCount, false);
		else
			CVWidgetGenerator::UpdateTableAndIconRows(iconTable, currPageLabel, m_perPageResultComboBox, m_resultsCount);
		nextButton->setEnabled(true);
		Logger::Info("PLMColorResults -> ClickedPreviousButton() -> End");
	}
	/*
	* Description - onResultPerPageCurrentIndexChanged() method is a slot for result per page click.
	* Parameter -  string.
	* Exception -
	* Return -
	*/
	void PLMColorResults::onResultPerPageCurrentIndexChanged(const QString&)
	{
		Logger::Info("PLMColorResults -> onResultPerPageCurrentIndexChanged() -> Start");
		int currPage = 1;
		currPageLabel->setText(QString::fromStdString(to_string(currPage)));
		if (currPage == 1)
			previousButton->setEnabled(false);
		CVWidgetGenerator::PopulateValuesOnResultsUI(nextButton, m_noOfResultLabel, totalPageLabel, m_perPageResultComboBox, m_resultsCount);
		CVWidgetGenerator::UpdateTableAndIconRows(resultTable, currPageLabel, m_perPageResultComboBox, m_resultsCount, false);
		CVWidgetGenerator::UpdateTableAndIconRows(iconTable, currPageLabel, m_perPageResultComboBox, m_resultsCount);
		//Caching selected option to remember irrespective of sessions
		Configuration::GetInstance()->SetSelectedResultsPerPage(m_perPageResultComboBox->currentText().toStdString(), true, false);
		Logger::Info("PLMColorResults -> onResultPerPageCurrentIndexChanged() -> End");
	}

	/*
	* Description - onResultViewIndexChanged() method is a slot for  views click.
	* Parameter -  string.
	* Exception - exception, Char *
	* Return -
	*/

	void PLMColorResults::onResultViewIndexChanged(const QString& _str)
	{
		Logger::Info("PLMColorResults -> onResultViewIndexChanged() -> Start");
		ResultViewIndexChanged(_str);
		Logger::Info("PLMColorResults -> onResultViewIndexChanged() -> End");
	}
	void PLMColorResults::ResultViewIndexChanged(const QString&)
	{
		Logger::Info("PLMColorResults -> ResultViewIndexChanged() -> Start");
		try
		{
			UTILITY_API->DeleteProgressBar(true);

			this->hide();

			UTILITY_API->CreateProgressBar();
			UTILITY_API->SetProgress("Loading", (qrand() % 101));
			Logger::Info("PLMColorResults -> onResultViewIndexChanged() -> Start");
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
			CVWidgetGenerator::DrawViewAndResultsWidget(m_viewComboBox, resultTable, iconTable, false, m_colorResults, ColorConfig::GetInstance()->GetColorViewJSON(), "Color Image", m_typename, false, m_viewComboBox->currentIndex(), ColorConfig::GetInstance()->GetAttScopes(), false, false, ColorConfig::GetInstance()->GetColorViewJSON());
			onTabViewClicked();
			setHeaderToolTip();
			AddConnectorForCheckbox();
			resultTable->setEnabled(true);
			RESTAPI::SetProgressBarData(0, "", false);
			this->show();

		}
		catch (string msg)
		{
			Logger::Error("PLMColorResults -> onResultViewIndexChanged Exception :: " + msg);
			UTILITY_API->DisplayMessageBox(msg);
			RESTAPI::SetProgressBarData(0, "", false);
			this->close();
		}
		catch (exception& e)
		{
			Logger::Error("PLMColorResults -> onResultViewIndexChanged Exception :: " + string(e.what()));
			UTILITY_API->DisplayMessageBox(e.what());
			RESTAPI::SetProgressBarData(0, "", false);
			this->close();
		}
		catch (const char* msg)
		{
			Logger::Error("PLMColorResults -> onResultViewIndexChanged Exception :: " + string(msg));
			wstring wstr(msg, msg + strlen(msg));
			UTILITY_API->DisplayMessageBoxW(wstr);
			RESTAPI::SetProgressBarData(0, "", false);
			this->close();
		}
		Logger::Info("PLMColorResults -> onResultViewIndexChanged() -> End");
	}

	/*
	* Description - SetColorHierarchyJSON() method is a slot for icon view click.
	* Parameter -
	* Exception -
	* Return -
	*/
	void PLMColorResults::onIconViewClicked()
	{
		Logger::Info("PLMColorResults -> onIconViewClicked() -> Start");
		updateTableForIconView();
		Logger::Info("PLMColorResults -> onIconViewClicked() -> End");
	}

	void PLMColorResults::updateTableForIconView()
	{
		Logger::Info("PLMColorResults -> updateTableForIconView() -> Start");
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
			CVWidgetGenerator::SortIconResultTable(m_isResultTableSorted, iconTable, resultTable, m_colorResults, false);
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
		Logger::Info("PLMColorResults -> updateTableForIconView() -> End");
	}

	/*
	* Description - SetColorHierarchyJSON() method is a slot for table view click.
	* Parameter -
	* Exception -
	* Return -
	*/
	void PLMColorResults::onTabViewClicked()
	{
		Logger::Info("PLMColorResults -> onTabViewClicked() -> Start");
		updateTableForTabView();
		Logger::Info("PLMColorResults -> onTabViewClicked() -> End");
	}


	void PLMColorResults::updateTableForTabView()
	{
		Logger::Info("PLMColorResults -> updateTableForTabView() -> Start");
		m_isTabularView = true;
		m_tabViewButton->setEnabled(false);
		iconTable->close();
		m_iconViewButton->setEnabled(true);
		CVWidgetGenerator::UpdateTableAndIconRows(resultTable, currPageLabel, m_perPageResultComboBox, m_resultsCount, false);
		resultTable->show();
		ui_resultTableLayout->addWidget(resultTable);
		Logger::Info("PLMColorResults -> updateTableForTabView() -> End");
	}

	/*
	* Description - onHorizontalHeaderClicked() method is a slot for horizontal header click and sorting the columns.
	* Parameter -  int.
	* Exception -
	* Return -
	*/
	void PLMColorResults::onHorizontalHeaderClicked(int _column)
	{
		Logger::Info("PLMColorResults -> onHorizontalHeaderClicked() -> Start");
		horizontalHeaderClicked(_column);
		Logger::Info("PLMColorResults -> onHorizontalHeaderClicked() -> End");
	}

	void PLMColorResults::horizontalHeaderClicked(int _column)
	{
		Logger::Info("ColorResultTable -> horizontalHeaderClicked() -> Start");
		Logger::Debug("Column.." + to_string(_column));
		if (_column == CHECKBOX_COLUMN || _column == IMAGE_COLUMN)
		{
			resultTable->setSortingEnabled(false);
			resultTable->horizontalHeader()->setSortIndicatorShown(false);
		}
		else
		{
			resultTable->setSortingEnabled(true);
			resultTable->horizontalHeader()->setSortIndicatorShown(true);
			m_isResultTableSorted = true;
			AddConnectorForCheckbox();
			CVWidgetGenerator::UpdateTableAndIconRows(resultTable, currPageLabel, m_perPageResultComboBox, m_resultsCount, false);
		}
		Logger::Info("ColorResultTable -> horizontalHeaderClicked() -> End");
	}

	/*
	* Description - connectSignalSlots() method used to connect and disconnect the signals and slots.
	* Parameter -  bool.
	* Exception -
	* Return -
	*/
	void PLMColorResults::connectSignalSlots(bool _b)
	{
		Logger::Info("PLMColorResults -> connectSignalSlots() -> Start");
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
		Logger::Info("PLMColorResults -> connectSignalSlots() -> End");
	}

	/*
	* Description - DownloadClicked() method used to download the selected items.
	* Parameter -
	* Exception - exception, Char *
	* Return -
	*/
	void PLMColorResults::onDownloadClicked()
	{
		Logger::Info("PLMColorResults -> onDownloadClicked() -> Start");
		downloadColorResult();
		Logger::Info("PLMColorResults -> onDownloadClicked() -> End");
	}

	void PLMColorResults::downloadColorResult()
	{
		Logger::Info("PLMColorResults -> downloadColorResult() -> Start");
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
				Logger::Debug("PLMColorResults -> colorResultTableDownload_clicked() -> CreateProduct::GetInstance()");
				RESTAPI::SetProgressBarData(30, "Adding Colors", true);
				UTILITY_API->SetProgress("Adding Colors", (qrand() % 101));

				RESTAPI::SetProgressBarData(14, "Adding Colors", true);
				if (Configuration::GetInstance()->GetIsUpdateColorClicked())
					returnValue = CreateProduct::GetInstance()->UpdateColorInColorways(m_totalSelected, m_colorResults);

				if (returnValue)
				{
					Logger::Debug("PLMColorResults -> void onDownloadClicked TRue");
					RESTAPI::SetProgressBarData(0, "", false);
					CreateProduct::GetInstance()->setModal(true);
					CreateProduct::GetInstance()->show();
				}
				else
				{
					Logger::Debug("PLMColorResults -> void onDownloadClicked False");
					RESTAPI::SetProgressBarData(0, "", false);
					UTILITY_API->DisplayMessageBox(Configuration::GetInstance()->GetLocalizedColorClassName() + " Specification must be unique");
					this->show();
				}

				
			}
			break;
			case UPDATE_PRODUCT_CLICKED:
			{
				Logger::Debug("PLMColorResults -> colorResultTableDownload_clicked() -> CreateProduct::GetInstance()");
				RESTAPI::SetProgressBarData(30, "Adding Colors", true);
				UTILITY_API->SetProgress("Adding Colors", (qrand() % 101));

				RESTAPI::SetProgressBarData(14, "Adding Colors", true);
				if (Configuration::GetInstance()->GetIsUpdateColorClicked())
					returnValue = UpdateProduct::GetInstance()->UpdateColorInColorways(m_totalSelected, m_colorResults);
				else
				{
					UpdateProduct::GetInstance()->AddColorwayDetails(m_totalSelected, m_colorResults);
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
					UTILITY_API->DisplayMessageBox("The Color Specification must be unique");
					this->show();
				}
			}
			break;
			case COLOR_SEARCH_CLICKED:
			{
				RESTAPI::SetProgressBarData(30, "Downloading " + Configuration::GetInstance()->GetLocalizedColorClassName(), true);
				UTILITY_API->SetProgress("Downloading " + Configuration::GetInstance()->GetLocalizedColorClassName(), (qrand() % 101));

				RESTAPI::SetProgressBarData(14, "Downloading "+ Configuration::GetInstance()->GetLocalizedColorClassName(), true);
				QString faildesObjectsString = UIHelper::DownloadResults(m_totalSelected, COLOR_MODULE, m_colorResults);
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
					if (!Configuration::GetInstance()->GetIsUpdateColorClicked())
					onClickedDeselectAllButton();
					this->show();
				}
			}
			}
			Logger::Info("PLMColorResults -> void onDownloadClicked end");
		}
		catch (string msg)
		{
			Logger::Error("PLMColorResults -> DownloadClicked Exception :: " + msg);
			UTILITY_API->DisplayMessageBox(msg);
			if (Configuration::GetInstance()->GetCloseResultsDialogue() && rowsSelected)
			{
				this->close();
				
			}
			else
			{
				if (!Configuration::GetInstance()->GetIsUpdateColorClicked())
				onClickedDeselectAllButton();
				this->show();
			}
		}
		catch (exception& e)
		{
			Logger::Error("PLMColorResults -> DownloadClicked Exception :: " + string(e.what()));
			UTILITY_API->DisplayMessageBox(e.what());
			if (Configuration::GetInstance()->GetCloseResultsDialogue() && rowsSelected)
			{
				this->close();
				
			}
			else
			{
				if (!Configuration::GetInstance()->GetIsUpdateColorClicked())
				onClickedDeselectAllButton();
				this->show();
			}
		}
		catch (const char* msg)
		{
			Logger::Error("PLMColorResults -> DownloadClicked Exception :: " + string(msg));
			wstring wstr(msg, msg + strlen(msg));
			UTILITY_API->DisplayMessageBoxW(wstr);
			if (Configuration::GetInstance()->GetCloseResultsDialogue() && rowsSelected)
			{
				this->close();
				
			}
			else
			{
				if (!Configuration::GetInstance()->GetIsUpdateColorClicked())
				onClickedDeselectAllButton();
				this->show();
			}
		}
		Logger::Info("PLMColorResults -> downloadColorResult() -> End");
	}

	/*
	* Description - DrawResultWidget() method is create/reset the result widget.
	* Parameter - bool
	* Exception - 
	* Return -
	*/
	void PLMColorResults::DrawResultWidget(bool _isFromConstructor)
	{
		Logger::Info("PLMColorResults -> DrawResultWidget() -> Start");
		m_colorResults = ColorConfig::GetInstance()->GetColorResultsSON();
		m_typename = ColorConfig::GetInstance()->GetTypename();
		m_maxResultsCount = ColorConfig::GetInstance()->GetMaxResultCount();
		m_resultsCount = ColorConfig::GetInstance()->GetResultsCount();
		resultTable->clear();
		CVWidgetGenerator::PopulateValuesOnResultsUI(nextButton, m_noOfResultLabel, totalPageLabel, m_perPageResultComboBox, Configuration::GetInstance()->GetResultsPerPage(), m_resultsCount);
		resultTable->setEnabled(false);
		m_tabViewButton->setEnabled(false);
		if (Configuration::GetInstance()->GetIsUpdateColorClicked())
		{
			CVWidgetGenerator::DrawViewAndResultsWidget(m_viewComboBox, resultTable, iconTable, false, m_colorResults, ColorConfig::GetInstance()->GetColorViewJSON(), "Color Image", m_typename, true, ColorConfig::GetInstance()->GetSelectedViewIdx(), ColorConfig::GetInstance()->GetAttScopes(), true, false, ColorConfig::GetInstance()->GetColorViewJSON());
			m_selectAllButton->hide();
			m_deSelectAllButton->hide();
		}
		else
			CVWidgetGenerator::DrawViewAndResultsWidget(m_viewComboBox, resultTable, iconTable, false, m_colorResults, ColorConfig::GetInstance()->GetColorViewJSON(), "Color Image", m_typename, true, ColorConfig::GetInstance()->GetSelectedViewIdx(), ColorConfig::GetInstance()->GetAttScopes(), false, false, ColorConfig::GetInstance()->GetColorViewJSON());

		CVWidgetGenerator::UpdateTableAndIconRows(resultTable, currPageLabel, m_perPageResultComboBox, m_resultsCount, _isFromConstructor);
		setHeaderToolTip();
		resultTable->setEnabled(true);
		AddConnectorForCheckbox();
		if (Configuration::GetInstance()->GetCurrentScreen() == COLOR_SEARCH_CLICKED)
		{
			m_downloadButton->setText("Download");
			m_downloadButton->setIcon(QIcon(QString(DOWNLOAD_HOVER_ICON_PATH)));
		}
		else
		{
			m_downloadButton->setText("Add");
			m_downloadButton->setIcon(QIcon(QString(ADD_HOVER_ICON_PATH)));
		}
		m_totalSelected.clear();
		horizontalHeaderClicked(ColorConfig::GetInstance()->m_sortedColumnNumber);
		Logger::Info("PLMColorResults -> DrawResultWidget() -> End");
	}
}
