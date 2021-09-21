#pragma once
/*
* Copyright 2020-2021 CLO-Vise. All rights reserved
*
* @file PLMColorData.h
*
* @brief Class declaration for cach Sample data in CLO from PLM.
* This class has all the variable declarations and function declarations which are used in storing PLM Sample configuration related data to search in CLO
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

class SampleConfig
{

public:
	bool isModelExecuted = false;
	int m_selectedViewIdx;
	bool m_isDateEditPresent = false;

	static SampleConfig* GetInstance();
	static void	Destroy();
	void InitializeSampleData();

	void SetSampleConfigJSON(string _module, string _rootType, bool _initDepedentJsons);
	void SetSamplePresetJSON(string _hierarchyJsonKey);
	void SetSampleFilterJSON(string _module, string _rootType, string _activity);
	void SetSampleViewJSON(string _module, string _rootType, string _activity);
	void SetSelectedViewIdx(int _selectedViewIdx);
	void SetSearchCriteriaJSON(json _criteria);
	void SetDateFlag(bool _isDateEditPresent);

	json GetSampleConfigJSON();
	json GetSampleHierarchyJSON();
	json GetSampleFieldsJSON();
	json GetSampleFilterJSON();
	json GetSampleViewJSON();
	int GetSelectedViewIdx();
	json GetSearchCriteriaJSON();
	QStringList GetAttScopes();
	bool GetDateFlag();

private:
	static SampleConfig* _instance; // zero initialized by default
	json m_sampleConfigJson = json::object();
	json m_sampleHierarchyJson = json::object();
	json m_sampleFieldsJson = json::object();
	json m_samplePresetJson = json::object();
	json m_sampleFilterJson = json::object();
	json m_sampleViewJson = json::object();
	json m_paletteJson = json::object();
	json m_searchCriteriaJson = json::object();
	QStringList m_attScopes;
	bool restData = true;
};
