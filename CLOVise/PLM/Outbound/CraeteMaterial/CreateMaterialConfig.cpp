/*
* Copyright 2020-2021 CLO-Vise. All rights reserved
*
* @file CreateMaterialConfig.cpp
*
* @brief Class implementation for cach downloaded Create Material data in CLO from PLM.
* This class has all the variable and methods implementation which are used in storing PLM Create Material configuration related data to downloaded in CLO
*
* @author GoVise
*
* @date 14-AUG-2020
*/
#include "CreateMaterialConfig.h"
#include "CLOAPIInterface.h"
#include "CLOVise/PLM/Helper/Util/RestAPIUtility.h"
#include "CLOVise/PLM/Helper/Util/FormatHelper.h"
#include "CLOVise/PLM/Helper/Util/Logger.h"

using json = nlohmann::json;

CreateMaterialConfig* CreateMaterialConfig::_instance = NULL;
CreateMaterialConfig* CreateMaterialConfig::GetInstance()
{
	if (_instance == NULL)
	{
		_instance = new CreateMaterialConfig();
	}
	return _instance;
}

/*
* Description - SetCreateMaterialConfigJSON() method used to set document configuration json.
* Parameter -  string, string, bool.
* Exception - exception, Char *
* Return -
*/
void CreateMaterialConfig::SetCreateMaterialConfigJSON(string _module, string _rootType, bool _initDepedentJsons)
{
	Logger::Info("CreateMaterialConfig -> SetCreateMaterialConfigJSON start");
	Logger::Logger(" Publish To PLM initialiazing...");
	json param = json::object();
	string configJsonString = "";
	const char* restErrorMsg = "";
	param = RESTAPI::AddToRestParamMap(param, MODULE_KEY, _module);
	param = RESTAPI::AddToRestParamMap(param, ROOT_TYPE_KEY, _rootType);

	try
	{
		Logger::Info("CreateMaterialConfig -> if (restData)  start");

		configJsonString = RESTAPI::RESTMethodSubmit(RESTAPI::CREATEORUPDATE_FIELDS_API, param);
		Logger::Debug("configJsonString---" + configJsonString);
		if (!FormatHelper::HasContent(configJsonString))
		{
			throw "Unable to initiliaze Document Configuration. Please try again or Contact your System Administrator.";
		}

		if (FormatHelper::HasError(configJsonString))
		{
			throw runtime_error(configJsonString);
		}
		string error = RESTAPI::CheckForErrorMsg(configJsonString);
		if (FormatHelper::HasContent(error))
		{
			throw std::logic_error(error);
		}
		m_documentConfigJson = json::parse(configJsonString);
		
		if (_initDepedentJsons)
		{
			Set3DModelType(_3DMODEL_TYPE_KEY);
			SetDocumentFieldsJSON(FIELDLIST_JSON_KEY);
		}
	}
	catch (string msg)
	{
		Logger::Error("CreateMaterialConfig -> SetCreateMaterialConfigJSON Exception :: " + msg);
		throw msg;
	}
	catch (exception& e)
	{
		Logger::Error("CreateMaterialConfig -> SetCreateMaterialConfigJSON Exception :: " + string(e.what()));
		throw e;
	}
	catch (const char* msg)
	{
		Logger::Error("CreateMaterialConfig -> SetCreateMaterialConfigJSON Exception :: " + string(msg));
		throw msg;
	}
	Logger::Logger(" CreateMaterialConfig intitialization Completed.");
	Logger::Info("CreateMaterialConfig -> SetCreateMaterialConfigJSON end");
}

/*
* Description - GetDocumentConfigJSON() method used to get document configuration json.
* Parameter -  
* Exception - 
* Return - json.
*/
json CreateMaterialConfig::GetCreateMaterialConfigJSON()
{
	Logger::Info("CreateMaterialConfig -> GetDocumentConfigJSON() start");

	if (m_documentConfigJson == nullptr || m_documentConfigJson.empty())
		SetCreateMaterialConfigJSON(PRODUCT_MODULE, PRODUCT_ROOT_TYPE, true);
	Logger::Info("CreateMaterialConfig -> GetDocumentConfigJSON() end");

	return m_documentConfigJson;
}

/*
* Description - Set3DModelType() method used to set 3d model type.
* Parameter -  string.
* Exception - 
* Return -
*/
void CreateMaterialConfig::Set3DModelType(string _key)
{
	m_3DModelType = Helper::GetJSONValue<string>(GetCreateMaterialConfigJSON(), _key, true);
}

/*
* Description - Get3DModelType() method used to get 3d model type.
* Parameter -  
* Exception - 
* Return - string.
*/
string CreateMaterialConfig::Get3DModelType()
{
	if (!FormatHelper::HasContent(m_3DModelType))
		Set3DModelType(_3DMODEL_TYPE_KEY);
	return m_3DModelType;
}

