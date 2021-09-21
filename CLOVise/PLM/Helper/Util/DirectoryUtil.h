#pragma once
/*
* Copyright 2020-2021 CLO-Vise. All rights reserved
*
* @file DirectoryUtil.h
*
* @brief Class declaration for creating directory CLO Assert path.
* This class has all the variable and function declaration which are used creating directory CLO Assert path.
*
* @author GoVise
*
* @date 27-MAY-2020
*/
#include <string>

#include <CLOAPIInterface.h>

using namespace std;

class DirectoryUtil
{
public:
	static void CreateDirectories();
	static const string GetSampleAssetsDirectory();
	static const string GetSampleStorageDirectory();
	static const string GetMaterialAssetsDirectory();
	static const string GetStyleAttachmentDirectory();
	static const string GetTrimAssetsDirectory();
	static const string GetColorsAssetsDirectory();
	static const string GetPLMPluginDirectory();
	static const string GetLogFilePath();
	static const string GetCLOPLMPluginConfigJson();
};

