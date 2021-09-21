#pragma once
/*
* Copyright 2020-2021 CLO-Vise. All rights reserved
*
* @file PLMProductSearch.h
*
* @brief Class declaration for accessing Product in CLO from PLM.
* This class has all the variable declarations and function declarations which are used in PLM Product Search.
*
* @author GoVise
*
* @date 28-JUL-2020
*/
#include "ui_PLMProductSearch.h"

#include <iostream>
#include <string>
#include <set>

#include <QtGui>
#include <qtreewidget.h>
#include <QtCore>
#include <QtWidgets>

#include "CLOVise/PLM/Helper/Util/Helper.h"
#include "classes/widgets/MVDialog.h"
#include "CLOAPIInterface.h"
#include "CLOVise/PLM/Helper/Util/ComboBoxItem.h"

namespace CLOVise
{
	class  PLMProductSearch : public MVDialog, public Ui::ProductSearch
	{
		Q_OBJECT

	public:
		int m_selectedViewIdx;

		static PLMProductSearch* GetInstance();
		void HideHirarchy(bool _hide);
		void ClickedSubmitButton();
		void ClickedBackButton();
		void FilterChanged(const QString& _item);
		void SeasonChanged(const QString& _item);
		void CheckBoxChecked(bool _checked);
		void TreeNodeClicked(QTreeWidgetItem* _item, int _column);
		void ResetDateEditWidget();
		void ClearAllFields();
		QTreeWidget* GetTreewidget(int _index);

	private:
		
		PLMProductSearch(QWidget* parent = nullptr);
		virtual ~PLMProductSearch();

		void collectSearchUIFields();
		
		void drawSearchUI(QString _selectType, bool _drawFilter, string _selectedFilter);
		void seasonChanged(string _selectedSeason, QComboBox* _paletteComboBox, string _selectedtype, QTreeWidget* _hirarchyTree);

		void connectSignalSlots(bool _b) override;

		void reject();

		static PLMProductSearch* _instance;
		bool m_isHidden = false;

		set<string> m_flextypeTree;
		QString selectType;
		json m_searchCriteriaJson = json::object();
		bool m_hierarchyLoading;

		QTreeWidget* m_hierarchyTreeWidget;
		QTreeWidget* m_searchTreeWidget_1;
		QTreeWidget* m_searchTreeWidget_2;
		QComboBox* m_viewComboBox;
		ComboBoxItem* m_seasoncomboBox;
		QComboBox* m_filterComboBox;
		QCheckBox* m_paletteCheckBox;
		QLabel* m_seasonLabel;
		QLineEdit* m_quickSearchLineEdit;
		QToolButton* m_dateResetButton;
		QToolButton* m_dateResetButton_1;
		QPushButton* m_backButton;
		QPushButton* m_unselectAllButton;
		QPushButton* m_searchButton;

	private slots:
		void onHideHirarchy(bool _hide);
		void onClickedSubmitButton();
		void onClickedBackButton();
		void onFilterChanged(const QString& _item);
		void onSeasonChanged(const QString& _item);
		void onCheckBoxChecked(bool _checked);
		void onTreeNodeClicked(QTreeWidgetItem* _item, int _column);
		void onResetDateEditWidget();
		void onSeasonSelected(const QString&);

	};
}