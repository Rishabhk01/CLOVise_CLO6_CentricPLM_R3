/*
* Copyright 2020-2021 CLO-Vise. All rights reserved
*
* @file CVDisplayMessageBox.cpp
*
* @brief Class implementation for display custom message.
* This class has all the variable and mehtods implementation which are used get custom input to show in CLO.
*
* @author GoVise
*
* @date 27-MAY-2020
*/
#include "CVDisplayMessageBox.h"

#include <qmessagebox.h>
#include <qcheckbox.h>

#include <CLOAPIInterface.h>
#include "CLOVise/PLM/Helper/Util/Configuration.h"
#include "CLOVise/PLM/Helper/Util/Logger.h"
#ifdef __APPLE__
#include <unistd.h>

#else

#include <windows.h>
#include <direct.h>

#endif

using namespace std;

namespace CLOVise
{
	

	

	CVDisplayMessageBox::CVDisplayMessageBox(QWidget* parent) : MVDialog(parent)
	{
		Logger::Debug("CVDisplayMessageBox::CVDisplayMessageBox constructor Start");
		setupUi(this);
		m_CVTitleBar = new CVTitleBar("Information", this);
		this->setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint | Qt::CustomizeWindowHint);
		layout()->setMenuBar(m_CVTitleBar);
		progressBar->setTextVisible(false);
		progressBar->setValue(0);
		progressBar->setMinimum(0);
		progressBar->setMaximum(Configuration::GetInstance()->GetProgressBarTimer());
		progressBar->setFixedHeight(10);
		QTimer::singleShot(0, this, SLOT(onProgress()));
		Logger::Debug("CVDisplayMessageBox::CVDisplayMessageBox constructor End");
	}

	CVDisplayMessageBox::~CVDisplayMessageBox()
	{
		Logger::Debug("CVDisplayMessageBox::CVDisplayMessageBox destructor Start");
		Logger::Debug("CVDisplayMessageBox::CVDisplayMessageBox destructor End");
		
	}

	/*
	* Description - progress() method used to process the progress bar.
	* Parameter -
	* Exception -
	* Return -
	*/
	void CVDisplayMessageBox::onProgress()
	{
		Logger::Debug("CVDisplayMessageBox::onProgress Start");
		for (int i = 0; i < Configuration::GetInstance()->GetProgressBarTimer(); i++)
		{
			progressBar->setValue(i);
			QApplication::processEvents();

            #ifdef __APPLE__
            usleep(1000);
            #else
			Sleep(1);
            #endif
		}
		Logger::Debug("CVDisplayMessageBox::onProgress End");
		this->close();
		
	}

	/*
	* Description - DisplyMessage() method used to the custom message.
	* Parameter -  string.
	* Exception -
	* Return -
	*/
	void CVDisplayMessageBox::DisplyMessage(string _message)
	{
		Logger::Debug("CVDisplayMessageBox::DisplyMessage Start");
		label->setText(QString::fromStdString(_message));
		Logger::Debug("CVDisplayMessageBox::DisplyMessage End");
	}
}