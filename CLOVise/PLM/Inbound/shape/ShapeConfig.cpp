/*
* Copyright 2020-2021 CLO-Vise. All rights reserved
*
* @file ShapeConfig.cpp
*
* @brief Class implementation for cach Shape data in CLO from PLM.
* This class has all the variable and methods implementation which are used in storing PLM Shape configuration related data to search in CLO
*
* @author GoVise
*
* @date 28-JUL-2020
*/
#include "ShapeConfig.h"
#include "CLOAPIInterface.h"
#include "CLOVise/PLM/Helper/Util/RestAPIUtility.h"
#include "CLOVise/PLM/Helper/Util/FormatHelper.h"
//#include "CLOVise/PLM/Outbound/PublishToPLM/CopyShape.h"
#include "CLOVise/PLM/Outbound/PublishToPLM/PublishToPLMData.h"
#include "CLOVise/PLM/Helper/Util/Logger.h"
#include "CLOVise/PLM/Helper/Util/DataHelper.h"
#include "CLOVise/PLM/Inbound/Shape/PLMShapeResults.h"
#include "CLOVise/PLM/Helper/UIHelper/UIHelper.h"

using json = nlohmann::json;

ShapeConfig* ShapeConfig::_instance = NULL;
ShapeConfig* ShapeConfig::GetInstance()
{
	if (_instance == NULL)
	{
		_instance = new ShapeConfig();
	}
	return _instance;
}

/*
* Description - SetShapeConfigJSON() method used to cache the Shape configuration json from rest/file.
* Parameter -  string, string, bool.
* Exception - exception, Char *
* Return -
*/
void ShapeConfig::SetShapeConfigJSONFromFile(string _module, string _rootType, bool _initDepedentJsons)
{
	Logger::Info("ShapeConfig -> SetShapeConfigJSON() start");
	m_shapeConfigJson = DataHelper::GetInstance()->SetConfigJSON(_module, _rootType, _initDepedentJsons, PLM_SHAPE_FILE_NAME);
	Logger::Info("ShapeConfig -> SetShapeConfigJSON() end");
}
//void ShapeConfig::SetShapeConfigJSON(string _module, string _rootType, bool _initDepedentJsons)
//{
//	Logger::Info("ShapeConfig -> SetShapeConfigJSON start");
//	Logger::Logger(" Shape Search initialiazing...");
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
//			Logger::Info("ShapeConfig -> if (restData)  start");
//
//			configJsonString = RESTAPI::RESTMethodSubmit(RESTAPI::SEARCH_FIELDS_API, param);
//			Logger::Debug("configJsonString---" + configJsonString);
//			if (!FormatHelper::HasContent(configJsonString))
//			{
//				throw "Unable to initiliaze Shape Configuration. Please try again or Contact your System Administrator.";
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
//			m_shapeConfigJson = json::parse(configJsonString);
//			Logger::Info("ShapeConfig -> if (restData)  end");
//		}
//		else
//		{
//			Logger::Info("ShapeConfig -> else  start");
//
//			m_shapeConfigJson = Helper::GetDataFromJsonFile(PLM_SHAPE_FILE_NAME);
//			if (!FormatHelper::HasContent(to_string(m_shapeConfigJson)))
//			{
//				throw "Unable to initiliaze Shape Configuration. Please try again or Contact your System Administrator.";
//			}
//			Logger::Info("ShapeConfig -> end  start");
//
//		}
//		if (_initDepedentJsons)
//		{
//			SetShapeHierarchyJSON(HIERARCHY_JSON_KEY);
//			SetShapeFieldsJSON(FIELDLIST_JSON_KEY);
//		}
//	}
//	catch (exception& e)
//	{
//		Logger::Error("ShapeConfig -> SetShapeConfigJSON Exception :: " + string(e.what()));
//		throw e;
//	}
//	catch (const char* msg)
//	{
//		Logger::Error("ShapeConfig -> SetShapeConfigJSON Exception :: " + string(msg));
//		throw msg;
//	}
//	Logger::Logger("Shape search intitialization completed.");
//	Logger::Info("ShapeConfig -> SetShapeConfigJSON end");
//}

/*
* Description - GetShapeConfigJSON() method used to get Shape configuration json.
* Parameter -
* Exception -
* Return - json.
*/
json ShapeConfig::GetShapeConfigJSON()
{
	Logger::Info("ShapeConfig -> GetShapeConfigJSON() start");

	if (m_shapeFieldsJson == nullptr || m_shapeFieldsJson.empty())
		SetShapeConfigJSON();
	Logger::Info("ShapeConfig -> GetShapeConfigJSON() end");

	return m_shapeFieldsJson;
}

/*
* Description - GetShapeConfigJSON() method used to get Shape configuration json.
* Parameter -
* Exception -
* Return - json.
*/
json ShapeConfig::GetShapeConfigJSONFromFile()
{
	Logger::Info("ShapeConfig -> GetShapeConfigJSON() start");

	if (m_shapeConfigJson == nullptr || m_shapeConfigJson.empty())
		SetShapeConfigJSONFromFile(SHAPE_MODULE, SHAPE_ROOT_TYPE, false);
	Logger::Info("ShapeConfig -> GetShapeConfigJSON() end");

	return m_shapeConfigJson;
}

/*
* Description - SetShapeHierarchyJSON() method used to get cached Shape Hierarchy JSON.
* Parameter -  string.
* Exception - exception, Char *
* Return -
*/
//void ShapeConfig::SetShapeHierarchyJSON(string _hierarchyJsonKey)
//{
//	Logger::Info("ShapeConfig -> SetShapeHierarchyJSON() start");
//
//	try
//	{
//		string shapeHierarchyStr = "";
//		shapeHierarchyStr = Helper::GetJSONValue<string>(GetShapeConfigJSON(), _hierarchyJsonKey, false);
//		m_shapeHierarchyJson = json::parse(shapeHierarchyStr);
//	}
//	catch (exception& e)
//	{
//		Logger::Error("ShapeConfig -> SetShapeHierarchyJSON Exception :: " + string(e.what()));
//		throw e;
//	}
//	catch (const char* msg)
//	{
//		Logger::Error("ShapeConfig -> SetShapeHierarchyJSON Exception :: " + string(msg));
//		throw msg;
//	}
//	Logger::Info("ShapeConfig -> SetShapeHierarchyJSON() end");
//
//}

