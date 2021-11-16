#pragma once
/*
* Copyright 2020-2021 CLO-Vise. All rights reserved
*
* @file RestAPIUtility.h
*
* @brief Class contains all necessary functions connecting to PLM servers through rest service.
*
* @author GoVise
*
* @date 27-MAY-2020
*/ 
#include <string>

#include <QString>

#include "CLOVise/PLM/Helper/Util/Helper.h"
#include "CLOVise/PLM/Libraries/json.h"
#include "CLOVise/PLM/Helper/Util/Definitions.h"
#include "CLOVise/PLM/Helper/Util/DataUtility.h"
#include "CLOVise/PLM/Helper/Util/Logger.h"

using namespace std;
using QtJson::JsonObject;
using json = nlohmann::json;

namespace RESTAPI
{
	//plm authorization endpoint
	const string AUTHORIZATION_URL = "/csi-requesthandler/api/v2/session";
	const string CSRF_TOKEN_URL = "/servlet/rest/security/csrf";
	const string USER_DETAILS_URL = "/servlet/rest/clovise/rs/flexplm/userdetails";
	const string PLM_DETAILS_URL = "/servlet/rest/clovise/rs/flexplm/plmversion";

	//season and palettes endpoints
	const string PALETTE_SEARCH_API = "/servlet/rest/clovise/rs/flexplm/search/seasonpalettes";

	/* color module endpoints starts */
	const string COLOR_SEARCH_RESULTS_API = "/servlet/rest/clovise/rs/flexplm/search/color/results";
	const string COLOR_ATTACHMENTS_RESULTS_API = "/servlet/rest/clovise/rs/flexplm/search/color/attachments";
	const string COLOR_VIEW_API = "/servlet/rest/clovise/rs/flexplm/search/color/viewlists";
	const string COLOR_FILTER_API = "/servlet/rest/clovise/rs/flexplm/search/color/filterlists";
	const string COLOR_SEARCH_FIELDS_API = "/servlet/rest/clovise/rs/flexplm/search/color/fields";
	/* color module endpoints ends */

	/* material module endpoints starts */
	const string SEARCH_RESULTS_API = "/servlet/rest/clovise/rs/flexplm/search/results";
	//const string ATTACHMENTS_RESULTS_API = "/servlet/rest/clovise/rs/flexplm/search/attachments";
	const string VIEW_API = "/servlet/rest/clovise/rs/flexplm/search/viewlists";
	const string FILTER_API = "/servlet/rest/clovise/rs/flexplm/search/filterlists";
	const string SEARCH_FIELDS_API = "/servlet/rest/clovise/rs/flexplm/search/fields";
	const string CREATEORUPDATE_FIELDS_API = "/servlet/rest/clovise/rs/flexplm/createorupdate/fields";
	/* material module endpoints ends */
	
	/*Centric rest calls start	*/
	const string SEARCH_ATT_API = "/csi-requesthandler/api/v2/business_objects?";
	const string SEARCH_ENUM_ATT_API = "/csi-requesthandler/api/v2/enum_lists";
	const string SEARCH_ENUM_LOCAL_ATT_API = "/csi-requesthandler/api/v2/localization/enum_lists";
	const string SEARCH_ATT_DEFINITION_API = "/csi-requesthandler/api/v2/create_form_definitions/";
	const string SEARCH_ATT_DEFINITION_ADDITIONAL_API = "/configurable_attributes?skip=0&limit=1000&decode=true";
	const string SEASON_SEARCH_API = "/csi-requesthandler/api/v2/seasons";
	const string SHAPE_API = "/csi-requesthandler/api/v2/shapes";
	const string THEME_API = "/csi-requesthandler/api/v2/themes";
	
	const string ATTACHMENTS_RESULTS_API = "/csi-requesthandler/api/v2/documents/";
	const string ATTACHMENTS_LATEST_REVISION_RESULTS_API = "/csi-requesthandler/api/v2/documents/parent/";
	const string IMAGE_API = "/csi-requesthandler/api/v2/images/parent";
	const string ATTACHMENTS_VERSION_API = "/csi-requesthandler/api/v2/document_revisions/";
	const string ATTACHMENTS_DOWNLOAD_API = "/csi-requesthandler/RequestHandler?";
	const string USER_DETAIL_API = "/csi-requesthandler/api/v2/users?";
	const string USER_SESSION_DETAIL_API = "/csi-requesthandler/api/v2/session";
	
