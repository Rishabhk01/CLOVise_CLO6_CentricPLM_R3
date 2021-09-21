/*
* Copyright 2020-2021 CLO-Vise. All rights reserved
*
* @file ProductConfig.cpp
*
* @brief Class implementation for cach Product data in CLO from PLM.
* This class has all the variable and methods implementation which are used in storing PLM Product configuration related data to search in CLO
*
* @author GoVise
*
* @date 28-JUL-2020
*/
#include "ProductConfig.h"
#include "CLOAPIInterface.h"
#include "CLOVise/PLM/Helper/Util/RestAPIUtility.h"
#include "CLOVise/PLM/Helper/Util/FormatHelper.h"
#include "CLOVise/PLM/Outbound/PublishToPLM/CopyProduct.h"
#include "CLOVise/PLM/Outbound/PublishToPLM/PublishToPLMData.h"
#include "CLOVise/PLM/Helper/Util/Logger.h"
#include "CLOVise/PLM/Helper/Util/DataHelper.h"
#include "CLOVise/PLM/Inbound/Product/PLMProductResults.h"
#include "CLOVise/PLM/Helper/UIHelper/UIHelper.h"

using json = nlohmann::json;

ProductConfig* ProductConfig::_instance = NULL;
ProductConfig* ProductConfig::GetInstance()
{
	if (_instance == NULL)
	{
		_instance = new ProductConfig();
	}
	return _instance;
}
void  ProductConfig::Destroy()
{
	if (_instance)
	{
		delete _instance;
		_instance = NULL;
	}
}

/*
* Description - SetProductConfigJSON() method used to cache the Product configuration json from rest/file.
* Parameter -  string, string, bool.
* Exception - exception, Char *
* Return -
*/
void ProductConfig::SetProductConfigJSONFromFile(string _module, string _rootType, bool _initDepedentJsons)
{
	Logger::Info("ProductConfig -> SetProductConfigJSON() start");
	m_productConfigJson = DataHelper::GetInstance()->SetConfigJSON(_module, _rootType, _initDepedentJsons, PLM_STYLE_FILE_NAME);
	Logger::Info("ProductConfig -> SetProductConfigJSON() end");
}
//void ProductConfig::SetProductConfigJSON(string _module, string _rootType, bool _initDepedentJsons)
//{
//	Logger::Info("ProductConfig -> SetProductConfigJSON start");
//	Logger::Logger(" Product Search initialiazing...");
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
//			Logger::Info("ProductConfig -> if (restData)  start");
//
//			configJsonString = RESTAPI::RESTMethodSubmit(RESTAPI::SEARCH_FIELDS_API, param);
//			Logger::Debug("configJsonString---" + configJsonString);
//			if (!FormatHelper::HasContent(configJsonString))
//			{
//				throw "Unable to initiliaze Product Configuration. Please try again or Contact your System Administrator.";
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
//			m_productConfigJson = json::parse(configJsonString);
//			Logger::Info("ProductConfig -> if (restData)  end");
//		}
//		else
//		{
//			Logger::Info("ProductConfig -> else  start");
//
//			m_productConfigJson = Helper::GetDataFromJsonFile(PLM_PRODUCT_FILE_NAME);
//			if (!FormatHelper::HasContent(to_string(m_productConfigJson)))
//			{
//				throw "Unable to initiliaze Product Configuration. Please try again or Contact your System Administrator.";
//			}
//			Logger::Info("ProductConfig -> end  start");
//
//		}
//		if (_initDepedentJsons)
//		{
//			SetProductHierarchyJSON(HIERARCHY_JSON_KEY);
//			SetProductFieldsJSON(FIELDLIST_JSON_KEY);
//		}
//	}
//	catch (exception& e)
//	{
//		Logger::Error("ProductConfig -> SetProductConfigJSON Exception :: " + string(e.what()));
//		throw e;
//	}
//	catch (const char* msg)
//	{
//		Logger::Error("ProductConfig -> SetProductConfigJSON Exception :: " + string(msg));
//		throw msg;
//	}
//	Logger::Logger("Product search intitialization completed.");
//	Logger::Info("ProductConfig -> SetProductConfigJSON end");
//}

/*
* Description - GetProductConfigJSON() method used to get Product configuration json.
* Parameter -
* Exception -
* Return - json.
*/
json ProductConfig::GetProductConfigJSON()
{
	Logger::Info("ProductConfig -> GetProductConfigJSON() start");

	if (m_productFieldsJson == nullptr || m_productFieldsJson.empty())
		SetProductConfigJSON();
	Logger::Info("ProductConfig -> GetProductConfigJSON() end");

	return m_productFieldsJson;
}

/*
* Description - GetProductConfigJSON() method used to get Product configuration json.
* Parameter -
* Exception -
* Return - json.
*/
json ProductConfig::GetProductConfigJSONFromFile()
{
	Logger::Info("ProductConfig -> GetProductConfigJSON() start");

	if (m_productConfigJson == nullptr || m_productConfigJson.empty())
		SetProductConfigJSONFromFile(STYLE_MODULE, STYLE_ROOT_TYPE, false);
	Logger::Info("ProductConfig -> GetProductConfigJSON() end");

	return m_productConfigJson;
}

/*
* Description - SetProductHierarchyJSON() method used to get cached Product Hierarchy JSON.
* Parameter -  string.
* Exception - exception, Char *
* Return -
*/
//void ProductConfig::SetProductHierarchyJSON(string _hierarchyJsonKey)
//{
//	Logger::Info("ProductConfig -> SetProductHierarchyJSON() start");
//
//	try
//	{
//		string productHierarchyStr = "";
//		productHierarchyStr = Helper::GetJSONValue<string>(GetProductConfigJSON(), _hierarchyJsonKey, false);
//		m_productHierarchyJson = json::parse(productHierarchyStr);
//	}
//	catch (exception& e)
//	{
//		Logger::Error("ProductConfig -> SetProductHierarchyJSON Exception :: " + string(e.what()));
//		throw e;
//	}
//	catch (const char* msg)
//	{
//		Logger::Error("ProductConfig -> SetProductHierarchyJSON Exception :: " + string(msg));
//		throw msg;
//	}
//	Logger::Info("ProductConfig -> SetProductHierarchyJSON() end");
//
//}

/*
* Description - GetProductHierarchyJSON() method used to get cached Product Hierarchy JSON.
* Parameter -
* Exception -
* Return - json.
*/
json ProductConfig::GetProductHierarchyJSON()
{
	Logger::Info("ProductConfig -> GetProductHierarchyJSON() start");

	if (m_productHierarchyJson == nullptr || m_productHierarchyJson.empty())
		m_productHierarchyJson = DataHelper::GetInstance()->SetJson(HIERARCHY_JSON_KEY, m_productConfigJson);
		//SetProductHierarchyJSON(HIERARCHY_JSON_KEY);

	Logger::Info("ProductConfig -> GetProductHierarchyJSON() end");

	return m_productHierarchyJson;
}

