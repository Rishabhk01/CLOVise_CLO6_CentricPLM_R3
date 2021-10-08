/*
* Copyright 2020-2021 CLO-Vise. All rights reserved
*
* @file PublishToPLMData.cpp
*
* @brief Class implementation for cach downloaded Product and Document data in CLO from PLM.
* This class has all the variable and methods implementation which are used in storing PLM Product and Document configuration related data to downloaded in CLO
*
* @author GoVise
*
* @date 14-AUG-2020
*/
#include "PublishToPLMData.h"
#include "CLOAPIInterface.h"
#include "CLOVise/PLM/Helper/Util/RestAPIUtility.h"
#include "CLOVise/PLM/Helper/Util/FormatHelper.h"
#include "CLOVise/PLM/Helper/Util/Logger.h"

using json = nlohmann::json;

PublishToPLMData* PublishToPLMData::_instance = NULL;
PublishToPLMData* PublishToPLMData::GetInstance()
{
	if (_instance == NULL)
	{
		_instance = new PublishToPLMData();
	}
	return _instance;
}

/*
* Description - SetDocumentConfigJSON() method used to set document configuration json.
* Parameter -  string, string, bool.
* Exception - exception, Char *
* Return -
*/
void PublishToPLMData::SetDocumentConfigJSON(string _module, string _rootType, bool _initDepedentJsons)
{
	Logger::Info("PublishToPLMData -> SetDocumentConfigJSON start");
	Logger::Logger(" Publish To PLM initialiazing...");
	string configJsonString = "";
	const char* restErrorMsg = "";
	try
	{
		m_documentConfigJson = Helper::makeRestcallGet(RESTAPI::STYLE_DETAIL_API, "/centric%3A%2F%2FREFLECTION%2FINSTANCE%2FCreateFormDefinition%2FStyle/configurable_attributes?skip=0&limit=100", "", "Loading " + Configuration::GetInstance()->GetLocalizedStyleClassName() + " details..");
		Logger::RestAPIDebug("m_documentConfigJson::" + to_string(m_documentConfigJson));
		//Logger::Info("PublishToPLMData -> if (restData)  start");
		//UTILITY_API->DisplayMessageBox("Calling rest api");
		//vector<pair<string, string>> headerNameAndValueList;
		//headerNameAndValueList.push_back(make_pair("content-Type", "application/json"));
		//headerNameAndValueList.push_back(make_pair("Cookie", Configuration::GetInstance()->GetBearerToken()));
		//string resultJsonString = REST_API->CallRESTGet(Configuration::GetInstance()->GetPLMServerURL() + RESTAPI::STYLE_DETAIL_API + "/centric%3A%2F%2FREFLECTION%2FINSTANCE%2FCreateFormDefinition%2FStyle/configurable_attributes", headerNameAndValueList, "Loading style details");
		////UTILITY_API->DisplayMessageBox(resultJsonString);

		////configJsonString = RESTAPI::RESTMethodSubmit(RESTAPI::CREATEORUPDATE_FIELDS_API, param);
		//Logger::Debug("configJsonString---" + resultJsonString);
		//if (!FormatHelper::HasContent(resultJsonString))
		//{
		//	throw "Unable to initiliaze Document Configuration. Please try again or Contact your System Administrator.";
		//}


		//int length = resultJsonString.length();
		//Logger::Debug("PublishToPLMData -> SetDocumentConfigJSON Length ::" + to_string(length));
		//int indexforjson = resultJsonString.find("[");
		//Logger::Debug("PublishToPLMData -> SetDocumentConfigJSON Index of [ ::" + to_string(indexforjson));
		////string resultJsonString = "";
		//string FinalresultJsonString = resultJsonString.substr(indexforjson, length);
		//Logger::Debug("PublishToPLMData -> SetDocumentConfigJSON FinalresultJsonString");
		//	//UTILITY_API->DisplayMessageBox(FinalresultJsonString);
		//	m_documentConfigJson = json::parse(FinalresultJsonString);


	//	m_documentConfigJson = json::parse(configJsonString);

		/*if (_initDepedentJsons)
		{
			Set3DModelType(_3DMODEL_TYPE_KEY);
			SetDocumentFieldsJSON(FIELDLIST_JSON_KEY);
		}*/
	}

	catch (string msg)
	{
		Logger::Error("PublishToPLMData -> SetDocumentConfigJSON() Exception - " + msg);
		throw msg;
	}
	catch (exception& e)
	{

		Logger::Error("PublishToPLMData -> SetDocumentConfigJSON() Exception - " + string(e.what()));
		throw e.what();
	}
	catch (const char* msg)
	{

		Logger::Error("PublishToPLMData -> SetDocumentConfigJSON() Exception - " + string(msg));
		throw msg;
	}
	Logger::Info("PublishToPLMData -> SetDocumentConfigJSON end");
}

