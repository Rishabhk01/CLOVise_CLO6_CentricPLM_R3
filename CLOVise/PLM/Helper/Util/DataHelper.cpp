#include "DataHelper.h"
#include "CLOAPIInterface.h"
#include "CLOVise/PLM/Helper/Util/RestAPIUtility.h"
#include "CLOVise/PLM/Helper/Util/FormatHelper.h"
#include "CLOVise/PLM/Helper/Util/Logger.h"
#include "CLOVise/PLM/Inbound/Material/MaterialConfig.h"

using json = nlohmann::json;

DataHelper* DataHelper::_instance = NULL;
DataHelper* DataHelper::GetInstance()
{
	if (_instance == NULL)
	{
		_instance = new DataHelper();
	}
	return _instance;
}

json DataHelper::SetConfigJSON(string _module, string _rootType, bool _initDepedentJsons, string _FileName)
{
	Logger::Info("DataHelper -> SetConfigJSON start");
	//Logger::Logger(" Color Search initialiazing...");

	json param = json::object();
	string configJsonString = "";
	string restErrorMsg = "";
	json configJson = json::object();
	param = RESTAPI::AddToRestParamMap(param, MODULE_KEY, _module);
	param = RESTAPI::AddToRestParamMap(param, ROOT_TYPE_KEY, _rootType);

	try
	{
		if (m_restData)
		{
			Logger::Info("DataHelper -> if (restData)  start");

			configJsonString = RESTAPI::RESTMethodSubmit(RESTAPI::SEARCH_FIELDS_API, param);
			if (!FormatHelper::HasContent(configJsonString))
			{
				throw "Unable to initiliaze Color Configuration. Please try again or Contact your System Administrator.";
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

			configJson = json::parse(configJsonString);
			Logger::Info("DataHelper -> if (restData)  end");

		}
		else
		{
			Logger::Info("DataHelper -> else  start");

			configJson = Helper::GetDataFromJsonFile(_FileName);
			if (!FormatHelper::HasContent(to_string(configJson)))
			{
				throw "Unable to initiliaze Color Configuration. Please try again or Contact your System Administrator.";
			}
			Logger::Info("DataHelper -> else end ");

		}
	}
	catch (exception e)
	{
		Logger::Error("DataHelper -> SetConfigJSON Exception :: " + string(e.what()));
		throw e;
	}
	catch (const char* msg)
	{
		Logger::Error("DataHelper -> SetConfigJSON Exception :: " + string(msg));
		throw msg;
	}
	//Logger::Logger(" Color Search intitialization completed.");
	
	Logger::Info("DataHelper -> SetConfigJSON  end");
	
	return configJson;

}

json DataHelper::SetJson(string _JsonKey,json _ColorConfigJson)
{
	Logger::Info("DataHelper -> SetJson() start");
	json JsonObj = json::object();
	try
	{
		string colorHierarchyStr = "";

		colorHierarchyStr = Helper::GetJSONValue<string>(_ColorConfigJson, _JsonKey, false);
		JsonObj = json::parse(colorHierarchyStr);
	}
	catch (exception e)
	{
		Logger::Error("DataHelper -> SetJson() Exception :: " + string(e.what()));
		throw e;
	}
	catch (const char* msg)
	{
		Logger::Error("DataHelper -> SetJson() Exception :: " + string(msg));
		throw msg;
	}
	Logger::Info("DataHelper -> SetJson() end");
	return JsonObj;
}

json DataHelper::SetFilterJSON(string _module, string _rootType, string _activity, string _FileName)
{
	Logger::Info("DataHelper -> SetFilterJSON() start");

	json param = json::object();
	json filterJson = json::object();
	
	string filterJsonString = "";
	string restErrorMsg = "";
	param = RESTAPI::AddToRestParamMap(param, MODULE_KEY, _module);
	param = RESTAPI::AddToRestParamMap(param, ROOT_TYPE_KEY, _rootType);
	param = RESTAPI::AddToRestParamMap(param, ACTIVITY_KEY, _activity);

	try
	{
		if (m_restData)
		{
			filterJsonString = RESTAPI::RESTMethodSubmit(RESTAPI::FILTER_API, param);
			if (!FormatHelper::HasContent(filterJsonString))
			{
				string msg = "Unable to initiliaze" + _module + " Configuration. Please try again or Contact your System Administrator.";
				throw msg;
			}

			if (FormatHelper::HasError(filterJsonString))
			{
				throw runtime_error(filterJsonString);
			}
			string error = RESTAPI::CheckForErrorMsg(filterJsonString);
			if (FormatHelper::HasContent(error))
			{
				throw std::logic_error(error);
			}
			filterJson = Helper::GetJSONParsedValue<string>(json::parse(filterJsonString), FILTERSLIST_JSON_KEY, false);
		}
		else
		{
			filterJson = Helper::GetJSONParsedValue<string>(Helper::GetDataFromJsonFile(_FileName), FILTERSLIST_JSON_KEY, false);
			if (!FormatHelper::HasContent(to_string(filterJson)))
			{
				 string msg = "Unable to initiliaze" + _module  +  " Configuration. Please try again or Contact your System Administrator.";
				 throw msg;
			}
		}
	}
	catch (exception e)
	{
		Logger::Error("DataHelper -> SetFilterJSON Exception :: " + string(e.what()));
		throw e;
	}
	catch (const char* msg)
	{
		Logger::Error("DataHelper -> SetFilterJSON Exception :: " + string(msg));
		throw msg;
	}
	Logger::Info("DataHelper -> SetFilterJSON() End");
	return filterJson;
}

json DataHelper::SetViewJSON(string _module, string _rootType, string _activity, string _FileName)
{
	Logger::Info("DataHelper -> SetViewJSON() start");

	json param = json::object();
	json viewJson = json::object();
	
	string viewJsonString = "";
	string restErrorMsg = "";
	param = RESTAPI::AddToRestParamMap(param, MODULE_KEY, _module);
	param = RESTAPI::AddToRestParamMap(param, ROOT_TYPE_KEY, _rootType);
	param = RESTAPI::AddToRestParamMap(param, ACTIVITY_KEY, _activity);

	try
	{
		if (m_restData)
		{
			viewJsonString = RESTAPI::RESTMethodSubmit(RESTAPI::VIEW_API, param);
			if (!FormatHelper::HasContent(viewJsonString))
			{
				 string msg = "Unable to initiliaze" + _module + "Configuration. Please try again or Contact your System Administrator.";
				 throw msg;
			}

			if (FormatHelper::HasError(viewJsonString))
			{
				throw runtime_error(viewJsonString);
			}
			string error = RESTAPI::CheckForErrorMsg(viewJsonString);
			if (FormatHelper::HasContent(error))
			{
				throw std::logic_error(error);
			}
			viewJson = Helper::GetJSONParsedValue<string>(json::parse(viewJsonString), VIEWS_LIST_JSON_KEY, false);
		}
		else
		{
			viewJson = Helper::GetJSONParsedValue<string>(Helper::GetDataFromJsonFile(_FileName), VIEWS_LIST_JSON_KEY, false);
			if (!FormatHelper::HasContent(to_string(viewJson)))
			{
				 string msg = "Unable to initiliaze" + _module + "Configuration. Please try again or Contact your System Administrator.";
				 throw msg;
			}
		}
	}
	catch (exception e)
	{
		Logger::Error("DataHelper -> SetViewJSON Exception :: " + string(e.what()));
		throw e;
	}
	catch (const char* msg)
	{
		Logger::Error("DataHelper -> SetViewJSON Exception :: " + string(msg));
		throw msg;
	}
	Logger::Info("DataHelper -> SetViewJSON() end");
	return viewJson;
}

json DataHelper::SetSeasonPaletteJSON(string _FileName)
{
	Logger::Info("DataHelper -> SetSeasonPaletteJSON() start");

	json param = json::object();
	json paletteJson = json::object();
	
	string paletteJsonString = "";
	string restErrorMsg = "";

	try
	{
		if (m_restData)
		{
			paletteJsonString = RESTAPI::RESTMethodSubmit(RESTAPI::PALETTE_SEARCH_API, param);
			if (!FormatHelper::HasContent(paletteJsonString))
			{
				throw "Unable to initiliaze Season/Palette Configuration. Please try again or Contact your System Administrator.";
			}

			if (FormatHelper::HasError(paletteJsonString))
			{
				throw runtime_error(paletteJsonString);
			}
			string error = RESTAPI::CheckForErrorMsg(paletteJsonString);
			if (FormatHelper::HasContent(error))
			{
				throw std::logic_error(error);
			}

			paletteJson = json::parse(paletteJsonString);
		}
		else
		{
			paletteJson = Helper::GetDataFromJsonFile(_FileName );
			if (!FormatHelper::HasContent(to_string(paletteJson)))
			{
				throw "Unable to initiliaze Configuration. Please try again or Contact your System Administrator.";
			}
		}
	}
	catch (exception e)
	{
		Logger::Error("DataHelper -> SetSeasonPaletteJSON Exception :: " + string(e.what()));
		throw e;
	}
	catch (const char* msg)
	{
		Logger::Error("DataHelper -> SetSeasonPaletteJSON Exception :: " + string(msg));
		throw msg;
	}
	Logger::Info("DataHelper -> SetSeasonPaletteJSON() end");
	return paletteJson;
}

void DataHelper::SetAttScopes(json _configJson , string _attScopesJsonKey , QStringList& _attScopes)
{
	Logger::Info("DataHelper -> SetAttScopes() start");
	json attScopesArray = json::array();
	try
	{
		string attScopesStr = "";
		attScopesStr = Helper::GetJSONValue<string>(_configJson, _attScopesJsonKey, false);
		Logger::Debug("attScopesStr::" + attScopesStr);
		attScopesArray = json::parse(attScopesStr);
		string attScope = "";
		for (int i = 0; i < attScopesArray.size(); i++)
		{
			attScope = Helper::GetJSONValue<int>(attScopesArray, i, true);
			_attScopes.push_back(QString::fromStdString(attScope));
		}
	}
	catch (exception e)
	{
		Logger::Error("DataHelper -> SetAttScopes Exception :: " + string(e.what()));
		throw e;
	}
	catch (const char* msg)
	{
		Logger::Error("DataHelper -> SetAttScopes Exception :: " + string(msg));
		throw msg;
	}
	Logger::Info("DataHelper -> SetAttScopes() end");
}

/*
* Description - SetRefJson()  method used to prepare reference json.
* Parameter -  json, json
* Exception -
* Return - 
*/
void DataHelper::SetRefJson(json& _attributesFieldsListJson, string _defaultAttributeKey)
{
	Logger::Info("DataHelper -> SetRefJson() start");
	string refApi = "";
	try
	{
		refApi = Helper::GetRefApi(_attributesFieldsListJson, _defaultAttributeKey);
		if (FormatHelper::HasContent(refApi))
		{
			string refListResponse = RESTAPI::CentricRestCallGet(refApi, APPLICATION_JSON_TYPE, "");
			Logger::RestAPIDebug("refListResponse::" + refListResponse);

			if (FormatHelper::HasError(refListResponse))
			{
				Helper::GetCentricErrorMessage(refListResponse);
				throw runtime_error(refListResponse);
			}
			json refListResponseJson = json::parse(refListResponse);
			Helper::GetReflistJson(refListResponseJson, _attributesFieldsListJson);
		}
	}
	catch (exception e)
	{
		Logger::Error("DataHelper -> SetRefJson Exception :: " + string(e.what()));
		throw e;
	}
	catch (const char* msg)
	{
		Logger::Error("DataHelper -> SetRefJson Exception :: " + string(msg));
		throw msg;
	}
	Logger::Info("DataHelper -> SetRefJson() end");
}

/*
* Description - SetEnumJson()  method used to prepare enum  json.
* Parameter -  json, json
* Exception -
* Return - 
*/
void DataHelper::SetEnumJson(json& _attributesFieldsListJson, json _createFieldResponse)
{
	Logger::Info("DataHelper -> SetEnumJson() start");
	try
	{		
		string format = Helper::GetJSONValue<string>(_createFieldResponse, "format", true);
		string restApiName = Helper::GetJSONValue<string>(_createFieldResponse, "rest_api_name", true);
		Logger::Debug("feildsvaluesJson:format:" + format);
		string enumListIdResponse = RESTAPI::CentricRestCallGet(Configuration::GetInstance()->GetPLMServerURL() + RESTAPI::SEARCH_ENUM_ATT_API + "?skip=0&limit=" + Configuration::GetInstance()->GetMaximumLimitForRefAttValue() + "&node_name=" + format, APPLICATION_JSON_TYPE, "");
		Logger::RestAPIDebug("enumListIdResponse::" + enumListIdResponse);
		if (FormatHelper::HasError(enumListIdResponse))
		{
			Helper::GetCentricErrorMessage(enumListIdResponse);
			throw runtime_error(enumListIdResponse);
		}
		json enumListIdResponseJson = json::parse(enumListIdResponse);
		json enumListIdResponseCountJson = Helper::GetJSONParsedValue<int>(enumListIdResponseJson, 0, false);
		string enumListId = Helper::GetJSONValue<string>(enumListIdResponseCountJson, "id", true);
		Logger::Debug("enumListId" + enumListId);
		if (restApiName == Configuration::GetInstance()->GetSubTypeInternalName())
		{
			MaterialConfig::GetInstance()->SetMaterialCategoryEnumId(enumListId);
		}
		string enumListResponse = RESTAPI::CentricRestCallGet(Configuration::GetInstance()->GetPLMServerURL() + RESTAPI::SEARCH_ENUM_ATT_API + "/" + enumListId + "/values?skip=0&limit=1000", APPLICATION_JSON_TYPE, "");
		Logger::RestAPIDebug("enumListResponse::" + enumListResponse);
		if (FormatHelper::HasError(enumListResponse))
		{
			Helper::GetCentricErrorMessage(enumListResponse);
			throw runtime_error(enumListResponse);
		}
		json enumListResponseJson = json::parse(enumListResponse);
		Helper::GetEnumlistJson(enumListResponseJson, _attributesFieldsListJson);
	}
	catch (exception e)
	{
		Logger::Error("DataHelper -> SetEnumJson Exception :: " + string(e.what()));
		throw e;
	}
	catch (const char* msg)
	{
		Logger::Error("DataHelper -> SetEnumJson Exception :: " + string(msg));
		throw msg;
	}
	Logger::Info("DataHelper -> SetEnumJson() end");
}

