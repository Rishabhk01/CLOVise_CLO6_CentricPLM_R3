#pragma once
/*
* Copyright 2021-2022 CLO-Vise. All rights reserved
*
* @file AddNewBom.h
*
* @brief Class implementation for create Bom table on tab.
* This class has all the variable declarations and function declarations which are used to create bom table and publish bom lines from CLO to PLM.
*
* @author GoVise
*
* @date 10-OCT-2021
*/
#include "ui_AddNewBom.h"

#include <string>
#include <iostream>

#include<qstring.h>
#include <QDialog>
#include <QtGui>
#include <qtreewidget.h>
#include <QtCore>
#include <QtWidgets>
#include <QCombobox> 
#include "classes/widgets/MVTableWidget.h"
#include "CLOVise/PLM/Libraries/json.h"
#include "classes/widgets/MVDialog.h"
#include "CLOVise/PLM/Outbound/PublishToPLM/Section.h"
using json = nlohmann::json;
using namespace std;

namespace CLOVise
{
	class AddNewBom : public MVDialog, public Ui::AddNewBom
	{
		Q_OBJECT
			
	public:
		static AddNewBom* GetInstance();
		QTreeWidget* m_createBOMTreeWidget;
		//vector<Section*> m_sectionVector;
		json m_BOMMetaData = json::object();
		json m_BOMTemplateJson = json::object();
	private:
		AddNewBom(QWidget* parent = nullptr);
		~AddNewBom();
		void connectSignalSlots(bool _b) override;
		void drawWidget(json _feildsJson, QTreeWidget* m_TreeWidget);

		map<string, string> m_subTypeNameIdMap;
		static AddNewBom* _instance;
		QPushButton* m_backButton;
		QPushButton* m_createButton;
		
		
	private slots:
		void onBackButtonClicked();
		void onCreateButtonClicked();
		void OnHandleDropDownValue(const QString& _item);
	};
}
