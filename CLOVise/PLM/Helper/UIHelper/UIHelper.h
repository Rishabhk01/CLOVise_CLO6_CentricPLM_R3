#pragma once
/*
* Copyright 2020-2021 CLO-Vise. All rights reserved
*
* @file UIHelper.h
*
* @brief used to help ui related progress.
*
* @author GoVise
*
* @date 27-MAY-2020
*/
#include <iostream>
#include <fstream>
#include <sstream>
#include <sys/stat.h>
#include <time.h>

#include <QString>
#include <QStringList>
#include <QVariant>
#include <QTextStream>
#include <QFile>

#include "qfileinfo.h"
#include "CLOAPIInterface.h"
#include "classes/APICLODataBase.h"
#include "classes/APIStorage.h"
#include "classes/APIUtility.h"
#include "CLOVise/PLM/Inbound/Material/MaterialConfig.h"

#ifdef __APPLE__

#include <unistd.h>
#include <wchar.h>
#include <dirent.h>

#define _T(x)     x
#define TCHAR       char
#define _tremove    unlink
#define _stprintf   sprintf
#define _sntprintf vsnprintf
#define _tcsncpy wcsncpy
#define _tcscpy wcscpy
#define _tcscmp wcscmp
#define _tcsrchr wcsrchr
#define _tfopen fopen
#define _tcsncpy strncpy
#define _tcscpy strcpy
#define _tcscmp strcmp
#define _tcsrchr strrchr
#define _sntprintf snprintf

#else

#include <windows.h>
#include <direct.h>
#include <tchar.h>

#endif

#include <QLineEdit>
#include <QTextEdit>
#include <QDateTimeEdit>
#include <QComboBox>
#include <QLabel>

#include <qlistwidget.h>
#include <qtextedit.h>
#include <qstringlist.h>
#include <QDateEdit>


#include "CLOVise/PLM/Libraries/jsonQt.h"
#include "CLOVise/PLM/Libraries/json.h"
#include "CLOVise/PLM/Helper/Util/ComboBoxItem.h"
#include "CLOVise/PLM/Helper/UIHelper/CVWidgetGenerator.h"
#include "CLOVise/PLM/Helper/Util/RestAPIUtility.h"
#include "CLOVise/PLM/Helper/UIHelper/CVDisplayMessageBox.h"
#include "CLOVise/PLM/Inbound/Document/DocumentConfig.h"
#include "CLOVise/PLM/Inbound/Product/ProductConfig.h"
#include "CLOVise/PLM/Outbound/PublishToPLM/PublishToPLMData.h"
#include "CLOVise/PLM/Helper/Util/Logger.h"

/*
* Brief used to help ui related progress.
*/
using namespace std;
using QtJson::JsonObject;
using json = nlohmann::json;

namespace UIHelper
{
	/*
	* Description - ReadUIFieldValue() method used to read user input.
	* Parameter -  QTreeWidget, int.
	* Exception - 
	* Return - json.
	*/
	static json ReadUIFieldValue(QTreeWidget* _searchTree, int _rowIndex)
	{
		Logger::Info("UIHelper -> ReadUIFieldValue() -> Start");
		string fromFieldValue = "";
		string toFieldValue = "";
		int fromFieldValueInt = 0;
		int toFieldValueInt = 0;
		string fieldValue = "";
		string fieldLabel = "";
		string attKey = "attKey";
		json attJson = json::object();
		QTreeWidgetItem* topItem = _searchTree->topLevelItem(_rowIndex);

		QWidget* qWidgetColumn0 = _searchTree->itemWidget(topItem, 0);
		QLabel* qlabel = qobject_cast<QLabel*>(qWidgetColumn0);
		fieldLabel = qlabel->text().toStdString();
		fieldLabel = qlabel->property(attKey.c_str()).toString().toStdString();
		
		QWidget* qWidgetColumn1 = _searchTree->itemWidget(topItem, 1);
		QWidget* qWidgetColumn2 = _searchTree->itemWidget(topItem, 2);

		if (QLineEdit* qLineEditC1 = qobject_cast<QLineEdit*>(qWidgetColumn1)) {
			fieldValue = qLineEditC1->text().toStdString();
		}
		else if (QTextEdit* qTextC1 = qobject_cast<QTextEdit*>(qWidgetColumn1)) {
			fieldValue = qTextC1->toPlainText().toStdString();
		}
		else if (QDateEdit* qDateC1 = qobject_cast<QDateEdit*>(qWidgetColumn1)) {
			fromFieldValue = FormatHelper::FormatDate(qDateC1);
			if (fromFieldValue == "1/1/2000" || fromFieldValue == "1/1/00" || fromFieldValue == "01/01/2000" || fromFieldValue == "01/01/00")
			{
				fromFieldValue = "";
			}
			if (QDateEdit* qDateC2 = qobject_cast<QDateEdit*>(qWidgetColumn2)) {
				toFieldValue = FormatHelper::FormatDate(qDateC2);
				if (toFieldValue == "1/1/2000" || toFieldValue == "1/1/00" || toFieldValue == "01/01/2000" || toFieldValue == "01/01/00")
				{
					toFieldValue = "";
				}
			}
			if (FormatHelper::HasContent(fromFieldValue) || FormatHelper::HasContent(toFieldValue))
			{
				fieldValue = fromFieldValue + DELIMITER_NEGATION + toFieldValue;
			}
		}
		else if (QListWidget* listC1 = qobject_cast<QListWidget*>(qWidgetColumn1)) {
			string tempValue = "";
			QListWidgetItem* listItem = nullptr;
			for (int row = 0; row < listC1->count(); row++)
			{
				listItem = listC1->item(row);
				if (listItem->checkState())
				{
					tempValue = listItem->text().toStdString();
					if (FormatHelper::HasContent(tempValue))
					{
						tempValue = listC1->property(tempValue.c_str()).toString().toStdString();
						if (FormatHelper::HasContent(tempValue))
						{
							tempValue = tempValue + DELIMITER_NEGATION;
							fieldValue = fieldValue + tempValue;
						}
					}
				}
			}
		}
		else if (QDoubleSpinBox* doubleSpinC1 = qobject_cast<QDoubleSpinBox*>(qWidgetColumn1)) {
			if (doubleSpinC1->value() != 0.00000)
			{
				fromFieldValue = to_string(doubleSpinC1->value());
				fromFieldValueInt = doubleSpinC1->value();
			}
			if (QDoubleSpinBox* doubleSpinC2 = qobject_cast<QDoubleSpinBox*>(qWidgetColumn2)) {
				if (doubleSpinC2->value() != 0.00000)
				{					
					toFieldValue = to_string(doubleSpinC2->value());
					toFieldValueInt = doubleSpinC2->value();
					if (toFieldValueInt < fromFieldValueInt)
					{
						string error = "To: value cannot be less than From: value";
						throw logic_error(error);
					}
				}
			}
			if (FormatHelper::HasContent(fromFieldValue) || FormatHelper::HasContent(toFieldValue))
			{
				fieldValue = fromFieldValue + DELIMITER_NEGATION + toFieldValue;
			}
		}
		else if (QSpinBox* spinC1 = qobject_cast<QSpinBox*>(qWidgetColumn1)) {
			if (spinC1->value() != 0)
			{
				fromFieldValue = to_string(spinC1->value());
				fromFieldValueInt = spinC1->value();
			}
			if (QSpinBox* spinC2 = qobject_cast<QSpinBox*>(qWidgetColumn2)) {
				if (spinC2->value() != 0)
				{
					toFieldValue = to_string(spinC2->value());
					toFieldValueInt = spinC2->value();
					if (toFieldValueInt < fromFieldValueInt)
					{
						string error = "To: value cannot be less than From: value";
						throw logic_error(error);
					}
				}
			}
			if (FormatHelper::HasContent(fromFieldValue) || FormatHelper::HasContent(toFieldValue))
			{
				fieldValue = fromFieldValue + DELIMITER_NEGATION + toFieldValue;
			}
		}
		else if (QComboBox* qComboBoxC1 = qobject_cast<QComboBox*>(qWidgetColumn1)) {
			fieldValue = qComboBoxC1->currentText().toStdString();
			if (FormatHelper::HasContent(fieldValue))
			{
				fieldValue = qComboBoxC1->property(fieldValue.c_str()).toString().toStdString();				
			}			

		}
		else if (QCheckBox* qCheckBoxC1 = qobject_cast<QCheckBox*>(qWidgetColumn1))
		{
			//UTILITY_API->DisplayMessageBox("first check box comming inside::checkbox");
			if (qCheckBoxC1->checkState() == Qt::Checked)
			{
				fieldValue = "true";
				//UTILITY_API->DisplayMessageBox("first check box comming inside::fieldValue::" + fieldValue);
			}
			else
			{
				fieldValue = "false";
				//UTILITY_API->DisplayMessageBox("first check box comming inside::fieldValue::" + fieldValue);
			}

		}
		if (FormatHelper::HasContent(fieldValue))
		{
			attJson[ATTRIBUTE_KEY] = fieldLabel;
			attJson[ATTRIBUTE_VALUE_KEY] = fieldValue;
		}
		Logger::Info("UIHelper -> ReadUIFieldValue() -> End");
		return attJson;
	}

	/*
	* Description - ReadVisualUIFieldValue() method used to read user input.
	* Parameter -  QTreeWidget, int.
	* Exception - 
	* Return - json.
	*/
	static json ReadVisualUIFieldValue(QTreeWidget* _searchTree, int _rowIndex)
	{
		Logger::Debug("Utility ReadVisualUIFieldValue() start....");
		string fieldValue = "";
		string fieldLabel = "";
		string attKey = ATTRIBUTE_KEY;
		string attType = "";
		bool isFieldDisabled = false;

		json attJson = json::object();
		QTreeWidgetItem* topItem = _searchTree->topLevelItem(_rowIndex);

		QWidget* qWidgetColumn0 = _searchTree->itemWidget(topItem, 0);
		QLabel* qlabel = qobject_cast<QLabel*>(qWidgetColumn0);
		fieldLabel = qlabel->text().toStdString();
		fieldLabel = qlabel->property(attKey.c_str()).toString().toStdString();
		attType = qlabel->property(ATTRIBUTE_TYPE_KEY.c_str()).toString().toStdString();
		QWidget* qWidgetColumn1 = _searchTree->itemWidget(topItem, 1);
		if (!qWidgetColumn1->isEnabled()) {
			isFieldDisabled = true;
			fieldValue = "|@isDisabled@|";
		}
		if (!isFieldDisabled) {
			if (QLineEdit* qLineEditC1 = qobject_cast<QLineEdit*>(qWidgetColumn1)) {
				if (qLineEditC1->isEnabled()) {
					fieldValue = qLineEditC1->text().toStdString();
				}
				else {
					fieldValue = "|@isDisabled@|";
				}
			}
			else if (QTextEdit* qTextC1 = qobject_cast<QTextEdit*>(qWidgetColumn1))
			{
				fieldValue = qTextC1->toPlainText().toStdString();
			}
			else if (QDateEdit* qDateC1 = qobject_cast<QDateEdit*>(qWidgetColumn1))
			{
				fieldValue = FormatHelper::FormatDate(qDateC1);
				if (fieldValue == "1/1/2000" || fieldValue == "1/1/00" || fieldValue == "01/01/2000" || fieldValue == "01/01/00")
				{
					fieldValue = "";
				}
			}
			else if (QListWidget* listC1 = qobject_cast<QListWidget*>(qWidgetColumn1))
			{
				string tempValue = "";
				QListWidgetItem* listItem = nullptr;
				for (int row = 0; row < listC1->count(); row++)
				{
					listItem = listC1->item(row);
					if (listItem->checkState())
					{
						tempValue = listItem->text().toStdString();
						if (FormatHelper::HasContent(tempValue))
						{
							tempValue = listC1->property(tempValue.c_str()).toString().toStdString();
							if (FormatHelper::HasContent(tempValue))
							{
								tempValue = tempValue + DELIMITER_NEGATION;
								fieldValue = fieldValue + tempValue;
							}
						}
					}
				}
			}
			else if (QDoubleSpinBox* doubleSpinC1 = qobject_cast<QDoubleSpinBox*>(qWidgetColumn1))
			{
				if (doubleSpinC1->value() != 0.00000)
				{
					fieldValue = to_string(doubleSpinC1->value());
				}
			}
			else if (QSpinBox* SpinC1 = qobject_cast<QSpinBox*>(qWidgetColumn1))
			{
				if (SpinC1->value() != 0)
				{
					fieldValue = to_string(SpinC1->value());
				}
			}
			else if (QComboBox* qComboBoxC1 = qobject_cast<QComboBox*>(qWidgetColumn1))
			{
				fieldValue = qComboBoxC1->currentText().toStdString();
				fieldValue = qComboBoxC1->property(fieldValue.c_str()).toString().toStdString();
				if (attType == BOOLEAN_ATT_TYPE_KEY && !FormatHelper::HasContent(fieldValue))
					fieldValue = FalseValue;
			}
			else if (QCheckBox* qCheckBoxC1 = qobject_cast<QCheckBox*>(qWidgetColumn1))
			{
				//UTILITY_API->DisplayMessageBox("second check box comming inside::");
				if (qCheckBoxC1->checkState() == Qt::Checked)
				{
					fieldValue = "true";
					//UTILITY_API->DisplayMessageBox("second check box comming inside::fieldValue::" + fieldValue);
				}
				else
				{
					fieldValue = "false";
					//UTILITY_API->DisplayMessageBox("second check box comming inside::fieldValue::" + fieldValue);
				}
			}
		}

		if (!FormatHelper::HasContent(fieldValue))
		{
			fieldValue = BLANK;
		}
		attJson[ATTRIBUTE_KEY] = fieldLabel;
		attJson[ATTRIBUTE_VALUE_KEY] = fieldValue;
		attJson[ATTRIBUTE_TYPE_KEY] = attType;
		Logger::Debug("Utility ReadVisualUIFieldValue() End....");
		return attJson;
	}

	/*
	* Description - ReadSearchUIFieldValue() method used to read user input.
	* Parameter -  QTreeWidget, int.
	* Exception -
	* Return - json.
	*/
	inline json ReadSearchUIFieldValue(QTreeWidget* _searchTree, int _rowIndex)
	{
		Logger::Info("UIHelper -> ReadSearchUIFieldValue() -> Start");
		string fieldValue = "";
		string fieldLabel = "";
		string attKey = ATTRIBUTE_KEY;
		string attType = "";

		json attJson = json::object();
		QTreeWidgetItem* topItem = _searchTree->topLevelItem(_rowIndex);

		QWidget* qWidgetColumn0 = _searchTree->itemWidget(topItem, 0);
		QLabel* qlabel = qobject_cast<QLabel*>(qWidgetColumn0);
		fieldLabel = qlabel->text().toStdString();
		fieldLabel = qlabel->property(attKey.c_str()).toString().toStdString();
		attType = qlabel->property(ATTRIBUTE_TYPE_KEY.c_str()).toString().toStdString();
		QWidget* qWidgetColumn1 = _searchTree->itemWidget(topItem, 1);
		if (QLineEdit* qLineEditC1 = qobject_cast<QLineEdit*>(qWidgetColumn1)) {
			fieldValue = qLineEditC1->text().toStdString();
		}
		else if (QTextEdit* qTextC1 = qobject_cast<QTextEdit*>(qWidgetColumn1))
		{
			fieldValue = qTextC1->toPlainText().toStdString();
		}
		else if (QDateEdit* qDateC1 = qobject_cast<QDateEdit*>(qWidgetColumn1))
		{
			//fieldValue = FormatHelper::FormatDate(qDateC1);
			fieldValue = FormatHelper::RetrieveDate(qDateC1);
			//if (fieldValue == "1/1/2000" || fieldValue == "1/1/00" || fieldValue == "01/01/2000" || fieldValue == "01/01/00")
			//UTILITY_API->DisplayMessageBox("fieldValue::" + fieldValue);
			if (fieldValue.find(DATE_FORMAT_TEXT.toStdString()) != string::npos)
			{
				fieldValue = "";
			}
			else
			{
				//UTILITY_API->DisplayMessageBox("fieldValue::" + fieldValue);
				fieldValue = fieldValue + "T00:00:00Z";
			}
		}
		else if (QListWidget* listC1 = qobject_cast<QListWidget*>(qWidgetColumn1))
		{
			string tempValue = "";
			QListWidgetItem* listItem = nullptr;
			for (int row = 0; row < listC1->count(); row++)
			{
				listItem = listC1->item(row);
				if (listItem->checkState())
				{
					tempValue = listItem->text().toStdString();
					if (FormatHelper::HasContent(tempValue))
					{
						tempValue = listC1->property(tempValue.c_str()).toString().toStdString();
						if (FormatHelper::HasContent(tempValue))
						{	
							tempValue = tempValue + DELIMITER_NEGATION;
							fieldValue = fieldValue + tempValue;
						}
					}
				}
			}
		}
		else if (QDoubleSpinBox* doubleSpinC1 = qobject_cast<QDoubleSpinBox*>(qWidgetColumn1))
		{
			if (doubleSpinC1->value() != 0.00000)
			{
				fieldValue = to_string(doubleSpinC1->value());
			}
		}
		else if (QSpinBox* SpinC1 = qobject_cast<QSpinBox*>(qWidgetColumn1))
		{
			if (SpinC1->value() != 0)
			{
				fieldValue = to_string(SpinC1->value());
			}
		}
		else if (QComboBox* qComboBoxC1 = qobject_cast<QComboBox*>(qWidgetColumn1))
		{
			fieldValue = qComboBoxC1->currentText().toStdString();
			if (!FormatHelper::HasContent(fieldValue))
			{
				fieldValue = "";
			}
			else {
				fieldValue = qComboBoxC1->property(fieldValue.c_str()).toString().toStdString();
			}
			/*if (attType == BOOLEAN_ATT_TYPE_KEY && !FormatHelper::HasContent(fieldValue))
				fieldValue = FalseValue;*/
		}
		else if (QCheckBox* qCheckBoxC1 = qobject_cast<QCheckBox*>(qWidgetColumn1))
		{
			if (qCheckBoxC1->checkState() == Qt::Checked)
			{
				fieldValue = "true";
			}
		}
		if (FormatHelper::HasContent(fieldValue))
		{
			attJson[ATTRIBUTE_KEY] = fieldLabel;
			attJson[ATTRIBUTE_VALUE_KEY] = fieldValue;
			attJson[ATTRIBUTE_TYPE_KEY] = attType;
		}		
		Logger::Info("UIHelper -> ReadSearchUIFieldValue() -> End");
		return attJson;
	}

