/*
* Copyright 2020-2021 CLO-Vise. All rights reserved
*
* @file Configuration.cpp
*
* @brief Class implementation for configure data in CLO from PLM.
* This class has all the variable and function implementation which are used configure the data as an input.
*
* @author GoVise
*
* @date 27-MAY-2020
*/
#include "Configuration.h"

#include <fstream>

#include "qdir.h"

#include "CLOAPIInterface.h"
#include "CLOVise/PLM/Authentication/CLOPLMSignIn.h"
#include "CLOVise/PLM/Helper/Util/Helper.h"
#include "CLOVise/PLM/Helper/Util/Definitions.h"
#include "CLOVise/PLM/Libraries/json.h"
#include "CLOVise/PLM/Helper/Util/DirectoryUtil.h"
#include "CLOVise/PLM/Helper/Util/Logger.h"

using json = nlohmann::json;

Configuration* Configuration::_instance = NULL;

Configuration* Configuration::GetInstance()
{
	if (_instance == NULL)
	{
		_instance = new Configuration();
	}
	return _instance;
}

Configuration::Configuration()
{
	m_currentScreen = -1;
}
Configuration::~Configuration()
{

}
void  Configuration::Destroy()
{
	if (_instance)
	{
		delete _instance;
		_instance = NULL;
	}
}

/*
* Description - GetPLMServerURL() method used to get plm server url.
* Parameter - 
* Exception -
* Return - string.
*/
string  Configuration::GetPLMServerURL()
{
	return m_plmServerURL;
}

/*
* Description - SetPLMServerURL() method used to set plm server url.
* Parameter -  string.
* Exception - 
* Return -
*/
void Configuration::SetPLMServerURL(string _str)
{
	if (!FormatHelper::HasContent(_str))
		_str = "";
	else
	{
		for (auto size = 0; size < _str.length(); size++)
		{
			size_t pos = _str.rfind("/");
			if (pos + 1 == _str.length())
				_str = _str.substr(0, _str.length() - 1);
		}
	}
	m_plmServerURL = _str;
}

/*
* Description - GetLogFileName() method used to get log file name.
* Parameter - 
* Exception -
* Return - string.
*/
string  Configuration::GetLogFileName()
{
	return m_logFileName;
}

/*
* Description - SetLogFileName() method used to set log file name.
* Parameter -  string.
* Exception -
* Return -
*/
void Configuration::SetLogFileName(string _logFileName)
{
	if (!FormatHelper::HasContent(_logFileName))
		_logFileName = "";
	m_logFileName = _logFileName;
}

/*
* Description - GetPLMVersion() method used to get plm version.
* Parameter - 
* Exception - 
* Return - string.
*/
string  Configuration::GetPLMVersion()
{
	if (!FormatHelper::HasContent(m_plmVersion))
		SetPLMVersion("Not Available");
	return m_plmVersion;
}

/*
* Description - SetPLMVersion() method used to set plm version.
* Parameter -  string.
* Exception - 
* Return -
*/
void Configuration::SetPLMVersion(string _PlmVersion)
{
	if (!FormatHelper::HasContent(_PlmVersion))
		_PlmVersion = "Not Available";
	m_plmVersion = _PlmVersion;
}

/*
* Description - GetCLOViseVersion() method used to get CLOVise version.
* Parameter - 
* Exception - 
* Return - string.
*/
string  Configuration::GetCLOViseVersion()
{
	return m_cloViseVersion;
}

/*
* Description - SetCLOViseVersion() method used to set CLOVise version.
* Parameter -  string.
* Exception - 
* Return -
*/
void Configuration::SetCLOViseVersion(string _cloPlugin)
{
	if (!FormatHelper::HasContent(_cloPlugin))
		_cloPlugin = "";
	m_cloViseVersion = _cloPlugin;
}

/*
* Description - GetConnectionStatus() method used to get connection status.
* Parameter - 
* Exception - 
* Return - string.
*/
string  Configuration::GetConnectionStatus()
{
	return m_connectionStatus;
}

/*
* Description - SetConnectionStatus() method used to set connection status.
* Parameter -  string.
* Exception - 
* Return -
*/
void Configuration::SetConnectionStatus(string _connectionStatus)
{
	m_connectionStatus = _connectionStatus;
}

/*
* Description - GetBearerToken() method used to get bearer token.
* Parameter - 
* Exception - 
* Return - string.
*/
string  Configuration::GetBearerToken()
{
	return m_bearerToken;
}

/*
* Description - CreateTemporaryFolderPath() method used to create temporary folder path.
* Parameter - 
* Exception -
* Return -
*/
void Configuration::CreateTemporaryFolderPath()
{
    QDir dir;
	string temporaryPath = UTILITY_API->GetCLOTemporaryFolderPath();
	MATERIALS_TEMP_DIRECTORY = temporaryPath + "TempMaterials/";
	PRODUCTS_TEMP_DIRECTORY = temporaryPath + "TempProducts/";
	TRIMS_TEMP_DIRECTORY = temporaryPath + "TempTrims/";
	COLORS_TEMP_DIRECTORY = temporaryPath + "TempColors/";
	TURNTABLE_IMAGES_WITH_AVATAR_TEMP_DIRECTORY = temporaryPath + "CLOViseTurntableImages/WithAvatar";
	TURNTABLE_IMAGES_WITHOUT_AVATAR_TEMP_DIRECTORY = temporaryPath + "CLOViseTurntableImages/WithoutAvatar";
	dir.mkpath(QString::fromStdString(MATERIALS_TEMP_DIRECTORY));
	dir.mkpath(QString::fromStdString(PRODUCTS_TEMP_DIRECTORY));
	dir.mkpath(QString::fromStdString(TRIMS_TEMP_DIRECTORY));
    dir.mkpath(QString::fromStdString(COLORS_TEMP_DIRECTORY));	
    dir.mkpath(QString::fromStdString(TURNTABLE_IMAGES_WITH_AVATAR_TEMP_DIRECTORY));
    dir.mkpath(QString::fromStdString(TURNTABLE_IMAGES_WITHOUT_AVATAR_TEMP_DIRECTORY));
}

