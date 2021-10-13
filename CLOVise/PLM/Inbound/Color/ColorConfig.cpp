/*
* Copyright 2020-2021 CLO-Vise. All rights reserved
*
* @file PLMColorData.cpp
*
* @brief Class implementation for cach Colors data in CLO from PLM.
* This class has all the variable and methods implementation which are used in storing PLM Color configuration related data to search in CLO
*
* @author GoVise
*
* @date  27-MAY-2020
*/
#include "ColorConfig.h"
#include "CLOAPIInterface.h"
#include "CLOVise/PLM/Helper/Util/RestAPIUtility.h"
#include "CLOVise/PLM/Helper/Util/FormatHelper.h"
#include "CLOVise/PLM/Helper/Util/Logger.h"
#include "CLOVise/PLM/Helper/Util/DataHelper.h"
#include "CLOVise/PLM/Helper/Util/Configuration.h"

using json = nlohmann::json;

ColorConfig* ColorConfig::_instance = NULL;
ColorConfig* ColorConfig::GetInstance()
{
	if (_instance == NULL)
	{
		_instance = new ColorConfig();
	}
	return _instance;
}

/*
* Description - SetColorConfigJSON() method used to get the configuration data from server/file.
* Parameter -  string.
* Exception - exception, Char *
* Return -
*/

//void ColorConfig::SetColorConfigJSON(string _module, string _rootType, bool _initDepedentJsons)
//{
//	Logger::Info("PLMColorData -> SetColorConfigJSON() start");
//   m_ColorConfigJson =	DataHelper::GetInstance()->SetConfigJSON( _module,  _rootType,  _initDepedentJsons, PLM_COLOR_FILE_NAME);
//   Logger::Info("PLMColorData -> SetColorConfigJSON() End");
//}

/*
* Description - SetColorConfigJSON() method used to get the configuration data from server/file.
* Parameter -  string.
* Exception - exception, Char *
* Return -
*/

void ColorConfig::SetColorConfigJSONFromFile(string _module, string _rootType, bool _initDepedentJsons)
{
	Logger::Info("PLMColorData -> SetColorConfigJSON() start");
   m_ColorConfigJson =	DataHelper::GetInstance()->SetConfigJSON( _module,  _rootType,  _initDepedentJsons, PLM_COLOR_FILE_NAME);
   Logger::Info("PLMColorData -> SetColorConfigJSON() End");
}

/*
* Description - GetColorConfigJSON() method used to get the cached configuration data.
* Parameter - 
* Exception - 
* Return - json.
*/
json ColorConfig::GetColorConfigJSON()
{
	Logger::Info("PLMColorData -> GetColorConfigJSON() start");

	if (m_colorFieldsJson == nullptr || m_colorFieldsJson.empty())
		SetColorConfigJSON();
	Logger::Info("PLMColorData -> GetColorConfigJSON() end");

	return m_colorFieldsJson;
}

/*
* Description - GetColorConfigJSONFromFile() method used to get the cached configuration data.
* Parameter - 
* Exception - 
* Return - json.
*/
json ColorConfig::GetColorConfigJSONFromFile()
{
	Logger::Info("PLMColorData -> GetColorConfigJSON() start");

	if (m_ColorConfigJson == nullptr || m_ColorConfigJson.empty())
		SetColorConfigJSONFromFile(COLOR_MODULE, COLOR_ROOT_TYPE, false);
	Logger::Info("PLMColorData -> GetColorConfigJSON() end");

	return m_ColorConfigJson;
}

/*
* Description - GetColorHierarchyJSON() method used to get cached color hierarchy json.
* Parameter -
* Exception -
* Return - json.
*/
json ColorConfig::GetColorHierarchyJSON()
{
	Logger::Info("PLMColorData -> GetColorHierarchyJSON() start");

	if (m_ColorHierarchyJson == nullptr || m_ColorHierarchyJson.empty())
		m_ColorHierarchyJson = DataHelper::GetInstance()->SetJson(HIERARCHY_JSON_KEY, m_ColorConfigJson);

	Logger::Info("PLMColorData -> GetColorHierarchyJSON() end");

	return m_ColorHierarchyJson;
}

