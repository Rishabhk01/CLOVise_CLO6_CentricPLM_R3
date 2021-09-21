/*
* Copyright 2020-2021 CLO-Vise. All rights reserved
*
* @file PLMMaterialResult.cpp
*
* @brief Class implementation for cach Material data in CLO from PLM.
* This class has all the variable and methods implementation which are used in storing PLM Material instance data to view and download in CLO.
*
* @author GoVise
*
* @date 16-JUN-2020
*/
#include "PLMMaterialResult.h"

#include <iostream>
#include <string>
#include <time.h>

#include "CLOVise/PLM/Helper/UIHelper/CVWidgetGenerator.h"
#include "CLOVise/PLM/Inbound/Material/PLMMaterialSearch.h"
#include "CLOVise/PLM/Inbound/Material/MaterialConfig.h"
#include "CLOVise/PLM/Helper/Util/FormatHelper.h"
#include "CLOVise/PLM/Helper/Util/RestAPIUtility.h"
#include "CLOVise/PLM/Helper/UIHelper/UIHelper.h"
#include "CLOVise/PLM/Helper/UIHelper/CVHoverDelegate.h"
#include "CLOVise/PLM/Helper/UIHelper/CVDisplayMessageBox.h"
#include "CLOVise/PLM/Helper/Util/Logger.h"
#include "CLOVise/PLM/Outbound/UpdateMaterial/UpdateMaterial.h"
#include "CLOVise/PLM/Outbound/CraeteMaterial/CreateMaterial.h"

using namespace std;

namespace CLOVise
{
	PLMMaterialResult* PLMMaterialResult::_instance = NULL;

	PLMMaterialResult* PLMMaterialResult::GetInstance()
	{
		if (_instance == NULL)
			_instance = new PLMMaterialResult();
		return _instance;
	}

	void PLMMaterialResult::Destroy()
	{
		if (_instance)
		{
			//delete _instance;
			_instance = NULL;
		}
	}

