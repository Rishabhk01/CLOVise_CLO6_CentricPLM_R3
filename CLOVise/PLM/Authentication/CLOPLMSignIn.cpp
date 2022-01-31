/*
* Copyright 2020-2021 CLO-Vise. All rights reserved
*
* @file CLOPLMSignIn.cpp
*
* @brief Class implementation for validate the user in CLO from PLM.
* This class has all the variable and methods implementation which are used to validate the user.
*
* @author GoVise
*
* @date 25-MAY-2020
*/
#include "CLOPLMSignIn.h"

#include <QDebug>
#include <QMouseEvent>
#include <QPushButton>
#include "classes/APIDefine.h"
#include "CLOAPIInterface.h"
#include "../Helper/Util/Helper.h"
#include "../Helper/Util/Logger.h"
#include "../Helper/Util/Configuration.h"
#include <CLOVise/PLM/Helper/Util/RestAPIUtility.h>
#include "CLOVise/PLM/Helper/Util/DirectoryUtil.h"
#include "CLOVise/PLM/Helper/Util/CVLicenseHelper.h"
#include "CLOVise/PLM/Helper/UIHelper/CVWidgetGenerator.h"
#include "CLOVise/PLM/Helper/UIHelper/UIHelper.h"

namespace CLOVise
{
	PLMSignin* PLMSignin::_instance = NULL;

	PLMSignin* PLMSignin::GetInstance()
	{
		if (_instance == NULL) {
			_instance = new PLMSignin();
		}

		return _instance;
	}

	void PLMSignin::Destroy()
	{
		if (_instance) {
			delete _instance;
			_instance = NULL;
		}
	}

	void PLMSignin::SetSignInPLM(bool b)
	{
		isSignInPLMEnabled = b;
	}

	bool PLMSignin::GetSignInPLM()
	{
		return isSignInPLMEnabled;
	}

	CLOPLMSignIn* CLOPLMSignIn::_instance = NULL;

	CLOPLMSignIn* CLOPLMSignIn::GetInstance()
	{
		if (_instance == NULL) {
			_instance = new CLOPLMSignIn();
		}

		return _instance;
	}

	void CLOPLMSignIn::Destroy()
	{
		if (_instance) {
			delete _instance;
			_instance = NULL;
		}
	}