/*
* Description - GetAttScopes() method used to get cached the attribute scopes data.
* Parameter - 
* Exception - 
* Return - QStringList.
*/
QStringList ColorConfig::GetAttScopes()
{
	Logger::Info("PLMColorData -> GetAttScopes() start");

	if (m_attScopes.empty())
		DataHelper::GetInstance()->SetAttScopes(m_ColorConfigJson, ATTSCOPE_JSON_KEY, m_attScopes);
	Logger::Info("PLMColorData -> GetAttScopes() end");

	return m_attScopes;
}

/*
* Description - GetColorFieldsJSON() method used to get cached the color fields data.
* Parameter - 
* Exception - 
* Return - json.
*/
json ColorConfig::GetColorFieldsJSON()
{
	Logger::Info("PLMColorData -> GetColorFieldsJSON() start");

	if (m_colorFieldsJson == nullptr || m_colorFieldsJson.empty())
		//m_ColorFieldsJson = DataHelper::GetInstance()->SetJson(FIELDLIST_JSON_KEY, m_ColorConfigJson);
		GetColorConfigJSON();
	Logger::Info("PLMColorData -> GetColorFieldsJSON() end");

	return m_colorFieldsJson;
}

/*
* Description - SetColorPresetJSON() method used to cache the preset data.
* Parameter -  string.
* Exception - exception, Char *
* Return -
*/
void ColorConfig::SetColorPresetJSON(string _presetJsonKey)
{
	Logger::Info("PLMColorData -> SetColorPresetJSON() start");

	try
	{
		string colorPresetStr = "";
		colorPresetStr = Helper::GetJSONValue<string>(GetColorConfigJSON(), _presetJsonKey, false);
		m_ColorPresetJson = json::parse(colorPresetStr);
	}
	catch (exception e)
	{
		Logger::Error("PLMColorData -> SetColorPresetJSON Exception :: " + string(e.what()));
		throw e;
	}
	catch (const char* msg)
	{
		Logger::Error("PLMColorData -> SetColorPresetJSON Exception :: " + string(msg));
		throw msg;
	}
	Logger::Info("PLMColorData -> SetColorPresetJSON() end");

}

/*
* Description - SetColorFilterJSON() method used to cache the filter data from server/file.
* Parameter -  string, string, string.
* Exception - exception, Char *
* Return -
*/

void ColorConfig::SetColorFilterJSON(string _module, string _rootType, string _activity)
{
	m_ColorFilterJson = DataHelper::GetInstance()->SetFilterJSON(_module, _rootType, _activity, PLM_COLOR_FILTER_FILE);
}

/*
* Description - GetColorFilterJSON() method used to get cached the filter data.
* Parameter - 
* Exception - 
* Return - json.
*/
json ColorConfig::GetColorFilterJSON()
{
	Logger::Info("PLMColorData -> GetColorFilterJSON() start");

	if (m_ColorFilterJson == nullptr || m_ColorFilterJson.empty())
		SetColorFilterJSON(COLOR_MODULE, COLOR_ROOT_TYPE, COLOR_SEARCH_ACTIVITY);
	Logger::Info("PLMColorData -> GetColorFilterJSON() end");

	return m_ColorFilterJson;
}

/*
* Description - SetColorViewJSON() method used to cache the views data from server/file.
* Parameter -  string, string, string.
* Exception - exception, Char *
* Return -
*/
void ColorConfig::SetColorViewJSON(string _module, string _rootType, string _activity)
{
	m_ColorViewJson = DataHelper::GetInstance()->SetViewJSON(_module, _rootType, _activity, PLM_COLOR_VIEW_FILE);
}

/*
* Description - GetColorViewJSON() method used to get cached the view data.
* Parameter - 
* Exception - 
* Return - json.
*/
json ColorConfig::GetColorViewJSON()
{
	Logger::Info("PLMColorData -> GetColorViewJSON() start");

	if (m_ColorViewJson == nullptr || m_ColorViewJson.empty())
		SetColorViewJSON(COLOR_MODULE, COLOR_ROOT_TYPE, COLOR_SEARCH_ACTIVITY);
	Logger::Info("PLMColorData -> GetColorViewJSON() end");

	return m_ColorViewJson;
}

/*
* Description - GetSeasonPaletteJSON() method used to get cached the seasonpalette data.
* Parameter -  beelean.
* Exception - 
* Return -
*/
json ColorConfig::GetSeasonPaletteJSON(bool _refresh)
{
	Logger::Info("PLMColorData -> GetSeasonPaletteJSON() start");

	if (_refresh)
	{
		m_paletteJson = DataHelper::GetInstance()->SetSeasonPaletteJSON(PLM_COLOR_PALETTE_FILE);
	}
	Logger::Info("PLMColorData -> GetSeasonPaletteJSON() end");

	return m_paletteJson;
}

