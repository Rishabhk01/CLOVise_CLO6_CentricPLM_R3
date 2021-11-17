#pragma once
/*
* Copyright 2020-2021 CLO-Vise. All rights reserved
*
* @file Helper.h
*
* @brief Class contains all helper calasses for CLOVise.
*
* @author GoVise
*
* @date 27-MAY-2020
*/
#include <fstream>
#include <sstream>
#include <sys/stat.h>
#include <sys/types.h>

#include <QString>
#include <QStringList>
#include <QVariant>
#include "qdir.h"


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

#include <time.h>
#include <QTextStream>
#include <QFile>
#include "qfileinfo.h"
#include "CLOAPIInterface.h"
#pragma comment(lib, "libcurl.lib")
#pragma comment(lib, "wldap32.lib")
#pragma comment(lib, "ws2_32.lib")
#include <iostream>

#include <QLineEdit>
#include <QTextEdit>
#include <QDateTimeEdit>
#include <QComboBox>
#include <QLabel>

#include "CLOVise/PLM/Libraries/curl/include/curl/curl.h"
#include "CLOVise/PLM/Libraries/jsonQt.h"
#include "CLOVise/PLM/Libraries/json.h"
#include "CLOVise/PLM/Helper/Util/DirectoryUtil.h"
#include "CLOVise/PLM/Helper/Util/Definitions.h"
#include "CLOVise/PLM/Helper/Util/Configuration.h"
#include "CLOVise/PLM/Helper/Util/FormatHelper.h"
#include "CLOVise/PLM/Helper/Util/DataUtility.h"
#include "CLOVise/PLM/Helper/Util/Logger.h"
#include "classes/APIStorage.h"
//#include "CLOVise/PLM/Helper/Util/RestAPIUtility.h"

using namespace std;
using QtJson::JsonObject;
using json = nlohmann::json;

namespace Helper
{
	/*
	* Description - GetFileSize() method used to calculates size of the content from file.
	* Parameter -  string.
	* Exception - 
	* Return - size_t.
	*/
	static size_t GetFileSize(const std::string& _path)
	{
		std::streampos begin, end;
		std::ifstream myFile(_path.c_str(), std::ios::binary);
		begin = myFile.tellg();
		myFile.seekg(0, std::ios::end);
		end = myFile.tellg();
		myFile.close();
		return ((end - begin) > 0) ? (end - begin) : 0;
	}

	/*
	* Description - ConvertBytesToMB() method used to convert bytes to MB.
	* Parameter -  int.
	* Exception -
	* Return - long.
	*/
	inline long ConvertBytesToMB(int _bytes)
	{
		long long MegaBytes = 0;
		MegaBytes = _bytes / (1024 * 1000);
		return MegaBytes;
	}

	/*
	* Description - RemoveSpaces() method used to removes spaces from input string and returns back.
	* Parameter -  string.
	* Exception - 
	* Return - string.
	*/
	static std::string RemoveSpaces(string  _inputString)
	{
		_inputString.erase(remove(_inputString.begin(), _inputString.end(), ' '), _inputString.end());
		return _inputString;
	}

	/*
	* Description - trim() method used to trim the string.
	* Parameter -  string.
	* Exception - 
	* Return - string
	*/
	inline string Trim(string _stripString)
	{
		while (!_stripString.empty() && isspace(*_stripString.begin()))
			_stripString.erase(_stripString.begin());

		while (!_stripString.empty() && isspace(*_stripString.rbegin()))
			_stripString.erase(_stripString.length() - 1);

		return _stripString;
	}

	/*
	* Description - CustomMessageBox() method used to displays message on screen, when is enabled is true.
	* Parameter -  string.
	* Exception - 
	* Return -
	*/
	inline void CustomMessageBox(string _inputString)
	{
		//Move as global variable
		bool isEnabled = false;	//true false

		if (isEnabled)
		{
			UTILITY_API->DisplayMessageBox(_inputString);
		}
	}

	static string basicAuthorizationString;
	
	/*
	* Description - IsBase64Char() method used to Checks the input char is base64 char or not.
	* Parameter -  char.
	* Exception - 
	* Return - bool.
	*/
	inline bool IsBase64Char(unsigned char _base64Char)
	{
		return (isalnum(_base64Char) || (_base64Char == '+') || (_base64Char == '/'));
	}

	/*
	* Description - ReadFile() method used to Reads content of file and returns string of content, given file name as parameter.
	* Parameter -  string.
	* Exception - 
	* Return - QString.
	*/
	inline QString ReadFile(const QString& _fileName)
	{
		Logger::Info("Helper::ReadFile() Started...");
		QFile file(_fileName);
		if (!file.open(QFile::ReadWrite | QFile::Text))
		{
			return QString();
		}
		else
		{
			QTextStream in(&file);
			return in.readAll();
		}
		Logger::Info("Helper::ReadFile() Ended...");

	}

	/*
	* Description - ConvertToBase64() method used to converts input string to Base64 string.
	* Parameter -  string.
	* Exception - 
	* Return - string.
	*/
	inline string ConvertToBase64(const std::string& _inputString)
	{
		Logger::Info("Helper::ConvertToBase64() Started...");
		std::string base64String;
		int upperLimit = 0, lowerLimit = -6;

		for (unsigned char c : _inputString)
		{
			upperLimit = (upperLimit << 8) + c;
			lowerLimit += 8;
			while (lowerLimit >= 0)
			{
				base64String.push_back(BASE64_CHARS[(upperLimit >> lowerLimit) & 0x3F]);
				lowerLimit -= 6;
			}
		}

		if (lowerLimit > -6) base64String.push_back(BASE64_CHARS[((upperLimit << 8) >> (lowerLimit + 8)) & 0x3F]);

		while (base64String.size() % 4) base64String.push_back('=');
		Logger::Info("Helper::ConvertToBase64() Ended...");
		return base64String;
	}

