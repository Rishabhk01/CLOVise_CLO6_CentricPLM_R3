/*
* Copyright 2020-2021 CLO-Vise. All rights reserved
*
* @file PLMMaterialData.cpp
*
* @brief Class implementation for cach Material data in CLO from PLM.
* This class has all the variable declarations and function implementation which are used in storing PLM Material configuration related data to search in CLO
*
* @author GoVise
*
* @date 16-JUN-2020
*/
#include <time.h>

#include "MaterialConfig.h"
#include "CLOAPIInterface.h"
#include "CLOVise/PLM/Helper/Util/RestAPIUtility.h"
#include "CLOVise/PLM/Helper/Util/FormatHelper.h"
#include "CLOVise/PLM/Helper/Util/Logger.h"
#include "CLOVise/PLM/Helper/Util/DataHelper.h"

using json = nlohmann::json;

MaterialConfig* MaterialConfig::_instance = NULL;
MaterialConfig* MaterialConfig::GetInstance()
{
	if (_instance == NULL)
	{
		_instance = new MaterialConfig();
	}
	return _instance;
}
void  MaterialConfig::Destroy()
{
	if (_instance)
	{
		delete _instance;
		_instance = NULL;
	}
}

/*
* Description - SetMaterialConfigJSON() method used to get the configuration data from server/file.
* Parameter -  string.
* Exception - exception, Char *
* Return -
*/
void MaterialConfig::SetMaterialConfigJSONFromFile(string _module, string _rootType, bool _initDepedentJsons)
{
	m_materialConfigJson.clear();
	Logger::Info("MaterialData -> GetMaterialConfigJSONFromFile() start");

	switch (Configuration::GetInstance()->GetCurrentScreen())
	{

	case CREATE_MATERIAL_CLICKED:
	{
		m_materialConfigJson = DataHelper::GetInstance()->SetConfigJSON(_module, _rootType, _initDepedentJsons, PLM_MATERIAL_CREATE_FILE_NAME);
		break;
	}
	case UPDATE_MATERIAL_CLICKED:
	{
		m_materialConfigJson = DataHelper::GetInstance()->SetConfigJSON(_module, _rootType, _initDepedentJsons, PLM_MATERIAL_UPDATE_FILE_NAME);
		break;
	}
	case SEARCH_MATERIAL_CLICKED:
	{
		m_materialConfigJson = DataHelper::GetInstance()->SetConfigJSON(_module, _rootType, _initDepedentJsons, PLM_MATERIAL_SEARCH_FILE_NAME);
		break;
	}
	}
	//if (_matFucntion == SEARCH_MATERIAL) {
	//	m_materialConfigJson = DataHelper::GetInstance()->SetConfigJSON(_module, _rootType, _initDepedentJsons, PLM_MATERIAL_SEARCH_FILE_NAME);
	//}
	//else if (_matFucntion == CREATE_MATERIAL) {
	//	m_materialConfigJson = DataHelper::GetInstance()->SetConfigJSON(_module, _rootType, _initDepedentJsons, PLM_MATERIAL_CREATE_FILE_NAME);
	//}
	//else if (_matFucntion == UPDATE_MATERIAL) {
	//	m_materialConfigJson = DataHelper::GetInstance()->SetConfigJSON(_module, _rootType, _initDepedentJsons, PLM_MATERIAL_UPDATE_FILE_NAME);
	//}
	//m_materialConfigJson = DataHelper::GetInstance()->SetConfigJSON(_module, _rootType, _initDepedentJsons, PLM_MATERIAL_SEARCH_FILE_NAME);
	Logger::Info("MaterialData -> GetMaterialConfigJSONFromFile() End");
}

/*
* Description - SetMaterialConfigJSON() method used to get the configuration data from server/file.
* Parameter -  string.
* Exception - exception, Char *
* Return -
*/
//void MaterialConfig::SetMaterialConfigJSON(string _module, string _rootType, bool _initDepedentJsons)
//{
//	Logger::Info("MaterialData -> SetMaterialConfigJSON() start");
//	m_materialConfigJson = DataHelper::GetInstance()->SetConfigJSON(_module, _rootType, _initDepedentJsons, PLM_MATERIAL_FILE_NAME);
//	Logger::Info("MaterialData -> SetMaterialConfigJSON() End");
//}

/*
* Description - GetMaterialConfigJSON() method used to get material configuration json.
* Parameter -
* Exception -
* Return - json.
*/
json MaterialConfig::GetMaterialConfigJSONFromFile()
{
	Logger::Info("MaterialConfig -> GetMaterialConfigJSONFromFile() start");

	//if (m_materialConfigJson == nullptr || m_materialConfigJson.empty())
		SetMaterialConfigJSONFromFile(MATERIAL_MODULE, MATERIAL_ROOT_TYPE, false);
	Logger::Info("PLMMaterialData -> GetMaterialConfigJSONFromFile() end");

	return m_materialConfigJson;
}

/*
* Description - GetMaterialConfigJSON() method used to get material configuration json.
* Parameter -
* Exception -
* Return - json.
*/
json MaterialConfig::GetMaterialConfigJSON()
{
	Logger::Info("MaterialConfig -> GetMaterialConfigJSON() start");

	//if (m_materialFieldsJson == nullptr || m_materialFieldsJson.empty())
		SetMaterialConfigJSON();
	Logger::Info("PLMMaterialData -> GetMaterialConfigJSON() end");

	return m_materialFieldsJson;
}

