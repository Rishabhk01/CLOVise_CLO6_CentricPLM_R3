/*
* Copyright 2020-2021 CLO-Vise. All rights reserved
*
* @file CVLicenseHelper.cpp
*
* @brief Class implementation for verify the user license CLO from PLM.
* This class has all the variable and function implementation which are used verify the user license CLO from PLM.
*
* @author GoVise
*
* @date 27-MAY-2020
*/
#include "CVLicenseHelper.h"

#include "QtWidgets"
#include "qhostinfo.h"

#include "CLOVise/PLM/Helper/Util/Logger.h"
#include "CLOVise/PLM/Helper/Util/Helper.h"
#include "CLOVise/PLM/Helper/Util/RestAPIUtility.h"
#include "CLOVise/PLM/Helper/Util/Configuration.h"

CVLicenseHelper* CVLicenseHelper::_instance = NULL;
CVLicenseHelper* CVLicenseHelper::GetInstance()
{
	if (_instance == NULL)
	{
		_instance = new CVLicenseHelper();
	}
	return _instance;
}

void  CVLicenseHelper::Destroy()
{
	if (_instance)
	{
		delete _instance;
		_instance = NULL;
	}
}

/*
* Description - ValidateCVLicense() method used to validate CV license.
* Parameter -  string.
* Exception - 
* Return - bool.
*/
bool CVLicenseHelper::ValidateCVLicense(string _userId)
{
	Logger::Info("CLOPLMSignIn::ValidateLicense() Started...");
	bool loggedIn = false;
	json resJson = ValidateCVUserLicense(Configuration::GetInstance()->GetCLOViseLicenseURL() + RESTAPI::LICENSE_LOGIN_API, _userId);
	int resultCode = resJson["result_code"].get<int>();
	if (resultCode == 0 || resultCode == 3) {
		Logger::Debug(LICENSE_NOT_VALID_MSG);
		if (UTILITY_API != nullptr)
			throw LICENSE_NOT_VALID_MSG.c_str();
	}
	else if (resultCode == 1) {
		loggedIn = true;
	}
	else if (resultCode == 2) {
		Logger::Debug(USER_ALREADY_ACTIVE);
		if (UTILITY_API != nullptr)
			throw USER_ALREADY_ACTIVE.c_str();
	}
	else if (resultCode == 4) {
		Logger::Debug(LICENSE_EXPIRED);
		if (UTILITY_API != nullptr)
			throw LICENSE_EXPIRED.c_str();
	}
	else if (resultCode == 5) {
		Logger::Debug(LICENSE_LIMIT_EXCEEDED);
		if (UTILITY_API != nullptr)
			throw LICENSE_LIMIT_EXCEEDED.c_str();
	}
	else if (resultCode == 101) {
		Logger::Debug(LICENSE_ERROR_CODE_101_MSG);
		if (UTILITY_API != nullptr)
			throw LICENSE_ERROR_CODE_101_MSG.c_str();
	}
	else if (resultCode == 102) {
		Logger::Debug(LICENSE_ERROR_CODE_102_MSG);
		if (UTILITY_API != nullptr)
			throw LICENSE_ERROR_CODE_102_MSG.c_str();
	}
	Logger::Debug("CLOPLMSignIn::ValidateLicense() return:: " + to_string(loggedIn));
	Logger::Info("CLOPLMSignIn::ValidateLicense() Started...");
	return loggedIn;
}

