#pragma once
/*
* Copyright 2020-2021 CLO-Vise. All rights reserved
*
* @file FormatHelper.h
*
* @brief Class contains all helper calasses for CLOVise.
*
* @author GoVise
*
* @date 27-MAY-2020
*/
#include <string>
#include <sstream>  
#include <iomanip>
#include "CLOVise/PLM/Helper/Util/Definitions.h"
#include "CLOVise/PLM/Libraries/json.h"
using namespace std;
using json = nlohmann::json;

namespace FormatHelper
{
	/*
	* Description - Parsebool() method used to parse string to bool.
	* Parameter -  string.
	* Exception - 
	* Return - bool.
	*/
	inline bool ParseBoolean(string _value) {
		if (_value == "YES" || _value == "1" || _value == "yes") {
			return true;
		}
		else {
			return false;
		}
	}

	/*
	* Description - FormatInt() method used to convert integer to string.
	* Parameter -  int.
	* Exception - 
	* Return - string.
	*/
	inline string FormatInt(int _size) {
		stringstream ss;
		ss << _size;
		string size;
		ss >> size;
		return size;
	}

	/*
	* Description - ParseInt() method used to convert string to integer.
	* Parameter -  string.
	* Exception - 
	* Return - int.
	*/
	inline  int ParseInt(string _str) {
		return stoi(_str);
	}

	/*
	* Description - SetFloatPrecision() method used to set float precision.
	* Parameter -  string, int.
	* Exception - 
	* Return - string.
	*/
	inline string SetFloatPrecision(string _str,int _precision) {
		float pi = stof(_str);
		std::stringstream stream;
		stream << std::fixed << std::setprecision(_precision) << pi;
		std::string s = stream.str();
		return s;
	}

	/*
	* Description - SetDoublePrecision() method used to set double precision.
	* Parameter -  string, int.
	* Exception - 
	* Return - string.
	*/
	inline string SetDoublePrecision(string _str, int _precision) {
		double pi = stod(_str);
		std::stringstream stream;
		stream << std::fixed << std::setprecision(_precision) << pi;
		std::string s = stream.str();
		return s;
	}

	/*
	* Description - hasContent() method used to check if the string has any content within it.
	* Parameter -  string.
	* Exception - 
	* Return - bool.
	*/
	inline bool HasContent(string _value) {
		if (_value == "null" || _value == "NULL" || _value == "undefined" || _value == "0" || _value == "0.0" || _value == "0,0" || _value == "") {
			return false;
		}
		if ((_value[0] == '0' && _value[1] == '.' && _value[2] == '0') | (_value[0] == '0' && _value[1] == ',' && _value[2] == '0')) {
			bool allZeros = true;
			for (int i = 0; i = 2; i < _value.length()) {
				if (_value[i] != 0) {
					allZeros = false;
					break;
				}
			}
			if (allZeros) {
				return false;
			}
		}

		bool whiteSpacesOnly = std::all_of(_value.begin(), _value.end(), ::isspace);

		if (whiteSpacesOnly)
		{
			return false;
		}
		return true;
	}

	/*
	* Description - HasContentAllowZero() method used to check if the string has any content within it. this allows zero as valid content
	* Parameter -  string.
	* Exception -
	* Return - bool.
	*/
	inline bool HasContentAllowZero(string _value) {
		if (_value == "null" || "NULL" == _value || "undefined" == _value)
		{
			return false;
		}
		bool whiteSpacesOnly = std::all_of(_value.begin(), _value.end(), ::isspace);
		if (whiteSpacesOnly)
		{
			return false;
		}
		return true;
	}

