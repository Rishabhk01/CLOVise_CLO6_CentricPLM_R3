/*
* Copyright 2020-2021 CLO-Vise. All rights reserved
*
* @file PLMDocumentData.cpp
*
* @brief Class implementation for cach Document data in CLO from PLM.
*  This class has all the variable and mehtods implementation which are used in storing PLM Document configuration related data to search in CLO
*
* @author GoVise
*
* @date  24-JUL-2020
*/
#include "DocumentConfig.h"
#include "CLOAPIInterface.h"
#include "CLOVise/PLM/Helper/Util/RestAPIUtility.h"
#include "CLOVise/PLM/Helper/Util/FormatHelper.h"
#include "CLOVise/PLM/Helper/Util/Logger.h"
#include "CLOVise/PLM/Helper/Util//DataHelper.h"

using json = nlohmann::json;

PLMDocumentData* PLMDocumentData::_instance = NULL;
PLMDocumentData* PLMDocumentData::GetInstance()
{
	if (_instance == NULL)
	{
		_instance = new PLMDocumentData();
	}
	return _instance;
}

void  PLMDocumentData::Destroy()
{
	if (_instance)
	{
		delete _instance;
		_instance = NULL;
	}
}

/*
* Description - SetDocumentConfigJSON() method used to cache the document configuration json from rest/file.
* Parameter -  string, string, bool.
* Exception - exception, Char *
* Return -
*/
void PLMDocumentData::SetDocumentConfigJSON(string _module, string _rootType, bool _initDepedentJsons)
{
	m_documentConfigJson = DataHelper::GetInstance()->SetConfigJSON(_module, _rootType, _initDepedentJsons, PLM_DOCUMENT_FILE_NAME);
}
//void PLMDocumentData::SetDocumentConfigJSON(string _module, string _rootType, bool _initDepedentJsons)
//{
//	Logger::Info("PLMDocumentData -> SetDocumentConfigJSON start");
//	Logger::Logger(" Document Search initialiazing...");
//	json param = json::object();
//	string configJsonString = "";
//	const char* restErrorMsg = "";
//	param = RESTAPI::AddToRestParamMap(param, MODULE_KEY, _module);
//	param = RESTAPI::AddToRestParamMap(param, ROOT_TYPE_KEY, _rootType);
//
//	try
//	{
//		if (restData)
//		{
//			Logger::Info("PLMDocumentData -> if (restData)  start");
//
//			configJsonString = RESTAPI::RESTMethodSubmit(RESTAPI::SEARCH_FIELDS_API, param);
//			Logger::Debug("configJsonString---" + configJsonString);
//			if (!FormatHelper::HasContent(configJsonString))
//			{
//				throw "Unable to initiliaze Document Configuration. Please try again or Contact your System Administrator.";
//			}
//
//			if (FormatHelper::HasError(configJsonString))
//			{
//				throw runtime_error(configJsonString);
//			}
//			string error = RESTAPI::CheckForErrorMsg(configJsonString);
//			if (FormatHelper::HasContent(error))
//			{
//				throw std::logic_error(error);
//			}
//			m_documentConfigJson = json::parse(configJsonString);
//			Logger::Info("PLMDocumentData -> if (restData)  end");
//		}
//		else
//		{
//			Logger::Info("PLMDocumentData -> else  start");
//
//			m_documentConfigJson = Helper::GetDataFromJsonFile(PLM_DOCUMENT_FILE_NAME);
//			if (!FormatHelper::HasContent(to_string(m_documentConfigJson)))
//			{
//				throw "Unable to initiliaze Document Configuration. Please try again or Contact your System Administrator.";
//			}
//			Logger::Info("PLMDocumentData -> end  start");
//
//		}
//		if (_initDepedentJsons)
//		{
//			SetDocumentHierarchyJSON(HIERARCHY_JSON_KEY);
//			SetDocumentFieldsJSON(FIELDLIST_JSON_KEY);
//		}
//	}
//	catch (exception& e)
//	{
//		Logger::Error("PLMDocumentData -> SetDocumentConfigJSON Exception :: " + string(e.what()));
//		throw e;
//	}
//	catch (const char* msg)
//	{
//		Logger::Error("PLMDocumentData -> SetDocumentConfigJSON Exception :: " + string(msg));
//		throw msg;
//	}
//	Logger::Logger(" Document Search intitialization Completed.");
//	Logger::Info("PLMDocumentData -> SetDocumentConfigJSON end");
//}