/*
* Description - ValidateCVUserLicense() method used to validate CV(client) user license.
* Parameter -  string, string.
* Exception - exception, Char *
* Return - json.
*/
json CVLicenseHelper::ValidateCVUserLicense(const string& _url, const string& _userId)
{
	Logger::Info("ValidateUserLicense :: License login - START");
	json propJson;
	bool byPassLicense = false;
	std::list<string> searchColumns;
	ifstream filename(DirectoryUtil::GetCLOPLMPluginConfigJson());
	filename >> propJson;
	filename.close();
	string byPassLicenceCheck = BLANK;
	if (propJson.contains("byPassLicenceCheck"))
	{
		byPassLicenceCheck = Helper::GetJSONValue<string>(propJson, "byPassLicenceCheck", false);
		if (FormatHelper::HasContent(byPassLicenceCheck)) {
			byPassLicense = propJson["byPassLicenceCheck"].get<bool>();
		}
	}
	if (byPassLicense) {
		Logger::Info("User license check has been skipped.");
		Logger::Debug("ValidateUserLicense :: return:: " + LICENSE_BYPASS);
		Logger::Info("ValidateUserLicense :: License login - END");
		return json::parse(LICENSE_BYPASS);
	}

	string alertMessage;
	char hostbuffer[256];
	// To retrieve hostname 
	int hostname = gethostname(hostbuffer, sizeof(hostbuffer));

	// To retrieve host information 
    QString qhostName = QHostInfo::localHostName();
    QHostInfo info = QHostInfo::fromName(qhostName);
    QHostAddress IPAddress;
    if (!info.addresses().isEmpty())
    {
        IPAddress = info.addresses().first();
    }

	if (Configuration::GetInstance()->GetCompanyName().empty()) {
		Logger::Info("ValidateUserLicense :: License login - END");
		return json::parse(LICENSE_ERROR_CODE_101);
	}
	else if (Configuration::GetInstance()->GetCompanyApiKey().empty()) {
		Logger::Info("ValidateUserLicense :: License login - END");
		return json::parse(LICENSE_ERROR_CODE_102);
	}

	string basic_auth = "Basic " + Helper::ConvertToBase64(Configuration::GetInstance()->GetCompanyName() + ":" + Configuration::GetInstance()->GetCompanyApiKey());

	vector<pair<string, string>> authCodeHeader;
	authCodeHeader.push_back(make_pair(CONTENTTYPE, FORM_ENCODED_TYPE));
	authCodeHeader.push_back(make_pair(AUTHORIZATION, basic_auth));
	authCodeHeader.push_back(make_pair(LICENSED_USER, CLOVISE));

    string loginRequestParam = "machineName=" + qhostName.toStdString();;
	loginRequestParam += "&privateIp=" + IPAddress.toString().toStdString();
	loginRequestParam += "&companyName=" + Configuration::GetInstance()->GetCompanyName();
	loginRequestParam += "&userId=" + _userId;
	loginRequestParam += "&publicIp=";
	Logger::Debug("License login param : " + loginRequestParam);
	Logger::Debug("License login url : " + _url);
	string response = REST_API->CallRESTPost(_url, &loginRequestParam, authCodeHeader, HTTP_POST);
	Logger::RestAPIDebug("License login response : " + response);
	size_t found1 = response.find(UNAUTHORIZED);
	if (found1 != string::npos)
	{
		return json::parse("{\"result_code\": 0}");
	}

	json resJson;
	int flowerBraceIndex = response.find("{");
	string resultsString = response.substr(flowerBraceIndex);

	try
	{
		resJson = json::parse(resultsString);
	}
	catch (string msg)
	{
		Logger::Error("ValidateUserLicense :: License login exception - " + msg);
		throw msg;
	}
	catch (const char* msg)
	{
		Logger::Error("ValidateUserLicense :: License login exception - " + string(msg));
		throw msg;
	}
	catch (exception& e)
	{
		Logger::Error("ValidateUserLicense :: License login exception - " + string(e.what()));
		throw e;
	}
	Logger::Debug("ValidateUserLicense :: return:: " + to_string(resJson));
	Logger::Info("ValidateUserLicense :: License login - END");

	return resJson;
}

/*
* Description - ValidateCVLicenseLogOut() method used to validate CV(client) license logout.
* Parameter -  string.
* Exception -
* Return - bool.
*/
bool CVLicenseHelper::ValidateCVLicenseLogOut(string _userId)
{
	bool loggedOut = false;
	json resJson = ValidateCVUserLicense(Configuration::GetInstance()->GetCLOViseLicenseURL() + RESTAPI::LICENSE_LOGOUT_API, _userId);
	Logger::RestAPIDebug("resJson:: " + to_string(resJson));
	int resultCode = resJson["result_code"].get<int>();
	Logger::Debug("resultCode:: " + to_string(resultCode));
	if (resultCode == 0) {
		throw LOGOUT_UNSUCCESSFUL_MSG.c_str();
		Logger::Debug(LOGOUT_UNSUCCESSFUL_MSG);
	}
	else if (resultCode == 1) {
		loggedOut = true;
	}
	else if (resultCode == 101) {
		throw LICENSE_ERROR_CODE_101_MSG.c_str();
		Logger::Debug(LICENSE_ERROR_CODE_101_MSG);
	}
	else if (resultCode == 102) {
		throw LICENSE_ERROR_CODE_102_MSG.c_str();
		Logger::Debug(LICENSE_ERROR_CODE_102_MSG);
	}

	return loggedOut;
}