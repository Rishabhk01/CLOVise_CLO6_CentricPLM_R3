#pragma once
#include <string>
#include <map>
#include <QStringList>
#include "CLOVise/PLM/Libraries/json.h"

using json = nlohmann::json;
using namespace std;

class DataHelper
{
public:
	json SetConfigJSON(string _module, string _rootType, bool _initDepedentJsons, string _FileName);
	json SetFilterJSON(string _module, string _rootType, string _activity, string _FileName);
	json SetViewJSON(string _module, string _rootType, string _activity, string _FileName);
	json SetJson(string _JsonKey, json _ColorConfigJson);
	void SetAttScopes(json _configJson, string _attScopesJsonKey, QStringList& _attScopes);
	json SetSeasonPaletteJSON(string _FileName);
	int SetSelectedViewIdx(int _selectedViewIdx);
	static DataHelper* GetInstance();
	void SetRefJson(json& _attributesFieldsListJson, string _defaultAttributeKey);
	void SetEnumJson(json& _attributesFieldsListJson, json _createFieldResponse);

	bool m_restData = false;

private:
	static DataHelper* _instance;
};