/*
* Description - SetSelectedViewIdx() method used to cache the selected index.
* Parameter -  int.
* Exception - 
* Return -
*/
void ColorConfig::SetSelectedViewIdx(int _selectedViewIdx)
{
	Logger::Info("PLMColorData -> SetSelectedViewIdx() Start");
	m_selectedViewIdx = _selectedViewIdx;
	Logger::Info("PLMColorData -> SetSelectedViewIdx() End");
}

/*
* Description - GetSelectedViewIdx() method used to get cached selected index.
* Parameter - 
* Exception - 
* Return - int
*/
int ColorConfig::GetSelectedViewIdx()
{
	return m_selectedViewIdx;
}

/*
* Description - SetSearchCriteriaJSON() method used to cache the data.
* Parameter -  json.
* Exception - 
* Return -
*/
void ColorConfig::SetSearchCriteriaJSON(json _criteria)
{
	Logger::Info("PLMColorData -> SetSelectedViewIdx() Start");
	m_searchCriteriaJson = _criteria;
	Logger::Info("PLMColorData -> SetSelectedViewIdx() End");
}

/*
* Description - GetSearchCriteriaJSON() method used to get cached data.
* Parameter - 
* Exception - 
* Return - json.
*/
json ColorConfig::GetSearchCriteriaJSON()
{
	return m_searchCriteriaJson;
}

/*
* Description - SetDateFlag() method used to set date exist flag.
* Parameter -  bool.
* Exception -
* Return -
*/
void ColorConfig::SetDateFlag(bool _isDateEditPresent)
{
	m_isDateEditPresent = _isDateEditPresent;
}

/*
* Description - GetDateFlag() method used to get date exist flag.
* Parameter -
* Exception -
* Return - bool.
*/
bool ColorConfig::GetDateFlag()
{
	return m_isDateEditPresent;
}

void  ColorConfig::UpdateResultJson()
{
	//UTILITY_API->DisplayMessageBox("m_materialResults.size()" + to_string(m_materialResults.size()));
	try
	{
		for (int colorResultsjsonCount = 0; colorResultsjsonCount < m_colorResults.size(); colorResultsjsonCount++)
		{
			json fieldsJson = Helper::GetJSONParsedValue<int>(m_colorResults, colorResultsjsonCount, false);
			string objectName = Helper::GetJSONValue<string>(fieldsJson, NODE_NAME_KEY, true);
			string objectId = Helper::GetJSONValue<string>(fieldsJson, "id", true);
			/*string colorType = Helper::GetJSONValue<string>(fieldsJson, "product_type", true);
			string colorTypeResponse = RESTAPI::CentricRestCallGet(Configuration::GetInstance()->GetPLMServerURL() + RESTAPI::MATERIAL_TYPE_SEARCH_API + "/", APPLICATION_JSON_TYPE, colorType);
			if (FormatHelper::HasError(colorTypeResponse))
			{
				throw runtime_error(colorTypeResponse);
			}
			json colorTypeResponseJson = json::parse(colorTypeResponse);
			colorType = Helper::GetJSONValue<string>(colorTypeResponseJson, NODE_NAME_KEY, true);*/
			//UTILITY_API->DisplayMessageBox("materialType::" + materialType);
			fieldsJson[OBJECT_NAME_KEY] = objectName;
			fieldsJson[OBJECT_ID_KEY] = objectId;
			//fieldsJson["material_type"] = colorType;
			//fieldsJson["thumbnail"] = GetThumbnailUrl(objectId);
			m_colorResults[colorResultsjsonCount] = fieldsJson;
		}
	}
	catch (string msg)
	{
		Logger::Error("ColorConfig->UpdateResultJson() Exception - " + msg);
	}
	catch (exception & e)
	{
		Logger::Error("ColorConfig -> UpdateResultJson() Exception - " + string(e.what()));
	}
	catch (const char* msg)
	{
		wstring wstr(msg, msg + strlen(msg));
		Logger::Error("ColorConfig -> UpdateResultJson() Exception - " + string(msg));
	}
	//CreateViewJson();
}

