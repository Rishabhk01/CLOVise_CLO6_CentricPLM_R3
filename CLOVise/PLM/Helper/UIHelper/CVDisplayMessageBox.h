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
		~CVDisplayMessageBox();

			

		void DisplyMessage(string _message);
		
	

	private slots:
		void onProgress();
	};
}
