/*
* Copyright 2020-2021 CLO-Vise. All rights reserved
*
* @file PLMPrintData.cpp
*
* @brief Class implementation for cach Prints data in CLO from PLM.
* This class has all the variable and methods implementation which are used in storing PLM Print configuration related data to search in CLO
*
* @author GoVise
*
* @date  27-MAY-2020
*/
#include "PrintConfig.h"
#include "CLOAPIInterface.h"
#include "CLOVise/PLM/Helper/Util/RestAPIUtility.h"
#include "CLOVise/PLM/Helper/Util/FormatHelper.h"
#include "CLOVise/PLM/Helper/Util/Logger.h"
#include "CLOVise/PLM/Helper/Util/DataHelper.h"
#include "CLOVise/PLM/Helper/Util/Configuration.h"

using json = nlohmann::json;

PrintConfig* PrintConfig::_instance = NULL;
PrintConfig* PrintConfig::GetInstance()
{
	if (_instance == NULL)
	{
		_instance = new PrintConfig();
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

void PrintConfig::SetPrintConfigJSONFromFile(string _module, string _rootType, bool _initDepedentJsons)
{
	Logger::Info("PrintConfig -> SetPrintConfigJSON() Start");
   m_PrintConfigJson =	DataHelper::GetInstance()->SetConfigJSON( _module,  _rootType,  _initDepedentJsons, PLM_PRINT_FILE_NAME);
   Logger::Info("PrintConfig -> SetPrintConfigJSON() End");
}

/*
* Description - GetColorConfigJSON() method used to get the cached configuration data.
* Parameter - 
* Exception - 
* Return - json.
*/
json PrintConfig::GetPrintConfigJSON()
{
	Logger::Info("PrintConfig -> GetPrintConfigJSON() Start");

	if (m_printFieldsJson == nullptr || m_printFieldsJson.empty())
		SetPrintConfigJSON();
	Logger::Info("PrintConfig -> GetPrintConfigJSON() end");

	return m_printFieldsJson;
}

/*
* Description - GetPrintConfigJSONFromFile() method used to get the cached configuration data.
* Parameter - 
* Exception - 
* Return - json.
*/
json PrintConfig::GetPrintConfigJSONFromFile()
{
	Logger::Info("PrintConfig -> GetPrintConfigJSON() Start");

	if (m_PrintConfigJson == nullptr || m_PrintConfigJson.empty())
		SetPrintConfigJSONFromFile(PRINT_MODULE, PRINT_ROOT_TYPE, false);
	Logger::Info("PrintConfig -> GetPrintConfigJSON() end");

	return m_PrintConfigJson;
}

/*
* Description - GetPrintHierarchyJSON() method used to get cached Print hierarchy json.
* Parameter -
* Exception -
* Return - json.
*/
json PrintConfig::GetPrintHierarchyJSON()
{
	Logger::Info("PrintConfig -> GetPrintHierarchyJSON() Start");

	if (m_PrintHierarchyJson == nullptr || m_PrintHierarchyJson.empty())
		m_PrintHierarchyJson = DataHelper::GetInstance()->SetJson(HIERARCHY_JSON_KEY, m_PrintConfigJson);

	Logger::Info("PrintConfig -> GetPrintHierarchyJSON() End");

	return m_PrintHierarchyJson;
}

/*
* Description - GetAttScopes() method used to get cached the attribute scopes data.
* Parameter - 
* Exception - 
* Return - QStringList.
*/
QStringList PrintConfig::GetAttScopes()
{
	Logger::Info("PrintConfig -> GetAttScopes() Start");

	if (m_attScopes.empty())
		DataHelper::GetInstance()->SetAttScopes(m_PrintConfigJson, ATTSCOPE_JSON_KEY, m_attScopes);
	Logger::Info("PrintConfig -> GetAttScopes() End");

	return m_attScopes;
}

/*
* Description - GetPrintFieldsJSON() method used to get cached the Print fields data.
* Parameter - 
* Exception - 
* Return - json.
*/
json PrintConfig::GetPrintFieldsJSON()
{
	Logger::Info("PrintConfig -> GetPrintFieldsJSON() start");

	if (m_printFieldsJson == nullptr || m_printFieldsJson.empty())
		GetPrintConfigJSON();
	Logger::Info("PrintConfig -> GetPrintFieldsJSON() end");

	return m_printFieldsJson;
}

/*
* Description - SetPrintPresetJSON() method used to cache the preset data.
* Parameter -  string.
* Exception - exception, Char *
* Return -
*/
void PrintConfig::SetPrintPresetJSON(string _presetJsonKey)
{
	Logger::Info("PrintConfig -> SetPrintPresetJSON() Start");

	try
	{
		string printPresetStr = "";
		printPresetStr = Helper::GetJSONValue<string>(GetPrintConfigJSON(), _presetJsonKey, false);
		m_PrintPresetJson = json::parse(printPresetStr);
	}
	catch (exception e)
	{
		Logger::Error("PrintConfig -> SetPrintPresetJSON Exception :: " + string(e.what()));
		throw e;
	}
	catch (const char* msg)
	{
		Logger::Error("PrintConfig -> SetPrintPresetJSON Exception :: " + string(msg));
		throw msg;
	}
	Logger::Info("PrintConfig -> SetPrintPresetJSON() End");

}

/*
* Description - SetPrintFilterJSON() method used to cache the filter data from server/file.
* Parameter -  string, string, string.
* Exception - exception, Char *
* Return -
*/

void PrintConfig::SetPrintFilterJSON(string _module, string _rootType, string _activity)
{
	Logger::Info("PrintConfig -> SetPrintFilterJSON() Start");
	m_PrintFilterJson = DataHelper::GetInstance()->SetFilterJSON(_module, _rootType, _activity, PLM_COLOR_FILTER_FILE);
	Logger::Info("PrintConfig -> SetPrintFilterJSON() End");
}

/*
* Description - GetPrintFilterJSON() method used to get cached the filter data.
* Parameter - 
* Exception - 
* Return - json.
*/
json PrintConfig::GetPrintFilterJSON()
{
	Logger::Info("PrintConfig -> GetPrintFilterJSON() Start");

	if (m_PrintFilterJson == nullptr || m_PrintFilterJson.empty())
		SetPrintFilterJSON(PRINT_MODULE, PRINT_ROOT_TYPE, PRINT_SEARCH_ACTIVITY);
	Logger::Info("PrintConfig -> GetPrintFilterJSON() End");

	return m_PrintFilterJson;
}

/*
* Description - SetPrintViewJSON() method used to cache the views data from server/file.
* Parameter -  string, string, string.
* Exception - exception, Char *
* Return -
*/
void PrintConfig::SetPrintViewJSON(string _module, string _rootType, string _activity)
{
	Logger::Info("PrintConfig -> SetPrintViewJSON() Start");

	m_PrintViewJson = DataHelper::GetInstance()->SetViewJSON(_module, _rootType, _activity, PLM_PRINT_VIEW_FILE);

	Logger::Info("PrintConfig -> SetPrintViewJSON() End");
}

/*
* Description - GetPrintViewJSON() method used to get cached the view data.
* Parameter - 
* Exception - 
* Return - json.
*/
json PrintConfig::GetPrintViewJSON()
{
	Logger::Info("PrintConfig -> GetPrintViewJSON() Start");

	if (m_PrintViewJson == nullptr || m_PrintViewJson.empty())
		SetPrintViewJSON(PRINT_MODULE, PRINT_ROOT_TYPE, PRINT_SEARCH_ACTIVITY);

	Logger::Info("PrintConfig -> GetPrintViewJSON() End");

	return m_PrintViewJson;
}

/*
* Description - GetSeasonPaletteJSON() method used to get cached the seasonpalette data.
* Parameter -  beelean.
* Exception - 
* Return -
*/
json PrintConfig::GetSeasonPaletteJSON(bool _refresh)
{
	Logger::Info("PrintConfig -> GetSeasonPaletteJSON() Start");

	if (_refresh)
	{
		m_paletteJson = DataHelper::GetInstance()->SetSeasonPaletteJSON(PLM_COLOR_PALETTE_FILE);
	}
	Logger::Info("PrintConfig -> GetSeasonPaletteJSON() End");

	return m_paletteJson;
}

/*
* Description - SetSelectedViewIdx() method used to cache the selected index.
* Parameter -  int.
* Exception - 
* Return -
*/
void PrintConfig::SetSelectedViewIdx(int _selectedViewIdx)
{
	Logger::Info("PrintConfig -> SetSelectedViewIdx() Start");
	m_selectedViewIdx = _selectedViewIdx;
	Logger::Info("PrintConfig -> SetSelectedViewIdx() End");
}

/*
* Description - GetSelectedViewIdx() method used to get cached selected index.
* Parameter - 
* Exception - 
* Return - int
*/
int PrintConfig::GetSelectedViewIdx()
{
	Logger::Info("PrintConfig -> GetSelectedViewIdx() Start");
	return m_selectedViewIdx;
	Logger::Info("PrintConfig -> GetSelectedViewIdx() End");
}

/*
* Description - SetSearchCriteriaJSON() method used to cache the data.
* Parameter -  json.
* Exception - 
* Return -
*/
void PrintConfig::SetSearchCriteriaJSON(json _criteria)
{
	Logger::Info("PrintConfig -> SetSelectedViewIdx() Start");
	m_searchCriteriaJson = _criteria;
	Logger::Info("PrintConfig -> SetSelectedViewIdx() End");
}

/*
* Description - GetSearchCriteriaJSON() method used to get cached data.
* Parameter - 
* Exception - 
* Return - json.
*/
json PrintConfig::GetSearchCriteriaJSON()
{
	Logger::Info("PrintConfig -> GetSearchCriteriaJSON() Start");
	return m_searchCriteriaJson;
	Logger::Info("PrintConfig -> GetSearchCriteriaJSON() End");
}

/*
* Description - SetDateFlag() method used to set date exist flag.
* Parameter -  bool.
* Exception -
* Return -
*/
void PrintConfig::SetDateFlag(bool _isDateEditPresent)
{
	Logger::Info("PrintConfig -> SetDateFlag() Start");
	m_isDateEditPresent = _isDateEditPresent;
	Logger::Info("PrintConfig -> SetDateFlag() End");
}

/*
* Description - GetDateFlag() method used to get date exist flag.
* Parameter -
* Exception -
* Return - bool.
*/
bool PrintConfig::GetDateFlag()
{
	Logger::Info("PrintConfig -> GetDateFlag() start");
	return m_isDateEditPresent;
	Logger::Info("PrintConfig -> GetDateFlag() End");
}

void  PrintConfig::UpdateResultJson()
{
	Logger::Info("PrintConfig -> UpdateResultJson() Start");
	try
	{
		for (int printResultsjsonCount = 0; printResultsjsonCount < m_printResults.size(); printResultsjsonCount++)
		{
			json fieldsJson = Helper::GetJSONParsedValue<int>(m_printResults, printResultsjsonCount, false);
			string objectName = Helper::GetJSONValue<string>(fieldsJson, NODE_NAME_KEY, true);
			string objectId = Helper::GetJSONValue<string>(fieldsJson, "id", true);
			fieldsJson[OBJECT_NAME_KEY] = objectName;
			fieldsJson[OBJECT_ID_KEY] = objectId;
			m_printResults[printResultsjsonCount] = fieldsJson;
		}
		Logger::Info("PrintConfig -> UpdateResultJson() End");
	}
	catch (string msg)
	{
		Logger::Error("PrintConfig->UpdateResultJson() Exception - " + msg);
	}
	catch (exception & e)
	{
		Logger::Error("PrintConfig -> UpdateResultJson() Exception - " + string(e.what()));
	}
	catch (const char* msg)
	{
		wstring wstr(msg, msg + strlen(msg));
		Logger::Error("PrintConfig -> UpdateResultJson() Exception - " + string(msg));
	}
	//CreateViewJson();
}

void  PrintConfig::UpdateResultJson(json& _listjson)
{
	Logger::Info("PrintConfig -> UpdateResultJson() Start");
		string objectName = Helper::GetJSONValue<string>(_listjson, NODE_NAME_KEY, true);
		string objectId = Helper::GetJSONValue<string>(_listjson, "id", true);
		_listjson[OBJECT_NAME_KEY] = objectName;
		_listjson[OBJECT_ID_KEY] = objectId;
		Logger::Info("PrintConfig -> UpdateResultJson() End");

}

string PrintConfig::GetThumbnailUrl(string _objectId)
{
	Logger::Info("PrintConfig -> GetThumbnailUrl() Start");
	string thumbnailUrl;
	try 
	{
		json imageResultJson = json::object();
		string resultResponse = RESTAPI::CentricRestCallGet(Configuration::GetInstance()->GetPLMServerURL() + RESTAPI::SEARCH_COLOR_API + "/" + _objectId + "/images?decode=true", APPLICATION_JSON_TYPE, "");
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
		Logger::Error("PrintConfig->GetThumbnailUrl()->Exception - " + msg);
	}
	catch (exception & e)
	{	
		Logger::Error("PrintConfig->GetThumbnailUrl() Exception - " + string(e.what()));
	}
	catch (const char* msg)
	{		
		wstring wstr(msg, msg + strlen(msg));
		Logger::Error("PrintConfig->GetThumbnailUrl() Exception - " + string(msg));
	}
	return thumbnailUrl;

	Logger::Info("PrintConfig -> GetThumbnailUrl() End");
}

void PrintConfig::GetEnumlistJson(string _restApi, json& _attributeJson)
{
	Logger::Info("PrintConfig -> GetEnumlistJson() Start");
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
		Logger::Error(" PrintConfig->GetEnumlistJson()->Exception - " + msg);
	}
	catch (exception & e)
	{
		Logger::Error(" PrintConfig->GetEnumlistJson() Exception - " + string(e.what()));
	}
	catch (const char* msg)
	{
		wstring wstr(msg, msg + strlen(msg));
		Logger::Error(" PrintConfig->GetEnumlistJson() Exception - " + string(msg));
	}
	Logger::Info("PrintConfig -> GetEnumlistJson() End");
}

void PrintConfig::ConvertToo(string& _convertValue, bool _isLower)
{
	Logger::Info("PrintConfig -> ConvertToo() Start");
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
		Logger::Error("PrintConfig->ConvertToo()->Exception - " + msg);
	}
	catch (exception & e)
	{
		Logger::Error("PrintConfig->ConvertToo() Exception - " + string(e.what()));
	}
	catch (const char* msg)
	{
		wstring wstr(msg, msg + strlen(msg));
		Logger::Error("PrintConfig->ConvertToo() Exception - " + string(msg));
	}
	Logger::Info("PrintConfig -> ConvertToo() End");
}

