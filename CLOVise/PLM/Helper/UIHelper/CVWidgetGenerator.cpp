/*
* Copyright 2020-2021 CLO-Vise. All rights reserved
*
* @file CVWidgetGenerator.cpp
*
*@brief Class implementation for creating widget.
* This class has all the variable and function implementation which are used creating widget.
*
* @author GoVise
*
* @date 27-MAY-2020
*/
#include "CVWidgetGenerator.h"

#include <fstream>
#include <regex>

#include <QStringList>
#include <QtWidgets>

#include "CLOVise/PLM/Helper/Util/Helper.h"
#include "CLOVise/PLM/Helper/Util/FormatHelper.h"
#include "CLOVise/PLM/Helper/Util/Definitions.h"
#include "CLOVise/PLM/Libraries/json.h"
#include "CLOVise/PLM/Helper/Util/DirectoryUtil.h"
#include "CLOVise/PLM/Helper/Util/FormatHelper.h"
#include "CLOVise/PLM/Inbound/Product/ProductConfig.h"
#include "CLOVise/PLM/Inbound/Document/DocumentConfig.h"
#include "CLOVise/PLM/Inbound/Color/ColorConfig.h"
#include "CLOVise/PLM/Inbound/Material/MaterialConfig.h"
#include "CLOVise/PLM/Helper/UIHelper/UIHelper.h"
#include "CLOVise/PLM/Helper/UIHelper/CVQDateEdit.h"
#include "CLOVise/PLM/Helper/Util/Logger.h"
#include "CLOVise/PLM/Helper/Util/CustomSpinBox.h"
#include "CLOVise/PLM/Helper/Util/CustomDoubleSpinBox.h"

using json = nlohmann::json;
using namespace std;

CVWidgetGenerator* CVWidgetGenerator::_instance = NULL;
CVWidgetGenerator* CVWidgetGenerator::GetInstance()
{
	if (_instance == NULL)
	{
		_instance = new CVWidgetGenerator();
	}
	return _instance;
}

/*
* Description - CreateCVMultiListWidget() method used to create CV multi list widget.
* Parameter -  QListWidget, QString, QListWidget, QListWidget.
* Exception - exception, Char *
* Return - QListWidget.
*/
QListWidget* CVWidgetGenerator::CreateCVMultiListWidget(QListWidget* _listWidget, QString _dispVal, QString _dispKey, QString _defaultdispValue)
{
	Logger::Debug("CVWidgetGenerator::CreateCVMultiListWidget() - dispVal::" + _dispVal.toStdString());
	_listWidget->verticalScrollBar()->setStyleSheet(SCROLLBAR_STYLE);
	_listWidget->horizontalScrollBar()->setStyleSheet(SCROLLBAR_STYLE);
	QListWidgetItem* item = new QListWidgetItem(_dispVal, _listWidget);
#ifdef __APPLE__
	_listWidget->setSpacing(-3);
	_listWidget->setStyleSheet("margin-left: 5px;");
#endif // __APPLE__
	_listWidget->setProperty(_dispVal.toStdString().c_str(), _dispKey);
	item->setFlags(item->flags() | Qt::ItemIsUserCheckable); // set checkable flag

	item->setCheckState(Qt::Unchecked); // AND initialize check state
	if (FormatHelper::HasContent(_defaultdispValue.toStdString()))
	{
		QStringList dispList;
		QString dispListValue = "";
		if (_defaultdispValue.contains(QString::fromStdString(DELIMITER_NEGATION)))
		{
			dispList = _defaultdispValue.split(QString::fromStdString(DELIMITER_NEGATION));			
			for (int i = 0; i < dispList.size(); i++)
			{
				dispListValue = dispList[i];
				if (dispListValue == _dispKey)
				{
					item->setCheckState(Qt::Checked);
				}
			}
		}
		else if (_defaultdispValue.contains(QString::fromStdString(DELIMITER_COMMA)))
		{
			dispList = _defaultdispValue.split(QString::fromStdString(DELIMITER_COMMA));			
			for (int i = 0; i < dispList.size(); i++)
			{
				dispListValue = dispList[i];
				if (dispListValue == _dispVal)
				{
					item->setCheckState(Qt::Checked);
				}
			}
		}
		
		else
		{
			dispListValue = _defaultdispValue;
			if (dispListValue == _dispVal || dispListValue == _dispKey)
			{
				item->setCheckState(Qt::Checked);
			}
		}

	}
	return _listWidget;
}

/*
* Description - CreateTextAreaWidget() method used to create text area widget.
* Parameter -  string.
* Exception -
* Return - QTextEdit.
*/
QTextEdit* CVWidgetGenerator::CreateTextAreaWidget(string defaultValue)
{
	return CreateTextAreaWidget(QString::fromStdString(defaultValue), 70, "border: 1px solid black; background-color: #222222;");
}

/*
* Description - CreateTextAreaWidget() method used to create text area widget.
* Parameter -  string, bool.
* Exception -
* Return - QTextEdit.
*/
QTextEdit* CVWidgetGenerator::CreateTextAreaWidget(string _defaultValue, bool _filterTemplateType)
{
	return CreateTextAreaWidget(QString::fromStdString(_defaultValue), _filterTemplateType);
}

/*
* Description - CreateTextAreaWidget() method used to create text area widget.
* Parameter -  QString, bool.
* Exception -
* Return - QTextEdit.
*/
QTextEdit* CVWidgetGenerator::CreateTextAreaWidget(QString _defaultValue, bool _filterTemplateType)
{

	return CreateTextAreaWidget(_defaultValue, _filterTemplateType, 70, "border: 1px solid black; background-color: #222222;");
}

/*
* Description - CreateTextAreaWidget() method used to create text area widget.
* Parameter -  QString, bool, int.
* Exception -
* Return - QTextEdit.
*/
QTextEdit* CVWidgetGenerator::CreateTextAreaWidget(QString _defaultValue, bool _filterTemplateType, int _fixedHeight)
{

	return CreateTextAreaWidget(_defaultValue, _filterTemplateType, _fixedHeight, "border: 1px solid black; background-color: #222222;");
}

/*
* Description - CreateTextAreaWidget() method used to create text area widget.
* Parameter -  QString, bool, int.
* Exception -
* Return - QTextEdit.
*/
QTextEdit* CVWidgetGenerator::CreateTextAreaWidget(string _defaultValue, bool _filterTemplateType, int _fixedHeight)
{

	return CreateTextAreaWidget(QString::fromStdString(_defaultValue), _filterTemplateType, _fixedHeight, "border: 1px solid black; background-color: #222222;");
}

/*
* Description - CreateTextAreaWidget() method used to create text area widget.
* Parameter -  QString, bool, int, char*.
* Exception -
* Return - QTextEdit.
*/
QTextEdit* CVWidgetGenerator::CreateTextAreaWidget(QString _defaultValue, bool _filterTemplateType, int _fixedHeight, char* _styleSheet)
{
	QTextEdit* TextEdit = new QTextEdit();
	TextEdit->setMaximumHeight(_fixedHeight);
	TextEdit->setStyleSheet(TEXTEDIT_STYLE);
	if (FormatHelper::HasContent(_defaultValue.toStdString()))
	{
		TextEdit->setProperty(ATTRIBUTE_INITIAL_VALUE.c_str(), _defaultValue);
		_defaultValue = FormatHelper::ReplaceString(_defaultValue, DELIMITER_NEGATION, "");
		TextEdit->setText(_defaultValue);
		TextEdit->setPlainText(_defaultValue);
	}
	if (!_filterTemplateType)
	{
		TextEdit->setDisabled(true);
		TextEdit->setStyleSheet(DISABLED_TEXTEDIT_STYLE);
	}
	return TextEdit;
}

/*
* Description - CreateTextAreaWidget() method used to create text area widget.
* Parameter -  QString, int, char*.
* Exception -
* Return - QTextEdit.
*/
QTextEdit* CVWidgetGenerator::CreateTextAreaWidget(QString _defaultValue, int _fixedHeight, char* _styleSheet)
{
	QTextEdit* TextEdit = new QTextEdit();
	//	TextEdit->setMaximumHeight(fixedHeight);
	TextEdit->setStyleSheet(TEXTEDIT_STYLE);
	TextEdit->setProperty(ATTRIBUTE_INITIAL_VALUE.c_str(), _defaultValue);
	if (FormatHelper::HasContent(_defaultValue.toStdString()))
	{
		_defaultValue = FormatHelper::ReplaceString(_defaultValue, DELIMITER_NEGATION, "");
		TextEdit->setText(_defaultValue);
		TextEdit->setPlainText(_defaultValue);
	}
	return TextEdit;
}

/*
* Description - CreateLabelWidget() method used to create label widget.
* Parameter -  string, string, string.
* Exception - exception, Char *
* Return - QLabel.
*/
QLabel* CVWidgetGenerator::CreateLabelWidget(string _defaultValue, string _attributeKey, string _attributeType)
{
	QLabel* label = new QLabel();
	label->setStyleSheet("QLabel{ outline : 0; }");
	label->setStyleSheet((FONT_STYLE));
	label->setWordWrap(true);
	label->setText(QString::fromStdString(_defaultValue));
	label->setProperty(ATTRIBUTE_KEY.c_str(), QString::fromStdString(_attributeKey));
	label->setProperty(ATTRIBUTE_TYPE_KEY.c_str(), QString::fromStdString(_attributeType));
	label->setAlignment(Qt::AlignTop);
	return label;
}

/*
* Description - CreateLabelWidget() method used to create label widget.
* Parameter -  string, string, string, bool.
* Exception -
* Return - QLabel.
*/
QLabel* CVWidgetGenerator::CreateLabelWidget(string _defaultValue, string _attributeKey, string _attributeType, bool _isRequired, bool _isDisable)
{
	QLabel* label = new QLabel();
	label->setStyleSheet(FONT_STYLE);
	label->setWordWrap(true);
	label->setText(QString::fromStdString(_defaultValue));
	label->setProperty(ATTRIBUTE_KEY.c_str(), QString::fromStdString(_attributeKey));
	label->setProperty(ATTRIBUTE_TYPE_KEY.c_str(), QString::fromStdString(_attributeType));
	label->setProperty(ATTRIBUTE_REQUIRED_KEY.c_str(), _isRequired);
	label->setProperty(ATTRIBUTE_NAME_KEY.c_str(), QString::fromStdString(_defaultValue));
	label->setAlignment(Qt::AlignTop);
	if (_isDisable) {
		label->setStyleSheet(DISABLED);
	}
	if (_isRequired)
		label->setText(QString::fromStdString(_defaultValue) + "<font color='red'>*</font>");
	return label;
}

/*
* Description - CreateDateWidget() method used to create date widget.
* Parameter -  string, bool.
* Exception -
* Return - QDateEdit.
*/
QDateEdit* CVWidgetGenerator::CreateDateWidget(string _defaultValue, bool _filterTemplateType)
{
	CVQDateEdit* DateTimeEdit = new CVQDateEdit();
	DateTimeEdit->setCalendarPopup(true);
	DateTimeEdit->IgnoreMask;
	DateTimeEdit->setStyleSheet("QDateEdit { background-color: #222224;font-size: 10px; font-face: ArialMT; min-height: 20px; border-bottom: 1px inset #262626; border-left: 1px inset #0D0D0D; border-top: 1px inset #0D0D0D; border-right: 1px inset #262626; padding: 1px; spacing: 1px; color: #FFF; font-size: 10px; }"
		"QDateEdit::drop-down { image: url(:/CLOVise/PLM/Images/ui_dropbox_none.svg); width: 18px; height: 18px; subcontrol-position: right center; subcontrol-origin:margin;}"
		"QDateEdit::drop-down:hover { image: url(:/CLOVise/PLM/Images/ui_dropbox_over.svg); width: 18px; height: 18px; subcontrol-position: right center; subcontrol-origin:margin;}");
	QCalendarWidget *calendar = new QCalendarWidget();
	calendar->setGridVisible(false);
	calendar->setVerticalHeaderFormat(QCalendarWidget::NoVerticalHeader);
	calendar->showToday();
	calendar->setStyleSheet("QCalendarWidget QMenu { color: #FFFFFF; background-color: #262628; }""QCalendarWidget QWidget { alternate-background-color: #46C8FF; color: #FFFFFF; }""QCalendarWidget QAbstractItemView:enabled { color: #FFFFFF; background-color: #262628; selection-color: #FFFFFF; selection-background-color: #46C8FF; }"
		"QCalendarWidget QWidget#qt_calendar_navigationbar { background-color: #262628; }""QCalendarWidget QToolButton {  }");
	DateTimeEdit->setCalendarWidget(calendar);
	DateTimeEdit->setDisplayFormat("yyyy-MM-dd");
	DateTimeEdit->setNullable(true);
	DateTimeEdit->setFocusPolicy(Qt::StrongFocus);
	if (FormatHelper::HasContent(_defaultValue))
	{		
		string defaultDate = _defaultValue.substr(0, _defaultValue.rfind("-")+3);		
		if (FormatHelper::HasContent(defaultDate))
		{			
			int day, month, year;
			sscanf(defaultDate.c_str(), "%4d-%2d-%2d", &year, &month, &day);
			QDate defaultQDate;
			defaultQDate.setDate(year, month, day);
			DateTimeEdit->setDate(defaultQDate);
			string fieldValue = FormatHelper::RetrieveDate(DateTimeEdit);
			if (fieldValue.find(DATE_FORMAT_TEXT.toStdString()) != string::npos)
			{
				fieldValue = "";
			}
			DateTimeEdit->setProperty(ATTRIBUTE_INITIAL_VALUE.c_str(), QString::fromStdString(fieldValue));
		}
	}
	if (!_filterTemplateType)
	{
		DateTimeEdit->setDisabled(true);
		DateTimeEdit->setStyleSheet("QDateEdit { background-color: #222224;font-size: 10px; font-face: ArialMT; min-height: 20px; border-bottom: 1px inset #262626; border-left: 1px inset #0D0D0D; border-top: 1px inset #0D0D0D; border-right: 1px inset #262626; padding: 1px; spacing: 1px; color: #808080; font-size: 10px; }"
			"QDateEdit::drop-down { image: url(:/CLOVise/PLM/Images/ui_dropbox_none.svg); width: 18px; height: 18px; subcontrol-position: right center; subcontrol-origin:margin;}"
			"QDateEdit::drop-down:hover { image: url(:/CLOVise/PLM/Images/ui_dropbox_over.svg); width: 18px; height: 18px; subcontrol-position: right center; subcontrol-origin:margin;}");
	}
	return DateTimeEdit;
}

/*
* Description - CreateDoubleSpinBoxWidget() method used to create double spin box widget.
* Parameter -  QString, string, string, double, double.
* Exception -
* Return - QDoubleSpinBox
*/
QDoubleSpinBox* CVWidgetGenerator::CreateDoubleSpinBoxWidget(QString _defaultPresetValue, string _input, string _decimalPrecision, double _min, double _max)
{
	CustomDoubleSpinBox* doublespinBox = new CustomDoubleSpinBox();
	doublespinBox->setMaximumHeight(SPINBOX_HEIGHT);
	doublespinBox->setStyleSheet(DOUBLE_SPINBOX_STYLE);
	doublespinBox->setDecimals(stoi(_decimalPrecision));
	doublespinBox->setRange(_min, _max);
	doublespinBox->setPrefix(QString::fromStdString(_input));
	doublespinBox->setValue(0);
	doublespinBox->setCorrectionMode(QAbstractSpinBox::CorrectToNearestValue);
	doublespinBox->setFocusPolicy(Qt::StrongFocus);
	if (FormatHelper::HasContent(_defaultPresetValue.toStdString()))
	{
		doublespinBox->setProperty(ATTRIBUTE_INITIAL_VALUE.c_str(), _defaultPresetValue);
		doublespinBox->setValue(stoi(_defaultPresetValue.toStdString()));
	}
	return doublespinBox;
}

/*
* Description - DrawSearchUI() method used to create search fields in search UI.
* Parameter -  QString, bool, string, string, double, double.
* Exception -
* Return - QDoubleSpinBox.
*/
QDoubleSpinBox* CVWidgetGenerator::CreateDoubleSpinBoxWidget(QString _defaultPresetValue, bool _templateType, string _input, string _decimalPrecision, double _min, double _max)
{
	CustomDoubleSpinBox* doublespinBox = new CustomDoubleSpinBox();
	doublespinBox->setMaximumHeight(SPINBOX_HEIGHT);
	doublespinBox->setStyleSheet(DOUBLE_SPINBOX_STYLE);
	doublespinBox->setDecimals(stoi(_decimalPrecision));
	doublespinBox->setRange(_min, _max);
	doublespinBox->setPrefix(QString::fromStdString(_input));
	doublespinBox->setValue(0);
	doublespinBox->setCorrectionMode(QAbstractSpinBox::CorrectToNearestValue);
	doublespinBox->setFocusPolicy(Qt::StrongFocus);
	if (FormatHelper::HasContent(_defaultPresetValue.toStdString()))
	{
		doublespinBox->setProperty(ATTRIBUTE_INITIAL_VALUE.c_str(), _defaultPresetValue);
		doublespinBox->setValue(stoi(_defaultPresetValue.toStdString()));
		if (!_templateType)
		{
			doublespinBox->setDisabled(true);
			doublespinBox->setStyleSheet(DISABLED_DOUBLE_SPINBOX_STYLE);
		}
	}
	return doublespinBox;
}

/*
* Description - CreateSpinBoxWidget() method used to create spin box widget.
* Parameter -  QString, bool, string.
* Exception -
* Return - QSpinBox
*/
QSpinBox* CVWidgetGenerator::CreateSpinBoxWidget(QString _defaultPresetValue, bool _templateType, string _input)
{
	CustomSpinBox* spinBox = new CustomSpinBox();
	spinBox->setMaximumHeight(SPINBOX_HEIGHT);
	spinBox->setStyleSheet(SPINBOX_STYLE);
	spinBox->setRange(0, 10000000000000);
	spinBox->setPrefix(QString::fromStdString(_input));
	spinBox->setValue(0);
	spinBox->setFocusPolicy(Qt::StrongFocus);
	spinBox->setCorrectionMode(QAbstractSpinBox::CorrectToNearestValue);
	if (FormatHelper::HasContent(_defaultPresetValue.toStdString()))
	{
		spinBox->setValue(stoi(_defaultPresetValue.toStdString()));
		spinBox->setProperty(ATTRIBUTE_INITIAL_VALUE.c_str(), _defaultPresetValue);
	}
	if (!_templateType)
	{
		spinBox->setDisabled(true);
		spinBox->setStyleSheet(DISABLED_SPINBOX_STYLE);
	}
	return spinBox;
}

/*
* Description - CreateSpinBoxWidget() method used to create spin box widget.
* Parameter -  QString,bool,string,int,int.
* Exception -
* Return - QSpinBox.
*/
QSpinBox* CVWidgetGenerator::CreateSpinBoxWidgetForRGB(QString _defaultPresetValue, bool _templateType, string _input, int _min, int _max)
{
	CustomSpinBox* spinBox = new CustomSpinBox();
	spinBox->setMaximumHeight(SPINBOX_HEIGHT);
	spinBox->setStyleSheet(SPINBOX_STYLE);
	spinBox->setRange(_min, _max);
	spinBox->setPrefix(QString::fromStdString(_input));
	spinBox->setValue(0);
	spinBox->setCorrectionMode(QAbstractSpinBox::CorrectToNearestValue);
	spinBox->setFocusPolicy(Qt::StrongFocus);
	if (FormatHelper::HasContent(_defaultPresetValue.toStdString()))
	{
		spinBox->setValue(stoi(_defaultPresetValue.toStdString()));
		spinBox->setProperty(ATTRIBUTE_INITIAL_VALUE.c_str(), _defaultPresetValue);
	}
	if (!_templateType)
	{
		spinBox->setDisabled(true);
		spinBox->setStyleSheet(DISABLED_SPINBOX_STYLE);
	}
	return spinBox;
}

/*
* Description - CreateSpinBoxWidget() method used to create spin box widget.
* Parameter -  QString, string.
* Exception -
* Return - QSpinBox.
*/
QSpinBox* CVWidgetGenerator::CreateSpinBoxWidget(QString _defaultPresetValue, string _input)
{
	CustomSpinBox* spinBox = new CustomSpinBox();
	spinBox->setMaximumHeight(SPINBOX_HEIGHT);
	spinBox->setStyleSheet(SPINBOX_STYLE);
	spinBox->setRange(0, 10000000000000);
	spinBox->setPrefix(QString::fromStdString(_input));
	spinBox->setValue(0);
	spinBox->setFocusPolicy(Qt::StrongFocus);
	spinBox->setCorrectionMode(QAbstractSpinBox::CorrectToNearestValue);
	if (FormatHelper::HasContent(_defaultPresetValue.toStdString()))
	{
		spinBox->setValue(stoi(_defaultPresetValue.toStdString()));
		spinBox->setProperty(ATTRIBUTE_INITIAL_VALUE.c_str(), _defaultPresetValue);
	}
	return spinBox;
}