/*
* Description - SetDocumentFieldsJSON() method used to set document fields json.
* Parameter -  string.
* Exception - exception, Char *
* Return -
*/
void CreateMaterialConfig::SetDocumentFieldsJSON(string _fieldsJsonKey)
{
	Logger::Info("CreateMaterialConfig -> SetDocumentFieldsJSON() start");

	try
	{
		string documentFieldsStr = "";
		documentFieldsStr = Helper::GetJSONValue<string>(GetCreateMaterialConfigJSON(), _fieldsJsonKey, false);
		m_documentFieldsJson = json::parse(documentFieldsStr);
	}
	catch (string msg)
	{
		Logger::Error("CreateMaterialConfig -> SetDocumentFieldsJSON Exception :: " + msg);
		throw msg;
	}
	catch (exception& e)
	{
		Logger::Error("CreateMaterialConfig -> SetDocumentFieldsJSON Exception :: " + string(e.what()));
		throw e;
	}
	catch (const char* msg)
	{
		Logger::Error("CreateMaterialConfig -> SetDocumentFieldsJSON Exception :: " + string(msg));
		throw msg;
	}
	Logger::Info("CreateMaterialConfig -> SetDocumentFieldsJSON() end");

}

/*
* Description - GetDocumentFieldsJSON() method used to get document fields json.
* Parameter -  
* Exception - 
* Return - json.
*/
json CreateMaterialConfig::GetDocumentFieldsJSON()
{
	Logger::Info("CreateMaterialConfig -> GetDocumentFieldsJSON() start");

	if (m_documentFieldsJson == nullptr || m_documentFieldsJson.empty())
		SetDocumentFieldsJSON(FIELDLIST_JSON_KEY);
	Logger::Info("CreateMaterialConfig -> GetDocumentFieldsJSON() end");

	return m_documentFieldsJson;
}

/*
* Description - SetDateFlag() method used to set date exist flag.
* Parameter -  bool.
* Exception -
* Return -
*/
void CreateMaterialConfig::SetDateFlag(bool _isDateEditPresent)
{
	m_isDateEditPresent = _isDateEditPresent;
}

/*
* Description - GetDateFlag() method used to get date exist flag.
* Parameter -
* Exception -
* Return - bool.
*/
bool CreateMaterialConfig::GetDateFlag()
{
	return m_isDateEditPresent;
}

/*
* Description - SetMaxUploadFileSize() method used to set maximum upload file size.
* Parameter -
* Exception -
* Return -
*/
void CreateMaterialConfig::SetMaxUploadFileSize(string _key)
{
	m_maxZPRJUploadFileSize = stoi(Helper::GetJSONValue<string>(GetCreateMaterialConfigJSON(), _key, true));
}

/*
* Description - GetMaxUploadFileSize() method used to get maximum upload file size.
* Parameter -
* Exception -
* Return - int.
*/
int CreateMaterialConfig::GetMaxUploadFileSize()
{
	Logger::Info("CreateMaterialConfig -> GetMaxUploadFileSize() Start");
	if (!FormatHelper::HasContent(to_string(m_maxZPRJUploadFileSize)))
		SetMaxUploadFileSize(MAX_ZPRJ_UPLOAD_FILE_SIZE);
	Logger::Info("CreateMaterialConfig -> GetMaxUploadFileSize() End");
	return m_maxZPRJUploadFileSize;
}

/*
* Description - GetIsModelExecuted() return a flag wether Rest call is being made and module is already executed, used for progress bar to work correctly
* Parameter -
* Exception -
* Return - bool
*/
bool CreateMaterialConfig::GetIsModelExecuted()
{
	return m_isModelExecuted;
}

/*
* Description - SetIsModelExecuted() sets a flag wether Rest call is being made and module is already executed, used for progress bar to work correctly
* Parameter -
* Exception -
* Return -
*/
void CreateMaterialConfig::SetIsModelExecuted(bool _isModelExecuted)
{
	m_isModelExecuted = _isModelExecuted;
}

/*
* Description - SetIsSaveAndCloseClicked() method used to set the status for save and close clicked
* Parameter - _isSaveAndCloseClicked
* Exception -
* Return - void
*/
void CreateMaterialConfig::SetIsSaveAndCloseClicked(bool _isSaveAndCloseClicked)
{
	m_isSaveAndCloseClicked = _isSaveAndCloseClicked;
}

/*
* Description - GetIsSaveAndCloseClicked() method used to Get the status for save and close clicked.
* Parameter -
* Exception -
* Return - bool
*/
bool CreateMaterialConfig::GetIsSaveAndCloseClicked()
{
	return m_isSaveAndCloseClicked;
}
/*
* Description - ResetCreateConfig() is used to reset the values of all the configurations.
* Parameter -
* Exception -
* Return -
*/
void CreateMaterialConfig::ResetCreateConfig()
{
	Logger::Info("CreateMaterialConfig -> ResetCreateConfig() Start");
	m_isModelExecuted = false;
	m_documentFieldsJson = nullptr;
	m_documentConfigJson = nullptr;
	SetIsSaveAndCloseClicked(false);
	SetIsModelExecuted(m_isModelExecuted);
	m_createMaterialLoggedOut = true;
	Logger::Info("CreateMaterialConfig -> ResetCreateConfig() End");
}
