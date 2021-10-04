#pragma once
/*
* Copyright 2020-2021 CLO-Vise. All rights reserved
*
* @file PLMPrintData.h
*
* @brief Class declaration for cach prints data in CLO from PLM.
* This class has all the variable declarations and function declarations which are used in storing PLM Print configuration related data to search in CLO
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

class PrintConfig
{
	
	public:
		bool m_isModelExecuted = false;
		int m_selectedViewIdx;
		bool m_isDateEditPresent = false;

		static PrintConfig* GetInstance();
		static void	Destroy();
		json m_printFieldsJson = json::object();

		void InitializePrintData();
		//void SetColorConfigJSON(string _module, string _rootType, bool _initDepedentJsons);
		void SetPrintConfigJSON();
		//void SetColorHierarchyJSON(string _hierarchyJsonKey);
		//void SetColorFieldsJSON(string _hierarchyJsonKey);
		void SetPrintPresetJSON(string _hierarchyJsonKey);
		void SetPrintFilterJSON(string _module, string _rootType, string _activity);
		void SetPrintViewJSON(string _module, string _rootType, string _activity);
		//void SetSeasonPaletteJSON();
		void SetSelectedViewIdx(int _selectedViewIdx);
		void SetSearchCriteriaJSON(json _criteria);
		//void SetAttScopes(string _attScopesJsonKey);
		void SetDateFlag(bool _isDateEditPresent);
		void SetPrintConfigJSONFromFile(string _module, string _rootType, bool _initDepedentJsons);
		//void InitializeColorData();
		bool GetIsModelExecuted();
		void SetIsModelExecuted(bool _isModelExecuted);
		void SetMaximumLimitForPrintResult();

		json GetPrintConfigJSON();
		json GetPrintHierarchyJSON();
		json GetPrintFieldsJSON();
		json GetPrintFilterJSON();
		json GetPrintViewJSON();
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
		json GetPrintResultsSON();
		string GetTypename();
		int GetMaxResultCount();
		int GetResultsCount();
		json GetPrintConfigJSONFromFile();
		string GetMaximumLimitForPrintResult();


	private:
		static PrintConfig* _instance; // zero initialized by default
		json m_PrintConfigJson = json::object();
		json m_PrintHierarchyJson = json::object();
		//json m_ColorFieldsJson = json::object();
		json m_PrintPresetJson = json::object();
		json m_PrintFilterJson = json::object();
		json m_PrintViewJson = json::object();
		json m_paletteJson = json::object();
		json m_searchCriteriaJson = json::object();
		QStringList m_attScopes;
		string m_maxPrintResultsLimit;

		bool restData = true;

	json m_printResults = json::array();
	string m_typename = "";
	int m_maxResultsCount = 0;
	int m_resultsCount = 0;

	void createFieldsJson(string& _fieldsJsonStringResponse, json& _defaultFieldsJson);
};
