#pragma once
/*
* Copyright 2020-2021 CLO-Vise. All rights reserved
*
* @file PLMMaterialData.h
*
* @brief Class declaration for cach Material data in CLO from PLM.
* This class has all the variable declarations and function declarations which are used in storing PLM Material configuration related data to search in CLO
*
* @author GoVise
*
* @date 16-JUN-2020
*/
#include <string>

#include <QString>
#include <QStringList>

#include "CLOVise/PLM/Libraries/json.h"

using json = nlohmann::json;
using namespace std;

class MaterialConfig
{

public:
	bool m_isModelExecuted = false;
	int m_selectedViewIdx;
	bool m_isDateEditPresent = false;
	int m_sortedColumnNumber;

	static MaterialConfig* GetInstance();
	static void	Destroy();
	//void InitializeMaterialData();
	json m_materialFieldsJson = json::object();
	bool m_materialLoggedOut = false;
	bool m_resultAfterLogout = false;

	void InitializeMaterialData();
	//void SetMaterialConfigJSON(string _module, string _rootType, bool _initDepedentJsons);
	void SetMaterialConfigJSON();
	//void SetMaterialHierarchyJSON(string _hierarchyJsonKey);
	//void SetMaterialFieldsJSON(string _hierarchyJsonKey);
	void SetMaterialPresetJSON(string _hierarchyJsonKey);
	void SetMaterialFilterJSON(string _module, string _rootType, string _activity);
	void SetMaterialViewJSON(string _module, string _rootType, string _activity);
	//void SetSeasonPaletteJSON();
	void SetSelectedViewIdx(int _selectedViewIdx);
	void SetSearchCriteriaJSON(json _criteria);
	//void SetAttScopes(string _attScopesJsonKey);
	void SetDateFlag(bool _isDateEditPresent);
	void SetMaterialConfigJSONFromFile(string _module, string _rootType, bool _initDepedentJsons);
	void SetMaximumLimitForMaterialResult();
	void SetIsSaveAndCloseClicked(bool _isSaveClicked);
	void SetMaterialCategoryEnumId(string _materialCategoryEnumId);
	void ResetMaterialConfig();

	json GetMaterialConfigJSON();
	json GetMaterialFieldsJSON();
	json GetMaterialHierarchyJSON();
	json GetMaterialFilterJSON();
	json GetMaterialViewJSON();
	json GetSeasonPaletteJSON(bool _refresh);
	int GetSelectedViewIdx();
	json GetSearchCriteriaJSON();
	QStringList GetAttScopes();
	bool GetDateFlag();
	void UpdateResultJson(json& _materialResults, json& _materialType);
	string GetThumbnailUrl(string _id);
	void GetEnumlistJson(string _restApi, json& _attributeJson);
	void ConvertToo(string& _convertValue, bool _isLower);
	//void InitializeMaterialData();	
	void SetDataFromResponse(json _param);
	json GetMaterialResultsSON();
	string GetTypename();
	int GetMaxResultCount();
	int GetResultsCount();
	void GetDataFromLocalJson(json& _fieldsJson, json _defaultFieldsJson, QStringList& _attKeyList);
	void SetUpdateMaterialCacheData(json _updateMaterialData);
	void SetIsRadioButton(bool _setIsRadioButton);
	bool GetIsRadioButton();
	json GetUpdateMaterialCacheData();
	void SetIsModelExecuted(bool _isModelExecuted);
	bool GetIsModelExecuted();
	json GetMaterialConfigJSONFromFile();
	string GetMaximumLimitForMaterialResult();
	bool GetIsSaveAndCloseClicked();
	string GetMaterialCategoryEnumId();

private:
	static MaterialConfig* _instance; // zero initialized by default
	json m_materialConfigJson = json::object();
	json m_materialHierarchyJson = json::object();
	json m_materialPresetJson = json::object();
	json m_materialFilterJson = json::object();
	json m_materialViewJson = json::object();
	json m_paletteJson = json::object();
	json m_searchCriteriaJson = json::object();
	json m_updateMaterialData = json::object();
	string m_maxMaterialResultsLimit;
	bool m_isSaveAndCloseClicked = false;
	string m_materialCategoryEnumId;

	QStringList m_attScopes;
	bool restData = true;
	bool m_setIsRadioButton = false;

	json m_materialResults = json::array();
	string m_typename = "";
	int m_maxResultsCount = 0;
	int m_resultsCount = 0;

	void createFieldsJson(string& _fieldsJsonStringResponse, json& _defaultFieldsJson);
};