/*
* Description - GetDocumentConfigJSON() method used to get document configuration json.
* Parameter -
* Exception -
* Return - json.
*/
json PublishToPLMData::GetDocumentConfigJSON()
{
	Logger::Info("PublishToPLMData -> GetDocumentConfigJSON() start");

	if (m_documentConfigJson == nullptr || m_documentConfigJson.empty())
		SetDocumentConfigJSON(PRODUCT_MODULE, PRODUCT_ROOT_TYPE, true);
	Logger::Info("PublishToPLMData -> GetDocumentConfigJSON() end");

	return m_documentConfigJson;
}

/*
* Description - Set3DModelType() method used to set 3d model type.
* Parameter -  string.
* Exception -
* Return -
*/
void PublishToPLMData::Set3DModelType(string _key)
{
	m_3DModelType = Helper::GetJSONValue<string>(GetDocumentConfigJSON(), _key, true);
}

/*
* Description - Get3DModelType() method used to get 3d model type.
* Parameter -
* Exception -
* Return - string.
*/
string PublishToPLMData::Get3DModelType()
{
	if (!FormatHelper::HasContent(m_3DModelType))
		Set3DModelType(_3DMODEL_TYPE_KEY);
	return m_3DModelType;
}

/*
* Description - SetDocumentFieldsJSON() method used to set document fields json.
* Parameter -  string.
* Exception - exception, Char *
* Return -
*/
void PublishToPLMData::SetDocumentFieldsJSON(string _fieldsJsonKey)
{
	Logger::Info("PublishToPLMData -> SetDocumentFieldsJSON() start");

	try
	{
		string documentFieldsStr = "";
		GetDocumentConfigJSON();
		//documentFieldsStr = Helper::GetJSONValue<string>(GetDocumentConfigJSON(), _fieldsJsonKey, false);
		//m_documentFieldsJson = json::parse(documentFieldsStr);
	}
	catch (string msg)
	{
		Logger::Error("PublishToPLMData -> SetDocumentFieldsJSON Exception :: " + msg);
		throw msg;
	}
	catch (exception& e)
	{
		Logger::Error("PublishToPLMData -> SetDocumentFieldsJSON Exception :: " + string(e.what()));
		throw e;
	}
	catch (const char* msg)
	{
		Logger::Error("PublishToPLMData -> SetDocumentFieldsJSON Exception :: " + string(msg));
		throw msg;
	}
	Logger::Info("PublishToPLMData -> SetDocumentFieldsJSON() end");
	
}

/*
* Description - GetDocumentFieldsJSON() method used to get document fields json.
* Parameter -
* Exception -
* Return - json.
*/
json PublishToPLMData::GetDocumentFieldsJSON()
{
	Logger::Info("PublishToPLMData -> GetDocumentFieldsJSON() start");

	if (m_documentFieldsJson == nullptr || m_documentFieldsJson.empty())
		SetDocumentFieldsJSON(FIELDLIST_JSON_KEY);
	Logger::Info("PublishToPLMData -> GetDocumentFieldsJSON() end");

	//m_documentConfigJson
	return m_documentConfigJson;
}

/*
* Description - SetActiveProductMetaData() method used to set active product meta data.
* Parameter -  json.
* Exception -
* Return -
*/
void PublishToPLMData::SetActiveProductMetaData(json _resultJson)
{
	m_ActiveProductJson = _resultJson;
}

/*
* Description - GetActiveProductMetaData() method used to get active product meta data.
* Parameter -
* Exception -
* Return - json.
*/
json PublishToPLMData::GetActiveProductMetaData()
{
	return m_ActiveProductJson;
}

/*
* Description - SetActive3DModelMetaData() method used to set active 3d model meta data.
* Parameter -  json.
* Exception -
* Return -
*/
void PublishToPLMData::SetActive3DModelMetaData(json _resultJson)
{
	m_Active3DModeJson = _resultJson;
}