void  ColorConfig::UpdateResultJson(json& _listjson)
{
		string objectName = Helper::GetJSONValue<string>(_listjson, NODE_NAME_KEY, true);
		string objectId = Helper::GetJSONValue<string>(_listjson, "id", true);
		_listjson[OBJECT_NAME_KEY] = objectName;
		_listjson[OBJECT_ID_KEY] = objectId;
		createdColorId << QString::fromStdString(objectId);
}

string ColorConfig::GetThumbnailUrl(string _objectId)
{
	string thumbnailUrl;
	try 
	{
		json imageResultJson = json::object();
		string resultResponse = RESTAPI::CentricRestCallGet(Configuration::GetInstance()->GetPLMServerURL() + RESTAPI::SEARCH_COLOR_API + "/" + _objectId + "/images", APPLICATION_JSON_TYPE, "");
		Logger::RestAPIDebug("resultResponse::" + resultResponse);
		if (FormatHelper::HasError(resultResponse))
		{
			Helper::GetCentricErrorMessage(resultResponse);
			throw runtime_error(resultResponse);
		}
		if (FormatHelper::HasContent(resultResponse))
		{
			imageResultJson = json::parse(resultResponse);
			//imageResultJson = json::parse(resultResponse);
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
		Logger::Error("ColorConfig->GetThumbnailUrl()->Exception - " + msg);
	}
	catch (exception & e)
	{	
		Logger::Error("ColorConfig->GetThumbnailUrl() Exception - " + string(e.what()));
	}
	catch (const char* msg)
	{		
		wstring wstr(msg, msg + strlen(msg));
		Logger::Error("ColorConfig->GetThumbnailUrl() Exception - " + string(msg));
	}
	return thumbnailUrl;
}

void ColorConfig::GetEnumlistJson(string _restApi, json& _attributeJson)
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
		Logger::Error(" ColorConfig->GetEnumlistJson()->Exception - " + msg);
	}
	catch (exception & e)
	{
		Logger::Error(" ColorConfig->GetEnumlistJson() Exception - " + string(e.what()));
	}
	catch (const char* msg)
	{
		wstring wstr(msg, msg + strlen(msg));
		Logger::Error(" ColorConfig->GetEnumlistJson() Exception - " + string(msg));
	}
}

void ColorConfig::ConvertToo(string& _convertValue, bool _isLower)
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
		Logger::Error("ColorConfig->ConvertToo()->Exception - " + msg);
	}
	catch (exception & e)
	{
		Logger::Error("ColorConfig->ConvertToo() Exception - " + string(e.what()));
	}
	catch (const char* msg)
	{
		wstring wstr(msg, msg + strlen(msg));
		Logger::Error("ColorConfig->ConvertToo() Exception - " + string(msg));
	}
}

void ColorConfig::SetColorConfigJSON()
{
	try
	{
		auto startTime = std::chrono::high_resolution_clock::now();
		string initialConfigJsonString = RESTAPI::CentricRestCallGet(Configuration::GetInstance()->GetPLMServerURL() + RESTAPI::SEARCH_ATT_API, APPLICATION_JSON_TYPE, "skip=0&limit=" + Configuration::GetInstance()->GetMaximumLimitForRefAttValue() + "&node_name=Library Item, Color Specification");
		auto finishTime = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double> totalDuration = finishTime - startTime;
		Logger::perfomance(PERFOMANCE_KEY + "Library Item, Color Specification API :: " + to_string(totalDuration.count()));
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
		//json defaultFieldsJson = ColorConfig::GetInstance()->GetColorConfigJSON();
		json defaultFieldsJson = ColorConfig::GetInstance()->GetColorConfigJSONFromFile();
		createFieldsJson(fieldsconfigJsonString, defaultFieldsJson);
	}

	catch (string msg)
	{
		Logger::Error("ColorConfig::SetMaterialConfigJSON() Exception - " + msg);
		throw msg;
		//UTILITY_API->DisplayMessageBox(msg);
	}
	catch (exception& e)
	{

		Logger::Error("ColorConfig::SetMaterialConfigJSON() Exception - " + string(e.what()));
		throw e.what();
		//UTILITY_API->DisplayMessageBox(e.what());
	}
	catch (const char* msg)
	{

		Logger::Error("ColorConfig::SetMaterialConfigJSON() Exception - " + string(msg));
		throw msg;
		//UTILITY_API->DisplayMessageBox(msg);
	}
	
}