/*
* Description - CreateBooleanWidget() method used to override CreateBooleanWidget().
* Parameter -  string, bool, QTreeWidgetItem, int.
* Exception -
* Return - ComboBoxItem.
*/
ComboBoxItem* CVWidgetGenerator::CreateBooleanWidget(string _defaultValue, bool _templateType, QTreeWidgetItem* _item, int _val)
{
	return CreateBooleanWidget(QString::fromStdString(_defaultValue), _templateType, _item, _val);
}

/*
* Description - CreateBooleanWidget() method used to create boolean widget.
* Parameter -  QString, bool, QTreeWidgetItem, int.
* Exception -
* Return - ComboBoxItem
*/
ComboBoxItem* CVWidgetGenerator::CreateBooleanWidget(QString _defaultValue, bool _templateType, QTreeWidgetItem* _item, int _val)
{
	ComboBoxItem* comboBox = new ComboBoxItem();
	comboBox->setStyleSheet(COMBOBOX_STYLE);
	QStringList comboBoxItemList;
	comboBoxItemList << QString::fromStdString(BLANK) << QString::fromStdString(TrueValue) << QString::fromStdString(FalseValue);
	comboBox->addItems(comboBoxItemList);
	comboBox->setProperty(TrueValue.c_str(), QString::fromStdString(TrueValue));
	comboBox->setProperty(FalseValue.c_str(), QString::fromStdString(FalseValue));
	comboBox->setFocusPolicy(Qt::StrongFocus);

	if (FormatHelper::HasContent(_defaultValue.toStdString()))
	{
		comboBox->setProperty(ATTRIBUTE_INITIAL_VALUE.c_str(), _defaultValue);
		if (_defaultValue.contains(QString::fromStdString(DELIMITER_NEGATION)))
			_defaultValue = QString::fromStdString(Helper::ReplaceDelimeter(_defaultValue.toStdString(), DELIMITER_NEGATION, BLANK));

		if (_defaultValue.toStdString() == TrueValue || _defaultValue.toStdString() == TrueValueInt || _defaultValue.toStdString() == YesDisplayValue)
		{
			comboBox->setCurrentIndex(1);
		}
		if (_defaultValue.toStdString() == FalseValue || _defaultValue.toStdString() == FalseValueInt || _defaultValue.toStdString() == NoDisplayValue)
		{
			comboBox->setCurrentIndex(2);
		}
	}
	else
		comboBox->setCurrentIndex(0);

	if (!_templateType)
	{
		comboBox->setDisabled(true);
		comboBox->setFocusPolicy(Qt::NoFocus);
		comboBox->setStyleSheet(DISABLED_COMBOBOX_STYLE);
	}
	return comboBox;
}

/*
* Description - CreateComboBox() method used to create combo box.
* Parameter -  bool, QComboBox, int, QStringList.
* Exception -
* Return - QComboBox
*/
QComboBox* CVWidgetGenerator::CreateComboBox(bool _disable, QComboBox* _comboBox, int _defaultIndext, QStringList _itemList)
{
	_comboBox->clear();
	_comboBox->addItem(QString::fromStdString(BLANK));
	_comboBox->setStyleSheet(VIEW_COMBOBOX_STYLE);
    //_itemList.sort();
	_comboBox->addItems(_itemList);
	_comboBox->setCurrentIndex(_defaultIndext);
	_comboBox->setFocusPolicy(Qt::StrongFocus);

	if (_disable)
	{
		_comboBox->setDisabled(true);
		_comboBox->setStyleSheet(DISABLED_VIEW_COMBOBOX_STYLE);
	}
	return _comboBox;
}

/*
* Description - CreateComboBox() method used to create combo box.
* Parameter -  bool, QComboBox, int, QStringList.
* Exception -
* Return - QComboBox
*/
ComboBoxItem* CVWidgetGenerator::CreateComboBoxItem(bool _disable, ComboBoxItem* _comboBox, int _defaultIndext, QStringList& _itemList)
{
	_comboBox->clear();
	_comboBox->addItem(QString::fromStdString(BLANK));
	_comboBox->setStyleSheet(VIEW_COMBOBOX_STYLE);
	_itemList.sort();
	_comboBox->addItems(_itemList);
	_comboBox->setCurrentIndex(_defaultIndext);
	_comboBox->setFocusPolicy(Qt::StrongFocus);

	if (_disable)
	{
		_comboBox->setDisabled(true);
		_comboBox->setStyleSheet(DISABLED_VIEW_COMBOBOX_STYLE);
	}
	return _comboBox;
}

/*
* Description - DrawFilterAndSearchCriteriaWidget() method used to draw filter and search criteria widget.
* Parameter -  json, QComboBox, QTreeWidget, QString, string, QStringList, bool.
* Exception - exception, Char *
* Return - bool.
*/
bool CVWidgetGenerator::DrawFilterAndSearchCriteriaWidget(json _filterjson, QComboBox* _filterCombo, QTreeWidget* _searchTree_1, QTreeWidget* _searctTree_2, QString _selectedType, string _selectedFilter, QStringList _attScops, bool _drawFilterWid)
{
	bool isSearchCriteriaDrwan = false;
	string attributeFilterType = "";
	try
	{
		string selectFilter = "", typeName = "", filtersStr = "", filterCountOfFilterName = "", filterTemplateType = "",
			filterId = "", defaultFilter = "";
		json filterListJson = json::object();
		json filtersStrJson = json::object();
		json attributesJson = json::object();
		json filterCountJson = json::object();
		for (int filterCount = 0; filterCount < _filterjson.size(); filterCount++)
		{
			filterListJson = Helper::GetJSONParsedValue<int>(_filterjson, filterCount, false);
			typeName = Helper::GetJSONValue<string>(filterListJson, TYPENAME_JSON_KEY, true);

			if (_selectedType == QString::fromStdString(typeName))
			{

				if (_drawFilterWid)
				{
					defaultFilter = Helper::GetJSONValue<string>(filterListJson, "defaultFilter", true);
					CVWidgetGenerator::DrawFilterWidget(filterListJson, _filterCombo);
				}
				if (FormatHelper::HasContent(defaultFilter) || (!FormatHelper::HasContent(defaultFilter) && FormatHelper::HasContent(_selectedFilter)))
				{
					filtersStrJson = Helper::GetJSONParsedValue<string>(filterListJson, FILTERS_JSON_KEY, false);
					for (int fSCount = 0; fSCount < filtersStrJson.size(); fSCount++)
					{
						filterCountJson = Helper::GetJSONParsedValue<int>(filtersStrJson, fSCount, false);
						filterCountOfFilterName = Helper::GetJSONValue<string>(filterCountJson, FILTER_NAME_KEY, true);
						filterTemplateType = Helper::GetJSONValue<string>(filterCountJson, FILTER_TYPE_KEY, true);
						filterId = Helper::GetJSONValue<string>(filterCountJson, FILTER_ID_KEY, true);
						if (defaultFilter == filterCountOfFilterName || _selectedFilter == filterCountOfFilterName)
						{
							attributesJson = Helper::GetJSONParsedValue<string>(filterCountJson, FILTER_ATTRIBUTES_KEY, false);
							attributeFilterType = Helper::GetJSONValue<string>(filterCountJson, FILTER_TYPE_KEY, true);
							if (attributeFilterType == TEMPLATE_KEY)
							{
								CVWidgetGenerator::CreateSearchCriteriaWidget(_searchTree_1, _searctTree_2, attributesJson, false, true, _attScops);
								isSearchCriteriaDrwan = true;
								break;
							}
							else
							{

								CVWidgetGenerator::CreateSearchCriteriaWidget(_searchTree_1, _searctTree_2, attributesJson, false, false, _attScops);
								isSearchCriteriaDrwan = true;
								break;
							}

						}
					}
				}
				else if (!FormatHelper::HasContent(defaultFilter) && !FormatHelper::HasContent(_selectedFilter))
				{
					isSearchCriteriaDrwan = false;
				}
				break;
			}
		}
	}
	catch (string msg)
	{
		throw msg;
	}
	catch (const char* msg)
	{
		throw msg;
	}
	catch (exception& e)
	{
		throw e;
	}

	return isSearchCriteriaDrwan;
}

/*
* Description - DrawFilterWidget() method used to draw filter widget.
* Parameter -  json, QComboBox.
* Exception -
* Return -
*/
void CVWidgetGenerator::DrawFilterWidget(json _filterjson, QComboBox* _filterComboBox)
{
	QStringList filterNameList;
	string filterCountOfFilterName = "", filterCountOfFilterId = "";
	filterNameList.clear();
	int defaultFilterIndex = 0;
	json filterCountJson = json::object();
	json filterCountOfFilterJson = json::object();
	json filtersStrJson = json::object();
	string defaultFilter = Helper::GetJSONValue<string>(_filterjson, "defaultFilter", true);// "defaultFilter" read this from CVJsonConstence.h
	json filterNamesJson = Helper::GetJSONParsedValue<string>(_filterjson, "availableFilters", false);//call anothermenthod like 258.
	_filterComboBox->setFocusPolicy(Qt::StrongFocus);

	for (int filterNameCount = 0; filterNameCount < filterNamesJson.size(); filterNameCount++)
	{
		string filterName = filterNamesJson[filterNameCount];
		filtersStrJson = Helper::GetJSONParsedValue<string>(_filterjson, FILTERS_JSON_KEY, false);
		for (int fSCount = 0; fSCount < filtersStrJson.size(); fSCount++)
		{
			filterCountJson = Helper::GetJSONParsedValue<int>(filtersStrJson, fSCount, false);
			filterCountOfFilterName = Helper::GetJSONValue<string>(filterCountJson, FILTER_NAME_KEY, true);
			if (filterName == filterCountOfFilterName)
			{
				filterCountOfFilterId = Helper::GetJSONValue<string>(filterCountJson, FILTER_ID_KEY, true);
				_filterComboBox->setProperty(filterName.c_str(), QString::fromStdString(filterCountOfFilterId));

			}
		}
		filterNameList << QString::fromStdString(filterName);
		if (defaultFilter == filterName)
		{
			defaultFilterIndex = filterNameCount + 1;
		}
	}
	CVWidgetGenerator::CreateComboBox(false, _filterComboBox, defaultFilterIndex, filterNameList);
}