/*
* Description - GetActive3DModelMetaData() method used to get active 3d model meta data.
* Parameter -
* Exception -
* Return - json.
*/
json PublishToPLMData::GetActive3DModelMetaData()
{
	return m_Active3DModeJson;
}

/*
* Description - SetActiveProductId() method used to set active product id.
* Parameter -  string.
* Exception -
* Return -
*/
void PublishToPLMData::SetActiveProductId(string _productId)
{
	if (!FormatHelper::HasContent(_productId))
	{
		_productId = "";
	}
	m_ActiveProductId = _productId;
}

/*
* Description - GetActiveProductId() method used to get active product id.
* Parameter -
* Exception -
* Return - string.
*/
string PublishToPLMData::GetActiveProductId()
{
	return m_ActiveProductId;
}

/*
* Description - SetActiveProductObjectId() method used to set active product object id.
* Parameter -  string.
* Exception -
* Return -
*/
void PublishToPLMData::SetActiveProductObjectId(string _productObjectId)
{
	if (!FormatHelper::HasContent(_productObjectId))
	{
		_productObjectId = "";
	}
	m_ActiveProductObjectId = _productObjectId;
}

/*
* Description - GetActiveProductObjectId() method used to get active product object id.
* Parameter -
* Exception -
* Return - string.
*/
string PublishToPLMData::GetActiveProductObjectId()
{
	return m_ActiveProductObjectId;
}

/*
* Description - Set3DModelObjectId() method used to set 3d model object id.
* Parameter -  string.
* Exception -
* Return -
*/
void PublishToPLMData::Set3DModelObjectId(string _3DModelObjectId)
{
	if (!FormatHelper::HasContent(_3DModelObjectId))
	{
		_3DModelObjectId = "";
	}
	m_3DModelObjectId = _3DModelObjectId;
}

/*
* Description - Get3DModelObjectId() method used to get 3d model object id.
* Parameter -
* Exception -
* Return - string.
*/
string PublishToPLMData::Get3DModelObjectId()
{
	return m_3DModelObjectId;
}

/*
* Description - SetActiveProductName() method used to set active product name.
* Parameter -
* Exception -
* Return -
*/
void PublishToPLMData::SetActiveProductName(string _productName)
{
	if (!FormatHelper::HasContent(_productName))
	{
		_productName = "";
	}
	m_ActiveProductName = _productName;
}

/*
* Description - GetActiveProductName() method used to get active product name.
* Parameter -
* Exception -
* Return - string.
*/
string PublishToPLMData::GetActiveProductName()
{
	return m_ActiveProductName;
}

/*
* Description - SetActiveProductStatus() method used to set active product status.
* Parameter -  string.
* Exception -
* Return -
*/
void PublishToPLMData::SetActiveProductStatus(string _productStatus)
{
	if (!FormatHelper::HasContent(_productStatus))
	{
		_productStatus = "";
	}
	m_ActiveProductStatus = _productStatus;
}

/*
* Description - GetActiveProductStatus() method used to get active product status.
* Parameter -
* Exception -
* Return - string.
*/
string PublishToPLMData::GetActiveProductStatus()
{
	return m_ActiveProductStatus;
}

/*
* Description - SetIsProductOverridden() method used to set is product overridden.
* Parameter -  bool.
* Exception -
* Return -
*/
void PublishToPLMData::SetIsProductOverridden(bool _flag)
{
	m_isProductOverridden = _flag;
}

/*
* Description - GetIsProductOverridden() method used to get is product overridden.
* Parameter -
* Exception -
* Return - bool.
*/
bool PublishToPLMData::GetIsProductOverridden()
{
	return m_isProductOverridden;
}

/*
* Description - SetAvailableResolutions() method used to set available resolutions.
* Parameter -
* Exception - exception, Char *
* Return -
*/
void PublishToPLMData::SetAvailableResolutions(string _key)
{
	Logger::Info("PublishToPLMData::SetAvailableResolutions() Started..");
	json array = json::array();
	m_availableResolutions.clear();
	try
	{
		array = Helper::GetJSONParsedValue<string>(GetDocumentConfigJSON(), _key, false);
		for (int i = 0; i < array.size(); i++)
		{
			m_availableResolutions.push_back(QString::fromStdString(Helper::GetJSONValue<int>(array, i, true)));
		}
	}
	catch (string msg)
	{
		Logger::Error("PublishToPLMData::SetAvailableResolutions() exception - " + msg);
		throw "Error in getting Resolutions options";
	}
	catch (exception& e)
	{
		Logger::Error("PublishToPLMData::SetAvailableResolutions() exception - " + string(e.what()));
		throw "Error in getting Resolutions options";
	}
	catch (const char* msg)
	{
		throw "Error in getting Resolutions options";
	}
	Logger::Info("PublishToPLMData::SetAvailableResolutions() Ended..");
}

