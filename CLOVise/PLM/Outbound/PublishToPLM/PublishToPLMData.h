#pragma once
/*
* Copyright 2020-2021 CLO-Vise. All rights reserved
*
* @file PublishToPLMData.h
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

class PublishToPLMData
{
	public:
		bool m_isModelExecuted = false;
		bool m_isDateEditPresent = false;

		static PublishToPLMData* GetInstance();
		static void	Destroy();
		
		void SetActiveProductMetaData(json _resultJson);
		void SetActiveProductId(string _objectIdKey);
		void SetActiveProductName(string _objectNameKey);
		void SetActiveProductStatus(string _objectStatusKey);
		void SetIsProductOverridden(bool _flag);
		void SetAvailableResolutions(string _key);
		void SetAvailableNoOfImagesOptions(string _key);
		void Set3DModelType(string _key);
		void SetDocumentConfigJSON(string _module, string _rootType, bool _initDepedentJsons);
		void SetDocumentFieldsJSON(string _fieldsJsonKey);
		void SetTurnTableImageResolution(string _key);
		void SetTurnTableNoOfImages(string _key);
		void SetTurnTableImageHeightAndWidth();
		void SetActive3DModelMetaData(json _resultJson);
		void SetActiveProductObjectId(string _productObjectId);
		void Set3DModelObjectId(string _3DModelObjectId);
		void SetSelectedNoOfImagePageImages(string _selected, bool _store, bool _setCached);
		void SetProductIdKeyExists(json _json);
		void SetDateFlag(bool _isDateEditPresent);
		void SetMaxUploadFileSize(string _key);
		QString GetStyleColorwayColumnNames(string _columnName);

		json GetActiveProductMetaData();
		string GetActiveProductId();
		string GetActiveProductName();
		string GetActiveProductStatus();
		bool GetIsProductOverridden();
		QStringList GetAvailableResolutions();
		QStringList GetAvailableNoOfImagesOptions();
		json GetDocumentConfigJSON();
		string Get3DModelType();
		json GetDocumentFieldsJSON();
		string GetTurnTableImageResolution();
		string GetTurnTableNoOfImages();
		string GetTurnTableImageHeight();
		string GetTurnTableImageWidth();
		json GetActive3DModelMetaData();
		string GetActiveProductObjectId();
		string Get3DModelObjectId();
		string GetSelectedNoOfImagePageImages();
		bool GetProductIdKeyExists();
		bool GetDateFlag();
		int GetMaxUploadFileSize();

		void SetIsModelExecuted(bool _isModelExecuted);
		bool GetIsModelExecuted();
		json GetUpdateStyleCacheData();
		 void SetUpdateStyleCacheData(json _upsateStyleJson);
		 string GetLatestRevision();
		 void  SetLatestRevision(string _latestRevisionId);
		 void SetIsCreateNewDocument(bool _isNewDocument);
		 bool GetIsCreateNewDocument();
		 
	private:
		static PublishToPLMData* _instance; // zero initialized by default
		json m_ActiveProductJson;
		string m_ActiveProductId = BLANK;
		string m_ActiveProductObjectId = BLANK;
		string m_3DModelObjectId = BLANK;
		string m_ActiveProductName = BLANK;
		string m_ActiveProductStatus = BLANK;
		bool m_isProductOverridden;
		QStringList m_availableResolutions;
		QStringList m_availableNoOfImages;
		json m_documentConfigJson = json::object();
		json m_documentFieldsJson = json::array();
		string m_3DModelType = BLANK;
		string m_turnTableImageResolution = BLANK;
		string m_turnTableNoOfImages = BLANK;
		string m_turnTableImageHeight = BLANK;
		string m_turnTableImageWidth = BLANK;
		json m_Active3DModeJson;
		string m_selectedNoOfImagePage = BLANK;
		json m_updateStyleData;
		bool m_productIdKeyExists = false;
		int m_maxZPRJUploadFileSize = 0;
		string m_latestDocRevisionId;
		bool m_createNewDocument;
};
