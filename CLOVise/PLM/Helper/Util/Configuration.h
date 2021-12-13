#pragma once
/*
* Copyright 2020-2021 CLO-Vise. All rights reserved
*
* @file Configuration.h
*
* @brief Class declaration for configure data in CLO from PLM.
* This class has all the variable and function declaration which are used configure the data as an input.
*
* @author GoVise
*
* @date 27-MAY-2020
*/
#include <string>

#include "qstringlist.h"

#include "CLOVise/PLM/Libraries/json.h"

using json = nlohmann::json;
using namespace std;

class Configuration
{
public:
	string MATERIALS_TEMP_DIRECTORY;
	string PRODUCTS_TEMP_DIRECTORY;
	string TRIMS_TEMP_DIRECTORY;
	string COLORS_TEMP_DIRECTORY;
	string TURNTABLE_IMAGES_TEMP_DIRECTORY;

	static Configuration* GetInstance();
	static void	Destroy();
	Configuration();
	~Configuration();

	void CachePLMsettingsFromFile();
	void InitializeCLOViseData();
	void ClearPLMsettingsCache();
	void CachePLMsettings(json _selectedTenantDetails);
	bool ValidatePLMSettings();
	void CreateTemporaryFolderPath();

	string GetPLMServerURL();
	string GetLogFileName();
	string GetPLMVersion();
	string GetCLOViseVersion();
	string GetConnectionStatus();
	string GetBearerToken();
	json GetPLMPluginConfigJSON();
	QStringList GetResultsPerPage();
	string GetSelectedResultsPerPage();
	bool GetCloseResultsDialogue();
	QStringList GetSupportedAttsList();
	QStringList GetModifySupportedAttsList();
	QStringList GetResultsSupportedAttsList();
	int GetMaxLogFileSize();
	int GetProgressBarTimer();
	bool GetLoggerEnabled();
	bool GetDebugEnabled();
	bool GetErrorEnabled();
	bool GetInfoEnabled();
	bool GetPerfomanceEnabled();
	string GetCLOViseLicenseURL();
	string GetCompanyApiKey();
	string GetCompanyName();
	string GetCsrfNonceToken();
	string GetCsrfNonceKey();
	bool GetPLMSignin();
	QStringList GetExcludedPreviewFields();
	std::map<QString, QString> GetColorwayImageLabels();
	std::map<QString, QString> GetStyleImageLabels();
	string GetInputUserName();
	string GetInputUserPassword();
	bool GetIsReadSavedCredential();
	int GetProgressBarProgress();
	string GetLoggedInUserName();
	json GetUserJson();
	bool GetRestAPILogEnabled();
	string GetMaximumLimitForRefAttValue();
	int GetCurrentScreen();
	bool GetIsUpdateColorClicked();
	bool GetIsPrintSearchClicked();
	bool GetIsNewStyleDownloaded();
	string GetLocalizedStyleClassName();
	string GetLocalizedMaterialClassName();
	string GetLocalizedColorClassName();
	bool GetNotFirstLogin();
	json GetClientSpecificJson();
	json GetClientSpecificAttCachedData();
	string GetSubTypeInternalName();
	string GetQueryParameterForMaterial();
	