/*
* Description - GetAvailableResolutions() method used to get available resolutions.
* Parameter -
* Exception -
* Return - QStringList.
*/
QStringList PublishToPLMData::GetAvailableResolutions()
{
	if (m_availableResolutions.empty())
		SetAvailableResolutions(RESOLUTION_OPTIONS_KEY);
	return m_availableResolutions;
}

/*
* Description - SetAvailableNoOfImagesOptions() method used to set available number of images options.
* Parameter -  string.
* Exception - exception, Char *
* Return -
*/
void PublishToPLMData::SetAvailableNoOfImagesOptions(string _key)
{
	Logger::Info("PublishToPLMData::SetAvailableNoOfImagesOptions() Started..");
	json array = json::array();
	m_availableNoOfImages.clear();
	try
	{
		array = Helper::GetJSONParsedValue<string>(GetDocumentConfigJSON(), _key, false);
		for (int i = 0; i < array.size(); i++)
		{
			m_availableNoOfImages.push_back(QString::fromStdString(Helper::GetJSONValue<int>(array, i, true)));
		}
	}
	catch (string msg)
	{
		Logger::Error("PublishToPLMData::SetAvailableNoOfImagesOptions() exception - " + msg);
		throw "Error in getting Resolutions options";
	}
	catch (exception& e)
	{
		Logger::Error("PublishToPLMData::SetAvailableNoOfImagesOptions() exception - " + string(e.what()));
		throw "Error in getting Resolutions options";
	}
	catch (const char* msg)
	{
		throw "Error in getting Resolutions options";
	}
	Logger::Info("PublishToPLMData::SetAvailableNoOfImagesOptions() Ended..");
}

/*
* Description - GetAvailableNoOfImagesOptions() method used to get available number of images options.
* Parameter -
* Exception -
* Return -
*/
QStringList PublishToPLMData::GetAvailableNoOfImagesOptions()
{
	if (m_availableNoOfImages.empty())
		SetAvailableNoOfImagesOptions(NOOFIMAGES_OPTIONS_KEY);
	return m_availableNoOfImages;
}

/*
* Description - DownloadProductMetadata() method used to download the selected items metadata from rest.
* Parameter -  string.
* Exception -
* Return -
*/
void PublishToPLMData::SetTurnTableImageResolution(string _key)
{
	m_turnTableImageResolution = Helper::GetJSONValue<string>(GetDocumentConfigJSON(), _key, true);
}

/*
* Description - GetTurnTableImageResolution() method used to get turn table image resolution.
* Parameter -
* Exception -
* Return - string.
*/
string PublishToPLMData::GetTurnTableImageResolution()
{
	if (!FormatHelper::HasContent(m_turnTableImageResolution))
		SetTurnTableImageResolution(TURNTABLE_IMAGE_RES_KEY);
	Logger::Debug("m_turnTableImageResolution = " + m_turnTableImageResolution);
	return m_turnTableImageResolution;
}

/*
* Description - SetTurnTableNoOfImages() method used to set turn table number of images.
* Parameter -  string.
* Exception -
* Return -
*/
void PublishToPLMData::SetTurnTableNoOfImages(string _key)
{
	m_turnTableNoOfImages = Helper::GetJSONValue<string>(GetDocumentConfigJSON(), _key, true);
}

/*
* Description - GetTurnTableNoOfImages() method used to get turn table number of images.
* Parameter -
* Exception -
* Return - string.
*/
string PublishToPLMData::GetTurnTableNoOfImages()
{
	if (!FormatHelper::HasContent(m_turnTableNoOfImages))
		SetTurnTableNoOfImages(NOOFTURNTABLE_IMAGES_KEY);
	Logger::Debug("m_turnTableNoOfImages = " + m_turnTableNoOfImages);
	return m_turnTableNoOfImages;
}