/*
* Description - GetShapeHierarchyJSON() method used to get cached Shape Hierarchy JSON.
* Parameter -
* Exception -
* Return - json.
*/
json ShapeConfig::GetShapeHierarchyJSON()
{
	Logger::Info("ShapeConfig -> GetShapeHierarchyJSON() start");

	if (m_shapeHierarchyJson == nullptr || m_shapeHierarchyJson.empty())
		m_shapeHierarchyJson = DataHelper::GetInstance()->SetJson(HIERARCHY_JSON_KEY, m_shapeConfigJson);
	//SetShapeHierarchyJSON(HIERARCHY_JSON_KEY);

	Logger::Info("ShapeConfig -> GetShapeHierarchyJSON() end");

	return m_shapeHierarchyJson;
}

/*
* Description - SetAttScopes() method used to cache attribute scopes list.
* Parameter -  string.
* Exception - exception, Char *
* Return -
*/
//void ShapeConfig::SetAttScopes(string _attScopesJsonKey)
//{
//	Logger::Info("ShapeConfig -> SetAttScopes() start");
//	json attScopesArray = json::array();
//	try
//	{
//		string attScopesStr = "";
//		attScopesStr = Helper::GetJSONValue<string>(GetShapeConfigJSON(), _attScopesJsonKey, false);
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
//		Logger::Error("ShapeConfig -> SetAttScopes Exception :: " + string(e.what()));
//		throw e;
//	}
//	catch (const char* msg)
//	{
//		Logger::Error("ShapeConfig -> SetAttScopes Exception :: " + string(msg));
//		throw msg;
//	}
//	Logger::Info("ShapeConfig -> SetAttScopes() end");
//}

/*
* Description - GetAttScopes() method used to get cached attribute scopes list.
* Parameter -
* Exception -
* Return - QStringList.
*/
QStringList ShapeConfig::GetAttScopes()
{
	Logger::Info("ShapeConfig -> GetAttScopes() start");

	if (m_attScopes.empty())
		DataHelper::GetInstance()->SetAttScopes(m_shapeConfigJson, ATTSCOPE_JSON_KEY, m_attScopes);
	//SetAttScopes(ATTSCOPE_JSON_KEY);
	Logger::Info("ShapeConfig -> GetAttScopes() end");

	return m_attScopes;
}

/*
* Description - SetShapeFieldsJSON() method used to cache shape fields json.
* Parameter -  string.
* Exception - exception, Char *
* Return -
*/
//void ShapeConfig::SetShapeFieldsJSON(string _fieldsJsonKey)
//{
//	Logger::Info("ShapeConfig -> SetShapeFieldsJSON() start");
//
//	try
//	{
//		string shapeFieldsStr = "";
//		shapeFieldsStr = Helper::GetJSONValue<string>(GetShapeConfigJSON(), _fieldsJsonKey, false);
//		m_shapeFieldsJson = json::parse(shapeFieldsStr);
//	}
//	catch (exception& e)
//	{
//		Logger::Error("ShapeConfig -> SetShapeFieldsJSON Exception :: " + string(e.what()));
//		throw e;
//	}
//	catch (const char* msg)
//	{
//		Logger::Error("ShapeConfig -> SetShapeFieldsJSON Exception :: " + string(msg));
//		throw msg;
//	}
//	Logger::Info("ShapeConfig -> SetShapeFieldsJSON() end");
//
//}

/*
* Description - GetShapeFieldsJSON() method used to get cached the shape fields json.
* Parameter -
* Exception -
* Return - json.
*/
json ShapeConfig::GetShapeFieldsJSON()
{
	Logger::Info("ShapeConfig -> GetShapeFieldsJSON() start");

	if (m_shapeFieldsJson == nullptr || m_shapeFieldsJson.empty())
		SetShapeConfigJSON();
	Logger::Info("ShapeConfig -> GetShapeFieldsJSON() end");

	return m_shapeFieldsJson;
}

/*
* Description - SetDocumentPresetJSON() method used to cache the shape preset json.
* Parameter -  string.
* Exception - exception, Char *
* Return -
* no where used, need to remove.
*/
void ShapeConfig::SetShapePresetJSON(string _presetJsonKey)
{
	Logger::Info("ShapeConfig -> SetShapePresetJSON() start");

	try
	{
		string shapePresetStr = "";
		shapePresetStr = Helper::GetJSONValue<string>(GetShapeConfigJSON(), _presetJsonKey, false);
		m_shapePresetJson = json::parse(shapePresetStr);
	}
	catch (string msg)
	{
		Logger::Error("ShapeConfig -> SetShapePresetJSON() Exception :: " + msg);
		throw msg;
	}
	catch (exception& e)
	{
		Logger::Error("ShapeConfig -> SetShapePresetJSON() Exception :: " + string(e.what()));
		throw e;
	}
	catch (const char* msg)
	{
		Logger::Error("ShapeConfig -> SetShapePresetJSON() Exception :: " + string(msg));
		throw msg;
	}
	Logger::Info("ShapeConfig -> SetShapePresetJSON() end");

}

/*
* Description - GetShapePresetJSON() method used get cached shape preset json.
* Parameter -
* Exception -
* Return - json.
*/
json ShapeConfig::GetShapePresetJSON()
{
	Logger::Info("ShapeConfig -> GetShapePresetJSON() start");

	if (m_shapePresetJson == nullptr || m_shapePresetJson.empty())
		SetShapePresetJSON(PRESETLIST_JSON_KEY);
	Logger::Info("ShapeConfig -> GetShapePresetJSON() end");

	return m_shapePresetJson;
}

