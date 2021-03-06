#pragma once
/*
* Copyright 2020-2021 CLO-Vise. All rights reserved
*
* @file PLMColorSearch.h
*
* @brief Class declaration for accessing Colors in CLO from PLM.
* This class has all the variable declarations and function declarations which are used in PLM Color Search.
*
* @author GoVise
*
* @date 05-OCT-2021
*/
#include "ui_PLMPrintSearch.h"

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
	class  PLMPrintSearch : public MVDialog, public Ui::PrintSearch
	{
		Q_OBJECT

	public:
		int m_selectedViewIdx;

		static PLMPrintSearch* GetInstance();
		void HideHirarchy(bool _hide);
		void ClickedSubmitButton();
		void ClickedBackButton();
		void FilterChanged(const QString& _item);
		void SeasonChanged(const QString& _item);
		void CheckBoxChecked(bool _checked);
		void TreeNodeClicked(QTreeWidgetItem* _item, int _column);
		void ResetDateEditWidget();
		void DrawSearchWidget(bool _isFromConstructor);
		QTreeWidget* GetTreewidget(int _index);

	private:
		
		PLMPrintSearch(QWidget* parent = nullptr);
		virtual ~PLMPrintSearch();

		void collectSearchUIFields();
		
		void drawSearchUI(QString _selectType, bool _drawFilter, string _selectedFilter);

		void connectSignalSlots(bool _b) override;

		void reject();

		static PLMPrintSearch* _instance;
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
		QComboBox* m_paletteComboBox;
		QComboBox* m_filterComboBox;
		QCheckBox* m_paletteCheckBox;
		QLabel* m_seasonLabel;
		QLabel* m_paletteLabel;
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
	};
}