#pragma once
/*
* Copyright 2020-2021 CLO-Vise. All rights reserved
*
* @file PLMColorSearch.h
*
* @brief Class declaration for accessing Sample in CLO from PLM.
* This class has all the variable declarations and function declarations which are used in PLM Color Search.
*
* @author GoVise
*
* @date 27-MAY-2020
*/
#include "ui_PLMSampleSearch.h"

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

namespace CLOVise
{
	class  PLMSampleSearch : public MVDialog, public Ui::SampleSearch
	{
		Q_OBJECT

	public:
		int m_selectedViewIdx;

		static PLMSampleSearch* GetInstance();
		static void Destroy();
		void HideHirarchy(bool _hide);
		void ClickedSubmitButton();
		void ClickedBackButton();
		void FilterChanged(const QString& _item);
		void CheckBoxChecked(bool _checked);
		void TreeNodeClicked(QTreeWidgetItem* _item, int _column);
		void ResetDateEditWidget();

	private:

		PLMSampleSearch(QWidget* parent = nullptr);
		virtual ~PLMSampleSearch();

		void collectSearchUIFields();

		void drawSearchUI(QString _selectType, bool _drawFilter, string _selectedFilter);

		void connectSignalSlots(bool _b) override;

		static PLMSampleSearch* _instance;
		bool m_isHidden = false;
		set<string> m_flextypeTree;
		QString selectType;
		json m_searchCriteriaJson = json::object();
		bool m_hierarchyLoading;

		QTreeWidget* m_hierarchyTreeWidget;
		QTreeWidget* m_searchTreeWidget;
		QComboBox* m_viewComboBox;
		QComboBox* m_filterComboBox;
		QLineEdit* m_productNameLineEdit;
		QLineEdit* m_productIdLineEdit;
		QCheckBox* m_productCheckBox;
		QLabel* m_productNameLabel;
		QLabel* m_productIdLabel;
		QLineEdit* m_quickSearchLineEdit;
		QToolButton* m_dateResetButton;
		QToolButton* m_dateResetButton_1;
		QPushButton* m_backButton;
		QPushButton* m_searchButton;

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