	PLMMaterialResult::PLMMaterialResult(QWidget* parent)
		: MVDialog(parent)
	{
		Logger::Logger("PLMMaterialResult preparing ui started...");
		Logger::Info("PLMMaterialResult -> Constructor() -> Start");
		setupUi(this);
		try
		{
			QString windowTitle = PLM_NAME + " PLM "+ QString::fromStdString(Configuration::GetInstance()->GetLocalizedMaterialClassName()) +" Search Results";
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
			ui_toolLayout->insertWidget(9, m_iconViewButton);


			QSpacerItem *horizontalSpacer_4 = new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Fixed);
			QSpacerItem *horizontalSpacer_5 = new QSpacerItem(10, 20, QSizePolicy::Expanding, QSizePolicy::Fixed);
			m_backButton = CVWidgetGenerator::CreatePushButton("Back", BACK_HOVER_ICON_PATH, "Back", BUTTON_STYLE, 30, true);
			m_backButton->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
			m_backButton->setFocusPolicy(Qt::NoFocus);
			ui_buttonLayout->insertWidget(1, m_backButton);

			ui_buttonLayout->insertSpacerItem(2, horizontalSpacer_5);
			m_deSelectAllButton = CVWidgetGenerator::CreatePushButton("Unselect All", OK_NONE_ICON_PATH, "Unselect All", PUSH_BUTTON_STYLE, 30, true);
			m_deSelectAllButton->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
			m_deSelectAllButton->setFocusPolicy(Qt::NoFocus);
			ui_buttonLayout->insertWidget(3, m_deSelectAllButton);
			ui_buttonLayout->insertSpacerItem(4, horizontalSpacer_5);

			m_downloadButton = CVWidgetGenerator::CreatePushButton("Download", DOWNLOAD_HOVER_ICON_PATH, "Download", PUSH_BUTTON_STYLE, 30, true);
			m_downloadButton->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
			m_downloadButton->setFocusPolicy(Qt::StrongFocus);
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
			//setDataFromResponse(MaterialConfig::GetInstance()->GetSearchCriteriaJSON());
			m_materialResults = MaterialConfig::GetInstance()->GetMaterialResultsSON();
			m_typename= MaterialConfig::GetInstance()->GetTypename();
			//m_maxResultsCount= MaterialConfig::GetInstance()->GetMaxResultCount();
			m_resultsCount= MaterialConfig::GetInstance()->GetResultsCount();
			/*if (m_resultsCount > m_maxResultsCount)
				throw "Maximum results limit exceeded. Please refine your search.";*/
			CVWidgetGenerator::PopulateValuesOnResultsUI(nextButton, m_noOfResultLabel, totalPageLabel, m_perPageResultComboBox, Configuration::GetInstance()->GetResultsPerPage(), m_resultsCount);
			resultTable->setEnabled(false);
			m_tabViewButton->setEnabled(false);

			if (MaterialConfig::GetInstance()->GetIsRadioButton()) {
				m_deSelectAllButton->hide();
				//CVWidgetGenerator::DrawViewAndResultsWidget(m_viewComboBox, resultTable, iconTable, m_materialResults, MaterialConfig::GetInstance()->GetMaterialViewJSON(), "Material Image", m_typename, true, MaterialConfig::GetInstance()->GetSelectedViewIdx(), MaterialConfig::GetInstance()->GetAttScopes(), true, false);
				CVWidgetGenerator::DrawViewAndResultsWidget(m_viewComboBox, resultTable, iconTable, false, m_materialResults, MaterialConfig::GetInstance()->GetMaterialViewJSON(), IMAGE_DISPLAY_NAME, m_typename, true, MaterialConfig::GetInstance()->GetSelectedViewIdx(), MaterialConfig::GetInstance()->GetAttScopes(), true, false, MaterialConfig::GetInstance()->GetMaterialViewJSON());
			}
			else {
				m_deSelectAllButton->show();
				CVWidgetGenerator::DrawViewAndResultsWidget(m_viewComboBox, resultTable, iconTable, false, m_materialResults, MaterialConfig::GetInstance()->GetMaterialViewJSON(), IMAGE_DISPLAY_NAME, m_typename, true, MaterialConfig::GetInstance()->GetSelectedViewIdx(), MaterialConfig::GetInstance()->GetAttScopes(), false, false, MaterialConfig::GetInstance()->GetMaterialViewJSON());
				//CVWidgetGenerator::DrawViewAndResultsWidget(m_viewComboBox, resultTable, iconTable, m_materialResults, MaterialConfig::GetInstance()->GetMaterialViewJSON(), "Material Image", m_typename, true, MaterialConfig::GetInstance()->GetSelectedViewIdx(), MaterialConfig::GetInstance()->GetAttScopes(), false, false);
			}
			CVWidgetGenerator::UpdateTableAndIconRows(resultTable, currPageLabel, m_perPageResultComboBox, m_resultsCount, false);
			CVWidgetGenerator::UpdateTableAndIconRows(iconTable, currPageLabel, m_perPageResultComboBox, m_resultsCount);
			setHeaderToolTip();
			resultTable->setEnabled(true);
			connectSignalSlots(true);
			if (MaterialConfig::GetInstance()->GetIsRadioButton()) {
				AddConnectorForRadioButton();
			}
			else {
				AddConnectorForCheckbox();
			}
			RESTAPI::SetProgressBarData(0, "", false);
		}
		catch (string msg)
		{
			Destroy();
			throw msg;
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
		Logger::Info("PLMMaterialResult -> Constructor() -> End");
		Logger::Logger("PLMMaterialResult preparing ui ended...");
	}

	PLMMaterialResult::~PLMMaterialResult()
	{
		Destroy();
	}

	/*
* Description - AddConnectorForCheckbox() method used connect the check box signal and slots.
* Parameter -
* Exception -
* Return -
*/
	void PLMMaterialResult::AddConnectorForCheckbox()
	{
		Logger::Info("PLMMaterialResult -> AddConnectorForCheckbox() -> Start");
		int currentPageValue = currPageLabel->text().toInt();
		int m_resultPerPage = m_perPageResultComboBox->currentText().toInt();
		QCheckBox* tempCheckBox;
		for (int row = 0; row < resultTable->rowCount(); row++) {
			auto itemWidget = iconTable->item(row);
			tempCheckBox = qobject_cast<QCheckBox*>(iconTable->itemWidget(itemWidget));
			QObject::connect(tempCheckBox, SIGNAL(clicked()), this, SLOT(onIconCheckBoxSelected()));

			QWidget* qWidget = resultTable->cellWidget(row, 0); //to select multiple rows
			tempCheckBox = qobject_cast <QCheckBox*> (qWidget->layout()->itemAt(0)->widget());
			QObject::connect(tempCheckBox, SIGNAL(clicked()), this, SLOT(onTableCheckBoxSelected()));
		}
		Logger::Info("PLMMaterialResult -> AddConnectorForCheckbox() -> End");
	}