/*
* Description - GetMaterialHierarchyJSON() method used to get cached material hierarchy json.
* Parameter -
* Exception -
* Return - json.
*/
json MaterialConfig::GetMaterialHierarchyJSON()
{
	Logger::Info("MaterialConfig -> GetMaterialHierarchyJSON() start");

	if (m_materialHierarchyJson == nullptr || m_materialHierarchyJson.empty())
		m_materialHierarchyJson = DataHelper::GetInstance()->SetJson(HIERARCHY_JSON_KEY, m_materialConfigJson);
	//SetMaterialHierarchyJSON(HIERARCHY_JSON_KEY);

	Logger::Info("MaterialConfig -> GetMaterialHierarchyJSON() end");

	return m_materialHierarchyJson;
}

/*
* Description - GetAttScopes() method used to get cached the attribute scopes data.
* Parameter -
* Exception -
* Return - QStringList.
*/
QStringList MaterialConfig::GetAttScopes()
{
	Logger::Info("MaterialConfig -> GetAttScopes() start");

	if (m_attScopes.empty())
		DataHelper::GetInstance()->SetAttScopes(m_materialConfigJson, ATTSCOPE_JSON_KEY, m_attScopes);
	Logger::Info("MaterialConfig -> GetAttScopes() end");

	return m_attScopes;
}

/*
* Description - GetMaterialFieldsJSON() method used to get cached the material fields json.
* Parameter -
* Exception -
* Return - json.
*/
json MaterialConfig::GetMaterialFieldsJSON()
{
	Logger::Info("MaterialConfig -> GetMaterialFieldsJSON() start");

	if (m_materialFieldsJson == nullptr || m_materialFieldsJson.empty())
		//m_materialFieldsJson = DataHelper::GetInstance()->SetJson(FIELDLIST_JSON_KEY, m_materialConfigJson);
		SetMaterialConfigJSON();
	Logger::Info("MaterialConfig -> GetMaterialFieldsJSON() end");

	return m_materialFieldsJson;
}

/*
* Description - SetMaterialPresetJSON() method used to cache the material preset json.
* Parameter -  string.
* Exception - exception, Char *
* Return -
*/
void MaterialConfig::SetMaterialPresetJSON(string _presetJsonKey)
{
	Logger::Info("MaterialConfig -> SetMaterialPresetJSON() start");

	try
	{
		string materialPresetStr = "";
		materialPresetStr = Helper::GetJSONValue<string>(GetMaterialConfigJSON(), _presetJsonKey, false);
		m_materialPresetJson = json::parse(materialPresetStr);
	}
	catch (string msg)
	{
		Logger::Error("MaterialConfig -> SetMaterialPresetJSON Exception :: " + msg);
		throw msg;
	}
	catch (exception& e)
	{
		Logger::Error("MaterialConfig -> SetMaterialPresetJSON Exception :: " + string(e.what()));
		throw e;
	}
	catch (const char* msg)
	{
		Logger::Error("MaterialConfig -> SetMaterialPresetJSON Exception :: " + string(msg));
		throw msg;
	}
	Logger::Info("MaterialConfig -> SetMaterialPresetJSON() end");

}

/*
* Description - SetMaterialFilterJSON() method used to cache the material filter json from rest/file.
* Parameter -  string, string, string.
* Exception - exception, Char *
* Return -
*/
void MaterialConfig::SetMaterialFilterJSON(string _module, string _rootType, string _activity)
{
	Logger::Info("MaterialConfig -> SetMaterialFilterJSON() start");
	m_materialFilterJson = DataHelper::GetInstance()->SetFilterJSON(_module, _rootType, _activity, PLM_MATERIAL_FILTER);
	Logger::Info("MaterialConfig -> SetMaterialFilterJSON() end");
}

/*
* Description - GetMaterialFilterJSON() method used to get cached material filter json.
* Parameter -
* Exception -
* Return - json.
*/
json MaterialConfig::GetMaterialFilterJSON()
{
	Logger::Info("MaterialConfig -> GetMaterialFilterJSON() start");

	if (m_materialFilterJson == nullptr || m_materialFilterJson.empty())
		SetMaterialFilterJSON(MATERIAL_MODULE, MATERIAL_ROOT_TYPE, MATERIAL_SEARCH_ACTIVITY);
	Logger::Info("MaterialConfig -> GetMaterialFilterJSON() end");

	return m_materialFilterJson;
}

/*
* Description - SetMaterialViewJSON() method used to cache the material view json from rest/file.
* Parameter -  string, string, string.
* Exception - exception, Char *
* Return -
*/
void MaterialConfig::SetMaterialViewJSON(string _module, string _rootType, string _activity)
{
	Logger::Info("MaterialConfig -> SetMaterialViewJSON() start");
	m_materialViewJson = DataHelper::GetInstance()->SetViewJSON(_module, _rootType, _activity, PLM_MATERIAL_VIEW);
	Logger::Info("MaterialConfig -> SetMaterialViewJSON() end");
}

