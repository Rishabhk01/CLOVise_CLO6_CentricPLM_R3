#include "stdafx.h"
#include "LibraryWindowImplPlugin.h"

#include <QFileInfo>
#include <QChar>
#include <QString>
#include <QStringList>
#include <QFile>
#include <QDir>
#include <QDateTime>
#include <QtAlgorithms>
#include "qhostinfo.h"

#include "classes/APIDefine.h"
#include "classes/APIStorage.h"
#include "classes/APIUtility.h"
#include "CLOVise/PLM/Helper/Util/RestAPIUtility.h"
#include "CLOAPIInterface.h"


#include "classes/APISigninDialogSample.h"
#include "classes/APISearchDialogSample.h"
#include "classes/PLMSettingsSample.h"

#include "CLOVise/PLM/Authentication/CLOPLMImplementation.h"
#include "CLOVise/PLM/Authentication/CLOPLMSignIn.h"
#include "CLOVise/PLM/Helper/Util/Configuration.h"
//#include "CLOVise/PLM/Utilities/Definitions.h"
#include "CLOVise/PLM/Helper/Util/Logger.h"


// customizing functions
#define USE_LIBRARY_WINDOW_CUSTOM_TAB  1
#define USE_CUSTOM_TAB_AS_DEFAULT	   1
#define USE_PLM_SETTINGS			   1
#define USE_DOFUNCTION				   1
#define USE_STARTUP_DOFUNCTION		   1
#define USE_AFTER_FILE_LOAD_DOFUNCTION 1

namespace CLOAPI
{
	 static size_t WriteCallback(void* _contents, size_t _size, size_t _nmemb, string* _userp) 
	{
		Logger::Logger("inside WriteCallback");
		_userp->append((char*)_contents, _size * _nmemb);
		Logger::Logger("_size * _nmemb" + (_size * _nmemb));
		return _size * _nmemb;
	}