/*
* Description - SetAttScopes() method used to cache attribute scopes list.
* Parameter -  string.
* Exception - exception, Char *
* Return -
*/
//void ProductConfig::SetAttScopes(string _attScopesJsonKey)
//{
//	Logger::Info("ProductConfig -> SetAttScopes() start");
//	json attScopesArray = json::array();
//	try
//	{
//		string attScopesStr = "";
//		attScopesStr = Helper::GetJSONValue<string>(GetProductConfigJSON(), _attScopesJsonKey, false);
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
//		Logger::Error("ProductConfig -> SetAttScopes Exception :: " + string(e.what()));
//		throw e;
//	}
//	catch (const char* msg)
//	{
//		Logger::Error("ProductConfig -> SetAttScopes Exception :: " + string(msg));
//		throw msg;
//	}
//	Logger::Info("ProductConfig -> SetAttScopes() end");
//}

/*
* Description - GetAttScopes() method used to get cached attribute scopes list.
* Parameter -
* Exception -
* Return - QStringList.
*/
QStringList ProductConfig::GetAttScopes()
{
	Logger::Info("ProductConfig -> GetAttScopes() start");

	if (m_attScopes.empty())
		DataHelper::GetInstance()->SetAttScopes(m_productConfigJson, ATTSCOPE_JSON_KEY, m_attScopes);
		//SetAttScopes(ATTSCOPE_JSON_KEY);
	Logger::Info("ProductConfig -> GetAttScopes() end");

	return m_attScopes;
}

/*
* Description - SetProductFieldsJSON() method used to cache product fields json.
* Parameter -  string.
* Exception - exception, Char *
* Return -
*/
//void ProductConfig::SetProductFieldsJSON(string _fieldsJsonKey)
//{
//	Logger::Info("ProductConfig -> SetProductFieldsJSON() start");
//
//	try
//	{
//		string productFieldsStr = "";
//		productFieldsStr = Helper::GetJSONValue<string>(GetProductConfigJSON(), _fieldsJsonKey, false);
//		m_productFieldsJson = json::parse(productFieldsStr);
//	}
//	catch (exception& e)
//	{
//		Logger::Error("ProductConfig -> SetProductFieldsJSON Exception :: " + string(e.what()));
//		throw e;
//	}
//	catch (const char* msg)
//	{
//		Logger::Error("ProductConfig -> SetProductFieldsJSON Exception :: " + string(msg));
//		throw msg;
//	}
//	Logger::Info("ProductConfig -> SetProductFieldsJSON() end");
//
//}

/*
* Description - GetProductFieldsJSON() method used to get cached the product fields json.
* Parameter -
* Exception -
* Return - json.
*/
json ProductConfig::GetProductFieldsJSON()
{
	Logger::Info("ProductConfig -> GetProductFieldsJSON() start");

	if (m_productFieldsJson == nullptr || m_productFieldsJson.empty())
		SetProductConfigJSON();
	Logger::Info("ProductConfig -> GetProductFieldsJSON() end");

	return m_productFieldsJson;
}

/*
* Description - SetDocumentPresetJSON() method used to cache the product preset json.
* Parameter -  string.
* Exception - exception, Char *
* Return -
* no where used, need to remove.
*/
void ProductConfig::SetProductPresetJSON(string _presetJsonKey)
{
	Logger::Info("ProductConfig -> SetProductPresetJSON() start");

	try
	{
		string productPresetStr = "";
		productPresetStr = Helper::GetJSONValue<string>(GetProductConfigJSON(), _presetJsonKey, false);
		m_productPresetJson = json::parse(productPresetStr);
	}
	catch (string msg)
	{
		Logger::Error("ProductConfig -> SetProductPresetJSON() Exception :: " + msg);
		throw msg;
	}
	catch (exception& e)
	{
		Logger::Error("ProductConfig -> SetProductPresetJSON() Exception :: " + string(e.what()));
		throw e;
	}
	catch (const char* msg)
	{
		Logger::Error("ProductConfig -> SetProductPresetJSON() Exception :: " + string(msg));
		throw msg;
	}
	Logger::Info("ProductConfig -> SetProductPresetJSON() end");

}

/*
* Description - GetProductPresetJSON() method used get cached product preset json.
* Parameter -
* Exception -
* Return - json.
*/
json ProductConfig::GetProductPresetJSON()
{
	Logger::Info("ProductConfig -> GetProductPresetJSON() start");

	if (m_productPresetJson == nullptr || m_productPresetJson.empty())
		SetProductPresetJSON(PRESETLIST_JSON_KEY);
	Logger::Info("ProductConfig -> GetProductPresetJSON() end");

	return m_productPresetJson;
}

/*
* Description - SetProductFilterJSON() method used to cache the product filter json from rest/file.
* Parameter -  string, string, string.
* Exception - exception, Char *
* Return -
*/
void ProductConfig::SetProductFilterJSON(string _module, string _rootType, string _activity)
{
	Logger::Info("ProductConfig -> SetProductFilterJSON() end");
	m_productFilterJson = DataHelper::GetInstance()->SetFilterJSON(_module, _rootType, _activity, PLM_PRODUCT_FILTER_FILE);
	Logger::Info("ProductConfig -> SetProductFilterJSON() end");
}
//void ProductConfig::SetProductFilterJSON(string _module, string _rootType, string _activity)
//{
//	Logger::Info("ProductConfig -> SetProductFilterJSON() start");
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
//				throw "Unable to initiliaze Product Configuration. Please try again or Contact your System Administrator.";
//			}
//
//			if (FormatHelper::HasError(filterJsonString))
//			{
//				throw runtime_error(filterJsonString);
//			}
//			string error = RESTAPI::CheckForErrorMsg(filterJsonString);
//			if (FormatHelper::HasContent(error))
//			{
//				throw std::logic_error(error);
//			}
//			m_productFilterJson = Helper::GetJSONParsedValue<string>(json::parse(filterJsonString), FILTERSLIST_JSON_KEY, false);
//		}
//		else
//		{
//			m_productFilterJson = Helper::GetJSONParsedValue<string>(Helper::GetDataFromJsonFile(PLM_PRODUCT_FILTER_FILE), FILTERSLIST_JSON_KEY, false);
//			if (!FormatHelper::HasContent(to_string(m_productFilterJson)))
//			{
//				throw "Unable to initiliaze Product Configuration. Please try again or Contact your System Administrator.";
//			}
//		}
//	}
//	catch (exception& e)
//	{
//		Logger::Error("ProductConfig -> SetProductFilterJSON Exception :: " + string(e.what()));
//		throw e;
//	}
//	catch (const char* msg)
//	{
//		Logger::Error("ProductConfig -> SetProductFilterJSON Exception :: " + string(msg));
//		throw msg;
//	}
//	Logger::Info("ProductConfig -> SetProductFilterJSON() start");
//
//}

