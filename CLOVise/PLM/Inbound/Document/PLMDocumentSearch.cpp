/*
* Copyright 2020-2021 CLO-Vise. All rights reserved
*
* @file PLMDocumentSearch.cpp
*
* @brief Class implementation for accessing Documents in CLO from PLM.
* This class has all the variable and methods implementation which are used in PLM Document Search.
*
* @author GoVise
*
* @date 24-JUL-2020
*/
#include "PLMDocumentSearch.h"

#include <iostream>
#include <string>

#include <qglobal.h>

#include "CLOVise/PLM/CLOViseSuite/CLOViseSuite.h"
#include "CLOVise/PLM/Inbound/Document/DocumentConfig.h"
#include "CLOVise/PLM/Inbound/Document/PLMDocumentResults.h"
#include "CLOVise/PLM/Helper/Util/Helper.h"
#include "CLOVise/PLM/Helper/Util/RestAPIUtility.h"
#include "CLOVise/PLM/Helper/Util/FormatHelper.h"
#include "CLOVise/PLM/Helper/Util/FlexTypeHelper.h"
#include "CLOVise/PLM/Helper/Util/Logger.h"
#include "CLOVise/PLM/Helper/UIHelper/CVWidgetGenerator.h"
#include "CLOVise/PLM/Helper/UIHelper/UIHelper.h"
#include"classes/widgets/MVDialog.h"

using namespace std;

namespace CLOVise
{
	PLMDocumentSearch* PLMDocumentSearch::_instance = NULL;
	
	PLMDocumentSearch* PLMDocumentSearch::GetInstance()
	{
		if (_instance == NULL)
			_instance = new PLMDocumentSearch();
		return _instance;
	}

	void PLMDocumentSearch::Destroy()
	{
		if (_instance)
		{
			delete _instance;
			_instance = NULL;
		}
	}

	PLMDocumentSearch::PLMDocumentSearch(QWidget* parent)
		: MVDialog(parent)
	{
		Logger::Info("PLMDocumentSearch -> Constructor() -> Start");
		setupUi(this);

		QString windowTitle = PLM_NAME + " PLM 3D Model Search Criteria ";
		this->setWindowTitle(windowTitle);

#ifdef __APPLE__
		this->setWindowFlags(Qt::Dialog | Qt::CustomizeWindowHint | Qt::WindowMinMaxButtonsHint | Qt::WindowCloseButtonHint);
#else
		this->setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint | Qt::CustomizeWindowHint);
        m_pTitleBar = new MVTitleBar(windowTitle, this);
        layout()->setMenuBar(m_pTitleBar);
#endif // !__APPLE__
	
		ViewLable_2->setStyleSheet(HEADER_STYLE);
		FilterLabel->setStyleSheet(HEADER_STYLE);
		PaletteCheckBoxLable->setStyleSheet(HEADER_STYLE);
		ViewComboBox->setStyleSheet(VIEW_COMBOBOX_STYLE + SCROLLBAR_STYLE);
		SearchFilter->setStyleSheet(VIEW_COMBOBOX_STYLE + SCROLLBAR_STYLE);
		ProductNameLabel->setStyleSheet(FONT_STYLE);
		ProductIdLabel->setStyleSheet(FONT_STYLE);
		ProductName->setStyleSheet(FONT_STYLE);
		ProductId->setStyleSheet(FONT_STYLE);
		QuickSearchCriteria->setStyleSheet(HEADER_STYLE);
		label_3->setStyleSheet(FONT_STYLE);
		lineEdit->setStyleSheet(FONT_STYLE);
		SearchCriteria->setStyleSheet(HEADER_STYLE);
		ProductName->setAttribute(Qt::WA_MacShowFocusRect, false);
		ProductId->setAttribute(Qt::WA_MacShowFocusRect, false);
		lineEdit->setAttribute(Qt::WA_MacShowFocusRect, false);

#ifdef __APPLE__
		horizontalLayout->setSpacing(10);
#endif

		Back->setIcon(QIcon(":/CLOVise/FlexPLM/Images/icon_back_over.svg"));
		Back->setStyleSheet(BUTTON_STYLE);
		Search->setStyleSheet(BUTTON_STYLE);

		dateResetButton->setStyleSheet("#dateResetButton { icon_size:18px; image: url(:/CLOVise/FlexPLM/Images/DateReset_none.svg); }""#dateResetButton:hover { image: url(:/CLOVise/FlexPLM/Images/DateReset_over.svg); }""QToolTip{ color: #46C8FF; background-color: #33414D; border: 1px #000000; }");

		dateResetButton->setToolTip("Reset Date");

