#pragma once
/*
* Copyright 2020-2021 CLO-Vise. All rights reserved
*
* @file PLMPrintResults.h
*
* @brief Class declaration for cach Prints data in CLO from PLM.
* This class has all the variable declarations and function declarations which are used in storing PLM Print instance data to view and download in CLO.
*
* @author GoVise
*
* @date 05-OCT-2021
*/
#include "ui_PLMPrintResults.h"

#include <iostream>
#include <string>

#include <qtablewidget.h>
#include <qlistwidget.h>

#include "classes/widgets/MVTableWidget.h"
#include "classes/widgets/MVDialog.h"
#include "CLOVise/PLM/Libraries/json.h"
#include <QtWidgets/QComboBox>
using json = nlohmann::json;
using namespace std;

namespace CLOVise
{
	class  PLMPrintResults : public MVDialog, public Ui::PrintResultTable
	{
		Q_OBJECT

	public:
		static PLMPrintResults* GetInstance();
		void setDataFromResponse(json _resultJson);
		void updateTableForTabView();
		void updateTableForIconView();
		void downloadPrintResult();
		void backButtonClicked();
		void horizontalHeaderClicked(int _column);
		void ResultViewIndexChanged(const QString&);

		void ClickedDeselectAllButton();
		void ClickedSelectAllButton();
		void ClickedNextButton();
		void ClickedPreviousButton();
		void TableCheckBoxSelected();
		void DrawResultWidget(bool _isFromConstructor);
		//void IconCheckBoxSelected();
		//json DownloadResults(QStringList _downloadIdList, string _module, json _previewjsonarray);

	private:
		 
		PLMPrintResults(QWidget* parent = nullptr);
		virtual ~PLMPrintResults();
		
		void setHeaderToolTip();

		void connectSignalSlots(bool _b) override;

		void reject();

		static PLMPrintResults* _instance;
		bool m_isHidden = false;
		MVTableWidget* resultTable;
		QListWidget* iconTable;

		int m_resultsCount = 0;
		bool m_isTabularView = true;
		json m_printResults = json::object();
		string m_typename = "";
		void AddConnectorForCheckbox();
		bool m_isResultTableSorted = false;

		QStringList m_iconsSelected;
		QStringList m_rowsSelected;
		QStringList m_totalSelected;
		QComboBox *m_viewComboBox;
		QComboBox *m_perPageResultComboBox;
		QLabel *m_noOfResultLabel;

		QToolButton *m_tabViewButton;
		QToolButton *m_iconViewButton;
		QPushButton *m_backButton;
		QPushButton *m_downloadButton;
		QPushButton *m_deSelectAllButton;
		QPushButton *m_selectAllButton;
		int m_maxResultsCount = 0;
	//	json m_widgetListJson = json::object();

	private slots:
		void onClickedDeselectAllButton();
		void onClickedSelectAllButton();
		void onClickedBackButton();
		void onClickedNextButton();
		void onClickedPreviousButton();
		void onResultPerPageCurrentIndexChanged(const QString&);
		void onResultViewIndexChanged(const QString&);
		void onIconViewClicked();
		void onTabViewClicked();
		void onTableCheckBoxSelected();
		//void onIconCheckBoxSelected();
		void onDownloadClicked();
		void onHorizontalHeaderClicked(int column);
		void TableRadioButtonSelected();

	};
}