/*
* Description - CreateSearchCriteriaWidget() method used to create search criteria widget.
* Parameter -  QTreeWidget, json, bool, bool, QStringList.
* Exception -
* Return -
*/
void CVWidgetGenerator::CreateSearchCriteriaWidget(QTreeWidget* _searchTree_1, QTreeWidget* _searchTree_2, json _attributeJson, bool _noFilter, bool _isTemplateType, QStringList _attScops)
{

	Logger::Debug("CVWidgetGenerator CreateSearchCriteriaWidget() -> Start");
	//need to add defaultvalues in json
	_searchTree_1->clear();
	_searchTree_2->clear();

	if (_attributeJson.size() < 10)
	{
		UIHelper::CreateWidgetsOnTreeWidget(_searchTree_1, STARTING_INDEX, _attributeJson.size(), _attributeJson, _noFilter, _isTemplateType);
		_searchTree_2->setVisible(false);
	}
	else
	{
		UIHelper::CreateWidgetsOnTreeWidget(_searchTree_1, STARTING_INDEX, ceil(_attributeJson.size() / DIVISION_FACTOR), _attributeJson, _noFilter, _isTemplateType);
		UIHelper::CreateWidgetsOnTreeWidget(_searchTree_2, ceil(_attributeJson.size() / DIVISION_FACTOR), _attributeJson.size(), _attributeJson, _noFilter, _isTemplateType);
	}
	Logger::Debug("CVWidgetGenerator CreateSearchCriteriaWidget() -> End");
	//QStringList attributeDefaultValueList;
	//string attributevalue = "", attributeName = "", attributeType = "", attributeEnum = "", attributeEnumJson = "",
	//	attributeEnumVale = "", attributeKey = "", attributeEnumKey = "", attributeDecimals = "", attributeTypeDefalutValue = "", attributeCurrencyCode = "", attributeAttscop = "", attributeApiExposed= "";
	//bool isAttrSearchable = false;
	//bool isMultiselectable = false;
	//bool isCheckable = false;
	//bool isInteger = false;
	//bool isSingleFloat = false;
	//json attCountOfAttJson = json::object();
	//json attEnumListStrJson = json::object();
	//for (int attributeCount = 0; attributeCount < (_attributeJson.size()/2); attributeCount++)
	//{
	//	attributeDefaultValueList.clear();

	//	attCountOfAttJson = Helper::GetJSONParsedValue<int>(_attributeJson, attributeCount, false);
	//	attributeName = Helper::GetJSONValue<string>(attCountOfAttJson, ATTRIBUTE_NAME_KEY, true);
	//	attributeAttscop = Helper::GetJSONValue<string>(attCountOfAttJson, ATTRIBUTE_ATTSCOPE_KEY, true);
	//	attributeApiExposed = Helper::GetJSONValue<string>(attCountOfAttJson, ATTRIBUTE_REST_API_EXPOSED, true);
	//	if (attributeApiExposed == "false")
	//		continue;

	//	if ("true" == Helper::GetJSONValue<string>(attCountOfAttJson, IS_MULTISELECT_KEY, true))
	//		isMultiselectable = true;
	//	else 
	//		isMultiselectable = false;
	//	if ("true" == Helper::GetJSONValue<string>(attCountOfAttJson, IS_CHECK_KEY, true))
	//		isCheckable = true;
	//	else
	//		isCheckable = false;
	//	if ("true" == Helper::GetJSONValue<string>(attCountOfAttJson, IS_SINGLE_INTERGER_KEY, true))
	//		isInteger = true;
	//	else
	//		isInteger = false;
	//	if ("true" == Helper::GetJSONValue<string>(attCountOfAttJson, IS_SINGLE_FLOAT_KEY, true))
	//		isSingleFloat = true;
	//	else
	//		isSingleFloat = false;
	//	//UTILITY_API->DisplayMessageBox(" Helper::GetJSONValue<string>(attCountOfAttJson, IS_CHECK_KEY, true)::" + Helper::GetJSONValue<string>(attCountOfAttJson, IS_CHECK_KEY, true));
	//	//UTILITY_API->DisplayMessageBox(" Helper::GetJSONValue<string>(attCountOfAttJson, IS_MULTISELECT_KEY, true)::" + Helper::GetJSONValue<string>(attCountOfAttJson, IS_MULTISELECT_KEY, true));
	//	/*if (!_attScops.isEmpty() && !_attScops.contains(QString::fromStdString(attributeAttscop)))
	//	{
	//		continue;
	//	}*/
	//	attributeKey = Helper::GetJSONValue<string>(attCountOfAttJson, ATTRIBUTE_KEY, true);
	//	attributeType = Helper::GetJSONValue<string>(attCountOfAttJson, ATTRIBUTE_TYPE_KEY, true);
	//	//Continue if no supported atttibute type
	//	/*if (!Configuration::GetInstance()->GetSupportedAttsList().contains(QString::fromStdString(attributeType)))
	//	{
	//		Logger::Error("CVWidgetGenerator::CreateSearchCriteriaWidget() - attributeType:: not supported");
	//		continue;
	//	}*/
	//	attributeTypeDefalutValue = Helper::GetJSONValue<string>(attCountOfAttJson, ATTRIBUTE_VALUE_KEY, true);
	//	attributeDefaultValueList << QString::fromStdString(attributeTypeDefalutValue);
	//	isAttrSearchable = Helper::IsValueTrue(Helper::GetJSONValue<string>(attCountOfAttJson, ATTRIBUTE_SEARCHABLE_KEY, true));
	//	if ((attributeType == ENUM_ATT_TYPE_KEY && !isMultiselectable) || (attributeType == REF_ATT_TYPE_KEY && !isMultiselectable) || (attributeType == CHOICE_ATT_TYPE_KEY && !isMultiselectable))
	//	{
	//		QTreeWidgetItem* topLevel = new QTreeWidgetItem();  // Creating new TreeWidgetItem
	//		if (!_noFilter || _noFilter && isAttrSearchable)
	//		{
	//			json attributeEnumJson = Helper::GetJSONParsedValue<string>(attCountOfAttJson, ENUMLIST_JSON_KEY, false);
	//			QStringList valueList;
	//			QString selectedValue;
	//			ComboBoxItem* comboBox = new ComboBoxItem();
	//			comboBox->setFocusPolicy(Qt::StrongFocus);
	//			comboBox->setStyleSheet(COMBOBOX_STYLE);
	//			comboBox->addItem(QString::fromStdString(BLANK));
	//			for (int attEnumCount = 0; attEnumCount < attributeEnumJson.size(); attEnumCount++)
	//			{
	//				attEnumListStrJson = Helper::GetJSONParsedValue<int>(attributeEnumJson, attEnumCount, false);;///use new method
	//				attributeEnumVale = Helper::GetJSONValue<string>(attEnumListStrJson, ENUM_VALUE_KEY, true);
	//				attributeEnumKey = Helper::GetJSONValue<string>(attEnumListStrJson, ENUM_KEY, true);
	//				valueList.push_back(QString::fromStdString(attributeEnumVale));
	//				comboBox->setProperty(attributeEnumVale.c_str(), QString::fromStdString(attributeEnumKey));
	//				if (attributeTypeDefalutValue == attributeEnumKey) {
	//					selectedValue = QString::fromStdString(attributeEnumVale);
	//				}
	//			}
	//			if (!_isTemplateType)
	//			{
	//				comboBox->addItems(valueList);
	//				comboBox->setCurrentIndex(comboBox->findText(selectedValue));
	//				comboBox->setDisabled(true);
	//				comboBox->setStyleSheet(DISABLED_COMBOBOX_STYLE);
	//			}
	//			else
	//			{
	//				comboBox->addItems(valueList);
	//				comboBox->setCurrentIndex(comboBox->findText(selectedValue));
	//			}
	//			_searchTree_1->addTopLevelItem(topLevel);						// Adding ToplevelItem
	//			_searchTree_1->setItemWidget(topLevel, 0, CVWidgetGenerator::CreateLabelWidget(attributeName, attributeKey, attributeType));	// Adding label at column 1
	//			_searchTree_1->setItemWidget(topLevel, 1, comboBox);
	//		}
	//	}
	//	else if ((attributeType == ENUM_ATT_TYPE_KEY && isMultiselectable ) || (attributeType == REF_ATT_TYPE_KEY && isMultiselectable) || attributeType == CHOICE_ATT_TYPE_KEY || attributeType == MOA_LIST_ATT_TYPE_KEY || attributeType == DRIVEN_ATT_TYPE_KEY)
	//	{
	//		QTreeWidgetItem* topLevel = new QTreeWidgetItem();
	//		QListWidget* listWidget = new QListWidget();
	//		json attributeEnumJson = Helper::GetJSONParsedValue<string>(attCountOfAttJson, ENUMLIST_JSON_KEY, false);
	//		for (int attEnumCount = 0; attEnumCount < attributeEnumJson.size(); attEnumCount++)
	//		{
	//			attEnumListStrJson = Helper::GetJSONParsedValue<int>(attributeEnumJson, attEnumCount, false);;///use new method
	//			attributeEnumVale = Helper::GetJSONValue<string>(attEnumListStrJson, ENUM_VALUE_KEY, true);
	//			attributeEnumKey = Helper::GetJSONValue<string>(attEnumListStrJson, ENUM_KEY, true);
	//			CVWidgetGenerator::CreateCVMultiListWidget(listWidget, QString::fromStdString(attributeEnumVale), QString::fromStdString(attributeEnumKey), QString::fromStdString(attributeTypeDefalutValue));
	//		}

	//		listWidget->setFocusPolicy(Qt::NoFocus);
	//		listWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
	//		listWidget->setStyleSheet("QListWidget{border-bottom: 1px inset #262626; font-size: 10px; font-face: ArialMT; border-left: 1px inset #0D0D0D; max-height:70px; border-top: 1px inset #0D0D0D; border-right: 1px inset #262626; background-color: #222224; } QListWidget::item{ background-color: #222224; margin: 2px;} QListWidget::indicator { width: 10px; height:10px; } QListWidget::indicator:unchecked {background-color: #343434 ; border: 1px solid #3B3B3B;} QListWidget::indicator:checked {image: url(:/CLOVise/PLM/Images/icon_ok_over.svg);background-color: #343434; border: 1px solid #3B3B3B;}");
	//		if (!_noFilter || _noFilter && isAttrSearchable)
	//		{
	//			_searchTree_1->addTopLevelItem(topLevel);		// Adding ToplevelItem
	//			_searchTree_1->setItemWidget(topLevel, 0, CVWidgetGenerator::CreateLabelWidget(attributeName, attributeKey, attributeType));	// Adding label at column 1
	//			if (!_isTemplateType)
	//			{
	//				listWidget->setDisabled(true);
	//				listWidget->setStyleSheet("QListWidget{ color: #808080; border-bottom: 1px inset #262626; font-size: 10px; font-face: ArialMT; border-left: 1px inset #0D0D0D; max-height:70px; border-top: 1px inset #0D0D0D; border-right: 1px inset #262626; background-color: #222224; } QListWidget::item{ color: #808080; background-color: #222224; margin: 2px;} QListWidget::indicator { width: 10px; height:10px; } QListWidget::indicator:unchecked {background-color: #343434 ; border: 1px solid #3B3B3B;} QListWidget::indicator:checked {image: url(:/CLOVise/PLM/Images/icon_ok_over.svg);background-color: #343434; border: 1px solid #3B3B3B;}");
	//			}
	//			_searchTree_1->setItemWidget(topLevel, 1, listWidget);
	//		}
	//	}
	//	//else if (attributeType == TEXT_AREA_ATT_TYPE_KEY)
	//	//{
	//	//	QTreeWidgetItem* topLevel = new QTreeWidgetItem();  // Creating new TreeWidgetItem
	//	//	if (!_noFilter || _noFilter && isAttrSearchable)
	//	//	{
	//	//		_searchTree->addTopLevelItem(topLevel);				// Adding ToplevelItem
	//	//		_searchTree->setItemWidget(topLevel, 0, CVWidgetGenerator::CreateLabelWidget(attributeName, attributeKey, attributeType));	// Adding label at column 1
	//	//		_searchTree->setItemWidget(topLevel, 1, CVWidgetGenerator::CreateTextAreaWidget(attributeTypeDefalutValue, _isTemplateType));
	//	//	}
	//	//}
	//	else if (attributeType == DATE_ATT_TYPE_KEY || attributeType == TIME_ATT_TYPE_KEY)
	//	{
	//		ColorConfig::GetInstance()->SetDateFlag(true);
	//		MaterialConfig::GetInstance()->SetDateFlag(true);
	//		ProductConfig::GetInstance()->SetDateFlag(true);
	//		QStringList dateList;
	//		QTreeWidgetItem* topLevel = new QTreeWidgetItem();  // Creating new TreeWidgetItem
	//		if (!_noFilter || _noFilter && isAttrSearchable)
	//		{
	//			_searchTree_1->addTopLevelItem(topLevel);			// Adding ToplevelItem
	//			_searchTree_1->setItemWidget(topLevel, 0, CVWidgetGenerator::CreateLabelWidget(attributeName, attributeKey, attributeType));	// Adding label at column 1
	//			if (FormatHelper::HasContent(attributeTypeDefalutValue))
	//			{
	//				dateList = QString::fromStdString(attributeTypeDefalutValue).split(QString::fromStdString(DELIMITER_HYPHAN));

	//				for (int dateListCount = 0; dateListCount < dateList.size(); dateListCount++)
	//				{
	//					_searchTree_1->setItemWidget(topLevel, dateListCount + 1, CVWidgetGenerator::CreateDateWidget(dateList[dateListCount].toStdString(), _isTemplateType));
	//				}
	//			}
	//			else
	//			{
	//				_searchTree_1->setItemWidget(topLevel, 1, CVWidgetGenerator::CreateDateWidget(attributeTypeDefalutValue, _isTemplateType));
	//				//_searchTree->setItemWidget(topLevel, 2, CVWidgetGenerator::CreateDateWidget(attributeTypeDefalutValue, _isTemplateType));
	//			}

	//		}
	//	}
	//	else if (attributeType == INTEGER_ATT_TYPE_KEY || attributeType == CONSTANT_ATT_TYPE_KEY || attributeType == SEQUENCE_ATT_TYPE_KEY)
	//	{
	//		QTreeWidgetItem* topLevel = new QTreeWidgetItem();  // Creating new TreeWidgetItem
	//		_searchTree_1->addTopLevelItem(topLevel);
	//		if (isInteger)
	//		{			// Adding ToplevelItem
	//			//UTILITY_API->DisplayMessageBox("isInteger::" + to_string(isInteger));
	//			_searchTree_1->setItemWidget(topLevel, 0, CVWidgetGenerator::CreateLabelWidget(attributeName, attributeKey, attributeType));
	//			_searchTree_1->setItemWidget(topLevel, 1, CVWidgetGenerator::CreateSpinBoxWidget(QString::fromStdString(attributevalue), _isTemplateType, ""));
	//		}
	//		else if (!_noFilter || _noFilter && isAttrSearchable)
	//		{
	//			//_searchTree->addTopLevelItem(topLevel);						// Adding ToplevelItem
	//			_searchTree_1->setItemWidget(topLevel, 0, CVWidgetGenerator::CreateLabelWidget(attributeName, attributeKey, attributeType));	// Adding label at column 1
	//			if (FormatHelper::HasContent(attributeTypeDefalutValue))
	//			{
	//				QStringList list = QString::fromStdString(attributeTypeDefalutValue).split(QString::fromStdString(DELIMITER_NEGATION));

	//				QStringList intList = list[0].split(DELIMITER_HYPHAN.c_str());

	//				_searchTree_1->setItemWidget(topLevel, 1, CVWidgetGenerator::CreateSpinBoxWidget(intList[0], _isTemplateType, "FROM:"));
	//				_searchTree_1->setItemWidget(topLevel, 2, CVWidgetGenerator::CreateSpinBoxWidget(intList[1], _isTemplateType, "TO:"));
	//			}
	//			else
	//			{
	//				_searchTree_1->setItemWidget(topLevel, 1, CVWidgetGenerator::CreateSpinBoxWidget(QString::fromStdString(attributevalue), _isTemplateType, "FROM:"));
	//				_searchTree_1->setItemWidget(topLevel, 2, CVWidgetGenerator::CreateSpinBoxWidget(QString::fromStdString(attributevalue), _isTemplateType, "TO:"));
	//			}
	//		}
	//	}
	//	else if (attributeType == FLOAT_ATT_TYPE_KEY|| attributeType== DOUBLE_ATT_TYPE_KEY)
	//	{
	//		QTreeWidgetItem* topLevel = new QTreeWidgetItem();  // Creating new TreeWidgetItem
	//		_searchTree_1->addTopLevelItem(topLevel);						// Adding ToplevelItem
	//		
	//		if (isSingleFloat)
	//		{
	//			_searchTree_1->setItemWidget(topLevel, 0, CVWidgetGenerator::CreateLabelWidget(attributeName, attributeKey, attributeType));	// Adding label at column 1
	//			_searchTree_1->setItemWidget(topLevel, 1, CVWidgetGenerator::CreateDoubleSpinBoxWidget(QString::fromStdString(attributeTypeDefalutValue), _isTemplateType, "", "0", 0.0000, 10000000000000.0000));
	//		}
	//		else if (!_noFilter || _noFilter && isAttrSearchable)
	//		{
	//			//attributeDecimals = Helper::GetJSONValue<string>(attCountOfAttJson, DECIMAP_FIGURES, true);
	//			//_searchTree->addTopLevelItem(topLevel);						// Adding ToplevelItem
	//			_searchTree_1->setItemWidget(topLevel, 0, CVWidgetGenerator::CreateLabelWidget(attributeName, attributeKey, attributeType));	// Adding label at column 1
	//			if (FormatHelper::HasContent(attributeTypeDefalutValue))
	//			{
	//				QStringList floatList = QString::fromStdString(attributeTypeDefalutValue).split(QString::fromStdString(DELIMITER_NEGATION));
	//				QStringList list = floatList[0].split(DELIMITER_HYPHAN.c_str());
	//				_searchTree_1->setItemWidget(topLevel, 1, CVWidgetGenerator::CreateDoubleSpinBoxWidget(list[0], _isTemplateType, "From:", attributeDecimals, 0.0000, 10000000000000.0000));
	//				_searchTree_1->setItemWidget(topLevel, 2, CVWidgetGenerator::CreateDoubleSpinBoxWidget(list[1], _isTemplateType, "To:", attributeDecimals, 0.0000, 10000000000000.0000));
	//			}
	//			else
	//			{
	//				_searchTree_1->setItemWidget(topLevel, 1, CVWidgetGenerator::CreateDoubleSpinBoxWidget(QString::fromStdString(attributeTypeDefalutValue), _isTemplateType, "From:", attributeDecimals, 0.0000, 10000000000000.0000));
	//				_searchTree_1->setItemWidget(topLevel, 2, CVWidgetGenerator::CreateDoubleSpinBoxWidget(QString::fromStdString(attributeTypeDefalutValue), _isTemplateType, "To:", attributeDecimals, 0.0000, 10000000000000.0000));
	//			}
	//		}
	//	}
	//	else if (attributeType == CURRENCY_ATT_TYPE_KEY)
	//	{
	//		QTreeWidgetItem* topLevel = new QTreeWidgetItem();  // Creating new TreeWidgetItem
	//		if (!_noFilter || _noFilter && isAttrSearchable)
	//		{
	//			attributeDecimals = Helper::GetJSONValue<string>(attCountOfAttJson, DECIMAP_FIGURES, true);
	//			attributeCurrencyCode = Helper::GetJSONValue<string>(attCountOfAttJson, CURRENCY_CODE, true) + " ";
	//			_searchTree_1->addTopLevelItem(topLevel);						// Adding ToplevelItem
	//			_searchTree_1->setItemWidget(topLevel, 0, CVWidgetGenerator::CreateLabelWidget(attributeName, attributeKey, attributeType));	// Adding label at column 1
	//			if (FormatHelper::HasContent(attributeTypeDefalutValue))
	//			{
	//				QStringList currencyList = QString::fromStdString(attributeTypeDefalutValue).split(QString::fromStdString(DELIMITER_NEGATION));
	//				_searchTree_1->setItemWidget(topLevel, 1, CVWidgetGenerator::CreateDoubleSpinBoxWidget(currencyList[0], _isTemplateType, "From: " + attributeCurrencyCode, attributeDecimals, 0.00, 10000000000000.00));
	//				_searchTree_1->setItemWidget(topLevel, 2, CVWidgetGenerator::CreateDoubleSpinBoxWidget(currencyList[1], _isTemplateType, "To: " + attributeCurrencyCode, attributeDecimals, 0.00, 10000000000000.00));
	//			}
	//			else
	//			{
	//				_searchTree_1->setItemWidget(topLevel, 1, CVWidgetGenerator::CreateDoubleSpinBoxWidget(QString::fromStdString(attributeTypeDefalutValue), _isTemplateType, "From: " + attributeCurrencyCode, attributeDecimals, 0.00, 10000000000000.00));
	//				_searchTree_1->setItemWidget(topLevel, 2, CVWidgetGenerator::CreateDoubleSpinBoxWidget(QString::fromStdString(attributeTypeDefalutValue), _isTemplateType, "To: " + attributeCurrencyCode, attributeDecimals, 0.00, 10000000000000.00));
	//			}
	//		}
	//	}
	//	else if (attributeType == BOOLEAN_ATT_TYPE_KEY)
	//	{
	//		QTreeWidgetItem* topLevel = new QTreeWidgetItem();  // Creating new TreeWidgetItem
	//		_searchTree_1->addTopLevelItem(topLevel);
	//		//if (isCheckable)
	//		//{
	//		//	//UTILITY_API->DisplayMessageBox("comming inside isCheckable::" + to_string(isCheckable));
	//		//	QListWidget* listWidget = new QListWidget();
	//		//	_searchTree->addTopLevelItem(topLevel);						// Adding ToplevelItem
	//		//	_searchTree->setItemWidget(topLevel, 0, CVWidgetGenerator::CreateLabelWidget(attributeName, attributeKey, attributeType));
	//		//	QCheckBox* checkBox = new QCheckBox();
	//		//	_searchTree->setItemWidget(topLevel, 1, checkBox);
	//		//}
	//		//else 
	//		if (!_noFilter || _noFilter && isAttrSearchable)
	//		{
	//			//UTILITY_API->DisplayMessageBox("coming inside bolean3::" + Helper::GetJSONValue<string>(attCountOfAttJson, IS_CHECK_KEY, true));
	//			// Adding ToplevelItem
	//			_searchTree_1->setItemWidget(topLevel, 0, CVWidgetGenerator::CreateLabelWidget(attributeName, attributeKey, attributeType));	// Adding label at column 1
	//			_searchTree_1->setItemWidget(topLevel, 1, CVWidgetGenerator::CreateBooleanWidget(attributeTypeDefalutValue, _isTemplateType, topLevel, 1));	// Adding label at column 2
	//		}
	//	}
	//	else if (attributeType == USER_LIST_ATT_TYPE_KEY)
	//	{
	//		QTreeWidgetItem* topLevel = new QTreeWidgetItem();  // Creating new TreeWidgetItem
	//		if (!_noFilter || _noFilter && isAttrSearchable)
	//		{
	//			json attributeEnumJson = Helper::GetJSONParsedValue<string>(attCountOfAttJson, USER_LIST_ATT_TYPE_KEY, false);
	//			QStringList valueList;
	//			QString selectedValue;
	//			ComboBoxItem* comboBox = new ComboBoxItem();
	//			comboBox->setFocusPolicy(Qt::StrongFocus);
	//			comboBox->setStyleSheet(COMBOBOX_STYLE);
	//			comboBox->addItem(QString::fromStdString(BLANK));
	//			for (int attEnumCount = 0; attEnumCount < attributeEnumJson.size(); attEnumCount++)
	//			{
	//				attEnumListStrJson = Helper::GetJSONParsedValue<int>(attributeEnumJson, attEnumCount, false);;///use new method
	//				attributeEnumVale = Helper::GetJSONValue<string>(attEnumListStrJson, ENUM_VALUE_KEY, true);
	//				attributeEnumKey = Helper::GetJSONValue<string>(attEnumListStrJson, ENUM_KEY, true);
	//				valueList.push_back(QString::fromStdString(attributeEnumVale));
	//				comboBox->setProperty(attributeEnumVale.c_str(), QString::fromStdString(attributeEnumKey));
	//				if (attributeTypeDefalutValue == attributeEnumKey) {
	//					selectedValue = QString::fromStdString(attributeEnumVale);
	//				}
	//			}
	//			if (!_isTemplateType)
	//			{
	//				comboBox->addItems(valueList);
	//				comboBox->setCurrentIndex(comboBox->findText(selectedValue));
	//				comboBox->setDisabled(true);
	//				comboBox->setStyleSheet(DISABLED_COMBOBOX_STYLE);
	//			}
	//			else
	//			{
	//				comboBox->addItems(valueList);
	//				comboBox->setCurrentIndex(comboBox->findText(selectedValue));
	//			}
	//			_searchTree_1->addTopLevelItem(topLevel);						// Adding ToplevelItem
	//			_searchTree_1->setItemWidget(topLevel, 0, CVWidgetGenerator::CreateLabelWidget(attributeName, attributeKey, attributeType));	// Adding label at column 1
	//			_searchTree_1->setItemWidget(topLevel, 1, comboBox);
	//		}
	//	}
	//	else if (attributeType == TEXT_ATT_TYPE_KEY || attributeType == DERIVEDSTRING_ATT_TYPE_KEY || attributeType == STRING_ATT_TYPE_KEY || attributeType == TEXT_AREA_ATT_TYPE_KEY)
	//	{
	//		QTreeWidgetItem* topLevel = new QTreeWidgetItem();  // Creating new TreeWidgetItem
	//		if (!_noFilter || _noFilter && isAttrSearchable)
	//		{
	//			QLineEdit* LineEdit = new QLineEdit();	// Creating new LineEdit Widget
	//			LineEdit->setAttribute(Qt::WA_MacShowFocusRect, false);
	//			LineEdit->setStyleSheet(LINEEDIT_STYLE);
	//			_searchTree_1->addTopLevelItem(topLevel);						// Adding ToplevelItem
	//			_searchTree_1->setItemWidget(topLevel, 0, CVWidgetGenerator::CreateLabelWidget(attributeName, attributeKey, attributeType));	// Adding label at column 1

	//			if (FormatHelper::HasContent(attributeTypeDefalutValue))
	//			{
	//				QStringList defalut = QString::fromStdString(attributeTypeDefalutValue).split(QString::fromStdString(DELIMITER_NEGATION));
	//				LineEdit->setText(defalut[0]);
	//			}
	//			if (!_isTemplateType)
	//			{
	//				LineEdit->setDisabled(true);
	//				LineEdit->setStyleSheet(DISABLED_LINEEDIT_STYLE);
	//			}
	//			_searchTree_1->setItemWidget(topLevel, 1, LineEdit);
	//		}
	//	}
	//}

	//for (int attributeCount = (_attributeJson.size() / 2); attributeCount < _attributeJson.size(); attributeCount++)
	//{
	//	attributeDefaultValueList.clear();

	//	attCountOfAttJson = Helper::GetJSONParsedValue<int>(_attributeJson, attributeCount, false);
	//	attributeName = Helper::GetJSONValue<string>(attCountOfAttJson, ATTRIBUTE_NAME_KEY, true);
	//	attributeAttscop = Helper::GetJSONValue<string>(attCountOfAttJson, ATTRIBUTE_ATTSCOPE_KEY, true);
	//	attributeApiExposed = Helper::GetJSONValue<string>(attCountOfAttJson, ATTRIBUTE_REST_API_EXPOSED, true);
	//	if (attributeApiExposed == "false")
	//		continue;

	//	if ("true" == Helper::GetJSONValue<string>(attCountOfAttJson, IS_MULTISELECT_KEY, true))
	//		isMultiselectable = true;
	//	else
	//		isMultiselectable = false;
	//	if ("true" == Helper::GetJSONValue<string>(attCountOfAttJson, IS_CHECK_KEY, true))
	//		isCheckable = true;
	//	else
	//		isCheckable = false;
	//	if ("true" == Helper::GetJSONValue<string>(attCountOfAttJson, IS_SINGLE_INTERGER_KEY, true))
	//		isInteger = true;
	//	else
	//		isInteger = false;
	//	if ("true" == Helper::GetJSONValue<string>(attCountOfAttJson, IS_SINGLE_FLOAT_KEY, true))
	//		isSingleFloat = true;
	//	else
	//		isSingleFloat = false;
	//	//UTILITY_API->DisplayMessageBox(" Helper::GetJSONValue<string>(attCountOfAttJson, IS_CHECK_KEY, true)::" + Helper::GetJSONValue<string>(attCountOfAttJson, IS_CHECK_KEY, true));
	//	//UTILITY_API->DisplayMessageBox(" Helper::GetJSONValue<string>(attCountOfAttJson, IS_MULTISELECT_KEY, true)::" + Helper::GetJSONValue<string>(attCountOfAttJson, IS_MULTISELECT_KEY, true));
	//	/*if (!_attScops.isEmpty() && !_attScops.contains(QString::fromStdString(attributeAttscop)))
	//	{
	//		continue;
	//	}*/
	//	attributeKey = Helper::GetJSONValue<string>(attCountOfAttJson, ATTRIBUTE_KEY, true);
	//	attributeType = Helper::GetJSONValue<string>(attCountOfAttJson, ATTRIBUTE_TYPE_KEY, true);
	//	//Continue if no supported atttibute type
	//	/*if (!Configuration::GetInstance()->GetSupportedAttsList().contains(QString::fromStdString(attributeType)))
	//	{
	//		Logger::Error("CVWidgetGenerator::CreateSearchCriteriaWidget() - attributeType:: not supported");
	//		continue;
	//	}*/
	//	attributeTypeDefalutValue = Helper::GetJSONValue<string>(attCountOfAttJson, ATTRIBUTE_VALUE_KEY, true);
	//	attributeDefaultValueList << QString::fromStdString(attributeTypeDefalutValue);
	//	isAttrSearchable = Helper::IsValueTrue(Helper::GetJSONValue<string>(attCountOfAttJson, ATTRIBUTE_SEARCHABLE_KEY, true));
	//	if ((attributeType == ENUM_ATT_TYPE_KEY && !isMultiselectable) || (attributeType == REF_ATT_TYPE_KEY && !isMultiselectable) || (attributeType == CHOICE_ATT_TYPE_KEY && !isMultiselectable))
	//	{
	//		QTreeWidgetItem* topLevel = new QTreeWidgetItem();  // Creating new TreeWidgetItem
	//		if (!_noFilter || _noFilter && isAttrSearchable)
	//		{
	//			json attributeEnumJson = Helper::GetJSONParsedValue<string>(attCountOfAttJson, ENUMLIST_JSON_KEY, false);
	//			QStringList valueList;
	//			QString selectedValue;
	//			ComboBoxItem* comboBox = new ComboBoxItem();
	//			comboBox->setFocusPolicy(Qt::StrongFocus);
	//			comboBox->setStyleSheet(COMBOBOX_STYLE);
	//			comboBox->addItem(QString::fromStdString(BLANK));
	//			for (int attEnumCount = 0; attEnumCount < attributeEnumJson.size(); attEnumCount++)
	//			{
	//				attEnumListStrJson = Helper::GetJSONParsedValue<int>(attributeEnumJson, attEnumCount, false);;///use new method
	//				attributeEnumVale = Helper::GetJSONValue<string>(attEnumListStrJson, ENUM_VALUE_KEY, true);
	//				attributeEnumKey = Helper::GetJSONValue<string>(attEnumListStrJson, ENUM_KEY, true);
	//				valueList.push_back(QString::fromStdString(attributeEnumVale));
	//				comboBox->setProperty(attributeEnumVale.c_str(), QString::fromStdString(attributeEnumKey));
	//				if (attributeTypeDefalutValue == attributeEnumKey) {
	//					selectedValue = QString::fromStdString(attributeEnumVale);
	//				}
	//			}
	//			if (!_isTemplateType)
	//			{
	//				comboBox->addItems(valueList);
	//				comboBox->setCurrentIndex(comboBox->findText(selectedValue));
	//				comboBox->setDisabled(true);
	//				comboBox->setStyleSheet(DISABLED_COMBOBOX_STYLE);
	//			}
	//			else
	//			{
	//				comboBox->addItems(valueList);
	//				comboBox->setCurrentIndex(comboBox->findText(selectedValue));
	//			}
	//			_searchTree_2->addTopLevelItem(topLevel);						// Adding ToplevelItem
	//			_searchTree_2->setItemWidget(topLevel, 0, CVWidgetGenerator::CreateLabelWidget(attributeName, attributeKey, attributeType));	// Adding label at column 1
	//			_searchTree_2->setItemWidget(topLevel, 1, comboBox);
	//		}
	//	}
	//	else if ((attributeType == ENUM_ATT_TYPE_KEY && isMultiselectable) || (attributeType == REF_ATT_TYPE_KEY && isMultiselectable) || attributeType == CHOICE_ATT_TYPE_KEY || attributeType == MOA_LIST_ATT_TYPE_KEY || attributeType == DRIVEN_ATT_TYPE_KEY)
	//	{
	//		QTreeWidgetItem* topLevel = new QTreeWidgetItem();
	//		QListWidget* listWidget = new QListWidget();
	//		json attributeEnumJson = Helper::GetJSONParsedValue<string>(attCountOfAttJson, ENUMLIST_JSON_KEY, false);
	//		for (int attEnumCount = 0; attEnumCount < attributeEnumJson.size(); attEnumCount++)
	//		{
	//			attEnumListStrJson = Helper::GetJSONParsedValue<int>(attributeEnumJson, attEnumCount, false);;///use new method
	//			attributeEnumVale = Helper::GetJSONValue<string>(attEnumListStrJson, ENUM_VALUE_KEY, true);
	//			attributeEnumKey = Helper::GetJSONValue<string>(attEnumListStrJson, ENUM_KEY, true);
	//			CVWidgetGenerator::CreateCVMultiListWidget(listWidget, QString::fromStdString(attributeEnumVale), QString::fromStdString(attributeEnumKey), QString::fromStdString(attributeTypeDefalutValue));
	//		}

	//		listWidget->setFocusPolicy(Qt::NoFocus);
	//		listWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
	//		listWidget->setStyleSheet("QListWidget{border-bottom: 1px inset #262626; font-size: 10px; font-face: ArialMT; border-left: 1px inset #0D0D0D; max-height:70px; border-top: 1px inset #0D0D0D; border-right: 1px inset #262626; background-color: #222224; } QListWidget::item{ background-color: #222224; margin: 2px;} QListWidget::indicator { width: 10px; height:10px; } QListWidget::indicator:unchecked {background-color: #343434 ; border: 1px solid #3B3B3B;} QListWidget::indicator:checked {image: url(:/CLOVise/PLM/Images/icon_ok_over.svg);background-color: #343434; border: 1px solid #3B3B3B;}");
	//		if (!_noFilter || _noFilter && isAttrSearchable)
	//		{
	//			_searchTree_2->addTopLevelItem(topLevel);		// Adding ToplevelItem
	//			_searchTree_2->setItemWidget(topLevel, 0, CVWidgetGenerator::CreateLabelWidget(attributeName, attributeKey, attributeType));	// Adding label at column 1
	//			if (!_isTemplateType)
	//			{
	//				listWidget->setDisabled(true);
	//				listWidget->setStyleSheet("QListWidget{ color: #808080; border-bottom: 1px inset #262626; font-size: 10px; font-face: ArialMT; border-left: 1px inset #0D0D0D; max-height:70px; border-top: 1px inset #0D0D0D; border-right: 1px inset #262626; background-color: #222224; } QListWidget::item{ color: #808080; background-color: #222224; margin: 2px;} QListWidget::indicator { width: 10px; height:10px; } QListWidget::indicator:unchecked {background-color: #343434 ; border: 1px solid #3B3B3B;} QListWidget::indicator:checked {image: url(:/CLOVise/PLM/Images/icon_ok_over.svg);background-color: #343434; border: 1px solid #3B3B3B;}");
	//			}
	//			_searchTree_2->setItemWidget(topLevel, 1, listWidget);
	//		}
	//	}
	//	//else if (attributeType == TEXT_AREA_ATT_TYPE_KEY)
	//	//{
	//	//	QTreeWidgetItem* topLevel = new QTreeWidgetItem();  // Creating new TreeWidgetItem
	//	//	if (!_noFilter || _noFilter && isAttrSearchable)
	//	//	{
	//	//		_searchTree->addTopLevelItem(topLevel);				// Adding ToplevelItem
	//	//		_searchTree->setItemWidget(topLevel, 0, CVWidgetGenerator::CreateLabelWidget(attributeName, attributeKey, attributeType));	// Adding label at column 1
	//	//		_searchTree->setItemWidget(topLevel, 1, CVWidgetGenerator::CreateTextAreaWidget(attributeTypeDefalutValue, _isTemplateType));
	//	//	}
	//	//}
	//	else if (attributeType == DATE_ATT_TYPE_KEY || attributeType == TIME_ATT_TYPE_KEY)
	//	{
	//		ColorConfig::GetInstance()->SetDateFlag(true);
	//		MaterialConfig::GetInstance()->SetDateFlag(true);
	//		ProductConfig::GetInstance()->SetDateFlag(true);
	//		QStringList dateList;
	//		QTreeWidgetItem* topLevel = new QTreeWidgetItem();  // Creating new TreeWidgetItem
	//		if (!_noFilter || _noFilter && isAttrSearchable)
	//		{
	//			_searchTree_2->addTopLevelItem(topLevel);			// Adding ToplevelItem
	//			_searchTree_2->setItemWidget(topLevel, 0, CVWidgetGenerator::CreateLabelWidget(attributeName, attributeKey, attributeType));	// Adding label at column 1
	//			if (FormatHelper::HasContent(attributeTypeDefalutValue))
	//			{
	//				dateList = QString::fromStdString(attributeTypeDefalutValue).split(QString::fromStdString(DELIMITER_HYPHAN));

	//				for (int dateListCount = 0; dateListCount < dateList.size(); dateListCount++)
	//				{
	//					_searchTree_2->setItemWidget(topLevel, dateListCount + 1, CVWidgetGenerator::CreateDateWidget(dateList[dateListCount].toStdString(), _isTemplateType));
	//				}
	//			}
	//			else
	//			{
	//				_searchTree_2->setItemWidget(topLevel, 1, CVWidgetGenerator::CreateDateWidget(attributeTypeDefalutValue, _isTemplateType));
	//				//_searchTree->setItemWidget(topLevel, 2, CVWidgetGenerator::CreateDateWidget(attributeTypeDefalutValue, _isTemplateType));
	//			}

	//		}
	//	}
	//	else if (attributeType == INTEGER_ATT_TYPE_KEY || attributeType == CONSTANT_ATT_TYPE_KEY || attributeType == SEQUENCE_ATT_TYPE_KEY)
	//	{
	//		QTreeWidgetItem* topLevel = new QTreeWidgetItem();  // Creating new TreeWidgetItem
	//		_searchTree_2->addTopLevelItem(topLevel);
	//		if (isInteger)
	//		{			// Adding ToplevelItem
	//			//UTILITY_API->DisplayMessageBox("isInteger::" + to_string(isInteger));
	//			_searchTree_2->setItemWidget(topLevel, 0, CVWidgetGenerator::CreateLabelWidget(attributeName, attributeKey, attributeType));
	//			_searchTree_2->setItemWidget(topLevel, 1, CVWidgetGenerator::CreateSpinBoxWidget(QString::fromStdString(attributevalue), _isTemplateType, ""));
	//		}
	//		else if (!_noFilter || _noFilter && isAttrSearchable)
	//		{
	//			//_searchTree->addTopLevelItem(topLevel);						// Adding ToplevelItem
	//			_searchTree_2->setItemWidget(topLevel, 0, CVWidgetGenerator::CreateLabelWidget(attributeName, attributeKey, attributeType));	// Adding label at column 1
	//			if (FormatHelper::HasContent(attributeTypeDefalutValue))
	//			{
	//				QStringList list = QString::fromStdString(attributeTypeDefalutValue).split(QString::fromStdString(DELIMITER_NEGATION));

	//				QStringList intList = list[0].split(DELIMITER_HYPHAN.c_str());

	//				_searchTree_2->setItemWidget(topLevel, 1, CVWidgetGenerator::CreateSpinBoxWidget(intList[0], _isTemplateType, "FROM:"));
	//				_searchTree_2->setItemWidget(topLevel, 2, CVWidgetGenerator::CreateSpinBoxWidget(intList[1], _isTemplateType, "TO:"));
	//			}
	//			else
	//			{
	//				_searchTree_2->setItemWidget(topLevel, 1, CVWidgetGenerator::CreateSpinBoxWidget(QString::fromStdString(attributevalue), _isTemplateType, "FROM:"));
	//				_searchTree_2->setItemWidget(topLevel, 2, CVWidgetGenerator::CreateSpinBoxWidget(QString::fromStdString(attributevalue), _isTemplateType, "TO:"));
	//			}
	//		}
	//	}
	//	else if (attributeType == FLOAT_ATT_TYPE_KEY || attributeType == DOUBLE_ATT_TYPE_KEY)
	//	{
	//		QTreeWidgetItem* topLevel = new QTreeWidgetItem();  // Creating new TreeWidgetItem
	//		_searchTree_2->addTopLevelItem(topLevel);						// Adding ToplevelItem

	//		if (isSingleFloat)
	//		{
	//			_searchTree_2->setItemWidget(topLevel, 0, CVWidgetGenerator::CreateLabelWidget(attributeName, attributeKey, attributeType));	// Adding label at column 1
	//			_searchTree_2->setItemWidget(topLevel, 1, CVWidgetGenerator::CreateDoubleSpinBoxWidget(QString::fromStdString(attributeTypeDefalutValue), _isTemplateType, "", "0", 0.0000, 10000000000000.0000));
	//		}
	//		else if (!_noFilter || _noFilter && isAttrSearchable)
	//		{
	//			//attributeDecimals = Helper::GetJSONValue<string>(attCountOfAttJson, DECIMAP_FIGURES, true);
	//			//_searchTree->addTopLevelItem(topLevel);						// Adding ToplevelItem
	//			_searchTree_2->setItemWidget(topLevel, 0, CVWidgetGenerator::CreateLabelWidget(attributeName, attributeKey, attributeType));	// Adding label at column 1
	//			if (FormatHelper::HasContent(attributeTypeDefalutValue))
	//			{
	//				QStringList floatList = QString::fromStdString(attributeTypeDefalutValue).split(QString::fromStdString(DELIMITER_NEGATION));
	//				QStringList list = floatList[0].split(DELIMITER_HYPHAN.c_str());
	//				_searchTree_2->setItemWidget(topLevel, 1, CVWidgetGenerator::CreateDoubleSpinBoxWidget(list[0], _isTemplateType, "From:", attributeDecimals, 0.0000, 10000000000000.0000));
	//				_searchTree_2->setItemWidget(topLevel, 2, CVWidgetGenerator::CreateDoubleSpinBoxWidget(list[1], _isTemplateType, "To:", attributeDecimals, 0.0000, 10000000000000.0000));
	//			}
	//			else
	//			{
	//				_searchTree_2->setItemWidget(topLevel, 1, CVWidgetGenerator::CreateDoubleSpinBoxWidget(QString::fromStdString(attributeTypeDefalutValue), _isTemplateType, "From:", attributeDecimals, 0.0000, 10000000000000.0000));
	//				_searchTree_2->setItemWidget(topLevel, 2, CVWidgetGenerator::CreateDoubleSpinBoxWidget(QString::fromStdString(attributeTypeDefalutValue), _isTemplateType, "To:", attributeDecimals, 0.0000, 10000000000000.0000));
	//			}
	//		}
	//	}
	//	else if (attributeType == CURRENCY_ATT_TYPE_KEY)
	//	{
	//		QTreeWidgetItem* topLevel = new QTreeWidgetItem();  // Creating new TreeWidgetItem
	//		if (!_noFilter || _noFilter && isAttrSearchable)
	//		{
	//			attributeDecimals = Helper::GetJSONValue<string>(attCountOfAttJson, DECIMAP_FIGURES, true);
	//			attributeCurrencyCode = Helper::GetJSONValue<string>(attCountOfAttJson, CURRENCY_CODE, true) + " ";
	//			_searchTree_2->addTopLevelItem(topLevel);						// Adding ToplevelItem
	//			_searchTree_2->setItemWidget(topLevel, 0, CVWidgetGenerator::CreateLabelWidget(attributeName, attributeKey, attributeType));	// Adding label at column 1
	//			if (FormatHelper::HasContent(attributeTypeDefalutValue))
	//			{
	//				QStringList currencyList = QString::fromStdString(attributeTypeDefalutValue).split(QString::fromStdString(DELIMITER_NEGATION));
	//				_searchTree_2->setItemWidget(topLevel, 1, CVWidgetGenerator::CreateDoubleSpinBoxWidget(currencyList[0], _isTemplateType, "From: " + attributeCurrencyCode, attributeDecimals, 0.00, 10000000000000.00));
	//				_searchTree_2->setItemWidget(topLevel, 2, CVWidgetGenerator::CreateDoubleSpinBoxWidget(currencyList[1], _isTemplateType, "To: " + attributeCurrencyCode, attributeDecimals, 0.00, 10000000000000.00));
	//			}
	//			else
	//			{
	//				_searchTree_2->setItemWidget(topLevel, 1, CVWidgetGenerator::CreateDoubleSpinBoxWidget(QString::fromStdString(attributeTypeDefalutValue), _isTemplateType, "From: " + attributeCurrencyCode, attributeDecimals, 0.00, 10000000000000.00));
	//				_searchTree_2->setItemWidget(topLevel, 2, CVWidgetGenerator::CreateDoubleSpinBoxWidget(QString::fromStdString(attributeTypeDefalutValue), _isTemplateType, "To: " + attributeCurrencyCode, attributeDecimals, 0.00, 10000000000000.00));
	//			}
	//		}
	//	}
	//	else if (attributeType == BOOLEAN_ATT_TYPE_KEY)
	//	{
	//		QTreeWidgetItem* topLevel = new QTreeWidgetItem();  // Creating new TreeWidgetItem
	//		_searchTree_2->addTopLevelItem(topLevel);
	//		//if (isCheckable)
	//		//{
	//		//	//UTILITY_API->DisplayMessageBox("comming inside isCheckable::" + to_string(isCheckable));
	//		//	QListWidget* listWidget = new QListWidget();
	//		//	_searchTree->addTopLevelItem(topLevel);						// Adding ToplevelItem
	//		//	_searchTree->setItemWidget(topLevel, 0, CVWidgetGenerator::CreateLabelWidget(attributeName, attributeKey, attributeType));
	//		//	QCheckBox* checkBox = new QCheckBox();
	//		//	_searchTree->setItemWidget(topLevel, 1, checkBox);
	//		//}
	//		//else 
	//		if (!_noFilter || _noFilter && isAttrSearchable)
	//		{
	//			//UTILITY_API->DisplayMessageBox("coming inside bolean3::" + Helper::GetJSONValue<string>(attCountOfAttJson, IS_CHECK_KEY, true));
	//			// Adding ToplevelItem
	//			_searchTree_2->setItemWidget(topLevel, 0, CVWidgetGenerator::CreateLabelWidget(attributeName, attributeKey, attributeType));	// Adding label at column 1
	//			_searchTree_2->setItemWidget(topLevel, 1, CVWidgetGenerator::CreateBooleanWidget(attributeTypeDefalutValue, _isTemplateType, topLevel, 1));	// Adding label at column 2
	//		}
	//	}
	//	else if (attributeType == USER_LIST_ATT_TYPE_KEY)
	//	{
	//		QTreeWidgetItem* topLevel = new QTreeWidgetItem();  // Creating new TreeWidgetItem
	//		if (!_noFilter || _noFilter && isAttrSearchable)
	//		{
	//			json attributeEnumJson = Helper::GetJSONParsedValue<string>(attCountOfAttJson, USER_LIST_ATT_TYPE_KEY, false);
	//			QStringList valueList;
	//			QString selectedValue;
	//			ComboBoxItem* comboBox = new ComboBoxItem();
	//			comboBox->setFocusPolicy(Qt::StrongFocus);
	//			comboBox->setStyleSheet(COMBOBOX_STYLE);
	//			comboBox->addItem(QString::fromStdString(BLANK));
	//			for (int attEnumCount = 0; attEnumCount < attributeEnumJson.size(); attEnumCount++)
	//			{
	//				attEnumListStrJson = Helper::GetJSONParsedValue<int>(attributeEnumJson, attEnumCount, false);;///use new method
	//				attributeEnumVale = Helper::GetJSONValue<string>(attEnumListStrJson, ENUM_VALUE_KEY, true);
	//				attributeEnumKey = Helper::GetJSONValue<string>(attEnumListStrJson, ENUM_KEY, true);
	//				valueList.push_back(QString::fromStdString(attributeEnumVale));
	//				comboBox->setProperty(attributeEnumVale.c_str(), QString::fromStdString(attributeEnumKey));
	//				if (attributeTypeDefalutValue == attributeEnumKey) {
	//					selectedValue = QString::fromStdString(attributeEnumVale);
	//				}
	//			}
	//			if (!_isTemplateType)
	//			{
	//				comboBox->addItems(valueList);
	//				comboBox->setCurrentIndex(comboBox->findText(selectedValue));
	//				comboBox->setDisabled(true);
	//				comboBox->setStyleSheet(DISABLED_COMBOBOX_STYLE);
	//			}
	//			else
	//			{
	//				comboBox->addItems(valueList);
	//				comboBox->setCurrentIndex(comboBox->findText(selectedValue));
	//			}
	//			_searchTree_2->addTopLevelItem(topLevel);						// Adding ToplevelItem
	//			_searchTree_2->setItemWidget(topLevel, 0, CVWidgetGenerator::CreateLabelWidget(attributeName, attributeKey, attributeType));	// Adding label at column 1
	//			_searchTree_2->setItemWidget(topLevel, 1, comboBox);
	//		}
	//	}
	//	else if (attributeType == TEXT_ATT_TYPE_KEY || attributeType == DERIVEDSTRING_ATT_TYPE_KEY || attributeType == STRING_ATT_TYPE_KEY || attributeType == TEXT_AREA_ATT_TYPE_KEY)
	//	{
	//		QTreeWidgetItem* topLevel = new QTreeWidgetItem();  // Creating new TreeWidgetItem
	//		if (!_noFilter || _noFilter && isAttrSearchable)
	//		{
	//			QLineEdit* LineEdit = new QLineEdit();	// Creating new LineEdit Widget
	//			LineEdit->setAttribute(Qt::WA_MacShowFocusRect, false);
	//			LineEdit->setStyleSheet(LINEEDIT_STYLE);
	//			_searchTree_2->addTopLevelItem(topLevel);						// Adding ToplevelItem
	//			_searchTree_2->setItemWidget(topLevel, 0, CVWidgetGenerator::CreateLabelWidget(attributeName, attributeKey, attributeType));	// Adding label at column 1

	//			if (FormatHelper::HasContent(attributeTypeDefalutValue))
	//			{
	//				QStringList defalut = QString::fromStdString(attributeTypeDefalutValue).split(QString::fromStdString(DELIMITER_NEGATION));
	//				LineEdit->setText(defalut[0]);
	//			}
	//			if (!_isTemplateType)
	//			{
	//				LineEdit->setDisabled(true);
	//				LineEdit->setStyleSheet(DISABLED_LINEEDIT_STYLE);
	//			}
	//			_searchTree_2->setItemWidget(topLevel, 1, LineEdit);
	//		}
	//	}
	//}
}