/*
* Description - GetMaterialViewJSON() method used to get cached material view json.
* Parameter -
* Exception -
* Return - json.
*/
json MaterialConfig::GetMaterialViewJSON()
{
	Logger::Info("MaterialConfig -> GetMaterialViewJSON() start");

	if (m_materialViewJson == nullptr || m_materialViewJson.empty())
		SetMaterialViewJSON(MATERIAL_MODULE, MATERIAL_ROOT_TYPE, MATERIAL_SEARCH_ACTIVITY);
	Logger::Info("MaterialConfig -> GetMaterialViewJSON() end");

	return m_materialViewJson;
}

/*
* Description - GetSeasonPaletteJSON() method used to get cached the season palette json.
* Parameter -  bool.
* Exception -
* Return - json.
*/
json MaterialConfig::GetSeasonPaletteJSON(bool _refresh)
{
	Logger::Info("MaterialConfig -> GetSeasonPaletteJSON() start");

	if (_refresh)
	{
		m_paletteJson = DataHelper::GetInstance()->SetSeasonPaletteJSON(PLM_MATERIAL_PALETTE);
	}
	Logger::Info("MaterialConfig -> GetSeasonPaletteJSON() end");

	return m_paletteJson;
}

/*
* Description - SetSelectedViewIdx() method used to cache the selected view index.
* Parameter -  int.
* Exception -
* Return -
*/
void MaterialConfig::SetSelectedViewIdx(int _selectedViewIdx)
{
	Logger::Info("MaterialConfig -> SetSelectedViewIdx() Start");
	m_selectedViewIdx = _selectedViewIdx;
	Logger::Info("MaterialConfig -> SetSelectedViewIdx() End");
}

/*
* Description - GetSelectedViewIdx() method used to get cached selected view index.
* Parameter -
* Exception -
* Return - int.
*/
int MaterialConfig::GetSelectedViewIdx()
{
	Logger::Info("MaterialConfig -> GetSelectedViewIdx() Start");
	return m_selectedViewIdx;
	Logger::Info("MaterialConfig -> GetSelectedViewIdx() End");
}

/*
* Description - SetSearchCriteriaJSON() method used to cache the search criteria json.
* Parameter -  json.
* Exception -
* Return -
*/
void MaterialConfig::SetSearchCriteriaJSON(json _criteria)
{
	Logger::Info("MaterialConfig -> SetSelectedViewIdx() Start");
	m_searchCriteriaJson = _criteria;
	Logger::Info("MaterialConfig -> SetSelectedViewIdx() End");
}

/*
* Description - GetSearchCriteriaJSON() method used to get cached search criteria json.
* Parameter -
* Exception -
* Return - json.
*/
json MaterialConfig::GetSearchCriteriaJSON()
{
	Logger::Info("MaterialConfig -> GetSearchCriteriaJSON() Start");
	return m_searchCriteriaJson;
	Logger::Info("MaterialConfig -> GetSearchCriteriaJSON() End");
}

/*
* Description - SetDateFlag() method used to set date exist flag.
* Parameter -  bool.
* Exception -
* Return -
*/
void MaterialConfig::SetDateFlag(bool _isDateEditPresent)
{
	Logger::Info("MaterialConfig -> SetDateFlag() Start");
	m_isDateEditPresent = _isDateEditPresent;
	Logger::Info("MaterialConfig -> SetDateFlag() End");
}

/*
* Description - GetDateFlag() method used to get date exist flag.
* Parameter -
* Exception -
* Return - bool.
*/
bool MaterialConfig::GetDateFlag()
{
	Logger::Info("MaterialConfig -> GetDateFlag() Start");
	return m_isDateEditPresent;
	Logger::Info("MaterialConfig -> GetDateFlag() End");
}



void  MaterialConfig::UpdateResultJson(json& _materialResults, json& _materialTypeValues)
{
	string objectName = Helper::GetJSONValue<string>(_materialResults, NODE_NAME_KEY, true);
	string objectId = Helper::GetJSONValue<string>(_materialResults, "id", true);

	string materialType = Helper::GetJSONValue<string>(_materialResults, "product_type", true);
	string materialAssert = Helper::GetJSONValue<string>(_materialResults, "default_3d_material", true);
	_materialResults["material_type"] = Helper::GetJSONValue<string>(_materialTypeValues, materialType, true);
	/*for (const auto& itrValues : _materialTypeValues.items())
	{
		string value = itrValues.value();
		if (!FormatHelper::HasContent(value))
			value = "";
		if (materialType != itrValues.key())
			continue;		
		_materialResults["material_type"] = value;
	}*/
	//UTILITY_API->DisplayMessageBox("materialType::" + materialType);
	_materialResults[OBJECT_NAME_KEY] = objectName;
	_materialResults[OBJECT_ID_KEY] = objectId;
	_materialResults[DFAULT_ASSET_KEY] = materialAssert;
	//_materialResults["thumbnail"] =  GetThumbnailUrl(objectId);

}

