#pragma once

/*
* Copyright 2020-2021 CLO-Vise. All rights reserved
*
* @file Definitions.h
*
* @brief Class defining variable.
* This class has all the variable and function declaration which are used defining variable.
*
* @author GoVise
*
* @date 27-MAY-2020
*/

#include <string>

#include <QString>
#include <QVariant>
#include <QColor>

using namespace std;

const string SERVER_DOWN_MSG = "The system is unable to process your request at this time";
const string SERVER_DOWN_RESPOSE = "Error in processing the request. Please try again later. If the problem persists contact your system administrator.";
const string RESTARTCLO_MSG = "You need to re-login to CLO-Vise for changes to take effect.";
const string PLMSETTINGS_EXCEPTION_MSG = "Something went wrong! Please verify PLM Settings/File. If problem still persists, please contact your System Adminastrator.";

const string NETWORK_ERROR_MESSAGE = "Please check your Network connection and try again";
const string TIMEOUT_ERROR_MESSAGE = "Request aborted after target server failed to respond within configured timeout";

const string LICENSE_NOT_VALID_MSG = "You are not authorized to access CLO-Vise. Please contact your system administrator.";
const string USER_ALREADY_ACTIVE = "User is already active. Please log out of the CLO-Vise on other devices.";
const string LICENSE_EXPIRED = "The license expiration date has passed.";
const string LICENSE_LIMIT_EXCEEDED = "License limit exceeded. Please log out of the CLO-Vise on other devices. If license is currently not active on other devices, please try again in ten minutes. If you continue to have issues or cannot log out of other devices, please contact The CLO Support Team.";
const string LICENSE_ERROR_CODE_101_MSG = "Company name is missing. Please verify PLM settings or contact your system administrator.";
const string LICENSE_ERROR_CODE_102_MSG = "Company Api Key is missing. Please verify PLM settings or contact your system administrator.";
const string LOGOUT_UNSUCCESSFUL_MSG = "You don't have active session.";
const string LICENSE_LOGOUT_MSG = "User logged out successfully.";

const string NO_3D_SAMPLE_MSG = "No active 3D Sample to download Product.";
const string NO_3D_SAMPLE_TO_SUBMIT_MSG = "No active 3D Sample to submit to PLM.";
const string NO_ACTIVE_SAMPLE_MSG = "No active Sample.Please download a sample from PLM.";
const string NO_ACTIVE_3D_GARMENT_PRODUCT_MSG = "No active 3D Garment to search Samples.";
const string NO_ACTIVE_3D_GARMENT_PUBLISH_MSG = "No active 3D Garment to publish to PLM.";
const string NO_PRODUCT_EXIST_MSG = "No sample exists. Please download a sample from PLM.";
const string NO_ACTIVE_PRODUCT_MSG = "No active sample. Please download a sample from PLM.";
const string NOT_FOUND_MSG = "Can't find Resource";