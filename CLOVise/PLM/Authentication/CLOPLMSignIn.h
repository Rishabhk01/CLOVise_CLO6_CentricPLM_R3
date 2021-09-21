#pragma once
/*
* Copyright 2020-2021 CLO-Vise. All rights reserved
*
* @file CLOPLMSignIn.h
*
* @brief Class declaration for validate the user in CLO from PLM.
* This class has all the variable declarations and function declarations which are used to validate the user.
*
* @author GoVise
*
* @date 25-MAY-2020
*/
#include "ui_CLOPLMSignIn.h"

#include <string>
#include <iostream>

#include <QDialog>
#include <QPushButton>
#include <QtWidgets>
#include <QtGui>
#include "classes/widgets/MVDialog.h"

using namespace std;

namespace CLOVise 
{
	class PLMSignin
	{
	public:
		static PLMSignin*	GetInstance();
		static void			Destroy();

		void SetSignInPLM(bool b);
		bool GetSignInPLM();

	private:
		static PLMSignin* _instance;

		bool isSignInPLMEnabled;

		PLMSignin()
		{
			isSignInPLMEnabled = false;
		}
		~PLMSignin()
		{
		}	
	};
	class CLOPLMSignIn : public MVDialog, public Ui::CLOPLMSignIn
	{
		Q_OBJECT

	public:
		static CLOPLMSignIn* GetInstance();
		static void	Destroy();

		const QString& GetID();
		const QString& GetPasswd();
		void ReadJsonFile();
		void SetPlmLogo(QLabel* _uiLogoLabel, QString _iconPath);
		void ConnectToPLM(string _userName, string _password);
		void ValidateLoginCredentials();
		void GetUserDetails();
		void GetUserPermissions();
		void GetConfigProperties();
		QPushButton *m_loginButton;
		QPushButton *m_cancelButton;
		QLineEdit  *m_userNameLineEdit;
		QLineEdit  *m_passwordLineEdit;

	private:		
		CLOPLMSignIn(QWidget* parent = nullptr);
		virtual ~CLOPLMSignIn();

		void connectSignalSlots(bool b) override;	

		bool isSignInSuccessful = false;
		static CLOPLMSignIn* _instance;
		QString m_plmLoginId;
		QString m_plmLoginPw;

	private slots:
		void onLoginClicked();
		void onCancel_Clicked();

	};
}