void PrintConfig::SetPrintConfigJSON()
{
	Logger::Info("PrintConfig -> SetPrintConfigJSON() Start");
	try
	{
		auto startTime = std::chrono::high_resolution_clock::now();
		string initialConfigJsonString = RESTAPI::CentricRestCallGet(Configuration::GetInstance()->GetPLMServerURL() + RESTAPI::SEARCH_ATT_API, APPLICATION_JSON_TYPE, "skip=0&decode=true&limit=" + Configuration::GetInstance()->GetMaximumLimitForRefAttValue() + "&node_name=PrintDesignColor");
		auto finishTime = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double> totalDuration = finishTime - startTime;
		Logger::perfomance(PERFOMANCE_KEY + "Print Design colors API :: " + to_string(totalDuration.count()));
		Logger::RestAPIDebug("initialConfigJsonString::" + initialConfigJsonString);
		if (FormatHelper::HasError(initialConfigJsonString))
		{
			Helper::GetCentricErrorMessage(initialConfigJsonString);
			throw runtime_error(initialConfigJsonString);
		}
		json initialConfigJson = json::parse(initialConfigJsonString);
		string formDefinition = Helper::GetJSONValue<string>(initialConfigJson[0], CREATE_DEFINITION_KEY, true);
		Logger::Debug("formDefinition :: " + formDefinition);
		auto FormDefinitionsApiStartTime = std::chrono::high_resolution_clock::now();
		string fieldsconfigJsonString = RESTAPI::CentricRestCallGet(Configuration::GetInstance()->GetPLMServerURL() + RESTAPI::SEARCH_ATT_DEFINITION_API + formDefinition + RESTAPI::SEARCH_ATT_DEFINITION_ADDITIONAL_API, APPLICATION_JSON_TYPE, "");
		Logger::Debug("fieldsconfigJsonString :: " + fieldsconfigJsonString);
		auto FormDefinitionsApiFinishTime = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double> FormDefinitionsApiTotalDuration = FormDefinitionsApiFinishTime - FormDefinitionsApiStartTime;
		Logger::perfomance(PERFOMANCE_KEY + "Create Form Definitions API :: " + to_string(FormDefinitionsApiTotalDuration.count()));
		Logger::RestAPIDebug("fieldsconfigJsonString::" + fieldsconfigJsonString);
		if (FormatHelper::HasError(fieldsconfigJsonString))
		{
			Helper::GetCentricErrorMessage(fieldsconfigJsonString);
			throw runtime_error(fieldsconfigJsonString);
		}
		
		json defaultFieldsJson = PrintConfig::GetInstance()->GetPrintConfigJSONFromFile();
		createFieldsJson(fieldsconfigJsonString, defaultFieldsJson);
	}

	catch (string msg)
	{
		Logger::Error("PrintConfig::SetPrintConfigJSON() Exception - " + msg);
		throw msg;
		//UTILITY_API->DisplayMessageBox(msg);
	}
	catch (exception& e)
	{

		Logger::Error("PrintConfig::SetPrintConfigJSON() Exception - " + string(e.what()));
		throw e.what();
		//UTILITY_API->DisplayMessageBox(e.what());
	}
	catch (const char* msg)
	{

		Logger::Error("PrintConfig::SetPrintConfigJSON() Exception - " + string(msg));
		throw msg;
		//UTILITY_API->DisplayMessageBox(msg);
	}
	Logger::Info("PrintConfig -> SetPrintConfigJSON() End");
	
}

