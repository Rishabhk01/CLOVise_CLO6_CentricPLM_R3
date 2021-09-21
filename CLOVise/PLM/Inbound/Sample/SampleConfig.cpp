/*
* Copyright 2020-2021 CLO-Vise. All rights reserved
*
* @file PLMSampleConfig.cpp
*
* @brief Class implementation for cach Sample data in CLO from PLM.
* This class has all the variable and methods implementation which are used in storing PLM Sample configuration related data to search in CLO
*
* @author GoVise
*
* @date  27-MAY-2020
*/
#include "SampleConfig.h"
#include "CLOAPIInterface.h"
#include "CLOVise/PLM/Helper/Util/RestAPIUtility.h"
#include "CLOVise/PLM/Helper/Util/FormatHelper.h"
#include "CLOVise/PLM/Helper/Util/Logger.h"
#include "CLOVise/PLM/Helper/Util/DataHelper.h"

using json = nlohmann::json;

SampleConfig* SampleConfig::_instance = NULL;
SampleConfig* SampleConfig::GetInstance()
{
	if (_instance == NULL)
	{
		_instance = new SampleConfig();
	}
	return _instance;
}
void  SampleConfig::Destroy()
{
	if (_instance)
	{
		delete _instance;
		_instance = NULL;
	}
}

/*
* Description - SetSampleConfigJSON() method used to get the configuration data from server/file.
* Parameter -  string.
* Exception - exception, Char *
* Return -
*/

void SampleConfig::SetSampleConfigJSON(string _module, string _rootType, bool _initDepedentJsons)
{
	Logger::Info("SampleConfig -> SetSampleConfigJSON() start");
	//need to gethe response from plm.
	m_sampleConfigJson = DataHelper::GetInstance()->SetConfigJSON(_module, _rootType, _initDepedentJsons, PLM_SAMPLE_FILE_NAME);
	Logger::Info("SampleConfig -> SetSampleConfigJSON() End");
}

/*
* Description - GetSampleConfigJSON() method used to get the cached configuration data.
* Parameter -
* Exception -
* Return - json.
*/
json SampleConfig::GetSampleConfigJSON()
{
	Logger::Info("SampleConfig -> GetSampleConfigJSON() start");

	if (m_sampleConfigJson == nullptr || m_sampleConfigJson.empty())
		SetSampleConfigJSON(SAMPLE_MODULE, SAMPLE_ROOT_TYPE, false);
	Logger::Info("m_sampleConfigJson -> GetSampleConfigJSON() end");

	return m_sampleConfigJson;
}

/*
* Description - GetSampleHierarchyJSON() method used to get cached hierarchy data.
* Parameter -
* Exception -
* Return - json.
*/
json SampleConfig::GetSampleHierarchyJSON()
{
	Logger::Info("SampleConfig -> GetSampleHierarchyJSON() start");
	if (m_sampleHierarchyJson == nullptr || m_sampleHierarchyJson.empty())
		m_sampleHierarchyJson = DataHelper::GetInstance()->SetJson(HIERARCHY_JSON_KEY, m_sampleConfigJson);

	Logger::Info("SampleConfig -> GetSampleHierarchyJSON() end");
	return m_sampleHierarchyJson;
}

/*
* Description - GetAttScopes() method used to get cached the attribute scopes data.
* Parameter -
* Exception -
* Return - QStringList.
*/
QStringList SampleConfig::GetAttScopes()
{
	Logger::Info("SampleConfig -> GetAttScopes() start");
	if (m_attScopes.empty())
		DataHelper::GetInstance()->SetAttScopes(m_sampleConfigJson, ATTSCOPE_JSON_KEY, m_attScopes);

	Logger::Info("SampleConfig -> GetAttScopes() end");
	return m_attScopes;
}

/*
* Description - GetSampleFieldsJSON() method used to get cached the sample fields data.
* Parameter -
* Exception -
* Return - json.
*/
json SampleConfig::GetSampleFieldsJSON()
{
	Logger::Info("SampleConfig -> GetSampleFieldsJSON() start");

	if (m_sampleFieldsJson == nullptr || m_sampleFieldsJson.empty())
		m_sampleFieldsJson = DataHelper::GetInstance()->SetJson(FIELDLIST_JSON_KEY, m_sampleConfigJson);
	Logger::Info("SampleConfigc -> GetSampleFieldsJSON() end");

	return m_sampleFieldsJson;
}

/*
* Description - SetSamplePresetJSON() method used to cache the preset data.
* Parameter -  string.
* Exception - exception, Char *
* Return -
*/
void SampleConfig::SetSamplePresetJSON(string _presetJsonKey)
{
	Logger::Info("SampleConfig -> SetSamplePresetJSON() start");

	try
	{
		string samplePresetStr = "";
		samplePresetStr = Helper::GetJSONValue<string>(GetSampleConfigJSON(), _presetJsonKey, false);
		m_samplePresetJson = json::parse(samplePresetStr);
	}
	catch (exception e)
	{
		Logger::Error("SampleConfig -> SetSamplePresetJSON Exception :: " + string(e.what()));
		throw e;
	}
	catch (const char* msg)
	{
		Logger::Error("SampleConfig -> SetSamplePresetJSON Exception :: " + string(msg));
		throw msg;
	}
	Logger::Info("SampleConfig -> SetSamplePresetJSON() end");

}