	/*
	* Description - AddConnectorForRadioButton() method used connect the ratio button signal and slots.
	* Parameter -
	* Exception -
	* Return -
	*/
	void PLMMaterialResult::AddConnectorForRadioButton()
	{
		Logger::Info("PLMMaterialResult -> AddConnectorForRadioButton() -> Start");
		int currentPageValue = currPageLabel->text().toInt();
		int m_resultPerPage = m_perPageResultComboBox->currentText().toInt();
		QRadioButton* tempRadioButton;
		for (int row = 0; row < resultTable->rowCount(); row++) {
			auto itemWidget = iconTable->item(row);
			tempRadioButton = qobject_cast<QRadioButton*>(iconTable->itemWidget(itemWidget));
			QObject::connect(tempRadioButton, SIGNAL(clicked()), this, SLOT(onIconRadioButtonSelected()));

			QWidget* qWidget = resultTable->cellWidget(row, 0); //to select multiple rows
			tempRadioButton = qobject_cast<QRadioButton*>(qWidget);
			QObject::connect(tempRadioButton, SIGNAL(clicked()), this, SLOT(onTableRadioButtonSelected()));
		}
		Logger::Info("PLMMaterialResult -> AddConnectorForRadioButton() -> end");
	}
	/*
	* Description - IconRadioButtonSelected() method is a slot for icon view radio button click.
	* Parameter -
	* Exception - exception, Char *
	* Return -
	*/
	void PLMMaterialResult::onIconRadioButtonSelected()
	{
		IconCheckBoxSelected();
	}
	/*
	* Description - TableRadioButtonSelected() method is a slot for tabular view radio button click.
	* Parameter -
	* Exception - exception, Char *
	* Return -
	*/
	void PLMMaterialResult::onTableRadioButtonSelected()
	{
		TableRadioButtonSelected();
	}
	/*
	* Description - TableRadioButtonSelected() method is a slot for tabular view radio button click.
	* Parameter -
	* Exception - exception, Char *
	* Return -
	*/
	void PLMMaterialResult::TableRadioButtonSelected()
	{
		Logger::Info("PLMMaterialResult -> TableRadioButtonSelected() -> Start");
		try
		{
			m_rowsSelected.clear();
			m_iconsSelected.clear();
			//this->hide();
			//UTILITY_API->CreateProgressBar();
			//RESTAPI::SetProgressBarData(10, "Loading 3D Documents..", true);
			m_rowsSelected = CVWidgetGenerator::TableRowSelected(resultTable, iconTable, true, false);
			m_totalSelected = CVWidgetGenerator::SetDownloadCount(m_iconsSelected, m_rowsSelected, m_downloadButton);
			//m_rowsSelected = CVWidgetGenerator::TableRowSelected(resultTable, iconTable, false);
			//m_totalSelected = CVWidgetGenerator::SetDownloadCount(m_iconsSelected, m_rowsSelected, m_downloadButton);
			//drawAddtionalResultTable(m_totalSelected);
			//m_isHidden = true;
			//hidebuttonClicked(false);
			//this->show();
			Logger::Info("PLMMaterialResult -> TableRadioButtonSelected() -> end");
		}
		catch (string msg)
		{
			Logger::Error("PLMMaterialResult -> TableRadioButtonSelected Exception :: " + msg);
			UTILITY_API->DisplayMessageBox(msg);
		}
		catch (exception& e)
		{
			Logger::Error("PLMMaterialResult -> TableRadioButtonSelected Exception :: " + string(e.what()));
			UTILITY_API->DisplayMessageBox(e.what());
		}
		catch (const char* msg)
		{
			Logger::Error("PLMMaterialResult -> TableRadioButtonSelected Exception :: " + string(msg));
			wstring wstr(msg, msg + strlen(msg));
			UTILITY_API->DisplayMessageBoxW(wstr);
		}
	}
	/*
	* Description - TableRadioButtonSelected() method is a slot for tabular view Check box click.
	* Parameter -
	* Exception - exception, Char *
	* Return -
	*/
	void PLMMaterialResult::onTableCheckBoxSelected()
	{
		Logger::Info("PLMMaterialResult -> onTableCheckBoxSelected() -> Start");
		TableCheckBoxSelected();
		Logger::Info("PLMMaterialResult -> onTableCheckBoxSelected() -> End");
	}