/*
* Description - GetProductFilterJSON() method used to get cached product filter json.
* Parameter -
* Exception -
* Return - json.
*/
json ProductConfig::GetProductFilterJSON()
{
	Logger::Info("ProductConfig -> GetProductFilterJSON() start");

	if (m_productFilterJson == nullptr || m_productFilterJson.empty())
		SetProductFilterJSON(PRODUCT_MODULE, PRODUCT_ROOT_TYPE, PRODUCT_SEARCH_ACTIVITY);
	Logger::Info("ProductConfig -> GetProductFilterJSON() end");

	return m_productFilterJson;
}

/*
* Description - SetProductViewJSON() method used to cache the product view json from rest/file.
* Parameter -  string, string, string.
* Exception - exception, Char *
* Return -
*/
void ProductConfig::SetProductViewJSON(string _module, string _rootType, string _activity)
{
	Logger::Info("ProductConfig -> SetProductViewJSON() end");
	m_productViewJson = DataHelper::GetInstance()->SetViewJSON(_module, _rootType, _activity, PLM_STYLE_VIEW_FILE);
	Logger::Info("ProductConfig -> SetProductViewJSON() end");
}
//void ProductConfig::SetProductViewJSON(string _module, string _rootType, string _activity)
//{
//	Logger::Info("ProductConfig -> SetProductViewJSON() start");
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
//				throw "Unable to initiliaze Product Configuration. Please try again or Contact your System Administrator.";
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
//			m_productViewJson = Helper::GetJSONParsedValue<string>(json::parse(viewJsonString), VIEWS_LIST_JSON_KEY, false);
//		}
//		else
//		{
//			m_productViewJson = Helper::GetJSONParsedValue<string>(Helper::GetDataFromJsonFile(PLM_PRODUCT_VIEW_FILE), VIEWS_LIST_JSON_KEY, false);
//			if (!FormatHelper::HasContent(to_string(m_productViewJson)))
//			{
//				throw "Unable to initiliaze Product Configuration. Please try again or Contact your System Administrator.";
//			}
//		}
//	}
//	catch (exception& e)
//	{
//		Logger::Error("ProductConfig -> SetProductViewJSON Exception :: " + string(e.what()));
//		throw e;
//	}
//	catch (const char* msg)
//	{
//		Logger::Error("ProductConfig -> SetProductViewJSON Exception :: " + string(msg));
//		throw msg;
//	}
//	Logger::Info("ProductConfig -> SetProductViewJSON() end");
//}*/

/*
* Description - GetProductViewJSON() method used to get cached product view json.
* Parameter -
* Exception -
* Return - json.
*/
json ProductConfig::GetProductViewJSON()
{
	Logger::Info("ProductConfig -> GetProductViewJSON() start");

	if (m_productViewJson == nullptr || m_productViewJson.empty())
		SetProductViewJSON(STYLE_MODULE, STYLE_ROOT_TYPE, STYLE_SEARCH_ACTIVITY);
	Logger::Info("ProductConfig -> GetProductViewJSON() end");

	return m_productViewJson;
}

/*
* Description - SetSeasonPaletteJSON() method used to cache the season palette json from rest/file.
* Parameter -
* Exception - exception, Char *
* Return -
*/
//void ProductConfig::SetSeasonPaletteJSON()
//{
//	Logger::Info("ProductConfig -> SetSeasonPaletteJSON() start");
//
//	json param = json::object();
//	string paletteJsonString = "";
//	const char* restErrorMsg = "";
//
//	try
//	{
//		if (restData)
//		{
//			paletteJsonString = RESTAPI::RESTMethodSubmit(RESTAPI::PALETTE_SEARCH_API, param);
//			if (!FormatHelper::HasContent(paletteJsonString))
//			{
//				throw "Unable to initiliaze Season/Palette Configuration. Please try again or Contact your System Administrator.";
//			}
//
//			if (FormatHelper::HasError(paletteJsonString))
//			{
//				throw runtime_error(paletteJsonString);
//			}
//			string error = RESTAPI::CheckForErrorMsg(paletteJsonString);
//			if (FormatHelper::HasContent(error))
//			{
//				throw std::logic_error(error);
//			}
//			m_paletteJson = json::parse(paletteJsonString);
//		}
//		else
//		{
//			m_paletteJson = Helper::GetDataFromJsonFile(PLM_PRODUCT_PALETTE_FILE);
//			if (!FormatHelper::HasContent(to_string(m_paletteJson)))
//			{
//				throw "Unable to initiliaze Product Configuration. Please try again or Contact your System Administrator.";
//			}
//		}
//	}
//	catch (exception& e)
//	{
//		Logger::Error("ProductConfig -> SetSeasonPaletteJSON Exception :: " + string(e.what()));
//		throw e;
//	}
//	catch (const char* msg)
//	{
//		Logger::Error("ProductConfig -> SetSeasonPaletteJSON Exception :: " + string(msg));
//		throw msg;
//	}
//	Logger::Info("ProductConfig -> SetSeasonPaletteJSON() end");
//
//}

/*
* Description - GetSeasonPaletteJSON() method used to get cached the season palette json.
* Parameter -  bool.
* Exception -
* Return - json.
*/
json ProductConfig::GetSeasonPaletteJSON(bool _refresh)
{
	Logger::Info("ProductConfig -> GetSeasonPaletteJSON() start");

	if (_refresh)
	{
		m_paletteJson = DataHelper::GetInstance()->SetSeasonPaletteJSON(PLM_PRODUCT_PALETTE_FILE);
	}
	Logger::Info("ProductConfig -> GetSeasonPaletteJSON() end");

	return m_paletteJson;
}

/*
* Description - SetSelectedViewIdx() method used to cache the selected view index.
* Parameter -  int.
* Exception -
* Return -
*/
void ProductConfig::SetSelectedViewIdx(int _selectedViewIdx)
{
	Logger::Info("ProductConfig -> SetSelectedViewIdx() Start");
	m_selectedViewIdx = _selectedViewIdx;
	Logger::Info("ProductConfig -> SetSelectedViewIdx() End");
}

/*
* Description - GetSelectedViewIdx() method used to get cached selected view index.
* Parameter -
* Exception -
* Return - int.
*/
int ProductConfig::GetSelectedViewIdx()
{
	return m_selectedViewIdx;
}

/*
* Description - SetSearchCriteriaJSON() method used to cache the search criteria json.
* Parameter -  json.
* Exception -
* Return -
*/
void ProductConfig::SetSearchCriteriaJSON(json _criteria)
{
	Logger::Info("ProductConfig -> SetSelectedViewIdx() Start");
	m_searchCriteriaJson = _criteria;
	Logger::Info("ProductConfig -> SetSelectedViewIdx() End");
}

/*
* Description - GetSearchCriteriaJSON() method used to get cached search criteria json.
* Parameter -
* Exception -
* Return - json.
*/
json ProductConfig::GetSearchCriteriaJSON()
{
	return m_searchCriteriaJson;
}

