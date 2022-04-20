#pragma once
/*
* Copyright 2020-2021 CLO-Vise. All rights reserved
*
* @file PLMShapeResults.h
*
* @brief Class declaration for cach Shape data in CLO from PLM.
* This class has all the variable declarations and function declarations which are used in storing PLM Shape instance data to view and download in CLO.
*
* @author GoVise
*
* @date 28-JUL-2020
*/
#include "ui_PLMShapeResults.h"

#include <iostream>
#include <string>

#include <qtablewidget.h>
#include <qlistwidget.h>
#include <QtGui>
#include <qtreewidget.h>
#include <QtCore>
#include <QtWidgets>

#include "classes/widgets/MVDialog.h"
#include "classes/widgets/MVTableWidget.h"
#include "CLOVise/PLM/Libraries/json.h"

using json = nlohmann::json;
using namespace std;

namespace CLOVise
{
	class  PLMShapeResults : public MVDialog, public Ui::ShapeResultTable
	{
		Q_OBJECT

	public:
		static PLMShapeResults* GetInstance();
		//void setDataFromResponse(json _resultJson);

		void DownloadClicked();
		void HorizontalHeaderClicked(int _column);
		void IconViewClicked();
		void TabViewClicked();
		void ResultViewIndexChanged(const QString&);
		void ResultPerPageCurrentIndexChanged(const QString&);
		//void ClickedNextButton();
		void ClickedBackButton();
		void ClickedPreviousButton();
		void TableRadioButtonSelected();
		//void IconRadioButtonSelected();
		void DrawResultWidget(bool _isFromConstructor);

		QPushButton *m_downloadButton;
	private:


		PLMShapeResults(QWidget* parent = nullptr);
		virtual ~PLMShapeResults();

		void setHeaderToolTip();
		QString downloadShapeMetadata(json _resultsJson, QStringList _selectedObjectIds);
		void AddConnectorForRadioButton();
		void connectSignalSlots(bool _b) override;
		void hidebuttonClicked(bool _hide);
		void drawAddtionalResultTable(QStringList _selectedIdList);

		static PLMShapeResults* _instance;
		bool m_isHidden = false;
		MVTableWidget* resultTable;
		QListWidget* iconTable;
		QListWidget* aditionalResultWidget;

		int m_resultsCount = 0;
		int m_maxResultsCount = 0;
		bool m_isTabularView = true;
		json m_shapeResults = json::object();
		string m_typename = "Shape";
		string m_perveiouselySelectedId = "";
		string m_selectedIdnameText = "";
		string m_tempId = "";

		QStringList m_iconsSelected;
		QStringList m_rowsSelected;
		QStringList m_totalSelected;
		bool m_isResultTableSorted = false;

		QComboBox *m_viewComboBox;
		QComboBox *m_perPageResultComboBox;
		QLabel *m_noOfResultLabel;
		QToolButton *m_tabViewButton;
		QToolButton *m_iconViewButton;
		QPushButton *m_backButton;

		//QPushButton *m_deSelectAllButton;

	private slots:
		//void onClickedDeselectAllButton();
		void onClickedBackButton();
		//void onClickedNextButton();
		void onClickedPreviousButton();
		void onResultPerPageCurrentIndexChanged(const QString&);
		void onResultViewIndexChanged(const QString&);
		void onIconViewClicked();
		void onTabViewClicked();
		void onTableRadioButtonSelected();
		//void onIconRadioButtonSelected();
		void onDownloadClicked();
		void onHorizontalHeaderClicked(int _column);
		void onHideButtonClicked(bool _hide);
		void onListItemClicked(QListWidgetItem* _listWiget);

	};
}