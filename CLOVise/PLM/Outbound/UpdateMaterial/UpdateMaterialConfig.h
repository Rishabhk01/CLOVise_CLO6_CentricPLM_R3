#pragma once
/*
* Copyright 2020-2021 CLO-Vise. All rights reserved
*
* @file UpdateMaterialConfig.h
*
* @brief Class declaration for cach downloaded Product and Document data in CLO from PLM.
* This class has all the variable declarations and function declarations which are used in storing PLM Product and Document configuration related data to downloaded in CLO
*
* @author GoVise
*
* @date 14-AUG-2020
*/
#include <string>

#include <QString>
#include <QStringList>

#include "CLOVise/PLM/Libraries/json.h"
#include "CLOVise/PLM/Helper/Util/Definitions.h"

using json = nlohmann::json;
using namespace std;

class UpdateMaterialConfig
{
public:
	bool isModelExecuted = false;
	bool m_isDateEditPresent = false;

	static UpdateMaterialConfig* GetInstance();
	static void	Destroy();

	void Set3DModelType(string _key);
	void SetUpdateMaterialConfigJSON(string _module, string _rootType, bool _initDepedentJsons);
	void SetDocumentFieldsJSON(string _fieldsJsonKey);
	void SetDateFlag(bool _isDateEditPresent);
	void SetMaxUploadFileSize(string _key);

	json GetUpdateMaterialConfigJSON();
	string Get3DModelType();
	json GetDocumentFieldsJSON();
	bool GetDateFlag();
	int GetMaxUploadFileSize();

private:
	static UpdateMaterialConfig* _instance; // zero initialized by default
	json m_documentConfigJson = json::object();
	json m_documentFieldsJson = json::array();
	string m_3DModelType = BLANK;
	json m_Active3DModeJson;
	string m_selectedNoOfImagePage = BLANK;
	bool m_productIdKeyExists = false;
	int m_maxZPRJUploadFileSize = 0;
};