	/*
	* Description - ReadJsonFile() method used to reads json file and creates json object.
	* Parameter -  QString, QString.
	* Exception - exception, Char *
	* Return -
	*/
	static void ReadJsonFile(QString _filePath, QString& _searchKey)
	{
		Logger::Info("Helper::ReadJsonFile() Started...");
		QString jsonString;
		try {
			jsonString = ReadFile(_filePath); // Reading the JSON file
		}
		catch (errno_t)
		{
			qFatal("Error in readJsonFile() - Could not read JSON file.");
		}

		bool parseSuccessful;
		JsonObject jsonObject;
		try {
			jsonObject = QtJson::parse(jsonString, parseSuccessful).toMap(); // Parsing the JSON file and storing it to JSON Object
		}
		catch (const char* e)
		{
			throw e;
		}
		catch (string e)
		{
			throw e;
		}
		catch (exception& e)
		{
			throw e;
		}
		if (!parseSuccessful)
		{
			qFatal("Error in readJsonFile() - An error occurred while parsing JSON file");
		}

		JsonObject urlsJSON = jsonObject["urlList"].toMap(); //Get URLs from JSON Object jsonObject. @TO-Do urlList move to Definitions.h
		_searchKey = urlsJSON[_searchKey].toString();
		Logger::Info("Helper::ReadJsonFile() Ended...");

	}

	/*
	* Description - SetUserIdAndPassword() method used to Forms Basic authorization string using user Id and Password for rest basic authentication.
	* Parameter -  string, string.
	* Exception - 
	* Return -
	*/
	static void SetUserIdAndPassword(string _userId, string _password)
	{
		basicAuthorizationString = BASIC + ConvertToBase64(_userId + ":" + _password);
	}

	/*
	* Description - DecodeBase64() method used to decodes base 64 encoded string returns.
	* Parameter -  QString, bool, string.
	* Exception - 
	* Return - string
	*/
	inline string DecodeBase64(std::string const& _encodedString)
	{
		Logger::Info("Helper::DecodeBase64() Started...");
		int encodedStringLength = _encodedString.size();
		int i = 0;
		int j = 0;
		int charIndex = 0;
		unsigned char charArrayOf4[4], charArrayOf3[3];
		std::string decodedString;

		while (encodedStringLength-- && (_encodedString[charIndex] != '=') && IsBase64Char(_encodedString[charIndex]))
		{
			charArrayOf4[i++] = _encodedString[charIndex]; charIndex++;
			if (i == 4) {
				for (i = 0; i < 4; i++)
					charArrayOf4[i] = BASE64_CHARS.find(charArrayOf4[i]);

				charArrayOf3[0] = (charArrayOf4[0] << 2) + ((charArrayOf4[1] & 0x30) >> 4);
				charArrayOf3[1] = ((charArrayOf4[1] & 0xf) << 4) + ((charArrayOf4[2] & 0x3c) >> 2);
				charArrayOf3[2] = ((charArrayOf4[2] & 0x3) << 6) + charArrayOf4[3];

				for (i = 0; (i < 3); i++)
				{
					decodedString += charArrayOf3[i];
				}

				i = 0;
			}
		}
		if (i)
		{
			for (j = i; j < 4; j++)
				charArrayOf4[j] = 0;

			for (j = 0; j < 4; j++)
				charArrayOf4[j] = BASE64_CHARS.find(charArrayOf4[j]);

			charArrayOf3[0] = (charArrayOf4[0] << 2) + ((charArrayOf4[1] & 0x30) >> 4);
			charArrayOf3[1] = ((charArrayOf4[1] & 0xf) << 4) + ((charArrayOf4[2] & 0x3c) >> 2);
			charArrayOf3[2] = ((charArrayOf4[2] & 0x3) << 6) + charArrayOf4[3];

			for (j = 0; (j < i - 1); j++)
			{
				decodedString += charArrayOf3[j];
			}
		}
		Logger::Info("Helper::DecodeBase64() Ended...");
		return decodedString;
	}

	/*
	* Description - CheckErrorDescription() method used to check the error description in Rest call response.
	* Parameter -  string.
	* Exception - exception, Char *
	* Return - string.
	*/
	static std::string CheckErrorDescription(string _response)
	{
		Logger::Info("Helper::CheckErrorDescription() Started...");
		string finalErrorMessage = "";
		int indexForJSON = _response.find("{");
		string strForJSON = _response.substr(indexForJSON);
		json responseJson, brokenRulesJson;
		string brokenRules, exceptionStr;
		try {
			responseJson = json::parse(QString::fromStdString(strForJSON).toUtf8().constData());
			brokenRules = responseJson["brokenRules"].dump();
			exceptionStr = responseJson["exception"].dump();
			brokenRulesJson = json::parse(brokenRules);
		}
		catch (const char* e)
		{
			throw e;
		}
		catch (string e)
		{
			throw e;
		}
		catch (exception& e)
		{
			throw e;
		}
		size_t found1 = _response.find(UNAUTHORIZED);
		size_t found2 = _response.find(INTERNAL_SERVER_ERROR);
		size_t found3 = _response.find(TIMEOUT_ERROR);
		if (found1 != string::npos)
		{
			Helper::CustomMessageBox(finalErrorMessage);
			return finalErrorMessage;
		}
		if (found2 != string::npos)
		{
			finalErrorMessage = responseJson["error"].dump();
			return finalErrorMessage;
		}
		if (found3 != string::npos)
		{
			finalErrorMessage = TIMEOUT_ERROR_MESSAGE;
			return finalErrorMessage;
		}
		string errorJson;
		json ruleDescriptionMsg;
		if (brokenRulesJson.size() != 0)
		{
			for (int i = 0; i < brokenRulesJson.size(); i++)
			{
				errorJson = brokenRulesJson[i].dump();
				try
				{
					ruleDescriptionMsg = json::parse(errorJson);
				}
				catch (exception& e)
				{
					throw e;
				}
				catch (const char* e)
				{
					throw e;
				}
				catch (string e)
				{
					throw e;
				}
				finalErrorMessage = ruleDescriptionMsg["ruleDescription"].dump();
				return finalErrorMessage;
			}
		}
		if (exceptionStr != "null") {
			json exceptionJson;
			try {
				exceptionJson = json::parse(exceptionStr);
			}
			catch (exception& e)
			{
				throw e;
			}
			catch (const char* e)
			{
				throw e;
			}
			catch (string e)
			{
				throw e;
			}
			finalErrorMessage = responseJson["exception"]["Message"].dump();
			return finalErrorMessage;
		}
		Logger::Info("CheckErrorDescription() Ended...");
		return finalErrorMessage;
	}