string MaterialConfig::GetThumbnailUrl(string _objectId)
{
	string thumbnailUrl="";
	try
	{
		json imageResultJson = json::object();
		auto startTime = std::chrono::high_resolution_clock::now();
		string resultResponse = RESTAPI::CentricRestCallGet(Configuration::GetInstance()->GetPLMServerURL() + RESTAPI::SEARCH_MATERIAL_API + "/" + _objectId + "/images?decode=true", APPLICATION_JSON_TYPE, "");
		auto finishTime = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double> totalDuration = finishTime - startTime;
		Logger::perfomance(PERFOMANCE_KEY + "Material images API :: " + to_string(totalDuration.count()));
		Logger::RestAPIDebug("resultResponse::" + resultResponse);
		if (FormatHelper::HasError(resultResponse))
		{
			thumbnailUrl = "";
		}
		else
		{
			imageResultJson = json::parse(resultResponse);
			for (size_t imageResultJsonCount = 0; imageResultJsonCount < imageResultJson.size(); imageResultJsonCount++)
			{
				json imageJson = Helper::GetJSONParsedValue<int>(imageResultJson, imageResultJsonCount, false);
				string thumbnailId = Helper::GetJSONValue<string>(imageJson, THUMBNAIL_KEY, true);
				thumbnailUrl = Helper::GetJSONValue<string>(imageJson, "_url_base_template", true);
				thumbnailUrl = Helper::FindAndReplace(thumbnailUrl, "%s", thumbnailId);
			}
		}
	}
	catch (string msg)
	{
		Logger::Error("MaterialConfig::GetThumbnailUrl() Exception - " + msg);
	}
	catch (exception & e)
	{
		Logger::Error("MaterialConfig::GetThumbnailUrl() Exception - " + string(e.what()));
	}
	catch (const char* msg)
	{
		wstring wstr(msg, msg + strlen(msg));
		Logger::Error("MaterialConfig::GetThumbnailUrl() Exception - " + string(msg));
	}
	return thumbnailUrl;
}

void MaterialConfig::GetEnumlistJson(string _restApi, json& _attributeJson)
{
	try 
	{
		json enumList;
		string enumListResponse = RESTAPI::CentricRestCallGet(_restApi, APPLICATION_JSON_TYPE, "");
		Logger::RestAPIDebug("enumListResponse::" + enumListResponse);
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
		Logger::Error("MaterialConfig::GetEnumlistJson() Exception - " + msg);
	}
	catch (exception & e)
	{
		Logger::Error("MaterialConfig::GetEnumlistJson() Exception - " + string(e.what()));
	}
	catch (const char* msg)
	{
		wstring wstr(msg, msg + strlen(msg));
		Logger::Error("MaterialConfig::GetEnumlistJson() Exception - " + string(msg));
	}
}

void MaterialConfig::ConvertToo(string& _convertValue, bool _isLower)
{
	string value;
	string space = " ";
	string underscore = "_";
	try
	{
		if (_isLower)
		{
			for (int i = 0; i < _convertValue.length(); i++)
			{
				if (i == 0 || !isupper(_convertValue[i]) && _convertValue[i] != space[0])
				{
					value = value + _convertValue[i];
				}
				else if (isupper(_convertValue[i]))
				{
					value = value + space + _convertValue[i];
				}
			}
			transform(value.begin(), value.end(), value.begin(), ::tolower);
		}
		else
		{
			for (int i = 0; i < _convertValue.length(); i++)
			{
				if (i == 0)
				{
					value = value + _convertValue[i];
				}
				else if (islower(_convertValue[i]))
				{
					value = value + space + _convertValue[i];
				}
				else
				{
					value = value + _convertValue[i];
				}
			}
			transform(value.begin(), value.end(), value.begin(), ::toupper);
		}
		_convertValue = Helper::FindAndReplace(value, space, underscore);
	}
	catch (string msg)
	{
		Logger::Error("MaterialConfig::ConvertToo() Exception - " + msg);
	}
	catch (exception & e)
	{
		Logger::Error("MaterialConfig::ConvertToo() Exception - " + string(e.what()));
	}
	catch (const char* msg)
	{
		wstring wstr(msg, msg + strlen(msg));
		Logger::Error("MaterialConfig::ConvertToo() Exception - " + string(msg));
	}
}

void MaterialConfig::SetMaterialConfigJSON()
{
	try
	{
		m_materialFieldsJson.clear();
		auto startTime = std::chrono::high_resolution_clock::now();
		string initialConfigJsonString = RESTAPI::CentricRestCallGet(Configuration::GetInstance()->GetPLMServerURL() + RESTAPI::SEARCH_ATT_API, APPLICATION_JSON_TYPE, "skip=0&decode=true&limit=" + Configuration::GetInstance()->GetMaximumLimitForRefAttValue() + "&node_name=Library Item, Material");
		auto finishTime = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double> totalDuration = finishTime - startTime;
		Logger::perfomance(PERFOMANCE_KEY + "Library Item, Material API :: " + to_string(totalDuration.count()));
		Logger::RestAPIDebug("initialConfigJsonString::" + initialConfigJsonString);
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
		//json defaultFieldsJson = MaterialConfig::GetInstance()->GetMaterialConfigJSON();
		json defaultFieldsJson = MaterialConfig::GetInstance()->GetMaterialConfigJSONFromFile();
		createFieldsJson(fieldsconfigJsonString, defaultFieldsJson);
	}
    catch (string msg)
	{
		Logger::Error("MaterialConfig::SetMaterialConfigJSON() Exception - " + msg);
		throw msg;
	}
	catch (exception& e)
	{

		Logger::Error("MaterialConfig::SetMaterialConfigJSON() Exception - " + string(e.what()));
		throw e.what();
	}
	catch (const char* msg)
	{

		Logger::Error("MaterialConfig::SetMaterialConfigJSON() Exception - "+ string(msg));
		throw msg;
	}
		
}