void PrintConfig::createFieldsJson(string& _fieldsJsonStringResponse, json& _defaultFieldsJson)
{
	Logger::Info("PrintConfig -> createFieldsJson() Start");

	json fieldsJsonResponse = json::parse(_fieldsJsonStringResponse);
	string attValue;
	string attType;
	string attKey;
	string attApiExposed;
	string isAttEnable;
	json fieldsListjson = json::array();

	json fieldsJson;
	fieldsJson["typeName"] = "Print";
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
			
			if (attType != REF_ATT_TYPE_KEY && attType != REFLIST_ATT_TYPE_KEY)
			{
				attKeyList << QString::fromStdString(attKey);
				if (attType == "enum")
				{
					DataHelper::GetInstance()->SetEnumJson(fieldsAttJson, feildsvaluesJson);

				}

				fieldsAttJson["attKey"] = attKey;
				fieldsAttJson["attType"] = attType;
				fieldsAttJson["attScope"] = "PRINT";
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
		m_printFieldsJson = fieldsListjson;
		Logger::Debug("m_printFieldsJson:" + to_string(m_printFieldsJson).size());
	}
	catch (string msg)
	{
		Logger::Error("PrintConfig->createFieldsJson() Exception :: " + msg);
		throw msg;
	}
	catch (exception& e)
	{
		Logger::Error("PrintConfig->createFieldsJson() Exception :: " + string(e.what()));
		//UTILITY_API->DisplayMessageBox(e.what());
		throw e;
	}
	catch (const char* msg)
	{
		Logger::Error("PrintConfig->createFieldsJson() Exception :: " + string(msg));
		throw msg;
	}

	Logger::Info("PrintConfig -> createFieldsJson() End");
}

