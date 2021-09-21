#pragma once
/*
* Copyright 2020-2021 CLO-Vise. All rights reserved
*
* @file DataUtility.h
*
* @brief Class declaration for set the requested data CLO from PLM.
* This class has all the variable and function declaration which are used set the requested data CLO from PLM.
*
* @author GoVise
*
* @date 27-MAY-2020
*/
#include <string>
#include <map>

#include "include/CloApiData.h"

using namespace std;

class DataUtility
{

public:
	map<string, string> GetPLMColorIDtoColorNameList();
	map<string, Marvelous::CloApiRgb> GetPLMIDtoColorList();
	map<string, string> GetPLMIDtoApiMetaDataList();

	static DataUtility* GetInstance();
	void Destroy();

	string GetProgressBarMsg();
	float GetProgressCounter();
	

	void SetProgressBarMsg(string _message);
	void SetProgressCounter(float _count);
	void SetPLMColorIDtoColorNameList(map<string, string>& _plmColorIDtoColorNameList);
	void SetPLMIDtoColorList(map<string, Marvelous::CloApiRgb>& _plmIDtoColorList);
	void SetPLMIDtoApiMetaDataList(map<string, string>& _plmIDtoApiMetaDataList);
	
private:
	static DataUtility* _instance;
	string m_ProgressBarMsg;
	float m_ProgressCounter;
	map<string, string> m_PLMColorIDtoColorNameList;
	map<string, Marvelous::CloApiRgb> m_PLMIDtoColorList;
	map<string, string> m_PLMIDtoApiMetaDataList;

};