	/*Centric material rest calls start	*/
	const string MATERIAL_SECURITY_GROUP_SEARCH_API = "/csi-requesthandler/api/v2/material_security_groups?";
	const string MATERIAL_LIBRARIES_API = "/csi-requesthandler/api/v2/lib_materials?";
	const string COLOR_LIBRARIES_API = "/csi-requesthandler/api/v2/lib_color_specifications?";
	const string SEARCH_MATERIAL_API = "/csi-requesthandler/api/v2/materials";
	const string MATERIAL_TYPE_SEARCH_API = "/csi-requesthandler/api/v2/material_types";
	const string MATERIAL_SEARCH_API_LIB = "/csi-requesthandler/api/v2/lib_materials";
	const string COLOR_SEARCH_API_LIB = "/csi-requesthandler/api/v2/lib_color_specifications";
	const string PLM_VERSION = "/csi-requesthandler/api/v2/version";
	const string LOCALIZATION_API = "/csi-requesthandler/api/v2/localization/classes/";
	/*Centric material rest calls end	*/
	
	/*Centric style rest calls start	*/
	const string COLLECTION_SEARCH_API = "/csi-requesthandler/api/v2/collections";
	const string SEARCH_STYLE_API = "/csi-requesthandler/api/v2/style";
	const string IMAGE_LABEL_ID_API = "/csi-requesthandler/api/v2/business_objects?skip=0&limit=1000&decoded=true&";
	const string IMAGE_LABELS_ENUMLIST_API = "/csi-requesthandler/api/v2/enum_lists";
	//const string MATERIAL_TYPE_SEARCH_API = "/csi-requesthandler/api/v2/material_types";
	/*Centric material rest calls end	*/
	
	/*Centric color specification rest calls start	*/
	const string SEARCH_COLOR_API = "/csi-requesthandler/api/v2/color_specifications";
	const string CREATE_COLOR_API = "/csi-requesthandler/api/v2/color_specifications";
	/*Centric color specification rest calls end	*/

	/*Centric Print Design Color rest calls start	*/
	const string SEARCH_PRINT_DESIGN_API = "/csi-requesthandler/api/v2/print_designs";
	const string SEARCH_PRINT_DESIGN_COLOR_API = "/csi-requesthandler/api/v2/print_design_colors";
	/*Centric Print Design Color rest calls end	*/

	/*Centric rest calls end	*/

	/*publish module endpoints */
	//const string UPLOAD_API = "/servlet/rest/clovise/rs/flexplm/3dmodelandvisuals/publish";

	/*clovise license module endpoints */
	const string LICENSE_LOGIN_API = "/plugin/govise/login";
	const string LICENSE_LOGOUT_API = "/plugin/govise/logout";
	
	const string UPLOAD_API = "/servlet/rest/clovise/rs/flexplm/3dmodelandvisuals/publish";
	const string STYLE_DETAIL_API = "/csi-requesthandler/api/v2/create_form_definitions";
	//const string SEASON_SEARCH_API = "/csi-requesthandler/api/v2/seasons";
	const string STYLE_TYPE_API = "/csi-requesthandler/api/v2/style_types";
	const string STYLE_THEME_API = "/csi-requesthandler/api/v2/themes?sort=node_name";
	const string STYLE_SHAPE_API = "/csi-requesthandler/api/v2/shapes?sort=node_name";
	const string ENUM_LIST_API = "/csi-requesthandler/api/v2/enum_lists/";
	const string LOCALIZATION_LIST_API = "/csi-requesthandler/api/v2/localization/enum_lists";
	const string DEPARTMENT_DETAIL_API = "/csi-requesthandler/api/v2/category1s";
	const string COLLECTION_DETAIL_API = "/csi-requesthandler/api/v2/category2s";
	const string CREATE_STYLE_API = "/csi-requesthandler/api/v2/collections";
	const string STYLE_ENDPOINT_API = "/csi-requesthandler/api/v2/styles";
	const string DOCUMENT_UPLOAD_API = "/csi-requesthandler/api/v2/document_revisions";
	const string DOCUMENT_CREATE_API = "/csi-requesthandler/api/v2/documents/parent";
	const string UPLOAD_IMAGE_API = "/csi-requesthandler/api/v2/images";
	const string COLORWAY_API = "/csi-requesthandler/api/v2/colorways";
	const string CREATE_STYLE_COPY_API = "/csi-requesthandler/api/v2/styles/copy";
	const string CREATE_MATERIAL_API = "/csi-requesthandler/api/v2/materials";
	const string STYLE_COPY_OPTION_API = "/csi-requesthandler/api/v2/style_copy_options";
	const string UPDATE_STYLE_API = "/csi-requesthandler/api/v2/styles";
	const string COLOR_SPEC_API = "/csi-requesthandler/api/v2/color_specifications";