	/*
	* Description - progress_callback() method used to monitor rest call progress.
	* Parameter -  void*, curl_off_t, curl_off_t, curl_off_t.
	* Exception - 
	* Return - int.
	*/
	inline int progress_callback(void* _clientp, curl_off_t _dltotal, curl_off_t _dlnow, curl_off_t _ultotal, curl_off_t _ulnow) {

		UTILITY_API->SetProgress(DataUtility::GetInstance()->GetProgressBarMsg(), DataUtility::GetInstance()->GetProgressCounter());
		float count = DataUtility::GetInstance()->GetProgressCounter();
		if (count < 95 && (((double)_dltotal) != ((double)_dlnow) || ((double)_ultotal) != ((double)_ulnow))) {
			count = count + 0.01;
			DataUtility::GetInstance()->SetProgressCounter(count);
		}

		return 0;
	}
	
	/*
	* Description - DownloadImageFromURL() method used to download image from url.
	* Parameter -  string, string.
	* Exception - 
	* Return -
	*/
	inline void DownloadImageFromURL(string& _url, string& _fileName)
	{
		Logger::Info("Helper::DownloadImageFromURL() Started...");
		CURL* image;
		CURLcode imageResult;
		FILE* fp = nullptr;

		const char* tempurl = _url.c_str();
		image = curl_easy_init();

		if (image)
		{
			struct curl_slist* headers = NULL;
			string authorization = "Authorization: " + Configuration::GetInstance()->GetBearerToken();
			headers = curl_slist_append(headers, authorization.c_str());

			if (FormatHelper::HasContent(Configuration::GetInstance()->GetCsrfNonceToken())) {
				string csrfNonceToken = Configuration::GetInstance()->GetCsrfNonceKey() + ": " + Configuration::GetInstance()->GetCsrfNonceToken();
				headers = curl_slist_append(headers, csrfNonceToken.c_str());
			}

			fp = fopen(_fileName.c_str(), "wb");
			if (fp == NULL)
			{
				throw std::logic_error("File cannot be opened");
			}

			curl_easy_setopt(image, CURLOPT_HTTPHEADER, headers);
			curl_easy_setopt(image, CURLOPT_WRITEFUNCTION, NULL);
			curl_easy_setopt(image, CURLOPT_WRITEDATA, fp);
			curl_easy_setopt(image, CURLOPT_URL, tempurl);
			curl_easy_setopt(image, CURLOPT_SSL_VERIFYPEER, 0);
			curl_easy_setopt(image, CURLOPT_NOPROGRESS, 0);
			curl_easy_setopt(image, CURLOPT_XFERINFOFUNCTION, progress_callback);
			// Grab image
			imageResult = curl_easy_perform(image);

			if (imageResult)
				cout << "Cannot grab the image." + '\n';
		}
		// Clean up the resources
		curl_easy_cleanup(image);
		// Close the file
		fclose(fp);
		Logger::Info("Helper::DownloadImageFromURL() Ended...");

	}

	/*
	* Description - WriteBytes() method used to writes data to file stream.
	* Parameter -  void*, size_t, size_t, QByteArray.
	* Exception - 
	* Return - size_t.
	*/
	inline size_t WriteBytes(void* _contents, size_t _size, size_t _nmemb, QByteArray*_stream)
	{
		_stream->append((char*)_contents, _size * _nmemb);

		return _size * _nmemb;
	}

	/*
	* Description - DownloadImageFromURL() method used to download image from url.
	* Parameter -  string.
	* Exception -
	* Return - QByteArray.
	*/
	inline QByteArray DownloadImageFromURL(string& _url)
	{
		Logger::Info("Utility::DownloadImageFromURL() Started...");
		CURL* image;
		CURLcode imageResult;
		QByteArray bytes;

		const char* tempurl = _url.c_str();
		image = curl_easy_init();

		if (image)
		{
			struct curl_slist* headers = NULL;
			headers = curl_slist_append(headers, "content-type: application/json");
			headers = curl_slist_append(headers, "Accept: application/json");
			headers = curl_slist_append(headers, ("Cookie: "+ Configuration::GetInstance()->GetBearerToken()).c_str());

			/*if (FormatHelper::HasContent(Configuration::GetInstance()->GetCsrfNonceToken())) {
				string csrfNonceToken = Configuration::GetInstance()->GetCsrfNonceKey() + ": " + Configuration::GetInstance()->GetCsrfNonceToken();
				headers = curl_slist_append(headers, csrfNonceToken.c_str());
			}*/
			curl_easy_setopt(image, CURLOPT_CUSTOMREQUEST, "GET");
			curl_easy_setopt(image, CURLOPT_HTTPHEADER, headers);
			curl_easy_setopt(image, CURLOPT_WRITEFUNCTION, WriteBytes);
			curl_easy_setopt(image, CURLOPT_WRITEDATA, &bytes);
			curl_easy_setopt(image, CURLOPT_URL, tempurl);
			curl_easy_setopt(image, CURLOPT_SSL_VERIFYPEER, 0);
			curl_easy_setopt(image, CURLOPT_NOPROGRESS, 0);
			curl_easy_setopt(image, CURLOPT_TIMEOUT, 1800L);
			curl_easy_setopt(image, CURLOPT_XFERINFOFUNCTION, progress_callback);
			// Grab image
			imageResult = curl_easy_perform(image);

			if (imageResult)
				cout << "Cannot grab the image!" + '\n';
		}
		// Clean up the resources
		curl_easy_cleanup(image);
		// Close the file
		Logger::Info("Utility::DownloadImageFromURL() Ended...");

		return bytes;
	}