void ColorConfig::createFieldsJson(string& _fieldsJsonStringResponse, json& _defaultFieldsJson)
{
	json fieldsJsonResponse = json::parse(_fieldsJsonStringResponse);
	string attValue;
	string attType;
	string attKey;
	string attApiExposed;
	string isAttEnable;
	json fieldsListjson = json::array();

	json fieldsJson;
	fieldsJson["typeName"] = "Color";
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
			attApiExposed = Helper::GetJSONValue<string>(feildsvaluesJson, ATTRIBUTE_REST_API_EXPOSED, true);
			//UTILITY_API->DisplayMessageBox("before convert::" + attKey + "::attValue::" + attValue);
			/*int last = attKey.find_last_of('/');
			attKey = attKey.substr(last + 1);
			ColorConfig::GetInstance()->ConvertToo(attKey, true);*/
			if (attType != REF_ATT_TYPE_KEY && attType != REFLIST_ATT_TYPE_KEY)
			{
				attKeyList << QString::fromStdString(attKey);
				if (attType == "enum")
				{
					DataHelper::GetInstance()->SetEnumJson(fieldsAttJson, feildsvaluesJson);

				}

				fieldsAttJson["attKey"] = attKey;
				fieldsAttJson["attType"] = attType;
				fieldsAttJson["attScope"] = "COLOR";
				fieldsAttJson["attName"] = attValue;
				fieldsAttJson["isAttSearchable"] = isAttEnable;
				fieldsAttJson["isMultiselectable"] = false;
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
		//UTILITY_API->DisplayMessageBox("after default loop.." + to_string(fieldsJsonCount));
		if (m_isSearchColor)
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
				if (!attKeyList.contains(QString::fromStdString(defaultAttributeKey)))
				{
					fieldsJsonCount = fieldsJsonCount + 1;
					if (attType == REF_ATT_TYPE_KEY)
					{
						DataHelper::GetInstance()->SetRefJson(attributesFieldsListJson, defaultAttributeKey);
					}
					if (FormatHelper::HasContent(to_string(attributesFieldsListJson)))
					{
						fieldsJson["attributes"].push_back(attributesFieldsListJson);
					}
				}
			}
		}
		fieldsListjson[0] = fieldsJson;
		m_colorFieldsJson = fieldsListjson;
		}
		else
		{
			fieldsListjson[0] = fieldsJson;
			m_PLMConfigJson = fieldsListjson;
		}
		
	}
	catch (string msg)
	{
		Logger::Error("ColorConfig->createFieldsJson() Exception :: " + msg);
		throw msg;
	}
	catch (exception& e)
	{
		Logger::Error("ColorConfig->createFieldsJson() Exception :: " + string(e.what()));
		//UTILITY_API->DisplayMessageBox(e.what());
		throw e;
	}
	catch (const char* msg)
	{
		Logger::Error("ColorConfig->createFieldsJson() Exception :: " + string(msg));
		throw msg;
	}


	//UTILITY_API->DisplayMessageBox("m_colorFieldsJson::" + to_string(m_colorFieldsJson));
}

