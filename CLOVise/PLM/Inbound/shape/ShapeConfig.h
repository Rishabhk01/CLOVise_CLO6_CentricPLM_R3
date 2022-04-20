#pragma once
/*
* Copyright 2020-2021 CLO-Vise. All rights reserved
*
* @file ShapeConfig.h
*
* @brief Class declaration for cach Shape data in CLO from PLM.
* This class has all the variable declarations and function declarations which are used in storing PLM Shape configuration related data to search in CLO
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

class ShapeConfig
{
public:
	bool m_isModelExecuted = false;
	int m_selectedViewIdx;
	bool m_isDateEditPresent = false;
	bool m_isShow3DAttWidget = false;
	int m_sortedColumnNumber;
	bool m_shapeLoggedOut = false;
	bool m_resultAfterLogout = false;
	json m_shapeFieldsJson = json::object();

	static ShapeConfig* GetInstance();
	void InitializeShapeData();

	//void SetShapeConfigJSON(string _module, string _rootType, bool _initDepedentJsons);
	void SetShapeConfigJSON();
	void SetShapeConfigJSONFromFile(string _module, string _rootType, bool _initDepedentJsons);
	//void SetShapeHierarchyJSON(string _hierarchyJsonKey);
	//void SetShapeFieldsJSON(string _hierarchyJsonKey);
	void SetShapePresetJSON(string _hierarchyJsonKey);
	void SetShapeFilterJSON(string _module, string _rootType, string _activity);
	void SetShapeViewJSON(string _module, string _rootType, string _activity);
	//void SetSeasonPaletteJSON();
	void SetSelectedViewIdx(int _selectedViewIdx);
	void SetSearchCriteriaJSON(json _criteria);
	//void SetAttScopes(string _attScopesJsonKey);
	void SetDateFlag(bool _isDateEditPresent);
	void SetDownloadFileName(string _downloadFileName);
	void SetDataFromResponse(json _param);
	void SetCopyShapeResult(string _copyresponse);
	void SetMaximumLimitForShapeResult();
	void CachePLMAttributes();
	map<string, string> GetAttributeIdAndValueMap(string _restApi);
	void GetAttributeDisplayNames(json& _materialResults);
	string GetDisplayName(map<string, string> _map, string _Id);
	void ResetShapeConfig();

	json m_seasonValuesJson;
	json m_shapemasterValuesJson;
	json GetShapeConfigJSON();
	json GetShapeHierarchyJSON();
	json GetShapeFieldsJSON();
	json GetShapePresetJSON();
	json GetShapeFilterJSON();
	json GetShapeViewJSON();
	json GetSeasonPaletteJSON(bool _refresh);
	int GetSelectedViewIdx();
	json GetSearchCriteriaJSON();
	QStringList GetAttScopes();
	bool GetDateFlag();
	string GetDownloadFileName();
	json GetShapeResultsSON();
	string GetTypename();
	int GetMaxResultCount();
	int GetResultsCount();
	void UpdateResultJson(json& _shapeResults, json& _shapeTypeValues, json& _shapemasters);
	//string GetThumbnailUrl(string _objectId);
	void GetEnumlistJson(string _restApi, json& _attributeJson);
	void SetIsModelExecuted(bool _isModelExecuted);
	bool GetIsModelExecuted();
	json GetShapeConfigJSONFromFile();
	string GetMaximumLimitForShapeResult();
	map <string, string> m_seasonIdAndDisplayMap;
	map <string, string> m_shapeTypeIdAndDisplayMap;
	map <string, string> m_shapeIdAndDisplayMap;
	map <string, string> m_shapemasterAndDisplayMap;
	map <string, string> m_themeIdAndDisplayMap;
	map <string, string> m_sleeveIdAndDisplayMap;
	map <string, string> m_lengthIdAndDisplayMap;
	map <string, string> m_silhouetteIdAndDisplayMap;
	map <string, string> m_fabricIdAndDisplayMap;
private:
	static ShapeConfig* _instance; // zero initialized by default
	json m_shapeConfigJson = json::object();
	json m_shapeHierarchyJson = json::object();
	json m_shapePresetJson = json::object();
	json m_shapeFilterJson = json::object();
	json m_shapeViewJson = json::object();
	json m_paletteJson = json::object();
	json m_searchCriteriaJson = json::object();
	QStringList m_attScopes;
	bool restData = true;
	string m_downloadFileName;
	string m_maxShapeResultsLimit;

	json m_shapeResults = json::array();
	string m_typename = "";
	int m_maxResultsCount = 0;
	int m_resultsCount = 0;

	void createFieldsJson(string& _fieldsJsonStringResponse, json& _defaultFieldsJson);
};
