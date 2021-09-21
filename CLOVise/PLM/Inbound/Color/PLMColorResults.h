#pragma once
/*
* Copyright 2020-2021 CLO-Vise. All rights reserved
*
* @file PLMColorResults.h
*
* @brief Class declaration for cach Colors data in CLO from PLM.
* This class has all the variable declarations and function declarations which are used in storing PLM Color instance data to view and download in CLO.
*
* @author GoVise
*
* @date 30-MAY-2020
*/
#include "ui_PLMColorResults.h"

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
	class  PLMColorResults : public MVDialog, public Ui::ColorResultTable
	{
		Q_OBJECT

	public:
		static PLMColorResults* GetInstance();
		static void Destroy();
		void setDataFromResponse(json _resultJson);
		void updateTableForTabView();
		void updateTableForIconView();
		void downloadColorResult();
		void backButtonClicked();
		void horizontalHeaderClicked(int _column);
		void ResultViewIndexChanged(const QString&);

		void ClickedDeselectAllButton();
		void ClickedSelectAllButton();
		void ClickedNextButton();
		void ClickedPreviousButton();
		void TableCheckBoxSelected();
		//void IconCheckBoxSelected();
		//json DownloadResults(QStringList _downloadIdList, string _module, json _previewjsonarray);

	private:
		 
		PLMColorResults(QWidget* parent = nullptr);
		virtual ~PLMColorResults();
		
		void setHeaderToolTip();

		void connectSignalSlots(bool _b) override;


		static PLMColorResults* _instance;
		bool m_isHidden = false;
		MVTableWidget* resultTable;
		QListWidget* iconTable;

		int m_resultsCount = 0;
		bool m_isTabularView = true;
		json m_colorResults = json::object();
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