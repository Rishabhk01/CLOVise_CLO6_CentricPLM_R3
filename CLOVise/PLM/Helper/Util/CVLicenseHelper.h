#pragma once
/*
* Copyright 2020-2021 CLO-Vise. All rights reserved
*
* @file CVLicenseHelper.h
*
* @brief Class declaration for verify the user license CLO from PLM.
* This class has all the variable and function declaration which are used verify the user license CLO from PLM.
*
* @author GoVise
*
* @date 27-MAY-2020
*/
#include <string>

#include "CLOVise/PLM/Libraries/json.h"

using namespace std;
using json = nlohmann::json;

class CVLicenseHelper
{
	public:
		static CVLicenseHelper* GetInstance();
		static void	Destroy();

		bool ValidateCVLicense(string _userId);
		bool ValidateCVLicenseLogOut(string _userId);
		json ValidateCVUserLicense(const string& _url, const string& _userId);

private:
	static CVLicenseHelper* _instance; // zero initialized by default

};