	 static string PostRestCall(string _authorisation, string _data, string _url)
	{
		 string response;
		 CURL *curl;
		 CURLcode res;
		 curl = curl_easy_init();
		 if (curl) {
			 curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "POST");
			 curl_easy_setopt(curl, CURLOPT_URL, _url.c_str());
			 curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false);
			 curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, false);
			 curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
			 curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
			 struct curl_slist *headers = NULL;
			 headers = curl_slist_append(headers, "X-User-Product: clovise");
			 headers = curl_slist_append(headers, "Content-Type: application/x-www-form-urlencoded");
			 headers = curl_slist_append(headers, _authorisation.c_str());
			 curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
			 const char *data = "machineName=DESKTOP-FCH5J2Q&privateIp=fe80%3A%3Acdaf%3A4fe7%3A2929%3A6374%2511&companyName=fastretailing&userId=CloVise";
			 curl_easy_setopt(curl, CURLOPT_POSTFIELDS, _data.c_str());
			 res = curl_easy_perform(curl);
		 }
		 curl_easy_cleanup(curl);
		 Logger::Logger(to_string(res));

		if (res != CURLE_OK) 
		{
			stringstream errormsg;
			errormsg << "unable to process your request, plm server is down or unavailable. please contact system administrator";
			Logger::Logger(errormsg.str());
			response = errormsg.str();
		}
		return response;
	}
	
	json ValidateCVUserLicense(const string& _url, const string& _userId)
	{
		Logger::Info("ValidateUserLicense :: License login - START");
		
		char hostbuffer[256];		
		int hostname = gethostname(hostbuffer, sizeof(hostbuffer)); // To retrieve hostname		
		QString qhostName = QHostInfo::localHostName(); // To retrieve host information 
		QHostInfo info = QHostInfo::fromName(qhostName);
		QHostAddress IPAddress;
		if (!info.addresses().isEmpty())
		{
			IPAddress = info.addresses().first();
		}
		if (Configuration::GetInstance()->GetCompanyName().empty())
		{
			Logger::Logger("ValidateUserLicense :: License login - END1");
			return json::parse(LICENSE_ERROR_CODE_101);
		}
		else if (Configuration::GetInstance()->GetCompanyApiKey().empty())
		{
			Logger::Logger("ValidateUserLicense :: License login - END2");
			return json::parse(LICENSE_ERROR_CODE_102);
		}
		string basic_auth = "Authorization: Basic " + Helper::ConvertToBase64(Configuration::GetInstance()->GetCompanyName() + ":" + Configuration::GetInstance()->GetCompanyApiKey());

		string formattedIp = IPAddress.toString().toStdString();
		//formattedIp = Helper::FindAndReplace(formattedIp, "%", "%25");
		//formattedIp = Helper::FindAndReplace(formattedIp, ":", "%3A");
		string loginRequestParam = "machineName=" + qhostName.toStdString();
		loginRequestParam += "&privateIp=" + formattedIp;
		loginRequestParam += "&companyName=" + Configuration::GetInstance()->GetCompanyName();
		loginRequestParam += "&userId=" + _userId;
		Logger::Debug("License login param : " + loginRequestParam);
		Logger::Debug("License login url : " + _url);
		Logger::Debug("License login basic_auth : " + basic_auth);
		
		string response = PostRestCall(basic_auth, loginRequestParam, _url);
		
		Logger::Info("License login response : " + response);
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

	bool ValidateCVLicenseLogOut(string _userId)
	{
		Logger::Logger("LibraryWindowImplPlugin ValidateCVLicenseLogOut start");
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
		Logger::Logger("LibraryWindowImplPlugin ValidateCVLicenseLogOut end");
		return loggedOut;
	}

	

	LibraryWindowImplPlugin::LibraryWindowImplPlugin()
	{
		Logger::Logger("LibraryWindowImplPlugin LibraryWindowImplPlugin start");
		// prepare api storage instance
		API_STORAGE->GenerateAPIItemListForSample();
		Logger::Logger("LibraryWindowImplPlugin LibraryWindowImplPlugin start");
	}

	LibraryWindowImplPlugin::~LibraryWindowImplPlugin()
	{
		
		if (Configuration::GetInstance()->GetPLMSignin())
		{			
			ValidateCVLicenseLogOut(CLOVise::CLOPLMSignIn::GetInstance()->GetID().toStdString());
		}
		// release api storage instance
		DESTROY_API_STORAGE;
		
		
	}

	bool LibraryWindowImplPlugin::EnableCustomUI()
	{
		Logger::Logger("LibraryWindowImplPlugin EnableCustomUI start");
#if USE_LIBRARY_WINDOW_CUSTOM_TAB 
		return true;
#else
		return false;
#endif
		Logger::Logger("LibraryWindowImplPlugin EnableCustomUI end");
	}

	bool LibraryWindowImplPlugin::IsDefaultTab()
	{

		Logger::Logger("LibraryWindowImplPlugin IsDefaultTab start");
#if USE_CUSTOM_TAB_AS_DEFAULT
		return true;
#else
		return false;
#endif

		
	}

	string LibraryWindowImplPlugin::GetTabName()
	{
		Logger::Logger("LibraryWindowImplPlugin GetTabName start");
		string str = "PLM";
		
		Logger::Logger("LibraryWindowImplPlugin GetTabName end");
		return str;
	}

	string LibraryWindowImplPlugin::GetItemList(const string& itemId, unsigned int pageNo, unsigned int pageSize, CLO_FINDER_SORT sort, bool bAscending, const string& searchText)
	{
		Logger::Logger("LibraryWindowImplPlugin GetItemList start");
		vector<CLOAPISample::LibraryAPIItem> itemList;
		//added these two lined to delete older progress bars of clovise if any, to avoid wrong message in progress bar
		UTILITY_API->SetProgress("Loading", 0);
		UTILITY_API->DeleteProgressBar(true);
		//API_STORAGE->GenerateAPIItemListForSample(); discuss with Prashant.
		API_STORAGE->SetPLMAssetItemList(QString::fromStdString(itemId));
		// search qvariantmap
		QVariantMap searchKeyValue;
		searchKeyValue[META_DATA_KEY_0_DATA_TYPE] = QString(itemId.c_str());
		
		if(API_STORAGE)
			itemList = API_STORAGE->GetSearchList("", searchKeyValue);

		// sort item list with ascending / descending 
		// - write code

		// get item list with pageNo / pageSize considered 
		// - write code

		// get/return json string
		QString jsonStr = generateJSONFromLibraryAPIItemList(itemList);
		string result = jsonStr.toStdString();
		Logger::Logger("LibraryWindowImplPlugin GetItemList result  ------" + result);
		try
		{
			string logPath = Configuration::GetInstance()->GetLogFileName();
			if (logPath == "")
				Logger::CreateLogFileName();
			//Create Cache of PLM Settings reading PLM Setting file
			Configuration::GetInstance()->CachePLMsettingsFromFile();
			Configuration::GetInstance()->InitializeCLOViseData();
			if (itemId.compare("CLO-VISE SUITE") == 0)
			{
				Logger::Logger("connecting to CLOVise suit....");
				//checking if user has filled in all the PLMSettings fields.
				if(Configuration::GetInstance()->ValidatePLMSettings())
				{
					
					CLOVise::CLOPLMImplementation plmSearchDlg;
					plmSearchDlg.ConnectToCLOVise(CLOVise::PLM_SEARCH, false);
				}
				else
				{
					UTILITY_API->DisplayMessageBox("Unable to Login. Please Verify PLMSettings.");
					Logger::Logger("Unable to Login. Please Verify PLMSettings.");
				}
				Logger::Logger("connected to CLOVise suit....");
			}
		}
		catch (string msg)
		{
			Logger::Error("LibraryWindowImplPlugin -> GetItmeList Exception :: " + msg);
			UTILITY_API->DisplayMessageBox("Exception:: " + msg);
		}
		catch (exception& e)
		{
			Logger::Error("LibraryWindowImplPlugin -> GetItmeList Exception :: " + string(e.what()));
			UTILITY_API->DisplayMessageBox("Exception:: " + string(e.what()));
		}
		catch (const char* msg)
		{
			Logger::Error("LibraryWindowImplPlugin -> GetItmeList Exception :: " + string(msg));
			wstring wstr(msg, msg + strlen(msg));
			UTILITY_API->DisplayMessageBoxW(wstr);
		}
		Logger::Logger("LibraryWindowImplPlugin GetItemList end");
		return result;
	}


	string LibraryWindowImplPlugin::GetSearchItemList(unsigned int pageNo, unsigned int pageSize, CLO_FINDER_SORT sort, bool bAscending, const string& searchText)
	{
		Logger::Logger("LibraryWindowImplPlugin GetSearchItemList start");
		string result = "";
		Logger::Logger("LibraryWindowImplPlugin GetSearchItemList end");
		return result;

	}

	bool LibraryWindowImplPlugin::GetSearchItemList(const string& itemId, unsigned int pageNo, unsigned int pageSize, CLO_FINDER_SORT sort, bool bAscending, string& resultItemList)
	{
		Logger::Logger("LibraryWindowImplPlugin GetSearchItemList start");
		string result = "";
		CLOAPISample::APISearchDialogSample searchDlg;
		Logger::Logger("LibraryWindowImplPlugin GetSearchItemList end");
		return false;
	}


	bool LibraryWindowImplPlugin::GetParentFolderID(const string& itemId, string& parentFolderID)
	{
		return false;	
	}

	CLO_BYTE* LibraryWindowImplPlugin::GetPNGThumbnail(const string& itemId, unsigned int& sizeInByte)
	{
		Logger::Logger("LibraryWindowImplPlugin GetPNGThumbnail start");
		CLOAPISample::LibraryAPIItem item;
		
		if (API_STORAGE)
		{
			QString filePath;
			if (API_STORAGE->GetIconThumbnailPathWithID(itemId.c_str(), filePath))
			{				
				if (UTILITY_API->IsCLOFileFormatWithThumbnail(filePath.toStdString()))						// for CLO file format which contains thumbnails
				{
					if (UTILITY_API->IsCLOFileFormatWithTripleThumbnail(filePath.toStdString()))			// for CLO file format which contains 3 preview images (index: 0 = front thumbnail, 1 = back thumbnail)
					{
						return UTILITY_API->GetThumbnailInCLOFile(filePath.toStdString(), 0, sizeInByte);	// index: 0 = front thumbnail, 1 = back thumbnail
					}
					else
					{
						// zfab and sst files are unusual case
						if (UTILITY_API && filePath.contains(".zfab", Qt::CaseInsensitive))
							return UTILITY_API->GetAssetIconInCLOFile(filePath.toStdString(), sizeInByte);
						else if (UTILITY_API && filePath.contains(".sst", Qt::CaseInsensitive))
							return UTILITY_API->GetThumbnailInCLOFile(filePath.toStdString(), 0, sizeInByte); // index: 0 = front thumbnail, 1 = back thumbnail
						else
							return UTILITY_API->GetThumbnailInCLOFile(filePath.toStdString(), 0, sizeInByte); // for CLO file format which contains only one thumnail 
					}
				}				
				else if (UTILITY_API->IsReadableImageFormat(filePath.toStdString()))							// for common type of images which can be displayed in Library Windows, CLO software
				{
					return FileLoadViaString(filePath, sizeInByte);
				}					
			}
		}
		Logger::Logger("LibraryWindowImplPlugin GetPNGThumbnail end");
		return NULL;
	}

	string LibraryWindowImplPlugin::GetMetaData(const string& itemId)
	{
		Logger::Logger("LibraryWindowImplPlugin GetMetaData start");
		string result = "";
		Logger::Logger("LibraryWindowImplPlugin GetMetaData end");
		return result;
	}

	vector<string> LibraryWindowImplPlugin::GetMetaDataKeyListForUI(const string & itemId)
	{
		Logger::Logger("LibraryWindowImplPlugin GetMetaDataKeyListForUI start");
		vector<string> strList;
		
		//strList.push_back(META_DATA_KEY_0_DATA_TYPE.toStdString()); // do not show in list view
		//strList.push_back(META_DATA_KEY_1_SERVICE_DIVISION.toStdString()); // do not show in list view
		strList.push_back(META_DATA_KEY_2_BRAND.toStdString()); // show brand in list view
		strList.push_back(META_DATA_KEY_3_PRODUCT_TYPE.toStdString()); // show product type in list view
		//strList.push_back(META_DATA_KEY_4_SEASON_CREATED.toStdString()); // do not show in list view
		Logger::Logger("LibraryWindowImplPlugin GetMetaDataKeyListForUI end");
		return strList;
	}

	CLO_BYTE* LibraryWindowImplPlugin::GetPreviewImage(const string& itemId, unsigned int& sizeInByte)
	{
		Logger::Logger("LibraryWindowImplPlugin GetPreviewImage start");
		CLOAPISample::LibraryAPIItem item;

		if (API_STORAGE)
		{
			QString filePath;
			if (API_STORAGE->GetPreviewThumbnailPathWithID(itemId.c_str(), filePath))
			{				
				if (UTILITY_API->IsCLOFileFormatWithThumbnail(filePath.toStdString()))						// for CLO file format which contains thumbnails
				{
					if (UTILITY_API->IsCLOFileFormatWithTripleThumbnail(filePath.toStdString()))			// for CLO file format which contains 3 preview images (index: 0 = front thumbnail, 1 = back thumbnail)
					{
						return UTILITY_API->GetThumbnailInCLOFile(filePath.toStdString(), 0, sizeInByte);	// index: 0 = front thumbnail, 1 = back thumbnail
					}
					else
					{
						// zfab and sst files are unusual case
						if (UTILITY_API && filePath.contains(".zfab", Qt::CaseInsensitive))
							return UTILITY_API->GetAssetIconInCLOFile(filePath.toStdString(), sizeInByte);
						else if (UTILITY_API && filePath.contains(".sst", Qt::CaseInsensitive))
							return UTILITY_API->GetThumbnailInCLOFile(filePath.toStdString(), 0, sizeInByte); // index: 0 = front thumbnail, 1 = back thumbnail
						else
							return UTILITY_API->GetThumbnailInCLOFile(filePath.toStdString(), 0, sizeInByte); // for CLO file format which contains only one thumnail 
					}
				}
				else if (UTILITY_API->IsReadableImageFormat(filePath.toStdString()))							// for common type of images which can be displayed in Library Windows, CLO software
				{
					return FileLoadViaString(filePath, sizeInByte);
				}
			}				
		}
		Logger::Logger("LibraryWindowImplPlugin GetPreviewImage end");
		return NULL;
	}

	CLO_BYTE* LibraryWindowImplPlugin::GetFile(const string& itemId, unsigned int& sizeInByte)
	{
		Logger::Logger("LibraryWindowImplPlugin GetFile start");
		CLOAPISample::LibraryAPIItem item;

		if (API_STORAGE)
		{
			QString filePath;
			if (API_STORAGE->GetFilePathWithID(itemId.c_str(), filePath))
				return FileLoadViaString(filePath, sizeInByte);
		}
		Logger::Logger("LibraryWindowImplPlugin GetFile end");
		return NULL;
	}

	bool LibraryWindowImplPlugin::IsSigninEnabled()
	{
		return true; 
	}

	bool LibraryWindowImplPlugin::Signin()
	{
		Logger::Logger("LibraryWindowImplPlugin Signin start");
		try
		{
			string logPath = Configuration::GetInstance()->GetLogFileName();
			if (logPath == "")
				Logger::CreateLogFileName();
			//create cache of plm settings reading plm setting file
			Configuration::GetInstance()->CachePLMsettingsFromFile();
			Configuration::GetInstance()->InitializeCLOViseData();
			Logger::Logger("connecting to CLOVise suit....");
			//checking if user has filled in all the plm settings fields.
			if (Configuration::GetInstance()->GetNotFirstLogin() && Configuration::GetInstance()->ValidatePLMSettings())
			{

				CLOVise::CLOPLMImplementation plmSearchDlg;
				plmSearchDlg.SetSearchMode(CLOVise::PLM_SEARCH, true);
			}
			else  if (Configuration::GetInstance()->GetNotFirstLogin())
			{
				UTILITY_API->DisplayMessageBox("Unable to Login. Please Verify PLMSettings.");
				Logger::Logger("Unable to Login. Please Verify PLMSettings.");
			}
			//Additional check to temporary fix login popup issue in SDK3.1.1
			Configuration::GetInstance()->SetNotFirstLogin(true);

			Logger::Logger("connected to CLOVise suit....");
			/*if (Configuration::GetInstance()->GetPLMSignin())
			{
				QFuture<void> future1 = QtConcurrent::run(logout);
				future1.waitForFinished();
				logout();
			}*/
		}
		catch (string msg)
		{
			Logger::Error("LibraryWindowImplPlugin -> GetItmeList Exception :: " + msg);
			UTILITY_API->DisplayMessageBox(msg);
		}
		catch (exception& e)
		{
			Logger::Error("LibraryWindowImplPlugin -> GetItmeList Exception :: " + string(e.what()));
			UTILITY_API->DisplayMessageBox("Exception:: " + string(e.what()));
		}
		catch (const char* msg)
		{
			Logger::Error("LibraryWindowImplPlugin -> GetItmeList Exception :: " + string(msg));
			wstring wstr(msg, msg + strlen(msg));
			UTILITY_API->DisplayMessageBoxW(wstr);
		}
		#if defined( __APPLE__ )
			return true;
		#endif
			Logger::Logger("LibraryWindowImplPlugin Signin end");
	}

	bool LibraryWindowImplPlugin::Signin(const string& id, const string& pw, string& token)
	{
		return false;
	}

	string LibraryWindowImplPlugin::GetFavoriteList()
	{
		Logger::Logger("LibraryWindowImplPlugin GetFavoriteList start");
		vector<CLOAPISample::LibraryAPIItem> itemList;
		//plm material
		CLOAPISample::LibraryAPIItem material;
		material.itemID = "MATERIAL";
		material.itemType = "FOLDER";
		material.itemName = "Material";
		itemList.push_back(material);

		CLOAPISample::LibraryAPIItem trim;
		trim.itemID = "TRIM";
		trim.itemType = "FOLDER";
		trim.itemName = "Trim";
		itemList.push_back(trim);

		//plm product
		CLOAPISample::LibraryAPIItem product;
		product.itemID = "PRODUCT";
		product.itemType = "FOLDER";
		product.itemName = "Product";
		itemList.push_back(product);

		//plm print Design
		CLOAPISample::LibraryAPIItem print;
		print.itemID = "PRINT";
		print.itemType = "FOLDER";
		print.itemName = "Print Design Color";
		itemList.push_back(print);

		//PLM item
		CLOAPISample::LibraryAPIItem plm;
		plm.itemID = "CLO-VISE SUITE";
		plm.itemType = "FOLDER";
		plm.itemName = "CLO-Vise Suite";
		itemList.push_back(plm);

		QString jsonStr = generateJSONFromLibraryAPIItemList(itemList);		
		string str = jsonStr.toStdString();
		Logger::Logger("LibraryWindowImplPlugin GetFavoriteList end");
		return str;
	}

	CLOAPISample::PLMSettingsSample* currentSettingsWidget = nullptr;

	bool LibraryWindowImplPlugin::IsPLMSettingsEnabled()
	{
		Logger::Logger("LibraryWindowImplPlugin IsPLMSettingsEnabled start");
#if USE_PLM_SETTINGS
		return true;
#else
		return false;
#endif
	}

	void* LibraryWindowImplPlugin::GetPLMSettingsWidget()
	{					
		Logger::Logger("LibraryWindowImplPlugin GetPLMSettingsWidget start");
		if (currentSettingsWidget)
			ReleasePLMSettingsWidget();

		currentSettingsWidget = new CLOAPISample::PLMSettingsSample();
		Logger::Logger("LibraryWindowImplPlugin GetPLMSettingsWidget end");
		return  static_cast<void*>(currentSettingsWidget);
	}

	void LibraryWindowImplPlugin::ReleasePLMSettingsWidget()
	{
		Logger::Logger("LibraryWindowImplPlugin ReleasePLMSettingsWidget start");
		if (currentSettingsWidget)
		{
			delete currentSettingsWidget;
			currentSettingsWidget = nullptr;
		}
		Logger::Logger("LibraryWindowImplPlugin ReleasePLMSettingsWidget end");
	}

	void LibraryWindowImplPlugin::SetNullptrPLMSettingsWidget()
	{
		Logger::Logger("LibraryWindowImplPlugin SetNullptrPLMSettingsWidget start");
		currentSettingsWidget = nullptr;
		Logger::Logger("LibraryWindowImplPlugin SetNullptrPLMSettingsWidget end");
	}
	
	void LibraryWindowImplPlugin::ResetPLMSettingsWidget()
	{
		Logger::Logger("LibraryWindowImplPlugin ResetPLMSettingsWidget start");
		if (currentSettingsWidget)
			currentSettingsWidget->Reset();
		Logger::Logger("LibraryWindowImplPlugin ResetPLMSettingsWidget end");
	}

	/// plug-in related functions
	bool LibraryWindowImplPlugin::IsPluginEnabled()
	{
		Logger::Logger("LibraryWindowImplPlugin ResetPLMSettingsWidget start");
#if USE_DOFUNCTION
		return true;
#else
		return false;
#endif
	}

	void LibraryWindowImplPlugin::DoFunction()
	{
		Logger::Logger("LibraryWindowImplPlugin DoFunction start");
#if USE_DOFUNCTION
		
#endif
		Logger::Logger("LibraryWindowImplPlugin DoFunction end");
	}

	void LibraryWindowImplPlugin::DoFunctionStartUp()
	{
		Logger::Logger("LibraryWindowImplPlugin DoFunctionStartUp start");
#if USE_STARTUP_DOFUNCTION
		
#endif
	}

	void LibraryWindowImplPlugin::DoFunctionAfterLoadingCLOFile(const char* fileExtenstion)
	{
		Logger::Logger("LibraryWindowImplPlugin DoFunctionAfterLoadingCLOFile start");
#if USE_AFTER_FILE_LOAD_DOFUNCTION
		
#endif
	}

	const char* LibraryWindowImplPlugin::GetActionName()
	{
		Logger::Logger("LibraryWindowImplPlugin GetActionName start");
		return "Library Windows Sample Plug-in";
	}

	const char* LibraryWindowImplPlugin::GetObjectNameTreeToAddAction()
	{
		Logger::Logger("LibraryWindowImplPlugin GetObjectNameTreeToAddAction start");
		const char* objetNameTree = "menu_Setting / menuPlug_In";

		return objetNameTree;
		Logger::Logger("LibraryWindowImplPlugin GetObjectNameTreeToAddAction end");
	}

	int LibraryWindowImplPlugin::GetPositionIndexToAddAction()
	{
		Logger::Logger("LibraryWindowImplPlugin GetPositionIndexToAddAction start");
		return 1; // 0: Above, 1: below (default = 0)
	}

	string LibraryWindowImplPlugin::GetIconOnFilePathForTab()
	{
		string filePath = "";

#if USE_CUSTOM_TAB_ICON
		if (UTILITY_API != nullptr)
			filePath = UTILITY_API->GetCLOExecutableFolderPath() + "library_tab_api_on.svg";	
#endif

		return filePath;
	}

	string LibraryWindowImplPlugin::GetIconOffFilePathForTab()
	{
		string filePath = "";

#if USE_CUSTOM_TAB_ICON
		if (UTILITY_API != nullptr)
			filePath = UTILITY_API->GetCLOExecutableFolderPath() + "library_tab_api_off.svg";
#endif

		return filePath;
	}
}