	/*
	* Description - CollectSearchCriteriaFields() method used to collect fields data in search UI.
	* Parameter -  QTreeWidget.
	* Exception - 
	* Return - json.
	*/
	inline json CollectSearchCriteriaFields(QTreeWidget* _searchTree)
	{
		Logger::Info("UIHelper -> CollectSearchCriteriaFields() -> Start");
		json attsJson = json::array();
		json attJson = json::object();
		for (int i = 0; i < _searchTree->topLevelItemCount(); i++)
		{
			attJson = ReadSearchUIFieldValue(_searchTree, i);
			if (!attJson.empty())
			{
				attsJson.push_back(attJson);
			}
		}
		Logger::Info("UIHelper -> CollectSearchCriteriaFields() -> End");
		return attsJson;
	}

	/*
	* Description - CollectSearchCriteriaFields() method used to collect fields data in search UI.
	* Parameter -  QTreeWidgets
	* Exception -
	* Return - json
	*/
	inline json CollectSearchCriteriaFields(QTreeWidget* _searchTree_1, QTreeWidget* _searchTree_2)
	{
		Logger::Info("UIHelper -> CollectSearchCriteriaFields() -> Start");
		json attsJson = json::array();
		json attJson = json::object();
		for (int i = 0; i < _searchTree_1->topLevelItemCount(); i++)
		{
			attJson = ReadSearchUIFieldValue(_searchTree_1, i);
			if (!attJson.empty())
			{
				attsJson.push_back(attJson);
			}
		}
		for (int i = 0; i < _searchTree_2->topLevelItemCount(); i++)
		{
			attJson = ReadSearchUIFieldValue(_searchTree_2, i);
			if (!attJson.empty())
			{
				attsJson.push_back(attJson);
			}
		}
		Logger::Info("UIHelper -> CollectSearchCriteriaFields() -> End");
		return attsJson;
	}

	/*
	* Description - ValidateForValidParams() method used to validate the user input from UI.
	* Parameter -  json, string.
	* Exception - 
	* Return -
	*/
	inline void ValidateForValidParams(json _searchCriteriaJson, string _uiModule)
	{
		Logger::Info("UIHelper -> ValidateForValidParams() -> Start");
		try
		{
			bool valid = true;
			if (!FormatHelper::HasContent(to_string(_searchCriteriaJson)))
			{
				valid = false;
			}
			string quickSearchValue = Helper::GetJSONValue<string>(_searchCriteriaJson, QUICK_SEARCK_CRITERIA_KEY, true);
			json attsJson = Helper::GetJSONParsedValue<string>(_searchCriteriaJson, ATTRIBUTES_KEY, false);
			if (_uiModule == PRODUCT_MODULE)
			{
				string seasonId = Helper::GetJSONValue<string>(_searchCriteriaJson, SEASONID_KEY, true);
				if (!FormatHelper::HasContent(quickSearchValue) && !FormatHelper::HasContent(seasonId) && attsJson.empty())
				{
					valid = false;
				}
			}
			else if (_uiModule == DOCUMENT_MODULE)
			{
				string productName = Helper::GetJSONValue<string>(_searchCriteriaJson, PRODUCTNAME_KEY, true);
				if (!FormatHelper::HasContent(quickSearchValue) && !FormatHelper::HasContent(productName) && attsJson.empty())
				{
					valid = false;
				}
			}
			else
			{
				string paletteId = Helper::GetJSONValue<string>(_searchCriteriaJson, PALETTEID_KEY, true);
				if (!FormatHelper::HasContent(quickSearchValue) && !FormatHelper::HasContent(paletteId) && attsJson.empty())
				{
					valid = false;
				}
			}
			if (!valid)
			{
				throw "Please enter atleast one criteria.";
			}
		}
		catch (string msg)
		{
			UTILITY_API->DeleteProgressBar(true);
			Logger::Error("UIHelper->GetEnumValues() Exception - " + msg);
			throw string(msg);
		}
		catch (exception & e)
		{
			UTILITY_API->DeleteProgressBar(true);
			Logger::Error("UIHelper -> GetEnumValues() Exception - " + string(e.what()));
			throw e;
		}
		catch (const char* msg)
		{
			UTILITY_API->DeleteProgressBar(true);
			wstring wstr(msg, msg + strlen(msg));
			Logger::Error("UIHelper -> GetEnumValues() Exception - " + string(msg));
			throw string(msg);
		}
		Logger::Info("UIHelper -> ValidateForValidParams() -> End");
	}