/*
* Description - SetDateFlag() method used to set date exist flag.
* Parameter -  bool.
* Exception -
* Return -
*/
void ProductConfig::SetDateFlag(bool _isDateEditPresent)
{
	Logger::Info("ProductConfig -> SetDateFlag() end");
	m_isDateEditPresent = _isDateEditPresent;
	Logger::Info("ProductConfig -> SetDateFlag() end");
}

/*
* Description - GetDateFlag() method used to get date exist flag.
* Parameter -
* Exception -
* Return - bool.
*/
bool ProductConfig::GetDateFlag()
{
	return m_isDateEditPresent;
}

/*
* Description - SetDownloadFileName() method used to cache downloaded file name.
* Parameter -  string.
* Exception -
* Return -
*/
void ProductConfig::SetDownloadFileName(string _downloadFileName)
{
	m_downloadFileName = _downloadFileName;
}

/*
* Description - GetDownloadFileName() method used to get cached downloaded file name.
* Parameter -
* Exception -
* Return - string.
*/
string  ProductConfig::GetDownloadFileName()
{
	return m_downloadFileName;
}

void  ProductConfig::UpdateResultJson(json& _materialResults, json& _materialTypeValues)
{
	try
	{
		string objectName = Helper::GetJSONValue<string>(_materialResults, NODE_NAME_KEY, true);
		json docIdJson = Helper::GetJSONParsedValue<string>(_materialResults, "documents", false);
		//UTILITY_API->DisplayMessageBox("docIdJson" + to_string(docIdJson));
		string docId = Helper::GetJSONValue<int>(docIdJson, 0, true);
		string objectId = Helper::GetJSONValue<string>(_materialResults, "id", true);
		//UTILITY_API->DisplayMessageBox("docId::" + docId);
		string season = Helper::GetJSONValue<string>(_materialResults, "parent_season", true);
		_materialResults["parent_season"] = Helper::GetJSONValue<string>(_materialTypeValues, season, true);
		_materialResults["parent_season_Id"] = season;
		_materialResults[OBJECT_NAME_KEY] = objectName;
		_materialResults[OBJECT_ID_KEY] = objectId;
		_materialResults[DFAULT_ASSET_KEY] = docId;
		GetAttributeDisplayNames(_materialResults);
	}
	catch (string msg)
	{
		Logger::Error("ProductConfig::UpdateResultJson() Exception - " + msg);
	}
	catch (exception & e)
	{
		Logger::Error("ProductConfig::UpdateResultJson() Exception - " + string(e.what()));
	}
	catch (const char* msg)
	{
		wstring wstr(msg, msg + strlen(msg));
		Logger::Error("ProductConfig::UpdateResultJson() Exception - " + string(msg));
	}

}
void  ProductConfig::CachePLMAttributes()
{
	QStringList list;
	Logger::Debug("ProductConfig -> CachePLMAttributes Start");
	m_seasonIdAndDisplayMap = GetAttributeIdAndValueMap(RESTAPI::SEASON_SEARCH_API);
	m_styleTypeIdAndDisplayMap = GetAttributeIdAndValueMap(RESTAPI::STYLE_TYPE_API);
	m_shapeIdAndDisplayMap = GetAttributeIdAndValueMap(RESTAPI::SHAPE_API);
	m_themeIdAndDisplayMap = GetAttributeIdAndValueMap(RESTAPI::THEME_API);
	string attTypesStr = "";
	json clientSpecificJson = Configuration::GetInstance()->GetClientSpecificJson();
	json styleTableAttributeArray = Helper::GetJSONParsedValue<string>(clientSpecificJson, "styleTableAttributes", false);
	string arrayItem = "";
	json attributeDataJson= json::object();
	for (int j = 0; j < styleTableAttributeArray.size(); j++)
	{
		json fieldsJson = Helper::GetJSONParsedValue<int>(styleTableAttributeArray, j, false);

		string attType = Helper::GetJSONValue<string>(fieldsJson, ATTRIBUTE_TYPE_KEY, true);
		string  attKey = Helper::GetJSONValue<string>(fieldsJson, ATTRIBUTE_KEY, true);
		if (attType == ENUM_ATT_TYPE_KEY)
		{
			string  format = Helper::GetJSONValue<string>(fieldsJson, ENUM_ATTRIBUTE_FORMAT_KEY, true);
			attributeDataJson[attKey] = UIHelper::GetEnumValues(format);
		}
	}
	 Configuration::GetInstance()->SetClientSpecificAttCachedData(attributeDataJson);
	
	Logger::Debug("ProductConfig -> CachePLMAttributes attributeDataJson" + to_string(attributeDataJson));
	Logger::Debug("ProductConfig -> CachePLMAttributes End");

}

