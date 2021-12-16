#pragma once
/*
* Copyright 2020-2021 CLO-Vise. All rights reserved
*
* @file CVDisplayMessageBox.h
*
* @brief Class declaration for display custom message.
* This class has all the variable and function declarations which are used get custom input to show in CLO.
*
* @author GoVise
*
* @date 27-MAY-2020
*/
#include "ui_CVDisplayMessageBox.h"

#include <string>
#include <iostream>
#include <fstream>

#include <QDialog>
#include <QtCore>
#include <QtGui>
#include <QDebug>
#include <QTreeWidgetItem>

#include "classes/widgets/MVDialog.h"

using namespace std; 

namespace CLOVise
{

	class CVDisplayMessageBox : public MVDialog, public Ui::CVDisplayMessageBox

	{
		Q_OBJECT

	public:
		CVDisplayMessageBox(QWidget* parent = nullptr);
		virtual ~CVDisplayMessageBox();

		static CVDisplayMessageBox* GetInstance();
		static void	Destroy();

		QTimer *timer;
		int CurProgress = 0;

		void DisplyMessage(string _message);

	private:
		static CVDisplayMessageBox* _instance;

	private slots:
		void on_pbGo_released();
		void TimerSlot();
	};
}
