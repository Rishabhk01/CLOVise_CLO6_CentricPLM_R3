#pragma once
/*
* Copyright 2020-2021 CLO-Vise. All rights reserved
*
* @file CLOPLMImplemntation.h
*
* @brief Class declaration for set the view/search mode in CLO to access PLM.
* This class has all the variable declarations and function declarations which are used access the PLM in CLO.
*
* @author GoVise
*
* @date 25-MAY-2020
*/

#include <string>
#include <iostream>

#include <QDialog>

using namespace std;
namespace CLOVise
{
	enum SEARCH_ITEM_MODE
	{
		PLM_SEARCH
	};

class CLOPLMImplementation : public QDialog
	{
		Q_OBJECT

	public:
		CLOPLMImplementation(QWidget* parent = 0, Qt::WindowFlags flags = Qt::Dialog | Qt::FramelessWindowHint, SEARCH_ITEM_MODE mode= PLM_SEARCH);
		~CLOPLMImplementation();

		void SetSearchMode(SEARCH_ITEM_MODE _mode, bool _signin);
		bool SigninPLM();
		void ConnectToCLOVise(SEARCH_ITEM_MODE _mode, bool _fromSignin);
		
	private:
		SEARCH_ITEM_MODE searchMode;
		
	};
}