	/*
	* Description - WriteData() method used to writes data to file stream.
	* Parameter -  void*, size_t, size_t, FILE*.
	* Exception - 
	* Return - size_t.
	*/
	inline size_t WriteData(void* _ptr, size_t _size, size_t _nmemb, FILE* _stream)
	{
		size_t written;
		written = fwrite(_ptr, _size, _nmemb, _stream);
		return written;
	}

	/*
	* Description - DownloadFilesFromURL() method used to downloads file from rest service url.
	* Parameter -  string, string.
	* Exception - 
	* Return -
	*/
	static void DownloadFilesFromURL(string& _url, string& _fileName)
	{
		Logger::Info("Helper::DownloadFilesFromURL() Started...");
		CURL* curl;
		FILE* fp = nullptr;;
		CURLcode res;
		string url1;
		const char* url = _url.c_str();
		curl_version_info_data* vinfo = curl_version_info(CURLVERSION_NOW);
		if (vinfo->features & CURL_VERSION_SSL) {
			printf("CURL: SSL enabled" + '\n');
		}
		else {
			printf("CURL: SSL not enabled\n");
		}
		curl = curl_easy_init();
		if (curl) {
			struct curl_slist* headers = NULL;
			string authorization = "Cookie: " + Configuration::GetInstance()->GetBearerToken();
			headers = curl_slist_append(headers, "Accept: application/json");
			headers = curl_slist_append(headers, "content-type: application/json");
			headers = curl_slist_append(headers, authorization.c_str());
			
			/*if (FormatHelper::HasContent(Configuration::GetInstance()->GetCsrfNonceToken())) {
				string csrfNonceToken = Configuration::GetInstance()->GetCsrfNonceKey() + ": " + Configuration::GetInstance()->GetCsrfNonceToken();
				headers = curl_slist_append(headers, csrfNonceToken.c_str());
			}*/

			fp = fopen(_fileName.c_str(), "wb");
			if (fp == NULL)
			{
				throw std::logic_error("File cannot be opened");
			}
			
			/* Setup the https:// verification options. Note we   */
			/* do this on all requests as there may be a redirect */
			/* from http to https and we still want to verify     */
			curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "GET");
			curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
			curl_easy_setopt(curl, CURLOPT_URL, url);
			curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
			curl_easy_setopt(curl, CURLOPT_DEFAULT_PROTOCOL, "https");
			curl_easy_setopt(curl, CURLOPT_CAINFO, "./ca-bundle.crt");
			curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false);
			curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, false);
			curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteData);
			curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
			curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0);
			curl_easy_setopt(curl, CURLOPT_XFERINFOFUNCTION, progress_callback);
			res = curl_easy_perform(curl);
			curl_easy_cleanup(curl);
			int i = fclose(fp);
		}
		Logger::Info("Helper::DownloadFilesFromURL() Ended...");
	}

	/*
	* Description - RemoveDirectory() method used to Deletes all files and directory of the provided folder.
	* Parameter -  QString.
	* Exception - 
	* Return - bool.
	*/
	inline bool RemoveDirectory(const QString& _dirName)
	{
		bool result = true;
		QDir dir(_dirName);

		if (dir.exists(_dirName)) {
			Q_FOREACH(QFileInfo info, dir.entryInfoList(QDir::NoDotAndDotDot | QDir::System | QDir::Hidden | QDir::AllDirs | QDir::Files, QDir::DirsFirst)) {
				if (info.isDir()) {
					result = RemoveDirectory(info.absoluteFilePath());
				}
				else {
					result = QFile::remove(info.absoluteFilePath());
				}

				if (!result) {
					return result;
				}
			}
			result = dir.rmdir(_dirName);
		}
		return result;
	}

	/*
	* Description - EraseSubString() method used to Search for Sub String in a String and deletes it.
	* Parameter -  string, string.
	* Exception - exception, Char *
	* Return -
	*/
	inline void EraseSubString(std::string& _completeString, const std::string& _toEraseString)
	{
		Logger::Info("Helper::EraseSubString() Started...");
		// Search for the substring in string
		size_t pos = _completeString.find(_toEraseString);

		if (pos != std::string::npos)
		{
			// If found then erase it from string
			_completeString.erase(pos, _toEraseString.length() + 5);
		}
		Logger::Info("Helper::EraseSubString() Ended...");
	}

	/*
	* Description - GetDropDownMap() method used to reads dropdown values from json.
	* Parameter -  json, string, map<string, string>.
	* Exception - exception, Char *
	* Return -
	*/
	inline void GetDropDownMap(const json& _lookUpsJson, const string& _attributeValue, map<string, string>& _attribMap)
	{
		Logger::Info("Helper::GetDropDownMap() Started...");
		map<string, string> tempMap;
		json lookupNameAndIdJson, colNameAndIDJSON;
		for (int i = 0; i < _lookUpsJson.size(); i++) {
			string lookupNameAndId = _lookUpsJson[i].dump();
			try {
				lookupNameAndIdJson = json::parse(lookupNameAndId);
			}
			catch (exception& e)
			{
				throw e;
			}
			catch (const char* e)
			{
				throw e;
			}
			catch (string e)
			{
				throw e;
			}
			string colName = lookupNameAndIdJson["name"].dump();
			colName = colName.erase(0, 1);
			colName = colName.erase(colName.size() - 1);
			if (_attributeValue.compare(colName) == 0)
			{
				string id, name;
				string columnStg = lookupNameAndIdJson["column"].dump();
				try {
					colNameAndIDJSON = json::parse(columnStg);
				}
				catch (exception& e)
				{
					throw e;
				}
				catch (const char* e)
				{
					throw e;
				}
				catch (string e)
				{
					throw e;
				}
				id = colNameAndIDJSON["Id"].dump();
				name = colNameAndIDJSON["Name"].dump();
				name = name.erase(0, 1);
				name = name.erase(name.size() - 1);
				tempMap.insert(std::make_pair(name, id));
			}
		}
		_attribMap = tempMap;

		Logger::Info("Helper::GetDropDownMap() Ended...");
	}

	/*
	* Description - GetJSONValue() method used to reads json object with passed key and trims '"' if required.
	* Parameter -  json, tempPar, bool.
	* Exception -
	* Return - string.
	*/
	template <typename tempPar>
	inline string GetJSONValue(json _json, tempPar _key, bool _trimQuotes)
	{
		//Logger::Info("Helper::GetJSONValue() Started...");
		string value = _json[_key].dump();
		if (_trimQuotes)
		{
			value.erase(std::remove(value.begin(), value.end(), '"'), value.end());
			//Logger::Debug(_key + "::" + value);
		}
		//Logger::Info("Helper::GetJSONValue() Ended...");
		return value;
	}

	/*
	* Description - GetJSONParsedValue() method used to parse in the form of json.
	* Parameter -  json, template, bool.
	* Exception - 
	* Return - json
	*/
	template <typename tempPar>
	inline json GetJSONParsedValue(json _json, tempPar _key, bool _trimQuotes)
	{
		return json::parse(GetJSONValue(_json, _key, _trimQuotes));
	}

	/*
	* Description - CopyFile() method used to copy a file from source to destination.
	* Parameter -  char, char.
	* Exception - 
	* Return - bool
	*/
	inline bool CopyFile(const char* _SRC, const char* _DEST)
	{
		std::ifstream src(_SRC, std::ios::binary);
		std::ofstream dest(_DEST, std::ios::binary);
		dest << src.rdbuf();
		return src && dest;
	}

	/*
	* Description - ReadJSONFile() method used to read json file.
	* Parameter -  string.
	* Exception - 
	* Return - json.
	*/
	inline json ReadJSONFile(string _fileName)
    {
        Logger::Info("Helper::ReadJSONFile() Started...");
        json baseJObject;
        ifstream inFileJSON(_fileName);

        if (inFileJSON.is_open())
        {
            Logger::Info("Helper::ReadJSONFile() File Opened...");
            inFileJSON >> baseJObject;
            Logger::Info("JSON Data :: " + to_string(baseJObject));
            inFileJSON.close();
            return baseJObject;
        }
        else
        {
            Logger::Info("Helper::ReadJSONFile() File can't be opened...");
            exit(1);
        }
    }

	/*
	* Description - WriteJSONFile() method used to writes string to file.
	* Parameter -  string, bool, json.
	* Exception - 
	* Return -
	*/
	inline void WriteJSONFile(string _jsonFilePath, bool _clearContent, json _jsonObject)
	{
		Logger::Info("Helper::WriteJSONFile() Started...");
		std::ofstream fileofs;
		if (_clearContent)
		{
			fileofs.open(_jsonFilePath.c_str(), std::ofstream::out | std::ofstream::trunc);
			fileofs << _jsonObject;
			fileofs.close();
		}
		else
		{
			fileofs.open(_jsonFilePath.c_str(), std::ios_base::out | std::ios_base::app);
			fileofs << _jsonObject;
			fileofs.close();
		}
		Logger::Info("Helper::WriteJSONFile() Ended...");
	}

	/*
	* Description - ConvertJSONArrayQStringList() method used to converts json array to QStringList.
	* Parameter -  json.
	* Exception - 
	* Return - QStringList.
	*/
	inline QStringList ConvertJSONArrayQStringList(json _jsonArray)
	{
		Logger::Info("Helper::ConvertJSONArrayQStringList() Started...");
		QStringList stringList;
		if (_jsonArray.is_array())
		{
			string value;
			for (auto count = 0; count < _jsonArray.size(); count++)
			{
				value = GetJSONValue<int>(_jsonArray, count, true);
				stringList << QString::fromStdString(value);  
			}
		}
		Logger::Info("Helper::ConvertJSONArrayQStringList() Ended...");
		return stringList;
	}

	/*
	* Description - GetDataFromJsonFile() method used to get data from json file.
	* Parameter -  string.
	* Exception - 
	* Return - json.
	*/
	inline json GetDataFromJsonFile(string _fileName )
	{
		
		json readJsonValue;
		try
		{
			string plmColorFilePath = DirectoryUtil::GetPLMPluginDirectory() + _fileName;
			readJsonValue = Helper::ReadJSONFile(plmColorFilePath);			
		}
		catch (errno_t)
		{
			qFatal("Could not read JSON fil.");
			UTILITY_API->DisplayMessageBox( "Could not read JSON file.");
		}
		return readJsonValue;
	}

	/*
	* Description - IsValueTrue() method used to value is true or not.
	* Parameter -  QString, bool, string.
	* Exception - 
	* Return - bool.
	*/
	inline bool IsValueTrue(string _value)
	{
		if (_value == "true")
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	/*
	* Description - StringSplitter() method used to spilt the string.
	* Parameter -  string, string.
	* Exception - 
	* Return - vector<string>.
	*/
	inline vector<string> StringSplitter(string _str, string _delimiter) {
		// Vector of string to save tokens 
		vector <string> tokens;
		size_t pos = 0;
		std::string subStr;
		while ((pos = _str.find(_delimiter)) != std::string::npos) {
			subStr = _str.substr(0, pos);
			tokens.push_back(subStr);
			_str.erase(0, pos + _delimiter.length());
		}
		tokens.push_back(_str);

		return tokens;
	}

	/*
	* Description - EncryptDecrypt() method used to encrypted to dcrypt.
	* Parameter -  string.
	* Exception - 
	* Return -
	*/
	inline void  EncryptDecrypt(string _toEncrypt)
	{
		char key = 'K'; //Any char will work
		string output = _toEncrypt;

		for (int count = 0; count < _toEncrypt.size(); count++)
			output[count] = _toEncrypt[count] ^ key;
		ofstream encryptFile(DirectoryUtil::GetPLMPluginDirectory() + ENCRYPT_DATA_FILE_NAME);
		encryptFile << output + "\n";
	}

	/*
	* Description - EncryptDecrypt() method used to encrypted to decrypt.
	* Parameter -  string, string.
	* Exception - 
	* Return -
	*/
	inline void  EncryptDecrypt(string _user, string _psw)
	{
		char key = 'K'; //Any char will work
		string userName = _user;
		string password = _psw;

		for (int count = 0; count < userName.size(); count++)
			userName[count] = _user[count] ^ key;
		for (int count = 0; count < password.size(); count++)
			password[count] = _psw[count] ^ key;
		ofstream encryptFile(DirectoryUtil::GetPLMPluginDirectory() + ENCRYPT_DATA_FILE_NAME, std::ios::trunc);
		encryptFile << userName + "\n";
		encryptFile << password;
		encryptFile.close();
	}

	/*
	* Description - DecryptEncrypt() method used to decrypt.
	* Parameter -  string.
	* Exception - 
	* Return - string.
	*/
	inline string  DecryptEncrypt(string _toDecrypt)
	{
		char key = 'K'; //Any char will work
		string decrypt = _toDecrypt;

		for (int count = 0; count < decrypt.size(); count++)
			decrypt[count] = _toDecrypt[count] ^ key;

		return decrypt;
	}
	
	/*
	* Description - ValidateFile() method used to create validate file.
	* Parameter -  string.
	* Exception - 
	* Return -
	*/
	inline void ValidateFile(string _filePath)
	{
		QFile templateFile(QString::fromStdString(_filePath));
		if (!templateFile.open(QFile::ReadOnly | QFile::Text))
		{
			throw std::logic_error("Unable to open template");
		}
	}

	/*
	* Description - ValidateZipFile() method used to create validate zip file.
	* Parameter -  string.
	* Exception - 
	* Return - bool.
	*/
	inline bool ValidateZipFile(string _filePath)
	{
		QFile templateFile(QString::fromStdString(_filePath));
		if (!templateFile.open(QFile::ReadOnly | QFile::Text))
		{
			UTILITY_API->DisplayMessageBox("zipfile not valid.");
			return false;
		}
		return true;
	}

	/*
	* Description - GetFileName() method used to get file name from path.
	* Parameter -  string.
	* Exception - 
	* Return - string.
	*/
	inline string GetFileName(string _filePath)
	{
		QFile templateFile(QString::fromStdString(_filePath));
		QFileInfo fileInfo(templateFile);
		string fileName = fileInfo.fileName().toStdString();
		if (!FormatHelper::HasContent(fileName))
			fileName = "";
		return fileName;
	}
	
	/*
	* Description - GetFileContetType() method used to create search fields in search UI.
	* Parameter -  string.
	* Exception - 
	* Return - string.
	*/
	inline string GetFileContetType(string _filePath)
	{
		QFile templateFile(QString::fromStdString(_filePath));
		QFileInfo fileInfo(templateFile);
		std::string extension = fileInfo.suffix().toStdString();
		string contentType = "";
		if (extension.compare("bmp") == 0 || extension.compare("BMP") == 0) {
			contentType = "image/bmp";
		}
		else if (extension.compare("gif") == 0 || extension.compare("GIF") == 0) {
			contentType = "image/gif";
		}
		else if (extension.compare("jpg") == 0 || extension.compare("JPG") == 0) {
			contentType = "image/jpeg";
		}
		else if (extension.compare("jpeg") == 0 || extension.compare("JPEG") == 0) {
			contentType = "image/jpeg";
		}
		else if (extension.compare("png") == 0 || extension.compare("PNG") == 0) {
			contentType = "image/png";
		}
		else if (extension.compare("pdf") == 0) {
			contentType = "application/pdf";
		}
		else if (extension.compare("doc") == 0 || extension.compare("docx") == 0) {
			contentType = "application/msword";
		}
		else if (extension.compare("ppt") == 0 || extension.compare("pptx") == 0) {
			contentType = "application/vnd.ms-powerpoint";
		}
		else if (extension.compare("zip") == 0 || extension.compare("rar") == 0) {
			contentType = "application/zip";
		}
		else if (extension.compare("rtf") == 0) {
			contentType = "application/rtf";
		}
		else if (extension.compare("xls") == 0 || extension.compare("xlsx") == 0) {
			contentType = "application/vnd.ms-excel";
		}
		else {
			contentType = "application/octet-stream";
		}
		return contentType;
	}

	/*
	* Description - GetFilestream() method used to get streaming the file.
	* Parameter -  string.
	* Exception - 
	* Return - string
	*/
	inline string GetFilestream(string _filePath)
	{
		std::stringstream fileStream;
		std::ifstream inFile(_filePath, std::ios_base::binary);
		inFile.seekg(0, std::ios_base::end);
		size_t length = inFile.tellg();
		inFile.seekg(0, std::ios_base::beg);
		std::vector<char> buffer;
		buffer.reserve(length);
		std::copy(std::istreambuf_iterator<char>(inFile), std::istreambuf_iterator<char>(), std::back_inserter(buffer));
		for (size_t i = 0; i < buffer.size(); ++i)
		{
			fileStream << buffer[i];
		}
		string stream = fileStream.str();
		if (!FormatHelper::HasContent(stream))
			stream = "";
		return stream;
	}
	
	/*
	* Description - getFileLength() method used to get file content length.
	* Parameter -  string.
	* Exception - 
	* Return - string.
	*/
	inline string getFileLength(string _filePath)
	{
		string contentLength;
		std::stringstream ss;
		ss << "Content-Length: " << Helper::GetFileSize(_filePath) << "";
		contentLength = ss.str();
		if (!FormatHelper::HasContent(contentLength))
			contentLength = "";
		return contentLength;
	}
	
	/*
	* Description - AddMultiPartFilesToRequest() method used to add multi part files to request.
	* Parameter -  string, string, string, string, string.
	* Exception - 
	* Return - string.
	*/
	inline string AddMultiPartFilesToRequest(string _requestParams, string _fileName, string _contentType, string _streamData, string _fileLength)
	{
		if (FormatHelper::HasContent(_fileName) && FormatHelper::HasContent(_contentType) && FormatHelper::HasContent(_streamData) && FormatHelper::HasContent(_fileLength))
		{
			_requestParams += "------WebKitFormBoundary7MA4YWxkTrZu0gW\r\nContent-Disposition: form-data; name=\"files\"; filename=\"" + _fileName + "\"\r\n";
			_requestParams += "Content-Type: " + _contentType + "\r\n";
			_requestParams += _fileLength + "\r\n\r\n";
			_requestParams += _streamData;
			_requestParams += "\r\n";
		}

		return _requestParams;
	}

	/*
	* Description - AddRquestParam() method used to add request parameter.
	* Parameter -  string, string, string, string, bool.
	* Exception - 
	* Return - string.
	*/
	inline string AddRquestParam(string _fileName, string _contentType, string _streemData, string fileLength,bool addMore)
	{
		string requestParams="";
		if (FormatHelper::HasContent(_fileName) && FormatHelper::HasContent(_contentType) && FormatHelper::HasContent(_streemData) && FormatHelper::HasContent(fileLength))
		{
			if (addMore)
			{
				requestParams = requestParams + "------WebKitFormBoundary7MA4YWxkTrZu0gW\r\nContent-Disposition: form-data; name=\"files\"; filename=\"" + _fileName + "\"\r\n";
			}
			else
			{
				requestParams = "------WebKitFormBoundary7MA4YWxkTrZu0gW\r\nContent-Disposition: form-data; name=\"files\"; filename=\"" + _fileName + "\"\r\n";
			}
			requestParams += "Content-Type: " + _contentType + "\r\n";
			requestParams += fileLength + "\r\n\r\n";
			requestParams += _streemData;
			requestParams += "\r\n";
		}
		return requestParams;
	}

	/*
	* Description - GetFileNameWithOutExtension() method used to remove extension from string.
	* Parameter -  string.
	* Exception - 
	* Return - string.
	*/
	inline string GetFileNameWithOutExtension(string _fileName)
	{
		int last = _fileName.find_last_of('.');
		_fileName = _fileName.substr(0, last);
		return _fileName;
	}

	/*
	* Description - ReplaceDelimeter() method used to replace delimeter.
	* Parameter -  string.
	* Exception - 
	* Return - string.
	*/
	inline string ReplaceDelimeter(string _value, string _delimiter, string _replaceDelimiter)
	{
		size_t pos = _value.find(_delimiter);
		// Repeat till end is reached
		while (pos != string::npos)
		{
			// Replace this occurrence of Sub String
			_value.replace(pos, _delimiter.size(), _replaceDelimiter);
			// Get the next occurrence from the current position
			pos = _value.find(_delimiter, pos + _replaceDelimiter.size());
		}
		return _value;
	}

	/*
	* Description - RemoveAsterisk() method used to remove asterisk.
	* Parameter -  string.
	* Exception - 
	* Return - string
	*/
	inline string  RemoveAsterisk(string& _value)
	{
		std::string removeChar = "*";
		_value.erase(remove_if(_value.begin(), _value.end(), [&removeChar](const char& rChar)
		{
			return removeChar.find(rChar) != std::string::npos;
		}),
			_value.end());
		return _value;
	}

	/*
	* Description - GetValidString() method used to removes special charecters from input string, like to remove special charecters from file name
	* Parameter -  string, string
	* Exception -
	* Return - string
	*/
	static std::string GetValidString(std::string  _inputString, std::string _chars)
	{
		for (char c : _chars) {
			_inputString.erase(std::remove(_inputString.begin(), _inputString.end(), c), _inputString.end());
		}
		return _inputString;
	}

	/*
	* Description - FindAndReplace() method used to replace.
	* Parameter -  string.
	* Exception -
	* Return - string.
	*/
	inline string FindAndReplace(string _value, string _findValue, string _replaceValue)
	{
		size_t pos = _value.find(_findValue);
		// Repeat till end is reached
		while (pos != string::npos)
		{
			// Replace this occurrence of Sub String
			_value.replace(pos, _findValue.size(), _replaceValue);
			// Get the next occurrence from the current position
			pos = _value.find(_findValue, pos + _replaceValue.size());
		}
		return _value;
	}

	/*
	* Description - URLEncode() method used to encode URL.
	* Parameter -  string.
	* Exception -
	* Return - string.
	*/
    inline string URLEncode(string _str)
    {
        string encodedString = "";
        char c;
        int ic;
        const char* chars = _str.c_str();
        char bufHex[10];
        int len = strlen(chars);

        for(int i=0;i<len;i++){
            c = chars[i];
            ic = c;
            // uncomment this if you want to encode spaces with +
            /*if (c==' ') encodedString += '+';
            else */if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~' || c == ':') encodedString += c;
            else {
                sprintf(bufHex,"%X",c);
                if(ic < 16)
                    encodedString += "%0";
                else
                    encodedString += "%";
                encodedString += bufHex;
            }
        }
        return encodedString;
     }

	inline bool CheckFileExist(QString _itemName, CLOAPISample::LibraryAPIItem* setItem)
	{
		bool alreadyExists = false;
		if (API_STORAGE)
		{
			for (int i = 0; i < API_STORAGE->m_LibraryAPIItemList.size(); i++)
			{
				if (API_STORAGE->m_LibraryAPIItemList[i]->itemName == _itemName)
				{
					API_STORAGE->m_LibraryAPIItemList[i] = setItem; //overwrite the same file
					alreadyExists = true;
					break;
				}
			}
		}
		return alreadyExists;
	}

	inline string GetFileExtension(string _fileName)
	{
		int pos = _fileName.find_last_of('.');
		string fileExt = _fileName.substr(pos + 1);
		return fileExt;
	}

	inline json makeRestcallGet(string _api, string _param = "", string _id = "", string _progressbartext = "")
	{
		Logger::Debug("UiHelper drawWidget() _attName == Season");
		Logger::Debug("UiHelper drawWidget() API: " + _api);
		//_api = _api + "&decode=true"; //appending decode=true to get decoded response from centric
		vector<pair<string, string>> headerNameAndValueList;
		headerNameAndValueList.push_back(make_pair("content-Type", "application/json"));
		headerNameAndValueList.push_back(make_pair("Cookie", Configuration::GetInstance()->GetBearerToken()));
		string resultJsonString = REST_API->CallRESTGet(Configuration::GetInstance()->GetPLMServerURL() + _api + _id + _param, headerNameAndValueList, _progressbartext);
		if (!FormatHelper::HasContent(resultJsonString))
		{
			throw "Unable to initiliaze Document Configuration. Please try again or Contact your System Administrator.";
		}

		int length = resultJsonString.length();
		int indexforjson = resultJsonString.find("[");
		string FinalresultJsonString = resultJsonString.substr(indexforjson, length);
		json detailJson = json::parse(FinalresultJsonString);

		return detailJson;
	}

	inline json GetJsonFromResponse(string _response, string _start)
	{
		Logger::Info("UIHelper -> GetJsonFromResponse() -> Start");
		int length = _response.length();
		int indexforjson = _response.find(_start);
		string FinalresultJsonString = _response.substr(indexforjson, length);
		json detailJson = json::parse(FinalresultJsonString);
		Logger::Info("UIHelper -> GetJsonFromResponse() -> End");
		return detailJson;
	}

	inline void GetCentricErrorMessage(string& _response)
	{
		if (QString::fromStdString(_response).contains("message"))
		{
			json errorRsponse = json::parse(_response);
			_response = Helper::GetJSONValue<string>(errorRsponse, "message", true);
		}
	}

	/*
	* Description - GetReflistJson() method used to prepare reference list json.
	* Parameter -  json, json
	* Exception -
	* Return - 
	*/
	inline void GetReflistJson(json _responsejson, json& _attributesFieldsListJson)
	{
		json refList;
		int tempRefCount = 0;
		for (size_t refListResponseJsonCount = 0; refListResponseJsonCount < _responsejson.size(); refListResponseJsonCount++)
		{
			json refListJson = Helper::GetJSONParsedValue(_responsejson, refListResponseJsonCount, false);
			string refValue = Helper::GetJSONValue<string>(refListJson, NODE_NAME_KEY, true);
			if (FormatHelper::HasContent(refValue))
			{
				refList["enumKey"] = Helper::GetJSONValue<string>(refListJson, "id", true);
				refList["enumValue"] = refValue;
				_attributesFieldsListJson[ENUMLIST_JSON_KEY][tempRefCount] = refList;
				tempRefCount = tempRefCount + 1;
			}
		}
	}

	/*
	* Description - GetRefApi() method used to prepare a Api for referenace attribute.
	* Parameter -  json, string
	* Exception -
	* Return - string
	*/

	inline string GetRefApi(json _attributesFieldsListJson, string _attributeKey)
	{
		string refApi = "";
		json refList;
		int tempRefCount = 0;
		string refUrl = Helper::GetJSONValue<string>(_attributesFieldsListJson, "url", true);
		string attName = Helper::GetJSONValue<string>(_attributesFieldsListJson, ATTRIBUTE_NAME_KEY, true);
		if (!FormatHelper::HasContent(refUrl))
		{
			throw "Please add Api for " + attName;
		}
		if (_attributeKey != "collection")
			refApi = Configuration::GetInstance()->GetPLMServerURL() + refUrl + "?skip=0&limit=1000&sort=node_name";
		return refApi;		
	}

	/*
	* Description - GetEnumlistJson()  method used to prepare enum list json.
	* Parameter -  json, json
	* Exception -
	* Return - string
	*/
	inline void GetEnumlistJson(json _enumListResponseJson, json& _attributesFieldsListJson)
	{
		json enumList;
		int tempEnumCount = 0;
		for (int enumListResponseJsonCount = 0; enumListResponseJsonCount < _enumListResponseJson.size(); enumListResponseJsonCount++)
		{
			json enumJson = Helper::GetJSONParsedValue<int>(_enumListResponseJson, enumListResponseJsonCount, false);
			string enumKey = Helper::GetJSONValue<string>(enumJson, ATTRIBUTE_NAME, true);
			string enumValue = Helper::GetJSONValue<string>(enumJson, ATTRIBUTE_DISPLAY_NAME, true);
			if (FormatHelper::HasContent(enumValue))
			{
				enumList["enumKey"] = enumKey;
				enumList["enumValue"] = enumValue;
				_attributesFieldsListJson[ENUMLIST_JSON_KEY][tempEnumCount] = enumList;
				tempEnumCount++;
			}
		}
	}

}