		Search->setIcon(QIcon(":/CLOVise/FlexPLM/Images/icon_search_over.svg"));
	

		treeWidget_H->verticalScrollBar()->setStyleSheet(SCROLLBAR_STYLE);
		treeWidget_H->horizontalScrollBar()->setStyleSheet(SCROLLBAR_STYLE);
		SearchTree->verticalScrollBar()->setStyleSheet(SCROLLBAR_STYLE);
		SearchTree->horizontalScrollBar()->setStyleSheet(SCROLLBAR_STYLE);

		treeWidget_H->setStyleSheet("QTreeWidget{min-height:230px; min-width: 50px;max-height:1500px; max-width: 230px; border : 1px solid #000;font-family: ArialMT; font-size: 10px; background-color: #262628; margin-bottom: 20px;}  QTreeWidget:item:selected{color: #46C8FF;}");
		this->setStyleSheet("QDialog{height:539px; width: 1050px;}");
		PaletteSearchWidget->setStyleSheet("#PaletteSearchWidget{padding: 10px; width: 920px;  border: 1px solid #000; background-color: #262628; max-height:50px;}");
		QuickSearchWidget->setStyleSheet("#QuickSearchWidget{padding: 10px; width: 920px;border: 1px solid #000; background-color: #262628; max-height:42px;}");

		
		if (!PLMDocumentData::GetInstance()->isModelExecuted)
			RESTAPI::SetProgressBarData(20, "Loading 3D Model Search", true);
		m_hierarchyLoading = false;
		//initializeDocumentData();
		m_flextypeTree.clear();
		selectType = FlexTypeHelper::CreateFlexTypeTree(PLMDocumentData::GetInstance()->GetDocumentHierarchyJSON(), treeWidget_H, m_flextypeTree);
		treeWidget_H->topLevelItem(0)->setSelected(true);
		treeWidget_H->expandAll();
		treeWidget_H->setExpandsOnDoubleClick(false);

		onHideHirarchy(false);
		CVWidgetGenerator::CreateViewComboBoxOnSearch(ViewComboBox, PLMDocumentData::GetInstance()->GetDocumentViewJSON(), selectType.toStdString());
		drawSearcUI(selectType, true, BLANK);