	/*
	* Description - SetProgressBarData() method used to set data, shown in progress bar.
	* Parameter -  float, string, bool.
	* Exception -
	* Return -
	*/
	static void SetProgressBarData(float _count, string _message, bool _flag)
	{		
		if (UTILITY_API == nullptr)
		{
			return;
		}
		DataUtility::GetInstance()->SetProgressBarMsg(_message);
		DataUtility::GetInstance()->SetProgressCounter(_count);
		UTILITY_API->SetProgress(_message, _count);

		if (_flag)
		{
			UTILITY_API->CreateProgressBar();
		}
		else
		{
			UTILITY_API->DeleteProgressBar(true);
		}
	}

	/*
	* Description - SetProgressBarData() method used to set data, shown in progress bar.
	* Parameter -  float, string, bool.
	* Exception -
	* Return -
	*/
	static int SetProgressBarProgress(int _count, int _increase, string _message)
	{
		if ((_count + _increase) < 100)
			_count = _count + _increase;
		UTILITY_API->SetProgress(_message, _count);

		return _count;
	}

	/*
	* Description - WriteCallback() method used to write result data.
	* Parameter -  float, string, bool.
	* Exception -
	* Return - size_t.
	*/
	static size_t WriteCallback(void* _contents, size_t _size, size_t _nmemb, string* _userp) {
		_userp->append((char*)_contents, _size * _nmemb);
		return _size * _nmemb;
	}

	/*
	* Description - progress_callback() method used to monitor rest call progress.
	* Parameter -  void*, curl_off_t, curl_off_t, curl_off_t.
	* Exception -
	* Return - int.
	*/
	static int progress_callback(void* _clientp, curl_off_t _dltotal, curl_off_t _dlnow, curl_off_t _ultotal, curl_off_t _ulnow) {

		UTILITY_API->SetProgress(DataUtility::GetInstance()->GetProgressBarMsg(), DataUtility::GetInstance()->GetProgressCounter());
		float count = DataUtility::GetInstance()->GetProgressCounter();
		if (count < 95 && (((double)_dltotal) != ((double)_dlnow) || ((double)_ultotal) != ((double)_ulnow))) {
			count = count + 0.05;
			DataUtility::GetInstance()->SetProgressCounter(count);
		}

		return 0;
	}

