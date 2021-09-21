#pragma once
/*
* Copyright 2020-2021 CLO-Vise. All rights reserved
*
* @file PLMDocumentData.h
*
* @brief Class declaration for cach Document data in CLO from PLM.
*  This class has all the variable declarations and function declarations which are used in storing PLM Document configuration related data to search in CLO
*
* @author GoVise
*
* @date 24-JUL-2020
*/
#include <string>

#include <QString>
#include <QStringList>

#include "CLOVise/PLM/Libraries/json.h"

using json = nlohmann::json;
using namespace std;

class PLMDocumentData
{

public:
	bool isModelExecuted = false;
	int m_selectedViewIdx;
	bool m_isDateEditPresent = false;

	static PLMDocumentData* GetInstance();
	static void	Destroy();
	void InitializeDocumentData();

	void SetDocumentConfigJSON(string _module, string _rootType, bool _initDepedentJsons);
	void SetDocumentHierarchyJSON(string _hierarchyJsonKey);
	void SetDocumentFieldsJSON(string _hierarchyJsonKey);
	void SetDocumentPresetJSON(string _hierarchyJsonKey);
	void SetDocumentFilterJSON(string _module, string _rootType, string _activity);
	void SetDocumentViewJSON(string _module, string _rootType, string _activity);
	void SetSeasonPaletteJSON();
	void SetSelectedViewIdx(int _selectedViewIdx);
	void SetSearchCriteriaJSON(json _criteria);
	void SetAttScopes(string _attScopesJsonKey);
	void Set3DModelFileMetaDataJSONList(json _json);
	void SetProductIdKeyExists(json _json);
	void SetDateFlag(bool _isDateEditPresent);

	json GetDocumentConfigJSON();
	json GetDocumentHierarchyJSON();
	json GetDocumentFieldsJSON();
	json GetDocumentPresetJSON();
	json GetDocumentFilterJSON();
	json GetDocumentViewJSON();
	json GetSeasonPaletteJSON(bool _refresh);
	int GetSelectedViewIdx();
	json GetSearchCriteriaJSON();
	QStringList GetAttScopes();
	json Get3DModelFileMetaDataJSONList();
	bool GetProductIdKeyExists();		
	bool GetDateFlag();

private:
	static PLMDocumentData* _instance; // zero initialized by default
	json m_documentConfigJson = json::object();
	json m_documentHierarchyJson = json::object();
	json m_documentFieldsJson = json::object();
	json m_documentPresetJson = json::object();
	json m_documentFilterJson = json::object();
	json m_documentViewJson = json::object();
	json m_paletteJson = json::object();
	json m_searchCriteriaJson = json::object();
	QStringList m_attScopes;
	json m_metadataJSONList;
	bool m_productIdKeyExists = false;
	bool restData = true;
};