void ColorConfig::SetDataFromResponse(json _param)
{
	//for now...
	Logger::Info("ColorConfig -> setDataFromResponse() -> Start");

	try
	{
		//string resultJsonString = RESTAPI::RESTMethodSubmit(RESTAPI::SEARCH_RESULTS_API, _param);
		string parameter = "";
		string matLibValue = "";
		bool isLibrarySearch = false;
		string resultResponse = "";
		json attributesJson = Helper::GetJSONParsedValue<string>(_param, ATTRIBUTES_KEY, false);
		if (!m_isSearchColor)
		{
			json createParam;
			for (int attributesJsonCount = 0; attributesJsonCount < attributesJson.size(); attributesJsonCount++)
			{
				string attKey = Helper::GetJSONParsedValue<string>(attributesJson[attributesJsonCount], ATTRIBUTE_KEY, false);
				string attValue = Helper::GetJSONParsedValue<string>(attributesJson[attributesJsonCount], ATTRIBUTE_VALUE_KEY, false);
				createParam[attKey] = attValue;
			}
			resultResponse = RESTAPI::PostRestCall(to_string(createParam), Configuration::GetInstance()->GetPLMServerURL() + RESTAPI::CREATE_COLOR_API, "content-type: application/json;");
		}
		else
		{
			for (int attributesJsonCount = 0; attributesJsonCount < attributesJson.size(); attributesJsonCount++)
			{
				string attKey = Helper::GetJSONParsedValue<string>(attributesJson[attributesJsonCount], ATTRIBUTE_KEY, false);
				string attType = Helper::GetJSONParsedValue<string>(attributesJson[attributesJsonCount], ATTRIBUTE_TYPE_KEY, false);
				string attValue = Helper::GetJSONParsedValue<string>(attributesJson[attributesJsonCount], ATTRIBUTE_VALUE_KEY, false);
				if (attKey == "lib_color_specifications") {
					matLibValue = attValue;
					isLibrarySearch = true;
					continue;
				}
				if (FormatHelper::HasContent(attValue))
				{
					if (FormatHelper::HasContent(parameter))
						parameter = parameter + "&";

					if (attType == TEXT_ATT_TYPE_KEY || attType == STRING_ATT_TYPE_KEY)
						parameter = parameter + attKey + ":wcm=" + attValue;
					else
						parameter = parameter + attKey + "=" + attValue;

				}
			}
			if (isLibrarySearch) {
				resultResponse = RESTAPI::CentricRestCallGet(Configuration::GetInstance()->GetPLMServerURL() + RESTAPI::COLOR_SEARCH_API_LIB + "/" + matLibValue + "/elements?", APPLICATION_JSON_TYPE, parameter + "&limit=" + ColorConfig::GetInstance()->GetMaximumLimitForColorResult());
			}
			else {
				auto startTime = std::chrono::high_resolution_clock::now();
				resultResponse = RESTAPI::CentricRestCallGet(Configuration::GetInstance()->GetPLMServerURL() + RESTAPI::SEARCH_COLOR_API + "?", APPLICATION_JSON_TYPE, parameter + "&limit=" + ColorConfig::GetInstance()->GetMaximumLimitForColorResult());
				auto finishTime = std::chrono::high_resolution_clock::now();
				std::chrono::duration<double> totalDuration = finishTime - startTime;
				Logger::perfomance(PERFOMANCE_KEY + "Search Results API :: " + to_string(totalDuration.count()));
			}
			
		}
		Logger::RestAPIDebug("resultResponse main::" + resultResponse);
		if (!FormatHelper::HasContent(resultResponse))
		{
			throw "Unable to fetch results. Please try again or Contact your System Administrator.";
		}

		if (FormatHelper::HasError(resultResponse))
		{
			Helper::GetCentricErrorMessage(resultResponse);
			throw runtime_error(resultResponse);
		}
		json colorResults = json::array();
		if (FormatHelper::HasContent(resultResponse))
		{
			if (!m_isSearchColor)
				colorResults[0] = json::parse(resultResponse);
			else
				colorResults = json::parse(resultResponse);
		}		
		m_colorResults.clear();
		createdColorId.clear();
		Logger::Logger("ColorResults.size()::" + to_string(colorResults.size()));
		for (int i = 0; i < colorResults.size(); i++)
		{
			string resultListStr = Helper::GetJSONValue<int>(colorResults, i, false);
			json resultListJson = json::parse(resultListStr);
			string rgbValue = Helper::GetJSONValue<string>(resultListJson, RGB_VALUE_KEY, true);
			if (rgbValue == "centric%3A" || !FormatHelper::HasContent(rgbValue))
				continue; 
			else
				ColorConfig::GetInstance()->UpdateResultJson(resultListJson);
			m_colorResults.push_back(resultListJson);
		}
		string resultsCount = to_string(m_colorResults.size());
		Logger::Debug("updatedJson::" + to_string(m_colorResults));
		/*if (stoi(resultsCount) <= 0)
			throw "No valid results found.";*/
		if (stoi(resultsCount) > 0)
		{
			m_resultsCount = stoi(resultsCount);
			// Helper::GetJSONValue<string>(m_materialResults[0], TYPENAME_KEY, true);
			//UTILITY_API->DisplayMessageBox("m_typename::" + m_typename);
		}
		else
		{
			m_resultsCount = 0;
		}
		m_typename = "Color";
		string maxResultsLimit;//= Helper::GetJSONValue<string>(m_materialResults, "maxResultsLimit", true);
		if (FormatHelper::HasContent(maxResultsLimit))
		{
			m_maxResultsCount = stoi(maxResultsLimit);
		}
		else
		{
			m_maxResultsCount = 1000;
		}		
		//m_resultsCount = ColorConfig::GetInstance()->GetResultsCount();
		if (m_resultsCount > m_maxResultsCount)
			throw "Maximum results limit exceeded. Please refine your search.";
	}
	catch (string msg)
	{
		Logger::Error("ColorConfig -> SetDataFromResponse Exception :: " + msg);
		throw msg;
	}
	catch (exception& e)
	{
		Logger::Error("ColorConfig -> SetDataFromResponse Exception :: " + string(e.what()));
		//UTILITY_API->DisplayMessageBox(e.what());
		throw e;
	}
	catch (const char* msg)
	{
		Logger::Error("ColorConfig -> SetDataFromResponse Exception :: " + string(msg));
		throw msg;
	}
}

