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
		//void getFabricTrimDetails(stringstream& _ssBOMDetails);
		void getMaterialDetails(string _str, json _techPackJson, bool _isFabric);
		//void CreateTableforEachSection(json _sectionIdsjson);
		void AddBomRows(QTableWidget* _sectionTable, json _rowDataJson, QString _tableName, json _placementMateriaTypeJson, bool _userAddedRow=false);
		void getColorInfo(json _FabricJson, json& rowDataJson, string _materailId, bool _isFabric);
		void UpdateColorwayColumns();
		void BackupBomDetails();
		void populateTechPackDataInBom();
		void RestoreBomDetails();
		bool ValidateBomFields();
		bool IsBomCreated();
		map<string, QTableWidget*> m_bomSectionTableInfoMap;
		map<QPushButton*, QTableWidget*> m_addMaterialButtonAndTableMap;
		map<QPushButton*, QTableWidget*> m_addSpecialMaterialButtonAndTableMap;
		map<string, json> m_backupBomDataMap;
		map<string, json> m_colorwayMapForBom;
		map<string, json> m_bomSectionNameAndTypeMap;
		map<string, string> m_materialTypeNameIdMap;
		map<string, QStringList> m_sectionMaterialTypeMap;
		QPushButton* currentAddMaterialButtonClicked;
		json m_mappedColorwaysArr;
		json m_colorwayOverridesJson;
		json m_bomTemplateJson;
		string m_currentTableName;
		QSignalMapper *m_addColorButtonSignalMapper;
		QSignalMapper *m_deleteButtonSignalMapper;
		int m_currentRow;
		int m_currentColumn;
		QTreeWidget* m_createBomTreeWidget;
		//vector<Section*> m_sectionVector;
		void ClearBomData();
		QTableWidget* GetSectionTable(string _sectionName);
		json GetMaterialTypeForSection(string _sectionName);
		//void CreateSectionInBom(string _sectionName, string _sectionId, QStringList _tablecolumnList, int& _sectionCount, json _placementProductTypeJson);
		//QWidget* CreateCustomWidget(string _rgbValue, string _colorName, QTableWidget* _sectionTable, QString _tableName, int _row, int _column);
		json m_BomMetaData = json::object();

	private:
		AddNewBom(QWidget* parent = nullptr);
		~AddNewBom();
		void connectSignalSlots(bool _b) override;
		void drawWidget(json _feildsJson, QTreeWidget* m_TreeWidget);

		map<string, string> m_subTypeNameIdMap;
		static AddNewBom* _instance;
		QPushButton* m_backButton;
		QPushButton* m_createButton;
		QStringList m_bomTableColumnlist;
		QStringList m_bomTableColumnKeys;
		QStringList m_materialTypeList;
		QStringList m_ColorwaysAddedInBom;
		json m_bomTableColumnJson;
		bool m_bomCreated;
		
	private slots:
		void onBackButtonClicked();
		void onCreateButtonClicked();
		void onClickAddFromMaterialButton();
		void onClickAddSpecialMaterialButton();
		void OnClickAddColorButton(const QString &string);
		void OnClickDeleteButton(const QString &string);
		//void OnClickDeleteButton(int _rowCount);
		void OnHandleDropDownValue(const QString& _item);
	};
}