void ProductConfig::GetAttributeDisplayNames(json& _materialResults)
{
	Logger::Debug("ProductConfig -> GetAttributeDisplayNames Start");
	string attvalue;
	string displayName;
	//UTILITY_API->DisplayMessageBox("Initial json" + to_string(_materialResults));
	try
	{
		if (!m_seasonIdAndDisplayMap.size())
		{
			CachePLMAttributes();
		}

		attvalue = Helper::GetJSONValue<string>(_materialResults, "parent_season_Id", true);
		displayName = GetDisplayName(m_seasonIdAndDisplayMap, attvalue);
		_materialResults["parent_season_name"] = displayName;

		attvalue = Helper::GetJSONValue<string>(_materialResults, "product_type", true);
		displayName = GetDisplayName(m_styleTypeIdAndDisplayMap, attvalue);
		_materialResults["style_type_name"] = displayName;

		attvalue = Helper::GetJSONValue<string>(_materialResults, "shape", true);
		displayName = GetDisplayName(m_shapeIdAndDisplayMap, attvalue);
		_materialResults["shape_name"] = displayName;

		attvalue = Helper::GetJSONValue<string>(_materialResults, "theme", true);
		displayName = GetDisplayName(m_themeIdAndDisplayMap, attvalue);
		_materialResults["theme_name"] = displayName;

		attvalue = Helper::GetJSONValue<string>(_materialResults, "uni_sleeve", true);
		displayName = GetDisplayName(m_sleeveIdAndDisplayMap, attvalue);
		_materialResults["uni_sleeve_name"] = displayName;

		attvalue = Helper::GetJSONValue<string>(_materialResults, "uni_length", true);
		displayName = GetDisplayName(m_lengthIdAndDisplayMap, attvalue);
		_materialResults["uni_length_name"] = displayName;

		attvalue = Helper::GetJSONValue<string>(_materialResults, "uni_silhouette", true);
		displayName = GetDisplayName(m_silhouetteIdAndDisplayMap, attvalue);
		_materialResults["uni_silhouette_name"] = displayName;

		attvalue = Helper::GetJSONValue<string>(_materialResults, "uni_fabric", true);
		displayName = GetDisplayName(m_fabricIdAndDisplayMap, attvalue);
		_materialResults["uni_fabric_name"] = displayName;

		//UTILITY_API->DisplayMessageBox("Final json" + to_string(_materialResults));
	}
	catch (string msg)
	{
		Logger::Error("ProductConfig -> GetAttributeDisplayNames() Exception - " + msg);
	}
	catch (exception & e)
	{
		Logger::Error("ProductConfig -> GetAttributeDisplayNames() Exception - " + string(e.what()));
	}
	catch (const char* msg)
	{
		wstring wstr(msg, msg + strlen(msg));
		Logger::Error("ProductConfig -> GetAttributeDisplayNames() Exception - " + string(msg));
	}
		Logger::Debug("ProductConfig -> GetAttributeDisplayNames End");
}
string ProductConfig::GetDisplayName(map<string, string> _map, string _Id)
{
	Logger::Debug("ProductConfig -> GetDisplayName Start");
	map<string, string>::iterator it;
	it = _map.find(_Id);
	string displayName;
	if (it != _map.end())
	{
		displayName = it->second;
		if (displayName.empty() || displayName == "centric%3A")
			displayName = BLANK;
	}
	Logger::Debug("ProductConfig -> GetDisplayName End");
	return displayName;
}
map<string,string> ProductConfig::GetAttributeIdAndValueMap(string _restApi)
{
	Logger::Debug("ProductConfig -> GetAttributeIdAndValueMap Start");
	json responseJson = Helper::makeRestcallGet(_restApi, "?limit=" + Configuration::GetInstance()->GetMaximumLimitForRefAttValue(), "", "Loading season details..");
	json attJson;
	string attName;
	string attId;
	map<string, string> attIdAndValueMap;
	try 
	{
		for (int i = 0; i < responseJson.size(); i++)
		{
			attJson = Helper::GetJSONParsedValue<int>(responseJson, i, false);;///use new method
			attName = Helper::GetJSONValue<string>(attJson, ATTRIBUTE_NAME, true);
			Logger::Debug("ProductConfig -> GetAttributeIdAndValueMap attName: " + attName);
			attId = Helper::GetJSONValue<string>(attJson, ATTRIBUTE_ID, true);
			Logger::Debug("ProductConfig -> SetDocumentConfigJSON attId: " + attId);
			attIdAndValueMap.insert(make_pair(attId, attName));
		}
	}
	catch (string msg)
	{
		Logger::Error("ProductConfig -> GetAttributeIdAndValueMap() Exception - " + msg);
	}
	catch (exception & e)
	{
		Logger::Error("ProductConfig -> GetAttributeIdAndValueMap() Exception - " + string(e.what()));
	}
	catch (const char* msg)
	{
		wstring wstr(msg, msg + strlen(msg));
		Logger::Error("ProductConfig -> GetAttributeIdAndValueMap() Exception - " + string(msg));
	}

	Logger::Debug("ProductConfig -> GetAttributeIdAndValueMap End");
	return attIdAndValueMap;
}


void ProductConfig::GetEnumlistJson(string _restApi, json& _attributeJson)
{
	json enumList;
	string enumListResponse = RESTAPI::CentricRestCallGet(_restApi, APPLICATION_JSON_TYPE, "");
	Logger::RestAPIDebug("enumListResponse::" + enumListResponse);
	try
	{
		if (FormatHelper::HasError(enumListResponse))
		{
			Helper::GetCentricErrorMessage(enumListResponse);
			throw runtime_error(enumListResponse);
		}
		json enumListResponseJson = json::parse(enumListResponse);
		for (size_t enumlistResponseJsonCount = 0; enumlistResponseJsonCount < enumListResponseJson.size(); enumlistResponseJsonCount++)
		{
			json EnumListJson = Helper::GetJSONParsedValue(enumListResponseJson, enumlistResponseJsonCount, false);
			enumList["enumKey"] = Helper::GetJSONValue<string>(EnumListJson, "id", true);
			enumList["enumValue"] = Helper::GetJSONValue<string>(EnumListJson, NODE_NAME_KEY, true);
			_attributeJson[ENUMLIST_JSON_KEY][enumlistResponseJsonCount] = enumList;
		}
	}
	catch (string msg)
	{
		Logger::Error("ProductConfig::GetEnumlistJsonp() Exception - " + msg);
	}
	catch (exception & e)
	{
		Logger::Error("ProductConfig::GetEnumlistJson() Exception - " + string(e.what()));
	}
	catch (const char* msg)
	{
		wstring wstr(msg, msg + strlen(msg));
		Logger::Error("ProductConfig::GetEnumlistJson() Exception - " + string(msg));
	}

}

/*
* Description - InitializeProductData() method used to Initialize product data from configure response from rest/file.
* Parameter -
* Exception -
* Return -
*/
//void ProductConfig::InitializeProductData()
//{
//	Logger::Info("ProductConfig -> InitializeProductData() -> Start");
//	RESTAPI::SetProgressBarData(20, "Loading Product configuration", true);
//	GetProductConfigJSON();
//	GetProductHierarchyJSON();
//	RESTAPI::SetProgressBarData(40, "Loading Product configuration", true);
//	GetProductFieldsJSON();
//	GetAttScopes();
//	RESTAPI::SetProgressBarData(60, "Loading Product configuration", true);
//	GetProductFilterJSON();
//	GetProductViewJSON();
//	RESTAPI::SetProgressBarData(80, "Loading Product configuration", true);
//	Configuration::GetInstance()->SetSupportedAttsList(GetProductConfigJSON());
//	Configuration::GetInstance()->SetResultsSupportedAttsList(GetProductConfigJSON());
//	RESTAPI::SetProgressBarData(100, "Loading Product configuration", true);
//	RESTAPI::SetProgressBarData(0, "", false);
//	Logger::Info("ProductConfig -> InitializeProductData() -> end");
//}

void ProductConfig::SetProductConfigJSON()
{
	auto startTime = std::chrono::high_resolution_clock::now();
	string initialConfigJsonString = RESTAPI::CentricRestCallGet(Configuration::GetInstance()->GetPLMServerURL() + RESTAPI::SEARCH_ATT_API, APPLICATION_JSON_TYPE, "skip=0&limit=10&node_name=Style");
	auto finishTime = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> totalDuration = finishTime - startTime;
	Logger::perfomance(PERFOMANCE_KEY + "Library Item, Material API :: " + to_string(totalDuration.count()));
	Logger::RestAPIDebug("initialConfigJsonString" + initialConfigJsonString);
	if (FormatHelper::HasError(initialConfigJsonString))
	{
		Helper::GetCentricErrorMessage(initialConfigJsonString);
		throw runtime_error(initialConfigJsonString);
	}
	json initialConfigJson = json::parse(initialConfigJsonString);
	string formDefinition = Helper::GetJSONValue<string>(initialConfigJson[0], CREATE_DEFINITION_KEY, true);
	auto FormDefinitionsApiStartTime = std::chrono::high_resolution_clock::now();
	string fieldsconfigJsonString = RESTAPI::CentricRestCallGet(Configuration::GetInstance()->GetPLMServerURL() + RESTAPI::SEARCH_ATT_DEFINITION_API + formDefinition + RESTAPI::SEARCH_ATT_DEFINITION_ADDITIONAL_API, APPLICATION_JSON_TYPE, "");
	auto FormDefinitionsApiFinishTime = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> FormDefinitionsApiTotalDuration = FormDefinitionsApiFinishTime - FormDefinitionsApiStartTime;
	Logger::perfomance(PERFOMANCE_KEY + "Create Form Definitions API :: " + to_string(FormDefinitionsApiTotalDuration.count()));
	Logger::RestAPIDebug("fieldsconfigJsonString::" + fieldsconfigJsonString);
	if (FormatHelper::HasError(fieldsconfigJsonString))
	{
		Helper::GetCentricErrorMessage(fieldsconfigJsonString);
		throw runtime_error(fieldsconfigJsonString);
	}
	//json defaultFieldsJson = ProductConfig::GetInstance()->GetProductConfigJSON();
	json defaultFieldsJson = ProductConfig::GetInstance()->GetProductConfigJSONFromFile();
	//UTILITY_API->DisplayMessageBox("defaultFieldsJson::" + to_string(defaultFieldsJson));
	createFieldsJson(fieldsconfigJsonString, defaultFieldsJson);
}