/*
* Description - CreateViewComboBoxOnSearch() method used to create view combo box on search UI.
* Parameter -  QComboBox, json, string, int.
* Exception -
* Return -
*/
void CVWidgetGenerator::CreateViewComboBoxOnSearch(QComboBox* _viewCB, json _viewJSON, string _selectedTypeName, int _selectedViewIdx)
{
	Logger::Info("CVWidgetGenerator::CreateViewComboBoxOnSearch() - Start");
	CreateViewComboBoxOnSearch(_viewCB, _viewJSON, _selectedTypeName);
	_viewCB->setCurrentIndex(_selectedViewIdx);
	Logger::Info("CVWidgetGenerator::CreateViewComboBoxOnSearch() - End");
}

/*
* Description - CreateViewComboBoxOnSearch() method used to create view combo box on search UI.
* Parameter -  QComboBox, json, string.
* Exception -
* Return -
*/
void CVWidgetGenerator::CreateViewComboBoxOnSearch(QComboBox* _viewCB, json _viewsListArray, string _selectedTypeName)
{
	Logger::Info("CVWidgetGenerator::CreateViewComboBoxAndPopulateViews() - Start");
	QStringList viewsListQStr;
	viewsListQStr << QString::fromStdString("");

	for (int i = 0; i < _viewsListArray.size(); i++)
	{
		string viewListStr = Helper::GetJSONValue<int>(_viewsListArray, i, false);
		json viewListJson = json::parse(viewListStr);
		string currentTypeName = Helper::GetJSONValue<string>(viewListJson, TYPENAME_JSON_KEY, true);
		Logger::Debug("CVWidgetGenerator::CreateViewComboBoxAndPopulateViews() - currentTypeName" + currentTypeName);
		Logger::Debug("CVWidgetGenerator::CreateViewComboBoxAndPopulateViews() - selectedTypeName" + _selectedTypeName);
		if (currentTypeName == _selectedTypeName)
		{
			string defaultViewValue = "";
			string defaultView = Helper::GetJSONValue<string>(viewListJson, DEFAULT_VIEW_JSON_KEY, true);
			string availableViewsStr = Helper::GetJSONValue<string>(viewListJson, AVAILABLE_VIEWS_JSON_KEY, false);
			json availableViewsJson = json::parse(availableViewsStr);
			for (int i = 0; i < availableViewsJson.size(); i++)
			{
				string viewStr = Helper::GetJSONValue<int>(availableViewsJson, i, true);
				if (defaultView == viewStr)
					defaultViewValue = viewStr;
				Logger::Debug("viewStr::" + viewStr);
				viewsListQStr << QString::fromStdString(viewStr);
			}
			viewsListQStr.sort();
			_viewCB->clear();
			_viewCB->addItems(viewsListQStr);
			_viewCB->setCurrentIndex(viewsListQStr.indexOf(QString::fromStdString(defaultViewValue)));
			break;
		}
	}
	Logger::Info("CVWidgetGenerator::CreateViewComboBoxAndPopulateViews() - End");
}