/*
* Description - GetDocumentConfigJSON() method used to get document configuration json.
* Parameter - 
* Exception - 
* Return - json.
*/
json PLMDocumentData::GetDocumentConfigJSON()
{
	Logger::Info("PLMDocumentData -> GetDocumentConfigJSON() start");

	if (m_documentConfigJson == nullptr || m_documentConfigJson.empty())
		SetDocumentConfigJSON(DOCUMENT_MODULE, DOCUMENT_ROOT_TYPE, false);
	Logger::Info("PLMDocumentData -> GetDocumentConfigJSON() end");

	return m_documentConfigJson;
}

/*
* Description - SetDocumentHierarchyJSON() method used to get cached document hierarchy json.
* Parameter -  string.
* Exception - exception, Char *
* Return -
*/
//void PLMDocumentData::SetDocumentHierarchyJSON(string _hierarchyJsonKey)
//{
//	Logger::Info("PLMDocumentData -> SetDocumentHierarchyJSON() start");
//
//	try
//	{
//		string documentHierarchyStr = "";
//		documentHierarchyStr = Helper::GetJSONValue<string>(GetDocumentConfigJSON(), _hierarchyJsonKey, false);
//		m_documentHierarchyJson = json::parse(documentHierarchyStr);
//	}
//	catch (exception& e)
//	{
//		Logger::Error("PLMDocumentData -> SetDocumentHierarchyJSON Exception :: " + string(e.what()));
//		throw e;
//	}
//	catch (const char* msg)
//	{
//		Logger::Error("PLMDocumentData -> SetDocumentHierarchyJSON Exception :: " + string(msg));
//		throw msg;
//	}
//	Logger::Info("PLMDocumentData -> SetDocumentHierarchyJSON() end");
//
//}

/*
* Description - GetDocumentHierarchyJSON() method used to get cached document hierarchy json.
* Parameter - 
* Exception - 
* Return - json.
*/
json PLMDocumentData::GetDocumentHierarchyJSON()
{
	Logger::Info("PLMDocumentData -> GetDocumentHierarchyJSON() start");

	if (m_documentHierarchyJson == nullptr || m_documentHierarchyJson.empty())
		m_documentHierarchyJson = DataHelper::GetInstance()->SetJson(HIERARCHY_JSON_KEY, m_documentConfigJson);
		//SetDocumentHierarchyJSON(HIERARCHY_JSON_KEY);

	Logger::Info("PLMDocumentData -> GetDocumentHierarchyJSON() end");

	return m_documentHierarchyJson;
}

/*
* Description - SetAttScopes() method used to cache attribute scopes list.
* Parameter -  string.
* Exception - exception, Char *
* Return -
*/
//void PLMDocumentData::SetAttScopes(string _attScopesJsonKey)
//{
//	Logger::Info("PLMDocumentData -> SetAttScopes() start");
//	json attScopesArray = json::array();
//	try
//	{
//		string attScopesStr = "";
//		attScopesStr = Helper::GetJSONValue<string>(GetDocumentConfigJSON(), _attScopesJsonKey, false);
//		attScopesArray = json::parse(attScopesStr);
//		string attScope = "";
//		for (int i = 0; i < attScopesArray.size(); i++)
//		{
//			attScope = Helper::GetJSONValue<int>(attScopesArray, i, true);
//			m_attScopes.push_back(QString::fromStdString(attScope));
//		}
//	}
//	catch (exception& e)
//	{
//		Logger::Error("PLMDocumentData -> SetAttScopes Exception :: " + string(e.what()));
//		throw e;
//	}
//	catch (const char* msg)
//	{
//		Logger::Error("PLMDocumentData -> SetAttScopes Exception :: " + string(msg));
//		throw msg;
//	}
//	Logger::Info("PLMDocumentData -> SetAttScopes() end");
//}

/*
* Description - GetAttScopes() method used to get cached Att Scopes list.
* Parameter - 
* Exception - 
* Return - QStringList.
*/
QStringList PLMDocumentData::GetAttScopes()
{
	Logger::Info("PLMDocumentData -> GetAttScopes() start");

	if (m_attScopes.empty())
		DataHelper::GetInstance()->SetAttScopes(m_documentConfigJson, ATTSCOPE_JSON_KEY, m_attScopes);
		//SetAttScopes(ATTSCOPE_JSON_KEY);
	Logger::Info("PLMDocumentData -> GetAttScopes() end");

	return m_attScopes;
}