void MaterialConfig::createFieldsJson(string& _fieldsJsonStringResponse, json& _defaultFieldsJson)
{
	json fieldsJsonResponse = json::parse(_fieldsJsonStringResponse);
	string attValue;
	string attRequired;
	string attType;
	string attKey;
	string isAttEnable;
	string isAttFlag;
	string isCreateOnlyFlag;
	string attDefaultValue;
	json fieldsListjson = json::array();
	string attApiExposed;
	json fieldsJson;
	fieldsJson["typeName"] = "Material";
	QStringList attKeyList;
	int fieldsJsonCount = 0;
	int fieldCount = 0;
	
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
			isAttFlag = Helper::GetJSONValue<string>(feildsvaluesJson, ATT_RESTRICT_EDIT, true);
			isCreateOnlyFlag = Helper::GetJSONValue<string>(feildsvaluesJson, CREATE_ONLY, true);
			attValue = Helper::GetJSONValue<string>(feildsvaluesJson, DISPLAY_NAME_KEY, true);
			attRequired = Helper::GetJSONValue<string>(feildsvaluesJson, ATTRIBUTE_REQUIRED, true);
			attDefaultValue = Helper::GetJSONValue<string>(feildsvaluesJson, INITIAL_VALUE_KEY, true);
			attApiExposed = Helper::GetJSONValue<string>(feildsvaluesJson, ATTRIBUTE_REST_API_EXPOSED, true);
			/*int last = attKey.find_last_of('/');
			attKey = attKey.substr(last + 1);
			MaterialConfig::GetInstance()->ConvertToo(attKey, true);*/
			if (attType != REF_ATT_TYPE_KEY && attType != REFLIST_ATT_TYPE_KEY)
			{
				attKeyList << QString::fromStdString(attKey);
				if (attType == "enum")
				{
					DataHelper::GetInstance()->SetEnumJson(fieldsAttJson, feildsvaluesJson);
				}
				fieldsAttJson["attKey"] = attKey;
				fieldsAttJson["attType"] = attType;
				fieldsAttJson["attScope"] = "MATERIAL";
				fieldsAttJson["attName"] = attValue;
				fieldsAttJson["isAttSearchable"] = isAttEnable;
				fieldsAttJson["isAttSettable"] = isAttFlag;
				fieldsAttJson["isAttUpdatable"] = isCreateOnlyFlag;
				fieldsAttJson[ATTRIBUTE_REQUIRED_KEY] = attRequired;
				fieldsAttJson[ATTRIBUTE_DEFAULT_VALUE_KEY] = attDefaultValue;
				fieldsAttJson["isMultiselectable"] = true;
				fieldsAttJson[IS_CHECK_KEY] = true;
				fieldsAttJson[IS_SINGLE_INTERGER_KEY] = true;
				fieldsAttJson[IS_SINGLE_FLOAT_KEY] = true;
				fieldsAttJson[ATTRIBUTE_REST_API_EXPOSED] = attApiExposed;
				if (FormatHelper::HasContent(to_string(fieldsAttJson)))
				{
					fieldsJson["attributes"].emplace_back(fieldsAttJson);
				}
			}
		}

		if (Configuration::GetInstance()->GetCurrentScreen() == SEARCH_MATERIAL_CLICKED)
		{
			GetDataFromLocalJson(fieldsJson, _defaultFieldsJson, attKeyList);
		}
		if (Configuration::GetInstance()->GetCurrentScreen() == CREATE_MATERIAL_CLICKED || Configuration::GetInstance()->GetCurrentScreen() == UPDATE_MATERIAL_CLICKED)
		{
			GetDataFromLocalJson(fieldsJson, _defaultFieldsJson, attKeyList);
		}
		fieldsListjson[0] = fieldsJson;
		m_materialFieldsJson = fieldsListjson;
		Logger::Debug("DEBUG::MaterialConfig: createFieldsJson() -> m_materialFieldsJson :: " + to_string(m_materialFieldsJson));
		Logger::Debug("DEBUG::MaterialConfig: createFieldsJson() -> m_materialFieldsJson.size() :: " + to_string(m_materialFieldsJson.size()));
	}
	catch (string msg)
	{
		Logger::Error("MaterialConfig->createFieldsJson() Exception - " + msg);
	}
	catch (exception & e)
	{
		Logger::Error("MaterialConfig->createFieldsJson() Exception - " + string(e.what()));
	}
	catch (const char* msg)
	{
		wstring wstr(msg, msg + strlen(msg));
		Logger::Error("MaterialConfig->createFieldsJson() Exception - " + string(msg));
	}
}