/*
* Description - SetShapeFilterJSON() method used to cache the shape filter json from rest/file.
* Parameter -  string, string, string.
* Exception - exception, Char *
* Return -
*/
void ShapeConfig::SetShapeFilterJSON(string _module, string _rootType, string _activity)
{
	Logger::Info("ShapeConfig -> SetShapeFilterJSON() end");
	m_shapeFilterJson = DataHelper::GetInstance()->SetFilterJSON(_module, _rootType, _activity, PLM_SHAPE_FILTER_FILE);
	Logger::Info("ShapeConfig -> SetShapeFilterJSON() end");
}
//void ShapeConfig::SetShapeFilterJSON(string _module, string _rootType, string _activity)
//{
//	Logger::Info("ShapeConfig -> SetShapeFilterJSON() start");
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
//				throw "Unable to initiliaze Shape Configuration. Please try again or Contact your System Administrator.";
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
//			m_shapeFilterJson = Helper::GetJSONParsedValue<string>(json::parse(filterJsonString), FILTERSLIST_JSON_KEY, false);
//		}
//		else
//		{
//			m_shapeFilterJson = Helper::GetJSONParsedValue<string>(Helper::GetDataFromJsonFile(PLM_SHAPE_FILTER_FILE), FILTERSLIST_JSON_KEY, false);
//			if (!FormatHelper::HasContent(to_string(m_shapeFilterJson)))
//			{
//				throw "Unable to initiliaze Shape Configuration. Please try again or Contact your System Administrator.";
//			}
//		}
//	}
//	catch (exception& e)
//	{
//		Logger::Error("ShapeConfig -> SetShapeFilterJSON Exception :: " + string(e.what()));
//		throw e;
//	}
//	catch (const char* msg)
//	{
//		Logger::Error("ShapeConfig -> SetShapeFilterJSON Exception :: " + string(msg));
//		throw msg;
//	}
//	Logger::Info("ShapeConfig -> SetShapeFilterJSON() start");
//
//}

/*
* Description - GetShapeFilterJSON() method used to get cached shape filter json.
* Parameter -
* Exception -
* Return - json.
*/
json ShapeConfig::GetShapeFilterJSON()
{
	Logger::Info("ShapeConfig -> GetShapeFilterJSON() start");

	if (m_shapeFilterJson == nullptr || m_shapeFilterJson.empty())
		SetShapeFilterJSON(SHAPE_MODULE, SHAPE_ROOT_TYPE, SHAPE_SEARCH_ACTIVITY);
	Logger::Info("ShapeConfig -> GetShapeFilterJSON() end");

	return m_shapeFilterJson;
}

/*
* Description - SetShapeViewJSON() method used to cache the shape view json from rest/file.
* Parameter -  string, string, string.
* Exception - exception, Char *
* Return -
*/
void ShapeConfig::SetShapeViewJSON(string _module, string _rootType, string _activity)
{
	Logger::Info("ShapeConfig -> SetShapeViewJSON() end");
	m_shapeViewJson = DataHelper::GetInstance()->SetViewJSON(_module, _rootType, _activity, PLM_SHAPE_VIEW_FILE);
	Logger::Info("ShapeConfig -> SetShapeViewJSON() end");
}
//void ShapeConfig::SetShapeViewJSON(string _module, string _rootType, string _activity)
//{
//	Logger::Info("ShapeConfig -> SetShapeViewJSON() start");
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
//				throw "Unable to initiliaze Shape Configuration. Please try again or Contact your System Administrator.";
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
//			m_shapeViewJson = Helper::GetJSONParsedValue<string>(json::parse(viewJsonString), VIEWS_LIST_JSON_KEY, false);
//		}
//		else
//		{
//			m_shapeViewJson = Helper::GetJSONParsedValue<string>(Helper::GetDataFromJsonFile(PLM_SHAPE_VIEW_FILE), VIEWS_LIST_JSON_KEY, false);
//			if (!FormatHelper::HasContent(to_string(m_shapeViewJson)))
//			{
//				throw "Unable to initiliaze Shape Configuration. Please try again or Contact your System Administrator.";
//			}
//		}
//	}
//	catch (exception& e)
//	{
//		Logger::Error("ShapeConfig -> SetShapeViewJSON Exception :: " + string(e.what()));
//		throw e;
//	}
//	catch (const char* msg)
//	{
//		Logger::Error("ShapeConfig -> SetShapeViewJSON Exception :: " + string(msg));
//		throw msg;
//	}
//	Logger::Info("ShapeConfig -> SetShapeViewJSON() end");
//}*/

/*
* Description - GetShapeViewJSON() method used to get cached shape view json.
* Parameter -
* Exception -
* Return - json.
*/
json ShapeConfig::GetShapeViewJSON()
{
	Logger::Info("ShapeConfig -> GetShapeViewJSON() start");

	if (m_shapeViewJson == nullptr || m_shapeViewJson.empty())
		SetShapeViewJSON(SHAPE_MODULE, SHAPE_ROOT_TYPE, SHAPE_SEARCH_ACTIVITY);
	Logger::Info("ShapeConfig -> GetShapeViewJSON() end");

	return m_shapeViewJson;
}

/*
* Description - SetSeasonPaletteJSON() method used to cache the season palette json from rest/file.
* Parameter -
* Exception - exception, Char *
* Return -
*/
//void ShapeConfig::SetSeasonPaletteJSON()
//{
//	Logger::Info("ShapeConfig -> SetSeasonPaletteJSON() start");
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
//			m_paletteJson = Helper::GetDataFromJsonFile(PLM_SHAPE_PALETTE_FILE);
//			if (!FormatHelper::HasContent(to_string(m_paletteJson)))
//			{
//				throw "Unable to initiliaze Shape Configuration. Please try again or Contact your System Administrator.";
//			}
//		}
//	}
//	catch (exception& e)
//	{
//		Logger::Error("ShapeConfig -> SetSeasonPaletteJSON Exception :: " + string(e.what()));
//		throw e;
//	}
//	catch (const char* msg)
//	{
//		Logger::Error("ShapeConfig -> SetSeasonPaletteJSON Exception :: " + string(msg));
//		throw msg;
//	}
//	Logger::Info("ShapeConfig -> SetSeasonPaletteJSON() end");
//
//}

