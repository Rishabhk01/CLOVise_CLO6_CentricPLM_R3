#pragma once
/*
* Copyright 2020-2021 CLO-Vise. All rights reserved
*
* @file CVWidgetGenerator.h
*
* @brief Class declaration for creating widget.
* This class has all the variable and function declaration which are used creating widget.
*
* @author GoVise
*
* @date 27-MAY-2020
*/
#include <string>

#include <qlistwidget.h>
#include <qtextedit.h>
#include <qstringlist.h>
#include <QDateEdit>
#include <QtWidgets>
#include <QtCore>
#include <QtGui>

#include "CLOVise/PLM/Helper/Util/ComboBoxItem.h"
#include "CLOVise/PLM/Helper/UIHelper/CVTableWidgetItem.h"
#include "CLOVise/PLM/Libraries/json.h"
#include "classes/widgets/MVTableWidget.h"

using json = nlohmann::json;
using namespace std;

class CVWidgetGenerator
{
public:
	static CVWidgetGenerator* GetInstance();

	static QListWidget* CreateCVMultiListWidget(QListWidget* _listWidget, QString _dispVal, QString _dispKey, QString _dispList);

	static QTextEdit* CreateTextAreaWidget(QString _defaultValue, bool _filterTemplateType);
	static QTextEdit* CreateTextAreaWidget(string _defaultValue, bool _filterTemplateType);
	static QTextEdit* CreateTextAreaWidget(QString _defaultValue, bool _filterTemplateType, int _fixedHeight);
	static QTextEdit* CreateTextAreaWidget(string _defaultValue, bool _filterTemplateType, int _fixedHeight);
	static QTextEdit* CreateTextAreaWidget(QString _defaultValue, bool _filterTemplateType, int _fixedHeight, char* _styleSheet);
	static QTextEdit* CreateTextAreaWidget(QString _defaultValue, int _fixedHeight, char* _styleSheet);
	static QTextEdit* CreateTextAreaWidget(string _defaultValue);

	static QDateEdit* CreateDateWidget(string _defaultValue, bool _tmplateType);

	static ComboBoxItem* CreateBooleanWidget(string _defaultValue, bool _templateType, QTreeWidgetItem* _item, int _val);
	static ComboBoxItem* CreateBooleanWidget(QString _defaultValue, bool _templateType, QTreeWidgetItem* _item, int _val);

	static QComboBox* CreateComboBox(bool _disable, QComboBox* _filterCombo, int _defaultIndext, QStringList _itemList);
	static ComboBoxItem* CreateComboBoxItem(bool _disable, ComboBoxItem* _filterCombo, int _defaultIndext, QStringList& _itemList);

	static QLabel* CreateLabelWidget(string _defaultValue, string _attributeKey, string _attributeType);
	static QLabel* CreateLabelWidget(string _defaultValue, string _attributeKey, string _attributeType, bool _isRequired, bool _isDisable);

	static QSpinBox* CreateSpinBoxWidget(QString _defaultValue, bool _templateType, string _input);
	static QSpinBox* CreateSpinBoxWidget(QString _defaultValue, string _input);
	static QSpinBox* CreateSpinBoxWidgetForRGB(QString _defaultValue, bool _templateType, string _input, int _min, int _max);
	static QDoubleSpinBox* CreateDoubleSpinBoxWidget(QString _defaultValue, bool _templateType, string _input, string _decimalPrecision, double _min, double _max);
	static QDoubleSpinBox* CreateDoubleSpinBoxWidget(QString _defaultValue, string _input, string _decimalPrecision, double _min, double _max);

	static bool DrawFilterAndSearchCriteriaWidget(json _filterjson, QComboBox* _filterCombo, QTreeWidget* _searctTree_1, QTreeWidget* _searctTree_2, QString _selecttypen, string _selectedFilter, QStringList _attScops, bool _drawFilterWid);
	static void DrawFilterWidget(json _filterjson, QComboBox* _filterComboBox);

	static void CreateSearchCriteriaWidget(QTreeWidget* _searchTree_1, QTreeWidget* _searchTree_2, json _attributesJson, bool _noFilter, bool _isTemplateType, QStringList _attScops);

