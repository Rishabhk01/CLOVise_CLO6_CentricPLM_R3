#pragma once
/*
* Copyright 2020-2021 CLO-Vise. All rights reserved
*
* @file PLMMaterialResult.h
*
* @brief Class declaration for cach Material data in CLO from PLM.
* This class has all the variable declarations and function declarations which are used in storing PLM Material instance data to view and download in CLO.
*
* @author GoVise
*
* @date 16-JUN-2020
*/
#include "ui_PLMMaterialResult.h"

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
	class  PLMMaterialResult : public MVDialog, public Ui::MaterialResultTable
	{
		Q_OBJECT

	public:
		static PLMMaterialResult* GetInstance();
		static void Destroy();
		//void setDataFromResponse(json _resultJson);
		void updateTableForTabView();
		void updateTableForIconView();
		void DownloadMaterialResult();
		void backButtonClicked();
		void horizontalHeaderClicked(int _column);
		void ResultViewIndexChanged(const QString&);

		void ClickedDeselectAllButton();
		void ClickedNextButton();
		void ClickedPreviousButton();
		void TableCheckBoxSelected();
		void IconCheckBoxSelected();
		void TableRadioButtonSelected();
		
		json m_materialResults = json::array();
		string m_typename = "";
		int m_maxResultsCount = 0;
		int m_resultsCount = 0;
		bool m_isUpdateMaterialClicked = false;

	private:

		PLMMaterialResult(QWidget* parent = nullptr);
		virtual ~PLMMaterialResult();


		void setHeaderToolTip();

		void connectSignalSlots(bool _b) override;

		static PLMMaterialResult* _instance;
		bool m_isHidden = false;
		MVTableWidget* resultTable;
		QListWidget* iconTable;

		//int m_resultsCount = 0;
		bool m_isTabularView = true;
		//json m_materialResults = json::array();
		//string m_typename = "";
		void AddConnectorForCheckbox();
		void AddConnectorForRadioButton();
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
		//int m_maxResultsCount = 0;

	private slots:
		void onClickedDeselectAllButton();
		void onClickedBackButton();
		void onClickedNextButton();
		void onClickedPreviousButton();
		void onResultPerPageCurrentIndexChanged(const QString&);
		void onResultViewIndexChanged(const QString&);
		void onIconViewClicked();
		void onTabViewClicked();
		void onTableCheckBoxSelected();
		void onIconCheckBoxSelected();
		void onTableRadioButtonSelected();
		void onIconRadioButtonSelected();
		void onDownloadClicked();
		void onHorizontalHeaderClicked(int _column);
	};
}