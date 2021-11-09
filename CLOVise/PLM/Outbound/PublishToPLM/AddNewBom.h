#pragma once
/*
* Copyright 2020-2021 CLO-Vise. All rights reserved
*
* @file PublishToPLM.h
*
* @brief Class declaration for publish Product and Document from CLO to PLM.
* This class has all the variable declarations and function declarations which are used to PLM Product and Document instance data update and Publish from CLO to PLM.
*
* @author GoVise
*
* @date 14-AUG-2020
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

using json = nlohmann::json;
using namespace std;

namespace CLOVise
{
	class AddNewBom : public MVDialog, public Ui::AddNewBom
	{
		Q_OBJECT

	public:
		static AddNewBom* GetInstance();
		static void	Destroy();
		//void getFabricTrimDetails(stringstream& _ssBOMDetails);
		void getMaterialDetails(string _str, json _techPackJson, bool _flag, QTableWidget* _sectionTable,QString _tableName, bool _isFabric);
		void CreateTableforEachSection();
		void AddBomRows(QTableWidget* _sectionTable, json _rowDataJson, QString _tableName);
		void getColorInfo(json _FabricJson, json& rowDataJson, string _materailId, bool _isFabric);
		void UpdateColorwayColumns();
		map<string, QTableWidget*> m_bomSectionTableInfoMap;
		map<QPushButton*, QTableWidget*> m_addMaterialButtonAndTableMap;
		map<string, json> m_colorwayMapForBom;
		QPushButton* currentAddMaterialButtonClicked;
		json m_mappedColorwaysArr;
		json m_colorwayOverridesJson;
		json m_bomTemplateJson;
		string m_currentTableName;
		QSignalMapper *m_buttonSignalMapper;
		QSignalMapper *m_deleteButtonSignalMapper;
		int m_currentRow;
		int m_currentColumn;
		QTreeWidget* m_createBomTreeWidget;
		json m_BomMetaData = json::object();
	private:
		AddNewBom(QWidget* parent = nullptr);
		virtual ~AddNewBom();
		void connectSignalSlots(bool _b) override;
		void drawWidget(json _feildsJson, QTreeWidget* m_TreeWidget);

		map<string,string> m_subTypeNameIdMap;
		static AddNewBom* _instance;
		QPushButton* m_backButton;
		QPushButton* m_createButton;
		QStringList m_bomTableColumnlist;
		QStringList m_bomTableColumnKeys;
		QStringList m_materialTypeList;
		json m_bomTableColumnJson;
		
		
	private slots:
		void onBackButtonClicked();
		void onCreateButtonClicked();
		void onClickAddFromMaterialButton();
		void onClickAddSpecialMaterialButton();
		void OnClickAddColorButton(const QString &string);
		void OnClickDeleteButton(int _rowCount);
		void OnHandleDropDownValue(const QString& _item);
	};
}