/*
* Description - SetDocumentFieldsJSON() method used to cache document fields json.
* Parameter -  string.
* Exception - exception, Char *
* Return -
*/
//void PLMDocumentData::SetDocumentFieldsJSON(string _fieldsJsonKey)
//{
//	Logger::Info("PLMDocumentData -> SetDocumentFieldsJSON() start");
//
//	try
//	{
//		string documentFieldsStr = "";
//		documentFieldsStr = Helper::GetJSONValue<string>(GetDocumentConfigJSON(), _fieldsJsonKey, false);
//		m_documentFieldsJson = json::parse(documentFieldsStr);
//	}
//	catch (exception& e)
//	{
//		Logger::Error("PLMDocumentData -> SetDocumentFieldsJSON Exception :: " + string(e.what()));
//		throw e;
//	}
//	catch (const char* msg)
//	{
//		Logger::Error("PLMDocumentData -> SetDocumentFieldsJSON Exception :: " + string(msg));
//		throw msg;
//	}
//	Logger::Info("PLMDocumentData -> SetDocumentFieldsJSON() end");
//
//}

/*
* Description - GetDocumentFieldsJSON() method used to get cached the document fields json.
* Parameter - 
* Exception - 
* Return - json.
*/
json PLMDocumentData::GetDocumentFieldsJSON()
{
	Logger::Info("PLMDocumentData -> GetDocumentFieldsJSON() start");

	if (m_documentFieldsJson == nullptr || m_documentFieldsJson.empty())
		m_documentFieldsJson = DataHelper::GetInstance()->SetJson(FIELDLIST_JSON_KEY, m_documentConfigJson);
		//SetDocumentFieldsJSON(FIELDLIST_JSON_KEY);
	Logger::Info("PLMDocumentData -> GetDocumentFieldsJSON() end");

	return m_documentFieldsJson;
}

/*
* Description - SetDocumentPresetJSON() method used to cache the document preset json.
* Parameter -  string.
* Exception - exception, Char *
* Return -
* no where used, need to remove.
*/
void PLMDocumentData::SetDocumentPresetJSON(string _presetJsonKey)
{
	Logger::Info("PLMDocumentData -> SetDocumentPresetJSON() start");

	try
	{
		string documentPresetStr = "";
		documentPresetStr = Helper::GetJSONValue<string>(GetDocumentConfigJSON(), _presetJsonKey, false);
		m_documentPresetJson = json::parse(documentPresetStr);
	}
	catch (exception& e)
	{
		Logger::Error("PLMDocumentData -> SetDocumentPresetJSON Exception :: " + string(e.what()));
		throw e;
	}
	catch (const char* msg)
	{
		Logger::Error("PLMDocumentData -> SetDocumentPresetJSON Exception :: " + string(msg));
		throw msg;
	}
	Logger::Info("PLMDocumentData -> SetDocumentPresetJSON() end");

}

/*
* Description - GetDocumentPresetJSON() method used get cached document preset json.
* Parameter - 
* Exception - 
* Return - json.
*/
json PLMDocumentData::GetDocumentPresetJSON()
{
	Logger::Info("PLMDocumentData -> GetDocumentPresetJSON() start");

	if (m_documentPresetJson == nullptr || m_documentPresetJson.empty())
		SetDocumentPresetJSON(PRESETLIST_JSON_KEY);
	Logger::Info("PLMDocumentData -> GetDocumentPresetJSON() end");

	return m_documentPresetJson;
}

