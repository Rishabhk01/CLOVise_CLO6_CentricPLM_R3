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
#include "UpdateMaterialConfig.h"
#include "CLOAPIInterface.h"
#include "CLOVise/PLM/Helper/Util/RestAPIUtility.h"
#include "CLOVise/PLM/Helper/Util/FormatHelper.h"
#include "CLOVise/PLM/Helper/Util/Logger.h"

using json = nlohmann::json;

UpdateMaterialConfig* UpdateMaterialConfig::_instance = NULL;
UpdateMaterialConfig* UpdateMaterialConfig::GetInstance()
{
	if (_instance == NULL)
	{
		_instance = new UpdateMaterialConfig();
	}
	return _instance;
}

void  UpdateMaterialConfig::Destroy()
{
	if (_instance)
	{
		delete _instance;
		_instance = NULL;
	}
}

/*
* Description - SetCreateMaterialConfigJSON() method used to set document configuration json.
* Parameter -  string, string, bool.
* Exception - exception, Char *
* Return -
*/
void UpdateMaterialConfig::SetUpdateMaterialConfigJSON(string _module, string _rootType, bool _initDepedentJsons)
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
		Logger::Error("CreateMaterialConfig -> SetCreateMaterialConfigJSON Exception :: " +msg);
		throw msg;
	}
	catch (exception& e)
	{
		Logger::Error("CreateMaterialConfig -> SetCreateMaterialConfigJSON Exception :: " + string(e.what()));
		throw e.what();
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
json UpdateMaterialConfig::GetUpdateMaterialConfigJSON()
{
	Logger::Info("CreateMaterialConfig -> GetDocumentConfigJSON() start");

	if (m_documentConfigJson == nullptr || m_documentConfigJson.empty())
		SetUpdateMaterialConfigJSON(PRODUCT_MODULE, PRODUCT_ROOT_TYPE, true);
	Logger::Info("CreateMaterialConfig -> GetDocumentConfigJSON() end");

	return m_documentConfigJson;
}

/*
* Description - Set3DModelType() method used to set 3d model type.
* Parameter -  string.
* Exception -
* Return -
*/
void UpdateMaterialConfig::Set3DModelType(string _key)
{
	m_3DModelType = Helper::GetJSONValue<string>(GetUpdateMaterialConfigJSON(), _key, true);
}

/*
* Description - Get3DModelType() method used to get 3d model type.
* Parameter -
* Exception -
* Return - string.
*/
string UpdateMaterialConfig::Get3DModelType()
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
void UpdateMaterialConfig::SetDocumentFieldsJSON(string _fieldsJsonKey)
{
	Logger::Info("CreateMaterialConfig -> SetDocumentFieldsJSON() start");

	try
	{
		string documentFieldsStr = "";
		documentFieldsStr = Helper::GetJSONValue<string>(GetUpdateMaterialConfigJSON(), _fieldsJsonKey, false);
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
		throw e.what();
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
json UpdateMaterialConfig::GetDocumentFieldsJSON()
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
void UpdateMaterialConfig::SetDateFlag(bool _isDateEditPresent)
{
	m_isDateEditPresent = _isDateEditPresent;
}

/*
* Description - GetDateFlag() method used to get date exist flag.
* Parameter -
* Exception -
* Return - bool.
*/
bool UpdateMaterialConfig::GetDateFlag()
{
	return m_isDateEditPresent;
}

/*
* Description - SetMaxUploadFileSize() method used to set maximum upload file size.
* Parameter -
* Exception -
* Return -
*/
void UpdateMaterialConfig::SetMaxUploadFileSize(string _key)
{
	m_maxZPRJUploadFileSize = stoi(Helper::GetJSONValue<string>(GetUpdateMaterialConfigJSON(), _key, true));
}

/*
* Description - GetMaxUploadFileSize() method used to get maximum upload file size.
* Parameter -
* Exception -
* Return - int.
*/
int UpdateMaterialConfig::GetMaxUploadFileSize()
{
	if (!FormatHelper::HasContent(to_string(m_maxZPRJUploadFileSize)))
		SetMaxUploadFileSize(MAX_ZPRJ_UPLOAD_FILE_SIZE);
	return m_maxZPRJUploadFileSize;
}