/*
* Description - InitializeIconView() method used to initialize icon view.
* Parameter -  QListWidget.
* Exception -
* Return -
*/
void CVWidgetGenerator::InitializeIconView(QListWidget* _iconWidget)
{
	_iconWidget->setStyleSheet("QListWidget { background-color: #262628; border-right: 1px solid #000000; border-top: 1px solid #000000; border-left: 1px solid #000000; }""QToolTip { color: #46C8FF; background-color: #33414D; border: 1px #000000; }""QListWidget::item:selected{ background-color: #33414D; color: #46C8FF }""QListWidget::item {padding: 10px 10px 5px 10px;}""QScrollBar::add-page:vertical { background: #000000; }""QScrollBar::sub-page:vertical {background: #000000;}""QScrollBar::add-page:horizontal { background: #000000; }""QScrollBar::sub-page:horizontal {background: #000000;}""QListWidget::item:hover{ background-color: #33414D; color: #46C8FF } QListView { outline: 0; }");	//"QListWidget::item{ spacing: 5px;}"margin:

	//_iconWidget->setStyleSheet("QListWidget { border-bottom: 1px inset #262626; font-size: 10px; font-face: ArialMT; border-left: 1px inset #0D0D0D; max-height:90px; border-top: 1px inset #0D0D0D; border-right: 1px inset #262626; background-color: #222224; outline: 0; } QListView { outline: 0; } QListWidget::item{ background-color: #222224; margin: 2px;} QListWidget::item:hover { background-color: #222224; } QListWidget::indicator { width: 10px; height:10px; } QListWidget::indicator:unchecked {background-color: #343434 ; border: 1px solid #3B3B3B;} QListWidget::indicator:checked {image: url(:/CLOVise/FlexPLM/Images/icon_ok_over.svg);background-color: #343434; border: 1px solid #3B3B3B;}");
	_iconWidget->setViewMode(QListView::ListMode);
	_iconWidget->setResizeMode(QListView::Adjust);
	_iconWidget->setFlow(QListView::TopToBottom);
	_iconWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
	_iconWidget->setSelectionMode(QAbstractItemView::SingleSelection);
	_iconWidget->setDragDropMode(QAbstractItemView::NoDragDrop);
	_iconWidget->setWordWrap(true);
	_iconWidget->setTextElideMode(Qt::ElideRight);
	_iconWidget->setContentsMargins(20, 20, 20, 20);
	_iconWidget->setIconSize(QSize(100, 100));
	_iconWidget->setSpacing(10);
	/*_iconWidget->setMovement(QListView::Static);*/
}

/*
* Description - InitializeTableView() method used to initialize table view.
* Parameter -  MVTableWidget.
* Exception -
* Return -
*/
void CVWidgetGenerator::InitializeTableView(MVTableWidget* _resultTable)
{
	auto button = _resultTable->findChild<QAbstractButton*>();
	if (button)
	{
		QVBoxLayout* lay = new QVBoxLayout(button);
		lay->setContentsMargins(0, 0, 0, 0);
		QLabel* label = new QLabel("No.");
		label->setStyleSheet("QLabel {font-face: ArialMT; font-size: 10px; color: #FFFFFF; margin: 2px 2px 2px 2px; }""QToolTip { color: #46C8FF; background-color: #33414D; border: 1px #000000; }");
		label->setAlignment(Qt::AlignCenter);
		label->setToolTip("No.");
		lay->addWidget(label);
	}
	QLabel* label = new QLabel;
	_resultTable->setCornerWidget(label);
	label->setStyleSheet("QLabel{background-color: #262628}");
	_resultTable->horizontalHeader()->setStyleSheet("QHeaderView { font-face: ArialMT; font-size: 10px; color: #FFFFFF;}" "QHeaderView::section:horizontal{border: 1px solid #000000; background-color:#262628;}"
		"QHeaderView::down-arrow { image: url(:/CLOVise/PLM/Images/ui_spin_icon_minus_over.svg); width: 18px; height: 18px; }"
		"QHeaderView::up-arrow { image: url(:/CLOVise/PLM/Images/ui_spin_icon_plus_over.svg); width: 18px; height: 18px; }");
	_resultTable->horizontalHeader()->setHighlightSections(false);
	_resultTable->setCornerButtonEnabled(false);
	_resultTable->setWordWrap(true);
	_resultTable->verticalHeader()->setStyleSheet("QHeaderView { font-face: ArialMT; font-size: 10px; color: #FFFFFF; min-width: 30px; }""QHeaderView::section:vertical{ background-color:#262628; border-right: 0.5px solid #262628; border-bottom: 0.5px solid #000000; border-top: 0.2px solid #000000; border-left: 0.5px solid #000000; min-width: 30px;}");
	_resultTable->verticalHeader()->setHighlightSections(false);
	_resultTable->setShowGrid(false);
	_resultTable->setStyleSheet("QTableWidget{ background-color: #262628; border-right: 1px solid #000000; border-top: 1px solid #000000; border-left: 1px solid #000000; font-face: ArialMT; font-size: 12px; color: #FFFFFF; }"
		"QTableCornerButton::section{border: 1px solid #000000; background-color: #262628; }""QTableWidget::item{ border-bottom: 2px solid #000000; font-face: ArialMT; font-size: 12px; color: #FFFFFF; }"
		"QToolTip{ color: #46C8FF; background-color: #33414D; border: 1px #000000; }""QTableWidget::item:selected{ background-color: #33414D; color: #46C8FF; }""QScrollBar::add-page:vertical { background: #000000; }"
		"QScrollBar::sub-page:vertical {background: #000000;}""QScrollBar::add-page:horizontal { background: #000000; }""QScrollBar::sub-page:horizontal { background: #000000; }");
	_resultTable->setWordWrap(true);
	_resultTable->horizontalHeader()->setDefaultAlignment(Qt::AlignCenter | (Qt::Alignment)Qt::TextWordWrap);

	_resultTable->verticalHeader()->setDefaultAlignment(Qt::AlignCenter);
	_resultTable->verticalHeader()->setDefaultSectionSize(VERTICAL_HEADER);
	_resultTable->setSelectionBehavior(QAbstractItemView::SelectRows);
	_resultTable->horizontalHeader()->setStretchLastSection(true);
	_resultTable->horizontalHeader()->setMinimumHeight(HORIZONTAL_HEADER_HEIGHT);
	_resultTable->setAutoScroll(false);
	_resultTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
	_resultTable->setSortingEnabled(true);
	_resultTable->horizontalHeader()->setTextElideMode(Qt::ElideRight);
	QPalette pal;
	pal.setColor(QPalette::Highlight, highlightColor);
	pal.setColor(QPalette::Text, DEFAULT_TEXT_COLOR);
	pal.setColor(QPalette::HighlightedText, highlightedTextColor);
	_resultTable->setPalette(pal);

}

/*
* Description - CreateViewComboBoxOnResults() method used to create view combo box on results table.
* Parameter -  MVTableWidget, QComboBox, json, string, string, string, string, string, QStringList, string, bool.
* Exception -
* Return - QStringList
*/
QStringList CVWidgetGenerator::CreateViewComboBoxOnResults(MVTableWidget* _resultTable, json _viewJSON, string _selectedTypeName, string _selectedView, string _thubmnailDisplayName, string _inlcudePalette, string _includeSeason, QStringList _attScopes, string _module, bool _isProductExist)
{
	Logger::Info("CVWidgetGenerator::CreateViewComboBoxOnResults() - Start");

	InitializeTableView(_resultTable);
	QStringList headerList;
	headerList << "Select" << QString::fromStdString(_thubmnailDisplayName);
	if (_inlcudePalette == "true")
	{
		headerList << "Palette";
	}
	if (_includeSeason == "true")
	{
		headerList << "Season";
	}
	if (DOCUMENT_MODULE == _module)
	{
		headerList << "Product Name";
		if (_isProductExist)
			headerList << "Product Id";
	}
	QStringList headerKeyList;
	Logger::Debug("CVWidgetGenerator::CreateViewComboBoxOnResults() - selectedView::" + _selectedView);
	Logger::Debug("CVWidgetGenerator::CreateViewComboBoxOnResults() - selectedTypeName::" + _selectedTypeName);
	if (FormatHelper::HasContent(_selectedView))
	{
		for (int i = 0; i < _viewJSON.size(); i++)
		{
			string viewListStr = Helper::GetJSONValue<int>(_viewJSON, i, false);
			json viewListJson = json::parse(viewListStr);
			string currentTypeName = Helper::GetJSONValue<string>(viewListJson, TYPENAME_JSON_KEY, true);
			Logger::Debug("CVWidgetGenerator::CreateViewComboBoxOnResults() - currentTypeName::" + currentTypeName);
			if (currentTypeName == _selectedTypeName)
			{
				string viewsStr = Helper::GetJSONValue<string>(viewListJson, VIEWS_JSON_KEY, false);
				json viewsJson = json::parse(viewsStr);
				for (int i = 0; i < viewsJson.size(); i++)
				{
					string viewStr = Helper::GetJSONValue<int>(viewsJson, i, false);
					json viewJson = json::parse(viewStr);

					string viewName = Helper::GetJSONValue<string>(viewJson, VIEW_NAME_JSON_KEY, true);
					if (viewName == _selectedView)
					{
						Logger::Debug("CVWidgetGenerator::CreateViewComboBoxOnResults() - viewName::" + viewName);
						string fieldListstr = Helper::GetJSONValue<string>(viewJson, FIELDS_LIST_JSON_KEY, false);
						json viewfieldListJsonArray = json::parse(fieldListstr);
						for (int j = 0; j < viewfieldListJsonArray.size(); j++)
						{
							string fieldsStr = Helper::GetJSONValue<int>(viewfieldListJsonArray, j, false);
							Logger::Debug("CVWidgetGenerator::CreateViewComboBoxOnResults() - fieldsStr::" + fieldsStr);
							json fieldsJson = json::parse(fieldsStr);

							string headerInternalName = Helper::GetJSONValue<string>(fieldsJson, ATTRIBUTE_KEY, true);
							string attScope = Helper::GetJSONValue<string>(fieldsJson, ATTSCOPE_KEY, true);
							string attributeType = Helper::GetJSONValue<string>(fieldsJson, ATTRIBUTE_TYPE_KEY, true);
							/*if (!Configuration::GetInstance()->GetResultsSupportedAttsList().contains(QString::fromStdString(attributeType)))
							{
								Logger::Error("CVWidgetGenerator::DrawViewAndResultsWidget() - attributeType:: not supported");
								continue;
							}
							Logger::Debug("CVWidgetGenerator::CreateViewComboBoxOnResults() - attScope::" + attScope);
							if (!_attScopes.isEmpty() && !_attScopes.contains(QString::fromStdString(attScope)))
							{
								continue;
							}*/
							Logger::Debug("CVWidgetGenerator::CreateViewComboBoxOnResults() Inside If");
							//continue;
							headerKeyList << QString::fromStdString(headerInternalName);
							string headerDisplaylName = Helper::GetJSONValue<string>(fieldsJson, ATTRIBUTE_NAME_KEY, true);
							headerList << QString::fromStdString(headerDisplaylName);
							Logger::Debug("CVWidgetGenerator::CreateViewComboBoxOnResults() - headerInternalName::" + headerInternalName);
						}
						break;
					}
				}
				_resultTable->setColumnCount(headerList.size());
				_resultTable->setHorizontalHeaderLabels(headerList);
				return headerKeyList;
			}
		}
	}
	else
	{
		QStringList stringList;
		return stringList;
	}
	Logger::Info("CVWidgetGenerator::CreateViewComboBoxOnResults() - End");
}

/*
* Description - DrawViewAndResultsWidget() method used to draw view and results widget.
* Parameter -  MVTableWidget, QListWidget, json, string, QStringList, string, string, QStringList, bool, string, bool, string, bool.
* Exception - exception, Char *
* Return -
*/
void CVWidgetGenerator::DrawViewAndResultsWidget(MVTableWidget* _resultTable, QListWidget* _iconWidget, bool _createIconwidget, json _resultJson, string _thubmnailDisplayName, QStringList _headerKeyList, string _inlcudePalette, string _inlcudeSeason, QStringList _attScopes, bool _isReadiobutton, string _module, bool _isProductExist, json _viewListJsonArray)
{
	Logger::Info("CVWidgetGenerator::DrawViewAndResultsWidget() - Start");
	try
	{
		Logger::Debug("HERE::TRY INSIDE");
		//UTILITY_API->DisplayMessageBox("viewListJsonArray::" + to_string(viewListJsonArray));
		QStringList headerList;
		headerList << "Select" << QString::fromStdString(_thubmnailDisplayName);
		if (_inlcudePalette == "true")
		{
			headerList << "Palette";
		}
		if (_inlcudeSeason == "true")
		{
			headerList << "Season";
		}
		if (DOCUMENT_MODULE == _module)
		{
			headerList << "Product Name";
			if (_isProductExist)
				headerList << "Product Id";
		}
		json viewfieldListJsonArray = json::object();
		for (int i = 0; i < _viewListJsonArray.size(); i++)
		{
			string viewListStr = Helper::GetJSONValue<int>(_viewListJsonArray, i, false);
			json viewListJson = json::parse(viewListStr);
			string viewfieldListJsonArrayStr = Helper::GetJSONValue<string>(viewListJson, FIELDS_LIST_JSON_KEY, false);
			viewfieldListJsonArray = json::parse(viewfieldListJsonArrayStr);

		}
		Logger::Debug("Create product constructor() viewfieldListJsonArray1...." + to_string(viewfieldListJsonArray));
		if (STYLE_MODULE == _module )
		{
			json clientSpecificJson = Configuration::GetInstance()->GetClientSpecificJson();
			Logger::Debug("Create product constructor() m_clientSpecificJson...." + to_string(clientSpecificJson));
			string attTypesStr = "";
			json attTypesArray = Helper::GetJSONParsedValue<string>(clientSpecificJson, "styleTableAttributes", false);
			int size = viewfieldListJsonArray.size();
			for (int i = 0; i < attTypesArray.size(); i++)
			{
				json fieldsJson = Helper::GetJSONParsedValue<int>(attTypesArray, i, false);
				viewfieldListJsonArray[size + i] = fieldsJson;
			}
		}
		Logger::Debug("Create product constructor() viewfieldListJsonArray2...." + to_string(viewfieldListJsonArray));
		QStringList iconToolTipKeyList, iconToolTipDisplayList;
		for (int j = 0; j < viewfieldListJsonArray.size(); j++)
		{
			json fieldsJson = Helper::GetJSONParsedValue<int>(viewfieldListJsonArray, j, false);

			string headerInternalName = Helper::GetJSONValue<string>(fieldsJson, ATTRIBUTE_KEY, true);
			//string attScope = Helper::GetJSONValue<string>(fieldsJson, ATTSCOPE_KEY, true);
			string attributeType = Helper::GetJSONValue<string>(fieldsJson, ATTRIBUTE_TYPE_KEY, true);
			Logger::Debug("CVWidgetGenerator::DrawViewAndResultsWidget() - attScope:: Inside Id");
			_headerKeyList << QString::fromStdString(headerInternalName);
			iconToolTipKeyList << QString::fromStdString(headerInternalName);
			string headerDisplaylName = Helper::GetJSONValue<string>(fieldsJson, ATTRIBUTE_NAME_KEY, true);
			headerList << QString::fromStdString(headerDisplaylName);
			iconToolTipDisplayList << QString::fromStdString(headerDisplaylName);
			Logger::Debug("headerInternalName::" + headerInternalName);
		}
		_resultTable->setColumnCount(headerList.size());
		_resultTable->setHorizontalHeaderLabels(headerList);
		int rows = _resultJson.size();

		_resultTable->setRowCount(_resultJson.size());
		//UTILITY_API->DisplayMessageBox("_resultJson.size()::" + to_string(_resultJson.size()));
		for (int i = 0; i < _resultJson.size(); i++)
		{
			int currentColumn = 0;
			string attachmentId;
			string resultListStr = Helper::GetJSONValue<int>(_resultJson, i, false);
			json resultListJson = json::parse(resultListStr);
			//UTILITY_API->DisplayMessageBox("resultListJson::" + to_string(resultListJson));
			string objectId = Helper::GetJSONValue<string>(resultListJson, OBJECT_ID_KEY, true);
			string objectName = Helper::GetJSONValue<string>(resultListJson, OBJECT_NAME_KEY, true);
			if (_module != COLOR_MODULE)
				attachmentId = Helper::GetJSONValue<string>(resultListJson, DFAULT_ASSET_KEY, true);
			//UTILITY_API->DisplayMessageBox("attachmentId::" + attachmentId);
			/*if (attachmentId == "centric%3A"|| !FormatHelper::HasContent( attachmentId))
				continue;*/
				//UTILITY_API->DisplayMessageBox("befor create icon...");			

			QPixmap pixmap = ReadQPixmap(resultListJson, objectId, _module);
			if (_createIconwidget)
				CreateIconWidget(resultListJson, _iconWidget, iconToolTipKeyList, iconToolTipDisplayList, attachmentId, _module, _isReadiobutton, pixmap);
			if (_isReadiobutton)
			{
				QRadioButton* selectItem = CreateRadioButtonWidget(objectId, attachmentId, objectName, "margin-left:20%; margin-right:50%;", _resultTable);
				_resultTable->setCellWidget(i, currentColumn, selectItem);
			}
			else
			{
				QWidget* selectItem = CreateCheckBoxWidget(false, objectId, attachmentId, objectName);
				_resultTable->setCellWidget(i, currentColumn, selectItem);
			}
			currentColumn++;
			//UTILITY_API->DisplayMessageBox("befor create thumbnail...");
			QWidget* thumbnailWidget = CreateThumbnailWidget(resultListJson, pixmap, _module);
			_resultTable->setCellWidget(i, currentColumn, thumbnailWidget);
			currentColumn++;
			if (_inlcudePalette == "true")
			{
				string dispValue = Helper::GetJSONValue<string>(resultListJson, PaletteKey, true);
				QTableWidgetItem* tableWidget = CreateTableWidgetItem(dispValue);
				_resultTable->setItem(i, currentColumn, tableWidget);
				currentColumn++;
			}
			if (_inlcudeSeason == "true")
			{
				string dispValue = Helper::GetJSONValue<string>(resultListJson, SEASON_KEY, true);
				QTableWidgetItem* tableWidget = CreateTableWidgetItem(dispValue);
				_resultTable->setItem(i, currentColumn, tableWidget);
				currentColumn++;
			}
			if (DOCUMENT_MODULE == _module)
			{
				string dispValue = Helper::GetJSONValue<string>(resultListJson, PRODUCT_NAME_KEY, true);
				QTableWidgetItem* tableWidget = CreateTableWidgetItem(dispValue);
				_resultTable->setItem(i, currentColumn, tableWidget);
				currentColumn++;
				if (_isProductExist)
				{
					dispValue = Helper::GetJSONValue<string>(resultListJson, PRODUCTID_KEY, true);
					tableWidget = CreateTableWidgetItem(dispValue);
					_resultTable->setItem(i, currentColumn, tableWidget);
					currentColumn++;
				}
			}

			json clientSpecificAttCachedJson = Configuration::GetInstance()->GetClientSpecificAttCachedData();
			for (int j = 0; j < _headerKeyList.size(); j++)
			{
				/*string dispValue = Helper::GetJSONValue<string>(resultListJson, _headerKeyList.at(j).toStdString(), true);
				Logger::Debug("CVWidgetGenerator::DrawViewAndResultsWidget() - dispValue::" + dispValue);*/
				if (resultListJson.contains(_headerKeyList.at(j).toStdString()))
				{
					Logger::Debug("IF bLOCK 1");
					string attJson = Helper::GetJSONValue<string>(clientSpecificAttCachedJson, _headerKeyList.at(j).toStdString(), false);
					Logger::Debug("IF bLOCK attJson"+ attJson);
					string dispValue = Helper::GetJSONValue<string>(resultListJson, _headerKeyList.at(j).toStdString(), true);
					Logger::Debug("IF bLOCK dispValue" + dispValue);
					if (FormatHelper::HasContent(attJson) && attJson != "null")
					{
						json attributeArray = json::array();
						attributeArray = json::parse(attJson);
						Logger::Debug("IF bLOCK 2");
						json fieldsJson = Helper::GetJSONParsedValue<int>(attributeArray, 0, false);
						Logger::Debug("IF bLOCK fieldsJson" + to_string(fieldsJson));
						dispValue = Helper::GetJSONValue<string>(fieldsJson, dispValue, true);

					}
					

					Logger::Debug("CVWidgetGenerator::DrawViewAndResultsWidget() - dispValue::" + dispValue);
					if (FormatHelper::HasContentAllowZero(dispValue) && dispValue != "null")
					{
						QTableWidgetItem* tableWidget = CreateTableWidgetItem(dispValue);

						_resultTable->setItem(i, currentColumn, tableWidget);
					}
				}
				else
				{
					Logger::Debug("ELse bLOCK 1");
					QTableWidgetItem* tableWidget = CreateTableWidgetItem("");
					_resultTable->setItem(i, currentColumn, tableWidget);
				}
				currentColumn++;
			}
		}
	}
	catch (exception e)
	{
		throw e;
	}
	Logger::Info("CVWidgetGenerator::DrawViewAndResultsWidget() - End");
}