/*
* Description - SetTurnTableImageHeightAndWidth() method used to set turn table image height and width.
* Parameter -
* Exception -
* Return -
*/
void PublishToPLMData::SetTurnTableImageHeightAndWidth()
{
	QString resolution = QString::fromStdString(GetTurnTableImageResolution());
	QStringList hightAndWidth = resolution.split("x");
	m_turnTableImageHeight = hightAndWidth[0].toStdString();
	m_turnTableImageWidth = hightAndWidth[1].toStdString();
}

/*
* Description - GetTurnTableImageHeight() method used to get turn table image height.
* Parameter -
* Exception -
* Return - string.
*/
string PublishToPLMData::GetTurnTableImageHeight()
{
	if (!FormatHelper::HasContent(m_turnTableImageHeight))
		SetTurnTableImageHeightAndWidth();
	Logger::Debug("m_turnTableImageHeight = " + m_turnTableImageHeight);
	return m_turnTableImageHeight;
}

/*
* Description - GetTurnTableImageWidth() method used to get turn table image width.
* Parameter -
* Exception -
* Return - string.
*/
string PublishToPLMData::GetTurnTableImageWidth()
{
	if (!FormatHelper::HasContent(m_turnTableImageWidth))
		SetTurnTableImageHeightAndWidth();
	Logger::Debug("m_turnTableImageWidth = " + m_turnTableImageWidth);
	return m_turnTableImageWidth;
}

/*
* Description - SetSelectedNoOfImagePageImages() method used to set selected number of image page images.
* Parameter -  string, bool, bool.
* Exception - exception, Char *
* Return -
*/
void PublishToPLMData::SetSelectedNoOfImagePageImages(string _selected, bool _store, bool _setCached)
{
	Logger::Info("PLMSettingsSample::SetSelectedImagePage() Started..");

	json configJson = json::object();
	try
	{
		if (_setCached)
		{
			string resultsPerPage = Helper::GetJSONValue<string>(Configuration::GetInstance()->GetPLMPluginConfigJSON(), SELECTED_NOOFIMAGES_KEY, true);
			m_selectedNoOfImagePage = resultsPerPage;
		}
		if (FormatHelper::HasContent(_selected))
		{
			m_selectedNoOfImagePage = _selected;
			if (_store)
			{
				string configFilePath = DirectoryUtil::GetPLMPluginDirectory() + PLMCONFIG_FILE_NAME;
				configJson = Helper::ReadJSONFile(configFilePath);
				configJson[SELECTED_NOOFIMAGES_KEY] = _selected;
				Helper::WriteJSONFile(configFilePath, true, configJson);
			}
		}
	}
	catch (string  msg)
	{
		Logger::Error("Configuration::SetSelectedImagePage() exception - " + msg);
		UTILITY_API->DisplayMessageBox("Unable to Cache Selected Image page.");
	}
	catch (exception& e)
	{
		Logger::Error("Configuration::SetSelectedImagePage() exception - " + string(e.what()));
		UTILITY_API->DisplayMessageBox("Unable to Cache Selected Image page.");
	}
	catch (const char* msg)
	{
		UTILITY_API->DisplayMessageBox("Unable to Cache Selected Image page.");
	}
	Logger::Info("Configuration::SetSelectedImagePage() Ended..");
}

/*
* Description - GetSelectedNoOfImagePageImages() method used to get selected number of image page images.
* Parameter -
* Exception -
* Return - string.
*/
string PublishToPLMData::GetSelectedNoOfImagePageImages()
{
	return m_selectedNoOfImagePage;
}

/*
* Description - SetProductIdKeyExists() method used to set product id key exist or not.
* Parameter -  json.
* Exception - exception, Char *
* Return -
*/
void PublishToPLMData::SetProductIdKeyExists(json _json)
{
	Logger::Info("PublishToPLMData -> SetProductIdKeyExists() Start");
	try
	{
		m_productIdKeyExists = false;
		string productIdKeyExists = Helper::GetJSONValue<string>(_json, PRODUCT_ID_EXIST_KEY, true);
		if (FormatHelper::HasContent(productIdKeyExists))
		{
			if (productIdKeyExists == "true")
			{
				m_productIdKeyExists = true;
			}
		}
	}
	catch (string msg)
	{
		Logger::Error("PublishToPLMData -> SetProductIdKeyExists Exception :: " + msg);
		throw msg;
	}
	catch (exception& e)
	{
		Logger::Error("PublishToPLMData -> SetProductIdKeyExists Exception :: " + string(e.what()));
		throw e;
	}
	catch (const char* msg)
	{
		Logger::Error("PublishToPLMData -> SetProductIdKeyExists Exception :: " + string(msg));
		throw msg;
	}
	Logger::Info("PublishToPLMData -> SetProductIdKeyExists() End");
}

