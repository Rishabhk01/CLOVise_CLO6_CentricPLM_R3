#pragma once
/*
* Copyright 2021-2022 CLO-Vise. All rights reserved
*
* @file CreateProductBOMHandler.h
*
* @brief Class implementation for create Bom table on tab.
* This class has all the variable declarations and function declarations which are used to create bom table and publish bom lines from CLO to PLM.
*
* @author GoVise
*
* @date 18-JAN-2021
*/


#include <string>
#include <iostream>

#include <qstring.h>
#include <QDialog>
#include <QtGui>
#include <qtreewidget.h>
#include <QtCore>
#include <QtWidgets>
#include <QCombobox> 
#include "classes/widgets/MVTableWidget.h"
#include "CLOVise/PLM/Libraries/json.h"
#include "classes/widgets/MVDialog.h"
#include "CLOVise/PLM/Outbound/PublishToPLM/BOM/BOMUtility.h"
using json = nlohmann::json;
using namespace std;





namespace CLOVise
{
	class CreateProductBOMHandler : public QObject
	{
		Q_OBJECT
			static CreateProductBOMHandler* _instance;
		CreateProductBOMHandler(QObject* parent = nullptr);
	public:
		static CreateProductBOMHandler* GetInstance();
		void CreateBom(json _sectionIdsJson);
		void readBomTableColumnJson();
		void AddBomRows(QTableWidget* _sectionTable, json _rowDataJson, QString _tableName, json _placementMateriaTypeJson, bool _userAddedRow = 0);
		void getMaterialDetails(string _str, json _techPackJson, bool _isFabric);
		void AddMaterialInBom();
		void populateTechPackDataInBom();
		void CreateBom(string _productId, json _BomMetaData, map<string, string> _CloAndPLMColorwayMap);
		void ClearBomData();
		void BackupBomDetails();
		void RestoreBomDetails();
		void UpdateColorwayColumnsInBom();
		json m_bomTableColumnJson;
		QStringList m_bomTableColumnlist;
		QStringList m_bomTableColumnKeys;
		QSignalMapper *m_addColorButtonSignalMapper;
		QSignalMapper *m_deleteButtonSignalMapper;
		map<string, QTableWidget*> m_bomSectionTableInfoMap;
		map<string, json> m_bomSectionNameAndTypeMap;
		map<QPushButton*, QTableWidget*> m_addMaterialButtonAndTableMap;
		map<QPushButton*, QTableWidget*> m_addSpecialMaterialButtonAndTableMap;
		map<string, QStringList> m_sectionMaterialTypeMap;
		map<string, string> m_materialTypeNameIdMap;
		map<string, json> m_backupBomDataMap;
		map<string, json> m_colorwayMapForBom;
		QPushButton* m_currentAddMaterialButtonClicked;
		string m_currentTableName;
		int m_currentRow;
		int m_currentColumn;
		bool m_bomCreated;
		bool IsBomCreated();
		void connectSignalSlots(bool _b);
		bool ValidateBomFields();
	private slots:
		void OnClickDeleteButton(const QString &position);
		void onClickAddFromMaterialButton();
		void onClickAddSpecialMaterialButton();
		void OnClickAddColorButton(const QString &position);
	};
}
