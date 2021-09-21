#pragma once
/*
* Copyright 2020-2021 CLO-Vise. All rights reserved
*
* @file PLMDocumentSearch.h
*
* @brief Class declaration for accessing Documents in CLO from PLM.
* This class has all the variable declarations and function declarations which are used in PLM Document Search.
*
* @author GoVise
*
* @date 24-JUL-2020
*/
#include "ui_PLMDocumentSearch.h"

#include <iostream>
#include <string>
#include <set>

#include "CLOVise/PLM/Helper/Util/Helper.h"
#include "classes/widgets/MVDialog.h"
#include "CLOAPIInterface.h"

namespace CLOVise
{
	class  PLMDocumentSearch : public MVDialog, public Ui::DocumentSearch
	{
		Q_OBJECT
	
	public:
		int m_selectedViewIdx;

		static PLMDocumentSearch* GetInstance();
		static void Destroy();	

	private:
		PLMDocumentSearch(QWidget* parent = nullptr);
		virtual ~PLMDocumentSearch();

		void collectSearchUIFields();
		
		void drawSearcUI(QString _selectType, bool _drawFilter, string _selectedFilter);

		void connectSignalSlots(bool _b) override;

		static PLMDocumentSearch* _instance;
		bool m_isHidden = false;
		set<string> m_flextypeTree;
		QString selectType;
		json m_searchCriteriaJson = json::object();
		bool m_hierarchyLoading;

	private slots:
		void onHideHirarchy(bool _hide);
		void onClickedSubmitButton();
		void onClickedBackButton();
		void onFilterChanged(const QString& _item);
		void onCheckBoxChecked(bool _checked);
		void onTreeNodeClicked(QTreeWidgetItem* _item, int _column);
		void onResetDateEditWidget();

	};
}