/*
* Description - DrawViewAndResultsWidget() method used to draw view and results widget.
* Parameter -  QComboBox, MVTableWidget, QListWidget, json, json, string, string, bool, int, QStringList, bool, bool.
* Exception - exception, Char *
* Return -
*/
void CVWidgetGenerator::DrawViewAndResultsWidget(QComboBox* _viewCB, MVTableWidget* _resultTable, QListWidget* _iconWidget, bool _createIconwidget, json _resultJson, json _viewJSON, string _thubmnailDisplayName, string _selectedTypeName, bool _drawView, int _selectedViewIdx, QStringList _attScopes, bool _isReadiobutton, bool _isProductExist, json _viewListJsonArray)
{
	Logger::Info("CVWidgetGenerator::DrawViewAndResultsWidget() - Start");
	try
	{
		InitializeTableView(_resultTable);
		if (_drawView)
			CreateViewComboBoxOnSearch(_viewCB, _viewJSON, _selectedTypeName, _selectedViewIdx);

		string inlcudePalette;// = Helper::GetJSONValue<string>(_resultJson, INCLUDEPALETTE_KEY, true);
		string includeSeason;// = Helper::GetJSONValue<string>(_resultJson, INCLUDESEASON_KEY, true);
		string module = _selectedTypeName;//Helper::GetJSONValue<string>(_resultJson, MODULE_KEY, true);
		QStringList headerKeyList = CreateViewComboBoxOnResults(_resultTable, _viewJSON, _selectedTypeName, _viewCB->currentText().toStdString(), _thubmnailDisplayName, inlcudePalette, includeSeason, _attScopes, module, _isProductExist);
		Logger::Debug("headerKeyList::" + headerKeyList.join(", ").toStdString());
		if (FormatHelper::HasContent(_viewCB->currentText().toStdString()))
		{
			string compatibilityKey = Helper::GetJSONValue<string>(_resultJson, COMP3D_KEY, true);
			string solidKey = Helper::GetJSONValue<string>(_resultJson, CLOSOLIDCOLOR_KEY, true);
			string nonSolidKey = Helper::GetJSONValue<string>(_resultJson, CLOPRINTANDPATTERN_KEY, true);
			string m_searchResults = Helper::GetJSONValue<string>(_resultJson, SEARCHRESULTS_KEY, false);
			json m_resultsJSONArray = json::parse(m_searchResults);
			json iconFieldsListJsonArray = Helper::GetJSONParsedValue<string>(_resultJson, "iconFieldsList", false);
			QStringList iconToolTipKeyList, iconToolTipDisplayList;
			for (int i = 0; i < iconFieldsListJsonArray.size(); i++)
			{
				json fieldsJson = Helper::GetJSONParsedValue<int>(iconFieldsListJsonArray, i, false);
				string headerInternalName = Helper::GetJSONValue<string>(fieldsJson, ATTRIBUTE_KEY, true);
				iconToolTipKeyList << QString::fromStdString(headerInternalName);
				string headerDisplaylName = Helper::GetJSONValue<string>(fieldsJson, ATTRIBUTE_NAME_KEY, true);
				iconToolTipDisplayList << QString::fromStdString(headerDisplaylName);
			}

			_resultTable->setRowCount(m_resultsJSONArray.size());
			for (int i = 0; i < m_resultsJSONArray.size(); i++)
			{
				int currentColumn = 0;

				string resultListStr = Helper::GetJSONValue<int>(m_resultsJSONArray, i, false);
				json resultListJson = json::parse(resultListStr);

				string objectId = Helper::GetJSONValue<string>(resultListJson, OBJECT_ID_KEY, true);
				string attachmentId = Helper::GetJSONValue<string>(resultListJson, DFAULT_ASSET_KEY, true);
				string objectName = Helper::GetJSONValue<string>(resultListJson, OBJECT_NAME_KEY, true);

				QPixmap pixmap = ReadQPixmap(resultListJson, objectId, module);
				if (_createIconwidget)
					CreateIconWidget(resultListJson, _iconWidget, iconToolTipKeyList, iconToolTipDisplayList, attachmentId, solidKey, _isReadiobutton, pixmap);
				if (_isReadiobutton)
				{
					QRadioButton* selectItem = CreateRadioButtonWidget(objectId, attachmentId, objectName, "margin-left:20%; margin-right:50%;", _resultTable);
					_resultTable->setCellWidget(i, currentColumn, selectItem);
				}
				else
				{
					QWidget* selectItem = CreateCheckBoxWidget(false, objectId, attachmentId, objectName);
					_resultTable->setCellWidget(i, currentColumn, selectItem);
				}
				currentColumn++;
				QWidget* thumbnailWidget = CreateThumbnailWidget(resultListJson, pixmap, module);
				_resultTable->setCellWidget(i, currentColumn, thumbnailWidget);
				currentColumn++;
				if (inlcudePalette == "true")
				{
					string dispValue = Helper::GetJSONValue<string>(resultListJson, PaletteKey, true);
					QTableWidgetItem* tableWidget = CreateTableWidgetItem(dispValue);
					_resultTable->setItem(i, currentColumn, tableWidget);
					currentColumn++;
				}
				if (includeSeason == "true")
				{
					string dispValue = Helper::GetJSONValue<string>(resultListJson, SEASON_KEY, true);
					QTableWidgetItem* tableWidget = CreateTableWidgetItem(dispValue);
					_resultTable->setItem(i, currentColumn, tableWidget);
					currentColumn++;
				}
				if (DOCUMENT_MODULE == module)
				{
					string dispValue = Helper::GetJSONValue<string>(resultListJson, PRODUCT_NAME_KEY, true);
					QTableWidgetItem* tableWidget = CreateTableWidgetItem(dispValue);
					_resultTable->setItem(i, currentColumn, tableWidget);
					currentColumn++;
					if (_isProductExist)
					{
						dispValue = Helper::GetJSONValue<string>(resultListJson, PRODUCTID_KEY, true);
						tableWidget = CreateTableWidgetItem(dispValue);
						_resultTable->setItem(i, currentColumn, tableWidget);
						currentColumn++;
					}
				}
				Logger::Debug("CVWidgetGenerator::DrawViewAndResultsWidget() - resultListStr::" + resultListStr);
				for (int j = 0; j < headerKeyList.size(); j++)
				{
					Logger::Debug("CVWidgetGenerator::DrawViewAndResultsWidget() - headerKeyList::" + headerKeyList.at(j).toStdString());
					if (resultListJson.contains(headerKeyList.at(j).toStdString()))
					{
						Logger::Debug("IF bLOCK");
						string dispValue = Helper::GetJSONValue<string>(resultListJson, headerKeyList.at(j).toStdString(), true);
						Logger::Debug("CVWidgetGenerator::DrawViewAndResultsWidget() - dispValue::" + dispValue);
						if (FormatHelper::HasContentAllowZero(dispValue) && dispValue != "null")
						{
							QTableWidgetItem* tableWidget = CreateTableWidgetItem(dispValue);
							_resultTable->setItem(i, currentColumn, tableWidget);
						}
					}
					else
					{
						Logger::Debug("ELse bLOCK");
						QTableWidgetItem* tableWidget = CreateTableWidgetItem("");
						_resultTable->setItem(i, currentColumn, tableWidget);
					}
					currentColumn++;
				}
			}
		}
		else
		{
			DrawViewAndResultsWidget(_resultTable, _iconWidget, _createIconwidget, _resultJson, _thubmnailDisplayName, headerKeyList, inlcudePalette, includeSeason, _attScopes, _isReadiobutton, module, _isProductExist, _viewListJsonArray);
		}
	}
	catch (exception e)
	{
		throw e;
	}
	Logger::Info("CVWidgetGenerator::DrawViewAndResultsWidget() - End");
}

/*
* Description - CreateCheckBoxWidget() method used to create check box widget.
* Parameter -  bool, string, string, string, char*.
* Exception -
* Return - QWidget.
*/
QWidget* CVWidgetGenerator::CreateCheckBoxWidget(bool _isChecked, string _objectId, string _attachmentId, string _objectName)
{
	Logger::Info("CVWidgetGenerator::CreateCheckBoxWidget() - Start");
	QWidget* pWidget = new QWidget();
	QCheckBox* selectItem = new QCheckBox();
	QHBoxLayout* pLayout = new QHBoxLayout(pWidget);

	pLayout->addWidget(selectItem);
	pLayout->setAlignment(Qt::AlignCenter);
	pLayout->setContentsMargins(0, 0, 0, 0);

	selectItem->setAttribute(Qt::WA_MacShowFocusRect, false);
	selectItem->setChecked(_isChecked);
	selectItem->setProperty("objectId", QString::fromStdString(_objectId));
	selectItem->setProperty(DFAULT_ASSET_KEY.c_str(), QString::fromStdString(_attachmentId));
	selectItem->setProperty("objectName", QString::fromStdString(_objectName));

	pWidget->setLayout(pLayout);
	Logger::Info("CVWidgetGenerator::CreateCheckBoxWidget() - End");
	return pWidget;
}

/*
* Description - CreateRadioButtonWidget() method used to create radio button.
* Parameter -  bool, string, string, string, char*, MVTableWidget.
* Exception -
* Return - QRadioButton
*/
QRadioButton* CVWidgetGenerator::CreateRadioButtonWidget(string _objectId, string _attachmentId, string _objectName, char* _styleSheet, MVTableWidget* _resultTable)
{
	Logger::Info("CVWidgetGenerator::CreateRadioButtonWidget() - Start");

	QRadioButton* selectItem = new QRadioButton();
	selectItem->setParent(_resultTable);
	selectItem->setStyleSheet(_styleSheet);
	selectItem->setContentsMargins(0, 0, 0, 0);
	selectItem->setAttribute(Qt::WA_MacShowFocusRect, false);
	selectItem->setProperty("objectId", QString::fromStdString(_objectId));
	selectItem->setProperty(DFAULT_ASSET_KEY.c_str(), QString::fromStdString(_attachmentId));
	selectItem->setProperty("objectName", QString::fromStdString(_objectName));
	Logger::Info("CVWidgetGenerator::CreateRadioButtonWidget() - End");
	return selectItem;
}

/*
* Description - CreateThumbnailWidget() method used to create thumbnail widget.
* Parameter -  json, string, string, string.
* Exception - exception, Char *
* Return - QWidget
*/
QWidget* CVWidgetGenerator::CreateThumbnailWidget(json _resultJson, string _imageId, string _module)
{
	Logger::Info("CVWidgetGenerator::CreateThumbnailWidget() - Start");
	QWidget* pWidget = new QWidget();
	try
	{
		QPixmap pixmap;
		QImage styleIcon;
		string filenameToSave = "";
		string rgbValue;
		//if (_compatibility.find(_solidKey) != string::npos)
		if (_module == COLOR_MODULE)
		{
			try
			{
				//UTILITY_API->DisplayMessageBox("comming inside .....");
				rgbValue = Helper::GetJSONValue<string>(_resultJson, RGB_VALUE_KEY, true);
				if (FormatHelper::HasContent(rgbValue))
				{
					rgbValue = Helper::FindAndReplace(rgbValue, "(", "");
					rgbValue = Helper::FindAndReplace(rgbValue, ")", "");
					rgbValue = Helper::FindAndReplace(rgbValue, " ", "");
					//UTILITY_API->DisplayMessageBox("comming inside ..rgbValue..."+ rgbValue);

					QStringList listRGB;
					QString colorRGB = QString::fromStdString(rgbValue);
					listRGB = colorRGB.split(',');
					int red = listRGB.at(0).toInt();
					int green = listRGB.at(1).toInt();
					int blue = listRGB.at(2).toInt();
					QColor color(red, green, blue);
					QImage image(90, 100, QImage::Format_ARGB32);
					image.fill(color);
					styleIcon = image;
					pixmap = QPixmap::fromImage(styleIcon);
					//UTILITY_API->DisplayMessageBox("comming inside ...if.. and end..");
				}
			}
			catch (exception e)
			{
				//UTILITY_API->DisplayMessageBox("comming inside ..rgbValue..." + string(e.what()));
				throw e;
			}
		}
		else
		{
			string thumbnail = UIHelper::GetThumbnailUrl(_imageId);// = Helper::GetJSONValue<string>(_resultJson, THUMBNAIL_KEY, true);
			Logger::Debug("CreateThumbnailWidget() thumbnail - " + thumbnail);
			if (FormatHelper::HasContent(thumbnail))
			{
				// Bytestream to image conversion Commented 
				/*QByteArray imageBytes = QByteArray::fromBase64(thumbnail.c_str());
				pixmap.loadFromData(imageBytes);*/

				QByteArray imageBytes;
				auto startTime = std::chrono::high_resolution_clock::now();
				imageBytes = Helper::DownloadImageFromURL(thumbnail);
				auto finishTime = std::chrono::high_resolution_clock::now();
				std::chrono::duration<double> totalDuration = finishTime - startTime;
				Logger::perfomance(PERFOMANCE_KEY + "Download Thumnail API :: " + to_string(totalDuration.count()));

				QBuffer buffer(&imageBytes);
				buffer.open(QIODevice::ReadOnly);
				QImageReader imageReader(&buffer);
				imageReader.setDecideFormatFromContent(true);
				styleIcon = imageReader.read();
				if (styleIcon.isNull())
				{
					Logger::Logger("FabricsSearchDialog -> CreateResultTable() Image is not loaded.  ");
				}
				else
				{
					pixmap = QPixmap::fromImage(styleIcon);
				}
			}
		}

		if (pixmap.isNull())
		{
			QImageReader imageReader(":/CLOVise/PLM/Images/NoImage.png");
			imageReader.setDecideFormatFromContent(true);
			styleIcon = imageReader.read();
			pixmap = QPixmap::fromImage(styleIcon);
		}

		QLabel* label = new QLabel;
		

		label->setMaximumSize(QSize(70, 70));
		int w = label->width();
		int h = label->height();
		//label->setScaledContents(true);
		label->setToolTip(QString::fromStdString(rgbValue));
		label->setPixmap(QPixmap(pixmap.scaled(w, h, Qt::KeepAspectRatio)));
		QHBoxLayout* pLayout = new QHBoxLayout(pWidget);
		pLayout->addWidget(label);
		pLayout->setAlignment(Qt::AlignCenter);
		pLayout->setContentsMargins(0, 0, 0, 0);
		pWidget->setLayout(pLayout);
	}
	catch (exception e)
	{
		throw e;
	}
	Logger::Info("CVWidgetGenerator::CreateThumbnailWidget() - End");
	return pWidget;
}

/*
* Description - CreateTableWidgetItem() method used to create table widget item.
* Parameter -  string.
* Exception -
* Return - QTableWidgetItem.
*/
QTableWidgetItem* CVWidgetGenerator::CreateTableWidgetItem(string _dispValue)
{
	Logger::Info("CVWidgetGenerator::CreateTableWidgetItem() - Start");
	Logger::Debug("CVWidgetGenerator::CreateTableWidgetItem() - dispValue::" + _dispValue);
	if (!IsNumber(_dispValue))
	{
		QTableWidgetItem* tableWidget = new QTableWidgetItem(QString::fromStdString(_dispValue));
		tableWidget->setTextAlignment(Qt::AlignCenter);
		tableWidget->setToolTip(QString::fromStdString(_dispValue));
		return tableWidget;
	}
	else
	{
		CVTableWidgetItem* customTableWidget = new CVTableWidgetItem;
		customTableWidget->setText(QString::fromStdString(_dispValue));
		customTableWidget->setTextAlignment(Qt::AlignCenter);
		customTableWidget->setToolTip(QString::fromStdString(_dispValue));
		return customTableWidget;
	}
	Logger::Info("CVWidgetGenerator::CreateTableWidgetItem() - End");
}

/*
* Description - UpdateTableAndIconRows() method used to update table and icon rows.
* Parameter -  MVTableWidget, QLabel, QComboBox, int.
* Exception -
* Return -
*/
void CVWidgetGenerator::UpdateTableAndIconRows(MVTableWidget* _resultTable, QLabel* _currPageLabel, QComboBox* _resultPerPageComboBox, int _resultsCount, bool _isFromConstructor)
{
	Logger::Info("CVWidgetGenerator::UpdateTableAndIconRows() - Start");
	int currentPage = _currPageLabel->text().toInt();
	int resultsPerPage = _resultPerPageComboBox->currentText().toInt();

	for (int i = 0; i < _resultsCount; i++)
	{
		if ((i >= (currentPage - 1) * resultsPerPage) && i < (min((currentPage * resultsPerPage), (_resultsCount))))
			_resultTable->setRowHidden(i, false);
		else
			_resultTable->setRowHidden(i, true);
	}
	if (_isFromConstructor)
	{
		_resultTable->horizontalHeader()->setMinimumHeight(HORIZONTAL_HEADER_HEIGHT);
		int tableWidth = _resultTable->width();
		int columnWidth = tableWidth / (_resultTable->columnCount() - 2);
		for (int i = 0; i < _resultTable->columnCount(); i++)
		{
			if (i == 0)
				_resultTable->setColumnWidth(i, 50);
			else if (i > 1)
				_resultTable->setColumnWidth(i, columnWidth);
		}
		_resultTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Fixed);
		_resultTable->horizontalHeader()->setDefaultAlignment(Qt::AlignCenter | Qt::Alignment(Qt::TextWordWrap));
	}
	Logger::Info("CVWidgetGenerator::UpdateTableAndIconRows() - End");
}

/*
* Description - UpdateTableAndIconRows() method used to update table and icon rows.
* Parameter -  QListWidget, QLabel, QComboBox, int.
* Exception -
* Return -
*/
void CVWidgetGenerator::UpdateTableAndIconRows(QListWidget* _iconWidget, QLabel* _currPageLabel, QComboBox* _resultPerPageComboBox, int _resultsCount)
{
	Logger::Info("CVWidgetGenerator::UpdateTableAndIconRows() - Start");
	int currentPage = _currPageLabel->text().toInt();
	int resultsPerPage = _resultPerPageComboBox->currentText().toInt();
	for (int i = 0; i < _resultsCount; i++)
	{
		if ((i >= (currentPage - 1) * resultsPerPage) && i < (min((currentPage * resultsPerPage), (_resultsCount))))
			_iconWidget->setRowHidden(i, false);
		else
			_iconWidget->setRowHidden(i, true);
	}
	Logger::Info("CVWidgetGenerator::UpdateTableAndIconRows() - End");
}

/*
* Description - CreateResultsPerPageWidget() method used to create results per page.
* Parameter -  QComboBox, QStringList.
* Exception -
* Return -
*/
void CVWidgetGenerator::CreateResultsPerPageWidget(QComboBox* _resultPerPageCB, QStringList _resultPerPageList)
{
	Logger::Info("CVWidgetGenerator::CreateResultsPerPageWidget() - Start");
	_resultPerPageCB->addItems(_resultPerPageList);
	int index = _resultPerPageCB->findText(QString::fromStdString(Configuration::GetInstance()->GetSelectedResultsPerPage()));
	if (index != -1) { // -1 for not found
		_resultPerPageCB->setCurrentIndex(index);
	}
	Logger::Info("CVWidgetGenerator::CreateResultsPerPageWidget() - End");
}