		connectSignalSlots(true);
		SearchTree->resizeColumnToContents(0);
		SearchTree->resizeColumnToContents(1);
		SearchTree->resizeColumnToContents(2);
		SearchTree->header()->setSectionResizeMode(0, QHeaderView::Stretch);
		SearchTree->header()->setSectionResizeMode(1, QHeaderView::Stretch);
		SearchTree->header()->setSectionResizeMode(2, QHeaderView::Stretch);
		treeWidget_H->setSortingEnabled(true);
		treeWidget_H->sortByColumn(0, Qt::AscendingOrder);
		treeWidget_H->setFocusPolicy(Qt::NoFocus);
		if (!PLMDocumentData::GetInstance()->isModelExecuted)
			RESTAPI::SetProgressBarData(0, "", false);
#ifdef __APPLE__
		RESTAPI::SetProgressBarData(0, "", false);
#endif
		ProductNameLabel->hide();
		ProductIdLabel->hide();
		ProductName->hide();
		ProductId->hide();
	}

	/*
	* Description - HideHirarchy() method used to show/hide the hierarchy.
	* Parameter -  bool.
	* Exception -
	* Return -
	*/
	void PLMDocumentSearch::onHideHirarchy(bool _hide)
	{
		if (!_hide)
		{
			if (!m_isHidden)
			{
				toolButton->setToolTip("Show");
				treeWidget_H->hide();
				toolButton->setStyleSheet("#toolButton {\n" "qproperty-icon: none; \n""image: url(:/CLOVise/FlexPLM/Images/icon_open_tree_none.svg);\n""}\n"
					"#toolButton:hover{\n""image: url(:/CLOVise/FlexPLM/Images/icon_open_tree_over.svg);\n""}""QToolTip{ color: #46C8FF; background-color: #33414D; border: 1px #000000; }");
				m_isHidden = true;
			}
			else
			{
				toolButton->setToolTip("Hide");
				treeWidget_H->show();
				toolButton->setStyleSheet("#toolButton {\n" "qproperty-icon: none; \n""image: url(:/CLOVise/FlexPLM/Images/icon_close_tree_none.svg);\n""}\n"
					"#toolButton:hover{\n""image: url(:/CLOVise/FlexPLM/Images/icon_close_tree_over.svg);\n""}""QToolTip{ color: #46C8FF; background-color: #33414D; border: 1px #000000; }");
				m_isHidden = false;
			}
		}
	}

	PLMDocumentSearch::~PLMDocumentSearch()
	{
		DisconnectSignalSlots();
	}

	/*
	* Description - ResetDateEditWidget() method is a slot for reset date button click and used to reset the date fields.
	* Parameter -
	* Exception -
	* Return -
	*/
	void PLMDocumentSearch::onResetDateEditWidget()
	{
		UIHelper::ResetDate(SearchTree);
	}

	/*
	* Description - connectSignalSlots() method used to connect and disconnect the signal and slot.
	* Parameter -   bool.
	* Exception -
	* Return -
	*/
	void PLMDocumentSearch::connectSignalSlots(bool _b)
	{
		if (_b)
		{
			QObject::connect(toolButton, SIGNAL(clicked(bool)), this, SLOT(onHideHirarchy(bool)));
			QObject::connect(treeWidget_H, SIGNAL(itemClicked(QTreeWidgetItem*, int)), this, SLOT(onTreeNodeClicked(QTreeWidgetItem*, int)));
			QObject::connect(Search, SIGNAL(clicked()), this, SLOT(onClickedSubmitButton())); // should be used
			QObject::connect(Back, SIGNAL(clicked()), this, SLOT(onClickedBackButton()));
			QObject::connect(SearchFilter, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(onFilterChanged(const QString&)));
			QObject::connect(paletteCheckBox, SIGNAL(toggled(bool)), this, SLOT(onCheckBoxChecked(bool)));
			QObject::connect(dateResetButton, SIGNAL(clicked()), this, SLOT(onResetDateEditWidget()));
		}
		else
		{
			QObject::disconnect(toolButton, SIGNAL(clicked()), this, SLOT(onHideHirarchy(bool)));
			QObject::disconnect(Search, SIGNAL(clicked()), this, SLOT(onClickedSubmitButton())); // should be used
			QObject::disconnect(Back, SIGNAL(clicked()), this, SLOT(onClickedBackButton()));
			QObject::disconnect(SearchFilter, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(onFilterChanged(const QString&)));
			QObject::disconnect(dateResetButton, SIGNAL(clicked()), this, SLOT(onResetDateEditWidget()));
		}
	}

	/*
	* Description - OnClickedBackButton() method is a slot for back button click and used to go back design suite.
	* Parameter -
	* Exception -
	* Return -
	*/
	void PLMDocumentSearch::onClickedBackButton()
	{
		CLOVise::CLOViseSuite::GetInstance()->setModal(true);
		CLOViseSuite::GetInstance()->show();
		Destroy();
	}

	/*
	* Description - OnClickedSubmitButton() method is a slot for submit button click, validate the input and calling the result table module.
	* Parameter -
	* Exception - exception, Char *
	* Return -
	*/
	void PLMDocumentSearch::onClickedSubmitButton()
	{
		try
		{
			collectSearchUIFields();
			string quickSearchValue = Helper::GetJSONValue<string>(m_searchCriteriaJson, QUICK_SEARCK_CRITERIA_KEY, true);
			string productName = Helper::GetJSONValue<string>(m_searchCriteriaJson, PRODUCTNAME_KEY, true);
			string productId = Helper::GetJSONValue<string>(m_searchCriteriaJson, PRODUCTID_KEY, true);
			json attsJson = Helper::GetJSONParsedValue<string>(m_searchCriteriaJson, ATTRIBUTES_KEY, false);
			if ((PLMDocumentData::GetInstance()->GetProductIdKeyExists()))
			{
				if (!FormatHelper::HasContent(quickSearchValue) && !FormatHelper::HasContent(productName) && !FormatHelper::HasContent(productId) && attsJson.empty())
				{
					throw "Please enter atleast one criteria.";
				}
			}
			else
			{
				if (!FormatHelper::HasContent(quickSearchValue) && !FormatHelper::HasContent(productName) && attsJson.empty())
				{
					throw "Please enter atleast one criteria.";
				}
			}
			PLMDocumentData::GetInstance()->SetSelectedViewIdx(ViewComboBox->currentIndex());
			PLMDocumentData::GetInstance()->SetSearchCriteriaJSON(m_searchCriteriaJson);
			this->hide();
			//tmporarilry clearing Results object
			UTILITY_API->CreateProgressBar();
			UTILITY_API->SetProgress("Searching 3D Models", (qrand() % 101));
			RESTAPI::SetProgressBarData((qrand() % 101), "Searching 3D Models", true);
			//PLMDocumentResults::Destroy();
			PLMDocumentResults::GetInstance()->setDataFromResponse(PLMDocumentData::GetInstance()->GetSearchCriteriaJSON());
			PLMDocumentResults::GetInstance()->setModal(true);
			PLMDocumentResults::GetInstance()->exec();
			RESTAPI::SetProgressBarData(0, "", false);
		}
		catch (exception& e)
		{
			RESTAPI::SetProgressBarData(0, "", false);
			Logger::Error("PLMDocumentResults -> OnClickedSubmitButton Exception :: " + string(e.what()));
			UTILITY_API->DisplayMessageBox(e.what());
			this->show();
		}
		catch (const char* msg)
		{
			Logger::Error("PLMDocumentResults -> OnClickedSubmitButton Exception :: " + string(msg));
			wstring wstr(msg, msg + strlen(msg));
			RESTAPI::SetProgressBarData(0, "", false);
			UTILITY_API->DisplayMessageBoxW(wstr);
			this->show();
		}
	}

	/*
	* Description - FilterChanged() method is a slot for filter item changed and update/create search UI fields.
	* Parameter -  QString.
	* Exception - exception, Char *
	* Return -
	*/
	void PLMDocumentSearch::onFilterChanged(const QString& _item)
	{
		try
		{
			if (!m_hierarchyLoading) 
			{
				UTILITY_API->DeleteProgressBar(true);
				this->hide();
				UTILITY_API->CreateProgressBar();
				UTILITY_API->SetProgress("Loading", (qrand() % 101));
			}
			drawSearcUI(selectType, false, _item.toStdString());
			if (!m_hierarchyLoading) {
				UTILITY_API->DeleteProgressBar(false);
				this->show();
			}
		}
		catch (exception& e)
		{
			Logger::Error("PLMDocumentSearch -> FilterChanged Exception :: " + string(e.what()));
			UTILITY_API->DisplayMessageBox(e.what());
			this->close();
		}
		catch (const char* msg)
		{
			Logger::Error("PLMDocumentSearch -> FilterChanged Exception :: " + string(msg));
			wstring wstr(msg, msg + strlen(msg));
			UTILITY_API->DisplayMessageBoxW(wstr);
			this->close();
		}
	}

	/*
	* Description - CheckBoxChecked() method is a slot for check box  for search through product name.
	* Parameter -  bool.
	* Exception - exception, Char *
	* Return -
	*/
	void PLMDocumentSearch::onCheckBoxChecked(bool _checked)
	{
		try
		{
			if (_checked)
			{
				ProductName->show();
				ProductNameLabel->show();
				if (PLMDocumentData::GetInstance()->GetProductIdKeyExists())
				{
					ProductId->show();
					ProductIdLabel->show();
				}
			}
			else
			{
				ProductName->setText("");
				ProductName->hide();
				ProductId->setText("");
				ProductId->hide();
				ProductNameLabel->hide();
				ProductIdLabel->hide();
			}

		}
		catch (exception& e)
		{
			Logger::Error("PLMDocumentSearch -> CheckBoxChecked Exception :: " + string(e.what()));
			UTILITY_API->DisplayMessageBox(e.what());
		}
		catch (const char* msg)
		{
			Logger::Error("PLMDocumentSearch -> CheckBoxChecked Exception :: " + string(msg));
			wstring wstr(msg, msg + strlen(msg));
			UTILITY_API->DisplayMessageBoxW(wstr);
		}
	}

	/*
	* Description - TreeNodeClicked() method is a slot for hierarchy is click.
	* Parameter -  QTreeWidgetItem, int.
	* Exception - exception, Char *
	* Return -
	*/
	void PLMDocumentSearch::onTreeNodeClicked(QTreeWidgetItem* _item, int _column)
	{
		m_hierarchyLoading = true;
		try
		{
			QString selectedRootDisplayName = _item->text(_column);
			if (!_item->isSelected())
			{
				UTILITY_API->DeleteProgressBar(true);
				this->hide();
				UTILITY_API->CreateProgressBar();
				treeWidget_H->clearSelection();
				_item->setSelected(true);
				selectType = _item->data(1, Qt::UserRole).toString();
				UTILITY_API->SetProgress("Loading", (qrand() % 101));
				CVWidgetGenerator::CreateViewComboBoxOnSearch(ViewComboBox, PLMDocumentData::GetInstance()->GetDocumentViewJSON(), selectType.toStdString());
				drawSearcUI(selectType, true, BLANK);
				m_hierarchyLoading = false;
				UTILITY_API->DeleteProgressBar(true);
				this->show();
			}
		}
		catch (exception& e)
		{
			Logger::Error("PLMDocumentSearch -> TreeNodeClicked Exception :: " + string(e.what()));
			UTILITY_API->DisplayMessageBox(e.what());
			this->close();
		}
		catch (const char* msg)
		{
			Logger::Error("PLMDocumentSearch -> TreeNodeClicked Exception :: " + string(msg));
			wstring wstr(msg, msg + strlen(msg));
			UTILITY_API->DisplayMessageBoxW(wstr);
			this->close();
		}

	}

	/*
	* Description - DrawSearchUI() method used to create search fields in search UI.
	* Parameter -  QString, bool, string.
	* Exception - exception, Char *
	* Return -
	*/
	void PLMDocumentSearch::drawSearcUI(QString _selectType, bool _drawFilter, string _selectedFilter)
	{
		try
		{
			PLMDocumentData::GetInstance()->SetDateFlag(false);
			QStringList attScops = PLMDocumentData::GetInstance()->GetAttScopes();
			//if (!CVWidgetGenerator::DrawFilterAndSearchCriteriaWidget(PLMDocumentData::GetInstance()->GetDocumentFilterJSON(), SearchFilter, SearchTree, _selectType, _selectedFilter, attScops, _drawFilter))
			//{
			//	//FlexTypeHelper::DrawDefaultSearchCriteriaWidget(PLMDocumentData::GetInstance()->GetDocumentFieldsJSON(), _selectType.toStdString(), SearchTree, attScops);
			//}
			SearchTree->setSelectionMode(QAbstractItemView::NoSelection);
			SearchTree->setStyleSheet("QTreeWidget { background-color: #262628; border: 1px solid #000; padding-left: 20px; }""QTreeWidget::item {height: 20px; width: 200px; margin-right: 20px; margin-top: 5px; margin-bottom: 5px; border: none; }""QTreeWidget::item:hover{ background-color: #262628; }");
			
			if (!PLMDocumentData::GetInstance()->GetDateFlag())
				dateResetButton->hide();
			else
				dateResetButton->show();
		}
		catch (exception e)
		{
			throw e;
			CLOViseSuite::GetInstance()->show();
			Destroy();
		}
	}

	/*
	* Description - CollectSearchUIFields() method used collect all the user input from search UI.
	* Parameter - 
	* Exception -
	* Return -
	*/
	void  PLMDocumentSearch::collectSearchUIFields()
	{
		string productNameValue = "";
		string productIdValue = "";
		string quickSearchValue = "";
		string filterValue = "";
		m_searchCriteriaJson.clear();
		if (paletteCheckBox->checkState())
		{
			productNameValue = ProductName->text().toStdString();
			productIdValue = ProductId->text().toStdString();
			UIHelper::ProductValidation(productNameValue, productIdValue, PLMDocumentData::GetInstance()->GetProductIdKeyExists());
			m_searchCriteriaJson[PRODUCTNAME_KEY] = productNameValue;
			m_searchCriteriaJson[PRODUCTID_KEY] = productIdValue;
			m_searchCriteriaJson[SEASONID_KEY] = BLANK;
			m_searchCriteriaJson[PALETTEID_KEY] = BLANK;
		}
		else
		{
			m_searchCriteriaJson[PRODUCTNAME_KEY] = BLANK;
			m_searchCriteriaJson[PRODUCTID_KEY] = BLANK;
			m_searchCriteriaJson[SEASONID_KEY] = BLANK;
			m_searchCriteriaJson[PALETTEID_KEY] = BLANK;
		}
		quickSearchValue = lineEdit->text().toStdString();
		if (FormatHelper::HasContent(quickSearchValue))
		{
			m_searchCriteriaJson[QUICKSEARCH_KEY] = quickSearchValue;
		}
		else
		{
			m_searchCriteriaJson[QUICKSEARCH_KEY] = "";
		}

		filterValue = SearchFilter->currentText().toStdString();
		m_searchCriteriaJson[FILTER_ID_KEY] = SearchFilter->property(filterValue.c_str()).toString().toStdString();
		string tempType = selectType.toStdString();
		tempType = FormatHelper::ReplaceString(tempType, "\\\\", "\\");
		m_searchCriteriaJson[FLEX_TYPE_STRING_KEY] = tempType;
		m_searchCriteriaJson[MODULE_KEY] = DOCUMENT_MODULE;
		m_searchCriteriaJson[INCLUDE_SUPPLIER_STRING_KEY] = "";
		m_searchCriteriaJson[SEARCHBY_SUPPLIER_STRING_KEY] = "";
		m_searchCriteriaJson[REQ_FROM_CLO_KEY] = "true";
		m_searchCriteriaJson[ATTRIBUTES_KEY] = UIHelper::CollectSearchCriteriaFields(SearchTree);
	}
}
