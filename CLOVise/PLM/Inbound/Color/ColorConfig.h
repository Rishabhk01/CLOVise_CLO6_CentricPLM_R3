#pragma once
/*
* Copyright 2020-2021 CLO-Vise. All rights reserved
*
* @file PLMColorData.h
*
* @brief Class declaration for cach Colors data in CLO from PLM.
* This class has all the variable declarations and function declarations which are used in storing PLM Color configuration related data to search in CLO
*
* @author GoVise
*
* @date 27-MAY-2020
*/
#include <string>

#include <QString>
#include <QStringList>

#include "CLOVise/PLM/Libraries/json.h"

using json = nlohmann::json;
using namespace std;

class ColorConfig
{
	
	public:
		bool m_isModelExecuted = false;
		int m_selectedViewIdx;
		bool m_isDateEditPresent = false;
		int m_sortedColumnNumber;
		bool m_colorLoggedOut = false;
		bool m_resultAfterLogout = false;
		static ColorConfig* GetInstance();
		json m_colorFieldsJson = json::object();
		bool m_isSearchColor = true;
		QStringList createdColorId;
		string m_mode;
		void InitializeColorData();
		//void SetColorConfigJSON(string _module, string _rootType, bool _initDepedentJsons);
		void SetColorConfigJSON();
		//void SetColorHierarchyJSON(string _hierarchyJsonKey);
		//void SetColorFieldsJSON(string _hierarchyJsonKey);
		void SetColorPresetJSON(string _hierarchyJsonKey);
		void SetColorFilterJSON(string _module, string _rootType, string _activity);
		void SetColorViewJSON(string _module, string _rootType, string _activity);
		//void SetSeasonPaletteJSON();
		void SetSelectedViewIdx(int _selectedViewIdx);
		void SetSearchCriteriaJSON(json _criteria);
		//void SetAttScopes(string _attScopesJsonKey);
		void SetDateFlag(bool _isDateEditPresent);
		void SetColorConfigJSONFromFile(string _module, string _rootType, bool _initDepedentJsons);
		//void InitializeColorData();
		bool GetIsModelExecuted();
		void SetIsModelExecuted(bool _isModelExecuted);
		void SetMaximumLimitForColorResult();

		json GetColorConfigJSON();
		json GetColorHierarchyJSON();
		json GetColorFieldsJSON();
		json GetColorFilterJSON();
		json GetColorViewJSON();
		json GetSeasonPaletteJSON(bool _refresh);
		int GetSelectedViewIdx();
		json GetSearchCriteriaJSON();
		QStringList GetAttScopes();
		bool GetDateFlag();
		void UpdateResultJson();
		void UpdateResultJson(json& _listJson);
		string GetThumbnailUrl(string _id);
		void GetEnumlistJson(string _restApi, json& _attributeJson);
		void ConvertToo(string& _convertValue, bool _isLower);
		void SetDataFromResponse(json _param);
		json GetColorResultsSON();
		string GetTypename();
		int GetMaxResultCount();
		int GetResultsCount();
		json GetColorConfigJSONFromFile();
		string GetMaximumLimitForColorResult();
		json GetPLMConfigJson();
		void ResetColorConfig();


	private:
		static ColorConfig* _instance; // zero initialized by default
		json m_ColorConfigJson = json::object();
		json m_ColorHierarchyJson = json::object();
		//json m_ColorFieldsJson = json::object();
		json m_ColorPresetJson = json::object();
		json m_ColorFilterJson = json::object();
		json m_ColorViewJson = json::object();
		json m_paletteJson = json::object();
		json m_searchCriteriaJson = json::object();
		json m_PLMConfigJson = json::object();
		QStringList m_attScopes;
		string m_maxColorResultsLimit;

		bool restData = true;

	json m_colorResults = json::array();
	string m_typename = "";
	int m_maxResultsCount = 0;
	int m_resultsCount = 0;

	void createFieldsJson(string& _fieldsJsonStringResponse, json& _defaultFieldsJson);
};
