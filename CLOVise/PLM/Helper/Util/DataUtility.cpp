/*
* Copyright 2020-2021 CLO-Vise. All rights reserved
*
* @file DataUtility.cpp
*
* @brief Class implementation for set the requested data CLO from PLM.
* This class has all the variable and function implementation which are used set the requested data CLO from PLM.
*
* @author GoVise
*
* @date 27-MAY-2020
*/
#include "DataUtility.h"

DataUtility* DataUtility::_instance = NULL;
DataUtility* DataUtility::GetInstance()
{
	if (_instance == NULL)
	{
		_instance = new DataUtility();
	}
	return _instance;
}

void  DataUtility::Destroy()
{
	if (_instance)
	{
		delete _instance;
		_instance = NULL;
	}
}

string DataUtility::GetProgressBarMsg()
{
	return m_ProgressBarMsg;
}

void DataUtility::SetProgressBarMsg(string _message)
{
	m_ProgressBarMsg = _message;
}

float DataUtility::GetProgressCounter()
{
	return m_ProgressCounter;
}

void DataUtility::SetProgressCounter(float _count)
{
	m_ProgressCounter = _count;
}

map<string, string> DataUtility::GetPLMColorIDtoColorNameList()
{
	return m_PLMColorIDtoColorNameList;
}

void DataUtility::SetPLMColorIDtoColorNameList(map<string, string>& _plmColorIDtoColorNameList)
{
	m_PLMColorIDtoColorNameList = _plmColorIDtoColorNameList;
}

map<string, Marvelous::CloApiRgb> DataUtility::GetPLMIDtoColorList()
{
	return m_PLMIDtoColorList;
}

void DataUtility::SetPLMIDtoColorList(map<string, Marvelous::CloApiRgb>& _plmIDtoColorList)
{
	m_PLMIDtoColorList = _plmIDtoColorList;
}

map<string, string> DataUtility::GetPLMIDtoApiMetaDataList()
{
	return m_PLMIDtoApiMetaDataList;
}

void DataUtility::SetPLMIDtoApiMetaDataList(map<string, string>& _plmIDtoApiMetaDataList)
{
	m_PLMIDtoApiMetaDataList = _plmIDtoApiMetaDataList;
}