void ProductConfig::createFieldsJson(string& _fieldsJsonStringResponse, json& _defaultFieldsJson)
{
	json fieldsJsonResponse = json::parse(_fieldsJsonStringResponse);
	string attValue;
	string attType;
	string attKey;
	string isAttEnable;
	json fieldsListjson = json::array();

	json fieldsJson;
	fieldsJson["typeName"] = "style";
	QStringList attKeyList;
	int fieldsJsonCount = 0;
	try
	{
		for (fieldsJsonCount; fieldsJsonCount < fieldsJsonResponse.size(); fieldsJsonCount++)
		{
			json fieldsAttJson;
			json enumList;
			json feildsvaluesJson = Helper::GetJSONParsedValue<int>(fieldsJsonResponse, fieldsJsonCount, false);
			attKey = Helper::GetJSONValue<string>(feildsvaluesJson, REST_API_NAME_KEY, true);
			attType = Helper::GetJSONValue<string>(feildsvaluesJson, VALUE_TYPE_KEY, true);
			isAttEnable = Helper::GetJSONValue<string>(feildsvaluesJson, ENABLED_KEY, true);
			attValue = Helper::GetJSONValue<string>(feildsvaluesJson, DISPLAY_NAME_KEY, true);
			/*int last = attKey.find_last_of('/');
			attKey = attKey.substr(last + 1);
			styleConfig::GetInstance()->ConvertToo(attKey, true);*/
			if (attType != REF_ATT_TYPE_KEY && attType != REFLIST_ATT_TYPE_KEY)
			{
				Logger::Logger("attType:::::" + attType);
				attKeyList << QString::fromStdString(attKey);
				if (attType == "enum")
				{
					DataHelper::GetInstance()->SetEnumJson(fieldsAttJson, feildsvaluesJson);

				}
				fieldsAttJson["attKey"] = attKey;
				fieldsAttJson["attType"] = attType;
				fieldsAttJson["attScope"] = "STYLE";
				fieldsAttJson["attName"] = attValue;
				fieldsAttJson["isAttSearchable"] = isAttEnable;
				fieldsAttJson["isMultiselectable"] = true;
				fieldsAttJson[IS_CHECK_KEY] = true;
				fieldsAttJson[IS_SINGLE_INTERGER_KEY] = true;
				fieldsAttJson[IS_SINGLE_FLOAT_KEY] = true;
				if (FormatHelper::HasContent(to_string(fieldsAttJson)))
				{
					fieldsJson["attributes"].emplace_back(fieldsAttJson);
				}
			}
		}
		//UTILITY_API->DisplayMessageBox("after default loop.." + to_string(fieldsJsonCount));
		/*for noe don't know the search style default attributs list so commented.*/
		json defaultFieldsList = Helper::GetJSONParsedValue<string>(_defaultFieldsJson, FIELDLIST_JSON_KEY, false);
		for (size_t defaultFeildsListCount = 0; defaultFeildsListCount < defaultFieldsList.size(); defaultFeildsListCount++)
		{
			json defaultFeildsListCountJson = Helper::GetJSONParsedValue<int>(defaultFieldsList, defaultFeildsListCount, false);
			json attributesListCountJson = Helper::GetJSONParsedValue<string>(defaultFeildsListCountJson, "attributes", false);
			for (size_t attributesFieldsListJsonCount = 0; attributesFieldsListJsonCount < attributesListCountJson.size(); attributesFieldsListJsonCount++)
			{
				json attributesFieldsListJson = Helper::GetJSONParsedValue<int>(attributesListCountJson, attributesFieldsListJsonCount, false);
				string defaultAttributeKey = Helper::GetJSONValue<string>(attributesFieldsListJson, "attKey", true);
				string attType = Helper::GetJSONValue<string>(attributesFieldsListJson, "attType", true);
				string refApi = "";
				if (!attKeyList.contains(QString::fromStdString(defaultAttributeKey)))
				{
					fieldsJsonCount = fieldsJsonCount + 1;
					//UTILITY_API->DisplayMessageBox("comming inside...."+ attType);

					if (attType == REF_ATT_TYPE_KEY)
					{
						DataHelper::GetInstance()->SetRefJson(attributesFieldsListJson, defaultAttributeKey);
					}
					//if (defaultAttributeKey == "parent_season")
					//{
					//	string seasonApi = Configuration::GetInstance()->GetPLMServerURL() + refUrl + "?skip=0&limit=1000&sort=node_name";
					//	//UTILITY_API->DisplayMessageBox("seasonApi::" + seasonApi);
					//	ProductConfig::GetInstance()->GetEnumlistJson(seasonApi, attributesFieldsListJson);
					//}
					/*else if (defaultAttributeKey == "Collection")
					{
						string divisionApi = Configuration::GetInstance()->GetPLMServerURL() + refUrl + "?skip=0&limit="+ Configuration::GetInstance()->GetMaximumLimitForRefAttValue();
						ProductConfig::GetInstance()->GetEnumlistJson(divisionApi, attributesFieldsListJson);
					}*/

					/*else if (defaultAttributeKey == "product_type")
					{
						string materialTypeApi = Configuration::GetInstance()->GetPLMServerURL() + RESTAPI::MATERIAL_TYPE_SEARCH_API + "?skip=0&limit=1000";
						UTILITY_API->DisplayMessageBox("materialTypeApi::" + materialTypeApi);
						StyleConfig::GetInstance()->GetEnumlistJson(materialTypeApi, attributesFieldsListJson);
					}*/
					if (FormatHelper::HasContent(to_string(attributesFieldsListJson)))
					{
						fieldsJson["attributes"].push_back(attributesFieldsListJson);
					}
				}
			}
		}
		fieldsListjson[0] = fieldsJson;
		m_productFieldsJson = fieldsListjson;
	}
	catch (string msg)
	{
		Logger::Error("ProductConfig::createFieldsJson() Exception - " + msg);
	}
	catch (exception & e)
	{
		Logger::Error("ProductConfig::createFieldsJson() Exception - " + string(e.what()));
	}
	catch (const char* msg)
	{
		wstring wstr(msg, msg + strlen(msg));
		Logger::Error("ProductConfig::createFieldsJson() Exception - " + string(msg));
	}
	//UTILITY_API->DisplayMessageBox("m_productFieldsJson:: " + to_string(m_productFieldsJson));
}