void PrintConfig::SetDataFromResponse(json _param)
{
	//for now...
	Logger::Info("PrintConfig -> setDataFromResponse() -> Start");

	try
	{
		//string resultJsonString = RESTAPI::RESTMethodSubmit(RESTAPI::SEARCH_RESULTS_API, _param);
		string parameter = "";
		string matLibValue = "";
		bool isPrintDesignSearch = false;
		json attributesJson = Helper::GetJSONParsedValue<string>(_param, ATTRIBUTES_KEY, false);
		for (int attributesJsonCount = 0; attributesJsonCount < attributesJson.size(); attributesJsonCount++)
		{
			string attKey = Helper::GetJSONParsedValue<string>(attributesJson[attributesJsonCount], ATTRIBUTE_KEY, false);
			string attType = Helper::GetJSONParsedValue<string>(attributesJson[attributesJsonCount], ATTRIBUTE_TYPE_KEY, false);
			string attValue = Helper::GetJSONParsedValue<string>(attributesJson[attributesJsonCount], ATTRIBUTE_VALUE_KEY, false);
			/*if (attKey == "print_designs") {
				matLibValue = attValue;
				isPrintDesignSearch = true;
				continue;
			}*/
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
		//UTILITY_API->DisplayMessageBox(parameter);
		string resultResponse = "";
		/*if (isPrintDesignSearch) {
			resultResponse = RESTAPI::CentricRestCallGet(Configuration::GetInstance()->GetPLMServerURL() + RESTAPI::SEARCH_PRINT_DESIGN_API + "/" + matLibValue + "/elements?", APPLICATION_JSON_TYPE, parameter + "&limit="+ PrintConfig::GetInstance()->GetMaximumLimitForColorResult());
		}
		else {*/
		
			auto startTime = std::chrono::high_resolution_clock::now();
			resultResponse = RESTAPI::CentricRestCallGet(Configuration::GetInstance()->GetPLMServerURL() + RESTAPI::SEARCH_PRINT_DESIGN_COLOR_API + "?", APPLICATION_JSON_TYPE, parameter + "&decode=true&limit=" + PrintConfig::GetInstance()->GetMaximumLimitForPrintResult());
			Logger::Debug("Print response ::" + resultResponse);
			auto finishTime = std::chrono::high_resolution_clock::now();
			std::chrono::duration<double> totalDuration = finishTime - startTime;
			Logger::perfomance(PERFOMANCE_KEY + "Search Results API :: " + to_string(totalDuration.count()));
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
		json printResults = json::array();
		if (FormatHelper::HasContent(resultResponse))
			printResults = json::parse(resultResponse);
		m_printResults.clear();
		Logger::Logger("PrintResults.size()::" + to_string(printResults.size()));
		for (int i = 0; i < printResults.size(); i++)
		{
			string resultListStr = Helper::GetJSONValue<int>(printResults, i, false);
			json resultListJson = json::parse(resultListStr);
		
	        PrintConfig::GetInstance()->UpdateResultJson(resultListJson);
			m_printResults.push_back(resultListJson);
		}
		string resultsCount = to_string(m_printResults.size());
		
		if (stoi(resultsCount) > 0)
		{
			m_resultsCount = stoi(resultsCount);
		}
		else
		{
			m_resultsCount = 0;
		}
		m_typename = "Print";
		string maxResultsLimit;
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
		Logger::Error("PrintConfig -> SetDataFromResponse Exception :: " + msg);
		throw msg;
	}
	catch (exception& e)
	{
		Logger::Error("PrintConfig -> SetDataFromResponse Exception :: " + string(e.what()));
		//UTILITY_API->DisplayMessageBox(e.what());
		throw e;
	}
	catch (const char* msg)
	{
		Logger::Error("PrintConfig -> SetDataFromResponse Exception :: " + string(msg));
		throw msg;
	}
	Logger::Info("PrintConfig -> setDataFromResponse() -> End");
}

json PrintConfig::GetPrintResultsSON()
{
	Logger::Info("PrintConfig -> GetPrintResultsSON() Start");
	return m_printResults;
	Logger::Info("PrintConfig -> GetPrintResultsSON() End");
}

string PrintConfig::GetTypename()
{
	Logger::Info("PrintConfig -> GetTypename() Start");
	return m_typename;
	Logger::Info("PrintConfig -> GetTypename() End");
}

int PrintConfig::GetMaxResultCount()
{
	Logger::Info("PrintConfig -> GetMaxResultCount() Start");
	return m_maxResultsCount;
	Logger::Info("PrintConfig -> GetMaxResultCount() End");
}

int PrintConfig::GetResultsCount()
{
	Logger::Info("PrintConfig -> GetResultsCount() Start");
	return m_resultsCount;
	Logger::Info("PrintConfig -> GetResultsCount() End");
}

/*
* Description - GetIsModelExecuted() return a flag wether Rest call is being made and module is already executed, used for progress bar to work correctly
* Parameter -
* Exception -
* Return - bool
*/
bool PrintConfig::GetIsModelExecuted()
{
	Logger::Info("PrintConfig -> GetIsModelExecuted() -> Start");
	return m_isModelExecuted;
	Logger::Info("PrintConfig -> GetIsModelExecuted() -> End");
}

/*
* Description - SetIsModelExecuted() sets a flag wether Rest call is being made and module is already executed, used for progress bar to work correctly
* Parameter -
* Exception -
* Return -
*/
void PrintConfig::SetIsModelExecuted(bool _isModelExecuted)
{
	Logger::Info("PrintConfig -> SetIsModelExecuted() -> Start");
	m_isModelExecuted = _isModelExecuted;
	Logger::Info("PrintConfig -> SetIsModelExecuted() -> End");
}

void PrintConfig::InitializePrintData()
{
	Logger::Info("PrintConfig -> InitializePrintData() Started..");
	Configuration::GetInstance()->SetProgressBarProgress(0);
	Configuration::GetInstance()->SetProgressBarProgress(qrand() % 101);
	SetMaximumLimitForPrintResult();
	RESTAPI::SetProgressBarData(Configuration::GetInstance()->GetProgressBarProgress(), "Loading Print Search", true);
	GetPrintConfigJSON();
	Logger::Info("PrintConfig -> InitializePrinData() End..");
}

/*
* Description - SetMaximumLimitForPrintResult() method used to set the maximum limit for Print Results in search response.
* Parameter -
* Exception -
* Return -
*/
void PrintConfig::SetMaximumLimitForPrintResult()
{
	Logger::Info("PrintConfig -> SetMaximumLimitForPrintResult() Start");
	m_maxPrintResultsLimit = Helper::GetJSONValue<string>(Configuration::GetInstance()->GetPLMPluginConfigJSON(), MAX_PRINT_SEARCH_LIMIT, true);
	Logger::Info("PrintConfig -> SetMaximumLimitForPrintResult() End");
}

/*
* Description - GetMaximumLimitForPrintResult() method used to get the maximum limit for Print Results in search response.
* Parameter -
* Exception -
* Return - 
*/
string PrintConfig::GetMaximumLimitForPrintResult()
{
	Logger::Info("PrintConfig -> GetMaximumLimitForPrintResult() Start");
	return m_maxPrintResultsLimit;
	Logger::Info("PrintConfig -> GetMaximumLimitForPrintResult() End");
}


/*
* Description - ResetPrintConfig() is to set view json as empty.
* Parameter -
* Exception -
* Return -
*/
void PrintConfig::ResetPrintConfig()
{
	Logger::Info("INFO::PrintConfig: ResetPrintConfig()-> Start");
	m_PrintConfigJson = nullptr;
	m_printFieldsJson = nullptr;
	m_isModelExecuted = false;
	m_sortedColumnNumber = 0;
	m_attScopes.clear();
	SetIsModelExecuted(m_isModelExecuted);
	m_printLoggedOut = true;
	Logger::Info("INFO::PrintConfig: ResetPrintConfig()-> End");
}