void MaterialConfig::GetDataFromLocalJson(json& _fieldsJson, json _defaultFieldsJson, QStringList& _attKeyList)
{
	try 
	{
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
				if (!_attKeyList.contains(QString::fromStdString(defaultAttributeKey)))
				{
					//_fieldsJsonCount = _fieldsJsonCount + 1;
					if (attType == REF_ATT_TYPE_KEY)
					{
						DataHelper::GetInstance()->SetRefJson(attributesFieldsListJson, defaultAttributeKey);
					}
					//if (defaultAttributeKey == "material_original_season")
					//{
					//	string seasonApi = Configuration::GetInstance()->GetPLMServerURL() + refUrl + "?skip=0&limit=1000&sort=node_name";
					//	//UTILITY_API->DisplayMessageBox("seasonApi::" + seasonApi);
					//	MaterialConfig::GetInstance()->GetEnumlistJson(seasonApi, attributesFieldsListJson);
					//}
					//else if (defaultAttributeKey == "material_security_groups")
					//{
					//	string materialSecurityApi = Configuration::GetInstance()->GetPLMServerURL() + refUrl + "?skip=0&limit=1000&sort=node_name";
					//	MaterialConfig::GetInstance()->GetEnumlistJson(materialSecurityApi, attributesFieldsListJson);
					//}
					//else if (defaultAttributeKey == "product_type")
					//{
					//	string materialTypeApi = Configuration::GetInstance()->GetPLMServerURL() + refUrl + "?skip=0&limit=1000&sort=node_name";
					//	//UTILITY_API->DisplayMessageBox("materialTypeApi::" + materialTypeApi);
					//	MaterialConfig::GetInstance()->GetEnumlistJson(materialTypeApi, attributesFieldsListJson);
					//}
					//else if (defaultAttributeKey == "lib_materials") {
					//	string materialLibraries = Configuration::GetInstance()->GetPLMServerURL() + refUrl + "?skip=0&limit=1000&sort=node_name";

					//	//string materialTypeApi = Configuration::GetInstance()->GetPLMServerURL() + RESTAPI::MATERIAL_TYPE_SEARCH_API + "?skip=0&limit=1000&sort=node_name";
					//	//UTILITY_API->DisplayMessageBox("materialTypeApi::" + materialTypeApi);
					//	MaterialConfig::GetInstance()->GetEnumlistJson(materialLibraries, attributesFieldsListJson);
					//}
					if (FormatHelper::HasContent(to_string(attributesFieldsListJson)))
					{
						_fieldsJson["attributes"].push_back(attributesFieldsListJson);
					}
				}
			}
		}
	}
	catch (string msg)
	{
		Logger::Error("MaterialConfig::GetDataFromLocalJson() Exception - " + msg);
	}
	catch (exception & e)
	{
		Logger::Error("MaterialConfig::GetDataFromLocalJson() Exception - " + string(e.what()));
	}
	catch (const char* msg)
	{
		wstring wstr(msg, msg + strlen(msg));
		Logger::Error("MaterialConfig::GetDataFromLocalJsons() Exception - " + string(msg));
	}
}