/*
* Description - GetProductIdKeyExists() method used to get product id key exist or not.
* Parameter -
* Exception -
* Return - bool.
*/
bool PublishToPLMData::GetProductIdKeyExists()
{
	Logger::Info("PublishToPLMData -> GetProductIdKeyExists() Start");
	return m_productIdKeyExists;
	Logger::Info("PublishToPLMData -> GetProductIdKeyExists() End");
}

/*
* Description - SetDateFlag() method used to set date exist flag.
* Parameter -  bool.
* Exception -
* Return -
*/
void PublishToPLMData::SetDateFlag(bool _isDateEditPresent)
{
	m_isDateEditPresent = _isDateEditPresent;
}

/*
* Description - GetDateFlag() method used to get date exist flag.
* Parameter -
* Exception -
* Return - bool.
*/
bool PublishToPLMData::GetDateFlag()
{
	return m_isDateEditPresent;
}

/*
* Description - SetMaxUploadFileSize() method used to set maximum upload file size.
* Parameter -
* Exception -
* Return -
*/
void PublishToPLMData::SetMaxUploadFileSize(string _key)
{
	m_maxZPRJUploadFileSize = stoi(Helper::GetJSONValue<string>(GetDocumentConfigJSON(), _key, true));
}

/*
* Description - GetMaxUploadFileSize() method used to get maximum upload file size.
* Parameter -
* Exception -
* Return - int.
*/
int PublishToPLMData::GetMaxUploadFileSize()
{
	if (!FormatHelper::HasContent(to_string(m_maxZPRJUploadFileSize)))
		SetMaxUploadFileSize(MAX_ZPRJ_UPLOAD_FILE_SIZE);
	return m_maxZPRJUploadFileSize;
}

/*
* Description - GetIsModelExecuted() return a flag wether Rest call is being made and module is already executed, used for progress bar to work correctly
* Parameter -
* Exception -
* Return - bool
*/
bool PublishToPLMData::GetIsModelExecuted()
{
	return m_isModelExecuted;
}

/*
* Description - SetIsModelExecuted() sets a flag wether Rest call is being made and module is already executed, used for progress bar to work correctly
* Parameter -
* Exception -
* Return -
*/
void PublishToPLMData::SetIsModelExecuted(bool _isModelExecuted)
{
	m_isModelExecuted = _isModelExecuted;
}


json PublishToPLMData::GetUpdateStyleCacheData()
{
	return m_updateStyleData;
}

void PublishToPLMData::SetUpdateStyleCacheData(json _upsateStyleJson)
{
	m_updateStyleData = _upsateStyleJson;
}

string PublishToPLMData::GetLatestRevision()
{
	return m_latestDocRevisionId;
}

void PublishToPLMData::SetLatestRevision(string _latestRevisionId)
{
	m_latestDocRevisionId = _latestRevisionId;
}
void PublishToPLMData::SetIsCreateNewDocument(bool _isNewDocument)
{
	m_createNewDocument = _isNewDocument;
}
bool PublishToPLMData::GetIsCreateNewDocument()
{
	return m_createNewDocument;
}

/*
* Description - ResetPublishData() method is reset the publish data.
* Parameter -
* Exception -
* Return -
*/
void PublishToPLMData::ResetPublishData()
{
	SetActiveProductId(BLANK);
	SetActiveProductObjectId(BLANK);
	SetActiveProductName(BLANK);
	SetActiveProductStatus(BLANK);
	SetIsProductOverridden(false);
	SetActive3DModelMetaData(json::object());
	Set3DModelObjectId(BLANK);
	SetDateFlag(false);
	SetIsCreateNewDocument(false);
	SetUpdateStyleCacheData(json::object());
	SetLatestRevision(BLANK);
	SetIsModelExecuted(false);
	m_documentFieldsJson = nullptr;
	m_isSaveClicked = false;
}
