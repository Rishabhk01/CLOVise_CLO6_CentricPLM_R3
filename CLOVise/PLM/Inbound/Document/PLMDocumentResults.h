#pragma once
/*
* Copyright 2020-2021 CLO-Vise. All rights reserved
*
* @file PLMDocumentResults.h
*
* brief Class declaration for cach Document data in CLO from PLM.
* This class has all the variable declarations and function declarations which are used in storing PLM Document instance data to view and download in CLO.
*
* @author GoVise
*
* @date 24-JUL-2020
*/
#include "ui_PLMDocumentResults.h"

#include <iostream>
#include <string>

#include <qtablewidget.h>
#include <qlistwidget.h>

#include "classes/widgets/MVDialog.h"
#include "classes/widgets/MVTableWidget.h"
#include "CLOVise/PLM/Libraries/json.h"

using json = nlohmann::json;
using namespace std;

namespace CLOVise
{
	class  PLMDocumentResults : public MVDialog, public Ui::DocumentResultTable
	{
		Q_OBJECT

	public:
		static PLMDocumentResults* GetInstance();
		static void Destroy();
		void setDataFromResponse(json _resultJson);
		
	private:		
		PLMDocumentResults(QWidget* parent = nullptr);
		virtual ~PLMDocumentResults();

		
		void setHeaderToolTip();
		QString downloadResults(QStringList _downloadIdList, string _module);

		void connectSignalSlots(bool _b) override;

		static PLMDocumentResults* _instance;
		bool m_isHidden = false;
		MVTableWidget* resultTable;
		QListWidget* iconTable;

		int m_resultsCount = 0;
		int m_maxResultsCount = 0;
		bool m_isTabularView = true;
		json m_documentResults = json::object();
		string m_typename = "";
		void AddConnectorForRadioButton();
		bool m_isResultTableSorted = false;

		QStringList m_iconsSelected;
		QStringList m_rowsSelected;
		QStringList m_totalSelected;
		json m_downloadJSON;
		json m_downloadedResultsJSON;
		json m_metadataJSONList;
		json m_downloadResultJSON;

	private slots:
		void onClickedDeselectAllButton();
		void onClickedBackButton();
		void onClickedNextButton();
		void onClickedPreviousButton();
		void onResultPerPageCurrentIndexChanged(const QString&);
		void onResultViewIndexChanged(const QString&);
		void onIconViewClicked();
		void onTabViewClicked();
		void onTableRadioButtonSelected();
		//void onIconRadioButtonSelected();
		void onDownloadClicked();
		void onHorizontalHeaderClicked(int _column);

	};
}