void ProductConfig::SetCopyStyleResult(string _copyResult)
{
	auto mTypestartTime = std::chrono::high_resolution_clock::now();
	string seasonResponse;
	m_styleResults.clear();
	try
	{
		
		if (m_seasonValuesJson.empty())
		{
			seasonResponse = RESTAPI::CentricRestCallGet(Configuration::GetInstance()->GetPLMServerURL() + RESTAPI::SEASON_SEARCH_API + "?", APPLICATION_JSON_TYPE, "&limit="+ Configuration::GetInstance()->GetMaximumLimitForRefAttValue() +"&sort=node_name");
			auto mTypefinishTime = std::chrono::high_resolution_clock::now();
			std::chrono::duration<double> mTypetotalDuration = mTypefinishTime - mTypestartTime;
			Logger::perfomance(PERFOMANCE_KEY + "Style seasons API :: " + to_string(mTypetotalDuration.count()));
			Logger::RestAPIDebug("seasonResponse::" + seasonResponse);
			if (FormatHelper::HasError(seasonResponse) || !FormatHelper::HasContent(seasonResponse))
			{
				//throw runtime_error(styleTypeResponse);
				m_seasonValuesJson.clear();
			}
			else
			{
				json seasonResponseJson = json::parse(seasonResponse);
				for (auto count = 0; count < seasonResponseJson.size(); count++)
				{
					string seaonName = Helper::GetJSONValue<string>(seasonResponseJson[count], NODE_NAME_KEY, true);
					string seasonId = Helper::GetJSONValue<string>(seasonResponseJson[count], "id", true);
					m_seasonValuesJson[seasonId] = seaonName;
				}
			}
		}


		//UTILITY_API->DisplayMessageBox("In side COPY_PRODUCT_CLICKED");
		json resultListJson = json::parse(_copyResult);
		//UTILITY_API->DisplayMessageBox(_copyResult);
		ProductConfig::GetInstance()->UpdateResultJson(resultListJson, m_seasonValuesJson);
		m_styleResults.push_back(resultListJson);

		string resultsCount = to_string(m_styleResults.size());
		Logger::Logger("updated result count::" + resultsCount);

		if (stoi(resultsCount) > 0)
		{
			m_resultsCount = stoi(resultsCount);
			//UTILITY_API->DisplayMessageBox("m_typename::" + m_typename);
		}
		else
		{
			m_resultsCount = 0;
		}
		m_typename = "Style";
		string maxResultsLimit = resultsCount;//= Helper::GetJSONValue<string>(m_styleResults, "maxResultsLimit", true);
		if (FormatHelper::HasContent(maxResultsLimit))
		{
			m_maxResultsCount = stoi(maxResultsLimit);
		}
		else
		{
			m_maxResultsCount = 500;
		}
		if (m_resultsCount > m_maxResultsCount)
			throw "Maximum results limit exceeded. Please refine your search.";
	}
	catch (string msg)
	{
		Logger::Error("PLMStyleResult -> SetDataFromResponse Exception :: " + msg);
		throw msg;
	}
	catch (exception& e)
	{
		Logger::Error("PLMStyleResult -> SetDataFromResponse Exception :: " + string(e.what()));
		//UTILITY_API->DisplayMessageBox(e.what());
		throw e;
	}
	catch (const char* msg)
	{
		Logger::Error("PLMStyleResult -> SetDataFromResponse Exception :: " + string(msg));
		throw msg;
	}

}
void ProductConfig::SetDataFromResponse(json _param)
{
	//for now...
	Logger::Info("ProductConfig -> setDataFromResponse() -> Start");
	
	try
	{
		//string resultJsonString = RESTAPI::RESTMethodSubmit(RESTAPI::SEARCH_RESULTS_API, _param);
		string parameter = "";
		json attributesJson = Helper::GetJSONParsedValue<string>(_param, ATTRIBUTES_KEY, false);
		for (int attributesJsonCount = 0; attributesJsonCount < attributesJson.size(); attributesJsonCount++)
		{
			string attKey = Helper::GetJSONParsedValue<string>(attributesJson[attributesJsonCount], ATTRIBUTE_KEY, false);
			string attType = Helper::GetJSONParsedValue<string>(attributesJson[attributesJsonCount], ATTRIBUTE_TYPE_KEY, false);
			string attValue = Helper::GetJSONParsedValue<string>(attributesJson[attributesJsonCount], ATTRIBUTE_VALUE_KEY, false);
			//UTILITY_API->DisplayMessageBox("attValue::" + attValue);
			if (FormatHelper::HasContent(attValue))
			{
				QStringList dispList;
				size_t found = attValue.find(DELIMITER_NEGATION);
				if (found != string::npos)
				{
					dispList = QString::fromStdString(attValue).split(QString::fromStdString(DELIMITER_NEGATION));
					for (int i = 0; i < dispList.size() - 1; i++)
					{
						if (FormatHelper::HasContent(parameter))
							parameter = parameter + "&";

						if (attType == TEXT_ATT_TYPE_KEY || attType == STRING_ATT_TYPE_KEY)
							parameter = parameter + attKey + ":wcm=" + dispList[i].toStdString();
						else
							parameter = parameter + attKey + "=" + dispList[i].toStdString();

					}
					Logger::Debug("parameter------------------------------------- > " + parameter);
				}
				else
				{
					if (FormatHelper::HasContent(parameter))
						parameter = parameter + "&";

					if (attType == TEXT_ATT_TYPE_KEY || attType == STRING_ATT_TYPE_KEY)
						parameter = parameter + attKey + ":wcm=" + attValue;
					else
						parameter = parameter + attKey + "=" + attValue;
				}
			}
		}
		//UTILITY_API->DisplayMessageBox("parameter1::" + parameter);
		Logger::Logger("parameter Final------------------------------------- > " + parameter);
		string resultResponse = "";

		auto startTime = std::chrono::high_resolution_clock::now();
		Logger::Logger("Search Results API 1:: " + Configuration::GetInstance()->GetPLMServerURL() + RESTAPI::SEARCH_STYLE_API + "s?");
		resultResponse = RESTAPI::CentricRestCallGet(Configuration::GetInstance()->GetPLMServerURL() + RESTAPI::SEARCH_STYLE_API + "s?", APPLICATION_JSON_TYPE, parameter + "&limit=" + ProductConfig::GetInstance()->GetMaximumLimitForStyleResult());
		auto finishTime = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double> totalDuration = finishTime - startTime;
		Logger::perfomance(PERFOMANCE_KEY + "Search Results API :: " + to_string(totalDuration.count()));
		Logger::Logger("resultResponse main::" + resultResponse);

		if (!FormatHelper::HasContent(resultResponse))
		{
			throw "Unable to fetch results. Please try again or Contact your System Administrator.";
		}

		if (FormatHelper::HasError(resultResponse))
		{
			Helper::GetCentricErrorMessage(resultResponse);
			throw runtime_error(resultResponse);
		}
		json styleResults = json::array();
		if (FormatHelper::HasContent(resultResponse))
			styleResults = json::parse(resultResponse);
		Logger::Logger("total count of result::" + to_string(styleResults.size()));

		auto mTypestartTime = std::chrono::high_resolution_clock::now();
		string seasonResponse = RESTAPI::CentricRestCallGet(Configuration::GetInstance()->GetPLMServerURL() + RESTAPI::SEASON_SEARCH_API + "?", APPLICATION_JSON_TYPE, "&limit=" + Configuration::GetInstance()->GetMaximumLimitForRefAttValue() + "&sort=node_name");
		auto mTypefinishTime = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double> mTypetotalDuration = mTypefinishTime - mTypestartTime;
		Logger::perfomance(PERFOMANCE_KEY + "Style seasons API :: " + to_string(mTypetotalDuration.count()));
		Logger::RestAPIDebug("seasonResponse::" + seasonResponse);
		if (FormatHelper::HasError(seasonResponse) || !FormatHelper::HasContent(seasonResponse))
		{
			//throw runtime_error(styleTypeResponse);
			m_seasonValuesJson.clear();
		}
		else
		{
			json seasonResponseJson = json::parse(seasonResponse);
			for (auto count = 0; count < seasonResponseJson.size(); count++)
			{
				string seaonName = Helper::GetJSONValue<string>(seasonResponseJson[count], NODE_NAME_KEY, true);
				string seasonId = Helper::GetJSONValue<string>(seasonResponseJson[count], "id", true);
				m_seasonValuesJson[seasonId] = seaonName;
			}
		}
		m_styleResults.clear();
		for (int i = 0; i < styleResults.size(); i++)
		{
			string resultListStr = Helper::GetJSONValue<int>(styleResults, i, false);
			json resultListJson = json::parse(resultListStr);
			//string attachmentId = Helper::GetJSONValue<string>(resultListJson, "default_3d_style", true);
			/*if (attachmentId == "centric%3A" || !FormatHelper::HasContent(attachmentId))
				continue;
			Logger::Debug("attachmentId inside::" + attachmentId);*/
			ProductConfig::GetInstance()->UpdateResultJson(resultListJson, m_seasonValuesJson);
			m_styleResults.push_back(resultListJson);
		}
		//UTILITY_API->DisplayMessageBox("m_styleResults::" + to_string(m_styleResults));
		//StyleConfig::GetInstance()->UpdateResultJson();
		string resultsCount = to_string(m_styleResults.size());
		Logger::Logger("updated result count::" + resultsCount);
		/*if (stoi(resultsCount) <= 0)
			throw "No valid results found.";*/
		if (stoi(resultsCount) > 0)
		{
			m_resultsCount = stoi(resultsCount);
			//UTILITY_API->DisplayMessageBox("m_typename::" + m_typename);
		}
		else
		{
			m_resultsCount = 0;
		}
		m_typename = "Style";
		string maxResultsLimit = resultsCount;//= Helper::GetJSONValue<string>(m_styleResults, "maxResultsLimit", true);
		if (FormatHelper::HasContent(maxResultsLimit))
		{
			m_maxResultsCount = stoi(maxResultsLimit);
		}
		else
		{
			m_maxResultsCount = 500;
		}
		if (m_resultsCount > m_maxResultsCount)
			throw "Maximum results limit exceeded. Please refine your search.";

	}
	catch (string msg)
	{
		Logger::Error("PLMStyleResult -> SetDataFromResponse Exception :: " + msg);
		throw msg;
	}
	catch (exception& e)
	{
		Logger::Error("PLMStyleResult -> SetDataFromResponse Exception :: " + string(e.what()));
		//UTILITY_API->DisplayMessageBox(e.what());
		throw e;
	}
	catch (const char* msg)
	{
		Logger::Error("PLMStyleResult -> SetDataFromResponse Exception :: " + string(msg));
		throw msg;
	}
}