/*
* Description - GetSeasonPaletteJSON() method used to get cached the season palette json.
* Parameter -  bool.
* Exception -
* Return - json.
*/
json ShapeConfig::GetSeasonPaletteJSON(bool _refresh)
{
	Logger::Info("ShapeConfig -> GetSeasonPaletteJSON() start");

	if (_refresh)
	{
		m_paletteJson = DataHelper::GetInstance()->SetSeasonPaletteJSON(PLM_SHAPE_PALETTE_FILE);
	}
	Logger::Info("ShapeConfig -> GetSeasonPaletteJSON() end");

	return m_paletteJson;
}

/*
* Description - SetSelectedViewIdx() method used to cache the selected view index.
* Parameter -  int.
* Exception -
* Return -
*/
void ShapeConfig::SetSelectedViewIdx(int _selectedViewIdx)
{
	Logger::Info("ShapeConfig -> SetSelectedViewIdx() Start");
	m_selectedViewIdx = _selectedViewIdx;
	Logger::Info("ShapeConfig -> SetSelectedViewIdx() End");
}

/*
* Description - GetSelectedViewIdx() method used to get cached selected view index.
* Parameter -
* Exception -
* Return - int.
*/
int ShapeConfig::GetSelectedViewIdx()
{
	return m_selectedViewIdx;
}

/*
* Description - SetSearchCriteriaJSON() method used to cache the search criteria json.
* Parameter -  json.
* Exception -
* Return -
*/
void ShapeConfig::SetSearchCriteriaJSON(json _criteria)
{
	Logger::Info("ShapeConfig -> SetSelectedViewIdx() Start");
	m_searchCriteriaJson = _criteria;
	Logger::Info("ShapeConfig -> SetSelectedViewIdx() End");
}

/*
* Description - GetSearchCriteriaJSON() method used to get cached search criteria json.
* Parameter -
* Exception -
* Return - json.
*/
json ShapeConfig::GetSearchCriteriaJSON()
{
	Logger::Info("ShapeConfig -> m_searchCriteriaJson() --------" + to_string(m_searchCriteriaJson));
	return m_searchCriteriaJson;
}

/*
* Description - SetDateFlag() method used to set date exist flag.
* Parameter -  bool.
* Exception -
* Return -
*/
void ShapeConfig::SetDateFlag(bool _isDateEditPresent)
{
	Logger::Info("ShapeConfig -> SetDateFlag() end");
	m_isDateEditPresent = _isDateEditPresent;
	Logger::Info("ShapeConfig -> SetDateFlag() end");
}

/*
* Description - GetDateFlag() method used to get date exist flag.
* Parameter -
* Exception -
* Return - bool.
*/
bool ShapeConfig::GetDateFlag()
{
	return m_isDateEditPresent;
}

/*
* Description - SetDownloadFileName() method used to cache downloaded file name.
* Parameter -  string.
* Exception -
* Return -
*/
void ShapeConfig::SetDownloadFileName(string _downloadFileName)
{
	m_downloadFileName = _downloadFileName;
}

/*
* Description - GetDownloadFileName() method used to get cached downloaded file name.
* Parameter -
* Exception -
* Return - string.
*/
string  ShapeConfig::GetDownloadFileName()
{
	return m_downloadFileName;
}