/*
* Description - PopulateValuesOnResultsUI() method used to populate values on results UI.
* Parameter -  QToolButton, QLabel, QLabel, QComboBox, int.
* Exception -
* Return -
*/
void CVWidgetGenerator::PopulateValuesOnResultsUI(QToolButton* _nextButton, QLabel* _numOfResults, QLabel* _totalPageLabel, QComboBox* _resultPerPageComboBox, int _resultsCount)
{
	int resultsPerPage = _resultPerPageComboBox->currentText().toInt();
	if (FormatHelper::HasContent(to_string(_resultsCount)))
	{
		_numOfResults->setText(QString::fromStdString(to_string(_resultsCount)));
	}
	else
	{
		_numOfResults->setText(QString::fromStdString(to_string(0)));
	}
	int totalNoOfPages = 1;
	if (_resultsCount % resultsPerPage == 0)
	{
		totalNoOfPages = _resultsCount / resultsPerPage;
	}
	else
	{
		totalNoOfPages = (_resultsCount / resultsPerPage) + 1;
	}
	if (totalNoOfPages == 0)
	{
		_totalPageLabel->setText("1");
		_nextButton->setEnabled(false);
	}
	else
	{
		_totalPageLabel->setText(QString::fromStdString(to_string(totalNoOfPages)));
		_nextButton->setEnabled(true);
	}
	if (totalNoOfPages == 1)
	{
		_nextButton->setEnabled(false);
	}
}

/*
* Description - PopulateValuesOnResultsUI() method used to populate values on results UI.
* Parameter -  QToolButton, QLabel, QLabel, QComboBox, QStringList, int.
* Exception -
* Return -
*/
void CVWidgetGenerator::PopulateValuesOnResultsUI(QToolButton* _nextButton, QLabel* _numOfResults, QLabel* _totalPageLabel, QComboBox* _resultPerPageComboBox, QStringList _rppList, int _resultsCount)
{
	Logger::Info("CVWidgetGenerator::PopulateValuesOnResultsUI() - Start");

	CreateResultsPerPageWidget(_resultPerPageComboBox, _rppList);
	PopulateValuesOnResultsUI(_nextButton, _numOfResults, _totalPageLabel, _resultPerPageComboBox, _resultsCount);

	Logger::Info("CVWidgetGenerator::PopulateValuesOnResultsUI() - End");
}

/*
* Description - SortIconResultTable() method used to sort icon result table.
* Parameter -  bool, QListWidget, MVTableWidget, QComboBox, json, bool.
* Exception - exception, Char *
* Return -
*/
void CVWidgetGenerator::SortIconResultTable(bool _isResultTableSort, QListWidget* _iconWidget, MVTableWidget* _resultTable, json _resultJson, bool _isRadioButton)
{
	Logger::Info("CVWidgetGenerator::SortIconResultTable() - Start");
	try
	{
		if (_isResultTableSort)
		{
			_iconWidget->clear();
			InitializeIconView(_iconWidget);
			string searchResults = Helper::GetJSONValue<string>(_resultJson, SEARCHRESULTS_KEY, false);
			json iconFieldsListJsonArray = Helper::GetJSONParsedValue<string>(_resultJson, "iconFieldsList", false);
			json resultsJSONArray = json::parse(searchResults);
			QStringList iconToolTipKeyList, iconToolTipDisplayList;
			for (int i = 0; i < iconFieldsListJsonArray.size(); i++)
			{
				json fieldsJson = Helper::GetJSONParsedValue<int>(iconFieldsListJsonArray, i, false);
				string headerInternalName = Helper::GetJSONValue<string>(fieldsJson, ATTRIBUTE_KEY, true);
				iconToolTipKeyList << QString::fromStdString(headerInternalName);
				string headerDisplaylName = Helper::GetJSONValue<string>(fieldsJson, ATTRIBUTE_NAME_KEY, true);
				iconToolTipDisplayList << QString::fromStdString(headerDisplaylName);
			}
			string compatibilityKey = Helper::GetJSONValue<string>(_resultJson, COMP3D_KEY, true);
			string solidKey = Helper::GetJSONValue<string>(_resultJson, CLOSOLIDCOLOR_KEY, true);
			for (int rowCount = 0; rowCount < _resultTable->rowCount(); rowCount++)
			{
				QRadioButton* tempRadioButton;
				QCheckBox* tempCheckBox;
				QString objectId;
				auto qWidget = _resultTable->cellWidget(rowCount, 0);
				if (_isRadioButton)
				{
					tempRadioButton = qobject_cast <QRadioButton*> (qWidget);
					objectId = tempRadioButton->property("objectId").toString();
				}
				else
				{
					tempCheckBox = qobject_cast <QCheckBox*> (qWidget->layout()->itemAt(0)->widget());
					objectId = tempCheckBox->property("objectId").toString();
				}
				for (int i = 0; i < resultsJSONArray.size(); i++)
				{
					string resultListStr = Helper::GetJSONValue<int>(resultsJSONArray, i, false);
					json resultListJson = json::parse(resultListStr);
					string objectIdFromJson = Helper::GetJSONValue<string>(resultListJson, OBJECT_ID_KEY, true);
					string compatibility = Helper::GetJSONValue<string>(resultListJson, compatibilityKey, true);
					if (objectId == QString::fromStdString(objectIdFromJson))
					{
						if (_isRadioButton)
							CVWidgetGenerator::CreateIconWidget(resultListJson, _iconWidget, iconToolTipKeyList, iconToolTipDisplayList, compatibility, solidKey, true);
						else
							CVWidgetGenerator::CreateIconWidget(resultListJson, _iconWidget, iconToolTipKeyList, iconToolTipDisplayList, compatibility, solidKey, false);
						break;
					}
				}
			}
			if (_isRadioButton)
				CVWidgetGenerator::TableRowSelected(_resultTable, _iconWidget, true, false);
			else
				CVWidgetGenerator::TableRowSelected(_resultTable, _iconWidget, false, false);
			UTILITY_API->SetProgress("Loading", (qrand() % 101));
		}
	}
	catch (exception e)
	{
		throw e;
	}
	Logger::Info("CVWidgetGenerator::SortIconResultTable() - End");
}

/*
* Description - CreateIconWidget() method used to create Icon related widget.
* Parameter -  json, QListWidget, QStringList, QStringList, string, string, bool.
* Exception - exception, Char *
* Return -
*/
void CVWidgetGenerator::CreateIconWidget(json _resultListJson, QListWidget* _iconWidget, QStringList _tootTipKeys, QStringList _tootTipKeyDisplay, string _compatibility, string _module, bool _isReadiobutton)
{
	Logger::Info("CVWidgetGenerator::CreateIconWidget() - Start");
	InitializeIconView(_iconWidget);

	QPixmap pixmap;
	QImage styleIcon;
	string objectName, thumbnail, rgbValue, filenameToSave;
	objectName = Helper::GetJSONValue<string>(_resultListJson, OBJECT_NAME_KEY, true);
	string objectId = Helper::GetJSONValue<string>(_resultListJson, OBJECT_ID_KEY, true);

	if (_module == COLOR_MODULE)
	{
		try
		{
			rgbValue = Helper::GetJSONValue<string>(_resultListJson, RGB_VALUE_KEY, true);
			rgbValue = Helper::FindAndReplace(rgbValue, "(", "");
			rgbValue = Helper::FindAndReplace(rgbValue, ")", "");
			rgbValue = Helper::FindAndReplace(rgbValue, " ", "");
			//UTILITY_API->DisplayMessageBox("rgbValue::" + rgbValue);
			if (FormatHelper::HasContent(rgbValue))
			{
				QStringList listRGB;
				QString colorRGB = QString::fromStdString(rgbValue);
				listRGB = colorRGB.split(',');
				int red = listRGB.at(0).toInt();
				int green = listRGB.at(1).toInt();
				int blue = listRGB.at(2).toInt();
				QColor color(red, green, blue);
				QImage image(90, 100, QImage::Format_ARGB32);
				image.fill(color);
				styleIcon = image;
				pixmap = QPixmap::fromImage(styleIcon);
			}
		}
		catch (exception e)
		{
			throw e;
		}
	}
	else
	{
		string thumbnail = Helper::GetJSONValue<string>(_resultListJson, THUMBNAIL_KEY, true);
		Logger::Debug("CreateThumbnailWidget() thumbnail - " + thumbnail);
		if (FormatHelper::HasContent(thumbnail))
		{
			// Bytestream to image conversion Commented 
			/*QByteArray imageBytes = QByteArray::fromBase64(thumbnail.c_str());
			pixmap.loadFromData(imageBytes);*/

			QByteArray imageBytes;
			imageBytes = Helper::DownloadImageFromURL(thumbnail);
			QBuffer buffer(&imageBytes);
			buffer.open(QIODevice::ReadOnly);
			QImageReader imageReader(&buffer);
			imageReader.setDecideFormatFromContent(true);
			styleIcon = imageReader.read();
			if (styleIcon.isNull())
			{
				Logger::Logger("FabricsSearchDialog -> CreateResultTable() Image is not loaded.  ");
			}
			else
			{
				pixmap = QPixmap::fromImage(styleIcon);
			}
		}
	}

	if (pixmap.isNull())
	{
		QImageReader imageReader(":/CLOVise/PLM/Images/NoImage.png");
		imageReader.setDecideFormatFromContent(true);
		styleIcon = imageReader.read();
		pixmap = QPixmap::fromImage(styleIcon);
	}

	QPixmap newPixmap = pixmap.scaled(QSize(80, 80), Qt::KeepAspectRatio);

	QIcon icon;
	icon.addPixmap(newPixmap, QIcon::Normal);
	icon.addPixmap(newPixmap, QIcon::Selected);
	icon.addPixmap(newPixmap, QIcon::Active);

	QListWidgetItem* item = new QListWidgetItem(icon, AddNewLinesToString(objectName, 10));
	item->setTextAlignment(Qt::AlignRight);
	string toolTipString = "";
	for (int i = 0; i < _tootTipKeys.size(); i++)
	{
		toolTipString = toolTipString + _tootTipKeyDisplay.at(i).toStdString() + ": ";
		string value = Helper::GetJSONValue<string>(_resultListJson, _tootTipKeys.at(i).toStdString(), true);
		toolTipString = toolTipString + value + "\n";
	}
	if (!(_compatibility.find(_module) != string::npos))
	{
		item->setToolTip(QString::fromStdString(toolTipString));
	}
	else
	{
		item->setToolTip(RGB_TOOLTIP_KEY + QString::fromStdString(rgbValue) + "\n" + QString::fromStdString(toolTipString));
	}
	if (_isReadiobutton)
	{
		QRadioButton* radioButton = new QRadioButton;
		radioButton->setCheckable(true);
		radioButton->setText(" ");
		radioButton->setAttribute(Qt::WA_MacShowFocusRect, false);
		radioButton->setParent(_iconWidget);
		radioButton->setStyleSheet("QRadioButton{spacing: 7px;}""margin: 2px 5px 2px 1px;");
		radioButton->setProperty(COLOR_ID_KEY.c_str(), QString::fromStdString(objectId));
		radioButton->setProperty(OBJECT_ID_KEY.c_str(), QString::fromStdString(objectId));

		_iconWidget->addItem(item);
		_iconWidget->setItemWidget(item, radioButton);
	}
	else
	{
		QCheckBox* checkBox = new QCheckBox;
		checkBox->setCheckable(true);
		checkBox->setText(" ");
		checkBox->setAttribute(Qt::WA_MacShowFocusRect, false);
		checkBox->setStyleSheet("QCheckBox{spacing: 7px;}""margin: 2px 5px 2px 1px;");
		checkBox->setCheckState(Qt::Unchecked);
		checkBox->setProperty(COLOR_ID_KEY.c_str(), QString::fromStdString(objectId));
		checkBox->setProperty(OBJECT_ID_KEY.c_str(), QString::fromStdString(objectId));

		_iconWidget->addItem(item);
		_iconWidget->setItemWidget(item, checkBox);
	}
	Logger::Info("CVWidgetGenerator::CreateIconWidget() - End");
}

/*
* Description - TableRowSelected() method used to get  selected table row and collect the selected rows id.
* Parameter -  MVTableWidget, QListWidget, bool.
* Exception -
* Return - QStringList.
*/
QStringList CVWidgetGenerator::TableRowSelected(MVTableWidget* _resultTable, QListWidget* _iconViewTable, bool _isRadioButton, bool _isIconWidgetAvailable)
{
	int totalRowCount = _resultTable->rowCount();
	_resultTable->setSelectionMode(QAbstractItemView::MultiSelection);
	_iconViewTable->setSelectionMode(QAbstractItemView::MultiSelection);
	QStringList selectedRows;
	QStringList selectedIds;
	QString objectId = "";

	for (int rowCount = 0; rowCount < totalRowCount; rowCount++)
	{
		QWidget* qWidget = _resultTable->cellWidget(rowCount, 0);
		if (_isRadioButton)
		{
			QRadioButton *temp = qobject_cast <QRadioButton*> (qWidget);
			if (totalRowCount == 1)
			{
				temp->setChecked(true);
			}
			if (temp->isChecked())
			{
				objectId = temp->property(OBJECT_ID_KEY.c_str()).toString();
				if (!FormatHelper::HasContent(objectId.toStdString()))
				{
					temp->setCheckable(Qt::Unchecked);
					throw "Unable so select row, please try again or contact System Administrator";
				}
				if (!selectedIds.contains(objectId))
				{
					selectedIds.push_back(objectId);
				}
				selectedRows << QString::fromStdString(to_string(rowCount));
			}
		}
		else
		{
			QCheckBox *temp = qobject_cast <QCheckBox*> (qWidget->layout()->itemAt(0)->widget());
			if (temp->checkState() == Qt::Checked)
			{
				objectId = temp->property(OBJECT_ID_KEY.c_str()).toString();
				if (objectId == "centric%3A" || !FormatHelper::HasContent(objectId.toStdString()))
				{
					temp->setCheckState(Qt::Unchecked);
					throw "Unable so select row, please try again or contact System Administrator";
				}
				if (!selectedIds.contains(objectId))
				{
					selectedIds.push_back(objectId);
				}
				selectedRows << QString::fromStdString(to_string(rowCount));
			}
		}
	}
	_resultTable->clearSelection();
	for (auto iterate : selectedRows)
	{

		_resultTable->selectRow(iterate.toInt());

	}
	_resultTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
	_resultTable->setFocusPolicy(Qt::NoFocus);
	_resultTable->setSelectionMode(QAbstractItemView::NoSelection);
	if (_isIconWidgetAvailable)
	{
		for (int rowCount = 0; rowCount < totalRowCount; rowCount++)
		{
			auto itemWidget = _iconViewTable->item(rowCount);
			if (_isRadioButton)
			{
				auto temp = qobject_cast<QRadioButton*>(_iconViewTable->itemWidget(itemWidget));
				if (totalRowCount == 1)
				{
					temp->setChecked(true);
				}
				objectId = temp->property(OBJECT_ID_KEY.c_str()).toString();
				if (selectedIds.contains(objectId))
				{
					if (!FormatHelper::HasContent(objectId.toStdString()))
					{
						temp->setCheckable(Qt::Unchecked);
						throw "Unable so select row, please try again or contact System Administrator";
					}
					itemWidget->setSelected(true);
					temp->setChecked(Qt::Checked);
				}
				else
				{
					itemWidget->setSelected(false);
					temp->setChecked(Qt::Unchecked);
				}
			}
			else
			{
				auto temp = qobject_cast<QCheckBox*>(_iconViewTable->itemWidget(itemWidget));
				objectId = temp->property(OBJECT_ID_KEY.c_str()).toString();
				if (selectedIds.contains(objectId))
				{
					if (!FormatHelper::HasContent(objectId.toStdString()))
					{
						temp->setCheckState(Qt::Unchecked);
						throw "Unable so select row, please try again or contact System Administrator";
					}
					temp->setCheckState(Qt::Checked);
					itemWidget->setSelected(true);
				}
				else
				{
					itemWidget->setSelected(false);
					temp->setCheckState(Qt::Unchecked);
				}
			}
		}
		_iconViewTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
		_iconViewTable->setFocusPolicy(Qt::NoFocus);
		_iconViewTable->setSelectionMode(QAbstractItemView::NoSelection);
	}
	return selectedIds;
}

/*
* Description - IconRowSelected() method used to getselect collected icon row and the selected rows id.
* Parameter -  QListWidget, MVTableWidget, bool.
* Exception -
* Return - QStringList.
*/
QStringList CVWidgetGenerator::IconRowSelected(QListWidget* _iconViewTable, MVTableWidget* _resultTable, bool _isRadioButton)
{
	int totalRowCount = _iconViewTable->count();
	QStringList selectedRows;
	QStringList selectedIds;
	QString objectId = "";
	_iconViewTable->setSelectionMode(QAbstractItemView::MultiSelection);
	_resultTable->setSelectionMode(QAbstractItemView::MultiSelection);
	for (int rowCount = 0; rowCount < totalRowCount; rowCount++)
	{
		auto itemWidget = _iconViewTable->item(rowCount);
		if (_isRadioButton)
		{
			auto temp = qobject_cast<QRadioButton*>(_iconViewTable->itemWidget(itemWidget));
			if (totalRowCount == 1)
			{
				temp->setChecked(true);
			}
			if (temp->isChecked())
			{
				objectId = temp->property(OBJECT_ID_KEY.c_str()).toString();
				if (objectId == "centric%3A" || !FormatHelper::HasContent(objectId.toStdString()))
				{
					temp->setCheckable(Qt::Unchecked);
					throw "There is no attachment, Unable so select row";
				}
				if (!selectedIds.contains(objectId))
				{
					selectedIds.push_back(objectId);
				}
				itemWidget->setSelected(true);
				selectedRows << QString::fromStdString(to_string(rowCount));
			}
			else
				itemWidget->setSelected(false);
		}
		else
		{
			auto temp = qobject_cast<QCheckBox*>(_iconViewTable->itemWidget(itemWidget));
			if (temp->checkState() == Qt::Checked)
			{
				objectId = temp->property(OBJECT_ID_KEY.c_str()).toString();
				if (objectId == "centric%3A" || !FormatHelper::HasContent(objectId.toStdString()))
				{
					temp->setCheckState(Qt::Unchecked);
					throw "There is no attachment, Unable so select row";
				}
				if (!selectedIds.contains(objectId))
				{
					selectedIds.push_back(objectId);
				}
				itemWidget->setSelected(true);
				selectedRows << QString::fromStdString(to_string(rowCount));
			}
			else
				itemWidget->setSelected(false);
		}
	}

	_iconViewTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
	_iconViewTable->setFocusPolicy(Qt::NoFocus);
	_iconViewTable->setSelectionMode(QAbstractItemView::NoSelection);

	_resultTable->clearSelection();
	for (int rowCount = 0; rowCount < totalRowCount; rowCount++) // Change totalRowCount to maxResultsPerPage
	{
		QWidget* qWidget = _resultTable->cellWidget(rowCount, 0);
		if (_isRadioButton)
		{
			QRadioButton *temp = qobject_cast <QRadioButton*> (qWidget);
			if (totalRowCount == 1)
			{
				temp->setChecked(true);
			}
			objectId = temp->property(OBJECT_ID_KEY.c_str()).toString();
			if (selectedIds.contains(objectId))
			{

				if (!FormatHelper::HasContent(objectId.toStdString()))
				{
					temp->setCheckable(Qt::Unchecked);
					throw "Unable so select row, please try again or contact System Administrator";
				}
				_resultTable->selectRow(rowCount);
				temp->setChecked(Qt::Checked);
			}
			else
			{
				temp->setChecked(Qt::Unchecked);
			}
		}
		else
		{
			QCheckBox *temp = qobject_cast <QCheckBox*> (qWidget->layout()->itemAt(0)->widget());
			objectId = temp->property(OBJECT_ID_KEY.c_str()).toString();
			if (selectedIds.contains(objectId))
			{
				if (!FormatHelper::HasContent(objectId.toStdString()))
				{
					temp->setCheckState(Qt::Unchecked);
					throw "Unable so select row, please try again or contact System Administrator";
				}
				_resultTable->selectRow(rowCount);
				temp->setCheckState(Qt::Checked);
			}
			else
			{
				temp->setCheckState(Qt::Unchecked);
			}
		}
	}
	_resultTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
	_resultTable->setFocusPolicy(Qt::NoFocus);
	_resultTable->setSelectionMode(QAbstractItemView::NoSelection);
	return selectedIds;
}