	void PLMMaterialResult::TableCheckBoxSelected()
	{
		Logger::Info("PLMMaterialResult -> TableCheckBoxSelected() -> Start");
		try
		{
			m_rowsSelected.clear();
			m_iconsSelected.clear();
			m_rowsSelected = CVWidgetGenerator::TableRowSelected(resultTable, iconTable, false, false);
			m_totalSelected = CVWidgetGenerator::SetDownloadCount(m_iconsSelected, m_rowsSelected, m_downloadButton);
		}
		catch (string msg)
		{
			Logger::Error("PLMMaterialResult -> TableCheckBoxSelected Exception :: " + msg);
			UTILITY_API->DisplayMessageBox(msg);
		}
		catch (exception& e)
		{
			Logger::Error("PLMMaterialResult -> TableCheckBoxSelected Exception :: " + string(e.what()));
			UTILITY_API->DisplayMessageBox(e.what());
		}
		catch (const char* msg)
		{
			Logger::Error("PLMMaterialResult -> TableCheckBoxSelected Exception :: " + string(msg));
			wstring wstr(msg, msg + strlen(msg));
			UTILITY_API->DisplayMessageBoxW(wstr);
		}
		Logger::Info("PLMMaterialResult -> TableCheckBoxSelected() -> End");
	}

	/*
	* Description - IconCheckBoxSelected() method is a slot for icon check box select.
	* Parameter -
	* Exception - exception, Char *
	* Return -
	*/
	void PLMMaterialResult::onIconCheckBoxSelected()
	{
		Logger::Info("PLMMaterialResult -> onIconCheckBoxSelected() -> Start");
		IconCheckBoxSelected();
		Logger::Info("PLMMaterialResult -> onIconCheckBoxSelected() -> End");
	}

	void PLMMaterialResult::IconCheckBoxSelected()
	{
		Logger::Info("PLMMaterialResult -> IconCheckBoxSelected() -> Start");
		try
		{
			m_iconsSelected.clear();
			m_rowsSelected.clear();
			m_iconsSelected = CVWidgetGenerator::IconRowSelected(iconTable, resultTable, false);
			m_totalSelected = CVWidgetGenerator::SetDownloadCount(m_iconsSelected, m_rowsSelected, m_downloadButton);
		}
		catch (string msg)
		{
			Logger::Error("PLMMaterialResult -> IconCheckBoxSelected Exception :: " + msg);
			UTILITY_API->DisplayMessageBox(msg);
		}
		catch (exception& e)
		{
			Logger::Error("PLMMaterialResult -> IconCheckBoxSelected Exception :: " + string(e.what()));
			UTILITY_API->DisplayMessageBox(e.what());
		}
		catch (const char* msg)
		{
			Logger::Error("PLMMaterialResult -> IconCheckBoxSelected Exception :: " + string(msg));
			wstring wstr(msg, msg + strlen(msg));
			UTILITY_API->DisplayMessageBoxW(wstr);
		}
		Logger::Info("PLMMaterialResult -> IconCheckBoxSelected() -> End");
	}

	/*
	* Description - setHeaderToolTip() method used to set header tool tip.
	* Parameter -
	* Exception -
	* Return -
	*/
	void PLMMaterialResult::setHeaderToolTip()
	{
		Logger::Info("PLMMaterialResult -> setHeaderToolTip() -> Start");
		for (int i = 0; i < resultTable->columnCount(); i++)
		{
			QString headerValue = resultTable->horizontalHeaderItem(i)->text();
			resultTable->horizontalHeaderItem(i)->setToolTip(headerValue);
		}
		Logger::Info("PLMMaterialResult -> setHeaderToolTip() -> Start");
	}

	/*
	* Description - onClickedDeselectAllButton() method is a slot for deselect all button click and deselect all the selected.
	* Parameter -
	* Exception -
	* Return -
	*/
	void PLMMaterialResult::onClickedDeselectAllButton()
	{
		Logger::Info("PLMMaterialResult -> onClickedDeselectAllButton() -> Start");
		ClickedDeselectAllButton();
		Logger::Info("PLMMaterialResult -> onClickedDeselectAllButton() -> End");
	}

	void PLMMaterialResult::ClickedDeselectAllButton()
	{
		Logger::Info("PLMMaterialResult -> ClickedDeselectAllButton() -> Start");
		CVWidgetGenerator::DeSelectAllClicked(resultTable, iconTable, m_downloadButton);
		m_rowsSelected.clear();
		m_iconsSelected.clear();
		m_totalSelected.clear();
		Logger::Info("PLMMaterialResult -> ClickedDeselectAllButton() -> End");
	}