/*
* Description - SetDocumentFilterJSON() method used to cache the Document Filter JSON from rest/file.
* Parameter -  string, string, string.
* Exception - exception, Char *
* Return -
*/
void PLMDocumentData::SetDocumentFilterJSON(string _module, string _rootType, string _activity)
{
	m_documentFilterJson = DataHelper::GetInstance()->SetFilterJSON(_module, _rootType, _activity, PLM_DOCUMENT_FILTER_FILE);
}
//void PLMDocumentData::SetDocumentFilterJSON(string _module, string _rootType, string _activity)
//{
//	Logger::Info("PLMDocumentData -> SetDocumentFilterJSON() start");
//
//	json param = json::object();
//	string filterJsonString = "";
//	const char* restErrorMsg = "";
//	param = RESTAPI::AddToRestParamMap(param, MODULE_KEY, _module);
//	param = RESTAPI::AddToRestParamMap(param, ROOT_TYPE_KEY, _rootType);
//	param = RESTAPI::AddToRestParamMap(param, ACTIVITY_KEY, _activity);
//
//	try
//	{
//		if (restData)
//		{
//			filterJsonString = RESTAPI::RESTMethodSubmit(RESTAPI::FILTER_API, param);
//			if (!FormatHelper::HasContent(filterJsonString))
//			{
//				throw "Unable to initiliaze Document Configuration. Please try again or Contact your System Administrator.";
//			}
//
//			if (FormatHelper::HasError(filterJsonString))
//			{
//				throw runtime_error(filterJsonString);
//			}
//			
//			string error = RESTAPI::CheckForErrorMsg(filterJsonString);
//			if (FormatHelper::HasContent(error))
//			{
//				throw std::logic_error(error);
//			}
//			m_documentFilterJson = Helper::GetJSONParsedValue<string>(json::parse(filterJsonString), FILTERSLIST_JSON_KEY, false);
//		}
//		else
//		{
//			m_documentFilterJson = Helper::GetJSONParsedValue<string>(Helper::GetDataFromJsonFile(PLM_DOCUMENT_FILTER_FILE), FILTERSLIST_JSON_KEY, false);
//			if (!FormatHelper::HasContent(to_string(m_documentFilterJson)))
//			{
//				throw "Unable to initiliaze Document Configuration. Please try again or Contact your System Administrator.";
//			}
//		}
//	}
//	catch (exception& e)
//	{
//		Logger::Error("PLMDocumentData -> SetDocumentFilterJSON Exception :: " + string(e.what()));
//		throw e;
//	}
//	catch (const char* msg)
//	{
//		Logger::Error("PLMDocumentData -> SetDocumentFilterJSON Exception :: " + string(msg));
//		throw msg;
//	}
//	Logger::Info("PLMDocumentData -> SetDocumentFilterJSON() start");
//
//}

/*
* Description - GetDocumentFilterJSON() method used to get cached document filter json.
* Parameter -
* Exception - 
* Return - json.
*/
json PLMDocumentData::GetDocumentFilterJSON()
{
	Logger::Info("PLMDocumentData -> GetDocumentFilterJSON() start");

	if (m_documentFilterJson == nullptr || m_documentFilterJson.empty())
		SetDocumentFilterJSON(DOCUMENT_MODULE, DOCUMENT_ROOT_TYPE, DOCUMENT_SEARCH_ACTIVITY);
	Logger::Info("PLMDocumentData -> GetDocumentFilterJSON() end");

	return m_documentFilterJson;
}

/*
* Description - SetDocumentViewJSON() method used to cache the document view json from rest/file.
* Parameter -  string, string, string.
* Exception - exception, Char *
* Return -
*/
void PLMDocumentData::SetDocumentViewJSON(string _module, string _rootType, string _activity)
{
	m_documentViewJson = DataHelper::GetInstance()->SetViewJSON(_module, _rootType, _activity, PLM_DOCUMENT_VIEW_FILE);
}
//void PLMDocumentData::SetDocumentViewJSON(string _module, string _rootType, string _activity)
//{
//	Logger::Info("PLMDocumentData -> SetDocumentViewJSON() start");
//
//	json param = json::object();
//	string viewJsonString = "";
//	const char* restErrorMsg = "";
//	param = RESTAPI::AddToRestParamMap(param, MODULE_KEY, _module);
//	param = RESTAPI::AddToRestParamMap(param, ROOT_TYPE_KEY, _rootType);
//	param = RESTAPI::AddToRestParamMap(param, ACTIVITY_KEY, _activity);
//
//	try
//	{
//		if (restData)
//		{
//			viewJsonString = RESTAPI::RESTMethodSubmit(RESTAPI::VIEW_API, param);
//			if (!FormatHelper::HasContent(viewJsonString))
//			{
//				throw "Unable to initiliaze Document Configuration. Please try again or Contact your System Administrator.";
//			}
//
//			if (FormatHelper::HasError(viewJsonString))
//			{
//				throw runtime_error(viewJsonString);
//			}
//			string error = RESTAPI::CheckForErrorMsg(viewJsonString);
//			if (FormatHelper::HasContent(error))
//			{
//				throw std::logic_error(error);
//			}
//			m_documentViewJson = Helper::GetJSONParsedValue<string>(json::parse(viewJsonString), VIEWS_LIST_JSON_KEY, false);
//		}
//		else
//		{
//			m_documentViewJson = Helper::GetJSONParsedValue<string>(Helper::GetDataFromJsonFile(PLM_DOCUMENT_VIEW_FILE), VIEWS_LIST_JSON_KEY, false);
//			if (!FormatHelper::HasContent(to_string(m_documentViewJson)))
//			{
//				throw "Unable to initiliaze Document Configuration. Please try again or Contact your System Administrator.";
//			}
//		}
//	}
//	catch (exception& e)
//	{
//		Logger::Error("PLMDocumentData -> SetDocumentViewJSON Exception :: " + string(e.what()));
//		throw e;
//	}
//	catch (const char* msg)
//	{
//		Logger::Error("PLMDocumentData -> SetDocumentViewJSON Exception :: " + string(msg));
//		throw msg;
//	}
//	Logger::Info("PLMDocumentData -> SetDocumentViewJSON() end");
//}