	/*
	* Description - hasError() method used to check if the string has error message.
	* Parameter -  string.
	* Exception - 
	* Return - bool.
	*/
	inline bool HasError(string& _value) {
		
		QString error = QString::fromStdString(_value);
		if (error.startsWith("Unable to process"))
		{
			return true;
		}
		else if (_value == SERVER_DOWN_RESPOSE)
		{
			return true;
		}
		else if (error.contains("exception") && error.contains("message"))
		{
			int index = _value.find('{');
			_value = _value.substr(index, _value.length() + 1);
			json message = json::parse(_value);
			string messageStr = message["message"].dump();
			messageStr.erase(std::remove(messageStr.begin(), messageStr.end(), '"'), messageStr.end());
			//UTILITY_API->DisplayMessageBox(messageStr);
			_value = messageStr;
			return true;
		}

		return false;
	}

	/*
	* Description - removeCharacter() method used to remove a character from a string.
	* Parameter -  QString, bool, string.
	* Exception - 
	* Return - string.
	*/
	inline string RemoveCharacter(string _str, string _character) {
		string to = "";
		size_t start_pos = 0;
		while ((start_pos = _str.find(_character, start_pos)) != std::string::npos) {
			_str.replace(start_pos, _character.length(), to);
			start_pos += to.length();
		}
		return _str;
	}

	/*
	* Description - removeSpaces() method used to  remove spaces from a string.
	* Parameter -  string.
	* Exception - 
	* Return - string
	*/
	inline string RemoveSpaces(string _str) {
		return RemoveCharacter(_str, " ");
	}

	/*
	* Description - replaceCharacter() method used to replace a character in a string.
	* Parameter -  string, string, string.
	* Exception - 
	* Return - string.
	*/
	inline string ReplaceCharacter(string _str, string _oldChar, string _newChar) {
		size_t start_pos = 0;
		while ((start_pos = _str.find(_oldChar, start_pos)) != std::string::npos) {
			_str.replace(start_pos, _oldChar.length(), _newChar);
			start_pos += _newChar.length();
		}
		return _str;
	}

	/*
	* Description - DrawSearchUI() method used to replace a string within a string.
	* Parameter -  string, string, string.
	* Exception - 
	* Return - string
	*/
	inline string ReplaceString(string _str, string _oldString, string _newString) {
		size_t start_pos = 0;
		while ((start_pos = _str.find(_oldString, start_pos)) != std::string::npos) {
			_str.replace(start_pos, _oldString.length(), _newString);
			start_pos += _newString.length();
		}
		return _str;
	}

	/*
	* Description - ReplaceString() method used to replace string in required place.
	* Parameter -  QString, QString, QString.
	* Exception -
	* Return - QString.
	*/
	inline QString ReplaceString(QString _str, QString _oldString, QString _newString) {
		return QString::fromStdString(ReplaceString(_str.toStdString(), _oldString.toStdString(), _newString.toStdString()));
	}

	/*
	* Description - ReplaceString() method used to replace string in required place.
	* Parameter -  QString, string, string.
	* Exception - 
	* Return - QString.
	*/
	inline QString ReplaceString(QString _str, string _oldString, string _newString) {
		return QString::fromStdString(ReplaceString(_str.toStdString(), _oldString, _newString));
	}

	/*
	* Description - FormatDate() method used to  re arrange date format.
	* Parameter -  QDateEdit.
	* Exception - 
	* Return - string.
	*/
	inline string FormatDate(QDateEdit* _inputDate) {
		QDate date = _inputDate->date();
		int year = date.year();
		int month = date.month();
		int day = date.day();
		//string dateStr = to_string(month) + "/" + to_string(day) + "/" + to_string(year);
		string dateStr = to_string(year) +"-" + to_string(month) + "-" + to_string(day);
		return dateStr;
	}

	/*
	* Description - RetrieveDate() method used to  re arrange date format.
	* Parameter -  QDateEdit.
	* Exception -
	* Return - string.
	*/
	inline string RetrieveDate(QDateEdit* _inputDate)
	{
		string dateStr = "";
		QLineEdit *edit = _inputDate->findChild<QLineEdit*>("qt_spinbox_lineedit");
		if (!edit->text().isEmpty())
		{
			dateStr = edit->text().toStdString();
		}
		return dateStr;
	}

}