json ProductConfig::GetStyleResultsSON()
{
	Logger::Info("ProductConfig -> GetSearchCriteriaJSON() Start");
	return m_styleResults;
	Logger::Info("ProductConfig -> GetStyleResultsSON() End");
}

string ProductConfig::GetTypename()
{
	Logger::Info("ProductConfig -> GetTypename() Start");
	return m_typename;
	Logger::Info("ProductConfig -> GetTypename() End");
}

int ProductConfig::GetMaxResultCount()
{
	Logger::Info("ProductConfig -> GetMaxResultCount() Start");
	return m_maxResultsCount;
	Logger::Info("ProductConfig -> GetMaxResultCount() End");
}

int ProductConfig::GetResultsCount()
{
	Logger::Info("ProductConfig -> GetResultsCount() Start");
	return m_resultsCount;
	Logger::Info("ProductConfig -> GetResultsCount() End");
}

/*
* Description - GetIsModelExecuted() return a flag wether Rest call is being made and module is already executed, used for progress bar to work correctly
* Parameter -
* Exception -
* Return - bool
*/
bool ProductConfig::GetIsModelExecuted()
{
	return m_isModelExecuted;
}

/*
* Description - SetIsModelExecuted() sets a flag wether Rest call is being made and module is already executed, used for progress bar to work correctly
* Parameter -
* Exception -
* Return -
*/
void ProductConfig::SetIsModelExecuted(bool _isModelExecuted)
{
	m_isModelExecuted = _isModelExecuted;
}


void ProductConfig::InitializeProductData()
{
	Logger::Info("Configuration::InitializeProductData() Started..");
	ProductConfig::GetInstance()->SetMaximumLimitForStyleResult();
	GetProductConfigJSON();
	Logger::Info("Configuration::InitializeProductData() End..");
}

/*
* Description - SetMaximumLimitForColorResult() method used to set the maximum limit for Color Results in search response.
* Parameter -
* Exception -
* Return -
*/
void ProductConfig::SetMaximumLimitForStyleResult()
{
	m_maxStyleResultsLimit = Helper::GetJSONValue<string>(Configuration::GetInstance()->GetPLMPluginConfigJSON(), MAX_STYLE_SEARCH_LIMIT, true);
}

/*
* Description - GetMaximumLimitForColorResult() method used to get the maximum limit for Color Results in search response.
* Parameter -
* Exception -
* Return - string
*/
string ProductConfig::GetMaximumLimitForStyleResult()
{
	return m_maxStyleResultsLimit;
}