/*
* Description - SetBearerToken() method used to set Bearer token.
* Parameter -  string.
* Exception - 
* Return -
*/
void Configuration::SetBearerToken(string _bearerToken)
{
	m_bearerToken = _bearerToken;
}

/*
* Description - CachePLMsettingsFromFile() method used to Caches all plm settings values from file.
* Parameter - 
* Exception - exception, Char *
* Return -
*/
void Configuration::CachePLMsettingsFromFile()
{
	Logger::Logger("Configuration::CachePLMsettingsFromFile() Started..");
	Logger::Logger(" Caching PLM settings...");
	string plmSettingsFilePath = DirectoryUtil::GetPLMPluginDirectory() + PLMSETTINGS_FILE_NAME;
	try
	{
		json plmSettingsJSON = Helper::ReadJSONFile(plmSettingsFilePath);
		string selectedPLMServer = Helper::GetJSONValue<string>(plmSettingsJSON, SELECTEDPLMSERVER_JSON_KEY, true);
		Logger::Debug("Configuration::CachePLMsettingsFromFile() selectedPLMServer - " + selectedPLMServer);

		if (selectedPLMServer == SELECT || selectedPLMServer.empty())
		{
			ClearPLMsettingsCache();
		}
		else
		{
			string selectedTenantDetails = Helper::GetJSONValue<string>(plmSettingsJSON, selectedPLMServer, false);
			json selectedTenantDetailsJSON;
			try {
				selectedTenantDetailsJSON = json::parse(selectedTenantDetails);
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
			CachePLMsettings(selectedTenantDetailsJSON);
		}
	}
	catch (string msg)
	{
		Logger::Logger("Configuration::CachePLMsettingsFromFile() exception - " + msg);
		UTILITY_API->DisplayMessageBox(PLMSETTINGS_EXCEPTION_MSG);
	}
	catch (const char* msg)
	{
		Logger::Logger("Configuration::CachePLMsettingsFromFile() exception - " + string(msg));
		UTILITY_API->DisplayMessageBox(PLMSETTINGS_EXCEPTION_MSG);
	}
	catch (exception & e)
	{
		Logger::Logger("Configuration::CachePLMsettingsFromFile() exception - " + string(e.what()));
		UTILITY_API->DisplayMessageBox(PLMSETTINGS_EXCEPTION_MSG);
	}
	Logger::Logger("Caching PLM setting completed.");
	Logger::Logger("Configuration::CachePLMsettingsFromFile() Ended..");
}

/*
* Description - ClearPLMsettingsCache() method used to clear plm settings cache.
* Parameter - 
* Exception - 
* Return -
*/
void Configuration::ClearPLMsettingsCache()
{
	Logger::Logger("Configuration::ClearPLMsettingsCache() Started..");

	Configuration::GetInstance()->SetPLMServerURL(BLANK);
	Configuration::GetInstance()->SetPLMVersion(BLANK);
	Configuration::GetInstance()->SetCLOViseVersion(BLANK);
	Configuration::GetInstance()->SetConnectionStatus(SERVER_NOTCONNECTED_STATUS);

	Logger::Logger("Configuration::ClearPLMsettingsCache() Ended..");
}

/*
* Description - DrawSearchUI() method used to caches all plm settings values.
* Parameter -  json, string.
* Exception - 
* Return -
*/
void Configuration::CachePLMsettings(json _selectedServerDetails)
{
	Logger::Info("Configuration::CachePLMsettings() Started..");
	Logger::Info("selected environment::");
	

	string plmSetting = Helper::GetJSONValue<string>(_selectedServerDetails, PLMSERVERURL_JSON_KEY, true);
	Configuration::GetInstance()->SetPLMServerURL(plmSetting);
	plmSetting = Helper::GetJSONValue<string>(_selectedServerDetails, CLOPLUGINVERSION_JSON_KEY, true);
	Configuration::GetInstance()->SetCLOViseVersion(plmSetting);

	if (Configuration::GetInstance()->GetBearerToken().empty())
	{
		Configuration::GetInstance()->SetConnectionStatus(SERVER_NOTCONNECTED_STATUS);
	}
	else
	{
		Configuration::GetInstance()->SetConnectionStatus(SERVER_CONNECTED_STATUS);
	}

	plmSetting = Helper::GetJSONValue<string>(_selectedServerDetails, COMPANY_NAME, true);
	Configuration::GetInstance()->SetCompanyName(plmSetting);

	plmSetting = Helper::GetJSONValue<string>(_selectedServerDetails, COMPANY_API_KEY, true);
	Configuration::GetInstance()->SetCompanyApiKey(plmSetting);

	plmSetting = Helper::GetJSONValue<string>(_selectedServerDetails, CLOVISE_LICENSE_URL, true);
	Configuration::GetInstance()->SetCLOViseLicenseURL(plmSetting);

	Logger::Info("Configuration::CachePLMsettings() Ended..");
}

/*
* Description - ValidatePLMSettings() method used to validate plm settings and return true or false.
* Parameter - 
* Exception - 
* Return - bool.
*/
bool Configuration::ValidatePLMSettings()
{
	bool validationPassed = false;
	if (!GetPLMServerURL().empty())
	{
		validationPassed = true;
	}
	return validationPassed;
}

/*
* Description - SetPLMPluginConfigJSON() method used to set plm plugin configuration json.
* Parameter - 
* Exception - exception, Char *
* Return -
*/
void Configuration::SetPLMPluginConfigJSON()
{
	Logger::Logger("Configuration::SetPLMPluginConfigJSON() Started..");
	string plmConfigFilePath = DirectoryUtil::GetPLMPluginDirectory() + PLMCONFIG_FILE_NAME;
	try
	{
		m_configJson = Helper::ReadJSONFile(plmConfigFilePath);
	}
	catch (string msg)
	{
		Logger::Logger("Configuration::SetPLMPluginConfigJSON() exception - " + msg);
		UTILITY_API->DisplayMessageBox(PLMSETTINGS_EXCEPTION_MSG);
	}
	catch (exception& e)
	{
		Logger::Logger("Configuration::SetPLMPluginConfigJSON() exception - " + string(e.what()));
		UTILITY_API->DisplayMessageBox(PLMSETTINGS_EXCEPTION_MSG);
	}
	catch (const char* msg)
	{
		Logger::Error("Configuration -> SetPLMPluginCondifgJSON Exception :: " + string(msg));
		UTILITY_API->DisplayMessageBox(PLMSETTINGS_EXCEPTION_MSG);
	}
	Logger::Logger("Configuration::SetPLMPluginConfigJSON() Ended..");
}

/*
* Description - GetPLMPluginConfigJSON() method used to get plm plugin configuration json.
* Parameter -  QString, bool, string.
* Exception - exception, Char *
* Return -
*/
json Configuration::GetPLMPluginConfigJSON()
{
	return m_configJson;
}

/*
* Description - SetResultsPerPage() method used to set results per page.
* Parameter -  string.
* Exception - exception, Char *
* Return -
*/
void Configuration::SetResultsPerPage(string _key)
{
	Logger::Logger("Configuration::SetResultsPerPage() Started..");
	json array = json::array();
	m_resultsPerPageOptions.clear();
	try
	{
		array = Helper::GetJSONParsedValue<string>(GetPLMPluginConfigJSON(), _key, false);
		for (int i = 0; i < array.size(); i++)
		{
			m_resultsPerPageOptions.push_back(QString::fromStdString(Helper::GetJSONValue<int>(array, i, true)));
		}
	}
	catch (string msg)
	{
		Logger::Logger("Configuration::SetResultsPerPage() exception - " + msg);
		UTILITY_API->DisplayMessageBox(PLMSETTINGS_EXCEPTION_MSG);
	}
	catch (exception& e)
	{
		Logger::Logger("Configuration::SetResultsPerPage() exception - " + string(e.what()));
		UTILITY_API->DisplayMessageBox(PLMSETTINGS_EXCEPTION_MSG);
	}
	catch (const char* msg)
	{
		UTILITY_API->DisplayMessageBox(PLMSETTINGS_EXCEPTION_MSG);
	}
	Logger::Logger("Configuration::SetResultsPerPage() Ended..");
}

/*
* Description - GetResultsPerPage() method used to get results per page.
* Parameter - 
* Exception - 
* Return - QStringList.
*/
QStringList Configuration::GetResultsPerPage()
{
	if (m_resultsPerPageOptions.empty())
		SetResultsPerPage(RESULTS_PER_PAGE_KEY);
	return m_resultsPerPageOptions;
}

/*
* Description - InitializeCLOViseData() method used to initialize clovise data.
* Parameter - 
* Exception - exception, Char *
* Return -
*/
void Configuration::InitializeCLOViseData()
{
	Logger::Logger("Configuration::InitializeCLOViseData() Started..");
	try
	{
		SetPLMPluginConfigJSON();
		SetResultsPerPage(RESULTS_PER_PAGE_KEY);
		SetCloseResultsDialogue(RESULTS_CLOSE_KEY);
		SetProgressBarCloseTimer(PROGRESSBAR_CLOSE_TIMER);
		SetSelectedResultsPerPage("", false, true);
		SetMaxLogFileSize();
		SetLoggerEnabled();
		SetDebugEnabled();
		SetErrorEnabled();
		SetInfoEnabled();
		SetPerfomanceEnabled();
		SetRestAPILogEnabled();
		SetMaximumLimitForRefAttValue();
	}
	catch (string msg)
	{
		Logger::Logger("Configuration::InitializeCLOViseData() exception - " + msg);
		UTILITY_API->DisplayMessageBox(PLMSETTINGS_EXCEPTION_MSG);
	}
	catch (exception& e)
	{
		Logger::Logger("Configuration::InitializeCLOViseData() exception - " + string(e.what()));
		UTILITY_API->DisplayMessageBox(PLMSETTINGS_EXCEPTION_MSG);
	}
	catch (const char* msg)
	{
		UTILITY_API->DisplayMessageBox(PLMSETTINGS_EXCEPTION_MSG);
	}
	Logger::Logger("Configuration::InitializeCLOViseData() successfully...");
}

/*
* Description - SetCloseResultsDialogue() method used to set close results dialogue before download or after download.
* Parameter -  string.
* Exception - exception, Char *
* Return -
*/
void Configuration::SetCloseResultsDialogue(string _key)
{
	Logger::Logger("Configuration::SetCloseResultsDialogue() Started..");
	try
	{
		string temp = Helper::GetJSONValue<string>(GetPLMPluginConfigJSON(), _key, true);
		if (temp == "true")
		{
			m_resultsClose = true;
		}
		else
		{
			m_resultsClose = false;
		}
	}
	catch (string msg)
	{
		Logger::Logger("Configuration::SetCloseResultsDialogue() exception - " + msg);
		UTILITY_API->DisplayMessageBox(PLMSETTINGS_EXCEPTION_MSG);
	}
	catch (exception& e)
	{
		Logger::Logger("Configuration::SetCloseResultsDialogue() exception - " + string(e.what()));
		UTILITY_API->DisplayMessageBox(PLMSETTINGS_EXCEPTION_MSG);
	}
	catch (const char* msg)
	{
		UTILITY_API->DisplayMessageBox(PLMSETTINGS_EXCEPTION_MSG);
	}
	Logger::Logger("Configuration::SetCloseResultsDialogue() Ended..");
}

/*
* Description - GetCloseResultsDialogue() method used to get close results dialogue before download or after download.
* Parameter - 
* Exception - exception, Char *
* Return - bool.
*/
bool Configuration::GetCloseResultsDialogue()
{
	return m_resultsClose;
}

/*
* Description - SetUserJson() method used to set user json.
* Parameter -  json.
* Exception - 
* Return -
*/
void Configuration::SetUserJson(json _userJson)
{
	m_UserJson = _userJson;
}

/*
* Description - SetProgressBarCloseTimer() method used to set progress bar close timer.
* Parameter -  string.
* Exception - exception, Char *
* Return -
*/
void Configuration::SetProgressBarCloseTimer(string _key)
{
	Logger::Info("Configuration::SetProgressBarCloseTimer() Started..");
	try
	{
		m_timer = Helper::GetJSONValue<string>(GetPLMPluginConfigJSON(), _key, true);
	}
	catch (string msg)
	{
		Logger::Error("Configuration::SetProgressBarCloseTimer() exception - " + msg);
		UTILITY_API->DisplayMessageBox(PLMSETTINGS_EXCEPTION_MSG);
	}
	catch (exception& e)
	{
		Logger::Error("Configuration::SetProgressBarCloseTimer() exception - " + string(e.what()));
		UTILITY_API->DisplayMessageBox(PLMSETTINGS_EXCEPTION_MSG);
	}
	catch (const char* msg)
	{
		UTILITY_API->DisplayMessageBox(PLMSETTINGS_EXCEPTION_MSG);
	}
	Logger::Info("Configuration::SetProgressBarCloseTimer() Ended..");
}

/*
* Description - GetProgressBarTimer() method used to get progress bar timer.
* Parameter - 
* Exception - 
* Return - int.
*/
int Configuration::GetProgressBarTimer()
{
	int timer = QString::fromStdString(m_timer).toInt();
	return timer;
}

/*
* Description - SetSelectedResultsPerPage() method used to set selected results per page.
* Parameter -  string, bool, bool.
* Exception - exception, Char *
* Return -
*/
void Configuration::SetSelectedResultsPerPage(string _selected, bool _store, bool _setCached)
{
	Logger::Info("PLMSettingsSample::SetSelectedResultsPerPage() Started..");
	
	json configJson = json::object();
	try
	{
		if (_setCached)
		{
			string resultsPerPage = Helper::GetJSONValue<string>(GetPLMPluginConfigJSON(), SELECTED_RESULTS_PER_PAGE_KEY, true);
			m_resultsPerPage = resultsPerPage;
		}
		if (FormatHelper::HasContent(_selected))
		{
			m_resultsPerPage = _selected;
			if (_store)
			{
				string configFilePath = DirectoryUtil::GetPLMPluginDirectory() + PLMCONFIG_FILE_NAME;
				configJson = Helper::ReadJSONFile(configFilePath);
				configJson[SELECTED_RESULTS_PER_PAGE_KEY] = _selected;
				Helper::WriteJSONFile(configFilePath, true, configJson);
			}
		}
	}
	catch (string msg)
	{
		Logger::Error("Configuration::SetSelectedResultsPerPage() exception - " + msg);
		UTILITY_API->DisplayMessageBox("Unable to Cache Results per page.");
	}
	catch (exception& e)
	{
		Logger::Error("Configuration::SetSelectedResultsPerPage() exception - " + string(e.what()));
		UTILITY_API->DisplayMessageBox("Unable to Cache Results per page.");
	}
	catch (const char* msg)
	{
		UTILITY_API->DisplayMessageBox("Unable to Cache Results per page.");
	}
	Logger::Info("Configuration::SetSelectedResultsPerPage() Ended..");
}

/*
* Description - GetSelectedResultsPerPage() method used to get selected results per page.
* Parameter - 
* Exception - 
* Return - string.
*/
string Configuration::GetSelectedResultsPerPage()
{
	return m_resultsPerPage;
}

/*
* Description - SetSupportedAttsList() method used to set supported attribute list.
* Parameter -  json.
* Exception - exception, Char *
* Return -
*/
void Configuration::SetSupportedAttsList(json _configJson)
{
	Logger::Info("Configuration -> SetSupportedAttsList() start");
	json attTypesArray = json::array();
	m_supportedAttTypes.clear();
	try
	{
		string attTypesStr = "";
		attTypesArray = Helper::GetJSONParsedValue<string>(_configJson, SUPPORTED_ATTYPE_JSON_KEY, false);
		string attType = "";
		for (int i = 0; i < attTypesArray.size(); i++)
		{
			attType = Helper::GetJSONValue<int>(attTypesArray, i, true);
			m_supportedAttTypes.push_back(QString::fromStdString(attType));
		}
	}
	catch (exception e)
	{
		Logger::Error("Configuration -> SetSupportedAttsList Exception :: " + string(e.what()));
		throw e;
	}
	catch (const char* msg)
	{
		Logger::Error("Configuration -> SetSupportedAttsList Exception :: " + string(msg));
		throw msg;
	}
	Logger::Info("Configuration -> SetSupportedAttsList() end");
}

/*
* Description - GetSupportedAttsList() method used to get supported attribute list.
* Parameter - 
* Exception - 
* Return - GetSupportedAttsList.
*/
QStringList Configuration::GetSupportedAttsList()
{
	return m_supportedAttTypes;
}

/*
* Description - SetModifySupportedAttsList() method used to set supported attribute list for create or update of objects in PLM.
* Parameter -  json
* Exception - exception, Char *
* Return -
*/
void Configuration::SetModifySupportedAttsList(json _configJson)
{
	json attTypesArray = json::array();
	m_modifySupportedAttTypes.clear();
	try
	{
		string attTypesStr = "";
		attTypesArray = Helper::GetJSONParsedValue<string>(_configJson, MODIFY_SUPPORTED_ATTYPE_JSON_KEY, false);
		string attType = "";
		for (int i = 0; i < attTypesArray.size(); i++)
		{
			attType = Helper::GetJSONValue<int>(attTypesArray, i, true);
			m_modifySupportedAttTypes.push_back(QString::fromStdString(attType));
		}
	}
	catch (exception e)
	{
		Logger::Error("Configuration -> SetModifySupportedAttsList Exception :: " + string(e.what()));
		throw e;
	}
	catch (const char* msg)
	{
		Logger::Error("Configuration -> SetModifySupportedAttsList Exception :: " + string(msg));
		throw msg;
	}
}

/*
* Description - GetModifySupportedAttsList() method used to get supported attribute list for create or update of objects in PLM.
* Parameter -
* Exception -
* Return - GetSupportedAttsList.
*/
QStringList Configuration::GetModifySupportedAttsList()
{
	return m_modifySupportedAttTypes;
}

/*
* Description - SetResultsSupportedAttsList() method used to set results supported attribute list.
* Parameter -  json.
* Exception - exception, Char *
* Return -
*/
void Configuration::SetResultsSupportedAttsList(json _configJson)
{
	Logger::Info("Configuration -> SetResultsSupportedAttsList() start");
	json attTypesArray = json::array();
	m_resultsSupportedAttTypes.clear();
	try
	{
		string attTypesStr = "";
		attTypesArray = Helper::GetJSONParsedValue<string>(_configJson, RESULTS_SUPPORTED_ATTYPE_JSON_KEY, false);
		string attType = "";
		for (int i = 0; i < attTypesArray.size(); i++)
		{
			attType = Helper::GetJSONValue<int>(attTypesArray, i, true);
			m_resultsSupportedAttTypes.push_back(QString::fromStdString(attType));
		}
	}
	catch (exception e)
	{
		Logger::Error("Configuration -> SetResultsSupportedAttsList Exception :: " + string(e.what()));
		throw e;
	}
	catch (const char* msg)
	{
		Logger::Error("Configuration -> SetResultsSupportedAttsList Exception :: " + string(msg));
		throw msg;
	}
	Logger::Info("Configuration -> SetResultsSupportedAttsList() end");
}

/*
* Description - GetResultsSupportedAttsList() method used to get results supported attribute list.
* Parameter - 
* Exception - 
* Return - QStringList.
*/
QStringList Configuration::GetResultsSupportedAttsList()
{
	return m_resultsSupportedAttTypes;
}

/*
* Description - GetMaxLogFileSize() method used to get maximum log file size.
* Parameter - 
* Exception -
* Return - int.
*/
int  Configuration::GetMaxLogFileSize()
{
	return m_maxLogFileSize;
}

/*
* Description - SetMaxLogFileSize() method used to set maximum log file size.
* Parameter - 
* Exception - 
* Return -
*/
void Configuration::SetMaxLogFileSize()
{
	json plmConfigJson = GetPLMPluginConfigJSON();
	string logFileSize = Helper::GetJSONValue<string>(plmConfigJson, MAX_LOGFILE_SIZE, true);
	Logger::Debug("Configuration::InitializeCLOViseData() logFileSize - " + logFileSize);
	int maxFileSize = std::stoi(logFileSize);
	m_maxLogFileSize = maxFileSize;
}

/*
* Description - SetLoggerEnabled() method used to set logger enabled or not.
* Parameter - 
* Exception - 
* Return -
*/
void Configuration::SetLoggerEnabled()
{
	string enabled = Helper::GetJSONValue<string>(GetPLMPluginConfigJSON(), LOGGER_ENABLED_KEY, true);
	if ("true" == enabled)
		m_LoggerEnabled = true;
	else
		m_LoggerEnabled = false;
}

/*
* Description - GetLoggerEnabled() method used to get logger enabled or not.
* Parameter - 
* Exception - 
* Return - bool.
*/
bool Configuration::GetLoggerEnabled()
{
	return m_LoggerEnabled;
}

/*
* Description - SetDebugEnabled() method used to set debug enabled or not.
* Parameter - 
* Exception - 
* Return -
*/
void Configuration::SetDebugEnabled()
{
	string enabled = Helper::GetJSONValue<string>(GetPLMPluginConfigJSON(), DEBUG_LOGGER_ENABLED_KEY, true);
	if ("true" == enabled)
		m_debugLoggEnabled = true;
	else
		m_debugLoggEnabled = false;
}

/*
* Description - GetDebugEnabled() method used to get debug enabled or not.
* Parameter - 
* Exception - 
* Return - bool.
*/
bool Configuration::GetDebugEnabled()
{
	return m_debugLoggEnabled;
}

/*
* Description - SetErrorEnabled() method used to set error enabled or not.
* Parameter - 
* Exception - 
* Return -
*/
void Configuration::SetErrorEnabled()
{
	string enabled = Helper::GetJSONValue<string>(GetPLMPluginConfigJSON(), ERROR_LOGGER_ENABLED_KEY, true);
	if ("true" == enabled)
		m_errorLoggEnabled = true;
	else
		m_errorLoggEnabled = false;
}

/*
* Description - GetErrorEnabled() method used to get error enabled or not.
* Parameter - 
* Exception - 
* Return - bool.
*/
bool Configuration::GetErrorEnabled()
{
	return m_errorLoggEnabled;
}

/*
* Description - SetInfoEnabled() method used to set info enabled or not.
* Parameter - 
* Exception - 
* Return -
*/
void Configuration::SetInfoEnabled()
{
	string enabled = Helper::GetJSONValue<string>(GetPLMPluginConfigJSON(), INFO_LOGGER_ENABLED_KEY, true);
	if ("true" == enabled)
		m_infoLoggEnabled = true;
	else
		m_infoLoggEnabled = false;
}

/*
* Description - GetInfoEnabled() method used to get info enabled or not.
* Parameter - 
* Exception - 
* Return - bool.
*/
bool Configuration::GetInfoEnabled()
{
	return m_infoLoggEnabled;
}

/*
* Description - SetPerfomanceEnabled() method used to set info enabled or not.
* Parameter - 
* Exception - 
* Return -
*/
void Configuration::SetPerfomanceEnabled()
{
	string enabled = Helper::GetJSONValue<string>(GetPLMPluginConfigJSON(), PERFOMANCE_LOGGER_ENABLED_KEY, true);
	if ("true" == enabled)
		m_perfomanceEnabled = true;
	else
		m_perfomanceEnabled = false;
}

/*
* Description - GetPerfomanceEnabled() method used to get info enabled or not.
* Parameter - 
* Exception - 
* Return - bool.
*/
bool Configuration::GetPerfomanceEnabled()
{
	return m_perfomanceEnabled;
}

/*
* Description - GetCLOViseLicenseURL() method used to get clovise license url.
* Parameter - 
* Exception - 
* Return - string.
*/
string Configuration::GetCLOViseLicenseURL()
{
	return m_CLOViseLicenseURL;
}

/*
* Description - SetCLOViseLicenseURL() method used to set clovise license url.
* Parameter -  string.
* Exception - 
* Return -
*/
void Configuration::SetCLOViseLicenseURL(string _url)
{
	m_CLOViseLicenseURL = _url;
}

/*
* Description - DrawSearchUI() method used to get company api key.
* Parameter - 
* Exception - 
* Return - string.
*/
string Configuration::GetCompanyApiKey()
{
	return m_companyApiKey;
}

/*
* Description - SetCompanyApiKey() method used to set company api key.
* Parameter -  string.
* Exception -
* Return -
*/
void Configuration::SetCompanyApiKey(string _companyApiKey)
{
	if (!FormatHelper::HasContent(_companyApiKey))
		_companyApiKey = "";
	m_companyApiKey = _companyApiKey;
}

/*
* Description - GetCompanyName() method used to get company name.
* Parameter - 
* Exception - 
* Return - string.
*/
string Configuration::GetCompanyName()
{
	return m_companyName;
}

/*
* Description - SetCompanyName() method used to set company name.
* Parameter -  string.
* Exception - 
* Return -
*/
void Configuration::SetCompanyName(string _companyName)
{
	if (!FormatHelper::HasContent(_companyName))
		_companyName = "";
	m_companyName = _companyName;
}

/*
* Description - GetCsrfNonceToken() method used to get csrf nonce token.
* Parameter - 
* Exception -
* Return - string.
*/
string Configuration::GetCsrfNonceToken()
{
	return m_csrfNonceToken;
}

/*
* Description - SetCsrfNonceToken() method used to set csrf nonce token.
* Parameter -  string.
* Exception - 
* Return -
*/
void Configuration::SetCsrfNonceToken(string _csrfNonceToken)
{
	m_csrfNonceToken = _csrfNonceToken;
}

/*
* Description - GetCsrfNonceKey() method used to get Csrf nonce key.
* Parameter -
* Exception - 
* Return - string
*/
string Configuration::GetCsrfNonceKey()
{
	return m_csrfNonceKey;
}

/*
* Description - SetCsrfNonceKey() method used to set Csrf nonce Key.
* Parameter -  string.
* Exception - 
* Return -
*/
void Configuration::SetCsrfNonceKey(string _csrfNonceKey)
{
	m_csrfNonceKey = _csrfNonceKey;
}

/*
* Description - GetPLMSignin() method used to get plm signin.
* Parameter - 
* Exception - 
* Return - bool.
*/
bool Configuration::GetPLMSignin()
{
	return m_isSignin;
}

/*
* Description - SetPLMSignin() method used to set plm signin.
* Parameter -  bool.
* Exception - 
* Return -
*/
void Configuration::SetPLMSignin(bool _isSignin)
{
	m_isSignin = _isSignin;
}

int Configuration::GetCurrentScreen()
{
	return m_currentScreen;
}

void Configuration::SetCurrentScreen(int _screen)
{
	m_currentScreen = _screen;
}


/*
* Description - GetExcludedPreviewFields() method used to get excluded preview fields list.
* Parameter -
* Exception -
* Return - QStringList.
*/
QStringList Configuration::GetExcludedPreviewFields()
{
	if (m_excludedPreviewFields.empty())
		SetExcludedPreviewFields(EXCLUDED_PREVIEW_FEILDS_KEY);
	return m_excludedPreviewFields;
}

/*
* Description - SetExcludedPreviewFields() method used to set excluded preview fields list.
* Parameter -  string.
* Exception - exception, Char *
* Return -
*/
void Configuration::SetExcludedPreviewFields(string _key)
{
	json array = json::array();
	m_excludedPreviewFields.clear();
	try
	{
		array = Helper::GetJSONParsedValue<string>(GetPLMPluginConfigJSON(), _key, false);
		for (int i = 0; i < array.size(); i++)
		{
			m_excludedPreviewFields.push_back(QString::fromStdString(Helper::GetJSONValue<int>(array, i, true)));
		}
	}
	catch (string msg)
	{
		UTILITY_API->DisplayMessageBox(PLMSETTINGS_EXCEPTION_MSG);
	}
	catch (exception& e)
	{
		UTILITY_API->DisplayMessageBox(PLMSETTINGS_EXCEPTION_MSG);
	}
	catch (const char* msg)
	{
		UTILITY_API->DisplayMessageBox(PLMSETTINGS_EXCEPTION_MSG);
	}
}

/*
* Description - SetInputUserName() method sets user entered user name
* Parameter -
* Exception -
* Return -
*/
void Configuration::SetInputUserName(string _name)
{
	m_inputUserName = _name;
}

/*
* Description - GetInputUserName() method used to get user entered user name
* Parameter -
* Exception -
* Return - string
*/
string Configuration::GetInputUserName()
{
	return m_inputUserName;
}
/*
* Description - SetInputUserPassword() method used to set user entered user password
* Parameter -
* Exception -
* Return -
*/
void Configuration::SetInputUserPassword(string _pass)
{
	m_inputUserPassword = _pass;
}

/*
* Description - GetInputUserPassword() method used to get user entered user password
* Parameter -
* Exception -
* Return - string
*/
string Configuration::GetInputUserPassword()
{
	return m_inputUserPassword;
}
/*
* Description - SetIsReadSavedCredential() method used to set wether read saved user credentials
* Parameter -
* Exception -
* Return -
*/
void Configuration::SetIsReadSavedCredential(bool _value)
{
	m_readSavedCredential = _value;
}

/*
* Description - GetIsReadSavedCredential() method used to get read saved user credentials
* Parameter -
* Exception -
* Return - bool
*/
bool Configuration::GetIsReadSavedCredential()
{
	return m_readSavedCredential;
}


/*
* Description - SetProgressBarProgress() method used to set and remember progress bar count
* Parameter -
* Exception -
* Return -
*/
void Configuration::SetProgressBarProgress(int _count)
{
	m_progress = _count;
}

/*
* Description - GetProgressBarProgress() method used to get remembered progress bar count
* Parameter -
* Exception -
* Return -
*/
int Configuration::GetProgressBarProgress()
{
	return m_progress;
}


/*
* Description - SetLoggedInUserName() method used to set loggedin user name.
* Parameter -  json
* Exception -
* Return -
*/
void Configuration::SetLoggedInUserName(json _userInfoJson)
{
	try
	{
		string firstName = Helper::GetJSONValue<string>(_userInfoJson, "first_name", true);
		if (!FormatHelper::HasContent(firstName))
			firstName = BLANK;
		string LastNnme = Helper::GetJSONValue<string>(_userInfoJson, "last_name", true);
		if (!FormatHelper::HasContent(LastNnme))
			LastNnme = BLANK;
		m_UserFullName = firstName + " " + LastNnme;
	}
	catch (string msg)
	{
		UTILITY_API->DeleteProgressBar(true);
		Logger::Error("Configuration::SetLoggedInUserName Exception - " + msg);
		
	}
	catch (exception & e)
	{
		UTILITY_API->DeleteProgressBar(true);
		Logger::Error("Configuration::SetLoggedInUserName Exception - " + string(e.what()));
		
	}
	catch (const char* msg)
	{
		UTILITY_API->DeleteProgressBar(true);
		wstring wstr(msg, msg + strlen(msg));
		Logger::Error("Configuration::SetLoggedInUserName Exception - " + string(msg));
		
	}
}

/*
* Description - GetLoggedInUserName() method used to get loggedin user name.
* Parameter -
* Exception -
* Return - json
*/
string Configuration::GetLoggedInUserName()
{
	return m_UserFullName;
}


/*
* Description - GetUserJson() method used to get User details Json.
* Parameter -
* Exception -
* Return - json
*/
json Configuration::GetUserJson()
{
	return m_UserJson;
}

/*
* Description - SetRestAPILogEnabled() method used to set info enabled or not.
* Parameter -
* Exception -
* Return -
*/
void Configuration::SetRestAPILogEnabled()
{
	try
	{
		string enabled = Helper::GetJSONValue<string>(GetPLMPluginConfigJSON(), REST_LOGGER_ENABLED_KEY, true);
		if ("true" == enabled)
			m_RestAPILogEnabled = true;
		else
			m_RestAPILogEnabled = false;
	}
	catch (string msg)
	{
		UTILITY_API->DeleteProgressBar(true);
		Logger::Error("Configuration::SetRestAPILogEnabled Exception - " + msg);

	}
	catch (exception & e)
	{
		UTILITY_API->DeleteProgressBar(true);
		Logger::Error("Configuration::SetRestAPILogEnabled Exception - " + string(e.what()));

	}
	catch (const char* msg)
	{
		UTILITY_API->DeleteProgressBar(true);
		wstring wstr(msg, msg + strlen(msg));
		Logger::Error("Configuration::SetRestAPILogEnabled Exception - " + string(msg));

	}
}

/*
* Description - GetRestAPILogEnabled() method used to get info enabled or not.
* Parameter -
* Exception -
* Return - bool
*/
bool Configuration::GetRestAPILogEnabled()
{
	return m_RestAPILogEnabled;
}

/* Description - GetColorwayImageLabels() method used to get colorway image labels list.
* Parameter -
* Exception -
* Return - QStringList.
*/
std::map<QString,QString> Configuration::GetColorwayImageLabels()
{
	return m_colorwayImageLabelsMap;
}

/*
* Description - SetColorwayImageLabels() method used to set colorway image labels.
* Parameter -  QStringList
* Exception -
* Return - void
*/
void Configuration::SetColorwayImageLabels(std::map<QString, QString> _colorwayImageLabelsMap)
{
	m_colorwayImageLabelsMap = _colorwayImageLabelsMap;
	Logger::Debug("Configuration::SetColorwayImageLabels ....m_colorwayImageLabelsMap" + to_string(m_colorwayImageLabelsMap.size()));
}

/*
* Description - GetStyleImageLabels() method used to get style image labels list.
* Parameter -
* Exception -
* Return - QStringList.
*/
std::map<QString,QString> Configuration::GetStyleImageLabels()
{
	return m_styleImageLabelsMap;
}

/*
* Description - SetStyleImageLabels() method used to set style image labels.
* Parameter -  QStringList
* Exception -
* Return - void
*/
void Configuration::SetStyleImageLabels(std::map<QString, QString> _styleImageLabelsMap)
{
	m_styleImageLabelsMap = _styleImageLabelsMap;
	Logger::Debug("Configuration::SetStyleImageLabels ....m_styleImageLabelsMap" + to_string(m_styleImageLabelsMap.size()));
}

/*
* Description - SetMaximumLimitForRefAttValue() method used to set the maximum limit for ref type attribute values.
* Parameter -
* Exception -
* Return -
*/
void Configuration::SetMaximumLimitForRefAttValue()
{
	try
	{
		m_maxRefAttValueLimit = Helper::GetJSONValue<string>(GetPLMPluginConfigJSON(), MAX_REF_ATT_SEARCH_LIMIT, true);
	}
	catch (string msg)
	{
		UTILITY_API->DeleteProgressBar(true);
		Logger::Error("Configuration::SetMaximumLimitForRefAttValue Exception - " + msg);

	}
	catch (exception & e)
	{
		UTILITY_API->DeleteProgressBar(true);
		Logger::Error("Configuration::SetMaximumLimitForRefAttValue Exception - " + string(e.what()));

	}
	catch (const char* msg)
	{
		UTILITY_API->DeleteProgressBar(true);
		wstring wstr(msg, msg + strlen(msg));
		Logger::Error("Configuration::SetMaximumLimitForRefAttValue Exception - " + string(msg));

	}
}

/*
* Description - GetMaximumLimitForRefAttValue() method used to get the maximum limit for ref type attribute values.
* Parameter -
* Exception -
* Return - string
*/
string Configuration::GetMaximumLimitForRefAttValue()
{
	return m_maxRefAttValueLimit;
}


bool Configuration::GetIsUpdateColorClicked()
{
	return m_UpdateColorClicked;
}

 void Configuration::SetIsUpdateColorClicked(bool _isClicked)
{
	 m_UpdateColorClicked = _isClicked;
}

 bool Configuration::GetIsPrintSearchClicked()
 {
	 return m_isPrintSearchClicked;
 }

 void Configuration::SetIsPrintSearchClicked(bool _isClicked)
 {
	 m_isPrintSearchClicked = _isClicked;
 }

 bool Configuration::GetIsNewStyleDownloaded()
 {
	 return m_StyleDownloaded;
 }

 void Configuration::SetIsNewStyleDownloaded(bool _isClicked)
 {
	 m_StyleDownloaded = _isClicked;
 }

 string Configuration::GetLocalizedStyleClassName()
 {
	 return m_styleClassName;
 }

 void Configuration::SetLocalizedStyleClassName(string _styleClassName)
 {
	 m_styleClassName = _styleClassName;
 }

 string Configuration::GetLocalizedMaterialClassName()
 {
	 return m_materialClassName;
 }

 void Configuration::SetLocalizedMaterialClassName(string _materialClassName)
 {
	 m_materialClassName = _materialClassName;
 }

 string Configuration::GetLocalizedColorClassName()
 {
	 return m_colorClassName;
 }

 void Configuration::SetLocalizedColorClassName(string _colorClassName)
 {
	 m_colorClassName = _colorClassName;
 }
 bool Configuration::GetNotFirstLogin()
 {
	 return m_notFirstLogin;
 }
 void Configuration::SetNotFirstLogin(bool _login)
 {
	 m_notFirstLogin = _login;
 }
 void Configuration::SetClientSpecificJson(json _clientSpecificJson)
 {
	 m_clientSpecificJson = _clientSpecificJson;
 }
 json Configuration::GetClientSpecificJson()
 {
	 return m_clientSpecificJson;
 }
 void Configuration::SetClientSpecificAttCachedData(json _clientSpecificAttCachedData)
 {
	 m_clientSpecificAttCachedData = _clientSpecificAttCachedData;
 }
 json Configuration::GetClientSpecificAttCachedData()
 {
	 return m_clientSpecificAttCachedData;
 }
 void Configuration::SetSubTypeInternalName(json _clientSpecificJson)
 {
	 string categoryInternalName = Helper::GetJSONValue<string>(_clientSpecificJson, "categoryInternalName", true);
	 if (FormatHelper::HasContent(categoryInternalName)) {
		 m_categoryInternalName = categoryInternalName;
	 }
	 else {
		 m_categoryInternalName = "cus_material_sub_type";
	 }
 }
 string Configuration::GetSubTypeInternalName()
 {
	 return m_categoryInternalName;
 }

 string Configuration::GetQueryParameterForMaterial()
 {
	 return m_queryParamsForMaterial;
 }
 void Configuration::SetQueryParameterForMaterial(string _queryParamsForMaterial)
 {
	 m_queryParamsForMaterial = _queryParamsForMaterial;
 }