/*
* Description - GetDocumentViewJSON() method used to get cached document view json.
* Parameter - 
* Exception - 
* Return - json.
*/
json PLMDocumentData::GetDocumentViewJSON()
{
	Logger::Info("PLMDocumentData -> GetDocumentViewJSON() start");

	if (m_documentViewJson == nullptr || m_documentViewJson.empty())
		SetDocumentViewJSON(DOCUMENT_MODULE, DOCUMENT_ROOT_TYPE, DOCUMENT_SEARCH_ACTIVITY);
	Logger::Info("PLMDocumentData -> GetDocumentViewJSON() end");

	return m_documentViewJson;
}

/*
* Description - SetSeasonPaletteJSON() method used to cache the Season Palette JSON from rest/file.
* Parameter - 
* Exception - exception, Char *
* Return -
*/
void PLMDocumentData::SetSeasonPaletteJSON()
{
	Logger::Info("PLMDocumentData -> SetSeasonPaletteJSON() start");

	json param = json::object();
	string paletteJsonString = "";
	const char* restErrorMsg = "";

	try
	{
		if (restData)
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

			m_paletteJson = json::parse(paletteJsonString);
		}
		else
		{
			m_paletteJson = Helper::GetDataFromJsonFile(PLM_DOCUMENT_PALETTE_FILE);
			if (!FormatHelper::HasContent(to_string(m_paletteJson)))
			{
				throw "Unable to initiliaze Document Configuration. Please try again or Contact your System Administrator.";
			}
		}
	}
	catch (exception& e)
	{
		Logger::Error("PLMDocumentData -> SetSeasonPaletteJSON Exception :: " + string(e.what()));
		throw e;
	}
	catch (const char* msg)
	{
		Logger::Error("PLMDocumentData -> SetSeasonPaletteJSON Exception :: " + string(msg));
		throw msg;
	}
	Logger::Info("PLMDocumentData -> SetSeasonPaletteJSON() end");

}

/*
* Description - GetSeasonPaletteJSON() method used to get cached the season palette json.
* Parameter -  bool.
* Exception - 
* Return - json.
*/
json PLMDocumentData::GetSeasonPaletteJSON(bool _refresh)
{
	Logger::Info("PLMDocumentData -> GetSeasonPaletteJSON() start");

	if (_refresh)
	{
		m_paletteJson = DataHelper::GetInstance()->SetSeasonPaletteJSON(PLM_DOCUMENT_PALETTE_FILE);
	}
	Logger::Info("PLMDocumentData -> GetSeasonPaletteJSON() end");

	return m_paletteJson;
}

/*
* Description - SetSelectedViewIdx() method used to cache the selected view index.
* Parameter -  int.
* Exception - 
* Return -
*/
void PLMDocumentData::SetSelectedViewIdx(int _selectedViewIdx)
{
	Logger::Info("PLMDocumentData -> SetSelectedViewIdx() Start");
	m_selectedViewIdx = _selectedViewIdx;
	Logger::Info("PLMDocumentData -> SetSelectedViewIdx() End");
}

/*
* Description - GetSelectedViewIdx() method used to get cached selected view index.
* Parameter - 
* Exception - 
* Return - int.
*/
int PLMDocumentData::GetSelectedViewIdx()
{
	return m_selectedViewIdx;
}

