#pragma once
/*
* Copyright 2020-2021 CLO-Vise. All rights reserved
*
* @file ProductConfig.h
*
* @brief Class declaration for cach Product data in CLO from PLM.
* This class has all the variable declarations and function declarations which are used in storing PLM Product configuration related data to search in CLO
*
* @author GoVise
*
* @date 28-JUL-2020
*/
#include <string>

#include <QString>
#include <QStringList>

#include "CLOVise/PLM/Libraries/json.h"

using json = nlohmann::json;
using namespace std;

class ProductConfig
{
public:
	bool m_isModelExecuted = false;
	int m_selectedViewIdx;
	bool m_isDateEditPresent = false;
	bool m_isShow3DAttWidget = false;

	json m_productFieldsJson = json::object();

	static ProductConfig* GetInstance();
	static void	Destroy();
	void InitializeProductData();
	
	//void SetProductConfigJSON(string _module, string _rootType, bool _initDepedentJsons);
	void SetProductConfigJSON();
	void SetProductConfigJSONFromFile(string _module, string _rootType, bool _initDepedentJsons);
	//void SetProductHierarchyJSON(string _hierarchyJsonKey);
	//void SetProductFieldsJSON(string _hierarchyJsonKey);
	void SetProductPresetJSON(string _hierarchyJsonKey);
	void SetProductFilterJSON(string _module, string _rootType, string _activity);
	void SetProductViewJSON(string _module, string _rootType, string _activity);
	//void SetSeasonPaletteJSON();
	void SetSelectedViewIdx(int _selectedViewIdx);
	void SetSearchCriteriaJSON(json _criteria);
	//void SetAttScopes(string _attScopesJsonKey);
	void SetDateFlag(bool _isDateEditPresent);
	void SetDownloadFileName(string _downloadFileName);
	void SetDataFromResponse(json _param);
	void SetCopyStyleResult(string _copyresponse);
	void SetMaximumLimitForStyleResult();
	void CachePLMAttributes();
	map<string, string> GetAttributeIdAndValueMap(string _restApi);
	void GetAttributeDisplayNames(json& _materialResults);
	string GetDisplayName(map<string, string> _map, string _Id);

	json m_seasonValuesJson;
	json GetProductConfigJSON();
	json GetProductHierarchyJSON();
	json GetProductFieldsJSON();
	json GetProductPresetJSON();
	json GetProductFilterJSON();
	json GetProductViewJSON();
	json GetSeasonPaletteJSON(bool _refresh);
	int GetSelectedViewIdx();
	json GetSearchCriteriaJSON();
	QStringList GetAttScopes();
	bool GetDateFlag();
	string GetDownloadFileName();
	json GetStyleResultsSON();
	string GetTypename();
	int GetMaxResultCount();
	int GetResultsCount();
	void UpdateResultJson(json& _materialResults, json& _materialTypeValues);
	//string GetThumbnailUrl(string _objectId);
	void GetEnumlistJson(string _restApi, json& _attributeJson);
	void SetIsModelExecuted(bool _isModelExecuted);
	bool GetIsModelExecuted();
	json GetProductConfigJSONFromFile();
	string GetMaximumLimitForStyleResult();
	map <string, string> m_seasonIdAndDisplayMap;
	map <string, string> m_styleTypeIdAndDisplayMap;
	map <string, string> m_shapeIdAndDisplayMap;
	map <string, string> m_themeIdAndDisplayMap;
	map <string, string> m_sleeveIdAndDisplayMap;
	map <string, string> m_lengthIdAndDisplayMap;
	map <string, string> m_silhouetteIdAndDisplayMap;
	map <string, string> m_fabricIdAndDisplayMap;
private:
	static ProductConfig* _instance; // zero initialized by default
	json m_productConfigJson = json::object();
	json m_productHierarchyJson = json::object();
	json m_productPresetJson = json::object();
	json m_productFilterJson = json::object();
	json m_productViewJson = json::object();
	json m_paletteJson = json::object();
	json m_searchCriteriaJson = json::object();
	QStringList m_attScopes;
	bool restData = true;
	string m_downloadFileName;
	string m_maxStyleResultsLimit;

	json m_styleResults = json::array();
	string m_typename = "";
	int m_maxResultsCount = 0;
	int m_resultsCount = 0;

	void createFieldsJson(string& _fieldsJsonStringResponse, json& _defaultFieldsJson);
};