/*
* Description - SetDownloadCount() method used to set the selected item count for download.
* Parameter -  QStringList, QStringList, QPushButton.
* Exception -
* Return - QStringList.
*/
QStringList CVWidgetGenerator::SetDownloadCount(QStringList _iconviewSelectedIds, QStringList _tableviewSelectedIds, QPushButton* _download)
{
	QString objectId = "";
	QStringList selectedIds;
	selectedIds.clear();

	for (int i = 0; i < _iconviewSelectedIds.size(); i++)
	{
		objectId = _iconviewSelectedIds[i];
		selectedIds.push_back(objectId);
	}
	for (int i = 0; i < _tableviewSelectedIds.size(); i++)
	{
		objectId = _tableviewSelectedIds[i];
		if (!selectedIds.contains(objectId))
		{
			selectedIds.push_back(objectId);
		}
	}

	QString buttonText = _download->text();
	int index = buttonText.toStdString().find("(");
	if (index != -1)
		buttonText = buttonText.left(index);
	if (selectedIds.size() > 0) {
		_download->setText(buttonText + "( " + QString::number(selectedIds.size()) + " )");
	}
	else {
		_download->setText(buttonText);
	}
	return selectedIds;
}

/*
* Description - DeSelectAllClicked() method used to unselect all selected item in result UI.
* Parameter -  MVTableWidget, QListWidget, bool, int,int,int, QPushButton, bool.
* Exception -
* Return -
*/
void CVWidgetGenerator::DeSelectAllClicked(MVTableWidget* _resultTable, QListWidget* _iconViewTable, QPushButton* _download)
{
	for (int i = 0; i < _iconViewTable->count(); i++)
	{
		auto itemWidget = _iconViewTable->item(i);
		auto temp = qobject_cast<QCheckBox*>(_iconViewTable->itemWidget(itemWidget));
		if (temp->checkState() == Qt::Checked)
		{
			temp->setCheckState(Qt::Unchecked);
			_iconViewTable->clearSelection();
		}
	}

	for (int i = 0; i < _resultTable->rowCount(); i++)
	{
		QWidget* qWidget1 = _resultTable->cellWidget(i, 0);
		QCheckBox *temp = qobject_cast <QCheckBox*> (qWidget1->layout()->itemAt(0)->widget());
		if (temp->checkState() == Qt::Checked)
		{
			temp->setCheckState(Qt::Unchecked);
			_resultTable->clearSelection();
		}
		_download->setText("Download");
	}
}


/*
* Description - IsNumber() method used to validate a string.
* Parameter -  string.
* Exception -
* Return - bool.
*/
bool CVWidgetGenerator::IsNumber(string _token)
{
	return regex_match(_token, regex(("((\\+|-)?[[:digit:]]+)(\\.(([[:digit:]]+)?))?")));
}

/*
* Description - AddNewLinesToString() method used to add newline to string.
* Parameter -  string, int.
* Exception -
* Return - QString
*/
QString CVWidgetGenerator::AddNewLinesToString(string& _str, int _sep)
{
	QString ans;
	int i = 0;
	if (!(FormatHelper::HasContent(_str)))
		return "";

	while (i < _str.size())
	{
		if (i % _sep == 0 && i)
			ans.push_back('\n');
		ans.push_back(_str[i]);
		i++;
	}

	if (ans.length() < 11)
		ans = ans + "\n\n";

	else if (ans.length() > 10 && ans.length() <= 21)
		ans = ans + "\n";

	if (ans.length() > 25)
		ans = QString::fromStdString(ans.toStdString().substr(0, 27)) + "...";

	return ans;
}


/*
* Description - SetButtonProperty() method used to add button properties on CLO-Vise Suite buttons.
* Parameter - QPushButton, QString, QString, QString
* Exception -
* Return -
*/
QPushButton* CVWidgetGenerator::CreatePushButton(QString _name, QString _iconPath, QString _toolTip, QString _styleSheet, int _height, bool _showWidget)
{
	QPushButton *button = new QPushButton();

	button->setText(_name);
	button->setIcon(QIcon(_iconPath));
	button->setToolTip(_toolTip);
	button->setStyleSheet(_styleSheet);
	button->setFixedHeight(_height);
	if (!_showWidget)
		button->hide();
	return button;
}

ComboBoxItem* CVWidgetGenerator::CreateComboBox(QString _toolTip, QString _styleSheet, bool _showWidget)
{
	ComboBoxItem *comboBox = new ComboBoxItem();

	comboBox->setToolTip(_toolTip);
	comboBox->setStyleSheet(_styleSheet);
	comboBox->setFocusPolicy(Qt::StrongFocus);

	if (!_showWidget)
		comboBox->hide();
	return comboBox;
}

QLabel* CVWidgetGenerator::CreateLabel(QString _name, QString _toolTip, QString _styleSheet, bool _showWidget)
{
	QLabel *label = new QLabel();

	label->setText(_name);
	label->setWordWrap(true);
	label->setToolTip(_toolTip);
	label->setStyleSheet(_styleSheet);
	if (!_showWidget)
		label->hide();
	return label;
}

QLineEdit* CVWidgetGenerator::CreateLineEdit(QString _toolTip, QString _styleSheet, bool _showWidget)
{
	QLineEdit *lineEdit = new QLineEdit();

	lineEdit->setToolTip(_toolTip);
	lineEdit->setStyleSheet(_styleSheet);
	lineEdit->setAttribute(Qt::WA_MacShowFocusRect, false);

	if (!_showWidget)
		lineEdit->hide();
	return lineEdit;
}
//static void SetButtonProperty(QPushButton* _button, QString _iconPath, QString _toolTip, QString _styleSheet);

/*
* Description - SetButtonProperty() method used to add button properties on CLO-Vise Suite buttons.
* Parameter - QPushButton, QString, QString, QString
* Exception -
* Return -
*/
QToolButton* CVWidgetGenerator::CreateToolButton(QString _iconPath, QString _toolTip, QString _styleSheet, bool _showWidget)
{
	QToolButton *button = new QToolButton();
	button->setIcon(QIcon(_iconPath));
	button->setToolTip(_toolTip);
	button->setStyleSheet(_styleSheet);
	if (!_showWidget)
		button->hide();
	return button;
}

/*
* Description - SetButtonProperty() method used to add button properties on CLO-Vise Suite buttons.
* Parameter - QPushButton, QString, QString, QString
* Exception -
* Return -
*/
void CVWidgetGenerator::SetButtonProperty(QPushButton* _button, QString _iconPath, QString _toolTip, QString _styleSheet)
{
	_button->setIcon(QIcon(_iconPath));
	_button->setToolTip(_toolTip);
	_button->setStyleSheet(_styleSheet);
}

/*
* Description - SetButtonProperty() method used to add button properties on CLO-Vise Suite buttons.
* Parameter - QPushButton, QString, QString, QString
* Exception -
* Return -
*/
void CVWidgetGenerator::SetButtonProperty(QToolButton* _button, QString _iconPath, QString _toolTip, QString _styleSheet)
{
	_button->setIcon(QIcon(_iconPath));
	_button->setToolTip(_toolTip);
	_button->setStyleSheet(_styleSheet);
}

/*
* Description - CreateHirarchyTreeWidget() method used to add button properties on CLO-Vise Suite buttons.
* Parameter - QString
* Exception -
* Return -
*/
QTreeWidget* CVWidgetGenerator::CreateHirarchyTreeWidget(QString _style, bool _showWidget)
{
	QTreeWidget* treeWidget = new QTreeWidget();

	treeWidget->setStyleSheet(_style);
	treeWidget->setHeaderHidden(true);
	treeWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	treeWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	treeWidget->verticalScrollBar()->setStyleSheet(SCROLLBAR_STYLE);
	treeWidget->horizontalScrollBar()->setStyleSheet(SCROLLBAR_STYLE);
	if (!_showWidget)
		treeWidget->hide();

	return treeWidget;
}

/*
* Description - CreateSearchCriteriaTreeWidget() method used to add button properties on CLO-Vise Suite buttons.
* Parameter - QString
* Exception -
* Return -
*/
QTreeWidget* CVWidgetGenerator::CreateSearchCriteriaTreeWidget(bool _showWidget)
{
	QTreeWidget* treeWidget = new QTreeWidget();

	//treeWidget->setStyleSheet(_style);
	treeWidget->setColumnCount(2);
	treeWidget->setHeaderHidden(true);
	treeWidget->setWordWrap(true);
	treeWidget->setDropIndicatorShown(false);
	treeWidget->setRootIsDecorated(false);
	treeWidget->verticalScrollBar()->setStyleSheet(SCROLLBAR_STYLE);
	treeWidget->horizontalScrollBar()->setStyleSheet(SCROLLBAR_STYLE);
	treeWidget->resizeColumnToContents(0);
	treeWidget->resizeColumnToContents(1);
	//treeWidget->resizeColumnToContents(2);
	treeWidget->header()->setSectionResizeMode(0, QHeaderView::Stretch);
	treeWidget->header()->setSectionResizeMode(1, QHeaderView::Stretch);
	//treeWidget->header()->setSectionResizeMode(2, QHeaderView::Stretch);
	treeWidget->setSelectionMode(QAbstractItemView::NoSelection);
	if (!_showWidget)
		treeWidget->hide();

	return treeWidget;
}

/*
* Description - CreatePublishTreeWidget() method used to add button properties on CLO-Vise Suite buttons.
* Parameter - QString
* Exception -
* Return -
*/
QTreeWidget* CVWidgetGenerator::CreatePublishTreeWidget(QString _style, bool _showWidget)
{
	QTreeWidget* treeWidget = new QTreeWidget();

	treeWidget->setStyleSheet(_style/*"QTreeWidget { background-color: #262628; border: 1px solid #000;}""QTreeWidget::item { padding :5px; height: 20px; color: #FFFFFF; font-face: ArialMT; font-size: 10px; width: 90px; margin-left: 5px; margin-right: 5px; margin-top: 5px; margin-bottom: 5px; border: none;}""QTreeWidget::item:hover{background-color: #262628;}"*/);

	treeWidget->setColumnCount(2);
	treeWidget->setHeaderHidden(true);
	treeWidget->setWordWrap(true);
	treeWidget->setDropIndicatorShown(false);
	treeWidget->setRootIsDecorated(false);
	treeWidget->resizeColumnToContents(0);
	treeWidget->header()->setSectionResizeMode(0, QHeaderView::Stretch);
	//treeWidget->setFocusPolicy(Qt::FocusPolicy::NoFocus);
	treeWidget->setSelectionMode(QAbstractItemView::NoSelection);

	treeWidget->verticalScrollBar()->setStyleSheet("QScrollBar::add-page:vertical { background: #000; }""QScrollBar::sub-page:vertical {background: #000;}""QScrollBar::add-page:horizontal { background: #000; }""QScrollBar::sub-page:horizontal {background: #000;}");
	treeWidget->horizontalScrollBar()->setStyleSheet("QScrollBar::add-page:vertical { background: #000; }""QScrollBar::sub-page:vertical {background: #000;}""QScrollBar::add-page:horizontal { background: #000; }""QScrollBar::sub-page:horizontal {background: #000;}");

	if (!_showWidget)
		treeWidget->hide();

	return treeWidget;
}

/*
* Description - CreateCheckBox() method used to create checkbox.
* Parameter - QString
* Exception -
* Return - QCheckBox
*/
QCheckBox* CVWidgetGenerator::CreateCheckBox(QString _toolTip, QString _style, bool _showWidget)
{
	QCheckBox* checkBox = new QCheckBox();

	checkBox->setToolTip(_toolTip);
	checkBox->setToolTip(_style);
	checkBox->setAttribute(Qt::WA_MacShowFocusRect, false);
	if (!_showWidget)
		checkBox->hide();

	return checkBox;
}

QWidget* CVWidgetGenerator::InsertWidgetInCenter(QWidget* _widget)
{
	QWidget* pWidget = new QWidget();
	QHBoxLayout* pLayout = new QHBoxLayout(pWidget);
	pLayout->addWidget(_widget);
	pLayout->setAlignment(Qt::AlignCenter);
	pLayout->setContentsMargins(10, 0, 10, 0);
	pWidget->setLayout(pLayout);
	return pWidget;
}



/*
* Description - CreateIconWidget() method used to create Icon related widget.
* Parameter -  json, QListWidget, QStringList, QStringList, string, string, bool
* Exception - exception, char *, string
* Return -
*/
QImage CVWidgetGenerator::ReadQImage(json _resultListJson, string _objectId, string _module)
{
	QImage styleIcon;
	string thumbnail;
	if (_module == COLOR_MODULE)
	{
		try
		{
			string rgbValue = Helper::GetJSONValue<string>(_resultListJson, RGB_VALUE_KEY, true);
			size_t found = rgbValue.find(".");
			if (found != string::npos)
			{
				Logger::Error("LOGGER::CVWidgetGenerator: CreateIconWidget() -> Image is not loaded.");
				QImageReader imageReader(":/CLOVise/PLM/Images/NoImage.png");
				imageReader.setDecideFormatFromContent(true);
				styleIcon = imageReader.read();

				return styleIcon;
			}
			rgbValue = Helper::FindAndReplace(rgbValue, "(", "");
			rgbValue = Helper::FindAndReplace(rgbValue, ")", "");
			rgbValue = Helper::FindAndReplace(rgbValue, " ", "");
			if (FormatHelper::HasContent(rgbValue))
			{
				QStringList listRGB;
				QString colorRGB = QString::fromStdString(rgbValue);
				listRGB = colorRGB.split(',');
				int red = listRGB.at(0).toInt();
				int green = listRGB.at(1).toInt();
				int blue = listRGB.at(2).toInt();
				QColor color(red, green, blue);
				QImage image(80, 80, QImage::Format_ARGB32);
				image.fill(color);
				styleIcon = image;
			}
		}
		catch (string msg)
		{
			throw string(msg);
		}
		catch (const char* msg)
		{
			throw msg;
		}
		catch (exception e)
		{
			throw e;
		}
	}
	else
	{
		//string thumbnail = Helper::GetJSONValue<string>(_resultListJson, THUMBNAIL_KEY, true);
		if (_module == PRINT_MODULE)
		{
			string images = Helper::GetJSONValue<string>(_resultListJson, "images", false);
			json imageIDJson = json::parse(images);
			string defaultImageID = Helper::GetJSONValue<string>(imageIDJson, "", true);
			
			thumbnail = UIHelper::GetPrintThumbnailUrl(defaultImageID);
		}
		else
		{
			thumbnail = UIHelper::GetThumbnailUrl(_objectId);
		}

		if (FormatHelper::HasContent(thumbnail))
		{
			// Bytestream to image conversion Commented 
			/*QByteArray imageBytes = QByteArray::fromBase64(thumbnail.c_str());
			pixmap.loadFromData(imageBytes);*/

			QByteArray imageBytes;
			auto startTime = std::chrono::high_resolution_clock::now();
			imageBytes = Helper::DownloadImageFromURL(thumbnail);
			auto finishTime = std::chrono::high_resolution_clock::now();
			std::chrono::duration<double> totalDuration = finishTime - startTime;
			Logger::perfomance(PERFOMANCE_KEY + "download Thambnail API :: " + to_string(totalDuration.count()));
			QBuffer buffer(&imageBytes);
			buffer.open(QIODevice::ReadOnly);
			QImageReader imageReader(&buffer);
			imageReader.setDecideFormatFromContent(true);
			styleIcon = imageReader.read();
		}
	}

	if (styleIcon.isNull())
	{
		Logger::Error("LOGGER::CVWidgetGenerator: CreateIconWidget() -> Image is not loaded.  ");
		QImageReader imageReader(":/CLOVise/PLM/Images/NoImage.png");
		imageReader.setDecideFormatFromContent(true);
		styleIcon = imageReader.read();
	}

	return styleIcon;
}

/*
* Description - CreateIconWidget() method used to create Icon related widget.
* Parameter -  json, QListWidget, QStringList, QStringList, string, string, bool
* Exception - exception, char *, string
* Return -
*/
QPixmap CVWidgetGenerator::ReadQPixmap(json _resultListJson, string _url, string _compatibility)
{
	QImage styleIcon = ReadQImage(_resultListJson, _url, _compatibility);

	return QPixmap::fromImage(styleIcon);
}

/*
* Description - CreateIconWidget() method used to create Icon related widget.
* Parameter -  json, QListWidget, QStringList, QStringList, string, string, bool
* Exception - exception, char *, string
* Return -
*/
void CVWidgetGenerator::CreateIconWidget(json _resultListJson, QListWidget* _iconWidget, QStringList _tootTipKeys, QStringList _tootTipKeyDisplay, string _compatibility, string _solidKey, bool _isReadiobutton, QPixmap _pixmap)
{
	Logger::Info("CVWidgetGenerator::CreateIconWidget() - Start");
	InitializeIconView(_iconWidget);

	string objectName, thumbnail, rgbValue, filenameToSave;
	objectName = Helper::GetJSONValue<string>(_resultListJson, OBJECT_NAME_KEY, true);
	string objectId = Helper::GetJSONValue<string>(_resultListJson, OBJECT_ID_KEY, true);
	rgbValue = Helper::GetJSONValue<string>(_resultListJson, RGB_VALUE_KEY, true);

	QPixmap newPixmap = _pixmap.scaled(QSize(80, 80), Qt::KeepAspectRatio);

	QIcon icon;
	icon.addPixmap(newPixmap, QIcon::Normal);
	icon.addPixmap(newPixmap, QIcon::Selected);
	icon.addPixmap(newPixmap, QIcon::Active);

	QListWidgetItem* item = new QListWidgetItem(icon, AddNewLinesToString(objectName, 10));
	item->setTextAlignment(Qt::AlignRight);
	string toolTipString = "";
	for (int i = 0; i < _tootTipKeys.size(); i++)
	{
		toolTipString = toolTipString + _tootTipKeyDisplay.at(i).toStdString() + ": ";
		string value = Helper::GetJSONValue<string>(_resultListJson, _tootTipKeys.at(i).toStdString(), true);
		toolTipString = toolTipString + value + "\n";
	}
	if (!(_compatibility.find(_solidKey) != string::npos))
	{
		item->setToolTip(QString::fromStdString(toolTipString));
	}
	else
	{
		item->setToolTip(RGB_TOOLTIP_KEY + QString::fromStdString(rgbValue) + "\n" + QString::fromStdString(toolTipString));
	}
	if (_isReadiobutton)
	{
		QRadioButton* radioButton = new QRadioButton;
		radioButton->setCheckable(true);
		radioButton->setText(" ");
		radioButton->setAttribute(Qt::WA_MacShowFocusRect, false);
		radioButton->setParent(_iconWidget);
		radioButton->setStyleSheet("QRadioButton{spacing: 7px;}""margin: 2px 5px 2px 1px;");
		radioButton->setProperty(COLOR_ID_KEY.c_str(), QString::fromStdString(objectId));
		radioButton->setProperty(OBJECT_ID_KEY.c_str(), QString::fromStdString(objectId));

		_iconWidget->addItem(item);
		_iconWidget->setItemWidget(item, radioButton);
	}
	else
	{
		QCheckBox* checkBox = new QCheckBox;
		checkBox->setCheckable(true);
		checkBox->setText(" ");
		checkBox->setAttribute(Qt::WA_MacShowFocusRect, false);
		checkBox->setStyleSheet("QCheckBox{spacing: 7px;}""margin: 2px 5px 2px 1px;");
		checkBox->setCheckState(Qt::Unchecked);
		checkBox->setProperty(COLOR_ID_KEY.c_str(), QString::fromStdString(objectId));
		checkBox->setProperty(OBJECT_ID_KEY.c_str(), QString::fromStdString(objectId));

		_iconWidget->addItem(item);
		_iconWidget->setItemWidget(item, checkBox);
	}
	Logger::Info("CVWidgetGenerator::CreateIconWidget() - End");
}

/*
* Description - CreateThumbnailWidget() method used to create thumbnail widget.
* Parameter -  json, string, string
* Exception - exception, char *, string
* Return - QWidget
*/
QWidget* CVWidgetGenerator::CreateThumbnailWidget(json _resultJson, QPixmap _pixmap, string _module)
{
	Logger::Info("CVWidgetGenerator::CreateThumbnailWidget() - Start");
	QWidget* pWidget = new QWidget();
	try
	{
		QLabel* label = new QLabel;
		label->setMaximumSize(QSize(70, 70));
		int w = label->width();
		int h = label->height();
		//label->setScaledContents(true);
		if (_module == COLOR_MODULE)
		{
			string rgbValue = "";
			rgbValue = Helper::GetJSONValue<string>(_resultJson, RGB_VALUE_KEY, true);
			label->setToolTip(QString::fromStdString(rgbValue));
		}
		label->setPixmap(QPixmap(_pixmap.scaled(w, h, Qt::KeepAspectRatio)));
		QHBoxLayout* pLayout = new QHBoxLayout(pWidget);
		pLayout->addWidget(label);
		pLayout->setAlignment(Qt::AlignCenter);
		pLayout->setContentsMargins(0, 0, 0, 0);
		pWidget->setLayout(pLayout);
	}
	catch (string msg)
	{
		throw string(msg);
	}
	catch (const char* msg)
	{
		throw msg;
	}
	catch (exception e)
	{
		throw e;
	}
	Logger::Info("CVWidgetGenerator::CreateThumbnailWidget() - End");
	return pWidget;
}