void  ShapeConfig::UpdateResultJson(json& _shapeResults, json& _shapeTypeValues, json& _shapemasters)
{
	Logger::Debug("ShapeConfig _shapeResults  - " + to_string(_shapeResults));
	Logger::Info("shape all season values - " + to_string(_shapeTypeValues));
	Logger::Info("shape all shape master - " + to_string(_shapemasters  ));

	//Logger::Info("shape all shape master value - " + to_string(_shapemasters));
	try
	{
		string objectName = Helper::GetJSONValue<string>(_shapeResults, NODE_NAME_KEY, true);
		Logger::Info("objectName - " + objectName);

		json docIdJson = Helper::GetJSONParsedValue<string>(_shapeResults, "documents", false);
		Logger::Info("docIdJson - " + to_string(docIdJson));
		//loop through the results, so we can get the shape seasons for every loop 
		 
		//UTILITY_API->DisplayMessageBox("docIdJson" + to_string(docIdJson));
		 string docId = Helper::GetJSONValue<int>(docIdJson, 0, true);
		string objectId = Helper::GetJSONValue<string>(_shapeResults, "id", true);
		
		string season = Helper::GetJSONValue<string>(_shapeResults, "shape_seasons", false);
		Logger::Info("whats stored in season - " + season);
		
		string shapemaster = Helper::GetJSONValue<string>(_shapeResults, "shape_master", true );
		Logger::Info("whats stored in shape master - " + shapemaster);
		
		Logger::Info("shape master--------- - " + Helper::GetJSONValue<string>(_shapemasters, shapemaster, false)); 
		string ShapeMasterName = Helper::GetJSONValue<string>(_shapemasters, shapemaster, false);
		Logger::Info("shape master--------- - " + Helper::GetJSONValue<string>(_shapemasters, shapemaster, true));
		Logger::Info("shape master--------- - " + ShapeMasterName);
		_shapeResults["shape_master"] = Helper::GetJSONValue<string>(_shapemasters, shapemaster, true);
	//	Logger::Info("shape master stored in Shapemaster - " + ShapeMasterName);

		
		//string Shapemaster = Helper::GetJSONValue<string>(_shapemasters, shapemaster, true);
			//Logger::Info("shape master - " + ShapeMasterName);
		//json shapemasterJson = json::parse(shapemaster);



	//	Logger::Info("shape seasonJson - " + to_string(seasonJson));
	//	Logger::Info("shape master - " + to_string(shapemasterJson));

		Logger::Info("shape season - " + season);
		json seasonJson = json::parse(season);
		
		QStringList seasonNameList;
		for (int j = 0; j < seasonJson.size(); j++)
		{
			string sesvar;
			sesvar = Helper::GetJSONValue<int>(seasonJson, j, true);

			Logger::Info("shape season_code - " + sesvar);
			Logger::Info("shape season--------- - " + Helper::GetJSONValue<string>(_shapeTypeValues, sesvar, true));
			QString seasonName = QString::fromStdString(Helper::GetJSONValue<string>(_shapeTypeValues, sesvar, true));
			Logger::Info("shape seasonName - " + seasonName.toStdString());
			seasonNameList.push_back(seasonName);
		}
		Logger::Debug("seasonNameList============= " + seasonNameList.join(",").toStdString());
		_shapeResults["shape_seasons"] = seasonNameList.join(",").toStdString();
		Logger::Info("shape seasonJson - " + to_string(seasonJson));

		//QStringList shapemasterNameList;
		//for (int j = 0; j < shapemasterJson.size(); j++)
		//{
		//	string shapmasvar;
		//	shapmasvar = Helper::GetJSONValue<int>(shapemasterJson, j, false);

		//	Logger::Info("shape shapemaste - " + shapmasvar);
		//	Logger::Info("shape shapemaste--------- - " + Helper::GetJSONValue<string>(_shapemasters, shapmasvar, true));
		//	QString shpemasterName = QString::fromStdString(Helper::GetJSONValue<string>(_shapemasters, shapmasvar, true));
		//	Logger::Info("shape shapemaste - " + shpemasterName.toStdString());
		//	shapemasterNameList.push_back(shpemasterName);
		//}
		////Logger::Debug("shapemasterNameList============= " + shapemasterNameList.join(",").toStdString());
		////_shapeResults["shape_master"] = shapemasterNameList.join(",").toStdString();
		//Logger::Info("shape seasonJson - " + to_string(shapemasterJson));



	

		//_shapeResults["parent_season_Id"] = season;
		_shapeResults[OBJECT_NAME_KEY] = objectName;
		_shapeResults[OBJECT_ID_KEY] = objectId;
		_shapeResults[DFAULT_ASSET_KEY] = docId;
		Logger::Info("shape details  - " + to_string(_shapeResults));
		GetAttributeDisplayNames(_shapeResults);
			
	}
	catch (string msg)
	{
		Logger::Error("ShapeConfig::UpdateResultJson() Exception - " + msg);
	}
	catch (exception & e)
	{
		Logger::Error("ShapeConfig::UpdateResultJson() Exception - " + string(e.what()));
	}
	catch (const char* msg)
	{
		wstring wstr(msg, msg + strlen(msg));
		Logger::Error("ShapeConfig::UpdateResultJson() Exception - " + string(msg));
	}

}
void  ShapeConfig::CachePLMAttributes()
{
	QStringList list;
	Logger::Debug("ShapeConfig -> CachePLMAttributes Start");
	m_seasonIdAndDisplayMap = GetAttributeIdAndValueMap(RESTAPI::SEASON_SEARCH_API);
	//m_shapeTypeIdAndDisplayMap = GetAttributeIdAndValueMap(RESTAPI::STYLE_TYPE_API);
	m_shapemasterAndDisplayMap = GetAttributeIdAndValueMap(RESTAPI::SHAPE_MASTER_SEARCH_API);
	m_shapeIdAndDisplayMap = GetAttributeIdAndValueMap(RESTAPI::SHAPE_API);
	m_themeIdAndDisplayMap = GetAttributeIdAndValueMap(RESTAPI::THEME_API);
	string attTypesStr = "";
	json clientSpecificJson = Configuration::GetInstance()->GetClientSpecificJson();
	json styleTableAttributeArray = Helper::GetJSONParsedValue<string>(clientSpecificJson, "shapeTableAttributes", false);
	string arrayItem = "";
	json attributeDataJson = json::object();
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

	Logger::Debug("ShapeConfig -> CachePLMAttributes attributeDataJson" + to_string(attributeDataJson));
	Logger::Debug("ShapeConfig -> CachePLMAttributes End");

}

void ShapeConfig::GetAttributeDisplayNames(json& _shapeResults)
{
	Logger::Debug("ShapeConfig -> GetAttributeDisplayNames Start");
	string attvalue;
	string displayName;
	//UTILITY_API->DisplayMessageBox("Initial json" + to_string(_shapeResults));
	try
	{
		if (!m_seasonIdAndDisplayMap.size())
		{
			CachePLMAttributes();
		}

		attvalue = Helper::GetJSONValue<string>(_shapeResults, "parent_season_Id", true);
		displayName = GetDisplayName(m_seasonIdAndDisplayMap, attvalue);
		_shapeResults["parent_season_name"] = displayName;

		attvalue = Helper::GetJSONValue<string>(_shapeResults, "shape", true);
		displayName = GetDisplayName(m_shapeIdAndDisplayMap, attvalue);
		_shapeResults["shape_name"] = displayName;

		attvalue = Helper::GetJSONValue<string>(_shapeResults, "shape_master", true);
		displayName = GetDisplayName(m_shapemasterAndDisplayMap, attvalue);
		_shapeResults["shape_master_name"] = displayName;

	}
	catch (string msg)
	{
		Logger::Error("ShapeConfig -> GetAttributeDisplayNames() Exception - " + msg);
	}
	catch (exception & e)
	{
		Logger::Error("ShapeConfig -> GetAttributeDisplayNames() Exception - " + string(e.what()));
	}
	catch (const char* msg)
	{
		wstring wstr(msg, msg + strlen(msg));
		Logger::Error("ShapeConfig -> GetAttributeDisplayNames() Exception - " + string(msg));
	}
	Logger::Debug("ShapeConfig -> GetAttributeDisplayNames End");
}