	static void CreateViewComboBoxOnSearch(QComboBox* _viewCB, json _viewJSON, string _selectedTypeName);
	static void CreateViewComboBoxOnSearch(QComboBox* _viewCB, json _viewJSON, string _selectedTypeName, int _selectedViewIdx);
	static QStringList CreateViewComboBoxOnResults(MVTableWidget* _resultTable, json _viewJSON, string _selectedTypeName, string _selectedView, string _thubmnailDisplayName, string _inlcudePalette, string _includeSeason, QStringList _attScopes, string _module, bool _isProductExist);
	static void DrawViewAndResultsWidget(MVTableWidget* _resultTable, QListWidget* _iconWidget, bool _createIconwidget, json _reultJson, string _thubmnailDisplayName, QStringList _headerKeyList, string _inlcudePalette, string _includeSeason, QStringList _attScopes, bool _isReadiobutton, string _module, bool _isProductExist, json _viewListJsonArray);
	static void DrawViewAndResultsWidget(QComboBox* _viewCB, MVTableWidget* _resultTable, QListWidget* _iconWidget, bool _createIconwidget, json _reultJson, json _viewJSON, string _thubmnailDisplayName, string _selectedTypeName, bool _drawView, int _selectedViewIdx, QStringList _attScopes, bool _isReadiobutton, bool _isProductExist, json _viewListJsonArray);
	static QRadioButton* CreateRadioButtonWidget(string _colorId, string _compatibility, string _colorName, char* _styleSheet, MVTableWidget* _resultTable);
	static QWidget* CreateCheckBoxWidget(bool _isChecked, string _colorId = "", string _compatibility = "", string _colorName = "");
	static QWidget* CreateThumbnailWidget(json _resultJson, string _compatibility, string _solidKey);
	static QWidget* CreateThumbnailWidget(json _resultJson, QPixmap pixmap, string _module);
	static QTableWidgetItem* CreateTableWidgetItem(string _dispValue);
	static void CreateIconWidget(json _resultListJson, QListWidget* _iconWidget, QStringList _tootTipKeys, QStringList _tootTipKeyDisplay, string _compatibility, string _solidKey, bool _isReadiobutton);
	static void CreateIconWidget(json _resultListJson, QListWidget* _iconWidget, QStringList _tootTipKeys, QStringList _tootTipKeyDisplay, string _compatibility, string _solidKey, bool _isReadiobutton, QPixmap _pixmap);
	static void CreateResultsPerPageWidget(QComboBox* _resultPerPageCB, QStringList _resultPerPageList);
	static void PopulateValuesOnResultsUI(QToolButton* _nextButton, QLabel* _numOfResults, QLabel* _totalPageLabel, QComboBox* _resultPerPageComboBox, QStringList _rppList, int _resultsCount);
	static void PopulateValuesOnResultsUI(QToolButton* _nextButton, QLabel* _numOfResults, QLabel* _totalPageLabel, QComboBox* _resultPerPageComboBox, int _resultsCount);
	static void UpdateTableAndIconRows(MVTableWidget* _resultTable, QLabel* _currPageLabel, QComboBox* _resultPerPageComboBox, int _resultsCount, bool _isSortEnabled);
	static void UpdateTableAndIconRows(QListWidget* _iconWidget, QLabel* _currPageLabel, QComboBox* _resultPerPageComboBox, int _resultsCount);
	static void InitializeIconView(QListWidget* _iconWidget);
	static void InitializeTableView(MVTableWidget* _resultTable);
	static QStringList TableRowSelected(MVTableWidget* _resultTable, QListWidget* _iconViewTable, bool _isRadioButton, bool _isIconWidgetAvailable);
	static QStringList IconRowSelected(QListWidget* _iconWidget, MVTableWidget* _resultTable, bool _isRadioButton);
	static void DeSelectAllClicked(MVTableWidget* _resultTable, QListWidget* _iconWidget, QPushButton* _download);
	static QStringList SetDownloadCount(QStringList _iconviewSelectedIds, QStringList _tableviewSelectedIds, QPushButton* _download);
	static bool IsNumber(string _token);
	static QString AddNewLinesToString(string& _str, int _sep);
	static void SortIconResultTable(bool _isResultTableSort, QListWidget* _iconWidget, MVTableWidget* _resultTable, json _resultJson, bool _isRadioButton);
	static QPushButton* CreatePushButton(QString _name, QString _iconPath, QString _toolTip, QString _styleSheet, int _height, bool _showWidget);
	static QToolButton* CreateToolButton(QString _iconPath, QString _toolTip, QString _styleSheet, bool _showWidget);
	static QLineEdit*CreateLineEdit(QString _toolTip, QString _styleSheet, bool _showWidget);
	static QLabel* CreateLabel(QString _name, QString _toolTip, QString _styleSheet, bool _showWidget);
	static ComboBoxItem* CreateComboBox(QString _toolTip, QString _styleSheet, bool _showWidget);
	static void SetButtonProperty(QPushButton* _button, QString _iconPath, QString _toolTip, QString _styleSheet);
	static void SetButtonProperty(QToolButton* _button, QString _iconPath, QString _toolTip, QString _styleSheet);
	static QTreeWidget* CreateHirarchyTreeWidget(QString _style, bool _showWidget);
	static QTreeWidget* CreateSearchCriteriaTreeWidget(/*QString _style*/bool _showWidget);
	static QTreeWidget* CreatePublishTreeWidget(QString _style, bool _showWidget);
	static QCheckBox* CreateCheckBox(QString _toolTip, QString _style, bool _showWidget);
	static QPixmap ReadQPixmap(json _resultListJson, string _url, string _compatibility);
	static QImage ReadQImage(json _resultListJson, string _url, string _compatibility);

	static QWidget* InsertWidgetInCenter(QWidget* _widget);
private:
	static CVWidgetGenerator* _instance; // zero initialized by default

};