	/*
	* Description - DownloadFilesFromURL() method used to downloads file from rest service url.
	* Parameter -  string, string.
	* Exception -
	* Return -
	*/
	static string CentricRestCallGet(string _url, string _contentType, string _parameter)
	{
		Logger::Info("RestAPI::CentricRestCallGet() Started...");
		//_url = _url + "&decode=true"; //appending decode=true to get decoded response from centric
		string response;
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
			headers = curl_slist_append(headers, authorization.c_str());
			string accept = "Accept: application/json";
			headers = curl_slist_append(headers, accept.c_str());
			if (_contentType == APPLICATION_JSON_TYPE)
			{
				string contentTypee = CONTENTTYPE + ": " + APPLICATION_JSON_TYPE + "; " + CHARSET_UTF8;
				headers = curl_slist_append(headers, contentTypee.c_str());
			}
			else if (!_contentType.empty()) 
			{
				string contentTypee = CONTENTTYPE + ": " + _contentType;
				headers = curl_slist_append(headers, contentTypee.c_str());
			}

			if (FormatHelper::HasContent(Configuration::GetInstance()->GetCsrfNonceToken())) {
				string csrfNonceToken = Configuration::GetInstance()->GetCsrfNonceKey() + ": " + Configuration::GetInstance()->GetCsrfNonceToken();
				headers = curl_slist_append(headers, csrfNonceToken.c_str());
			}
			//UTILITY_API->DisplayMessageBox("urlbefor encode::" + _url);
			if (FormatHelper::HasContent(_parameter))
            {
				//	Commented below code because it is not working in MAC
//              char* singleEncode = curl_easy_escape(curl, _parameter.c_str(), _parameter.length());
//              if (singleEncode)
//                  curl_free(singleEncode);
//              string singleConvert = singleEncode;
//              //UTILITY_API->DisplayMessageBox("single encode::" + singleConvert);
//              char* doubleEncode = curl_easy_escape(curl, singleConvert.c_str(), singleConvert.length());
//				if (doubleEncode)
//					curl_free(doubleEncode);
//              string doubleConvert = doubleEncode;

				//	Added below code which is compatible with MAC
                string encodedString = Helper::URLEncode(_parameter);
                string doubleConvert = Helper::URLEncode(encodedString);

//              UTILITY_API->DisplayMessageBox("encodedString::" + doubleConvert);
                doubleConvert = Helper::FindAndReplace(doubleConvert, "%253D", "=");
                doubleConvert = Helper::FindAndReplace(doubleConvert, "%2526", "&");
//              UTILITY_API->DisplayMessageBox("after str::" + doubleConvert);
                _url = _url + doubleConvert;
            }
            
            Logger::RestAPIDebug("Get_URL:: "+ _url);
			/* Setup the https:// verification options. Note we   */
			/* do this on all requests as there may be a redirect */
			/* from http to https and we still want to verify     */
			curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
			curl_easy_setopt(curl, CURLOPT_URL, _url.c_str());
			curl_easy_setopt(curl, CURLOPT_CAINFO, "./ca-bundle.crt");
			curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false);
			curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, false); 
			curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
			curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
			curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0);
			curl_easy_setopt(curl, CURLOPT_XFERINFOFUNCTION, progress_callback);
			res = curl_easy_perform(curl);
			
			if (res != CURLE_OK) {
				stringstream errorMsg;
				errorMsg << "Unable to process your request, PLM Server is down or unavailable. Please contact system administrator";
				Logger::Error(errorMsg.str());
				response = errorMsg.str();
			}
			else if (QString::fromStdString(response).contains(QString::fromStdString(SERVER_DOWN_MSG))) {
				response = SERVER_DOWN_RESPOSE;
			}
		}
		curl_easy_cleanup(curl);
		Logger::Debug("Response..." + response);
		Logger::Info("RestAPI::CentricRestCallGet() Ended...");
		if (FormatHelper::HasError(response))
		{
			RESTAPI::SetProgressBarData(0, "", false);
			throw runtime_error(response);
		}
		return response;
	}
	/*
	* Description - RestCall() method used to make a Rest call method.
	* Parameter -  string, string, string.
	* Exception -
	* Return - int.
	*/
	static string RestCall(string _parameter, string _api, string _contentType)
	{
		Logger::Debug("API----"+ _api +"....parameter----"+ _parameter);
		CURL* RESTAPI;
		CURLcode res_code;
		string* data;
		string config;
		string response;

		RESTAPI = curl_easy_init();

		if (RESTAPI)
		{
			string authorization = "Cookie: " + Configuration::GetInstance()->GetBearerToken();
			string url = Configuration::GetInstance()->GetPLMServerURL() + _api;

			struct curl_slist* headers = NULL;
			if (_contentType == APPLICATION_JSON_TYPE) {
				string contentTypee = CONTENTTYPE + ": " + APPLICATION_JSON_TYPE + "; " + CHARSET_UTF8;
				headers = curl_slist_append(headers, contentTypee.c_str());
			}
			else if (!_contentType.empty()) {
				string contentTypee = CONTENTTYPE + ": " + _contentType;
				headers = curl_slist_append(headers, contentTypee.c_str());
			}

			if (FormatHelper::HasContent(Configuration::GetInstance()->GetCsrfNonceToken())) {
				string csrfNonceToken = Configuration::GetInstance()->GetCsrfNonceKey() + ": " + Configuration::GetInstance()->GetCsrfNonceToken();
				headers = curl_slist_append(headers, csrfNonceToken.c_str());
			}

			headers = curl_slist_append(headers, authorization.c_str());
			
			curl_easy_setopt(RESTAPI, CURLOPT_CUSTOMREQUEST, "GET");
			curl_easy_setopt(RESTAPI, CURLOPT_HTTPHEADER, headers);
			curl_easy_setopt(RESTAPI, CURLOPT_URL, url.c_str());
			curl_easy_setopt(RESTAPI, CURLOPT_CAINFO, "./ca-bundle.crt");
			curl_easy_setopt(RESTAPI, CURLOPT_SSL_VERIFYPEER, false);
			curl_easy_setopt(RESTAPI, CURLOPT_SSL_VERIFYHOST, false);
			curl_easy_setopt(RESTAPI, CURLOPT_POSTFIELDS, _parameter.c_str());
			curl_easy_setopt(RESTAPI, CURLOPT_POSTFIELDSIZE, _parameter.size());
			curl_easy_setopt(RESTAPI, CURLOPT_VERBOSE, 1L);
			curl_easy_setopt(RESTAPI, CURLOPT_TIMEOUT, 600L);

			curl_easy_setopt(RESTAPI, CURLOPT_WRITEFUNCTION, WriteCallback);
			curl_easy_setopt(RESTAPI, CURLOPT_WRITEDATA, &response);
			curl_easy_setopt(RESTAPI, CURLOPT_NOPROGRESS, 0);
			curl_easy_setopt(RESTAPI, CURLOPT_XFERINFOFUNCTION, progress_callback);

			res_code = curl_easy_perform(RESTAPI);

			if (res_code != CURLE_OK) {
				stringstream errorMsg;
				errorMsg << "Unable to process your request, PLM Server is down or unavailable. Please contact system administrator";
				Logger::Error(errorMsg.str());
				response = errorMsg.str();
			}
			else if (QString::fromStdString(response).contains(QString::fromStdString(SERVER_DOWN_MSG))) {
				response = SERVER_DOWN_RESPOSE;
			}
		}
		// Clean up the resources
		curl_easy_cleanup(RESTAPI);
		Logger::RestAPIDebug("Response..."+ response);
		if (FormatHelper::HasError(response))
		{
			RESTAPI::SetProgressBarData(0, "", false);
			throw runtime_error(response);
		}
		return response;
	}

	/*
	* Description - AddToRestParamMap() method used to add to rest parameter map.
	* Parameter -  json, string, string.
	* Exception -
	* Return - json.
	*/
	inline json AddToRestParamMap(json _param, string _key, string _value)
	{
		Logger::Info("RestAPIUtility -> AddToRestParamMap start");
		_param[_key] = _value;
		Logger::Info("RestAPIUtility -> AddToRestParamMap end");

		return _param;
	}

	/*
	* Description - RESTMethodSubmit() method used to  submit the rest method.
	* Parameter -  json, string.
	* Exception -
	* Return - string.
	*/
	static string RESTMethodSubmit(string _endPoint, json _parameter) {
		return RestCall(to_string(_parameter), _endPoint, APPLICATION_JSON_TYPE);
	}

	/*
	* Description - ProcessesResponse() method used to processes response.
	* Parameter -  string.
	* Exception -
	* Return - string.
	*/
	inline string ProcessesResponse(string _response) {
		size_t found = _response.find("HTTP/1.1 200");
		string strForJSON = "";
		if (found != string::npos)
		{
			int indexForJSON = _response.find("{");
			strForJSON = _response.substr(indexForJSON);
		}
		else {
			string error = "Something went wrong please try again";
			throw error;
			UTILITY_API->DisplayMessageBox(error);
		}
		return strForJSON;
	}

	/*
	* Description - CheckForErrorMsg() method used to check the error message.
	* Parameter -  string.
	* Exception -
	* Return - string.
	*/
	inline string CheckForErrorMsg(string _response)
	{
		json responseJson = json::parse(_response);
		string message = "";
		string status = "";
		if (responseJson.contains("statusCode"))
			status = Helper::GetJSONValue<string>(responseJson, "statusCode", true);
		if (status == "500")
		{
			if (responseJson.contains("message"))
			{
				message = Helper::GetJSONValue<string>(responseJson, "message", true);
			}
			if (!FormatHelper::HasContent(message))
				message = SERVER_DOWN_RESPOSE;
		}

		return message;
	}

	/*
	* Description - CheckForRestErrorMsg() method used to check the error message.
	* Parameter -  string.
	* Exception -
	* Return - char.
	*/
	inline const char* CheckForRestErrorMsg(string _response)
	{
		const char* error = "";
		string errorJsonString = "";
		json errorJson = json::object();

		if (_response.find("HTTP/1.1 503") != string::npos || _response.find("HTTP/1.1 502") != string::npos)
		{
			error = "Service Unavailable. Please try after sometime.";
		}
		else if (_response.find("HTTP/1.1 500") != string::npos)
		{
			error = "Something went wrong. Please try again or Contact your System Administrator";
		}
		else if (_response.find("HTTP/1.1 404") != string::npos)
		{
			error = "Endpoint not found";
		}
		else if (_response.find("HTTP/1.1 405") != string::npos)
		{
			error = "Method Not Allowed";
		}
		else if (_response.find("HTTP/1.1 401") != string::npos)
		{
			error = "Unable to Login. Please verify Login Credentials.";
		}
		else if (_response.find("HTTP/1.1 403") != string::npos)
		{
			error = "Forbidden request";
		}
		return error;
	}
	static string PutRestCall(string _data, string _api, string _contentType)
	{
		CURLcode res;
		string response;

		CURL *hnd = curl_easy_init();
		struct curl_slist *headers = NULL;
		string token = "cookie: " + Configuration::GetInstance()->GetBearerToken();
		headers = curl_slist_append(headers, "cache-control: no-cache");
		headers = curl_slist_append(headers, "accept: application/json");
		headers = curl_slist_append(headers, token.c_str());
		headers = curl_slist_append(headers, _contentType.c_str());


		curl_easy_setopt(hnd, CURLOPT_CUSTOMREQUEST, "PUT");
		curl_easy_setopt(hnd, CURLOPT_URL, _api.c_str());
		curl_easy_setopt(hnd, CURLOPT_SSL_VERIFYPEER, false);
		curl_easy_setopt(hnd, CURLOPT_SSL_VERIFYHOST, false);
		curl_easy_setopt(hnd, CURLOPT_WRITEFUNCTION, WriteCallback);
		curl_easy_setopt(hnd, CURLOPT_WRITEDATA, &response);
		curl_easy_setopt(hnd, CURLOPT_HTTPHEADER, headers);
		curl_easy_setopt(hnd, CURLOPT_COOKIE, "SecurityTokenURL=centric://_CS_SecurityToken/453113d8-5441-414e-b9fe-9c18c403c8ac");
		curl_easy_setopt(hnd, CURLOPT_POSTFIELDS, _data.c_str());
		curl_easy_setopt(hnd, CURLOPT_POSTFIELDSIZE, _data.size());
		curl_easy_setopt(hnd, CURLOPT_NOPROGRESS, 0);
		curl_easy_setopt(hnd, CURLOPT_XFERINFOFUNCTION, progress_callback);
		CURLcode ret = curl_easy_perform(hnd);
		curl_easy_cleanup(hnd);
		//UTILITY_API->DisplayMessageBox(response);
		if (ret != CURLE_OK) {
			stringstream errorMsg;
			errorMsg << "Unable to process your request, PLM Server is down or unavailable. Please contact system administrator";
			Logger::Error(errorMsg.str());
		}

		if (FormatHelper::HasError(response))
		{
			RESTAPI::SetProgressBarData(0, "", false);
			throw runtime_error(response);
		}
		return response;
	}

	static string PostRestCall(string _parameter, string _api, string _contentType)
	{
		CURLcode res;
		string response;
		
		CURL *hnd = curl_easy_init();
		string token = "cookie: " + Configuration::GetInstance()->GetBearerToken();
		curl_easy_setopt(hnd, CURLOPT_CUSTOMREQUEST, "POST");
		curl_easy_setopt(hnd, CURLOPT_URL, _api.c_str());
		struct curl_slist *headers = NULL;
		//string postField = "------WebKitFormBoundary7MA4YWxkTrZu0gW\r\nContent-Disposition: form-data; name=\"file\"; filename=\"image.png\"\r\nContent-Type: image/png\r\n" + contentLength + "\r\n\r\n" + fileStream.str() + "\r\n------WebKitFormBoundary7MA4YWxkTrZu0gW--";
		//int postFieldSize = postField.size();
		//headers = curl_slist_append(headers, "postman-token: 7da5b7f0-0458-1947-17f7-1eb112a2dd0b");
		headers = curl_slist_append(headers, "cache-control: no-cache");
		headers = curl_slist_append(headers, "accept: application/json");
		headers = curl_slist_append(headers, token.c_str());
		headers = curl_slist_append(headers, _contentType.c_str());
		curl_easy_setopt(hnd, CURLOPT_SSL_VERIFYPEER, false);
		curl_easy_setopt(hnd, CURLOPT_SSL_VERIFYHOST, false);
		curl_easy_setopt(hnd, CURLOPT_WRITEFUNCTION, WriteCallback);
		curl_easy_setopt(hnd, CURLOPT_WRITEDATA, &response);
		curl_easy_setopt(hnd, CURLOPT_HTTPHEADER, headers);
		curl_easy_setopt(hnd, CURLOPT_COOKIE, "SecurityTokenURL=centric://_CS_SecurityToken/453113d8-5441-414e-b9fe-9c18c403c8ac");
		curl_easy_setopt(hnd, CURLOPT_POSTFIELDS, _parameter.c_str());
		curl_easy_setopt(hnd, CURLOPT_POSTFIELDSIZE, _parameter.size());
		curl_easy_setopt(hnd, CURLOPT_NOPROGRESS, 0);
		curl_easy_setopt(hnd, CURLOPT_XFERINFOFUNCTION, progress_callback);

		//	curl_easy_setopt(hnd, CURLOPT_POSTFIELDS, "------WebKitFormBoundary7MA4YWxkTrZu0gW\r\nContent-Disposition: form-data; name=\"file\"; filename=\"image.png\"\r\nContent-Type: image/png\r\n\r\n" + fileStream.str() +"\r\n------WebKitFormBoundary7MA4YWxkTrZu0gW--");
		CURLcode ret = curl_easy_perform(hnd);
		curl_easy_cleanup(hnd);
		//UTILITY_API->DisplayMessageBox(to_string(ret));
		//UTILITY_API->DisplayMessageBox(response);
		if (ret != CURLE_OK) {
			stringstream errorMsg;
			errorMsg << "Unable to process your request, PLM Server is down or unavailable. Please contact system administrator";
			Logger::Error(errorMsg.str());
			//response = errorMsg.str();
		}
		if (FormatHelper::HasError(response))
		{
			RESTAPI::SetProgressBarData(0, "", false);
			throw runtime_error(response);
		}
		return response;
	}

	static string DeleteRestCall(string _data, string _api, string _contentType)
	{
		CURLcode res;
		string response;

		CURL *hnd = curl_easy_init();
		struct curl_slist *headers = NULL;
		string token = "cookie: " + Configuration::GetInstance()->GetBearerToken();
		headers = curl_slist_append(headers, "cache-control: no-cache");
		headers = curl_slist_append(headers, "accept: application/json");
		headers = curl_slist_append(headers, token.c_str());
		headers = curl_slist_append(headers, _contentType.c_str());


		curl_easy_setopt(hnd, CURLOPT_CUSTOMREQUEST, "DELETE");
		curl_easy_setopt(hnd, CURLOPT_URL, _api.c_str());
		curl_easy_setopt(hnd, CURLOPT_SSL_VERIFYPEER, false);
		curl_easy_setopt(hnd, CURLOPT_SSL_VERIFYHOST, false);
		curl_easy_setopt(hnd, CURLOPT_WRITEFUNCTION, WriteCallback);
		curl_easy_setopt(hnd, CURLOPT_WRITEDATA, &response);
		curl_easy_setopt(hnd, CURLOPT_HTTPHEADER, headers);
		curl_easy_setopt(hnd, CURLOPT_COOKIE, "SecurityTokenURL=centric://_CS_SecurityToken/453113d8-5441-414e-b9fe-9c18c403c8ac");
		curl_easy_setopt(hnd, CURLOPT_POSTFIELDS, _data.c_str());
		curl_easy_setopt(hnd, CURLOPT_POSTFIELDSIZE, _data.size());
		curl_easy_setopt(hnd, CURLOPT_NOPROGRESS, 0);
		curl_easy_setopt(hnd, CURLOPT_XFERINFOFUNCTION, progress_callback);
		CURLcode ret = curl_easy_perform(hnd);
		curl_easy_cleanup(hnd);
		//UTILITY_API->DisplayMessageBox(response);
		if (ret != CURLE_OK) {
			stringstream errorMsg;
			errorMsg << "Unable to process your request, PLM Server is down or unavailable. Please contact system administrator";
			Logger::Error(errorMsg.str());
		}
		if (FormatHelper::HasError(response))
		{
			RESTAPI::SetProgressBarData(0, "", false);
			throw runtime_error(response);
		}
		return response;
	}

}