	void SetPLMServerURL(string _plmServerURL);
	void SetLogFileName(string _logFileName);
	void SetPLMVersion(string _PlmVersion);
	void SetCLOViseVersion(string _schemaWithoutQuotes);
	void SetConnectionStatus(string _connectionStatus);
	void SetBearerToken(string _bearerToken);
	void SetPLMPluginConfigJSON();
	void SetResultsPerPage(string _key);
	void SetSelectedResultsPerPage(string _selected, bool _store, bool _setCached);
	void SetCloseResultsDialogue(string _key);
	void SetUserJson(json _userJson);
	void SetSupportedAttsList(json _configJson);
	void SetModifySupportedAttsList(json _configJson);
	void SetResultsSupportedAttsList(json _configJson);
	void SetMaxLogFileSize();
	void SetProgressBarCloseTimer(string _key);
	void SetLoggerEnabled();
	void SetDebugEnabled();
	void SetErrorEnabled();
	void SetInfoEnabled();
	void SetPerfomanceEnabled();
	void SetCLOViseLicenseURL(string _url);
	void SetCompanyApiKey(string _companyApiKey);
	void SetCompanyName(string _companyName);
	void SetCsrfNonceToken(string _csrfNonceToken);
	void SetCsrfNonceKey(string _csrfNonceKey);
	void SetPLMSignin(bool _isSignin);
	void SetCurrentScreen(int _screen);
	void SetExcludedPreviewFields(string _key);
	void SetInputUserName(string _name);
	void SetInputUserPassword(string _pass);
	void SetIsReadSavedCredential(bool _value);
	void SetProgressBarProgress(int _count = 0);
	void SetLoggedInUserName(json _userJson);
	void SetRestAPILogEnabled();
	void SetColorwayImageLabels(std::map<QString, QString> _colorwayImageLabelsMap);
	void SetStyleImageLabels(std::map<QString, QString> _styleImageLabelsMap);
	void SetMaximumLimitForRefAttValue();
	void SetIsUpdateColorClicked(bool _flag);
	void SetIsPrintSearchClicked(bool _flag);
	void SetIsNewStyleDownloaded(bool _flag);
	void SetLocalizedStyleClassName(string _styleClassName);
	void SetLocalizedMaterialClassName(string _materialClassName);
	void SetLocalizedColorClassName(string _colorClassName);
	void SetNotFirstLogin(bool _login = false);
	void SetClientSpecificJson(json _clientSpecificJson);
	void SetClientSpecificAttCachedData(json _clientSpecificAttCachedData);
	void SetSubTypeInternalName(json _clientSpecificJson);
	void SetQueryParameterForMaterial(string queryParamsForMaterial);
	

private:
	static Configuration* _instance; // zero initialized by default
	string m_plmServerURL;
	string m_plmVersion;
	string m_cloViseVersion;
	string m_connectionStatus;
	string m_logFileName;
	string m_bearerToken;
	json m_configJson = json::object();
	json m_UserJson = json::object();
	QStringList m_resultsPerPageOptions;
	string m_resultsPerPage;
	bool m_resultsClose = false;
	QStringList m_supportedAttTypes;
	QStringList m_resultsSupportedAttTypes;
	QStringList m_modifySupportedAttTypes;
	string m_maxRefAttValueLimit;
	int m_maxLogFileSize;
	string m_timer;
	bool m_LoggerEnabled = false;
	bool m_debugLoggEnabled = false;
	bool m_errorLoggEnabled = false;
	bool m_infoLoggEnabled = false;
	bool m_perfomanceEnabled = false;
	string m_CLOViseLicenseURL = "";
	string m_companyName = "";
	string m_companyApiKey = "";
	string m_csrfNonceKey;
	string m_csrfNonceToken;
	bool m_isSignin = false;
	int m_currentScreen;
	QStringList m_excludedPreviewFields;
	std::map<QString, QString> m_colorwayImageLabelsMap;
	std::map<QString, QString> m_styleImageLabelsMap;
	string m_inputUserName = "";
	string m_inputUserPassword = "";
	bool m_readSavedCredential = true;
	int m_progress = 0;
	string m_UserFullName = "";
	bool m_RestAPILogEnabled = false;
	bool m_UpdateColorClicked = false;
	bool m_isPrintSearchClicked = false;
	bool m_StyleDownloaded = false;
	string m_styleClassName = "";
	string m_materialClassName = "";
	string m_colorClassName = "";
	bool m_notFirstLogin = false;
	json m_clientSpecificJson;
	json m_clientSpecificAttCachedData;
	string m_categoryInternalName;
	string m_queryParamsForMaterial;
	};