void MaterialConfig::SetDataFromResponse(json _param)
{
	//for now...
	Logger::Info("MaterialConfig -> setDataFromResponse() -> Start");

	try
	{
		//string resultJsonString = RESTAPI::RESTMethodSubmit(RESTAPI::SEARCH_RESULTS_API, _param);
		string parameter = "";
		string matLibValue = "";
		bool isLibrarySearch = false;
		json attributesJson = Helper::GetJSONParsedValue<string>(_param, ATTRIBUTES_KEY, false);
		for (int attributesJsonCount = 0; attributesJsonCount < attributesJson.size(); attributesJsonCount++)
		{
			string attKey = Helper::GetJSONParsedValue<string>(attributesJson[attributesJsonCount], ATTRIBUTE_KEY, false);
			string attValue = Helper::GetJSONParsedValue<string>(attributesJson[attributesJsonCount], ATTRIBUTE_VALUE_KEY, false);
			string attType = Helper::GetJSONParsedValue<string>(attributesJson[attributesJsonCount], ATTRIBUTE_TYPE_KEY, false);
			if (attKey == "lib_materials") {
				matLibValue = attValue;
				isLibrarySearch = true;
				continue;
			}
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
		//UTILITY_API->DisplayMessageBox(parameter);
		string resultResponse = "";
		if (isLibrarySearch) {
			resultResponse = RESTAPI::CentricRestCallGet(Configuration::GetInstance()->GetPLMServerURL() + RESTAPI::MATERIAL_SEARCH_API_LIB + "/" + matLibValue +"/elements?", APPLICATION_JSON_TYPE, parameter + "&decode=true&limit="+ MaterialConfig::GetInstance()->GetMaximumLimitForMaterialResult());
		}
		else {
			auto startTime = std::chrono::high_resolution_clock::now();
			resultResponse = RESTAPI::CentricRestCallGet(Configuration::GetInstance()->GetPLMServerURL() + RESTAPI::SEARCH_MATERIAL_API + "?", APPLICATION_JSON_TYPE, parameter + "&decode=true&limit="+ MaterialConfig::GetInstance()->GetMaximumLimitForMaterialResult());
			auto finishTime = std::chrono::high_resolution_clock::now();
			std::chrono::duration<double> totalDuration = finishTime - startTime;
			Logger::perfomance(PERFOMANCE_KEY + "Search Results API :: " + to_string(totalDuration.count()));
		}
		Logger::RestAPIDebug("resultResponse main::" + resultResponse);

		if (!FormatHelper::HasContent(resultResponse))
		{
			throw "Unable to fetch results. Please try again or Contact your System Administrator.";
		}

		/*if (QString::fromStdString(resultResponse).contains("message"))
		{
			json errorRsponse = json::parse(resultResponse);
			resultResponse = Helper::GetJSONValue<string>(errorRsponse, "message", true);
		}
		throw runtime_error(resultResponse);*/

		if (FormatHelper::HasError(resultResponse))
		{
			Helper::GetCentricErrorMessage(resultResponse);
			throw runtime_error(resultResponse);
		}
		json materialResults = json::array();
		if (FormatHelper::HasContent(resultResponse))
			materialResults = json::parse(resultResponse);
		Logger::Logger("total count of result::" + to_string(materialResults.size()));
		auto mTypestartTime = std::chrono::high_resolution_clock::now();
		string materialTypeResponse = RESTAPI::CentricRestCallGet(Configuration::GetInstance()->GetPLMServerURL() + RESTAPI::MATERIAL_TYPE_SEARCH_API + "?", APPLICATION_JSON_TYPE, "&decode=true&limit="+ Configuration::GetInstance()->GetMaximumLimitForRefAttValue()+"&sort=node_name");
		auto mTypefinishTime = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double> mTypetotalDuration = mTypefinishTime - mTypestartTime;
		Logger::perfomance(PERFOMANCE_KEY + "Material Types API :: " + to_string(mTypetotalDuration.count()));
		Logger::RestAPIDebug("materialTypeResponse::" + materialTypeResponse);
		json materialTypeValuesJson;
		if (FormatHelper::HasError(materialTypeResponse) || !FormatHelper::HasContent(materialTypeResponse))
		{
			//throw runtime_error(materialTypeResponse);
			materialTypeValuesJson.clear();
		}
		else
		{
			json materialTypeResponseJson = json::parse(materialTypeResponse);
			for (auto count = 0; count < materialTypeResponseJson.size(); count++)
			{
				string materialTypeName = Helper::GetJSONValue<string>(materialTypeResponseJson[count], NODE_NAME_KEY, true);
				string materialTypeId = Helper::GetJSONValue<string>(materialTypeResponseJson[count], "id", true);
				materialTypeValuesJson[materialTypeId] = materialTypeName;
			}
		}
		m_materialResults.clear();
		for (int i = 0; i < materialResults.size(); i++)
		{
			string resultListStr = Helper::GetJSONValue<int>(materialResults, i, false);
			json resultListJson = json::parse(resultListStr);
			string attachmentId = Helper::GetJSONValue<string>(resultListJson, "default_3d_material", true);
			if (Configuration::GetInstance()->GetCurrentScreen() == SEARCH_MATERIAL_CLICKED)
			{
				if (attachmentId == "centric%3A" || !FormatHelper::HasContent(attachmentId))
					continue;
				Logger::Debug("attachmentId inside::" + attachmentId);
			}
			MaterialConfig::GetInstance()->UpdateResultJson(resultListJson, materialTypeValuesJson);
			m_materialResults.push_back(resultListJson);
		}
		Logger::Debug("m_materialResults::" + to_string(m_materialResults));
		//MaterialConfig::GetInstance()->UpdateResultJson();
		string resultsCount = to_string(m_materialResults.size());
		Logger::Debug("updated result count::" + resultsCount);
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
		m_typename = "Material";
		string maxResultsLimit = resultsCount;//= Helper::GetJSONValue<string>(m_materialResults, "maxResultsLimit", true);
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
		Logger::Error("PLMMaterialResult -> SetDataFromResponse Exception :: " + msg);
		throw msg;
	}
	catch (exception& e)
	{
		Logger::Error("PLMMaterialResult -> SetDataFromResponse Exception :: " + string(e.what()));
		//UTILITY_API->DisplayMessageBox(e.what());
		throw e;
	}
	catch (const char* msg)
	{
		Logger::Error("PLMMaterialResult -> SetDataFromResponse Exception :: " + string(msg));
		throw msg;
	}
}

json MaterialConfig::GetMaterialResultsSON()
{
	Logger::Info("MaterialConfig -> GetSearchCriteriaJSON() Start");
	return m_materialResults;
	Logger::Info("MaterialConfig -> GetMaterialResultsSON() End");
}

string MaterialConfig::GetTypename()
{
	Logger::Info("MaterialConfig -> GetTypename() Start");
	return m_typename;
	Logger::Info("MaterialConfig -> GetTypename() End");
}

int MaterialConfig::GetMaxResultCount()
{
	Logger::Info("MaterialConfig -> GetMaxResultCount() Start");
	return m_maxResultsCount;
	Logger::Info("MaterialConfig -> GetMaxResultCount() End");
}

int MaterialConfig::GetResultsCount()
{
	Logger::Info("MaterialConfig -> GetResultsCount() Start");
	return m_resultsCount;
	Logger::Info("MaterialConfig -> GetResultsCount() End");
}
/*
* Description - GetIsModelExecuted() return a flag wether Rest call is being made and module is already executed, used for progress bar to work correctly
* Parameter -
* Exception -
* Return - bool
*/
bool MaterialConfig::GetIsModelExecuted()
{
	return m_isModelExecuted;
}

/*
* Description - SetIsModelExecuted() sets a flag wether Rest call is being made and module is already executed, used for progress bar to work correctly
* Parameter -
* Exception -
* Return -
*/
void MaterialConfig::SetIsModelExecuted(bool _isModelExecuted)
{
	m_isModelExecuted = _isModelExecuted;
}

/*
* Description - SetUpdateMaterialCacheData() method used to cache downloaded file name.
* Parameter -  string.
* Exception -
* Return -
*/
void MaterialConfig::SetUpdateMaterialCacheData(json _updateMaterialData)
{
	m_updateMaterialData = _updateMaterialData;
}

/*
* Description - GetUpdateMaterialCacheData() method used to get cached downloaded file name.
* Parameter -
* Exception -
* Return - string.
*/
json MaterialConfig::GetUpdateMaterialCacheData()
{
	return m_updateMaterialData;
}
/*
* Description - SetIsRadioButton() method used to cache downloaded file name.
* Parameter -  string.
* Exception -
* Return -
*/
void MaterialConfig::SetIsRadioButton(bool _setIsRadioButton)
{
	m_setIsRadioButton = _setIsRadioButton;
}

/*
* Description - GetIsRadioButton() method used to get cached downloaded file name.
* Parameter -
* Exception -
* Return - string.
*/
bool MaterialConfig::GetIsRadioButton()
{
	return m_setIsRadioButton;
}

void MaterialConfig::InitializeMaterialData()
{
	Logger::Info("MaterialConfig::InitializeMaterialData() Started..");
	/*Configuration::GetInstance()->SetProgressBarProgress(0);
	UTILITY_API->CreateProgressBar();
	Configuration::GetInstance()->SetProgressBarProgress(qrand() % 101);*/
	MaterialConfig::GetInstance()->SetMaximumLimitForMaterialResult();

	//RESTAPI::SetProgressBarData(Configuration::GetInstance()->GetProgressBarProgress(), "Loading Material Search", true);
	GetMaterialConfigJSON();
	Logger::Info("MaterialConfig::InitializeMaterialData() End..");
}
/*
* Description - SetMaximumLimitForMaterialResult() method used to set the maximum limit for Material Results in search response.
* Parameter -
* Exception -
* Return -
*/
void MaterialConfig::SetMaximumLimitForMaterialResult()
{
	m_maxMaterialResultsLimit = Helper::GetJSONValue<string>(Configuration::GetInstance()->GetPLMPluginConfigJSON(), MAX_MATERIAL_SEARCH_LIMIT, true);
}

/*
* Description - GetMaximumLimitForMaterialResult() method used to get the maximum limit for Material Results in search response.
* Parameter -
* Exception -
* Return - string
*/
string MaterialConfig::GetMaximumLimitForMaterialResult()
{
	return m_maxMaterialResultsLimit;
}

/*
* Description - SetIsSaveAndCloseClicked() method used to set the status for save and close clicked
* Parameter - _isSaveAndCloseClicked
* Exception -
* Return - void
*/
void MaterialConfig::SetIsSaveAndCloseClicked(bool _isSaveAndCloseClicked)
{
	m_isSaveAndCloseClicked = _isSaveAndCloseClicked;
}

/*
* Description - GetIsSaveAndCloseClicked() method used to Get the status for save and close clicked.
* Parameter -
* Exception -
* Return - bool
*/
bool MaterialConfig::GetIsSaveAndCloseClicked()
{
	return m_isSaveAndCloseClicked;
}

/*
* Description - SetMaterialCategoryEnumId() method used to set the material category Id.
* Parameter - _materialCategoryEnumId
* Exception -
* Return - void
*/
 void MaterialConfig::SetMaterialCategoryEnumId(string _materialCategoryEnumId)
{
	m_materialCategoryEnumId = _materialCategoryEnumId;
}

/*
* Description - GetMaterialCategoryEnumId() method used to get the material category Id.
* Parameter -
* Exception -
* Return - string
*/
 string MaterialConfig::GetMaterialCategoryEnumId()
{
	return m_materialCategoryEnumId;
}

 /*
* Description - ResetMaterialConfig() is to set basic need jsons and some other attributs as default value.
* Parameter -
* Exception -
* Return -
*/
 void MaterialConfig::ResetMaterialConfig()
 {
	 Logger::Info("INFO::MaterialConfig: ResetMaterialConfig()-> Start");
	 m_materialConfigJson = nullptr;
	 m_materialFieldsJson = nullptr;
	 m_isModelExecuted = false;
	 m_sortedColumnNumber = 0;
	 SetIsSaveAndCloseClicked(false);
	 SetIsModelExecuted(m_isModelExecuted);
	 m_materialLoggedOut = true;
	 Logger::Info("INFO::MaterialConfig: ResetMaterialConfig()-> End");
 }