	/*
	* Description - onClickedBackButton() method is slot for back button click.
	* Parameter -
	* Exception -
	* Return -
	*/
	void PLMMaterialResult::onClickedBackButton()
	{
		Logger::Info("PLMMaterialResult -> onClickedBackButton() -> Start");
		backButtonClicked();
		Logger::Info("PLMMaterialResult -> onClickedBackButton() -> End");

	}

	void PLMMaterialResult::backButtonClicked()
	{
		Logger::Info("PLMMaterialResult -> backButtonClicked() -> Start");
		this->close();
		CLOVise::PLMMaterialSearch::GetInstance()->setModal(true);
		RESTAPI::SetProgressBarData(0, "", false);
		CLOVise::PLMMaterialSearch::GetInstance()->show();
		//PLMMaterialResult::Destroy();
		
		Logger::Info("PLMMaterialResult -> backButtonClicked() -> End");
		Destroy();
	}

	/*
	* Description - onClickedNextButton() method is a slot for next button click.
	* Parameter -
	* Exception -
	* Return -
	*/
	void PLMMaterialResult::onClickedNextButton()
	{
		Logger::Info("PLMMaterialResult -> onClickedNextButton() -> Start");
		ClickedNextButton();
		Logger::Info("PLMMaterialResult -> onClickedNextButton() -> End");
	}

	void PLMMaterialResult::ClickedNextButton()
	{
		Logger::Info("PLMMaterialResult -> ClickedNextButton() -> Start");
		int currPage = currPageLabel->text().toInt() + 1;
		currPageLabel->setText(QString::fromStdString(to_string(currPage)));
		if (currPage == totalPageLabel->text().toInt())
			nextButton->setEnabled(false);
		if (m_isTabularView)
			CVWidgetGenerator::UpdateTableAndIconRows(resultTable, currPageLabel, m_perPageResultComboBox, m_resultsCount, false);
		else
			CVWidgetGenerator::UpdateTableAndIconRows(iconTable, currPageLabel, m_perPageResultComboBox, m_resultsCount);
		previousButton->setEnabled(true);
		Logger::Info("PLMMaterialResult -> ClickedNextButton() -> End");
	}

	/*
	* Description - onClickedPreviousButton() method is a slot for previous button click.
	* Parameter -
	* Exception -
	* Return -
	*/
	void PLMMaterialResult::onClickedPreviousButton()
	{
		Logger::Info("PLMMaterialResult -> onClickedPreviousButton() -> Start");
		ClickedPreviousButton();
		Logger::Info("PLMMaterialResult -> onClickedPreviousButton() -> End");
	}

	void PLMMaterialResult::ClickedPreviousButton()
	{
		Logger::Info("PLMMaterialResult -> ClickedPreviousButton() -> Start");
		int currPage = currPageLabel->text().toInt() - 1;
		currPageLabel->setText(QString::fromStdString(to_string(currPage)));
		if (currPage == 1)
			previousButton->setEnabled(false);
		if (m_isTabularView)
			CVWidgetGenerator::UpdateTableAndIconRows(resultTable, currPageLabel, m_perPageResultComboBox, m_resultsCount, false);
		else
			CVWidgetGenerator::UpdateTableAndIconRows(iconTable, currPageLabel, m_perPageResultComboBox, m_resultsCount);
		nextButton->setEnabled(true);
		Logger::Info("PLMMaterialResult -> ClickedPreviousButton() -> End");
	}

