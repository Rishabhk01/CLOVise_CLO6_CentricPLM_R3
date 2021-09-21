/*
* Copyright 2020-2021 CLO-Vise. All rights reserved
*
* @file DirectoryUtil.cpp
*
* @brief Class implementation for creating directory CLO Assert path.
* This class has all the variable and function implementation which are used creating directory CLO Assert path.
*
* @author GoVise
*
* @date 27-MAY-2020
*/
#include "DirectoryUtil.h"
#include "qdir.h"

/*
* Description - createDirectories() method used to create directories.
* Parameter - 
* Exception -
* Return - 
*/
void DirectoryUtil::CreateDirectories()
{
    QDir dir;
	string sample_assets = UTILITY_API->GetCLOAssetFolderPath(true) + "/CLOPLMPlugin_Assets/";
	dir.mkpath(QString::fromStdString(sample_assets));
	string plm_folder = sample_assets + "plm/";
	dir.mkpath(QString::fromStdString(plm_folder));
	string plm_plugin = sample_assets + "/config/";
	dir.mkpath(QString::fromStdString(plm_plugin));
	string logs = sample_assets + "/logs/";
	dir.mkpath(QString::fromStdString(logs));
	string product_dir = plm_folder + "product/";
	dir.mkpath(QString::fromStdString(product_dir));
	string material_dir = plm_folder + + "material/";
	dir.mkpath(QString::fromStdString(material_dir));
	string trim_dir = plm_folder + + "trim/";
	dir.mkpath(QString::fromStdString(trim_dir));
	string color_dir = plm_folder + + "color/";
	dir.mkpath(QString::fromStdString(color_dir));
}

/*
* Description - getSampleAssetsDirectory() method used to get sample assets directory.
* Parameter - 
* Exception -
* Return - string.
*/
const string DirectoryUtil::GetSampleAssetsDirectory() {
	return UTILITY_API->GetCLOAssetFolderPath(true) + "CLOPLMPlugin_Assets/";
}

/*
* Description - getSampleStorageDirectory() method used to get sample storage directory.
* Parameter - 
* Exception -
* Return - string.
*/
const string DirectoryUtil::GetSampleStorageDirectory() {
	return GetSampleAssetsDirectory() + "plm/";
}

/*
* Description - getMaterialAssetsDirectory() method used to get material assets directory.
* Parameter - 
* Exception -
* Return - string.
*/
const string DirectoryUtil::GetMaterialAssetsDirectory() {
	return GetSampleStorageDirectory() + "material/";
}

/*
* Description - getStyleAttachmentDirectory() method used to get style attachment directory.
* Parameter - 
* Exception -
* Return - string.
*/
const string DirectoryUtil::GetStyleAttachmentDirectory() {
	return GetSampleStorageDirectory() + "product/";
}

/*
* Description - getTrimAssetsDirectory() method used to get trim assets directory.
* Parameter - 
* Exception -
* Return - string.
*/
const string DirectoryUtil::GetTrimAssetsDirectory() {
	return GetSampleStorageDirectory() + "trim/";
}

/*
* Description - getColorsAssetsDirectory() method used to get colors assets directory.
* Parameter - 
* Exception -
* Return - string.
*/
const string DirectoryUtil::GetColorsAssetsDirectory() {
	return GetSampleStorageDirectory() + "color/";
}

/*
* Description - getPLMPluginDirectory() method used to get plm plug-in directory.
* Parameter - 
* Exception -
* Return - string.
*/
const string DirectoryUtil::GetPLMPluginDirectory() {
	return GetSampleAssetsDirectory() + "/config/";
}

/*
* Description - getLogFilePath() method used to get log file path.
* Parameter - 
* Exception -
* Return - string.
*/
const string DirectoryUtil::GetLogFilePath() {
	return GetSampleAssetsDirectory() + "/logs/";
}

/*
* Description - getCLOPLMPluginConfigJson() method used to get CLO plm plug-in configuration json file path.
* Parameter -
* Exception -
* Return - string.
*/
const string DirectoryUtil::GetCLOPLMPluginConfigJson() {
	return GetPLMPluginDirectory() + "cloplmpluginconfig.json";
}