/*
* Description - GetDisplayName() method used to set display name of the attributs .
* Parameter -  string.
* Exception -
* Return - displayName(string)
*/
string ShapeConfig::GetDisplayName(map<string, string> _map, string _Id)
{
	Logger::Debug("ShapeConfig -> GetDisplayName Start");
	map<string, string>::iterator it;
	it = _map.find(_Id);
	string displayName;
	if (it != _map.end())
	{
		displayName = it->second;
		if (displayName.empty() || displayName == "centric%3A")
			displayName = BLANK;
	}
	Logger::Debug("ShapeConfig -> GetDisplayName End");
	return displayName;
}
map<string, string> ShapeConfig::GetAttributeIdAndValueMap(string _restApi)
{
	Logger::Debug("ShapeConfig -> GetAttributeIdAndValueMap Start");
	json responseJson = RESTAPI::makeRestcallGet(_restApi, "?limit=" + Configuration::GetInstance()->GetMaximumLimitForRefAttValue(), "", "Loading season details..");
	Logger::Debug("ShapeConfig.cpp ->responseJson " + to_string(responseJson));

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
			Logger::Debug("ShapeConfig -> GetAttributeIdAndValueMap attName: " + attName);
			attId = Helper::GetJSONValue<string>(attJson, ATTRIBUTE_ID, true);
			Logger::Debug("ShapeConfig -> SetDocumentConfigJSON attId: " + attId);
			attIdAndValueMap.insert(make_pair(attId, attName));
		}
	}
	catch (string msg)
	{
		Logger::Error("ShapeConfig -> GetAttributeIdAndValueMap() Exception - " + msg);
	}
	catch (exception & e)
	{
		Logger::Error("ShapeConfig -> GetAttributeIdAndValueMap() Exception - " + string(e.what()));
	}
	catch (const char* msg)
	{
		wstring wstr(msg, msg + strlen(msg));
		Logger::Error("ShapeConfig -> GetAttributeIdAndValueMap() Exception - " + string(msg));
	}

	Logger::Debug("ShapeConfig -> GetAttributeIdAndValueMap End");
	return attIdAndValueMap;
}


void ShapeConfig::GetEnumlistJson(string _restApi, json& _attributeJson)
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
		Logger::Error("ShapeConfig::GetEnumlistJsonp() Exception - " + msg);
	}
	catch (exception & e)
	{
		Logger::Error("ShapeConfig::GetEnumlistJson() Exception - " + string(e.what()));
	}
	catch (const char* msg)
	{
		wstring wstr(msg, msg + strlen(msg));
		Logger::Error("ShapeConfig::GetEnumlistJson() Exception - " + string(msg));
	}

}

/*
* Description - InitializeShapeData() method used to Initialize shape data from configure response from rest/file.
* Parameter -
* Exception -
* Return -
*/
//void ShapeConfig::InitializeShapeData()
//{
//	Logger::Info("ShapeConfig -> InitializeShapeData() -> Start");
//	RESTAPI::SetProgressBarData(20, "Loading Shape configuration", true);
//	GetShapeConfigJSON();
//	GetShapeHierarchyJSON();
//	RESTAPI::SetProgressBarData(40, "Loading Shape configuration", true);
//	GetShapeFieldsJSON();
//	GetAttScopes();
//	RESTAPI::SetProgressBarData(60, "Loading Shape configuration", true);
//	GetShapeFilterJSON();
//	GetShapeViewJSON();
//	RESTAPI::SetProgressBarData(80, "Loading Shape configuration", true);
//	Configuration::GetInstance()->SetSupportedAttsList(GetShapeConfigJSON());
//	Configuration::GetInstance()->SetResultsSupportedAttsList(GetShapeConfigJSON());
//	RESTAPI::SetProgressBarData(100, "Loading Shape configuration", true);
//	RESTAPI::SetProgressBarData(0, "", false);
//	Logger::Info("ShapeConfig -> InitializeShapeData() -> end");
//}

void ShapeConfig::SetShapeConfigJSON()
{
	auto startTime = std::chrono::high_resolution_clock::now();
	string initialConfigJsonString = RESTAPI::CentricRestCallGet(Configuration::GetInstance()->GetPLMServerURL() + RESTAPI::SEARCH_ATT_API, APPLICATION_JSON_TYPE, "skip=0&limit=10&decode=true&node_name=Shape");
	auto finishTime = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> totalDuration = finishTime - startTime;
	Logger::perfomance(PERFOMANCE_KEY + "Library Item, Shape API :: " + to_string(totalDuration.count()));
	Logger::RestAPIDebug("shape initialConfigJsonString" + initialConfigJsonString);
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
	//json defaultFieldsJson = ShapeConfig::GetInstance()->GetShapeConfigJSON();
	json defaultFieldsJson = ShapeConfig::GetInstance()->GetShapeConfigJSONFromFile();
	//UTILITY_API->DisplayMessageBox("defaultFieldsJson::" + to_string(defaultFieldsJson));
	createFieldsJson(fieldsconfigJsonString, defaultFieldsJson);
}

