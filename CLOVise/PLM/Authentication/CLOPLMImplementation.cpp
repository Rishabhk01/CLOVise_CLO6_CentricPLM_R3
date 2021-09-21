/*
* Copyright 2020-2021 CLO-Vise. All rights reserved
*
* @file CLOPLMImplemntation.cpp
*
* @brief Class implementation for set the view/search mode in CLO to access PLM.
* This class has all the variable and methods implementation which are used access the PLM in CLO.
* Signin GUI calling untill validating the user name and password is correct.
* The basic search library will popup.
*
* @author GoVise
*
* @date 25-MAY-2020
*/
#include "CLOPLMImplementation.h"

#include "CLOAPIInterface/CLOAPIInterface.h"
#include "LibraryWindowImplPlugin.h"
#include "CLOPLMSignIn.h"
#include "CLOVise/PLM/CLOViseSuite/CLOViseSuite.h"
#include "CLOVise/PLM/Helper/UIHelper/CVDisplayMessageBox.h"
#include "CLOVise/PLM/Helper/Util/Helper.h"

namespace CLOVise
{
	CLOPLMImplementation::CLOPLMImplementation(QWidget* parent, Qt::WindowFlags flags, SEARCH_ITEM_MODE mode)
		: QDialog(parent, flags)
	{
		searchMode = mode;
	}

	CLOPLMImplementation::~CLOPLMImplementation()
	{

	}

	void CLOPLMImplementation::ConnectToCLOVise(SEARCH_ITEM_MODE _mode, bool _fromSignin)
	{
		SetSearchMode(_mode, _fromSignin);
	}
	/*
	* Description - SetSearchMode() method to set the search mode and showing the design suite.
	* Parameter -  enum, bool.
	* Exception -
	* Return -
	*/
	void CLOPLMImplementation::SetSearchMode(SEARCH_ITEM_MODE _mode, bool _fromSignin)
	{
		Logger::Logger("connecting to CLOVise suit 4....");

		const string plmId, plmPw;
		string token;
		searchMode = _mode;
		Logger::Logger("connecting to CLOVise suit 5....");

		if (searchMode == PLM_SEARCH)
		{
			Logger::Logger("connecting to CLOVise suit 6....");

			if (_fromSignin)
			{
				Logger::Logger("connecting to CLOVise suit 7....");

				if (SigninPLM() && !Configuration::GetInstance()->GetPLMSignin())
				{
					CVDisplayMessageBox DownloadDialogObject;
					DownloadDialogObject.DisplyMessage("Login successful.");
					DownloadDialogObject.setModal(true);
					Logger::Logger("connecting to CLOVise suit 8....");

					DownloadDialogObject.exec();
					Logger::Logger("connecting to CLOVise suit 9....");

					Configuration::GetInstance()->SetPLMSignin(true);
					Logger::Logger("connecting to CLOVise suit 10....");
				}
			}
			else
			{
				Logger::Logger("connecting to CLOVise suit 11....");

				if(!Configuration::GetInstance()->GetPLMSignin())
				{
					Logger::Logger("connecting to CLOVise suit 12....");

					UTILITY_API->DisplayMessageBox("Please login to CLO-Vise.");
				}
				else if (SigninPLM())
				{
					Logger::Logger("connecting to CLOVise suit 13....");
					CLOViseSuite::GetInstance()->setModal(false);
					Logger::Logger("connecting to CLOVise suit 14....");
					CLOViseSuite::GetInstance()->open();
					Logger::Logger("connecting to CLOVise suit 15....");
				}
			}
		}
		Logger::Logger("connecting to CLOVise suit 5....");
	}

	/*
	* Description - SigninPLM() method used to validate the user input. through accessing PLMSignin instance methods.
	* Parameter - 
	* Exception -
	* Return - bool
	*/
	bool CLOPLMImplementation::SigninPLM()
	{

		if (PLMSignin::GetInstance()->GetSignInPLM())
			return true;

		CLOPLMSignIn::GetInstance()->exec(); // make this a singleton and get instance


		if (CLOPLMSignIn::GetInstance()->result() == QDialog::Accepted)
		{
			QString getId = CLOPLMSignIn::GetInstance()->GetID();
			QString getPasswd = CLOPLMSignIn::GetInstance()->GetPasswd();
			PLMSignin::GetInstance()->SetSignInPLM(true);
		}
		else
		{
			PLMSignin::GetInstance()->SetSignInPLM(false);
		}

		return PLMSignin::GetInstance()->GetSignInPLM();
	}
}