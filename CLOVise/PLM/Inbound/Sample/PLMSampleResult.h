#pragma once
/*
* Copyright 2020-2021 CLO-Vise. All rights reserved
*
* @file PLMSampleResult.h
*
* @brief Class declaration for cach Sample data in CLO from PLM.
* This class has all the variable declarations and function declarations which are used in storing PLM Sample instance data to view and download in CLO.
*
* @author GoVise
*
* @date 30-MAY-2020
*/
#include "ui_PLMSampleResult.h"

#include <iostream>
#include <string>

#include <qtablewidget.h>
#include <qlistwidget.h>
#include <QtWidgets/QComboBox>

#include "classes/widgets/MVTableWidget.h"
#include "classes/widgets/MVDialog.h"
#include "CLOVise/PLM/Libraries/json.h"

using json = nlohmann::json;
using namespace std;

namespace CLOVise
{
	class  PLMSampleResult : public MVDialog, public Ui::SampleResultTable
	{
		Q_OBJECT

	public:
		static PLMSampleResult* GetInstance();
		static void Destroy();
		void setDataFromResponse(json _resultJson);
		void updateTableForTabView();
		void updateTableForIconView();
		void downloadSampleResult();
		void backButtonClicked();
		void horizontalHeaderClicked(int _column);
		void ResultViewIndexChanged(const QString&);

		void ClickedNextButton();
		void ClickedPreviousButton();
		void TableRadioButtonSelected();
		//void IconRadioButtonSelected();
	private:

		PLMSampleResult(QWidget* parent = nullptr);
		virtual ~PLMSampleResult();


		void setHeaderToolTip();
		QString downloadSampleMetadata(json _resultsJson, QStringList _selectedObjectIds);

		void connectSignalSlots(bool _b) override;

		static PLMSampleResult* _instance;
		bool m_isHidden = false;
		MVTableWidget* resultTable;
		QListWidget* iconTable;

		int m_resultsCount = 0;
		bool m_isTabularView = true;
		json m_sampleResults = json::object();
		string m_typename = "";
		void addConnectorForRadioButton();
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
		int m_maxResultsCount = 0;

	private slots:
		void onClickedBackButton();
		void onClickedNextButton();
		void onClickedPreviousButton();
		void onResultPerPageCurrentIndexChanged(const QString&);
		void onResultViewIndexChanged(const QString&);
		void onIconViewClicked();
		void onTabViewClicked();
		void onTableRadioButtonSelected();
		void onDownloadClicked();
		void onHorizontalHeaderClicked(int _column);
		//void onIconRadioButtonSelected();

	};
}