void ShapeConfig::createFieldsJson(string& _fieldsJsonStringResponse, json& _defaultFieldsJson)
{
	json fieldsJsonResponse = json::parse(_fieldsJsonStringResponse);
	string attValue;
	string attType;
	string attKey;
	string isAttEnable;
	json fieldsListjson = json::array();

	json fieldsJson;
	fieldsJson["typeName"] = "shape";
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
			ShapeConfig::GetInstance()->ConvertToo(attKey, true);*/
			if (attType != REF_ATT_TYPE_KEY) //&& attType != REFLIST_ATT_TYPE_KEY)
			{
				Logger::Logger("attType:::::" + attType);
				attKeyList << QString::fromStdString(attKey);
				if (attType == "enum")
				{
					DataHelper::GetInstance()->SetEnumJson(fieldsAttJson, feildsvaluesJson);
				}
				fieldsAttJson["attKey"] = attKey;
				fieldsAttJson["attType"] = attType;
				fieldsAttJson["attScope"] = "SHAPE";
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
		/*for noe don't know the search Shape default attributs list so commented.*/
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
					//	ShapeConfig::GetInstance()->GetEnumlistJson(seasonApi, attributesFieldsListJson);
					//}
					/*else if (defaultAttributeKey == "Collection")
					{
						string divisionApi = Configuration::GetInstance()->GetPLMServerURL() + refUrl + "?skip=0&limit="+ Configuration::GetInstance()->GetMaximumLimitForRefAttValue();
						ShapeConfig::GetInstance()->GetEnumlistJson(divisionApi, attributesFieldsListJson);
					}*/

					/*else if (defaultAttributeKey == "shape_type")
					{
						string materialTypeApi = Configuration::GetInstance()->GetPLMServerURL() + RESTAPI::MATERIAL_TYPE_SEARCH_API + "?skip=0&limit=1000";
						UTILITY_API->DisplayMessageBox("materialTypeApi::" + materialTypeApi);
						ShapeConfig::GetInstance()->GetEnumlistJson(materialTypeApi, attributesFieldsListJson);
					}*/
					if (FormatHelper::HasContent(to_string(attributesFieldsListJson)))
					{
						fieldsJson["attributes"].push_back(attributesFieldsListJson);
					}
				}
			}
		}
		fieldsListjson[0] = fieldsJson;
		m_shapeFieldsJson = fieldsListjson;
	}
	catch (string msg)
	{
		Logger::Error("ShapeConfig::createFieldsJson() Exception - " + msg);
	}
	catch (exception & e)
	{
		Logger::Error("ShapeConfig::createFieldsJson() Exception - " + string(e.what()));
	}
	catch (const char* msg)
	{
		wstring wstr(msg, msg + strlen(msg));
		Logger::Error("ShapeConfig::createFieldsJson() Exception - " + string(msg));
	}
	//UTILITY_API->DisplayMessageBox("m_shapeFieldsJson:: " + to_string(m_shapeFieldsJson));
}