/*
* Description - SetSampleFilterJSON() method used to cache the filter data from server/file.
* Parameter -  string, string, string.
* Exception - exception, Char *
* Return -
*/

void SampleConfig::SetSampleFilterJSON(string _module, string _rootType, string _activity)
{
	m_sampleFilterJson = DataHelper::GetInstance()->SetFilterJSON(_module, _rootType, _activity, PLM_SAMPLE_FILTER_FILE);
}

/*
* Description - GetSampleFilterJSON() method used to get cached the filter data.
* Parameter -
* Exception -
* Return - json.
*/
json SampleConfig::GetSampleFilterJSON()
{
	Logger::Info("SampleConfig -> GetSampleFilterJSON() start");
	if (m_sampleFilterJson == nullptr || m_sampleFilterJson.empty())
		SetSampleFilterJSON(SAMPLE_MODULE, SAMPLE_ROOT_TYPE, SAMPLE_SEARCH_ACTIVITY);

	Logger::Info("SampleConfig -> GetSampleFilterJSON() end");
	return m_sampleFilterJson;
}

/*
* Description - SetSampleViewJSON() method used to cache the views data from server/file.
* Parameter -  string, string, string.
* Exception - exception, Char *
* Return -
*/
void SampleConfig::SetSampleViewJSON(string _module, string _rootType, string _activity)
{
	m_sampleViewJson = DataHelper::GetInstance()->SetViewJSON(_module, _rootType, _activity, PLM_SAMPLE_VIEW_FILE);
}
/*
* Description - GetSampleViewJSON() method used to get cached the view data.
* Parameter -
* Exception -
* Return - json.
*/
json SampleConfig::GetSampleViewJSON()
{
	Logger::Info("SampleConfig -> GetSampleViewJSON() start");

	if (m_sampleViewJson == nullptr || m_sampleViewJson.empty())
		SetSampleViewJSON(SAMPLE_MODULE, SAMPLE_ROOT_TYPE, SAMPLE_SEARCH_ACTIVITY);

	Logger::Info("SampleConfig -> GetSampleViewJSON() end");
	return m_sampleViewJson;
}


/*
* Description - SetSelectedViewIdx() method used to cache the selected index.
* Parameter -  int.
* Exception -
* Return -
*/
void SampleConfig::SetSelectedViewIdx(int _selectedViewIdx)
{
	Logger::Info("SampleConfig -> SetSelectedViewIdx() Start");
	m_selectedViewIdx = _selectedViewIdx;
	Logger::Info("SampleConfig -> SetSelectedViewIdx() End");
}

/*
* Description - GetSelectedViewIdx() method used to get cached selected index.
* Parameter -
* Exception -
* Return - int
*/
int SampleConfig::GetSelectedViewIdx()
{
	return m_selectedViewIdx;
}

/*
* Description - SetSearchCriteriaJSON() method used to cache the data.
* Parameter -  json.
* Exception -
* Return -
*/
void SampleConfig::SetSearchCriteriaJSON(json _criteria)
{
	Logger::Info("SampleConfig -> SetSelectedViewIdx() Start");
	m_searchCriteriaJson = _criteria;
	Logger::Info("SampleConfig -> SetSelectedViewIdx() End");
}

/*
* Description - GetSearchCriteriaJSON() method used to get cached data.
* Parameter -
* Exception -
* Return - json.
*/
json SampleConfig::GetSearchCriteriaJSON()
{
	return m_searchCriteriaJson;
}

/*
* Description - SetDateFlag() method used to set date exist flag.
* Parameter -  bool.
* Exception -
* Return -
*/
void SampleConfig::SetDateFlag(bool _isDateEditPresent)
{
	m_isDateEditPresent = _isDateEditPresent;
}

/*
* Description - GetDateFlag() method used to get date exist flag.
* Parameter -
* Exception -
* Return - bool.
*/
bool SampleConfig::GetDateFlag()
{
	return m_isDateEditPresent;
}

/*
* Description - InitializeColorData() method used to initialize color data from configure response from rest/file.
* Parameter -
* Exception -
* Return -
*/
void SampleConfig::InitializeSampleData()
{
	Logger::Info("PLMSampleSearch -> InitializeSampleData() -> Start");
	RESTAPI::SetProgressBarData(20, "Loading Sample configuration", true);
	SampleConfig::GetInstance()->GetSampleConfigJSON();
	SampleConfig::GetInstance()->GetSampleHierarchyJSON();
	RESTAPI::SetProgressBarData(40, "Loading Sample configuration", true);
	SampleConfig::GetInstance()->GetSampleFieldsJSON();
	SampleConfig::GetInstance()->GetAttScopes();
	RESTAPI::SetProgressBarData(60, "Loading Sample configuration", true);
	SampleConfig::GetInstance()->GetSampleFilterJSON();
	SampleConfig::GetInstance()->GetSampleViewJSON();
	RESTAPI::SetProgressBarData(80, "Loading Sample configuration", true);
	Configuration::GetInstance()->SetSupportedAttsList(SampleConfig::GetInstance()->GetSampleConfigJSON());
	Configuration::GetInstance()->SetResultsSupportedAttsList(SampleConfig::GetInstance()->GetSampleConfigJSON());
	RESTAPI::SetProgressBarData(100, "Loading Sample configuration", true);
	RESTAPI::SetProgressBarData(0, "", false);
	Logger::Info("PLMSampleSearch -> InitializeSampleData() -> end");
}