json ColorConfig::GetColorResultsSON()
{
	Logger::Info("ColorConfig -> GetColorResultsSON() Start");
	return m_colorResults;
	Logger::Info("ColorConfig -> GetColorResultsSON() End");
}

string ColorConfig::GetTypename()
{
	Logger::Info("ColorConfig -> GetTypename() Start");
	return m_typename;
	Logger::Info("ColorConfig -> GetTypename() End");
}

int ColorConfig::GetMaxResultCount()
{
	Logger::Info("ColorConfig -> GetMaxResultCount() Start");
	return m_maxResultsCount;
	Logger::Info("ColorConfig -> GetMaxResultCount() End");
}

int ColorConfig::GetResultsCount()
{
	Logger::Info("ColorConfig -> GetResultsCount() Start");
	return m_resultsCount;
	Logger::Info("ColorConfig -> GetResultsCount() End");
}

/*
* Description - GetIsModelExecuted() return a flag wether Rest call is being made and module is already executed, used for progress bar to work correctly
* Parameter -
* Exception -
* Return - bool
*/
bool ColorConfig::GetIsModelExecuted()
{
	return m_isModelExecuted;
}

/*
* Description - SetIsModelExecuted() sets a flag wether Rest call is being made and module is already executed, used for progress bar to work correctly
* Parameter -
* Exception -
* Return -
*/
void ColorConfig::SetIsModelExecuted(bool _isModelExecuted)
{
	m_isModelExecuted = _isModelExecuted;
}

void ColorConfig::InitializeColorData()
{
	Logger::Info("ColorConfig::InitializeColorData() Started..");
	Configuration::GetInstance()->SetProgressBarProgress(0);
	Configuration::GetInstance()->SetProgressBarProgress(qrand() % 101);
	SetMaximumLimitForColorResult();
	RESTAPI::SetProgressBarData(Configuration::GetInstance()->GetProgressBarProgress(), "Loading Color Search", true);
	GetColorConfigJSON();
	Logger::Info("ColorConfig::InitializeColorData() End..");
}

/*
* Description - SetMaximumLimitForColorResult() method used to set the maximum limit for Color Results in search response.
* Parameter -
* Exception -
* Return -
*/
void ColorConfig::SetMaximumLimitForColorResult()
{
	m_maxColorResultsLimit = Helper::GetJSONValue<string>(Configuration::GetInstance()->GetPLMPluginConfigJSON(), MAX_COLOR_SEARCH_LIMIT, true);
}

/*
* Description - GetMaximumLimitForColorResult() method used to get the maximum limit for Color Results in search response.
* Parameter -
* Exception -
* Return - string
*/
string ColorConfig::GetMaximumLimitForColorResult()
{
	return m_maxColorResultsLimit;
}

json ColorConfig::GetPLMConfigJson()
{
	if (m_PLMConfigJson == nullptr || m_PLMConfigJson.empty())
		SetColorConfigJSON();
	return m_PLMConfigJson;
}
/*
* Description - ResetColorConfig() is to set basic need jsons and some other attributs as default value.
* Parameter -
* Exception -
* Return -
*/
void ColorConfig::ResetColorConfig()
{
	Logger::Info("INFO::ColorConfig: ResetColorConfig()-> Start");
	m_ColorConfigJson = nullptr;
	m_colorFieldsJson = nullptr;
	m_isModelExecuted = false; 
	m_sortedColumnNumber = 0;
	m_attScopes.clear();
	SetIsModelExecuted(m_isModelExecuted);
	m_colorLoggedOut = true;
	Logger::Info("INFO::ColorConfig: ResetColorConfig()-> End");
}