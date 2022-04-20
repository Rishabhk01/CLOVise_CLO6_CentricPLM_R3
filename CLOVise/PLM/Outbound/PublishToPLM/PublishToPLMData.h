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
		bool m_createProductLoggedOut = false;
		bool m_createShapeLoggedOut = false;
		bool m_isSaveClicked = false;

		static PublishToPLMData* GetInstance();
		
		void SetActiveProductMetaData(json _resultJson);
		void SetActiveProductId(string _objectIdKey);
		void SetActiveShapeId(string _objectIdKey);
		void SetActiveShapeName(string _objectNameKey);
		void SetActiveShapeStatus(string _objectStatusKey);
		void SetActiveShapeObjectId(string _shapeObjectId);

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
		string GetActiveShapeId();
		string GetActiveShapeName();
		string GetActiveShapeStatus();
		string GetActiveShapeObjectId();

		string GetActiveProductStatus();
		bool GetIsProductOverridden();
		QStringList GetAvailableResolutions();
		QStringList GetAvailableNoOfImagesOptions();
		json GetDocumentConfigJSON();
		string Get3DModelType();
		json GetDocumentFieldsJSON();
		json GetUpdateShapeCacheData();

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
		void SetUpdateShapeCacheData(json _upsateShapeJson);

		 void SetUpdateStyleCacheData(json _upsateStyleJson);
		 string GetLatestRevision();
		 void  SetLatestRevision(string _latestRevisionId);
		 string GetGLBLatestRevision();
		 void  SetGLBLatestRevision(string _latestRevisionId);
		 void SetIsCreateNewDocument(bool _isNewDocument);
		 bool GetIsCreateNewDocument();
		 void SetIsCreateNewGLBDocument(bool _isNewGLBDocument);
		 bool GetIsCreateNewGLBDocument();
		 
		 void ResetPublishData();

	private:
		static PublishToPLMData* _instance; // zero initialized by default
		json m_ActiveProductJson;
		string m_ActiveProductId = BLANK;
		string m_ActiveShapeId = BLANK;
		string m_ActiveShapeObjectId = BLANK;
		string m_ActiveShapeName = BLANK;
		string m_ActiveShapeStatus = BLANK;
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
		json m_updateShapeData;
		string m_selectedNoOfImagePage = BLANK;
		json m_updateStyleData;
		bool m_productIdKeyExists = false;
		int m_maxZPRJUploadFileSize = 0;
		string m_latestDocRevisionId;
		string m_latestGLBRevisionId;
		bool m_createNewDocument;
		bool m_createNewGLBDocument=false;
};