	/*
	* Description - CheckBoxChecked() method used to enable season and palette search.
	* Parameter -  bool, json, QComboBox, QComboBox, QLabel, QLabel, string.
	* Exception - exception, Char *
	* Return -
	*/
	inline void CheckBoxChecked(bool _checked, json _seasonPaletteJson, ComboBoxItem* _seasoncomboBox, QComboBox* _paletteComboBox, QLabel* _seasonLabel, QLabel* _paletteLabel, string _uiModule)
	{
		json readPaletteJsonValue = json::object();
		json activeSeasons = json::object();
		json activeSeasonsJson = json::object();
		QStringList seasonList;
		string seasonName = "", seasonId = "";
		try
		{
			if (_checked)
			{
				_seasonLabel->show();
				_seasoncomboBox->show();

				if (_seasonPaletteJson != Null_Value)
				{
					activeSeasons = Helper::GetJSONParsedValue<string>(_seasonPaletteJson, ACTIVESEASONS_JSON_KEY, false);
					for (int asCount = 0; asCount < activeSeasons.size(); asCount++)
					{
						activeSeasonsJson = Helper::GetJSONParsedValue<int>(activeSeasons, asCount, false);

						seasonName = Helper::GetJSONValue<string>(activeSeasonsJson, SEASONNAME_KEY, true);
						seasonId = Helper::GetJSONValue<string>(activeSeasonsJson, SEASONID_KEY, true);
						_seasoncomboBox->setProperty(seasonName.c_str(), QString::fromStdString(seasonId));
						if (_uiModule == PRODUCT_MODULE)
						{
							string producttypeValue = Helper::GetJSONValue<string>(activeSeasonsJson, PRODUCT_SEASON_TYPE_KEY, true);
							string productTypeKey = seasonName + "productType";
							_seasoncomboBox->setProperty(productTypeKey.c_str(), QString::fromStdString(producttypeValue));
						}
						seasonList << QString::fromStdString(seasonName);
					}
					CVWidgetGenerator::CreateComboBoxItem(false, _seasoncomboBox, 0, seasonList);
				}
			}
			else
			{
				_seasonLabel->hide();
				_seasoncomboBox->hide();
				if (_uiModule != PRODUCT_MODULE)
				{
					_paletteLabel->hide();
					_paletteComboBox->hide();
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
	}

	/*
	* Description - CheckHirarchyChiltedItem() method used to check child hierarchy.
	* Parameter -  QTreeWidgetItem, int, string.
	* Exception - 
	* Return -
	*/
	inline void CheckHirarchyChiltedItem(QTreeWidgetItem* _parent, int _level, string _productType)
	{
		for (int childIndex = 0; childIndex < _parent->childCount(); childIndex++)
		{
			QTreeWidgetItem* child = _parent->child(childIndex);
			string childName = child->text(0).toStdString();
			if (child->data(1, Qt::UserRole).toString().toStdString() == _productType)
			{
				child->setSelected(true);
				break;
			}
			else
				CheckHirarchyChiltedItem(child, _level + 1, _productType);
		}
	}
	
	/*
	* Description - SeasonChanged() method used to update the palette season changed.
	* Parameter -  string, json, QComboBox, QLabel.
	* Exception - exception, Char *
	* Return -
	*/
	static void SeasonChanged(string _selectedSeason, json _seasonPaletteJson, QComboBox * _paletteComboBox, QLabel * _paletteLabel)
	{
		_paletteLabel->show();
		_paletteComboBox->show();
		json subPaletteJson = json::object();
		json subPaletteNumberJson = json::object();
		json activeSeasons = json::object();
		json activeSeasonsJson = json::object();
		json palette = json::object();
		QStringList paletteList;
		string seasonName = "", paletteName = "", paletteId = "", subPaletteName = "", subPaletteId = "";
		try {
			if (_seasonPaletteJson != Null_Value)
			{
				activeSeasons = Helper::GetJSONParsedValue<string>(_seasonPaletteJson, ACTIVESEASONS_JSON_KEY, false);
				for (int asCount = 0; asCount < activeSeasons.size(); asCount++)
				{
					activeSeasonsJson = Helper::GetJSONParsedValue<int>(activeSeasons, asCount, false);
					seasonName = Helper::GetJSONValue<string>(activeSeasonsJson, SEASONNAME_KEY, true);
					if (_selectedSeason == BLANK.c_str())
					{
						_paletteComboBox->addItem(QString::fromStdString(BLANK));
						CVWidgetGenerator::CreateComboBox(false, _paletteComboBox, 0, paletteList);
						break;
					}
					else if (_selectedSeason == seasonName)
					{
						paletteName = Helper::GetJSONValue<string>(activeSeasonsJson, PALETTENAME_KEY, true);
						paletteId = Helper::GetJSONValue<string>(activeSeasonsJson, PALETTEID_KEY, true);

						_paletteComboBox->setProperty(paletteName.c_str(), QString::fromStdString(paletteId));
						if (!paletteList.contains(QString::fromStdString(paletteName)))
						{
							paletteList << QString::fromStdString(paletteName);
						}
						subPaletteJson = Helper::GetJSONParsedValue<string>(activeSeasonsJson, SUBPALETTE_JSON_KEY, false);
						for (int spCount = 0; spCount < subPaletteJson.size(); spCount++)
						{
							subPaletteNumberJson = Helper::GetJSONParsedValue<int>(subPaletteJson, spCount, false);
							string subPaletteName = Helper::GetJSONValue<string>(subPaletteNumberJson, SUB_PALETTENAME_KEY, true);
							string subPaletteId = Helper::GetJSONValue<string>(subPaletteNumberJson, PALETTEID_KEY, true);
							_paletteComboBox->setProperty(subPaletteName.c_str(), QString::fromStdString(subPaletteId));
							if (!paletteList.contains(QString::fromStdString(subPaletteName)))
							{
								paletteList << QString::fromStdString(subPaletteName);
							}
						}
						CVWidgetGenerator::CreateComboBox(false, _paletteComboBox, 0, paletteList);
						break;
					}
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
	}

	/*
	* Description - SeasonPaletteValidation() method used to validate the season palette.
	* Parameter -  string, string.
	* Exception - 
	* Return -
	*/
	inline void SeasonPaletteValidation(string _seasonValue, string _paletteValue)
	{
		if (!FormatHelper::HasContent(_seasonValue) && !FormatHelper::HasContent(_paletteValue))
		{
			throw "Please select a Season and Palette.";
		}
		else if (FormatHelper::HasContent(_seasonValue) && !FormatHelper::HasContent(_paletteValue))
		{
			throw "Please select a Palette.";
		}
	}

	/*
	* Description - ProductSeasonValidation() method used to validate season.
	* Parameter -  string.
	* Exception - 
	* Return -
	*/
	inline void ProductSeasonValidation(string _seasonValue)
	{
		if (!FormatHelper::HasContent(_seasonValue))
		{
			throw "Please select a Season.";
		}
	}

	/*
	* Description - ProductValidation() method used to validate the product.
	* Parameter -  string, string, bool.
	* Exception -
	* Return -
	*/
	inline void ProductValidation(string _nameValue, string _idValue, bool _prodIdKeyExists)
	{
		
		if (!_prodIdKeyExists)
		{
			if (!FormatHelper::HasContent(_nameValue))
			{
				throw "Please enter a value for Product Name.";
			}
		}
		else
		{
			if (!FormatHelper::HasContent(_idValue) && !FormatHelper::HasContent(_nameValue))
			{
				throw "Please enter a value for Product Name or Product Id.";
			}
		}
	}

	inline void MetadataUpdate(json _iconFieldsListJsonArray, json _resultListJson, json&  _metaDataJson)
	{
		
		for (int iconArrayCount = 0; iconArrayCount < _iconFieldsListJsonArray.size(); iconArrayCount++)
		{
			json fieldsJson = Helper::GetJSONParsedValue<int>(_iconFieldsListJsonArray, iconArrayCount, false);
			string internalName = Helper::GetJSONValue<string>(fieldsJson, ATTRIBUTE_KEY, true);
			string displaylName = Helper::GetJSONValue<string>(fieldsJson, ATTRIBUTE_NAME_KEY, true);
			if (FormatHelper::HasContent(Helper::GetJSONValue<string>(_resultListJson, internalName, true)))
			{
				_metaDataJson[displaylName] = Helper::GetJSONValue<string>(_resultListJson, internalName, true);
			}
		}
		//UTILITY_API->DisplayMessageBox("updated metadata" + to_string(metaDataJson));
	}

	static QString DownloadResults(json _downloadJson, string _module, json _resultJsonArray)
	{
	
		Logger::Debug("downloadJson::" + to_string(_downloadJson));
		Logger::Debug("_resultJsonArray::" + to_string(_resultJsonArray));
		json searchResultsJsonArray = Helper::GetJSONParsedValue<string>(_resultJsonArray, SEARCHRESULTS_KEY, false);
		Logger::Debug("searchResultsJsonArray::" + to_string(searchResultsJsonArray));
		string error = RESTAPI::CheckForErrorMsg(to_string(_downloadJson));
		if (FormatHelper::HasContent(error))
		{
			throw std::logic_error(error);
		}
		/*string objectTypeKey = Helper::GetJSONValue<string>(_downloadJson, COMP3D_KEY, true);
		string cloFabricKey = Helper::GetJSONValue<string>(_downloadJson, "cloFabricMaterialKey", true);
		string cloTrimKey = Helper::GetJSONValue<string>(_downloadJson, "cloTrimMaterialKey", true);
		string cloPrintsKey = Helper::GetJSONValue<string>(_downloadJson, "cloPrintAndPatternKey", true);
		string cloSolidKey = Helper::GetJSONValue<string>(_downloadJson, CLOSOLIDCOLOR_KEY, true);*/
		json downloadJsonArray = Helper::GetJSONParsedValue<string>(_downloadJson, ATTACHMENTS_KEY, false);
		//string cloDocumentKey = Helper::GetJSONValue<string>(_downloadJson, CLODOCUMENT_KEY, true);
		//json documentMetaDataListKey = Helper::GetJSONValue<string>(_downloadJson, DOCUMENT_METADATA_LIST_KEY, false);
		json attachmentsArray = json::array();
		json attachmentsListJson = json::object();
		json attachmentJson = json::object();
		string objectName = "";
		string documentName = "";
		string objectId = "";
		string objectTypeValue = "";
		string attachmentURL = "";
		string attachmentName = "";
		string rgbValue = "";
		QStringList attachmentNameSplit;
		QStringList faildesObjects;
		string fileName = "";
		string cloSideMenuSection = "";
		string fileExt = "";
		string filePath = "";
		int last;
		string	strSwatchName;
		Marvelous::CloApiRgb rgbValues;
		string	strRetSwatchColor;
		string  colorSwatchfilename;
		json metaDataJson = json::object();

	map<string, string> plmColorIDtoColorNameList = DataUtility::GetInstance()->GetPLMColorIDtoColorNameList();
	map<string, Marvelous::CloApiRgb> plmIDtoColorList = DataUtility::GetInstance()->GetPLMIDtoColorList();
	map<string, string> plmIDtoApiMetaDataList = DataUtility::GetInstance()->GetPLMIDtoApiMetaDataList();

		for (int i = 0; i < downloadJsonArray.size(); i++)
		{
			metaDataJson.clear();
			attachmentsListJson = Helper::GetJSONParsedValue<int>(downloadJsonArray, i, false);

			objectName = Helper::GetJSONValue<string>(attachmentsListJson, OBJECT_NAME_KEY, true);
			objectName = Helper::Trim(objectName);
			objectId = Helper::GetJSONValue<string>(attachmentsListJson, OBJECT_ID_KEY, true);
			Logger::Logger("objectId::" + objectId + " ::objectName::" + objectName);
			if (!FormatHelper::HasContent(objectName) || !FormatHelper::HasContent(objectId))
			{
				faildesObjects.push_back(QString::fromStdString(objectName) + " - Doesn't have valid Name or Id.");
				continue;
			}
			if (_module == COLOR_MODULE)
			{
				/*if (!COLOR_SUPPORTING_LIST.contains(QString::fromStdString(fileExt)))
				{*/
				rgbValue = Helper::GetJSONValue<string>(attachmentsListJson, RGB_VALUE_KEY, true);
				rgbValue = Helper::FindAndReplace(rgbValue, "(", "");
				rgbValue = Helper::FindAndReplace(rgbValue, ")", "");
				rgbValue = Helper::FindAndReplace(rgbValue, " ", "");
				if (!FormatHelper::HasContent(rgbValue))
				{
					faildesObjects.push_back(QString::fromStdString(objectName) + " - Doesn't have valid RGB Code.");
					continue;
				}
				for (int SearchArrayCount = 0; SearchArrayCount < searchResultsJsonArray.size(); SearchArrayCount++)
				{
					string resultListStr = Helper::GetJSONValue<int>(searchResultsJsonArray, SearchArrayCount, false);
					json resultListJson = json::parse(resultListStr);
					Logger::Logger("download resultListJson::" + to_string(resultListJson));
					string SearchObjectId = Helper::GetJSONValue<string>(resultListJson, OBJECT_ID_KEY, true);
					Logger::Debug("objectId::" + objectId + " ::SearchObjectId::" + SearchObjectId);
					if (objectId != SearchObjectId)
						continue;
					for (const auto& itrValues : resultListJson.items())
					{
						string value = itrValues.value();
						if (!FormatHelper::HasContent(value))
							value = "";
						if (!Configuration::GetInstance()->GetExcludedPreviewFields().contains(QString::fromStdString(itrValues.key())))
						{
							metaDataJson[itrValues.key()] = value;
						}
					}
					metaDataJson[OBJECT_ID_KEY] = objectId;
				}
				string apiMetaData = to_string(metaDataJson);
				//Write code to download RGB Colors, Use RGB Value
				QStringList listRGB;
				QString colorRGB = QString::fromStdString(rgbValue);
				listRGB = colorRGB.split(',');
				rgbValues.R = listRGB.at(0).toInt();
				rgbValues.G = listRGB.at(1).toInt();
				rgbValues.B = listRGB.at(2).toInt();
				plmColorIDtoColorNameList.insert(make_pair(objectId, objectName)); // code and name
				plmIDtoColorList.insert(make_pair(objectId, rgbValues));	//code and RGB list
				plmIDtoApiMetaDataList.insert(make_pair(objectId, apiMetaData));
				strSwatchName = COLOR_SWATCH_NAME;
				strRetSwatchColor = UTILITY_API->AddColorSwatch(plmColorIDtoColorNameList, plmIDtoColorList, plmIDtoApiMetaDataList, strSwatchName);
				colorSwatchfilename = strRetSwatchColor;
				size_t index1 = 0;
				while (true)
				{
					index1 = colorSwatchfilename.find("/", index1);
					if (index1 == std::string::npos) break;
					colorSwatchfilename.replace(index1, 1, "\\\\");
					index1 += 1;
				}
				CLOAPISample::APIStorage::getInstance()->SendFileName(strRetSwatchColor);
				/*}*/
			}
			else
			{
				attachmentJson = Helper::GetJSONParsedValue<string>(attachmentsListJson, ATTACHMENT_KEY, false);
				attachmentURL = Helper::GetJSONValue<string>(attachmentJson, ATTACHMENT_URL_KEY, true);
				attachmentName = Helper::GetJSONValue<string>(attachmentJson, ATTACHMENT_NAME_KEY, true); 
				//documentName = Helper::GetJSONValue<string>(attachmentsListJson, DOCUMENT_NAME, true);
				//UTILITY_API->DisplayMessageBox("documentName::" + documentName);
				try
				{
					last = attachmentName.find_last_of('.');
					Logger::Debug("last - " + last);
					fileExt = attachmentName.substr(last + 1);
					Logger::Debug("fileExt - " + fileExt);
					if (!FormatHelper::HasContent(fileExt)) {
						faildesObjects.push_back(QString::fromStdString(objectName));
						continue;
					}

					//fileName = documentName + "." + fileExt;
					fileName = attachmentName;
					//Logger::Debug("fileName - " + fileName);
					fileName = Helper::GetValidString(fileName, INVALID_STRINGS_CHARS);
					Logger::Debug("fileName - " + fileName);
					Logger::Debug("attachmentName - " + attachmentName);
					if (!FormatHelper::HasContent(attachmentURL))
					{
						faildesObjects.push_back(QString::fromStdString(objectName) + " - Doesn't have valid Attachment.");
						continue;
					}
					for (int SearchArrayCount = 0; SearchArrayCount < searchResultsJsonArray.size(); SearchArrayCount++)
					{
						string resultListStr = Helper::GetJSONValue<int>(searchResultsJsonArray, SearchArrayCount, false);
						json resultListJson = json::parse(resultListStr);
						Logger::Logger("download resultListJson::" + to_string(resultListJson));
						string SearchObjectId = Helper::GetJSONValue<string>(resultListJson, OBJECT_ID_KEY, true);
						Logger::Debug("objectId::" + objectId + " ::SearchObjectId::" + SearchObjectId);
						if (objectId == SearchObjectId)
						{
							if (_module == COLOR_MODULE)
							{
								if (COLOR_SUPPORTING_LIST.contains(QString::fromStdString(fileExt), Qt::CaseInsensitive))
									filePath = DirectoryUtil::GetColorsAssetsDirectory();
							}
							else if (_module == MATERIAL_MODULE)
							{
								if (MATERIAL_SUPPORTING_LIST.contains(QString::fromStdString(fileExt), Qt::CaseInsensitive))
									filePath = DirectoryUtil::GetMaterialAssetsDirectory();
								else if (TRIM_SUPPORTING_LIST.contains(QString::fromStdString(fileExt), Qt::CaseInsensitive))
									filePath = DirectoryUtil::GetTrimAssetsDirectory();
							}
							else if (_module == STYLE_MODULE)
							{
								if (PRODUCT_SUPPORTING_LIST.contains(QString::fromStdString(fileExt), Qt::CaseInsensitive))
									filePath = DirectoryUtil::GetStyleAttachmentDirectory();
								filePath = filePath + attachmentName;
							}
							else if (_module == PRINT_MODULE)
							{
								if (COLOR_SUPPORTING_LIST.contains(QString::fromStdString(fileExt), Qt::CaseInsensitive))
									filePath = DirectoryUtil::GetPrintAssetsDirectory();
							}
							else
							{
								faildesObjects.push_back(QString::fromStdString(objectName));
								continue;
							}
							if (QString::fromStdString(filePath).isEmpty())
							{
								faildesObjects.push_back(QString::fromStdString(objectName));
								continue;
							}
							if (_module != STYLE_MODULE)
								filePath = filePath + fileName;
							Logger::Debug("filePath -" + filePath);
							Logger::Debug("filePathfilePath::" + filePath);
							Logger::Debug("attachmentURLfilePath::" + attachmentURL);
							Helper::DownloadFilesFromURL(attachmentURL, filePath);
							Logger::Debug("filePathfilePathfilePath::" + filePath);
							std::replace(filePath.begin(), filePath.end(), '\\', '/');
							//GetAPIMetadata from downloaded file for update metadata.							
							string metaData = UTILITY_API->GetAPIMetaData(filePath);
							Logger::Debug("metaData:filePath:" + metaData);							
							bool writtenToFile = true;
							if (METADATA_SUPPORTED_FILE_LIST.contains(QString::fromStdString(fileExt), Qt::CaseInsensitive))
							{
								if (_module == STYLE_MODULE)
								{
									ProductConfig::GetInstance()->SetDownloadFileName(filePath);
								}
								//ProductConfig::GetInstance()->SetDownloadFileName(filePath);
								//for now don't know what exactily we need to write into the downloaded file, so need to get an info from PLM team.
								for (const auto& itrValues : resultListJson.items())
								{
									string value = itrValues.value();
									if (!FormatHelper::HasContent(value))
										value = "";
									if (!Configuration::GetInstance()->GetExcludedPreviewFields().contains(QString::fromStdString(itrValues.key())))
									{

										metaDataJson[itrValues.key()] = value;
										//UTILITY_API->DisplayMessageBox(to_string(metaDataJson));
									}
								}
								metaDataJson[OBJECT_ID_KEY] = SearchObjectId;
								/*for (int iconArrayCount = 0; iconArrayCount < resultListJson.size(); iconArrayCount++)
								{
									json fieldsJson = Helper::GetJSONParsedValue<int>(iconFieldsListJsonArray, iconArrayCount, false);
									string internalName = Helper::GetJSONValue<string>(fieldsJson, ATTRIBUTE_KEY, true);
									string displaylName = Helper::GetJSONValue<string>(fieldsJson, ATTRIBUTE_NAME_KEY, true);
									if (FormatHelper::HasContent(Helper::GetJSONValue<string>(resultListJson, internalName, true)))
									{
										metaDataJson[displaylName] = Helper::GetJSONValue<string>(resultListJson, internalName, true);
									}
								}*/
								writtenToFile = UTILITY_API->SetAPIMetaData(filePath, to_string(metaDataJson));
							}

						if (!writtenToFile)
						{
							faildesObjects.push_back(QString::fromStdString(objectName) + " - Error while writing data into file.");
							//Delete filePath if any error found.
							int result = remove(filePath.c_str());
							continue;
						}
						Logger::Debug("filePath last -" + filePath);
						break;
					}

				}
			}
			catch (exception e)
			{
				Logger::Error(e.what());
				faildesObjects.push_back(QString::fromStdString(objectName));
				continue;
			}
		}
	}
	QString faildesObjectsString = faildesObjects.join(", ");
	Logger::Debug("filePath faildesObjectsString -" + faildesObjectsString.toStdString());
	DataUtility::GetInstance()->SetPLMColorIDtoColorNameList(plmColorIDtoColorNameList);
	DataUtility::GetInstance()->SetPLMIDtoApiMetaDataList(plmIDtoApiMetaDataList);
	DataUtility::GetInstance()->SetPLMIDtoColorList(plmIDtoColorList);
	return faildesObjectsString;
}

	/*
	* Description - DownloadResults() method used to download selected item using rest.
	* Parameter -  QStringList, string, json.
	* Exception - 
	* Return - QString
	*/
	inline QString DownloadResults(QStringList _downloadIdList, string _module,json _resultJsonArray)
	{
		
		string latestVersionAttUrl;
		string latestVersionAttName;
		string documentName;
		string documentId;
		json downloadJson = json::object();
		json searchArray;
		//UTILITY_API->DisplayMessageBox("_downloadIdList::" + _downloadIdList.join(",").toStdString());
		
		for (int ResultsjsonCount = 0; ResultsjsonCount < _resultJsonArray.size(); ResultsjsonCount++)
		{
			latestVersionAttUrl = BLANK;
			latestVersionAttName = BLANK;
			documentName = BLANK;
			documentId = BLANK;
			json fieldsJson = Helper::GetJSONParsedValue<int>(_resultJsonArray, ResultsjsonCount, false);
			string objectId = Helper::GetJSONValue<string>(fieldsJson, "objectId", true);
			string defaultAttachmentId = Helper::GetJSONValue<string>(fieldsJson, DFAULT_ASSET_KEY, true);

			for (int i = 0; i < _downloadIdList.size(); i++)
			{
				if (_downloadIdList[i] == QString::fromStdString(objectId))
				{
					if (Configuration::GetInstance()->GetCurrentScreen() == UPDATE_MATERIAL_CLICKED || Configuration::GetInstance()->GetCurrentScreen() == CREATE_PRODUCT_CLICKED)
					{
						MaterialConfig::GetInstance()->SetUpdateMaterialCacheData(fieldsJson);
						return "";
					}

					//string objectName = Helper::GetJSONValue<string>(fieldsJson, OBJECT_NAME_KEY, true);
					string rgbValue = Helper::GetJSONValue<string>(fieldsJson, RGB_VALUE_KEY, true);
					string objectName = Helper::GetJSONValue<string>(fieldsJson, OBJECT_NAME_KEY, true);
					string code = Helper::GetJSONValue<string>(fieldsJson, "code", true);
					string type = Helper::GetJSONValue<string>(fieldsJson, "material_type", true);
					string description = Helper::GetJSONValue<string>(fieldsJson, "description", true);
					//_previewjsonarray[ResultsjsonCount] = fieldsJson;
					json searchArrayFields;	
						searchArrayFields["objectId"] = objectId;
						searchArrayFields[OBJECT_NAME_KEY] = objectName;
						searchArrayFields["Name"] = objectName;
						searchArrayFields["Code"] = code;
						if (_module != COLOR_MODULE && _module!=PRINT_MODULE)
							searchArrayFields["Type"] = type;
						searchArrayFields["Description"] = description;
						searchArray["SearchResults"][i] = searchArrayFields;
					if (_module != COLOR_MODULE)
					{
						//auto startTime = std::chrono::high_resolution_clock::now();
						//string atttachmentResponse = RESTAPI::CentricRestCallGet(Configuration::GetInstance()->GetPLMServerURL() + RESTAPI::ATTACHMENTS_RESULTS_API + materialId, APPLICATION_JSON_TYPE, "");
						//auto finishTime = std::chrono::high_resolution_clock::now();
						//std::chrono::duration<double> totalDuration = finishTime - startTime;
						//Logger::perfomance(PERFOMANCE_KEY + "Documents API :: " + to_string(totalDuration.count()));
						////UTILITY_API->DisplayMessageBox("atttachmentResponse::"+atttachmentResponse);
						//if (!FormatHelper::HasContent(atttachmentResponse))
						//{
						//	throw "Unable to download, please try again or contact your System Administrator.";
						//}

						//if (FormatHelper::HasError(atttachmentResponse))
						//{
						//	string response;
						//	if (QString::fromStdString(atttachmentResponse).contains("message"))
						//	{
						//		json errorRsponse = json::parse(atttachmentResponse);
						//		response = Helper::GetJSONValue<string>(errorRsponse, "message", true);
						//	}
						//	throw runtime_error(response);
						//}
						//json materialAttachmentjson;
						//if (FormatHelper::HasContent(atttachmentResponse))
						//	materialAttachmentjson = json::parse(atttachmentResponse);
						//string latestVersionAtt = Helper::GetJSONValue<string>(materialAttachmentjson, "latest_revision", true);
						//documnetName = Helper::GetJSONValue<string>(materialAttachmentjson, NODE_NAME_KEY, true);
						string attachemntRevisionApi = BLANK;
						if (Configuration::GetInstance()->GetCurrentScreen() == SEARCH_MATERIAL_CLICKED || Configuration::GetInstance()->GetCurrentScreen() == UPDATE_MATERIAL_CLICKED)
						{
							string supportedExt = MATERIAL_FILTER_LIST.join(",").toStdString();
							attachemntRevisionApi = Configuration::GetInstance()->GetPLMServerURL() + RESTAPI::ATTACHMENTS_LATEST_REVISION_RESULTS_API + objectId + "?revision_details=true&file_ext=" + supportedExt;
						}
						else if (Configuration::GetInstance()->GetCurrentScreen() == SEARCH_PRODUCT_CLICKED || Configuration::GetInstance()->GetCurrentScreen() == COPY_PRODUCT_CLICKED)
						{
							attachemntRevisionApi = Configuration::GetInstance()->GetPLMServerURL() + RESTAPI::ATTACHMENTS_LATEST_REVISION_RESULTS_API + objectId + "?revision_details=true&file_ext="+ ZPRJ;
						}
						else
						{
							attachemntRevisionApi = Configuration::GetInstance()->GetPLMServerURL() + RESTAPI::ATTACHMENTS_LATEST_REVISION_RESULTS_API + objectId + "?revision_details=true";
						}
						auto latestStartTime = std::chrono::high_resolution_clock::now();						
						string attachmentResponse = RESTAPI::CentricRestCallGet(attachemntRevisionApi + "&decode=true", APPLICATION_JSON_TYPE, "");
						auto latestVersionApiFinishTime = std::chrono::high_resolution_clock::now();
						std::chrono::duration<double> latestVersionApiTotalDuration = latestVersionApiFinishTime - latestStartTime;
						Logger::perfomance(PERFOMANCE_KEY + "Documents Revisions API :: " + to_string(latestVersionApiTotalDuration.count()));
						Logger::RestAPIDebug("attachment response::" + attachmentResponse);
						if (!FormatHelper::HasContent(attachmentResponse))
						{
							throw "Unable to download, please try again or contact your System Administrator.";
						}
						if (FormatHelper::HasError(attachmentResponse))
						{
							Helper::GetCentricErrorMessage(attachmentResponse);
							throw runtime_error(attachmentResponse);
						}
						json attachmentjson;
						if (FormatHelper::HasContent(attachmentResponse))
							attachmentjson = json::parse(attachmentResponse);

						if (_module == PRINT_MODULE && attachmentjson.empty())
						{
							string parentId = Helper::GetJSONValue<string>(fieldsJson, "parent", true);
							if (FormatHelper::HasContent(parentId))
							{
								attachemntRevisionApi = Configuration::GetInstance()->GetPLMServerURL() + RESTAPI::ATTACHMENTS_LATEST_REVISION_RESULTS_API + parentId + "?revision_details=true";
								attachmentResponse = RESTAPI::CentricRestCallGet(attachemntRevisionApi + "&decode=true", APPLICATION_JSON_TYPE, "");
								if (!FormatHelper::HasContent(attachmentResponse))
								{
									throw "Unable to download, please try again or contact your System Administrator.";
								}
								if (FormatHelper::HasError(attachmentResponse))
								{
									Helper::GetCentricErrorMessage(attachmentResponse);
									throw runtime_error(attachmentResponse);
								}
								if (FormatHelper::HasContent(attachmentResponse))
								{
									attachmentjson = json::parse(attachmentResponse);
								}
							}
						}

						for (int attachmenAarrayCount = 0; attachmenAarrayCount < attachmentjson.size(); attachmenAarrayCount++)
						{
							json attachmentCountJson = Helper::GetJSONParsedValue<int>(attachmentjson, attachmenAarrayCount, false);
							documentName = Helper::GetJSONValue<string>(attachmentCountJson, NODE_NAME_KEY, true);
							string currentAttachmentId = Helper::GetJSONValue<string>(attachmentCountJson, "id", true);
							Logger::Logger("currentAttachmentId::" + currentAttachmentId + " ::defaultAttachmentId::" + defaultAttachmentId);
							if (currentAttachmentId == defaultAttachmentId)
							{
								string latestRevisionId = Helper::GetJSONValue<string>(attachmentCountJson, "latest_revision", true);
								json revisionDetailsJson = Helper::GetJSONParsedValue<string>(attachmentCountJson, "revision_details", false);
								Logger::Debug("revisionDetailsJson::" + to_string(revisionDetailsJson));
								for (int revisionCount = 0; revisionCount < revisionDetailsJson.size(); revisionCount++)
								{
									if (latestRevisionId != Helper::GetJSONValue<string>(revisionDetailsJson[revisionCount], "id", true))
										continue;
									string latestVersionAttId = Helper::GetJSONValue<string>(revisionDetailsJson[revisionCount], "file", true);
									latestVersionAttName = Helper::GetJSONValue<string>(revisionDetailsJson[revisionCount], "file_name", true);

									latestVersionAttUrl = Helper::GetJSONValue<string>(revisionDetailsJson[revisionCount], "_url_base_template", true);
									
									latestVersionAttUrl = Helper::FindAndReplace(latestVersionAttUrl, "%s", latestVersionAttId);

									//latestVersionAttUrl = latestVersionAttUrl + latestVersionAttId;
									Logger::Debug("latestVersionAttName::" + latestVersionAttName + "::latestVersionAttId::" + latestVersionAttId + "  ::latestVersionAttUrl::" + latestVersionAttUrl);
									documentId = latestRevisionId;
									break;
								}
								break;
							}
							
							else
							{
								string latestRevisionId = Helper::GetJSONValue<string>(attachmentCountJson, "latest_revision", true);
								json revisionDetailsJson = Helper::GetJSONParsedValue<string>(attachmentCountJson, "revision_details", false);
								Logger::Logger("revisionDetailsJson::" + to_string(revisionDetailsJson));
								for (int revisionCount = 0; revisionCount < revisionDetailsJson.size(); revisionCount++)
								{
									if (latestRevisionId != Helper::GetJSONValue<string>(revisionDetailsJson[revisionCount], "id", true))
										continue;
									string latestVersionAttId = Helper::GetJSONValue<string>(revisionDetailsJson[revisionCount], "file", true);
									latestVersionAttName = Helper::GetJSONValue<string>(revisionDetailsJson[revisionCount], "file_name", true);

									latestVersionAttUrl = Helper::GetJSONValue<string>(revisionDetailsJson[revisionCount], "_url_base_template", true);
									
									latestVersionAttUrl = Helper::FindAndReplace(latestVersionAttUrl, "%s", latestVersionAttId);

									//latestVersionAttUrl = latestVersionAttUrl + latestVersionAttId;
									Logger::Logger("latestVersionAttName::" + latestVersionAttName + "::latestVersionAttId::" + latestVersionAttId + "  ::latestVersionAttUrl::" + latestVersionAttUrl);
									documentId = latestRevisionId;
									break;
								}
								break;
							}
						}
						
					}			

					if (!FormatHelper::HasContent(documentName))
						documentName = objectName;
					json attFields;
					json fieldsJson;
					attFields["attachmentURL"] = latestVersionAttUrl;
					attFields["attachmentName"] = latestVersionAttName;
					attFields["docName"] = documentName;
					attFields["docId"] = documentId;
					fieldsJson["attachment"] = attFields;
					fieldsJson["objectId"] = objectId;
					fieldsJson["objectName"] = objectName;
					fieldsJson[RGB_VALUE_KEY] = rgbValue;
					downloadJson["attachments"][i] = fieldsJson;
					//downloadJson["productIdKeyExist"] = false;
					//downloadJson["productNameKey"] = "productName";
					//downloadJson["productIdKey"] = "productId";
				}
			}
		}
		_resultJsonArray =searchArray;
		//UTILITY_API->DisplayMessageBox("final _previewjsonarray::" + to_string(_previewjsonarray));
		//UTILITY_API->DisplayMessageBox("downloadJson full::" + to_string(downloadJson));
		return DownloadResults(downloadJson, _module, _resultJsonArray);
	}

	/*
	* Description - AddProductMetaDataIn3DModelFile() method used to add the metadata of the downloaded product to 3d model.
	* Parameter -  json, json.
	* Exception - 
	* Return - json.
	*/
	inline json AddProductMetaDataIn3DModelFile(json _metadataJSON, json _resultJSON)
	{

		string productId = Helper::GetJSONValue<string>(_resultJSON, PRODUCTID_KEY, true);
		string productObjectId = Helper::GetJSONValue<string>(_resultJSON, PRODUCT_OBJ_ID, true);
		string productName = Helper::GetJSONValue<string>(_resultJSON, PRODUCT_NAME_KEY, true);
		string productStatus = Helper::GetJSONValue<string>(_resultJSON, PRODUCT_STATUS_KEY, true);

		_metadataJSON[PRODUCT_ID] = productId;
		_metadataJSON[PRODUCT_OBJ_ID] = productObjectId;
		_metadataJSON[PRODUCT_NAME] = productName;
		_metadataJSON[PRODUCT_STATUS] = productStatus;

		PublishToPLMData::GetInstance()->SetActiveProductId(productId);
		PublishToPLMData::GetInstance()->SetActiveProductObjectId(productObjectId);
		PublishToPLMData::GetInstance()->SetActiveProductName(productName);
		PublishToPLMData::GetInstance()->SetActiveProductStatus(productStatus);
		return _metadataJSON;
	}

	/*
	* Description - Add3DModelMetaDataIn3DModelFile() method used to add 3d model metadata to file.
	* Parameter -  json, json, json, QStringList.
	* Exception - 
	* Return - json.
	*/
	inline json Add3DModelMetaDataIn3DModelFile(json _metadataJSON, json _resultJSON, json _documentMetadataJSON, QStringList _attScopes)
	{
		json attJson = json::object();
		string attName, attKey, attScope, attValue;
		_metadataJSON[OBJECT_ID] = Helper::GetJSONValue<string>(_resultJSON, OBJECT_ID_KEY, true);
		_metadataJSON[OBJECT_NAME] = Helper::GetJSONValue<string>(_resultJSON, OBJECT_NAME_KEY, true);
		_metadataJSON[OBJECT_STATUS] = Helper::GetJSONValue<string>(_resultJSON, OBJECT_STATUS_KEY, true);

		_metadataJSON[OBJECT_ID_KEY] = Helper::GetJSONValue<string>(_resultJSON, OBJECT_ID_KEY, true);
		_metadataJSON[DOCUMENTID_KEY] = Helper::GetJSONValue<string>(_resultJSON, OBJECT_ID_KEY, true);
		_metadataJSON[OBJECT_NAME_KEY] = Helper::GetJSONValue<string>(_resultJSON, OBJECT_NAME_KEY, true);
		_metadataJSON[OBJECT_STATUS_KEY] = Helper::GetJSONValue<string>(_resultJSON, OBJECT_STATUS_KEY, true);

		PublishToPLMData::GetInstance()->Set3DModelObjectId(Helper::GetJSONValue<string>(_resultJSON, OBJECT_ID_KEY, true));

		for (int i = 0; i < _documentMetadataJSON.size(); i++)
		{
			Logger::Debug("documentMetadataJSON - "+to_string(_documentMetadataJSON));
			attJson = Helper::GetJSONParsedValue<int>(_documentMetadataJSON, i, false);
			attScope = Helper::GetJSONValue<string>(attJson, ATTRIBUTE_ATTSCOPE_KEY, true);
			if (!_attScopes.isEmpty() && !_attScopes.contains(QString::fromStdString(attScope)))
			{
				continue;
			}
			attKey = Helper::GetJSONValue<string>(attJson, ATTRIBUTE_KEY, true);
			attName = Helper::GetJSONValue<string>(attJson, ATTRIBUTE_NAME_KEY, true);
			attValue = Helper::GetJSONValue<string>(_resultJSON, attKey, true);
			_metadataJSON[attName] = attValue;
		}
		return _metadataJSON;
	}

	/*
	* Description - AddMetaDataIn3DModelFile() method used to add meta data to 3d model file.
	* Parameter -  json, json, json, QStringList.
	* Exception - 
	* Return - json.
	*/
	inline json AddMetaDataIn3DModelFile(json _metadataJSON, json _resultsJSON, json _documentMetadataJSON, QStringList _attScopes)
	{
		Logger::Debug("AddMetaDataIn3DModelFile() metadataJSON start - " + to_string(_metadataJSON));
		json resultJSON = Helper::GetJSONParsedValue<int>(_resultsJSON, 0, false);
		Logger::Debug("AddMetaDataIn3DModelFile() resultJSON - " + to_string(resultJSON));
		Logger::Debug("AddMetaDataIn3DModelFile() after resultJSON - " + to_string(resultJSON));
		_metadataJSON = AddProductMetaDataIn3DModelFile(_metadataJSON, resultJSON);
		Logger::Debug("AddMetaDataIn3DModelFile() after 2 resultJSON - " + to_string(resultJSON));
		_metadataJSON = Add3DModelMetaDataIn3DModelFile(_metadataJSON, resultJSON, _documentMetadataJSON, _attScopes);
		Logger::Debug("AddMetaDataIn3DModelFile() after 3 resultJSON - " + to_string(resultJSON));
		return _metadataJSON;
	}

	/*
	* Description - Rename3DWindowTitle() method used to re name the 3d window title.
	* Parameter -  string, string, string.
	* Exception - 
	* Return -
	*/
	inline void Rename3DWindowTitle(string _filePath, string _fileName, string _productName)
	{
		string fileExt = "";
		int last = _filePath.find_last_of('.');
		fileExt = _filePath.substr(last + 1);
		_fileName = _fileName + "." + fileExt;
		Logger::Debug(_productName);
		if (FormatHelper::HasContent(_productName))
		{
			UTILITY_API->Set3DWindowTitle(_fileName + " (" + _productName + ")");
		}
		else
		{
			UTILITY_API->Set3DWindowTitle(_fileName);
		}
	}

	/*
	* Description - OpenDownloadedZPRJ() method used to open download zprj.
	* Parameter - 
	* Exception - 
	* Return -
	*/
	inline void OpenDownloadedZPRJ()
	{
		const Marvelous::ImportZPRJOption option;
		//UTILITY_API->DisplayMessageBox("ProductConfig::GetInstance()->GetDownloadFileName()::" + ProductConfig::GetInstance()->GetDownloadFileName());
		IMPORT_API->ImportZprj(ProductConfig::GetInstance()->GetDownloadFileName(), option);
		//IMPORT_API->ImportFile(PLMDocumentData::GetInstance()->GetDownloadFileName());
	}

	/*
	* Description - LookFor3DGarmentChange() method used to check 3d garment is changed or not.
	* Parameter - 
	* Exception - 
	* Return -
	*/
	inline void LookFor3DGarmentChange()
	{
		string metaData = UTILITY_API->GetMetaDataForCurrentGarment();
		if (!FormatHelper::HasContent(metaData))
		{
			throw "No product exist. Please download a Product from PLM.";
		}
		json metadataJSON = json::parse(metaData);
		string documentId = Helper::GetJSONValue<string>(metadataJSON, OBJECT_ID_KEY, true);
		string activeDocObjectId = PublishToPLMData::GetInstance()->Get3DModelObjectId();
		
		if (!FormatHelper::HasContent(documentId))
		{
			documentId = BLANK;
		}
		if (!FormatHelper::HasContent(activeDocObjectId))
		{
			activeDocObjectId = BLANK;
			return;
		}
		if (activeDocObjectId != documentId)
		{
			PublishToPLMData::GetInstance()->SetActiveProductId(BLANK);
			PublishToPLMData::GetInstance()->SetActiveProductObjectId(BLANK);
			PublishToPLMData::GetInstance()->SetActiveProductName(BLANK);
			PublishToPLMData::GetInstance()->SetActiveProductStatus(BLANK);
			PublishToPLMData::GetInstance()->SetIsProductOverridden(false);
			PublishToPLMData::GetInstance()->SetActive3DModelMetaData(json::object());
			PublishToPLMData::GetInstance()->Set3DModelObjectId(BLANK);
		}
	}

	/*
	* Description - ValidateRquiredProductData() method used to validate user input.
	* Parameter -  QLabel, QLabel, QLabel,  bool.
	* Exception - 
	* Return - bool.
	*/
	inline bool ValidateRquiredProductData(QLabel* _productName, QLabel* _productId, bool _productIdKeyExist)
	{
		bool validated = true;
		
		if (!FormatHelper::HasContent(_productName->text().toStdString()))
		{
			validated = false;
		}
		if (_productIdKeyExist)
		{
			if (!FormatHelper::HasContent(_productId->text().toStdString()))
			{
				validated = false;
			}
		}
		return validated;
	}

	/*
	* Description - ValidateRquired3DVisualData() method used to validate rquired 3D visual data.
	* Parameter -  QComboBox, QLineEdit.
	* Exception - 
	* Return - bool.
	*/
	inline bool ValidateRquired3DVisualData(QComboBox* _resolution, QLineEdit* _zprjName)
	{
		bool validated = true;
		if (!FormatHelper::HasContent(_resolution->currentText().toStdString()) || !FormatHelper::HasContent(_zprjName->text().toStdString()))
		{
			validated = false;
		}
		return validated;
	}

	/*
	* Description - AddExtentionTo3DFileName() method used add extention to 3d file name.
	* Parameter -  QLineEdit.
	* Exception - 
	* Return - string.
	*/
	inline string AddExtentionTo3DFileName(QLineEdit* _lineEdit)
	{
		QString currentName = _lineEdit->text();
		if (!currentName.contains(".zprj") && !currentName.contains(".Zprj"))
		{
			currentName = currentName + ".zprj";
		}
		return currentName.toStdString();
	}

	/*
	* Description - Get3DModelNameFromSearchTree() method used to get 3D model name from search tree.
	* Parameter -  QTreeWidget, string.
	* Exception - 
	* Return - string.
	*/
	inline string Get3DModelNameFromSearchTree(QTreeWidget* _searchTree, string _fieldName)
	{
		string _3ModelName = "";
		string fieldLabel = "";
		for (int i = 0; i < _searchTree->topLevelItemCount(); i++)
		{
			QTreeWidgetItem* topItem = _searchTree->topLevelItem(i);

			QWidget* qWidgetColumn0 = _searchTree->itemWidget(topItem, 0);
			QLabel* qlabel = qobject_cast<QLabel*>(qWidgetColumn0);
			fieldLabel = qlabel->text().toStdString();
			fieldLabel = qlabel->property(ATTRIBUTE_KEY.c_str()).toString().toStdString();
			QWidget* qWidgetColumn1 = _searchTree->itemWidget(topItem, 1);
			if (fieldLabel.find("name") != string::npos || fieldLabel.find(_fieldName) != string::npos)
			{
				if (QLineEdit* qLineEditC1 = qobject_cast<QLineEdit*>(qWidgetColumn1))
				{
					_3ModelName = qLineEditC1->text().toStdString();
					break;
				}
			}
		}

		return _3ModelName;
	}

	/*
	* Description - ValidateRquired3DModelData() method used to validate rquired 3D model data.
	* Parameter -  QTreeWidget.
	* Exception - 
	* Return -
	*/
	static void ValidateRquired3DModelData(QTreeWidget* _3DModelTreeWidget)
	{
		string fieldValue = "";
		bool fieldLabel = false;
		string feildName = "";
		string  throwValue = "";
		for (int rowIndex = 0; rowIndex < _3DModelTreeWidget->topLevelItemCount(); rowIndex++)
		{
			QTreeWidgetItem* topItem = _3DModelTreeWidget->topLevelItem(rowIndex);
			QWidget* qWidgetColumn0 = _3DModelTreeWidget->itemWidget(topItem, 0);
			QLabel* qlabel = qobject_cast<QLabel*>(qWidgetColumn0);
			fieldLabel = Helper::IsValueTrue(qlabel->property(ATTRIBUTE_REQUIRED_KEY.c_str()).toString().toStdString());
			feildName = qlabel->property(ATTRIBUTE_NAME_KEY.c_str()).toString().toStdString();
			QWidget* qWidgetColumn1 = _3DModelTreeWidget->itemWidget(topItem, 1);
			if (QLineEdit* qLineEditC1 = qobject_cast<QLineEdit*>(qWidgetColumn1))
			{
				fieldValue.clear();
				fieldValue = qLineEditC1->text().toStdString();
				if (!FormatHelper::HasContent(fieldValue) && fieldLabel)
				{
					throwValue = feildName + REQUIRED_THROW_KEY;
					throw (throwValue);
				}
			}
			else if (QTextEdit* qTextC1 = qobject_cast<QTextEdit*>(qWidgetColumn1))
			{
				fieldValue.clear();
				fieldValue = qTextC1->toPlainText().toStdString();
				if (!FormatHelper::HasContent(fieldValue) && fieldLabel)
				{
					throwValue = feildName + REQUIRED_THROW_KEY;
					throw (throwValue);
				}
			}
			else if (QDateEdit* qDateC1 = qobject_cast<QDateEdit*>(qWidgetColumn1))
			{
				fieldValue.clear();
				fieldValue = FormatHelper::FormatDate(qDateC1);
				if (fieldValue == "1/1/2000" || fieldValue == "1/1/00" || fieldValue == "01/01/2000" || fieldValue == "01/01/00")
				{
					fieldValue = BLANK;
				}
				if (!FormatHelper::HasContent(fieldValue) && fieldLabel)
				{
					throwValue = feildName + REQUIRED_THROW_KEY;
					throw (throwValue);
				}
			}
			else if (QListWidget* listC1 = qobject_cast<QListWidget*>(qWidgetColumn1)) {
				fieldValue.clear();
				QListWidgetItem* listItem = nullptr;
				for (int row = 0; row < listC1->count(); row++)
				{
					listItem = listC1->item(row);
					if (listItem->checkState())
					{
						fieldValue = listItem->text().toStdString();
					}
				}
				if (!FormatHelper::HasContent(fieldValue) && fieldLabel)
				{
					throwValue = feildName + REQUIRED_THROW_KEY;
					throw (throwValue);
				}
			}
			else if (QDoubleSpinBox* doubleSpinC1 = qobject_cast<QDoubleSpinBox*>(qWidgetColumn1))
			{
				fieldValue.clear();
				if (doubleSpinC1->value() != 0.00000)
				{
					fieldValue = to_string(doubleSpinC1->value());
				}
				if (!FormatHelper::HasContent(fieldValue) && fieldLabel)
				{
					throwValue = feildName + REQUIRED_THROW_KEY;
					throw (throwValue);
				}
			}
			else if (QSpinBox* SpinC1 = qobject_cast<QSpinBox*>(qWidgetColumn1))
			{
				fieldValue.clear();
				if (SpinC1->value() != 0)
				{
					fieldValue = to_string(SpinC1->value());
				}
				if (!FormatHelper::HasContent(fieldValue) && fieldLabel)
				{
					throwValue = feildName + REQUIRED_THROW_KEY;
					throw (throwValue);
				}
			}
			else if (QComboBox* qComboBoxC1 = qobject_cast<QComboBox*>(qWidgetColumn1))
			{
				fieldValue = qComboBoxC1->currentText().toStdString();
				if (!FormatHelper::HasContent(fieldValue) && fieldLabel)
				{
					throwValue = feildName + REQUIRED_THROW_KEY;
					throw (throwValue);
				}
			}
		}
	}

	/*
	* Description - ResetDate() method used to reset the date.
	* Parameter -  QTreeWidget.
	* Exception - 
	* Return -
	*/
	inline void ResetDate(QTreeWidget* _searchTree)
	{
		string fromFieldValue = "";
		string toFieldValue = "";
		for (int i = 0; i < _searchTree->topLevelItemCount(); i++)
		{
			QTreeWidgetItem* topItem = _searchTree->topLevelItem(i);

			QWidget* qWidgetColumn1 = _searchTree->itemWidget(topItem, 1);
			QWidget* qWidgetColumn2 = _searchTree->itemWidget(topItem, 2);

			if (QDateEdit* qDateC1 = qobject_cast<QDateEdit*>(qWidgetColumn1))
			{
				fromFieldValue = FormatHelper::FormatDate(qDateC1);
				if (fromFieldValue != "1/1/2000" || fromFieldValue != "1/1/00" || fromFieldValue != "01/01/2000" || fromFieldValue != "01/01/00")
				{
					qDateC1->setDate(QDate(2000, 01, 01));
				}
				if (QDateEdit* qDateC2 = qobject_cast<QDateEdit*>(qWidgetColumn2))
				{
					toFieldValue = FormatHelper::FormatDate(qDateC2);
					if (toFieldValue != "1/1/2000" || toFieldValue != "1/1/00" || toFieldValue != "01/01/2000" || toFieldValue != "01/01/00")
					{
						qDateC2->setDate(QDate(2000, 01, 01));
					}
				}
			}
		}
	}

	/*
	* Description - Cache3DModelDataFromFile() method used to chach the data from currently opened 3d model irrespective of session.
	* Parameter -  json,string.
	* Exception - throw if any error found in the process.
	* Return - json.
	*/
	inline json Cache3DModelDataFromFile(json _metadataJSON, string _module)
	{
		string objectId = Helper::GetJSONValue<string>(_metadataJSON, DOCUMENTID_KEY, true);
		json downloadJson = json::object();
		json checkedIds = json::array();

		checkedIds.push_back(objectId);
		downloadJson[MODULE_KEY] = _module;
		downloadJson[CHECKED_IDS_KEY] = checkedIds;
		string response = RESTAPI::RESTMethodSubmit(RESTAPI::ATTACHMENTS_RESULTS_API, downloadJson);

		if (!FormatHelper::HasContent(response))
		{
			throw "Unable to fetch 3D Model, please try again or contact your System Administrator.";
		}

		if (FormatHelper::HasError(response))
		{
			throw runtime_error(response);
		}
		string error = RESTAPI::CheckForErrorMsg(response);
		if (FormatHelper::HasContent(error))
		{
			throw std::logic_error(error);
		}
		Logger::Debug("response response - " + response);

		return json::parse(response);
	}	

	inline void DeletePointer(QWidget*_ptr)
	{
		Logger::Info("UIHelper -> DeletePointer() -> Start");
		if (_ptr != nullptr)
		{
			delete _ptr;
		}
		Logger::Info("UIHelper -> DeletePointer() -> End");

	}
		
	inline string GetThumbnailUrl(string _imageId)
	{
		string thumbnailUrl = "";
		string resultResponse = "";
		json imageResultJson = json::object();
		auto startTime = std::chrono::high_resolution_clock::now();
		
		resultResponse = RESTAPI::CentricRestCallGet(Configuration::GetInstance()->GetPLMServerURL() + RESTAPI::IMAGE_API + "/" + _imageId + "?", APPLICATION_JSON_TYPE, "");
			auto finishTime = std::chrono::high_resolution_clock::now();
			std::chrono::duration<double> totalDuration = finishTime - startTime;
			Logger::perfomance(PERFOMANCE_KEY + "Get Thambnail API :: " + to_string(totalDuration.count()));
			Logger::RestAPIDebug("resultResponse::" + resultResponse);
			if (FormatHelper::HasError(resultResponse))
			{
				thumbnailUrl = "";
			}
			else
			{
				imageResultJson = json::parse(resultResponse);
				string defaultThumbnailId = Helper::GetJSONValue<string>(imageResultJson, DEFAULT_IMAGE_KEY, true);
				json imagesJeson = Helper::GetJSONParsedValue<string>(imageResultJson, "images", false);
				for (int imagesJsonCount = 0; imagesJsonCount < imagesJeson.size(); imagesJsonCount++)
				{
					json imagesJsonCountJeson = Helper::GetJSONParsedValue<int>(imagesJeson, imagesJsonCount, false);
					string imageId = Helper::GetJSONValue<string>(imagesJsonCountJeson, "id", true);
					if (defaultThumbnailId == imageId)
					{
						string thumbnailId = Helper::GetJSONValue<string>(imagesJsonCountJeson, THUMBNAIL_KEY, true);
						thumbnailUrl = Helper::GetJSONValue<string>(imagesJsonCountJeson, "_url_base_template", true);
						thumbnailUrl = Helper::FindAndReplace(thumbnailUrl, "%s", thumbnailId);
						break;
					}
				}
			}
		return thumbnailUrl;
	}

	inline string GetPrintThumbnailUrl(string _defaultImageId)
	{
		string thumbnailUrl = "";
		string resultResponse = "";
		json imageResultJson = json::object();
		auto startTime = std::chrono::high_resolution_clock::now();
		if (FormatHelper::HasContent(_defaultImageId))
		{
			string imageResponse = RESTAPI::CentricRestCallGet(Configuration::GetInstance()->GetPLMServerURL() + RESTAPI::PRINT_IMAGE_API + "/" + _defaultImageId, APPLICATION_JSON_TYPE, BLANK);
			auto finishTime = std::chrono::high_resolution_clock::now();
			std::chrono::duration<double> totalDuration = finishTime - startTime;
			Logger::perfomance(PERFOMANCE_KEY + "Get Thambnail API :: " + to_string(totalDuration.count()));
			Logger::RestAPIDebug("resultResponse::" + imageResponse);
			if (FormatHelper::HasError(imageResponse))
			{
				thumbnailUrl = "";
			}
			else
			{
				imageResultJson = json::parse(imageResponse);
				string thumbnailId = Helper::GetJSONValue<string>(imageResultJson, THUMBNAIL_KEY, true);
				thumbnailUrl = Helper::GetJSONValue<string>(imageResultJson, "_url_base_template", true);
				thumbnailUrl = Helper::FindAndReplace(thumbnailUrl, "%s", thumbnailId);
			}
		}
		return thumbnailUrl;
	}
	
	inline string getPLMVersion() 
	{
		
		string plmVersionResponse = RESTAPI::CentricRestCallGet(Configuration::GetInstance()->GetPLMServerURL() + RESTAPI::PLM_VERSION, APPLICATION_JSON_TYPE, "");
		if (!FormatHelper::HasContent(plmVersionResponse))
		{
			throw "Unable to fetch PLM Version, please try again or contact your System Administrator.";
		}
		if (FormatHelper::HasError(plmVersionResponse))
		{
			Helper::GetCentricErrorMessage(plmVersionResponse);
			throw runtime_error(plmVersionResponse);
		}
		json plmVersionJson;
		if (FormatHelper::HasContent(plmVersionResponse)) {
			plmVersionJson = json::parse(plmVersionResponse);
		}
		string plmVersion = Helper::GetJSONValue<string>(plmVersionJson, "title", true);
			
		return plmVersion;
	}

	inline std::map<string, string> GetEnumValues(string _format, QStringList& _valueList, QComboBox* _combobox = nullptr, bool _mapOfIdName = false)
	{
		Logger::Info("UIHelper -> GetEnumValues() -> Start");
		std::map<string, string> displayNameInternalNameMap;
		try
		{
			string enumListIdResponse = RESTAPI::CentricRestCallGet(Configuration::GetInstance()->GetPLMServerURL() + RESTAPI::SEARCH_ENUM_ATT_API + "?skip=0&limit=1000&decode=true&node_name=" + _format, APPLICATION_JSON_TYPE, "");

			if (FormatHelper::HasError(enumListIdResponse))
			{
				if (QString::fromStdString(enumListIdResponse).contains("message"))
				{
					json errorRsponse = json::parse(enumListIdResponse);
					enumListIdResponse = Helper::GetJSONValue<string>(errorRsponse, "message", true);
				}
				throw runtime_error(enumListIdResponse);
			}
			json enumListIdResponseJson = json::parse(enumListIdResponse);
			string enumListId = Helper::GetJSONValue<string>(enumListIdResponseJson[0], ATTRIBUTE_ID, true);
			auto EnumListsApiStartTime = std::chrono::high_resolution_clock::now();
			string enumListResponse = RESTAPI::CentricRestCallGet(Configuration::GetInstance()->GetPLMServerURL() + RESTAPI::SEARCH_ENUM_ATT_API + "/" + enumListId + "/values?skip=0&limit=1000&decode=true" , APPLICATION_JSON_TYPE, "");

			if (FormatHelper::HasError(enumListResponse))
			{
				if (QString::fromStdString(enumListResponse).contains("message"))
				{
					json errorRsponse = json::parse(enumListResponse);
					enumListResponse = Helper::GetJSONValue<string>(errorRsponse, "message", true);
				}
				throw runtime_error(enumListResponse);
			}
			json enumListResponseJson = json::parse(enumListResponse);

			for (int index = 0; index < enumListResponseJson.size(); index++)
			{
				json imagesJsonCountJeson = Helper::GetJSONParsedValue<int>(enumListResponseJson, index, false);
				string displayName = Helper::GetJSONValue<string>(imagesJsonCountJeson, "display_name", true);
				string internalName = Helper::GetJSONValue<string>(imagesJsonCountJeson, "node_name", true);
				if (_combobox)
				{
					_combobox->setProperty(internalName.c_str(), displayName.c_str());
					_combobox->setProperty(displayName.c_str(), internalName.c_str());
				}
				if (FormatHelper::HasContent(displayName))
				{
					_valueList.append(QString::fromStdString(displayName));
					if (_mapOfIdName)
						displayNameInternalNameMap.insert(make_pair(internalName, displayName));
					else
						displayNameInternalNameMap.insert(make_pair(displayName, internalName));
				}
			}
		}
		catch (string msg)
		{
			UTILITY_API->DeleteProgressBar(true);
			Logger::Error("UIHelper->GetEnumValues() Exception - " + msg);
			
		}
		catch (exception & e)
		{
			UTILITY_API->DeleteProgressBar(true);
			Logger::Error("UIHelper -> GetEnumValues() Exception - " + string(e.what()));
			
		}
		catch (const char* msg)
		{
			UTILITY_API->DeleteProgressBar(true);
			wstring wstr(msg, msg + strlen(msg));
			Logger::Error("UIHelper -> GetEnumValues() Exception - " + string(msg));
			
		}
		Logger::Info("UIHelper -> GetEnumValues() -> end");
		return displayNameInternalNameMap;
	}
	
	inline json GetEnumValues(string _format)
	{
		Logger::Info("UIHelper -> GetEnumValues() -> Start");
		std::map<string, string> displayNameInternalNameMap;
		json attributeDataCacheJson = json::array();
		
		try
		{
			string enumListIdResponse = RESTAPI::CentricRestCallGet(Configuration::GetInstance()->GetPLMServerURL() + RESTAPI::SEARCH_ENUM_ATT_API + "?skip=0&limit=1000&decode=true&node_name=" + _format, APPLICATION_JSON_TYPE, "");

			if (FormatHelper::HasError(enumListIdResponse))
			{
				if (QString::fromStdString(enumListIdResponse).contains("message"))
				{
					json errorRsponse = json::parse(enumListIdResponse);
					enumListIdResponse = Helper::GetJSONValue<string>(errorRsponse, "message", true);
				}
				throw runtime_error(enumListIdResponse);
			}
			json enumListIdResponseJson = json::parse(enumListIdResponse);
			string enumListId = Helper::GetJSONValue<string>(enumListIdResponseJson[0], ATTRIBUTE_ID, true);
			auto EnumListsApiStartTime = std::chrono::high_resolution_clock::now();
			string enumListResponse = RESTAPI::CentricRestCallGet(Configuration::GetInstance()->GetPLMServerURL() + RESTAPI::SEARCH_ENUM_ATT_API + "/" + enumListId + "/values?skip=0&limit=1000&decode=true", APPLICATION_JSON_TYPE, "");

			if (FormatHelper::HasError(enumListResponse))
			{
				if (QString::fromStdString(enumListResponse).contains("message"))
				{
					json errorRsponse = json::parse(enumListResponse);
					enumListResponse = Helper::GetJSONValue<string>(errorRsponse, "message", true);
				}
				throw runtime_error(enumListResponse);
			}
			json enumListResponseJson = json::parse(enumListResponse);
			json attJson = json::object();
			for (int index = 0; index < enumListResponseJson.size(); index++)
			{
				
				json imagesJsonCountJeson = Helper::GetJSONParsedValue<int>(enumListResponseJson, index, false);
				string displayName = Helper::GetJSONValue<string>(imagesJsonCountJeson, "display_name", true);
				string internalName = Helper::GetJSONValue<string>(imagesJsonCountJeson, "node_name", true);
				attJson[internalName] = displayName;
				
				
			}
			attributeDataCacheJson[0] = attJson;
		}
		catch (string msg)
		{
			UTILITY_API->DeleteProgressBar(true);
			Logger::Error("UIHelper->GetEnumValues() Exception - " + msg);

		}
		catch (exception & e)
		{
			UTILITY_API->DeleteProgressBar(true);
			Logger::Error("UIHelper -> GetEnumValues() Exception - " + string(e.what()));

		}
		catch (const char* msg)
		{
			UTILITY_API->DeleteProgressBar(true);
			wstring wstr(msg, msg + strlen(msg));
			Logger::Error("UIHelper -> GetEnumValues() Exception - " + string(msg));

		}
		Logger::Info("UIHelper -> GetEnumValues() -> attributeDataCacheJson"+ to_string(attributeDataCacheJson));
		Logger::Info("UIHelper -> GetEnumValues() -> end");
		return attributeDataCacheJson;
	}

	inline std::map<QString, QString> GetImageLabels(string _module )
	{
		Logger::Debug(" UiHelper GetImageLabels() -> Start");
		std::map<QString, QString> displayNameNodeNameMap;
		try
		{
			string moduleName = "node_name=" + _module;
			string response = RESTAPI::CentricRestCallGet(Configuration::GetInstance()->GetPLMServerURL() + RESTAPI::IMAGE_LABEL_ID_API + moduleName, APPLICATION_JSON_TYPE, "");
			json ImageLabelJson = json::object();

			if (FormatHelper::HasError(response))
			{
				Helper::GetCentricErrorMessage(response);
				throw runtime_error(response);
			}

			ImageLabelJson = json::parse(response);

			string image_labels_enum_list = Helper::GetJSONValue<string>(ImageLabelJson[0], "image_labels_enum_list", true);
			//UTILITY_API->DisplayMessageBox(" imageLabelList3: " + image_labels_enum_list);
			string labelsResponse = RESTAPI::CentricRestCallGet(Configuration::GetInstance()->GetPLMServerURL() + RESTAPI::IMAGE_LABELS_ENUMLIST_API + "/" + image_labels_enum_list + "/values?skip=0&decode=true&limit=" + Configuration::GetInstance()->GetMaximumLimitForRefAttValue(), APPLICATION_JSON_TYPE, "");

			if (FormatHelper::HasError(labelsResponse))
			{
				Helper::GetCentricErrorMessage(labelsResponse);
				throw runtime_error(labelsResponse);
			}
			auto imageLabelsjson = json::parse(labelsResponse);
			

			for (int labelsArrayCount = 0; labelsArrayCount < imageLabelsjson.size(); labelsArrayCount++)
			{

				json labelCountJson = Helper::GetJSONParsedValue<int>(imageLabelsjson, labelsArrayCount, false);
				string imageLabelDisplayName = Helper::GetJSONValue<string>(labelCountJson, DISPLAY_NAME_KEY, true);
				string imageLabelNodeName = Helper::GetJSONValue<string>(labelCountJson, NODE_NAME_KEY, true);

				displayNameNodeNameMap.insert(make_pair(QString::fromStdString(imageLabelDisplayName), QString::fromStdString(imageLabelNodeName)));
			}
		}
		catch (string msg)
		{
			UTILITY_API->DeleteProgressBar(true);
			Logger::Error("UIHelper->GetImageLabels() Exception - " + msg);

		}
		catch (exception & e)
		{
			UTILITY_API->DeleteProgressBar(true);
			Logger::Error("UIHelper -> GetImageLabels() Exception - " + string(e.what()));

		}
		catch (const char* msg)
		{
			UTILITY_API->DeleteProgressBar(true);
			wstring wstr(msg, msg + strlen(msg));
			Logger::Error("UIHelper -> GetImageLabels() Exception - " + string(msg));

		}
		Logger::Debug(" UiHelper GetImageLabels() -> End");
		return displayNameNodeNameMap;
		
	}

	inline void UpdateDivisionField( string _seasonId, QWidget* _widgetItem)
	{
		Logger::Debug(" UiHelper UpdateDivisionField() -> Start");
		try
		{
			UTILITY_API->CreateProgressBar();
			Configuration::GetInstance()->SetProgressBarProgress(0);
			Configuration::GetInstance()->SetProgressBarProgress(qrand() % 101);
			RESTAPI::SetProgressBarData(Configuration::GetInstance()->GetProgressBarProgress(), "Loading Division Values..", true);
			string refApi = Configuration::GetInstance()->GetPLMServerURL() + RESTAPI::COLLECTION_SEARCH_API + "?skip=0&limit=1000&decode=true&parent_season=" + _seasonId;
			string refListResponse = RESTAPI::CentricRestCallGet(refApi, APPLICATION_JSON_TYPE, "");
			Configuration::GetInstance()->SetProgressBarProgress(RESTAPI::SetProgressBarProgress(Configuration::GetInstance()->GetProgressBarProgress(), 10, "Loading Division Values.."));
			if (FormatHelper::HasError(refListResponse))
			{
				Helper::GetCentricErrorMessage(refListResponse);
				throw runtime_error(refListResponse);
			}
			json refListResponseJson = json::parse(refListResponse);
			QListWidget* listC1 = new QListWidget();
			for (size_t refListResponseJsonCount = 0; refListResponseJsonCount < refListResponseJson.size(); refListResponseJsonCount++)
			{
				json refListJson = Helper::GetJSONParsedValue(refListResponseJson, refListResponseJsonCount, false);
				Logger::Logger("refListJson::" + to_string(refListJson));
				string refValue = Helper::GetJSONValue<string>(refListJson, NODE_NAME_KEY, true);
				string refKey = Helper::GetJSONValue<string>(refListJson, "id", true);
				if (FormatHelper::HasContent(refValue))
				{
					if (listC1 = qobject_cast<QListWidget*>(_widgetItem))
					{
						CVWidgetGenerator::CreateCVMultiListWidget(listC1, QString::fromStdString(refValue), QString::fromStdString(refKey), QString::fromStdString(BLANK));
					}
					else if (QComboBox* qComboBoxC1 = qobject_cast<QComboBox*>(_widgetItem))
					{
						qComboBoxC1->setProperty(refValue.c_str(), QString::fromStdString(refKey));
						qComboBoxC1->addItem(QString::fromStdString(refValue));
					}
				}
			}
			listC1->setStyleSheet("QListWidget{border-bottom: 1px inset #262626; font-size: 10px; font-face: ArialMT; border-left: 1px inset #0D0D0D; max-height:90px; border-top: 1px inset #0D0D0D; border-right: 1px inset #262626; background-color: #222224; outline: 0; } QListView { outline: 0; } QListWidget::item{ background-color: #222224; margin: 2px;} QListWidget::item:hover { background-color: #222224; } QListWidget::indicator { width: 10px; height:10px; } QListWidget::indicator:unchecked {background-color: #343434 ; border: 1px solid #3B3B3B;} QListWidget::indicator:checked {image: url(:/CLOVise/PLM/Images/icon_ok_over.svg);background-color: #343434; border: 1px solid #3B3B3B;}");
			listC1->sortItems(Qt::AscendingOrder);
			UTILITY_API->DeleteProgressBar(true);
		}
		catch (string msg)
		{
			UTILITY_API->DeleteProgressBar(true);
			Logger::Error("UIHelper->UpdateDivisionField() Exception - " + msg);

		}
		catch (exception & e)
		{
			UTILITY_API->DeleteProgressBar(true);
			Logger::Error("UIHelper -> UpdateDivisionField() Exception - " + string(e.what()));

		}
		catch (const char* msg)
		{
			UTILITY_API->DeleteProgressBar(true);
			wstring wstr(msg, msg + strlen(msg));
			Logger::Error("UIHelper -> UpdateDivisionField() Exception - " + string(msg));

		}
		Logger::Debug(" Ui helper UpdateDivisionField() -> End");
	}


	inline void ClearAllFieldsForSearch(QTreeWidget * TreeWidget)
	{
		Logger::Debug(" UiHelper ClearAllFieldsForSearch() -> Start");
		try
		{
			for (int itemIndex = 0; itemIndex < TreeWidget->topLevelItemCount(); ++itemIndex)
			{
				QTreeWidgetItem* topItem = TreeWidget->topLevelItem(itemIndex);
				QWidget* qWidgetColumn0 = TreeWidget->itemWidget(topItem, 0);
				QWidget* qWidgetColumn1 = TreeWidget->itemWidget(topItem, 1);
				if (!qWidgetColumn0 || !qWidgetColumn1)
				{
					continue;
				}
				QLabel* qlabel = qobject_cast<QLabel*>(qWidgetColumn0);
				string label = qlabel->text().toStdString();
				QLineEdit* qlineedit = qobject_cast<QLineEdit*>(qWidgetColumn1);
				//QString labelId = qlabel->property("Id").toString();

				if (QLineEdit* qlineedit = qobject_cast<QLineEdit*>(qWidgetColumn1))
				{
					qlineedit->setText("");
				}
				if (QComboBox* qComboBox = qobject_cast<QComboBox*>(qWidgetColumn1))
				{
					int indexOfEmptyString = qComboBox->findText(QString::fromStdString(BLANK));
					qComboBox->setCurrentIndex(indexOfEmptyString);
				}
				if (QTextEdit* qtextEdit = qobject_cast<QTextEdit*>(qWidgetColumn1))
				{
					qtextEdit->setText("");
				}
				else if (QDateEdit* qDateC1 = qobject_cast<QDateEdit*>(qWidgetColumn1))
				{
					QLineEdit *edit = qDateC1->findChild<QLineEdit*>("qt_spinbox_lineedit");
					edit->clear();
					qDateC1->clear();
					edit->setText("YYYY-MM-DD");
				}
				else if (QListWidget* listC1 = qobject_cast<QListWidget*>(qWidgetColumn1))
				{
					for (int row = 0; row < listC1->count(); row++)
					{
						QListWidgetItem* listItem = nullptr;
						listItem = listC1->item(row);
						if (listItem->checkState())
						{
							listItem->setCheckState(Qt::Unchecked);
						}
					}
				}
				else if (QSpinBox* SpinC1 = qobject_cast<QSpinBox*>(qWidgetColumn1))
				{
					if (SpinC1->value() != 0)
					{
						SpinC1->setValue(0);
					}
				}
				else if (QDoubleSpinBox* SpinC1 = qobject_cast<QDoubleSpinBox*>(qWidgetColumn1))
				{
					if (SpinC1->value() != 0)
					{
						SpinC1->setValue(0);
					}
				}
			}
		}
		catch (string msg)
		{
			UTILITY_API->DeleteProgressBar(true);
			Logger::Error("UIHelper->ClearAllFieldsForSearch() Exception - " + msg);

		}
		catch (exception & e)
		{
			UTILITY_API->DeleteProgressBar(true);
			Logger::Error("UIHelper -> ClearAllFieldsForSearch() Exception - " + string(e.what()));

		}
		catch (const char* msg)
		{
			UTILITY_API->DeleteProgressBar(true);
			wstring wstr(msg, msg + strlen(msg));
			Logger::Error("UIHelper -> ClearAllFieldsForSearch() Exception - " + string(msg));

		}
		Logger::Debug("UiHelper ClearAllFieldsForSearch() -> End");
	}
	/*
	* Description - CreateWidgetsOnTreeWidget() method used to create widgets on QTreeWidget.
	* Parameter -	QTreeWidget, int, int, json, bool, bool
	* Exception - 
	* Return - 
	*/
	inline void CreateWidgetsOnTreeWidget(QTreeWidget* _treeWidget, int _start, int _end, json _attJSON, bool _noFilter, bool _isTemplateType)
	{
		Logger::Debug("UIHelper CreateWidgetsonTreeWidget() -> Start");
		try
		{
			QStringList attributeDefaultValueList;
			string attributevalue = "", attributeName = "", attributeType = "", attributeEnum = "", attributeEnumJson = "",
				attributeEnumVale = "", attributeKey = "", attributeEnumKey = "", attributeDecimals = "", attributeTypeDefalutValue = "", attributeCurrencyCode = "", attributeAttscop = "", attributeApiExposed = "";
			bool isAttrSearchable = false;
			bool isMultiselectable = false;
			bool isCheckable = false;
			bool isInteger = false;
			bool isSingleFloat = false;
			json attCountOfAttJson = json::object();
			json attEnumListStrJson = json::object();
			for (int attributeCount = _start; attributeCount < _end; attributeCount++)
			{
				attributeDefaultValueList.clear();

				attCountOfAttJson = Helper::GetJSONParsedValue<int>(_attJSON, attributeCount, false);
				attributeName = Helper::GetJSONValue<string>(attCountOfAttJson, ATTRIBUTE_NAME_KEY, true);
				attributeAttscop = Helper::GetJSONValue<string>(attCountOfAttJson, ATTRIBUTE_ATTSCOPE_KEY, true);
				attributeApiExposed = Helper::GetJSONValue<string>(attCountOfAttJson, ATTRIBUTE_REST_API_EXPOSED, true);
				if (attributeApiExposed == "false")
					continue;

				if ("true" == Helper::GetJSONValue<string>(attCountOfAttJson, IS_MULTISELECT_KEY, true))
					isMultiselectable = true;
				else
					isMultiselectable = false;
				if ("true" == Helper::GetJSONValue<string>(attCountOfAttJson, IS_CHECK_KEY, true))
					isCheckable = true;
				else
					isCheckable = false;
				if ("true" == Helper::GetJSONValue<string>(attCountOfAttJson, IS_SINGLE_INTERGER_KEY, true))
					isInteger = true;
				else
					isInteger = false;
				if ("true" == Helper::GetJSONValue<string>(attCountOfAttJson, IS_SINGLE_FLOAT_KEY, true))
					isSingleFloat = true;
				else
					isSingleFloat = false;
				//UTILITY_API->DisplayMessageBox(" Helper::GetJSONValue<string>(attCountOfAttJson, IS_CHECK_KEY, true)::" + Helper::GetJSONValue<string>(attCountOfAttJson, IS_CHECK_KEY, true));
				//UTILITY_API->DisplayMessageBox(" Helper::GetJSONValue<string>(attCountOfAttJson, IS_MULTISELECT_KEY, true)::" + Helper::GetJSONValue<string>(attCountOfAttJson, IS_MULTISELECT_KEY, true));
				/*if (!_attScops.isEmpty() && !_attScops.contains(QString::fromStdString(attributeAttscop)))
				{
					continue;
				}*/
				attributeKey = Helper::GetJSONValue<string>(attCountOfAttJson, ATTRIBUTE_KEY, true);
				attributeType = Helper::GetJSONValue<string>(attCountOfAttJson, ATTRIBUTE_TYPE_KEY, true);
				//Continue if no supported atttibute type
				/*if (!Configuration::GetInstance()->GetSupportedAttsList().contains(QString::fromStdString(attributeType)))
				{
					Logger::Error("CVWidgetGenerator::CreateSearchCriteriaWidget() - attributeType:: not supported");
					continue;
				}*/
				attributeTypeDefalutValue = Helper::GetJSONValue<string>(attCountOfAttJson, ATTRIBUTE_VALUE_KEY, true);
				attributeDefaultValueList << QString::fromStdString(attributeTypeDefalutValue);
				isAttrSearchable = Helper::IsValueTrue(Helper::GetJSONValue<string>(attCountOfAttJson, ATTRIBUTE_SEARCHABLE_KEY, true));
				if ((attributeType == ENUM_ATT_TYPE_KEY && !isMultiselectable) || (attributeType == REF_ATT_TYPE_KEY && !isMultiselectable) || (attributeType == CHOICE_ATT_TYPE_KEY && !isMultiselectable))
				{
					QTreeWidgetItem* topLevel = new QTreeWidgetItem();  // Creating new TreeWidgetItem
					if (!_noFilter || _noFilter && isAttrSearchable)
					{
						json attributeEnumJson = Helper::GetJSONParsedValue<string>(attCountOfAttJson, ENUMLIST_JSON_KEY, false);
						QStringList valueList;
						QString selectedValue = "";
						ComboBoxItem* comboBox = new ComboBoxItem();
						comboBox->setFocusPolicy(Qt::StrongFocus);
						comboBox->setStyleSheet(COMBOBOX_STYLE);
						comboBox->addItem(QString::fromStdString(BLANK));

						for (int attEnumCount = 0; attEnumCount < attributeEnumJson.size(); attEnumCount++)
						{
							attEnumListStrJson = Helper::GetJSONParsedValue<int>(attributeEnumJson, attEnumCount, false);;///use new method
							attributeEnumVale = Helper::GetJSONValue<string>(attEnumListStrJson, ENUM_VALUE_KEY, true);
							attributeEnumKey = Helper::GetJSONValue<string>(attEnumListStrJson, ENUM_KEY, true);
							valueList.push_back(QString::fromStdString(attributeEnumVale));
							comboBox->setProperty(attributeEnumVale.c_str(), QString::fromStdString(attributeEnumKey));
							if (attributeTypeDefalutValue == attributeEnumKey) {
								selectedValue = QString::fromStdString(attributeEnumVale);
							}
						}
						if (comboBox->isEnabled())
						{
							comboBox->setEditable(true);
							comboBox->fillItemListAndDefaultValue(valueList, selectedValue);
							QCompleter* m_nameCompleter = new QCompleter(valueList, comboBox); // using QCompleter class
							m_nameCompleter->setCompletionMode(QCompleter::PopupCompletion);
							m_nameCompleter->setCaseSensitivity(Qt::CaseInsensitive);
							comboBox->setCompleter(m_nameCompleter);
						}
						valueList.sort();
						comboBox->addItems(valueList);
						comboBox->setCurrentIndex(comboBox->findText(selectedValue));
						if (!_isTemplateType)
						{

							comboBox->setDisabled(true);
							comboBox->setStyleSheet(DISABLED_COMBOBOX_STYLE);
						}
						else
						{
							comboBox->setDisabled(false);
							comboBox->setStyleSheet(COMBOBOX_STYLE);
						}

						_treeWidget->addTopLevelItem(topLevel);						// Adding ToplevelItem
						_treeWidget->setItemWidget(topLevel, 0, CVWidgetGenerator::CreateLabelWidget(attributeName, attributeKey, attributeType));	// Adding label at column 1
						_treeWidget->setItemWidget(topLevel, 1, comboBox);
					}
				}
				else if ((attributeType == ENUM_ATT_TYPE_KEY && isMultiselectable) || (attributeType == REF_ATT_TYPE_KEY && isMultiselectable) || attributeType == CHOICE_ATT_TYPE_KEY || attributeType == MOA_LIST_ATT_TYPE_KEY || attributeType == DRIVEN_ATT_TYPE_KEY)
				{
					QTreeWidgetItem* topLevel = new QTreeWidgetItem();
					QListWidget* listWidget = new QListWidget();
					json attributeEnumJson = Helper::GetJSONParsedValue<string>(attCountOfAttJson, ENUMLIST_JSON_KEY, false);
					for (int attEnumCount = 0; attEnumCount < attributeEnumJson.size(); attEnumCount++)
					{
						attEnumListStrJson = Helper::GetJSONParsedValue<int>(attributeEnumJson, attEnumCount, false);;///use new method
						attributeEnumVale = Helper::GetJSONValue<string>(attEnumListStrJson, ENUM_VALUE_KEY, true);
						attributeEnumKey = Helper::GetJSONValue<string>(attEnumListStrJson, ENUM_KEY, true);
						CVWidgetGenerator::CreateCVMultiListWidget(listWidget, QString::fromStdString(attributeEnumVale), QString::fromStdString(attributeEnumKey), QString::fromStdString(attributeTypeDefalutValue));
					}
					listWidget->sortItems(Qt::AscendingOrder);
					listWidget->setFocusPolicy(Qt::NoFocus);
					listWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
					listWidget->setStyleSheet("QListWidget{border-bottom: 1px inset #262626; font-size: 10px; font-face: ArialMT; border-left: 1px inset #0D0D0D; max-height:70px; border-top: 1px inset #0D0D0D; border-right: 1px inset #262626; background-color: #222224; } QListWidget::item{ background-color: #222224; margin: 2px;} QListWidget::indicator { width: 10px; height:10px; } QListWidget::indicator:unchecked {background-color: #343434 ; border: 1px solid #3B3B3B;} QListWidget::indicator:checked {image: url(:/CLOVise/PLM/Images/icon_ok_over.svg);background-color: #343434; border: 1px solid #3B3B3B;}");
					if (!_noFilter || _noFilter && isAttrSearchable)
					{
						_treeWidget->addTopLevelItem(topLevel);		// Adding ToplevelItem
						_treeWidget->setItemWidget(topLevel, 0, CVWidgetGenerator::CreateLabelWidget(attributeName, attributeKey, attributeType));	// Adding label at column 1
						if (!_isTemplateType)
						{
							listWidget->setDisabled(true);
							listWidget->setStyleSheet("QListWidget{ color: #808080; border-bottom: 1px inset #262626; font-size: 10px; font-face: ArialMT; border-left: 1px inset #0D0D0D; max-height:70px; border-top: 1px inset #0D0D0D; border-right: 1px inset #262626; background-color: #222224; } QListWidget::item{ color: #808080; background-color: #222224; margin: 2px;} QListWidget::indicator { width: 10px; height:10px; } QListWidget::indicator:unchecked {background-color: #343434 ; border: 1px solid #3B3B3B;} QListWidget::indicator:checked {image: url(:/CLOVise/PLM/Images/icon_ok_over.svg);background-color: #343434; border: 1px solid #3B3B3B;}");
						}
						_treeWidget->setItemWidget(topLevel, 1, listWidget);
					}
				}
				//else if (attributeType == TEXT_AREA_ATT_TYPE_KEY)
				//{
				//	QTreeWidgetItem* topLevel = new QTreeWidgetItem();  // Creating new TreeWidgetItem
				//	if (!_noFilter || _noFilter && isAttrSearchable)
				//	{
				//		_searchTree->addTopLevelItem(topLevel);				// Adding ToplevelItem
				//		_searchTree->setItemWidget(topLevel, 0, CVWidgetGenerator::CreateLabelWidget(attributeName, attributeKey, attributeType));	// Adding label at column 1
				//		_searchTree->setItemWidget(topLevel, 1, CVWidgetGenerator::CreateTextAreaWidget(attributeTypeDefalutValue, _isTemplateType));
				//	}
				//}
				else if (attributeType == DATE_ATT_TYPE_KEY || attributeType == TIME_ATT_TYPE_KEY)
				{
					//ColorConfig::GetInstance()->SetDateFlag(true);
					//MaterialConfig::GetInstance()->SetDateFlag(true);
					//ProductConfig::GetInstance()->SetDateFlag(true);
					QStringList dateList;
					QTreeWidgetItem* topLevel = new QTreeWidgetItem();  // Creating new TreeWidgetItem
					if (!_noFilter || _noFilter && isAttrSearchable)
					{
						_treeWidget->addTopLevelItem(topLevel);			// Adding ToplevelItem
						_treeWidget->setItemWidget(topLevel, 0, CVWidgetGenerator::CreateLabelWidget(attributeName, attributeKey, attributeType));	// Adding label at column 1
						if (FormatHelper::HasContent(attributeTypeDefalutValue))
						{
							dateList = QString::fromStdString(attributeTypeDefalutValue).split(QString::fromStdString(DELIMITER_HYPHAN));

							for (int dateListCount = 0; dateListCount < dateList.size(); dateListCount++)
							{
								_treeWidget->setItemWidget(topLevel, dateListCount + 1, CVWidgetGenerator::CreateDateWidget(dateList[dateListCount].toStdString(), _isTemplateType));
							}
						}
						else
						{
							_treeWidget->setItemWidget(topLevel, 1, CVWidgetGenerator::CreateDateWidget(attributeTypeDefalutValue, _isTemplateType));
							//_searchTree->setItemWidget(topLevel, 2, CVWidgetGenerator::CreateDateWidget(attributeTypeDefalutValue, _isTemplateType));
						}

					}
				}
				else if (attributeType == INTEGER_ATT_TYPE_KEY || attributeType == CONSTANT_ATT_TYPE_KEY || attributeType == SEQUENCE_ATT_TYPE_KEY)
				{
					QTreeWidgetItem* topLevel = new QTreeWidgetItem();  // Creating new TreeWidgetItem
					_treeWidget->addTopLevelItem(topLevel);
					if (isInteger)
					{			// Adding ToplevelItem
						//UTILITY_API->DisplayMessageBox("isInteger::" + to_string(isInteger));
						if (attributeName == "RGB")
						{
							_treeWidget->setItemWidget(topLevel, 0, CVWidgetGenerator::CreateLabelWidget(attributeName, attributeKey, attributeType));
							_treeWidget->setItemWidget(topLevel, 1, CVWidgetGenerator::CreateSpinBoxWidgetForRGB(QString::fromStdString(attributevalue), _isTemplateType, "", -1000000000, 1000000000));
						}
						else
						{
							_treeWidget->setItemWidget(topLevel, 0, CVWidgetGenerator::CreateLabelWidget(attributeName, attributeKey, attributeType));
							_treeWidget->setItemWidget(topLevel, 1, CVWidgetGenerator::CreateSpinBoxWidget(QString::fromStdString(attributevalue), _isTemplateType, ""));
						}
					}
					else if (!_noFilter || _noFilter && isAttrSearchable)
					{
						//_searchTree->addTopLevelItem(topLevel);						// Adding ToplevelItem
						_treeWidget->setItemWidget(topLevel, 0, CVWidgetGenerator::CreateLabelWidget(attributeName, attributeKey, attributeType));	// Adding label at column 1
						if (FormatHelper::HasContent(attributeTypeDefalutValue))
						{
							QStringList list = QString::fromStdString(attributeTypeDefalutValue).split(QString::fromStdString(DELIMITER_NEGATION));

							QStringList intList = list[0].split(DELIMITER_HYPHAN.c_str());

							_treeWidget->setItemWidget(topLevel, 1, CVWidgetGenerator::CreateSpinBoxWidget(intList[0], _isTemplateType, "FROM:"));
							_treeWidget->setItemWidget(topLevel, 2, CVWidgetGenerator::CreateSpinBoxWidget(intList[1], _isTemplateType, "TO:"));
						}
						else
						{
							_treeWidget->setItemWidget(topLevel, 1, CVWidgetGenerator::CreateSpinBoxWidget(QString::fromStdString(attributevalue), _isTemplateType, "FROM:"));
							_treeWidget->setItemWidget(topLevel, 2, CVWidgetGenerator::CreateSpinBoxWidget(QString::fromStdString(attributevalue), _isTemplateType, "TO:"));
						}
					}
				}
				else if (attributeType == FLOAT_ATT_TYPE_KEY || attributeType == DOUBLE_ATT_TYPE_KEY)
				{
					QTreeWidgetItem* topLevel = new QTreeWidgetItem();  // Creating new TreeWidgetItem
					_treeWidget->addTopLevelItem(topLevel);						// Adding ToplevelItem

					if (isSingleFloat)
					{
						_treeWidget->setItemWidget(topLevel, 0, CVWidgetGenerator::CreateLabelWidget(attributeName, attributeKey, attributeType));	// Adding label at column 1
						_treeWidget->setItemWidget(topLevel, 1, CVWidgetGenerator::CreateDoubleSpinBoxWidget(QString::fromStdString(attributeTypeDefalutValue), _isTemplateType, "", "0", 0.0000, 10000000000000.0000));
					}
					else if (!_noFilter || _noFilter && isAttrSearchable)
					{
						//attributeDecimals = Helper::GetJSONValue<string>(attCountOfAttJson, DECIMAP_FIGURES, true);
						//_searchTree->addTopLevelItem(topLevel);						// Adding ToplevelItem
						_treeWidget->setItemWidget(topLevel, 0, CVWidgetGenerator::CreateLabelWidget(attributeName, attributeKey, attributeType));	// Adding label at column 1
						if (FormatHelper::HasContent(attributeTypeDefalutValue))
						{
							QStringList floatList = QString::fromStdString(attributeTypeDefalutValue).split(QString::fromStdString(DELIMITER_NEGATION));
							QStringList list = floatList[0].split(DELIMITER_HYPHAN.c_str());
							_treeWidget->setItemWidget(topLevel, 1, CVWidgetGenerator::CreateDoubleSpinBoxWidget(list[0], _isTemplateType, "From:", attributeDecimals, 0.0000, 10000000000000.0000));
							_treeWidget->setItemWidget(topLevel, 2, CVWidgetGenerator::CreateDoubleSpinBoxWidget(list[1], _isTemplateType, "To:", attributeDecimals, 0.0000, 10000000000000.0000));
						}
						else
						{
							_treeWidget->setItemWidget(topLevel, 1, CVWidgetGenerator::CreateDoubleSpinBoxWidget(QString::fromStdString(attributeTypeDefalutValue), _isTemplateType, "From:", attributeDecimals, 0.0000, 10000000000000.0000));
							_treeWidget->setItemWidget(topLevel, 2, CVWidgetGenerator::CreateDoubleSpinBoxWidget(QString::fromStdString(attributeTypeDefalutValue), _isTemplateType, "To:", attributeDecimals, 0.0000, 10000000000000.0000));
						}
					}
				}
				else if (attributeType == CURRENCY_ATT_TYPE_KEY)
				{
					QTreeWidgetItem* topLevel = new QTreeWidgetItem();  // Creating new TreeWidgetItem
					if (!_noFilter || _noFilter && isAttrSearchable)
					{
						attributeDecimals = Helper::GetJSONValue<string>(attCountOfAttJson, DECIMAP_FIGURES, true);
						attributeCurrencyCode = Helper::GetJSONValue<string>(attCountOfAttJson, CURRENCY_CODE, true) + " ";
						_treeWidget->addTopLevelItem(topLevel);						// Adding ToplevelItem
						_treeWidget->setItemWidget(topLevel, 0, CVWidgetGenerator::CreateLabelWidget(attributeName, attributeKey, attributeType));	// Adding label at column 1
						if (FormatHelper::HasContent(attributeTypeDefalutValue))
						{
							QStringList currencyList = QString::fromStdString(attributeTypeDefalutValue).split(QString::fromStdString(DELIMITER_NEGATION));
							_treeWidget->setItemWidget(topLevel, 1, CVWidgetGenerator::CreateDoubleSpinBoxWidget(currencyList[0], _isTemplateType, "From: " + attributeCurrencyCode, attributeDecimals, 0.00, 10000000000000.00));
							_treeWidget->setItemWidget(topLevel, 2, CVWidgetGenerator::CreateDoubleSpinBoxWidget(currencyList[1], _isTemplateType, "To: " + attributeCurrencyCode, attributeDecimals, 0.00, 10000000000000.00));
						}
						else
						{
							_treeWidget->setItemWidget(topLevel, 1, CVWidgetGenerator::CreateDoubleSpinBoxWidget(QString::fromStdString(attributeTypeDefalutValue), _isTemplateType, "From: " + attributeCurrencyCode, attributeDecimals, 0.00, 10000000000000.00));
							_treeWidget->setItemWidget(topLevel, 2, CVWidgetGenerator::CreateDoubleSpinBoxWidget(QString::fromStdString(attributeTypeDefalutValue), _isTemplateType, "To: " + attributeCurrencyCode, attributeDecimals, 0.00, 10000000000000.00));
						}
					}
				}
				else if (attributeType == BOOLEAN_ATT_TYPE_KEY)
				{
					QTreeWidgetItem* topLevel = new QTreeWidgetItem();  // Creating new TreeWidgetItem
					_treeWidget->addTopLevelItem(topLevel);
					//if (isCheckable)
					//{
					//	//UTILITY_API->DisplayMessageBox("comming inside isCheckable::" + to_string(isCheckable));
					//	QListWidget* listWidget = new QListWidget();
					//	_searchTree->addTopLevelItem(topLevel);						// Adding ToplevelItem
					//	_searchTree->setItemWidget(topLevel, 0, CVWidgetGenerator::CreateLabelWidget(attributeName, attributeKey, attributeType));
					//	QCheckBox* checkBox = new QCheckBox();
					//	_searchTree->setItemWidget(topLevel, 1, checkBox);
					//}
					//else 
					if (!_noFilter || _noFilter && isAttrSearchable)
					{
						//UTILITY_API->DisplayMessageBox("coming inside bolean3::" + Helper::GetJSONValue<string>(attCountOfAttJson, IS_CHECK_KEY, true));
						// Adding ToplevelItem
						_treeWidget->setItemWidget(topLevel, 0, CVWidgetGenerator::CreateLabelWidget(attributeName, attributeKey, attributeType));	// Adding label at column 1
						_treeWidget->setItemWidget(topLevel, 1, CVWidgetGenerator::CreateBooleanWidget(attributeTypeDefalutValue, _isTemplateType, topLevel, 1));	// Adding label at column 2
					}
				}
				else if (attributeType == USER_LIST_ATT_TYPE_KEY)
				{
					QTreeWidgetItem* topLevel = new QTreeWidgetItem();  // Creating new TreeWidgetItem
					if (!_noFilter || _noFilter && isAttrSearchable)
					{
						json attributeEnumJson = Helper::GetJSONParsedValue<string>(attCountOfAttJson, USER_LIST_ATT_TYPE_KEY, false);
						QStringList valueList;
						QString selectedValue = "";
						ComboBoxItem* comboBox = new ComboBoxItem();
						comboBox->setFocusPolicy(Qt::StrongFocus);
						comboBox->setStyleSheet(COMBOBOX_STYLE);
						comboBox->addItem(QString::fromStdString(BLANK));

						for (int attEnumCount = 0; attEnumCount < attributeEnumJson.size(); attEnumCount++)
						{
							attEnumListStrJson = Helper::GetJSONParsedValue<int>(attributeEnumJson, attEnumCount, false);;///use new method
							attributeEnumVale = Helper::GetJSONValue<string>(attEnumListStrJson, ENUM_VALUE_KEY, true);
							attributeEnumKey = Helper::GetJSONValue<string>(attEnumListStrJson, ENUM_KEY, true);
							valueList.push_back(QString::fromStdString(attributeEnumVale));
							comboBox->setProperty(attributeEnumVale.c_str(), QString::fromStdString(attributeEnumKey));
							if (attributeTypeDefalutValue == attributeEnumKey) {
								selectedValue = QString::fromStdString(attributeEnumVale);
							}
						}
						if (comboBox->isEnabled())
						{
							comboBox->setEditable(true);
							comboBox->fillItemListAndDefaultValue(valueList, selectedValue);
							QCompleter* m_nameCompleter = new QCompleter(valueList, comboBox); // using QCompleter class
							m_nameCompleter->setCompletionMode(QCompleter::PopupCompletion);
							m_nameCompleter->setCaseSensitivity(Qt::CaseInsensitive);
							comboBox->setCompleter(m_nameCompleter);
						}
						valueList.sort();
						comboBox->addItems(valueList);
						comboBox->setCurrentIndex(comboBox->findText(selectedValue));

						if (!_isTemplateType)
						{

							comboBox->setDisabled(true);
							comboBox->setStyleSheet(DISABLED_COMBOBOX_STYLE);
						}
						else
						{
							comboBox->setDisabled(false);
							comboBox->setStyleSheet(COMBOBOX_STYLE);
						}
						_treeWidget->addTopLevelItem(topLevel);						// Adding ToplevelItem
						_treeWidget->setItemWidget(topLevel, 0, CVWidgetGenerator::CreateLabelWidget(attributeName, attributeKey, attributeType));	// Adding label at column 1
						_treeWidget->setItemWidget(topLevel, 1, comboBox);
					}
				}
				else if (attributeType == TEXT_ATT_TYPE_KEY || attributeType == DERIVEDSTRING_ATT_TYPE_KEY || attributeType == STRING_ATT_TYPE_KEY || attributeType == TEXT_AREA_ATT_TYPE_KEY)
				{
					QTreeWidgetItem* topLevel = new QTreeWidgetItem();  // Creating new TreeWidgetItem
					if (!_noFilter || _noFilter && isAttrSearchable)
					{
						QLineEdit* LineEdit = new QLineEdit();	// Creating new LineEdit Widget
						LineEdit->setAttribute(Qt::WA_MacShowFocusRect, false);
						LineEdit->setStyleSheet(LINEEDIT_STYLE);
						_treeWidget->addTopLevelItem(topLevel);						// Adding ToplevelItem
						_treeWidget->setItemWidget(topLevel, 0, CVWidgetGenerator::CreateLabelWidget(attributeName, attributeKey, attributeType));	// Adding label at column 1

						if (FormatHelper::HasContent(attributeTypeDefalutValue))
						{
							QStringList defalut = QString::fromStdString(attributeTypeDefalutValue).split(QString::fromStdString(DELIMITER_NEGATION));
							LineEdit->setText(defalut[0]);
						}
						if (!_isTemplateType)
						{
							LineEdit->setDisabled(true);
							LineEdit->setStyleSheet(DISABLED_LINEEDIT_STYLE);
						}
						_treeWidget->setItemWidget(topLevel, 1, LineEdit);
					}
				}
			}
		}
		catch (string msg)
		{
			UTILITY_API->DeleteProgressBar(true);
			Logger::Error("UIHelper->CreateWidgetsonTreeWidget() Exception - " + msg);

		}
		catch (exception & e)
		{
			UTILITY_API->DeleteProgressBar(true);
			Logger::Error("UIHelper -> CreateWidgetsonTreeWidget() Exception - " + string(e.what()));

		}
		catch (const char* msg)
		{
			UTILITY_API->DeleteProgressBar(true);
			wstring wstr(msg, msg + strlen(msg));
			Logger::Error("UIHelper -> CreateWidgetsonTreeWidget() Exception - " + string(msg));

		}

		Logger::Debug("UIHelper CreateWidgetsonTreeWidget() -> End");
	}




	/*
	* Description - CreateWidgetsOnOutBoundTreeWidget() method used to create widgets on QTreeWidget.
	* Parameter -	QTreeWidget, int, int, json
	* Exception -
	* Return -
	*/
	inline void CreateWidgetsOnOutBoundTreeWidget(QTreeWidget* _treeWidget, int _start, int _end, json _attributesJsonArray, string _mode)
	{
		Logger::Debug("DEBUG::UIHelper: CreateWidgetsOnOutBoundTreeWidget() -> Start");
		try
		{
			json attJson = json::object();
			QStringList attScopes;
			string attScope = "";
			string isAttEnable = "";
			//string isAttUpdatable = "";
			string attKey = "";
			string attValue = "";
			for (int i = _start; i < _end; i++)
			{
				//bool isAttSettable = false;

				attJson = Helper::GetJSONParsedValue<int>(_attributesJsonArray, i, false);
				bool isAttSettable = false;
				bool isAttUpdatable = false;

				attScope = Helper::GetJSONValue<string>(attJson, ATTRIBUTE_ATTSCOPE_KEY, true);
				Logger::Debug("UIHelper::CreateWidgetsOnOutBoundTreeWidget() - attScope:: " + attScope);
				/*if (!attScopes.isEmpty() && !attScopes.contains(QString::fromStdString(attScope)))
				{
					continue;
				}
	*/
				string attType = Helper::GetJSONValue<string>(attJson, ATTRIBUTE_TYPE_KEY, true);
				Logger::Debug("UIHelper::CreateWidgetsOnOutBoundTreeWidget() - attType:: " + attType);
				/*if (!Configuration::GetInstance()->GetModifySupportedAttsList().contains(QString::fromStdString(attType)))
				{
					Logger::Error("CVWidgetGenerator::CreateSearchCriteriaWidget() - attributeType:: not supported");
					continue;
				}*/

				isAttEnable = Helper::IsValueTrue(Helper::GetJSONValue<string>(attJson, ATTRIBUTE_ENABLED_KEY, true));
				Logger::Debug("UIHelper::CreateWidgetsOnOutBoundTreeWidget() - isAttEnable:: " + isAttEnable);
				isAttUpdatable = Helper::IsValueTrue(Helper::GetJSONValue<string>(attJson, ATTRIBUTE_UPDATABLE_KEY, true));
				Logger::Debug("UIHelper::CreateWidgetsOnOutBoundTreeWidget() - isAttUpdatable:: " + to_string(isAttUpdatable));
				if ("false" == isAttEnable)
				{
					continue;
				}
				attKey = Helper::GetJSONValue<string>(attJson, ATTRIBUTE_KEY, true);
				Logger::Debug("UIHelper::CreateWidgetsOnOutBoundTreeWidget() - attKey:: " + attKey);
				attValue = Helper::GetJSONValue<string>(attJson, ATTRIBUTE_DEFAULT_VALUE_KEY, true);
				Logger::Debug("UIHelper::CreateWidgetsOnOutBoundTreeWidget() - attValue:: " + attValue);
				string attSettable = Helper::GetJSONValue<string>(attJson, ATTRIBUTE_SETTABLE_KEY, true);
				Logger::Debug("UIHelper::CreateWidgetsOnOutBoundTreeWidget() - attSettable:: " + attSettable);
				string attRestApiExposed = Helper::GetJSONValue<string>(attJson, ATTRIBUTE_REST_API_EXPOSED, true);
				Logger::Debug("UIHelper::CreateWidgetsOnOutBoundTreeWidget() - attRestApiExposed:: " + attRestApiExposed);

				if (attRestApiExposed == "false")
					continue;
				if (attValue.find("ref(centric:)") != -1)
				{
					attValue = "";
				}
				else
				{
					int found = attValue.find_first_of(":");
					if (found != -1)
						attValue = attValue.substr(found + 1);
				}

				if (_mode == "CREATE")
				{
					if (attSettable == "false")// || !isAttUpdatable)
						isAttSettable = true;
				}

				if (_mode == "UPDATE")
				{
					if (attSettable == "false" && !isAttUpdatable)// || !isAttUpdatable)
						isAttSettable = true;
				}

				string attName = Helper::GetJSONValue<string>(attJson, ATTRIBUTE_NAME_KEY, true);
				Logger::Debug("UIHelper::CreateWidgetsOnOutBoundTreeWidget() - attName:: " + attName);
				//string attType = Helper::GetJSONValue<string>(attJson, ATTRIBUTE_TYPE_KEY, true);
				bool attRequired = Helper::IsValueTrue((Helper::GetJSONValue<string>(attJson, ATTRIBUTE_REQUIRED_KEY, true)));
				Logger::Debug("UIHelper::CreateWidgetsOnOutBoundTreeWidget() - attRequired:: " + to_string(attRequired));
				if (!FormatHelper::HasContent(attValue))
					attValue = BLANK;
				json enumJson = json::object();
				json attEnumsJson = json::array();
				string attEnumVale = BLANK;
				string attEnumKey = BLANK;
				if (attType == MOA_LIST_ATT_TYPE_KEY)
				{
					QTreeWidgetItem* topLevel = new QTreeWidgetItem();
					QListWidget* listWidget = new QListWidget();

					attEnumsJson = Helper::GetJSONParsedValue<string>(attJson, ENUMLIST_JSON_KEY, false);
					for (int i = 0; i < attEnumsJson.size(); i++)
					{
						enumJson = Helper::GetJSONParsedValue<int>(attEnumsJson, i, false);
						attEnumVale = Helper::GetJSONValue<string>(enumJson, ENUM_VALUE_KEY, true);
						attEnumKey = Helper::GetJSONValue<string>(enumJson, ENUM_KEY, true);
						CVWidgetGenerator::CreateCVMultiListWidget(listWidget, QString::fromStdString(attEnumVale), QString::fromStdString(attEnumKey), QString::fromStdString(attValue));
					}

					listWidget->sortItems(Qt::AscendingOrder);
					listWidget->setFocusPolicy(Qt::NoFocus);
					listWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
					listWidget->setProperty(ATTRIBUTE_INITIAL_VALUE.c_str(), QString::fromStdString(attValue));

					listWidget->setStyleSheet("QListWidget{border-bottom: 1px inset #262626; font-size: 10px; font-face: ArialMT; border-left: 1px inset #0D0D0D; max-height:70px; border-top: 1px inset #0D0D0D; border-right: 1px inset #262626; background-color: #222224; } QListWidget::item{ background-color: #222224; margin: 2px;} QListWidget::indicator { width: 10px; height:10px; } QListWidget::indicator:unchecked {background-color: #343434 ; border: 1px solid #3B3B3B;} QListWidget::indicator:checked {image: url(:/CLOVise/PLM/Images/icon_ok_over.svg);background-color: #343434; border: 1px solid #3B3B3B;}");

					_treeWidget->addTopLevelItem(topLevel);
					_treeWidget->setItemWidget(topLevel, 0, CVWidgetGenerator::CreateLabelWidget(attName, attKey, attType, attRequired, !isAttSettable));	// Adding label at column 1
					if (!isAttSettable)
					{
						listWidget->setDisabled(true);
						listWidget->setStyleSheet("QListWidget{ color: #808080; border-bottom: 1px inset #262626; font-size: 10px; font-face: ArialMT; border-left: 1px inset #0D0D0D; max-height:70px; border-top: 1px inset #0D0D0D; border-right: 1px inset #262626; background-color: #222224; } QListWidget::item{ color: #808080; background-color: #222224; margin: 2px;} QListWidget::indicator { width: 10px; height:10px; } QListWidget::indicator:unchecked {background-color: #343434 ; border: 1px solid #3B3B3B;} QListWidget::indicator:checked {image: url(:/CLOVise/PLM/Images/icon_ok_over.svg);background-color: #343434; border: 1px solid #3B3B3B;}");
					}
					_treeWidget->setItemWidget(topLevel, 1, listWidget);

				}
				else if (attType == TEXT_AREA_ATT_TYPE_KEY)
				{
					QTreeWidgetItem* topLevel = new QTreeWidgetItem();  // Creating new TreeWidgetItem		

					_treeWidget->addTopLevelItem(topLevel);				// Adding ToplevelItem
					_treeWidget->setItemWidget(topLevel, 0, CVWidgetGenerator::CreateLabelWidget(attName, attKey, attType, attRequired, !isAttSettable));	// Adding label at column 1
					_treeWidget->setItemWidget(topLevel, 1, CVWidgetGenerator::CreateTextAreaWidget(attValue, isAttSettable));

				}
				else if (attType == DATE_ATT_TYPE_KEY || attType == TIME_ATT_TYPE_KEY)
				{
					PublishToPLMData::GetInstance()->SetDateFlag(true);
					QStringList dateList;
					QTreeWidgetItem* topLevel = new QTreeWidgetItem();  // Creating new TreeWidgetItem
					_treeWidget->addTopLevelItem(topLevel);			// Adding ToplevelItem
					_treeWidget->setItemWidget(topLevel, 0, CVWidgetGenerator::CreateLabelWidget(attName, attKey, attType, attRequired, !isAttSettable));	// Adding label at column 1
					_treeWidget->setItemWidget(topLevel, 1, CVWidgetGenerator::CreateDateWidget(attValue, isAttSettable));

				}
				else if (attType == INTEGER_ATT_TYPE_KEY || attType == CONSTANT_ATT_TYPE_KEY)
				{
					QTreeWidgetItem* topLevel = new QTreeWidgetItem();  // Creating new TreeWidgetItem
					QSpinBox* spinBixWidget = new QSpinBox();
					spinBixWidget->setMaximumHeight(SPINBOX_HEIGHT);
					spinBixWidget->setStyleSheet(SPINBOX_STYLE);
					_treeWidget->addTopLevelItem(topLevel);						// Adding ToplevelItem
					_treeWidget->setItemWidget(topLevel, 0, CVWidgetGenerator::CreateLabelWidget(attName, attKey, attType, attRequired, !isAttSettable));	// Adding label at column 1
					spinBixWidget = CVWidgetGenerator::CreateSpinBoxWidget(QString::fromStdString(attValue), BLANK);
					_treeWidget->setItemWidget(topLevel, 1, spinBixWidget);
					if (!isAttSettable)
					{
						// for desable 
						spinBixWidget->setDisabled(true);
						spinBixWidget->setStyleSheet(DISABLED_SPINBOX_STYLE);
					}

				}
				else if (attType == FLOAT_ATT_TYPE_KEY || attType == DOUBLE_ATT_TYPE_KEY)
				{
					QTreeWidgetItem* topLevel = new QTreeWidgetItem();  // Creating new TreeWidgetItem
					QDoubleSpinBox* doubleSpinBoxWidget = new QDoubleSpinBox();
					doubleSpinBoxWidget->setMaximumHeight(SPINBOX_HEIGHT);
					string attDecimals = Helper::GetJSONValue<string>(attJson, DECIMAP_FIGURES, true);
					_treeWidget->addTopLevelItem(topLevel);						// Adding ToplevelItem
					_treeWidget->setItemWidget(topLevel, 0, CVWidgetGenerator::CreateLabelWidget(attName, attKey, attType, attRequired, !isAttSettable));	// Adding label at column 1
					doubleSpinBoxWidget = CVWidgetGenerator::CreateDoubleSpinBoxWidget(QString::fromStdString(attValue), BLANK, "0", 0.0000, 10000000000000.0000);
					_treeWidget->setItemWidget(topLevel, 1, doubleSpinBoxWidget);
					if (!isAttSettable)
					{
						// for desable 
						doubleSpinBoxWidget->setDisabled(true);
						doubleSpinBoxWidget->setStyleSheet(DISABLED_DOUBLE_SPINBOX_STYLE);
					}

				}
				else if (attType == CURRENCY_ATT_TYPE_KEY)
				{
					QTreeWidgetItem* topLevel = new QTreeWidgetItem();  // Creating new TreeWidgetItem
					QDoubleSpinBox* doubleSpinBoxWidget = new QDoubleSpinBox();

					string attDecimals = Helper::GetJSONValue<string>(attJson, DECIMAP_FIGURES, true);
					string attCurrencyCode = Helper::GetJSONValue<string>(attJson, CURRENCY_CODE, true) + " ";
					_treeWidget->addTopLevelItem(topLevel);						// Adding ToplevelItem
					_treeWidget->setItemWidget(topLevel, 0, CVWidgetGenerator::CreateLabelWidget(attName, attKey, attType, attRequired, !isAttSettable));	// Adding label at column 1
					doubleSpinBoxWidget = CVWidgetGenerator::CreateDoubleSpinBoxWidget(QString::fromStdString(attValue), BLANK + attCurrencyCode, attDecimals, 0.00, 10000000000000.00);
					_treeWidget->setItemWidget(topLevel, 1, doubleSpinBoxWidget);
					if (!isAttSettable)
					{
						// for desable 
						doubleSpinBoxWidget->setDisabled(true);
						doubleSpinBoxWidget->setStyleSheet(DISABLED_DOUBLE_SPINBOX_STYLE);
					}

				}
				else if (attType == BOOLEAN_ATT_TYPE_KEY)
				{
					QTreeWidgetItem* topLevel = new QTreeWidgetItem();  // Creating new TreeWidgetItem

					_treeWidget->addTopLevelItem(topLevel);						// Adding ToplevelItem
					_treeWidget->setItemWidget(topLevel, 0, CVWidgetGenerator::CreateLabelWidget(attName, attKey, attType, attRequired, !isAttSettable));	// Adding label at column 1
					_treeWidget->setItemWidget(topLevel, 1, CVWidgetGenerator::CreateBooleanWidget(attValue, isAttSettable, topLevel, 1));	// Adding label at column 2

				}
				else if (attType == USER_LIST_ATT_TYPE_KEY || attType == CHOICE_ATT_TYPE_KEY || attType == REF_ATT_TYPE_KEY || attType == ENUM_ATT_TYPE_KEY)
				{
					QTreeWidgetItem* topLevel = new QTreeWidgetItem();  // Creating new TreeWidgetItem
					if (attType == REF_ATT_TYPE_KEY || attType == ENUM_ATT_TYPE_KEY)
					{
						attEnumsJson = Helper::GetJSONParsedValue<string>(attJson, ENUMLIST_JSON_KEY, false);
					}
					else
					{
						attEnumsJson = Helper::GetJSONParsedValue<string>(attJson, USER_LIST_ATT_TYPE_KEY, false);
					}
					QStringList valueList;
					Logger::Debug("UIHelper::CreateWidgetsOnOutBoundTreeWidget() - ComboBoxItem:: 1");
					ComboBoxItem* comboBox = new ComboBoxItem();

					comboBox->setFocusPolicy(Qt::StrongFocus);
					comboBox->setStyleSheet(COMBOBOX_STYLE);
					comboBox->addItem(QString::fromStdString(BLANK));
					comboBox->setProperty(ATTRIBUTE_INITIAL_VALUE.c_str(), QString::fromStdString(attValue));

					for (int i = 0; i < attEnumsJson.size(); i++)
					{
						attJson = Helper::GetJSONParsedValue<int>(attEnumsJson, i, false);///use new method
						attEnumVale = Helper::GetJSONValue<string>(attJson, ENUM_VALUE_KEY, true);
						attEnumKey = Helper::GetJSONValue<string>(attJson, ENUM_KEY, true);
						if (attKey != Configuration::GetInstance()->GetSubTypeInternalName())
						{
							valueList.push_back(QString::fromStdString(attEnumVale));
						}
						comboBox->setProperty(attEnumVale.c_str(), QString::fromStdString(attEnumKey));
						comboBox->setProperty("LabelName", QString::fromStdString(attKey));
					}
					valueList.sort();
					comboBox->addItems(valueList);
					int index = comboBox->findText(QString::fromStdString(attValue));
					if (index != -1) { // -1 for not found
						comboBox->setCurrentIndex(index);
					}
					if (!isAttSettable)
					{
						comboBox->setDisabled(true);
						comboBox->setStyleSheet(DISABLED_COMBOBOX_STYLE);
					}
					if (comboBox->isEnabled())
					{
						Logger::Debug("UIHelper::CreateWidgetsOnOutBoundTreeWidget() - ComboBoxItem:: 2");
						comboBox->setEditable(true);
						comboBox->fillItemListAndDefaultValue(valueList, comboBox->currentText());
						QCompleter* m_nameCompleter = new QCompleter(valueList, comboBox); // using QCompleter class
						m_nameCompleter->setCompletionMode(QCompleter::PopupCompletion);
						m_nameCompleter->setCaseSensitivity(Qt::CaseInsensitive);
						comboBox->setCompleter(m_nameCompleter);
					}
					Logger::Debug("UIHelper::CreateWidgetsOnOutBoundTreeWidget() - ComboBoxItem:: 3");
					_treeWidget->addTopLevelItem(topLevel);						// Adding ToplevelItem
					_treeWidget->setItemWidget(topLevel, 0, CVWidgetGenerator::CreateLabelWidget(attName, attKey, attType, attRequired, !isAttSettable));	// Adding label at column 1
					_treeWidget->setItemWidget(topLevel, 1, comboBox);
				}
				else if (attType == STRING_ATT_TYPE_KEY || attType == TEXT_ATT_TYPE_KEY)
				{
					QTreeWidgetItem* topLevel = new QTreeWidgetItem();  // Creating new TreeWidgetItem
					QLineEdit* LineEdit = new QLineEdit();						// Creating new LineEdit Widget
					LineEdit->setAttribute(Qt::WA_MacShowFocusRect, false);
					LineEdit->setStyleSheet(LINEEDIT_STYLE);
					LineEdit->setProperty(ATTRIBUTE_INITIAL_VALUE.c_str(), QString::fromStdString(attValue));
					LineEdit->setText(QString::fromStdString(attValue));
					_treeWidget->addTopLevelItem(topLevel);						// Adding ToplevelItem
					_treeWidget->setItemWidget(topLevel, 0, CVWidgetGenerator::CreateLabelWidget(attName, attKey, attType, attRequired, !isAttSettable));	// Adding label at column 1
					if (!isAttSettable)
					{
						LineEdit->setDisabled(true);
						LineEdit->setStyleSheet(DISABLED_LINEEDIT_STYLE);
					}
					_treeWidget->setItemWidget(topLevel, 1, LineEdit);

				}
			}
		}
		catch (string msg)
		{
			UTILITY_API->DeleteProgressBar(true);
			Logger::Error("UIHelper->CreateWidgetsOnOutBoundTreeWidget() Exception - " + msg);

		}
		catch (exception & e)
		{
			UTILITY_API->DeleteProgressBar(true);
			Logger::Error("UIHelper -> CreateWidgetsOnOutBoundTreeWidget() Exception - " + string(e.what()));

		}
		catch (const char* msg)
		{
			UTILITY_API->DeleteProgressBar(true);
			wstring wstr(msg, msg + strlen(msg));
			Logger::Error("UIHelper -> CreateWidgetsOnOutBoundTreeWidget() Exception - " + string(msg));

		}
		Logger::Debug("DEBUG::UIHelper: CreateWidgetsOnOutBoundTreeWidget() -> End");
	}


	/*
	* Description - GetLocalizedClassNames()  method used to get the localized value for class names.
	* Parameter - 
	* Exception -
	* Return -
	*/
	inline void GetLocalizedClassNames()
	{
		Logger::Info("Helper -> GetLocalizedClassNames() -> Start");
		try
		{
			QStringList classNameList;
			classNameList << "Style" << "Material" << "Color";
			for (int index = 0; index < classNameList.size(); index++)
			{
				string localizedResponse = RESTAPI::CentricRestCallGet(Configuration::GetInstance()->GetPLMServerURL() + RESTAPI::LOCALIZATION_API + classNameList[index].toStdString() + "/en", APPLICATION_JSON_TYPE, "");
				Logger::Debug("Helper -> GetLocalizedClassNames() -> localizedResponse:: " + localizedResponse);
				json localizedResponseJson = json::parse(localizedResponse);
				string localizedClassName = Helper::GetJSONValue<string>(localizedResponseJson, "class_display_name", true);
				Logger::Debug("Helper -> GetLocalizedClassNames() -> localizedClassName:: " + localizedClassName);
				if (classNameList[index].toStdString() == STYLE_MODULE)
					Configuration::GetInstance()->SetLocalizedStyleClassName(localizedClassName);

				else if (classNameList[index].toStdString() == MATERIAL_MODULE)
					Configuration::GetInstance()->SetLocalizedMaterialClassName(localizedClassName);

				else if (classNameList[index].toStdString() == COLOR_MODULE)
					Configuration::GetInstance()->SetLocalizedColorClassName(localizedClassName);
			}
		}
		catch (string msg)
		{
			UTILITY_API->DeleteProgressBar(true);
			Logger::Error("UIHelper->GetLocalizedClassNames() Exception - " + msg);

		}
		catch (exception & e)
		{
			UTILITY_API->DeleteProgressBar(true);
			Logger::Error("UIHelper -> GetLocalizedClassNames() Exception - " + string(e.what()));

		}
		catch (const char* msg)
		{
			UTILITY_API->DeleteProgressBar(true);
			wstring wstr(msg, msg + strlen(msg));
			Logger::Error("UIHelper -> GetLocalizedClassNames() Exception - " + string(msg));

		}
		Logger::Info("Helper -> GetLocalizedClassNames() -> End");
	}

	/*
	* Description - GetButtonWidgetFromCell()  method used to get the button widget from the cell.
	* Parameter -QTableWidget, int, int, int.
	* Exception -
	* Return - QPushButton
	*/
	inline QPushButton* GetButtonWidgetFromCell(QTableWidget* _table, int _row, int _col,int _index)
	{
		Logger::Info("INFO::Helper -> getButtonWidgetFromCell() -> Start");
		QPushButton* button = nullptr;

		if (QWidget* widget = _table->cellWidget(_row, _col))
		{
			if (QLayout* layout = widget->layout())
			{
				if (QLayoutItem* layoutItem = layout->itemAt(_index))
				{
					if (QWidgetItem* widgetItem = dynamic_cast<QWidgetItem*>(layoutItem))
					{
						button = qobject_cast<QPushButton*>(widgetItem->widget());
					}
				}
			}
		}
		Logger::Info("INFO::Helper -> getButtonWidgetFromCell() -> End");
		return button;
	}
}
