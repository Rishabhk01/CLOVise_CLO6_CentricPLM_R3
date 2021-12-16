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
	CVDisplayMessageBox* CVDisplayMessageBox::_instance = NULL;
	CVDisplayMessageBox* CVDisplayMessageBox::GetInstance()
	{
		if (_instance == NULL)
		{
			_instance = new CVDisplayMessageBox();
		}
		return _instance;
	}

	void CVDisplayMessageBox::Destroy()
	{
		if (_instance)
		{
			delete _instance;
			_instance = NULL;
		}
	}

	CVDisplayMessageBox::CVDisplayMessageBox(QWidget* parent) : MVDialog(parent)
	{
		setupUi(this);
		m_CVTitleBar = new CVTitleBar("Information", this);
		this->setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint | Qt::CustomizeWindowHint);
		layout()->setMenuBar(m_CVTitleBar);
		progressBar->setTextVisible(false);
		progressBar->setValue(0);
		progressBar->setMinimum(0);
		progressBar->setFixedHeight(10);
		timer = new QTimer(this);
		QTimer::singleShot(0, this, &CVDisplayMessageBox::on_pbGo_released);
		connect(timer, &QTimer::timeout, this, &CVDisplayMessageBox::TimerSlot);
	}

	CVDisplayMessageBox::~CVDisplayMessageBox()
	{
		Destroy();
	}

	/*
	* Description - on_pbGo_released() method used to process the progress bar.
	* Parameter -
	* Exception -
	* Return -
	*/
	void CVDisplayMessageBox::on_pbGo_released()
	{
		CurProgress = 0;
		progressBar->setValue(CurProgress);
		timer->start(20);
	}

	/*
	* Description - TimerSlot() method used to close the progress bar after process.
	* Parameter -
	* Exception -
	* Return -
	*/
	void CVDisplayMessageBox::TimerSlot()
	{
		CurProgress++;
		if (CurProgress > progressBar->maximum()) // stop logic
		{
			timer->stop();
			this->close();
		}

		else
			progressBar->setValue(CurProgress);
	}

	/*
	* Description - DisplyMessage() method used to the custom message.
	* Parameter -  string
	* Exception -
	* Return -
	*/
	void CVDisplayMessageBox::DisplyMessage(string _message)
	{
		label->setText(QString::fromStdString(_message));
	}
}