	CLOPLMSignIn::CLOPLMSignIn(QWidget* parent)
		: MVDialog(parent)
	{
		Logger::Info("CLOPLMSignIn -> CLOPLMSignIn() -> Start");
		setupUi(this);
		try
		{

#ifdef __APPLE__
			this->setWindowFlags(Qt::Dialog | Qt::CustomizeWindowHint | Qt::WindowMinMaxButtonsHint | Qt::WindowCloseButtonHint);
#else
			this->setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint | Qt::CustomizeWindowHint);
			m_pTitleBar = new MVTitleBar("Centric 8 PLM Login", this);
			layout()->setMenuBar(m_pTitleBar);
#endif // !__APPLE__

			this->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
			this->setSizeGripEnabled(false);

			SetPlmLogo(ui_CloViseLogoLabel, CLOVISE_LOGO_WHITE_PATH/*, 150,25*/);
			SetPlmLogo(ui_PlmLogoLabel, ":/CLOVise/PLM/Images/img_bi_white_centricPLM.svg"/*, 150,20*/);
			SetPlmLogo(ui_PoweredByLogoLabel, POWERED_BY_IMAGE_WHITE_PATH/*,150,35*/);

			QSpacerItem *horizontalSpacer_2 = new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Fixed);
			QSpacerItem *horizontalSpacer_3 = new QSpacerItem(10, 20, QSizePolicy::Fixed, QSizePolicy::Fixed);
			QSpacerItem *horizontalSpacer_4 = new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Fixed);

			m_loginButton = CVWidgetGenerator::CreatePushButton("Login", OK_HOVER_ICON_PATH, "Login", PUSH_BUTTON_STYLE, 30, true);
			m_cancelButton = CVWidgetGenerator::CreatePushButton("Cancel", CANCEL_HOVER_ICON_PATH, "Cancel", PUSH_BUTTON_STYLE, 30, true);

			ui_horizontalLayout->insertSpacerItem(0, horizontalSpacer_2);
			ui_horizontalLayout->insertWidget(1, m_loginButton);
			ui_horizontalLayout->insertSpacerItem(2, horizontalSpacer_3);
			ui_horizontalLayout->insertWidget(3, m_cancelButton);
			ui_horizontalLayout->insertSpacerItem(4, horizontalSpacer_4);

			QLabel  *userNameLabel = CVWidgetGenerator::CreateLabel("User Name", "", HEADER_STYLE, true);
			userNameLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
			QLabel  *passwordLabel = CVWidgetGenerator::CreateLabel("Password", "", HEADER_STYLE, true);
			passwordLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
			m_userNameLineEdit = CVWidgetGenerator::CreateLineEdit("", HEADER_STYLE, true);
			m_userNameLineEdit->setAttribute(Qt::WA_MacShowFocusRect, false);
			m_userNameLineEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

			m_passwordLineEdit = CVWidgetGenerator::CreateLineEdit("", HEADER_STYLE, true);
			m_passwordLineEdit->setAttribute(Qt::WA_MacShowFocusRect, false);
			m_passwordLineEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
			m_passwordLineEdit->setEchoMode(QLineEdit::Password);

			ui_gridLayout->addWidget(userNameLabel, 0, 0);
			ui_gridLayout->addWidget(passwordLabel, 1, 0);
			ui_gridLayout->addWidget(m_userNameLineEdit, 0, 1);
			ui_gridLayout->addWidget(m_passwordLineEdit, 1, 1);
			this->setWindowTitle("Centric 8 PLM Login");

			if (Configuration::GetInstance()->GetIsReadSavedCredential())
				ReadJsonFile();
			else
			{
				m_userNameLineEdit->setText(QString::fromStdString(Configuration::GetInstance()->GetInputUserName()));
				m_passwordLineEdit->setText(QString::fromStdString(Configuration::GetInstance()->GetInputUserPassword()));
			}
			ConnectSignalSlots();
		}
		catch (string msg)
		{
			throw string(msg);
		}
		catch (exception& e)
		{
			Destroy();
			throw e;
		}
		catch (const char* msg)
		{
			Destroy();
			throw msg;
		}
		Logger::Info("CLOPLMSignIn -> CLOPLMSignIn() -> End");
	}

	CLOPLMSignIn::~CLOPLMSignIn()
	{
		DisconnectSignalSlots();
	}

	void CLOPLMSignIn::SetPlmLogo(QLabel* _uiLogoLabel, QString _iconPath)
	{
		_uiLogoLabel->setStyleSheet("#" + _uiLogoLabel->objectName() + " { image:url(" + _iconPath + "); }");
		//_uiLogoLabel->setStyleSheet("#" + _uiLogoLabel->objectName() + " { border-image:url(" + _iconPath + "); width: " + _width + "px; height: " + _height + "px; }");
	}
	/*
	*Description - GetID() used to return the stored user id.
	* Parameter -
	* Exception -
	* Return - Qstring.
	*/
	const QString& CLOPLMSignIn::GetID()
	{
		return m_plmLoginId;
	}

	/*
	* Description - GetPasswd() used to return the user password.
	* Parameter -
	* Exception -
	* Return - Qstring.
	*/
	const QString& CLOPLMSignIn::GetPasswd()
	{
		return m_plmLoginPw;
	}

	/*
	* Description - ReadJsonFile() method used to read user id and user password.
	*	 this user id and passwor is belongs to previous session.
	* Parameter -
	* Exception -
	* Return -
	*/
	void CLOPLMSignIn::ReadJsonFile()
	{
		string line, user, password;
		ifstream readData(DirectoryUtil::GetPLMPluginDirectory() + ENCRYPT_DATA_FILE_NAME);
		int fileSize = Helper::GetFileSize(DirectoryUtil::GetPLMPluginDirectory() + ENCRYPT_DATA_FILE_NAME);
		if (fileSize > 0)
		{
			while (getline(readData, line))
			{
				if (user.length() == 0)
				{
					user = line;
					m_userNameLineEdit->setText(QString::fromStdString(Helper::DecryptEncrypt(user)));
				}
				else
				{
					password = line;
					m_passwordLineEdit->setText(QString::fromStdString(Helper::DecryptEncrypt(password)));
				}
			}
			readData.close();
		}
	}
	/*
	* Description - ValidateLoginCredentials() method used to get the user input and validate.
	* Parameter -
	* Exception - exception, Char *
	* Return -
	*/
	void CLOPLMSignIn::onLoginClicked()
	{
		ValidateLoginCredentials();
	}
	void CLOPLMSignIn::ValidateLoginCredentials()
	{
		auto startTime = std::chrono::high_resolution_clock::now();
		m_plmLoginId = m_userNameLineEdit->text();
		m_plmLoginPw = m_passwordLineEdit->text();

		string userName = m_plmLoginId.toStdString();
		string password = m_plmLoginPw.toStdString();

		Configuration::GetInstance()->SetInputUserName(userName);
		Configuration::GetInstance()->SetInputUserPassword(password);
		Configuration::GetInstance()->SetIsReadSavedCredential(false);

		try
		{
			if (!FormatHelper::HasContent(userName))
			{
				throw "Please enter Username.";
			}
			if (!FormatHelper::HasContent(password))
			{
				throw "Please enter Password.";
			}
			ConnectToPLM(userName, password);
			auto finishTime = std::chrono::high_resolution_clock::now();
			std::chrono::duration<double> totalDuration = finishTime - startTime;
			Logger::perfomance(PERFOMANCE_KEY + "Time for Login :: " + to_string(totalDuration.count()));
		}
		catch (string msg)
		{
			Logger::Error("PLMSignIn -> ValidateLoginCredentials Exception :: " + string(msg));
			UTILITY_API->DisplayMessageBox(msg);
			CLOPLMSignIn::Destroy();
			CLOPLMSignIn::GetInstance()->exec();
		}
		catch (exception& e)
		{
			Logger::Error("PLMSignIn -> ValidateLoginCredentials Exception :: " + string(e.what()));
			UTILITY_API->DisplayMessageBox(e.what());
			CLOPLMSignIn::Destroy();
			CLOPLMSignIn::GetInstance()->exec();
		}
		catch (const char* msg)
		{
			wstring wstr(msg, msg + strlen(msg));
			Logger::Error("PLMSignIn -> ValidateLoginCredentials Exception :: " + string(msg));
			UTILITY_API->DisplayMessageBoxW(wstr);
			CLOPLMSignIn::Destroy();
			CLOPLMSignIn::GetInstance()->exec();
		}
	}

	void CLOPLMSignIn::ConnectToPLM(string _userName, string _password)
	{
		/*GetUserDetails();
		GetUserPermissions();
		GetConfigProperties();*/


		try
		{
			string basicauthorizationstring = "basic " + Helper::ConvertToBase64(_userName + ":" + _password);
			vector<pair<string, string>> headernameandvalue;
			headernameandvalue.push_back(make_pair(CONTENTTYPE, APPLICATION_JSON_TYPE));
			headernameandvalue.push_back(make_pair("Accept", APPLICATION_JSON_TYPE));
			std::string jsonstring = "{ \"username\":\"" + _userName + "\",\"password\":\"" + _password + "\"}";
			string serverURL = Configuration::GetInstance()->GetPLMServerURL();

			this->hide();

			UTILITY_API->CreateProgressBar();
			UTILITY_API->SetProgress("Loading", (qrand() % 101));
			string basicTokenResponse = "";
			bool validToken = false;
			json responseTokenJson;
			auto startTime = std::chrono::high_resolution_clock::now();
			string response = RESTAPI::PostRestCall(jsonstring, Configuration::GetInstance()->GetPLMServerURL() + RESTAPI::AUTHORIZATION_URL, "content-type: application/json; boundary=----WebKitFormBoundary7MA4YWxkTrZu0gW");
			//UTILITY_API->DisplayMessageBox("response::" + response);
			auto finishTime = std::chrono::high_resolution_clock::now();
			std::chrono::duration<double> totalDuration = finishTime - startTime;
			Logger::perfomance(PERFOMANCE_KEY + "Time for Login API :: " + to_string(totalDuration.count()));
			Logger::RestAPIDebug("response :: " + response);
			const char* restErrorMsgg = RESTAPI::CheckForRestErrorMsg(response);
			if (!FormatHelper::HasContent(response))
			{
				throw "Unable to login. Please try again or Contact your System Administrator.";
			}	
			else if (FormatHelper::HasError(response))
			{
				throw response;
			}
			else
			{

				if (!CVLicenseHelper::GetInstance()->ValidateCVLicense(_userName))
				{
					UTILITY_API->DeleteProgressBar(true);
				}
				else
				{
					int length = response.length();
					int indexforjson = response.find("token");
					basicTokenResponse = response.substr(indexforjson - 2, length);
					if (FormatHelper::HasContent(basicTokenResponse))
						responseTokenJson = json::parse(basicTokenResponse);
					//UTILITY_API->DisplayMessageBox("responseTokenJson:: " + to_string(responseTokenJson));
					string tokenValue = Helper::GetJSONValue<string>(responseTokenJson, "token", true);
					Configuration::GetInstance()->SetBearerToken(tokenValue);
					Logger::Debug("tokenValue::" + tokenValue);
					Configuration::GetInstance()->SetConnectionStatus("Connected");
					Configuration::GetInstance()->SetPLMVersion(UIHelper::getPLMVersion());
					UIHelper::GetLocalizedClassNames();
					Helper::EncryptDecrypt(_userName, _password);
					string userInfoResponse = RESTAPI::CentricRestCallGet(Configuration::GetInstance()->GetPLMServerURL() + RESTAPI::USER_SESSION_DETAIL_API + "?decode=true", APPLICATION_JSON_TYPE, "");
					if (!FormatHelper::HasError(userInfoResponse))
					{
						Configuration::GetInstance()->SetLoggedInUserName(json::parse(userInfoResponse));
					}
					else
						throw runtime_error(userInfoResponse);
					accept();
					json jsonvalueArray = json::object();
					string jsonValues = DirectoryUtil::GetPLMPluginDirectory() + "Client_Specific_attributes.json";//Reading client specific json
					jsonvalueArray = Helper::ReadJSONFile(jsonValues);
					Configuration::GetInstance()->SetClientSpecificJson(jsonvalueArray);
					Configuration::GetInstance()->SetSubTypeInternalName(jsonvalueArray);

					UTILITY_API->DeleteProgressBar(false);
				}
			}
			/*else
			{
				throw "Unable to login. Please verify login credentials.";
			}*/
		}
		catch (string msg)
		{
			UTILITY_API->DeleteProgressBar(false);
			Logger::Error("PLMSignIn -> ValidateLoginCredentials Exception :: " + msg);
			UTILITY_API->DisplayMessageBox(msg);
			CLOPLMSignIn::Destroy();
			CLOPLMSignIn::GetInstance()->exec();
		}
		catch (exception& e)
		{
			UTILITY_API->DeleteProgressBar(false);
			Logger::Error("PLMSignIn -> ValidateLoginCredentials Exception :: " + string(e.what()));
			UTILITY_API->DisplayMessageBox(e.what());
			CLOPLMSignIn::Destroy();
			CLOPLMSignIn::GetInstance()->exec();
		}
		catch (const char* msg)
		{
			wstring wstr(msg, msg + strlen(msg));
			UTILITY_API->DeleteProgressBar(false);
			Logger::Error("PLMSignIn -> ValidateLoginCredentials Exception :: " + string(msg));
			UTILITY_API->DisplayMessageBoxW(wstr);
			CLOPLMSignIn::Destroy();
			CLOPLMSignIn::GetInstance()->exec();
		}
	}

	void CLOPLMSignIn::GetUserDetails()
	{
	}
	void CLOPLMSignIn::GetUserPermissions()
	{
	}
	void CLOPLMSignIn::GetConfigProperties()
	{
	}

	/*
	* Description - cancel_Clicked() method is used to close the login ui.
	* Parameter -
	* Exception -
	* Return -
	*/
	void CLOPLMSignIn::onCancel_Clicked()
	{
		this->close();
	}

	/*
	* Description - connectSignalSlots() method used to connect and disconnect the signals and slots.
	* Parameter - bool.
	* Exception -
	* Return -
	*/
	void CLOPLMSignIn::connectSignalSlots(bool b)
	{
		if (b)
		{
			QObject::connect(m_loginButton, SIGNAL(clicked()), this, SLOT(onLoginClicked()));
			QObject::connect(m_cancelButton, SIGNAL(clicked()), this, SLOT(onCancel_Clicked()));
		}
		else
		{
			QObject::disconnect(m_loginButton, SIGNAL(clicked()), this, SLOT(onLoginClicked()));
			QObject::disconnect(m_cancelButton, SIGNAL(clicked()), this, SLOT(onCancel_Clicked()));

		}
	}
}