void ShapeConfig::SetDataFromResponse(json _param)
{
	//for now...
	Logger::Info("ShapeConfig -> setDataFromResponse() -> Start");

	try
	{
		//string resultJsonString = RESTAPI::RESTMethodSubmit(RESTAPI::SEARCH_RESULTS_API, _param);
		string parameter = "";
		json attributesJson = Helper::GetJSONParsedValue<string>(_param, ATTRIBUTES_KEY, false);
		Logger::Debug("ShapeConfig -> setDataFromResponse() -> Start" + to_string(attributesJson));
		Logger::Debug("ShapeConfig -> setDataFromResponse att ributes-> Start" + ATTRIBUTES_KEY);
		Logger::Debug("ShapeConfig -> setDataFromResponse att ributes-> Start" + to_string(_param));
		for (int attributesJsonCount = 0; attributesJsonCount < attributesJson.size(); attributesJsonCount++)
		{
			string attKey = Helper::GetJSONParsedValue<string>(attributesJson[attributesJsonCount], ATTRIBUTE_KEY, false);
			Logger::Debug("ShapeConfig -> attKey" + attKey);
			string attType = Helper::GetJSONParsedValue<string>(attributesJson[attributesJsonCount], ATTRIBUTE_TYPE_KEY, false);
			Logger::Debug("ShapeConfig -> attType" + attType);
			string attValue = Helper::GetJSONParsedValue<string>(attributesJson[attributesJsonCount], ATTRIBUTE_VALUE_KEY, false);
			Logger::Debug("ShapeConfig -> attValue" + attValue);

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
		Logger::Logger("Search Results API 1:: " + Configuration::GetInstance()->GetPLMServerURL() + RESTAPI::SEARCH_SHAPE_API + "s?");
		resultResponse = RESTAPI::CentricRestCallGet(Configuration::GetInstance()->GetPLMServerURL() + RESTAPI::SEARCH_SHAPE_API + "s?", APPLICATION_JSON_TYPE, parameter + "&decode=true&limit=" + ShapeConfig::GetInstance()->GetMaximumLimitForShapeResult());
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
		json ShapeResults = json::array();
		if (FormatHelper::HasContent(resultResponse))
			ShapeResults = json::parse(resultResponse);
		Logger::Logger("total count of result::" + to_string(ShapeResults.size()));

		//************shapeseason try ***************************************
		auto mTypestartTime = std::chrono::high_resolution_clock::now();
		string seasonResponse = RESTAPI::CentricRestCallGet(Configuration::GetInstance()->GetPLMServerURL() + RESTAPI::SEASON_SEARCH_API + "?", APPLICATION_JSON_TYPE, "&decode=true&limit=" + Configuration::GetInstance()->GetMaximumLimitForRefAttValue() + "&sort=node_name");
		auto mTypefinishTime = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double> mTypetotalDuration = mTypefinishTime - mTypestartTime;
		Logger::perfomance(PERFOMANCE_KEY + "Shape seasons API :: " + to_string(mTypetotalDuration.count()));
		Logger::RestAPIDebug("seasonResponse::" + seasonResponse);

		if (FormatHelper::HasError(seasonResponse) || !FormatHelper::HasContent(seasonResponse))
		{
			//throw runtime_error(ShapeTypeResponse);
			m_seasonValuesJson.clear();
		}
		else
		{
			json seasonResponseJson = json::parse(seasonResponse);
			for (auto count = 0; count < seasonResponseJson.size(); count++)
			{
				string seasonName = Helper::GetJSONValue<string>(seasonResponseJson[count], NODE_NAME_KEY, true);
				string seasonId = Helper::GetJSONValue<string>(seasonResponseJson[count], "id", true);
				m_seasonValuesJson[seasonId] = seasonName;
			}
		}

		
		//************shapemaster try ***************************************
		//auto mTypestartTime = std::chrono::high_resolution_clock::now();
		string shapemasterResponse = RESTAPI::CentricRestCallGet(Configuration::GetInstance()->GetPLMServerURL() + RESTAPI::SHAPE_MASTER_SEARCH_API + "?", APPLICATION_JSON_TYPE, "&decode=true&limit=" + Configuration::GetInstance()->GetMaximumLimitForRefAttValue() + "&sort=node_name");
		//auto mTypefinishTime = std::chrono::high_resolution_clock::now();
		//std::chrono::duration<double> mTypetotalDuration = mTypefinishTime - mTypestartTime;
		Logger::perfomance(PERFOMANCE_KEY + "Shape seasons API :: " + to_string(mTypetotalDuration.count()));
		Logger::RestAPIDebug("shapemasterResponse::" + shapemasterResponse);

		if (FormatHelper::HasError(shapemasterResponse) || !FormatHelper::HasContent(shapemasterResponse))
		{
			//throw runtime_error(ShapeTypeResponse);
			m_shapemasterValuesJson.clear();
		}
		else
		{
			json shapemasterResponseJson = json::parse(shapemasterResponse);
			for (auto count = 0; count < shapemasterResponseJson.size(); count++)
			{
				string shapemasterName = Helper::GetJSONValue<string>(shapemasterResponseJson[count], NODE_NAME_KEY, true);
				string shapemasterId = Helper::GetJSONValue<string>(shapemasterResponseJson[count], "id", true);
				m_shapemasterValuesJson[shapemasterId] = shapemasterName;
			}
		}

		m_shapeResults.clear();
		for (int i = 0; i < ShapeResults.size(); i++)
		{
			string resultListStr = Helper::GetJSONValue<int>(ShapeResults, i, false);
			json resultListJson = json::parse(resultListStr);
			//string attachmentId = Helper::GetJSONValue<string>(resultListJson, "default_3d_Shape", true);
			/*if (attachmentId == "centric%3A" || !FormatHelper::HasContent(attachmentId))
				continue;
			Logger::Debug("attachmentId inside::" + attachmentId);*/
			ShapeConfig::GetInstance()->UpdateResultJson(resultListJson, m_seasonValuesJson, m_shapemasterValuesJson);
			m_shapeResults.push_back(resultListJson);
		}
		//UTILITY_API->DisplayMessageBox("m_shapeResults::" + to_string(m_shapeResults));
		//ShapeConfig::GetInstance()->UpdateResultJson();
		string resultsCount = to_string(m_shapeResults.size());
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
		m_typename = "Shape";
		string maxResultsLimit = resultsCount;//= Helper::GetJSONValue<string>(m_shapeResults, "maxResultsLimit", true);
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
		Logger::Error("PLMShapeResult -> SetDataFromResponse Exception :: " + msg);
		throw msg;
	}
	catch (exception& e)
	{
		Logger::Error("PLMShapeResult -> SetDataFromResponse Exception :: " + string(e.what()));
		//UTILITY_API->DisplayMessageBox(e.what());
		throw e;
	}
	catch (const char* msg)
	{
		Logger::Error("PLMShapeResult -> SetDataFromResponse Exception :: " + string(msg));
		throw msg;
	}
}

json ShapeConfig::GetShapeResultsSON()
{
	Logger::Info("ShapeConfig -> GetSearchCriteriaJSON() Start");
	return m_shapeResults;
	Logger::Info("ShapeConfig -> GetShapeResultsSON() End");
}

string ShapeConfig::GetTypename()
{
	Logger::Info("ShapeConfig -> GetTypename() Start");
	return m_typename;
	Logger::Info("ShapeConfig -> GetTypename() End");
}

int ShapeConfig::GetMaxResultCount()
{
	Logger::Info("ShapeConfig -> GetMaxResultCount() Start");
	return m_maxResultsCount;
	Logger::Info("ShapeConfig -> GetMaxResultCount() End");
}

int ShapeConfig::GetResultsCount()
{
	Logger::Info("ShapeConfig -> GetResultsCount() Start");
	return m_resultsCount;
	Logger::Info("ShapeConfig -> GetResultsCount() End");
}

/*
* Description - GetIsModelExecuted() return a flag wether Rest call is being made and module is already executed, used for progress bar to work correctly
* Parameter -
* Exception -
* Return - bool
*/
bool ShapeConfig::GetIsModelExecuted()
{
	return m_isModelExecuted;
}

/*
* Description - SetIsModelExecuted() sets a flag wether Rest call is being made and module is already executed, used for progress bar to work correctly
* Parameter -
* Exception -
* Return -
*/
void ShapeConfig::SetIsModelExecuted(bool _isModelExecuted)
{
	m_isModelExecuted = _isModelExecuted;
}


void ShapeConfig::InitializeShapeData()
{
	Logger::Info("Configuration::InitializeShapeData() Started..");
	ShapeConfig::GetInstance()->SetMaximumLimitForShapeResult();
	GetShapeConfigJSON();
	Logger::Info("Configuration::InitializeShapeData() End..");
}

/*
* Description - SetMaximumLimitForColorResult() method used to set the maximum limit for Color Results in search response.
* Parameter -
* Exception -
* Return -
*/
void ShapeConfig::SetMaximumLimitForShapeResult()
{
	m_maxShapeResultsLimit = Helper::GetJSONValue<string>(Configuration::GetInstance()->GetPLMPluginConfigJSON(), MAX_SHAPE_SEARCH_LIMIT, true);
}

/*
* Description - GetMaximumLimitForColorResult() method used to get the maximum limit for Color Results in search response.
* Parameter -
* Exception -
* Return - string
*/
string ShapeConfig::GetMaximumLimitForShapeResult()
{
	return m_maxShapeResultsLimit;
}

/*
* Description - ResetShapeConfig() is to set basic need jsons and some other attributs as default value.
* Parameter -
* Exception -
* Return -
*/
void ShapeConfig::ResetShapeConfig()
{
	Logger::Info("INFO::ShapeConfig: ResetShapeConfig()-> Start");
	m_shapeConfigJson = nullptr;
	m_shapeFieldsJson = nullptr;
	m_isModelExecuted = false;
	m_sortedColumnNumber = 0;
	SetIsModelExecuted(m_isModelExecuted);
	m_shapeLoggedOut = true;
	PublishToPLMData::GetInstance()->m_createShapeLoggedOut = true;
	json configJson = nullptr;
	PublishToPLMData::GetInstance()->SetActive3DModelMetaData(configJson);
	Logger::Info("INFO::ShapeConfig: ResetShapeConfig()-> End");
}