	/*
	* Description - onResultPerPageCurrentIndexChanged() method is a slot for result per page option and cache the latest selected option.
	* Parameter -  QString.
	* Exception -
	* Return -
	*/
	void PLMMaterialResult::onResultPerPageCurrentIndexChanged(const QString&)
	{
		Logger::Info("PLMMaterialResult -> onResultPerPageCurrentIndexChanged() -> Start");
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
			Logger::Error("PLMMaterialResult::onResultPerPageCurrentIndexChanged() Exception - " + msg);
		}
		catch (exception & e)
		{
			Logger::Error("PLMMaterialResult::onResultPerPageCurrentIndexChanged() Exception - " + string(e.what()));
		}
		catch (const char* msg)
		{
			wstring wstr(msg, msg + strlen(msg));
			Logger::Error("PLMMaterialResult::onResultPerPageCurrentIndexChanged() Exception - " + string(msg));
		}
		Logger::Info("PLMMaterialResult -> onResultPerPageCurrentIndexChanged() -> End");
	}

	/*
	* Description - onResultViewIndexChanged() method is a slot for views changes  and re arranging the column of the table.
	* Parameter -  QString.
	* Exception - exception, Char *
	* Return -
	*/

	void PLMMaterialResult::onResultViewIndexChanged(const QString& _str)
	{
		Logger::Info("PLMMaterialResult -> onResultViewIndexChanged() -> Start");
		ResultViewIndexChanged(_str);
		Logger::Info("PLMMaterialResult -> onResultViewIndexChanged() -> End");
	}

	void PLMMaterialResult::ResultViewIndexChanged(const QString&)
	{
		Logger::Info("PLMMaterialResult -> ResultViewIndexChanged() -> Start");
		try
		{
			UTILITY_API->DeleteProgressBar(true);
			this->hide();
			UTILITY_API->CreateProgressBar();
			UTILITY_API->SetProgress("Loading", (qrand() % 101));
			Logger::Info("PLMMaterialResult -> onResultViewIndexChanged() -> Start");
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
			CVWidgetGenerator::DrawViewAndResultsWidget(m_viewComboBox, resultTable, iconTable, false, m_materialResults, MaterialConfig::GetInstance()->GetMaterialViewJSON(), IMAGE_DISPLAY_NAME, m_typename, false, m_viewComboBox->currentIndex(), MaterialConfig::GetInstance()->GetAttScopes(), false, false, MaterialConfig::GetInstance()->GetMaterialViewJSON());
			onTabViewClicked();
			setHeaderToolTip();
			if (MaterialConfig::GetInstance()->GetIsRadioButton()) {
				AddConnectorForRadioButton();
			}
			else {
				AddConnectorForCheckbox();
			}
			resultTable->setEnabled(true);
			RESTAPI::SetProgressBarData(0, "", false);
			this->show();
		}
		catch (string msg)
		{
			Logger::Error("PLMMaterialResult -> onResultViewIndexChanged Exception :: " + msg);
			UTILITY_API->DisplayMessageBox(msg);
			RESTAPI::SetProgressBarData(0, "", false);
			this->close();
		}
		catch (exception& e)
		{
			Logger::Error("PLMMaterialResult -> onResultViewIndexChanged Exception :: " + string(e.what()));
			UTILITY_API->DisplayMessageBox(e.what());
			RESTAPI::SetProgressBarData(0, "", false);
			this->close();
		}
		catch (const char* msg)
		{
			Logger::Error("PLMMaterialResult -> onResultViewIndexChanged Exception :: " + string(msg));
			wstring wstr(msg, msg + strlen(msg));
			UTILITY_API->DisplayMessageBoxW(wstr);
			RESTAPI::SetProgressBarData(0, "", false);
			this->close();
		}
		Logger::Info("PLMMaterialResult -> onResultViewIndexChanged() -> End");
	}

	/*
	* Description - iconViewClicked() method is a slot for icon click and updated the view in the form of icon.
	* Parameter -
	* Exception -
	* Return -
	*/
	void PLMMaterialResult::onIconViewClicked()
	{
		Logger::Info("PLMMaterialResult -> onIconViewClicked() -> Start");
		updateTableForIconView();
		Logger::Info("PLMMaterialResult -> onIconViewClicked() -> End");
	}

	void PLMMaterialResult::updateTableForIconView()
	{
		Logger::Info("PLMMaterialResult -> updateTableForIconView() -> Start");
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
				CVWidgetGenerator::SortIconResultTable(m_isResultTableSorted, iconTable, resultTable, m_materialResults, false);
				iconTable->show();
				ui_resultTableLayout->addWidget(iconTable);
				CVWidgetGenerator::UpdateTableAndIconRows(iconTable, currPageLabel, m_perPageResultComboBox, m_resultsCount);
				CVWidgetGenerator::SetDownloadCount(m_iconsSelected, m_rowsSelected, m_downloadButton); iconTable->show();
				if (MaterialConfig::GetInstance()->GetIsRadioButton()) {
					AddConnectorForRadioButton();
				}
				else {
					AddConnectorForCheckbox();
				}
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
			Logger::Error("PLMMaterialResult -> updateTableForIconView() Exception - " + msg);
		}
		catch (exception & e)
		{
			UTILITY_API->DeleteProgressBar(true);
			Logger::Error("PLMMaterialResult -> updateTableForIconView() Exception - " + string(e.what()));
		}
		catch (const char* msg)
		{
			UTILITY_API->DeleteProgressBar(true);
			wstring wstr(msg, msg + strlen(msg));
			Logger::Error("PLMMaterialResult -> updateTableForIconView() Exception - " + string(msg));
		}
		Logger::Info("PLMMaterialResult -> updateTableForIconView() -> End");
	}

	/*
	* Description - tabViewClicked() method is slot for the table view click and updated the view in the form of tabular.
	* Parameter -
	* Exception -
	* Return -
	*/
	void PLMMaterialResult::onTabViewClicked()
	{
		Logger::Info("PLMMaterialResult -> onTabViewClicked() -> Start");
		updateTableForTabView();
		Logger::Info("PLMMaterialResult -> onTabViewClicked() -> End");
	}

	void PLMMaterialResult::updateTableForTabView()
	{
		Logger::Info("PLMMaterialResult -> updateTableForTabView() -> Start");
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
			Logger::Error("PLMMaterialResult->updateTableForTabView() Exception - " + msg);
		}
		catch (exception & e)
		{
			Logger::Error("PLMMaterialResult->updateTableForTabView() Exception - " + string(e.what()));
		}
		catch (const char* msg)
		{
			wstring wstr(msg, msg + strlen(msg));
			Logger::Error("PLMMaterialResult->updateTableForTabView() Exception - " + string(msg));
		}
		Logger::Info("PLMMaterialResult -> updateTableForTabView() -> End");
	}

	/*
	* Description - onHorizontalHeaderClicked() method is a slot for horizontal header click and sorting the columns.
	* Parameter -  int.
	* Exception -
	* Return -
	*/
	void PLMMaterialResult::onHorizontalHeaderClicked(int _column)
	{
		Logger::Info("PLMMaterialResult -> onHorizontalHeaderClicked() -> Start");
		horizontalHeaderClicked(_column);
		Logger::Info("PLMMaterialResult -> onHorizontalHeaderClicked() -> End");
	}

	void PLMMaterialResult::horizontalHeaderClicked(int _column)
	{
		Logger::Info("PLMMaterialResult -> horizontalHeaderClicked() -> Start");
		Logger::Debug("Column.." + to_string(_column));
		if (_column == CHECKBOX_COLUMN || _column == IMAGE_COLUMN)
			resultTable->setSortingEnabled(false);
		else
		{
			resultTable->setSortingEnabled(true);
			m_isResultTableSorted = true;
			if (MaterialConfig::GetInstance()->GetIsRadioButton()) {
				AddConnectorForRadioButton();
			}
			else {
				AddConnectorForCheckbox();
			}
			CVWidgetGenerator::UpdateTableAndIconRows(resultTable, currPageLabel, m_perPageResultComboBox, m_resultsCount, true);
		}
		Logger::Info("PLMMaterialResult -> horizontalHeaderClicked() -> End");
	}

	/*
	* Description - connectSignalSlots() method used connect and disconnect the slot.
	* Parameter -  bool.
	* Exception -
	* Return -
	*/
	void PLMMaterialResult::connectSignalSlots(bool _b)
	{
		Logger::Info("PLMMaterialResult -> connectSignalSlots() -> Start");
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
			QObject::disconnect(resultTable->horizontalHeader(), SIGNAL(sectionClicked(int)), this, SLOT(onHorizontalHeaderClicked(int)));
		}
		Logger::Info("PLMMaterialResult -> connectSignalSlots() -> End");
	}

	/*
	* Description - DownloadClicked() method is slot for download button.
	* Parameter -
	* Exception - exception, Char *
	* Return -
	*/
	void PLMMaterialResult::onDownloadClicked()
	{
		Logger::Info("PLMMaterialResult -> onDownloadClicked() -> Start");
		DownloadMaterialResult();
		Logger::Info("PLMMaterialResult -> onDownloadClicked() -> End");
	}

	void PLMMaterialResult::DownloadMaterialResult()
	{
		Logger::Info("PLMMaterialResult -> downloadColorResult() -> Start");
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
			UTILITY_API->CreateProgressBar();
			//UTILITY_API->SetProgress("Downloading Materials", 30);
			RESTAPI::SetProgressBarData(15, "Downloading "+ Configuration::GetInstance()->GetLocalizedMaterialClassName(), true);
			UTILITY_API->SetProgress("Downloading "+ Configuration::GetInstance()->GetLocalizedMaterialClassName(), (qrand() % 101));
			//RESTAPI::SetProgressBarData(14, "Downloading Materials", true);
			//json downloadJson = UIHelper::DownloadResults(m_totalSelected, MATERIAL_MODULE, m_materialResults);
			QString totalSelected = m_totalSelected.join(", ");

			QString faildesObjectsString;
				    faildesObjectsString = UIHelper::DownloadResults(m_totalSelected, MATERIAL_MODULE, m_materialResults);
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
				if (Configuration::GetInstance()->GetCurrentScreen() == UPDATE_MATERIAL_CLICKED)
				{
					this->close();
					CLOVise::PLMMaterialSearch::GetInstance()->setModal(true);
					CLOVise::PLMMaterialSearch::GetInstance()->show();
				}
			}
			else
			{	
				CVDisplayMessageBox DownloadDialogObject;
				DownloadDialogObject.DisplyMessage("Download Completed.");
				DownloadDialogObject.setModal(true);
				DownloadDialogObject.exec();
				if (Configuration::GetInstance()->GetCurrentScreen() == UPDATE_MATERIAL_CLICKED) 
				{
					RESTAPI::SetProgressBarData(20, "Loading Update Material..", true);
					UTILITY_API->SetProgress("Loading Update Material..", (qrand() % 101));
					MaterialConfig::GetInstance()->InitializeMaterialData();
					UpdateMaterial::GetInstance()->setModal(true);
					UpdateMaterial::GetInstance()->exec();
				}
				if (Configuration::GetInstance()->GetCloseResultsDialogue())
				{
					this->close();
					//PLMMaterialSearch::Destroy();
					UIHelper::ClearAllFieldsForSearch(PLMMaterialSearch::GetInstance()->GetTreewidget(0));
					UIHelper::ClearAllFieldsForSearch(PLMMaterialSearch::GetInstance()->GetTreewidget(1));
					this->Destroy();
				}
				else
				{
					if (!MaterialConfig::GetInstance()->GetIsRadioButton())
					onClickedDeselectAllButton();
					this->show();
				}
			}
		}
		catch (string msg)
		{
			Logger::Error("PLMMaterialResult -> DownloadClicked Exception :: " + msg);
			UTILITY_API->DisplayMessageBox(msg);
			UTILITY_API->DeleteProgressBar(true);
			if (Configuration::GetInstance()->GetCloseResultsDialogue() && rowsSelected)
			{
				this->close();
				UIHelper::ClearAllFieldsForSearch(PLMMaterialSearch::GetInstance()->GetTreewidget(0));
				UIHelper::ClearAllFieldsForSearch(PLMMaterialSearch::GetInstance()->GetTreewidget(1));
				this->Destroy();
			}
			else
			{
				if (!MaterialConfig::GetInstance()->GetIsRadioButton())
				onClickedDeselectAllButton();
				this->show();
			}
		}
		catch (exception& e)
		{
			Logger::Error("PLMMaterialResult -> DownloadClicked Exception :: " + string(e.what()));
			UTILITY_API->DisplayMessageBox(e.what());
			UTILITY_API->DeleteProgressBar(true);
			if (Configuration::GetInstance()->GetCloseResultsDialogue() && rowsSelected)
			{
				this->close();
				UIHelper::ClearAllFieldsForSearch(PLMMaterialSearch::GetInstance()->GetTreewidget(0));
				UIHelper::ClearAllFieldsForSearch(PLMMaterialSearch::GetInstance()->GetTreewidget(1));
				this->Destroy();
			}
			else
			{
				if (!MaterialConfig::GetInstance()->GetIsRadioButton())
				onClickedDeselectAllButton();
				this->show();
			}
		}
		catch (const char* msg)
		{
			Logger::Error("PLMMaterialResult -> DownloadClicked Exception :: " + string(msg));
			wstring wstr(msg, msg + strlen(msg));
			UTILITY_API->DisplayMessageBoxW(wstr);
			UTILITY_API->DeleteProgressBar(true);
			if (Configuration::GetInstance()->GetCloseResultsDialogue() && rowsSelected)
			{
				this->close();
				UIHelper::ClearAllFieldsForSearch(PLMMaterialSearch::GetInstance()->GetTreewidget(0));
				UIHelper::ClearAllFieldsForSearch(PLMMaterialSearch::GetInstance()->GetTreewidget(1));
				this->Destroy();
			}
			else
			{
				//onClickedDeselectAllButton();
				this->show();
			}
		}
		Logger::Info("PLMMaterialResult -> downloadColorResult() -> End");
	}
}