/*
* Description - SetSearchCriteriaJSON() method used to cache the search criteria json.
* Parameter -  json.
* Exception - 
* Return -
*/
void PLMDocumentData::SetSearchCriteriaJSON(json _criteria)
{
	Logger::Info("PLMDocumentData -> SetSelectedViewIdx() Start");
	m_searchCriteriaJson = _criteria;
	Logger::Info("PLMDocumentData -> SetSelectedViewIdx() End");
}

/*
* Description - GetSearchCriteriaJSON() method used to get cached search criteria json.
* Parameter - 
* Exception - 
* Return - json.
*/
json PLMDocumentData::GetSearchCriteriaJSON()
{
	return m_searchCriteriaJson;
}

/*
* Description - Set3DModelFileMetaDataJSONList() method used to cache the json list.
* Parameter -  json.
* Exception -
* Return -
*/
void PLMDocumentData::Set3DModelFileMetaDataJSONList(json _json)
{
	m_metadataJSONList = _json;
}

/*
* Description - Get3DModelFileMetaDataJSONList() method used to get cached data.
* Parameter - 
* Exception - 
* Return - json
*/
json PLMDocumentData::Get3DModelFileMetaDataJSONList()
{
	return m_metadataJSONList;
}

/*
* Description - SetProductIdKeyExists() method used to cache product id key is exist or not.
* Parameter -  json.
* Exception - exception, Char *
* Return -
*/
void PLMDocumentData::SetProductIdKeyExists(json _json)
{
	Logger::Info("PLMDocumentData -> SetProductIdKeyExists() Start");
	try
	{
		m_productIdKeyExists = false;
		string productIdKeyExists = Helper::GetJSONValue<string>(_json, PRODUCT_ID_EXIST_KEY, true);
		if (FormatHelper::HasContent(productIdKeyExists))
		{
			if (productIdKeyExists == "true") 
			{
				m_productIdKeyExists = true;
			}
		}
	}
	catch (exception& e)
	{
		Logger::Error("PLMDocumentData -> SetProductIdKeyExists Exception :: " + string(e.what()));
		throw e;
	}
	catch (const char* msg)
	{
		Logger::Error("PLMDocumentData -> SetProductIdKeyExists Exception :: " + string(msg));
		throw msg;
	}
	Logger::Info("PLMDocumentData -> SetProductIdKeyExists() End");
}

/*
* Description - GetProductIdKeyExists() method used to get cached data.
* Parameter - 
* Exception - 
* Return - bool.
*/
bool PLMDocumentData::GetProductIdKeyExists()
{
	Logger::Info("PLMDocumentData -> GetProductIdKeyExists() Start");
	return m_productIdKeyExists;
	Logger::Info("PLMDocumentData -> GetProductIdKeyExists() End");
}

/*
* Description - SetDateFlag() method used to set date exist flag.
* Parameter -  bool.
* Exception -
* Return -
*/
void PLMDocumentData::SetDateFlag(bool _isDateEditPresent)
{
	m_isDateEditPresent = _isDateEditPresent;
}

/*
* Description - GetDateFlag() method used to get date exist flag.
* Parameter -
* Exception -
* Return - bool.
*/
bool PLMDocumentData::GetDateFlag()
{
	return m_isDateEditPresent;
}

/*
* Description - InitializeDocumentData() method used to initialize document data from configure response from rest/file.
* Parameter -
* Exception -
* Return -
*/
void PLMDocumentData::InitializeDocumentData()
{
	RESTAPI::SetProgressBarData(20, "Loading Document configuration", true);
	GetDocumentConfigJSON();
	SetProductIdKeyExists(GetDocumentConfigJSON());
	RESTAPI::SetProgressBarData(40, "Loading Document configuration", true);
	GetDocumentHierarchyJSON();
	GetDocumentFieldsJSON();
	RESTAPI::SetProgressBarData(60, "Loading Document configuration", true);
	GetAttScopes();
	GetDocumentFilterJSON();
	RESTAPI::SetProgressBarData(80, "Loading Document configuration", true);
	GetDocumentViewJSON();
	Configuration::GetInstance()->SetSupportedAttsList(GetDocumentConfigJSON());
	RESTAPI::SetProgressBarData(90, "Loading Document configuration", true);
	Configuration::GetInstance()->SetResultsSupportedAttsList(GetDocumentConfigJSON());
	RESTAPI::SetProgressBarData(100, "Loading Document configuration", true);
	RESTAPI::SetProgressBarData(0, "", false);
}