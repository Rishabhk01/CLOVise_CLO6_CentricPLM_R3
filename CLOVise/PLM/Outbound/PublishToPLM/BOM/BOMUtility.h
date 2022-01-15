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
#include "CLOVise/PLM/Helper/UIHelper/UIHelper.h"
#include "CLOVise/PLM/Helper/Util/Logger.h"
#include "CLOVise/PLM/Outbound/PublishToPLM/Section.h"
#include "CLOVise/PLM/Outbound/PublishToPLM/BOM/AddNewBom.h"
using json = nlohmann::json;
using namespace std;

namespace BOMUtility
{
	static json m_mappedColorwaysArr;
	static QStringList m_materialTypeList;
	
	
	static QSignalMapper *m_addColorButtonSignalMapper;
	static QSignalMapper *m_deleteButtonSignalMapper;
	static map<string, json> m_bomSectionNameAndTypeMap;
	static map<string, QTableWidget*> m_bomSectionTableInfoMap;
	static QStringList m_mappedColorways;
	static QStringList m_bomTableColumnlist;
	static QStringList m_bomTableColumnKeys;

	inline  void SetBomTableColumnNameAndKey(QStringList& _bomTableColumnlist, QStringList& _bomTableColumnKeys)
	{
		m_bomTableColumnlist = _bomTableColumnlist;
		m_bomTableColumnKeys = _bomTableColumnKeys;
	}
	inline  void SetSignalMappers(QSignalMapper* _deleteButtonMapper, QSignalMapper* _AddColorButtonMapper )
	{
		m_addColorButtonSignalMapper = _AddColorButtonMapper;
		m_deleteButtonSignalMapper = _deleteButtonMapper;
	}
	inline void SetTableInfoMap(map<string, QTableWidget*>& _bomSectionTableInfoMap, map<string, json>& _bomSectionNameAndTypeMap, QStringList _mappedColorways)
	{
		m_bomSectionTableInfoMap = _bomSectionTableInfoMap;
		m_bomSectionNameAndTypeMap = _bomSectionNameAndTypeMap;
		m_mappedColorways = _mappedColorways;
	}
	inline QTableWidget* GetSectionTable(map<string, QTableWidget*> _map, string _sectionName)
	{
		Logger::Debug("AddNewBom -> AddBomRows() -> Start");
		Logger::Debug("AddNewBom -> AddBomRows() -> m_bomSectionTableInfoMap.size()" + to_string(m_bomSectionTableInfoMap.size()));
		auto itr = _map.find(_sectionName);
		QTableWidget* table = nullptr;
		if (itr != m_bomSectionTableInfoMap.end())
		{
			table = itr->second;
		}
		Logger::Debug("AddNewBom -> AddBomRows() -> end");
		return table;
	}
	static json GetMaterialTypeForSection(map<string, json> _map,string _sectionName)
	{
		Logger::Debug("BOMUtility -> GetMaterialTypeForSection() -> Start");
		Logger::Debug("BOMUtility -> GetMaterialTypeForSection() -> m_bomSectionNameAndTypeMap.size()" + to_string(_map.size()));
		auto itr = _map.find(_sectionName);
		json materialTypeJson;
		if (itr != _map.end())
		{
			materialTypeJson = itr->second;
		}
		return materialTypeJson;
		Logger::Debug("BOMUtility -> GetMaterialTypeForSection() -> End");
	}

	

	inline  map<string, string> GetCentricMaterialTypes()
	{
		map<string, string> materialTypeMap;
		json responseJson = Helper::makeRestcallGet(RESTAPI::MATERIAL_TYPE_SEARCH_API, "?&limit=100", "", "Loading materail type details..");

		m_materialTypeList.append(QString::fromStdString(BLANK));
		for (int i = 0; i < responseJson.size(); i++)
		{
			json attJson = Helper::GetJSONParsedValue<int>(responseJson, i, false);;///use new method
			string attName = Helper::GetJSONValue<string>(attJson, ATTRIBUTE_NAME, true);
			Logger::Debug("AddNewBom -> AddNewBom attName: " + attName);
			string attId = Helper::GetJSONValue<string>(attJson, ATTRIBUTE_ID, true);
			Logger::Debug("AddNewBom -> AddNewBom attId: " + attId);
			materialTypeMap.insert(make_pair(attId, attName));
		}
		return materialTypeMap;
	}
	inline void CreateSectionInBom(QVBoxLayout* _mainLayout, string _sectionName, string _sectionId, QStringList _tablecolumnList, int& _sectionCount, json _placementProductTypeJson, map<string, QTableWidget*>& _bomSectionTableInfoMap, map<string, json>& _bomSectionNameAndTypeMap, map<QPushButton*, QTableWidget*>& _addMaterialButtonAndTableMap, map<QPushButton*, QTableWidget*>& _addSpecialMaterialButtonAndTableMap, QStringList& _bomTableColumnlist, QStringList& _bomTableColumnKeys)
	{
		Logger::Debug("BOMUtility CreateSectionInBom Start: ");
		bool isSectionValidForStyleType = false;
		Section* section = new Section(QString::fromStdString(_sectionName), 300);
		Logger::Debug("BOMUtility CreateSectionInBom 1: ");
		//	m_sectionVector.push_back(section);
		MVTableWidget* sectionTable = new MVTableWidget();
		CVWidgetGenerator::InitializeTableView(sectionTable);
		sectionTable->setShowGrid(false);
		sectionTable->setProperty("TableName", QString::fromStdString(_sectionName));
		sectionTable->setProperty("SectionId", QString::fromStdString(_sectionId));
		sectionTable->setColumnCount(_tablecolumnList.size());
		sectionTable->setHorizontalHeaderLabels(_tablecolumnList);
		Logger::Debug("BOMUtility CreateSectionInBom 2: ");
		_bomSectionTableInfoMap.insert(make_pair(_sectionName, sectionTable));
		Logger::Debug("BOMUtility CreateSectionInBom 2: _bomSectionTableInfoMap " + to_string(_bomSectionTableInfoMap.size()));
		_bomSectionNameAndTypeMap.insert(make_pair(_sectionName, _placementProductTypeJson));
		sectionTable->setStyleSheet("QTableWidget{ background-color: #262628; border-right: 1px solid #000000; border-top: 1px solid #000000; border-left: 1px solid #000000; font-face: ArialMT; font-size: 12px; color: #FFFFFF; } QTableWidget::Item{outline:0;}");
		sectionTable->verticalHeader()->hide();
		//sectionTable->horizontalHeader()->setMinimumHeight(10);
		sectionTable->verticalHeader()->setDefaultSectionSize(30);
		sectionTable->setShowGrid(false);
		sectionTable->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
		//sectionTable->setSelectionMode(QAbstractItemView::NoSelection);
		sectionTable->horizontalHeader()->setStretchLastSection(true);

		auto* addMaterialButtonLayout = new QHBoxLayout();
		QPushButton* addPlmMaterialButton = CVWidgetGenerator::CreatePushButton("New From Material", ADD_HOVER_ICON_PATH, "New From Material", PUSH_BUTTON_STYLE, 30, true);
		addPlmMaterialButton->setParent(sectionTable);
		addPlmMaterialButton->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
		addPlmMaterialButton->setProperty("TableName", QString::fromStdString(_sectionName));
		QPushButton* addSpecialMaterialButton = CVWidgetGenerator::CreatePushButton("New Special", ADD_HOVER_ICON_PATH, "New Special", PUSH_BUTTON_STYLE, 30, true);
		addSpecialMaterialButton->setParent(sectionTable);
		addSpecialMaterialButton->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);


		//QAction* AddFromMaterialAction = new QAction(tr("New From Material"), this);
		//connect(AddFromMaterialAction, SIGNAL(triggered()), this, SLOT(onClickAddFromMaterialButton()));

		//QAction* AddSpecialMaterialAction = new QAction(tr("New Special"), this);
		//connect(AddSpecialMaterialAction, SIGNAL(triggered()), this, SLOT(onClickAddSpecialMaterialButton()));

		_addMaterialButtonAndTableMap.insert(make_pair(addPlmMaterialButton, sectionTable));
		_addSpecialMaterialButtonAndTableMap.insert(make_pair(addSpecialMaterialButton, sectionTable));
		//QMenu* menu = new QMenu(addMaterialButton);
		//menu->addAction(AddFromMaterialAction);
		//menu->addAction(AddSpecialMaterialAction);
		//addMaterialButton->setMenu(menu);
		Logger::Debug("BOMUtility CreateSectionInBom 2: _addMaterialButtonAndTableMap " + to_string(_addMaterialButtonAndTableMap.size()));
		Logger::Debug("BOMUtility CreateSectionInBom 2: _addSpecialMaterialButtonAndTableMap " + to_string(_addSpecialMaterialButtonAndTableMap.size()));
		QSpacerItem *horizontalSpacer = new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Fixed);
	//	connect(addPlmMaterialButton, SIGNAL(clicked()), this, SLOT(onClickAddFromMaterialButton()));
	//	connect(addSpecialMaterialButton, SIGNAL(clicked()), this, SLOT(onClickAddSpecialMaterialButton()));

		addMaterialButtonLayout->insertSpacerItem(0, horizontalSpacer);
		addMaterialButtonLayout->insertWidget(1, addPlmMaterialButton);
		//addMaterialButtonLayout->insertSpacerItem(2, horizontalSpacer);
		addMaterialButtonLayout->insertWidget(2, addSpecialMaterialButton);
		auto* anyLayout = new QVBoxLayout();
		anyLayout->insertLayout(0, addMaterialButtonLayout);
		anyLayout->insertWidget(1, sectionTable);
		section->setContentLayout(*anyLayout);
		section->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
		Logger::Debug("BOMUtility CreateSectionInBom _sectionCount: " + to_string(_sectionCount));
		_mainLayout->insertWidget(_sectionCount, section);
		Logger::Debug("BOMUtility CreateSectionInBom End: ");
	}

	inline void CreateTableforEachSection(QVBoxLayout* _mainLayout, json _sectionIdsjson, map<string, QTableWidget*>& _bomSectionTableInfoMap, map<string, json>& _bomSectionNameAndTypeMap, map<string, QStringList>& m_sectionMaterialTypeMap, map<QPushButton*, QTableWidget*>& _addMaterialButtonAndTableMap, map<QPushButton*, QTableWidget*>& _addSpecialMaterialButtonAndTableMap, QStringList _mappedColorways, string _styleTypeId, QStringList& _bomTableColumnlist, QStringList& _bomTableColumnKeys)
	{
		Logger::Debug("AddNewBom CreateTableforEachSection Start: ");

		json colorwayListJsonArr = json::array();
		string colorwayList = Helper::GetJSONValue<string>(Configuration::GetInstance()->GetTechPackJson(), "colorwayList", false);
		colorwayListJsonArr = json::parse(colorwayList);
		for (int colorwayListCount = 0; colorwayListCount < colorwayListJsonArr.size(); colorwayListCount++)
		{
			json colorwayJson = json::object();
			json colorJson = json::object();
			string cloColorwayStr = colorwayListJsonArr[colorwayListCount].dump();
			json cloColorwayJson = json::parse(cloColorwayStr);

			string colorwayName = Helper::GetJSONValue<string>(cloColorwayJson, "name", true);
			colorwayJson["colorwayName"] = colorwayName;
			colorwayJson["colorwayIndex"] = to_string(colorwayListCount);
			m_mappedColorwaysArr.push_back(colorwayJson);

		}

		string sectionId;
		for (int sectionCount = 0; sectionCount < _sectionIdsjson.size(); sectionCount++)
		{
			string section = Helper::GetJSONValue<int>(_sectionIdsjson, sectionCount, true);
			Logger::Debug("AddNewBom -> CreateTableforEachSection() -> apiMetadataStr" + sectionId);
			sectionId += "id=" + section + "&";
		}
		sectionId = sectionId.substr(0, sectionId.length() - 1);
		string sectionDefinitions;
		if (!sectionId.empty())
			sectionDefinitions = RESTAPI::CentricRestCallGet(Configuration::GetInstance()->GetPLMServerURL() + RESTAPI::BOM_SECTION_DEFINITION_API + "?" + sectionId + "&sort=sort_order&limit=1000", APPLICATION_JSON_TYPE, "");
		Logger::Debug("AddNewBom -> CreateTableforEachSection() -> resultResponse" + sectionDefinitions);

		//QStringList sectionList;
		//sectionList << "FABRICS" << "TRIMS" << "LABELS";

		QStringList tablecolumnList;
		QStringList bomTableColumnKeys;
		tablecolumnList = _bomTableColumnlist;
		bomTableColumnKeys = _bomTableColumnKeys;
		Logger::Debug("AddNewBom -> CreateTableforEachSection() -> 1");
		if (_mappedColorways.size())
		{

			tablecolumnList.append(_mappedColorways);
			bomTableColumnKeys.append(_mappedColorways);

		}
		int sectionCountOnBomTab = 0;
		json placementProductTypeJson;
		if (!sectionDefinitions.empty())
		{
			json sectionDefinitionsJson = json::parse(sectionDefinitions);

			for (int sectionCount = 0; sectionCount < sectionDefinitionsJson.size(); sectionCount++)
			{
				Logger::Debug("AddNewBom -> CreateTableforEachSection() -> 1");
				json sectionCountJson = Helper::GetJSONParsedValue<int>(sectionDefinitionsJson, sectionCount, false);;///use new method
				Logger::Debug("AddNewBom -> CreateTableforEachSection() -> 1");
				Logger::Debug("AddNewBom -> CreateTableforEachSection() -> sectionCountJson" + to_string(sectionCountJson));
				string sectionId = Helper::GetJSONValue<string>(sectionCountJson, ATTRIBUTE_ID, true);
				Logger::Debug("AddNewBom -> CreateTableforEachSection() -> sectionId" + sectionId);
				string sectionName = Helper::GetJSONValue<string>(sectionCountJson, "node_name", true);
				Logger::Debug("AddNewBom -> CreateTableforEachSection() -> sectionName" + sectionName);
				json bomProductTypeJson = Helper::GetJSONParsedValue<string>(sectionCountJson, "bom_product_types", false);
				Logger::Debug("AddNewBom -> CreateTableforEachSection() -> bomProductTypeJson" + to_string(bomProductTypeJson));
				placementProductTypeJson = Helper::GetJSONParsedValue<string>(sectionCountJson, "placement_product_types", false);
				Logger::Debug("AddNewBom -> CreateTableforEachSection() -> placementProductTypeJson" + to_string(placementProductTypeJson));
				bool isSectionValidForStyleType = false;


				for (int itr = 0; itr < bomProductTypeJson.size(); itr++)
				{
					string bomProductType = Helper::GetJSONValue<int>(bomProductTypeJson, itr, true);
					if (bomProductType == _styleTypeId)
						isSectionValidForStyleType = true;
				}

				if (!isSectionValidForStyleType)
					continue;

				map<string, QStringList>::iterator it;

				for (int itr = 0; itr < placementProductTypeJson.size(); itr++)
				{
					QStringList sectionNamelist;
					string bomPlacementProductTypeId = Helper::GetJSONValue<int>(placementProductTypeJson, itr, true);

					it = m_sectionMaterialTypeMap.find(bomPlacementProductTypeId);
					if (it != m_sectionMaterialTypeMap.end())
					{
						sectionNamelist = it->second;
						m_sectionMaterialTypeMap.erase(bomPlacementProductTypeId);
					}
					sectionNamelist.append(QString::fromStdString(sectionName));

					m_sectionMaterialTypeMap.insert(make_pair(bomPlacementProductTypeId, sectionNamelist));
				}

				CreateSectionInBom(_mainLayout, sectionName, sectionId, tablecolumnList, sectionCountOnBomTab, placementProductTypeJson, _bomSectionTableInfoMap, _bomSectionNameAndTypeMap, _addMaterialButtonAndTableMap, _addSpecialMaterialButtonAndTableMap, _bomTableColumnlist, _bomTableColumnKeys);
				sectionCountOnBomTab++;


			}
		}
		// Create blank section
		CreateSectionInBom(_mainLayout, "Blank", "", tablecolumnList, sectionCountOnBomTab, placementProductTypeJson, _bomSectionTableInfoMap, _bomSectionNameAndTypeMap,_addMaterialButtonAndTableMap,_addSpecialMaterialButtonAndTableMap, _bomTableColumnlist, _bomTableColumnKeys);


	}
	inline void getColorInfo(json _FabricJson, json& _rowDataJson, string _materailId, bool _isFabric, map<string, json>& _colorwayMapForBom)
	{
		Logger::Debug("AddNewBom -> getColorInfo () Start");

		json colorwayJson = json::object();
		json colorJson = json::object();
		json colorwayListJsonArr = json::array();
		string colorwayList = Helper::GetJSONValue<string>(_FabricJson, "colorwayList", false);
		colorwayListJsonArr = json::parse(colorwayList);
		int count = 0;
		int Tempcount = 0;
		json MaterialColorwayDetailsJson = json::object();
		for (int colorwayListCount = 0; colorwayListCount < colorwayListJsonArr.size(); colorwayListCount++)
		{
			colorwayJson = json::object();
			colorJson = json::object();


			string cloColorwayStr = colorwayListJsonArr[colorwayListCount].dump();
			json cloColorwayJson = json::parse(cloColorwayStr);
			Logger::Logger("cloColorwayJsoncloColorwayJson - " + to_string(cloColorwayJson));
			string frontJsonStr;
			if (_isFabric)
				frontJsonStr = Helper::GetJSONValue<string>(cloColorwayJson, "materialFront", false);
			else
				frontJsonStr = Helper::GetJSONValue<string>(cloColorwayJson, "bodyMaterial", false);
			json frontJson = json::parse(frontJsonStr);
			Logger::Logger("frontJsonfrontJsonfrontJson - " + to_string(frontJson));
			string cloColorName = Helper::GetJSONValue<string>(frontJson, "baseColorName", true);
			string colorApiMetadataStr = Helper::GetJSONValue<string>(frontJson, TECHPACK_API_META_DATA_KEY, false);
			if (FormatHelper::HasContent(colorApiMetadataStr))
			{
				json colorApimetaDataJson = json::parse(colorApiMetadataStr);
				Logger::Logger("colorApimetaDataJsoncolorApimetaDataJsoncolorApimetaDataJson - " + to_string(colorApimetaDataJson));
				string colorId = Helper::GetJSONValue<string>(colorApimetaDataJson, "CLOVISE_COLOR_ID", true);
				for (int mappedColorwaysCount = 0; mappedColorwaysCount < m_mappedColorwaysArr.size(); mappedColorwaysCount++)
				{
					string mappedColorwaysStr = m_mappedColorwaysArr[mappedColorwaysCount].dump();
					json mappedColorwaysJson = json::parse(mappedColorwaysStr);
					Logger::Logger("mappedColorwaysJsonmappedColorwaysJsonmappedColorwaysJson - " + to_string(mappedColorwaysJson));
					string colorwayIndex = Helper::GetJSONValue<string>(mappedColorwaysJson, "colorwayIndex", true);
					string colorwayName = Helper::GetJSONValue<string>(mappedColorwaysJson, "colorwayName", true);
					Logger::Logger("colorwayIndexcolorwayIndex - " + (colorwayIndex));
					if (to_string(colorwayListCount) == colorwayIndex)
					{
						string colorwayObjectId = Helper::GetJSONValue<string>(mappedColorwaysJson, "plmColorwayObjId", true);
						Logger::Logger("colorwayObjectIdcolorwayObjectId - " + (colorwayObjectId));
						/*colorwayJson["colorwayObjectId"] = colorwayObjectId;
						colorwayJson["colorwayName"] = colorwayName;
						colorwayJson["colorObjectId"] = colorId;
						colorwayJson["colorName"] = colorwayName;*/
						colorJson["colorObjectId"] = colorId;
						colorJson["colorwayName"] = colorwayName;
						colorJson["colorName"] = cloColorName;
						_rowDataJson["color"][count++] = colorJson;


						MaterialColorwayDetailsJson[colorwayName] = colorJson;
						//m_colorwayOverridesJson.push_back(colorwayJson);
					}
				}
			}
		}
		Logger::Debug("AddNewBom -> getColorInfo () _materailId" + _materailId);
		Logger::Debug("AddNewBom -> getColorInfo () MaterialColorwayDetailsJson" + to_string(MaterialColorwayDetailsJson));
		_colorwayMapForBom.insert(make_pair(_materailId, MaterialColorwayDetailsJson));
		//Logger::Debug("AddNewBom -> getColorInfo () m_colorwayOverridesJson" + to_string(m_colorwayOverridesJson));
		Logger::Debug("AddNewBom -> getColorInfo () End");
	}

	inline void UpdateColorwayColumns(map<string, json> _colorwayMapForBom)
	{

		Logger::Debug("AddNewBom -> UpdateColorwayColumns () Start");


		QStringList tablecolumnList;
		QStringList bomTableColumnKeys;
		tablecolumnList = m_bomTableColumnlist;
		bomTableColumnKeys = m_bomTableColumnKeys;
		QStringList bomColorwayColumns;


		if (m_mappedColorways.size() && m_bomSectionTableInfoMap.size() > 0)
		{

			tablecolumnList.append(m_mappedColorways);
			bomTableColumnKeys.append(m_mappedColorways);

		}
		int rowCount = 0;
		for (auto itr = m_bomSectionTableInfoMap.begin(); itr != m_bomSectionTableInfoMap.end(); itr++)// map contain section name and corresponding table pointer  
		{
			Logger::Debug("AddNewBom -> UpdateColorwayColumns () 1");
			QTableWidget* sectionTable = itr->second;
			int columnCount = sectionTable->columnCount();
			int bomColumnCountWOColorway = m_bomTableColumnlist.size();
			if (columnCount > bomColumnCountWOColorway)// means colorway column in table
			{
				int diff = columnCount - bomColumnCountWOColorway;
				Logger::Debug("AddNewBom -> UpdateColorwayColumns () diff" + to_string(diff));
				while (diff)
				{
					int columnNumber = bomColumnCountWOColorway + diff;
					Logger::Debug("AddNewBom -> UpdateColorwayColumns () columnNumber" + to_string(columnNumber));
					QString columnName = sectionTable->horizontalHeaderItem(columnNumber - 1)->text();
					bomColorwayColumns.append(columnName);
					if (!m_mappedColorways.contains(columnName))
						sectionTable->removeColumn(columnNumber - 1);
					diff--;
				}
			}
		}
		Logger::Debug("AddNewBom -> UpdateColorwayColumns () m_bomSectionTableInfoMap.size()" + to_string(m_bomSectionTableInfoMap.size()));
		for (auto itr = m_bomSectionTableInfoMap.begin(); itr != m_bomSectionTableInfoMap.end(); itr++)// map contain section name and corresponding table pointer  
		{
			QTableWidget* sectionTable = itr->second;
			Logger::Debug("AddNewBom -> UpdateColorwayColumns () itr->first" + itr->first);
			QString tableName = QString::fromStdString(itr->first);
			//Logger::Debug("AddNewBom -> UpdateColorwayColumns () 11");
			sectionTable->setColumnCount(bomTableColumnKeys.size());
			sectionTable->setHorizontalHeaderLabels(tablecolumnList);

			Logger::Debug("AddNewBom -> UpdateColorwayColumns () sectionTable->rowCount()" + to_string(sectionTable->rowCount()));
			for (int rowCount = 0; rowCount < sectionTable->rowCount(); rowCount++)
			{
				QComboBox* typeCombo;
				typeCombo = static_cast<QComboBox*>(sectionTable->cellWidget(rowCount, MATERIAL_TYPE_COLUMN)->children().last());
				if (typeCombo == nullptr)
					continue;
				QString matrialId = typeCombo->property("materialId").toString();
				Logger::Debug("AddNewBom -> UpdateColorwayColumns () matrialId" + matrialId.toStdString());

				Logger::Debug("AddNewBom -> UpdateColorwayColumns () sectionTable->columnCount()" + to_string(sectionTable->columnCount()));
				for (int columnIndex = 0; columnIndex < sectionTable->columnCount(); columnIndex++)
				{
					QString columnName = sectionTable->horizontalHeaderItem(columnIndex)->text();
					Logger::Debug("AddNewBom -> UpdateColorwayColumns1 () columnName" + columnName.toStdString());
					Logger::Debug("AddNewBom -> UpdateColorwayColumns1 () bomColorwayColumns" + bomColorwayColumns.join(',').toStdString());

					if (!bomColorwayColumns.contains(columnName))
					{
						if (FormatHelper::HasContent(matrialId.toStdString()))
						{

							//sectionTabl
							Logger::Debug("AddNewBom -> UpdateColorwayColumns () 2");
							if (m_mappedColorways.contains(columnName))
							{
								Logger::Debug("AddNewBom -> UpdateColorwayColumns () columnName" + columnName.toStdString());
								auto colorwayJsonItr = _colorwayMapForBom.find(matrialId.toStdString());
								if (colorwayJsonItr != _colorwayMapForBom.end())
								{
									json colorwayJson = colorwayJsonItr->second;
									Logger::Debug("AddNewBom -> UpdateColorwayColumns () colorwayJson" + to_string(colorwayJson));
									string colorwayNameStr = Helper::GetJSONValue<string>(colorwayJson, columnName.toStdString(), false);
									json colorwayNameJson1 = json::parse(colorwayNameStr);
									Logger::Debug("AddNewBom -> UpdateColorwayColumns () colorwayNameJson1" + to_string(colorwayNameJson1));
									string colorObjId = Helper::GetJSONValue<string>(colorwayNameJson1, "colorObjectId", true);
									Logger::Debug("AddNewBom -> UpdateColorwayColumns () colorObjId" + colorObjId);
									if (FormatHelper::HasContent(colorObjId))
									{
										//json dependentFieldJson = Helper::makeRestcallGet("csi-requesthandler/api/v2/color_specifications/", "&skip=0&limit=100", "" + colorId, "");
										string resultResponse = RESTAPI::CentricRestCallGet(Configuration::GetInstance()->GetPLMServerURL() + RESTAPI::SEARCH_COLOR_API + "/" + colorObjId, APPLICATION_JSON_TYPE, "");
										Logger::Debug("AddNewBom -> UpdateColorwayColumns() -> resultResponse" + resultResponse);
										json ColoreResultJson = json::parse(resultResponse);
										Logger::Debug("AddNewBom -> UpdateColorwayColumns() -> ColoreResultJson" + to_string(ColoreResultJson));
										string rgbValue = Helper::GetJSONValue<string>(ColoreResultJson, RGB_VALUE_KEY, true);
										string colorName = Helper::GetJSONValue<string>(ColoreResultJson, ATTRIBUTE_NAME, true);
										Logger::Debug("AddNewBom -> UpdateColorwayColumns() -> rgbValue" + rgbValue);

										rgbValue = Helper::FindAndReplace(rgbValue, "(", "");
										rgbValue = Helper::FindAndReplace(rgbValue, ")", "");
										rgbValue = Helper::FindAndReplace(rgbValue, " ", "");
										Logger::Debug("AddNewBom -> UpdateColorwayColumns() -> 3");
										if (FormatHelper::HasContent(rgbValue))
										{
											QStringList listRGB;
											QWidget* p_widget = new QWidget(sectionTable);
											QGridLayout* gridLayout = new QGridLayout(sectionTable);
											gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
											gridLayout->setContentsMargins(0, 0, 0, 0);
											QString colorRGB = QString::fromStdString(rgbValue);
											listRGB = colorRGB.split(',');
											int red = listRGB.at(0).toInt();
											int green = listRGB.at(1).toInt();
											int blue = listRGB.at(2).toInt();
											QColor color(red, green, blue);
											QImage image(20, 20, QImage::Format_ARGB32);
											image.fill(color);
											QPixmap pixmap;
											Logger::Debug("AddNewBom -> UpdateColorwayColumns() -> 4");
											QLabel* label = new QLabel();
											label->setToolTip(QString::fromStdString(colorName));
											pixmap = QPixmap::fromImage(image);
											label->setPixmap(QPixmap(pixmap));
											Logger::Debug("AddNewBom -> UpdateColorwayColumns() -> 5");

											QWidget *colorchip = nullptr;
											colorchip = CVWidgetGenerator::InsertWidgetInCenter(label);
											colorchip->setProperty("colorId", colorObjId.c_str());

											gridLayout->addWidget(colorchip, 0, 0, 1, 1, Qt::AlignHCenter);
											QPushButton* pushButton_2 = CVWidgetGenerator::CreatePushButton("", ADD_HOVER_ICON_PATH, "", PUSH_BUTTON_STYLE, 30, true);
											pushButton_2->setFixedHeight(20);
											pushButton_2->setFixedWidth(20);
											if (m_addColorButtonSignalMapper != nullptr)
											{
												m_addColorButtonSignalMapper->setProperty("TableName", tableName);
												//connect(pushButton_2, SIGNAL(clicked()), m_addColorButtonSignalMapper, SLOT(map()));
												//int number = rowCount * 10 + columnIndex;
												m_addColorButtonSignalMapper->setMapping(pushButton_2, QString("%1-%2-%3").arg(rowCount).arg(columnIndex).arg(tableName));
												//m_buttonSignalMapper->setMapping(pushButton_2, number);
											}
											pushButton_2->setProperty("TableName", tableName);
											gridLayout->addWidget(pushButton_2, 0, 1, 1, 1, Qt::AlignHCenter);
											p_widget->setLayout(gridLayout);
											p_widget->setProperty("colorId", colorObjId.c_str());
											sectionTable->setCellWidget(rowCount, columnIndex, p_widget);
											Logger::Debug("AddNewBom -> AddBomRows() -> 7");
											//colorChip = true;
										}
									}
								}
								else
								{
									if (m_mappedColorways.contains(columnName))
									{
										QWidget* p_widget = new QWidget(sectionTable);
										QGridLayout* gridLayout = new QGridLayout(sectionTable);
										gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
										gridLayout->setContentsMargins(0, 0, 0, 0);

										QPixmap pixmap;
										Logger::Debug("AddNewBom -> AddBomRows() -> 2");
										QLabel* label = new QLabel();

										QImage styleIcon;
										QImageReader imageReader(":/CLOVise/PLM/Images/NoImage.png");
										imageReader.setDecideFormatFromContent(true);
										styleIcon = imageReader.read();
										pixmap = QPixmap::fromImage(styleIcon);

										label->setMaximumSize(QSize(20, 20));
										int w = label->width();
										int h = label->height();
										label->setPixmap(QPixmap(pixmap.scaled(w, h, Qt::KeepAspectRatio)));
										Logger::Debug("AddNewBom -> AddBomRows() -> 3");
										QWidget *colorchip = nullptr;
										colorchip = CVWidgetGenerator::InsertWidgetInCenter(label);

										gridLayout->addWidget(colorchip, 0, 0, 1, 1, Qt::AlignHCenter);
										QPushButton* pushButton_2 = CVWidgetGenerator::CreatePushButton("", ADD_HOVER_ICON_PATH, "", PUSH_BUTTON_STYLE, 30, true);
										pushButton_2->setFixedHeight(20);
										pushButton_2->setFixedWidth(20);
										if (m_addColorButtonSignalMapper != nullptr)
										{
											m_addColorButtonSignalMapper->setProperty("TableName", tableName);
											//connect(pushButton_2, SIGNAL(clicked()), m_addColorButtonSignalMapper, SLOT(map()));
											//int number = rowCount * 10 + columnIndex;
											m_addColorButtonSignalMapper->setMapping(pushButton_2, QString("%1-%2-%3").arg(rowCount).arg(columnIndex).arg(tableName));
											//m_buttonSignalMapper->setMapping(pushButton_2, number);
										}
										pushButton_2->setProperty("TableName", tableName);
										gridLayout->addWidget(pushButton_2, 0, 1, 1, 1, Qt::AlignHCenter);
										p_widget->setLayout(gridLayout);

										Logger::Debug("AddNewBom -> AddBomRows() -> 10");
										sectionTable->setCellWidget(rowCount, columnIndex, p_widget);
									}
								}

							}

						}
						else
						{
							if (m_mappedColorways.contains(columnName))
							{
								QWidget* p_widget = new QWidget(sectionTable);
								QGridLayout* gridLayout = new QGridLayout(sectionTable);
								gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
								gridLayout->setContentsMargins(0, 0, 0, 0);

								QPixmap pixmap;
								Logger::Debug("AddNewBom -> AddBomRows() -> 2");
								QLabel* label = new QLabel();

								QImage styleIcon;
								QImageReader imageReader(":/CLOVise/PLM/Images/NoImage.png");
								imageReader.setDecideFormatFromContent(true);
								styleIcon = imageReader.read();
								pixmap = QPixmap::fromImage(styleIcon);

								label->setMaximumSize(QSize(20, 20));
								int w = label->width();
								int h = label->height();
								label->setPixmap(QPixmap(pixmap.scaled(w, h, Qt::KeepAspectRatio)));
								Logger::Debug("AddNewBom -> AddBomRows() -> 3");
								QWidget *colorchip = nullptr;
								colorchip = CVWidgetGenerator::InsertWidgetInCenter(label);

								gridLayout->addWidget(colorchip, 0, 0, 1, 1, Qt::AlignHCenter);
								QPushButton* pushButton_2 = CVWidgetGenerator::CreatePushButton("", ADD_HOVER_ICON_PATH, "", PUSH_BUTTON_STYLE, 30, true);
								pushButton_2->setFixedHeight(20);
								pushButton_2->setFixedWidth(20);
								if (m_addColorButtonSignalMapper != nullptr)
								{
									m_addColorButtonSignalMapper->setProperty("TableName", tableName);
									//connect(pushButton_2, SIGNAL(clicked()), m_addColorButtonSignalMapper, SLOT(map()));
									//int number = rowCount * 10 + columnIndex;
									m_addColorButtonSignalMapper->setMapping(pushButton_2, QString("%1-%2-%3").arg(rowCount).arg(columnIndex).arg(tableName));
									//m_buttonSignalMapper->setMapping(pushButton_2, number);
								}
								pushButton_2->setProperty("TableName", tableName);
								gridLayout->addWidget(pushButton_2, 0, 1, 1, 1, Qt::AlignHCenter);
								p_widget->setLayout(gridLayout);

								Logger::Debug("AddNewBom -> AddBomRows() -> 10");
								sectionTable->setCellWidget(rowCount, columnIndex, p_widget);
							}
						}
					}
				}

			}
		}

		Logger::Debug("AddNewBom -> UpdateColorwayColumns () End");
	}
	

	inline json AddMaterialInBom()
	{
		
			Logger::Debug("BOMUtility -> AddMaterialInBom() -> 1");
			json fieldsJson;
			Logger::Debug("BOMUtility -> AddMaterialInBom() -> 2");
			fieldsJson = MaterialConfig::GetInstance()->GetUpdateMaterialCacheData();
			Logger::Debug("BOMUtility -> AddMaterialInBom() -> fieldsJson" + to_string(fieldsJson));
			string code = Helper::GetJSONValue<string>(fieldsJson, "code", true);
			string objectId = Helper::GetJSONValue<string>(fieldsJson, "id", true);
			string name = Helper::GetJSONValue<string>(fieldsJson, "node_name", true);
			string materialType = Helper::GetJSONValue<string>(fieldsJson, "product_type", true);
			string description = Helper::GetJSONValue<string>(fieldsJson, "description", true);
			Logger::Debug("BOMUtility -> AddMaterialInBom() -> 3");
			json rowDataJson = json::object();
			rowDataJson["Code"] = code;
			rowDataJson["material_name"] = name;
			rowDataJson["Type"] = materialType;
			rowDataJson["comment"] = description;
			rowDataJson["qty_default"] = "";
			rowDataJson["uom"] = "";
			rowDataJson["materialId"] = objectId;
			Logger::Debug("BOMUtility -> AddMaterialInBom() -> rowDataJson" + to_string(rowDataJson));
			
		Logger::Debug("BOMUtility -> AddMaterialInBom() -> End");
		return rowDataJson;
	}

	
	inline void CreateBom(string _productId, json _BomMetaData, map<string, QTableWidget*> _bomSectionTableInfoMap, QStringList _mappedColorways, map<string, string> _CloAndPLMColorwayMap)
	{
		Logger::Debug("CreateProduct -> CreateBom() -> Start");
		vector<pair<string, string>> headerNameAndValueList;
		headerNameAndValueList.push_back(make_pair("content-Type", "application/json"));
		headerNameAndValueList.push_back(make_pair("Accept", "application/json"));
		headerNameAndValueList.push_back(make_pair("Cookie", Configuration::GetInstance()->GetBearerToken()));

		Logger::Debug("CreateProduct -> CreateBom() -> AddNewBom::GetInstance()->m_BomMetaData" + to_string(_BomMetaData));
		json bomData = _BomMetaData;
		string bomTemplateId = Helper::GetJSONValue<string>(bomData, "bom_template", true);
		bomData.erase("bom_template");
		bomData["style_id"] = _productId;
		string bomMetaData = to_string(bomData);
		//UTILITY_API->DisplayMessageBox(to_string(bomData));
		string response = REST_API->CallRESTPost(Configuration::GetInstance()->GetPLMServerURL() + RESTAPI::CREATE_BOM_API + "/" + bomTemplateId, &bomMetaData, headerNameAndValueList, "Loading");
		if (!FormatHelper::HasContent(response))
		{
			RESTAPI::SetProgressBarData(0, "", false);
			throw "Unable to publish to PLM. Please try again or Contact your System Administrator.";
		}

		if (FormatHelper::HasError(response))
		{
			RESTAPI::SetProgressBarData(0, "", false);
			//Logger::Debug("PublishToPLMData -> onPublishToPLMClicked 1");
			throw runtime_error(response);
		}
		Logger::Debug("CreateProduct -> CreateBom() -> response" + response);
		json bomJson = Helper::GetJsonFromResponse(response, "{");
		string bomLatestRevision = Helper::GetJSONValue<string>(bomJson, "latest_revision", true);

		if (FormatHelper::HasContent(bomLatestRevision))
		{
			string modifiedbyFlag = "{\"modified_by_application\":\"CLO3D\"}";
			Logger::Debug("AddNewBom -> CreateBom() -> modifiedbyFlag" + modifiedbyFlag);
			string resultJsonString = RESTAPI::PutRestCall(modifiedbyFlag, Configuration::GetInstance()->GetPLMServerURL() + RESTAPI::BOM_REVISION_API + "/" + bomLatestRevision, "content-type: application/json");
			Logger::Debug("CreateProduct -> CreateBom() -> resultJsonStringModifiedBy" + resultJsonString);
		}
		for (auto itr = _bomSectionTableInfoMap.begin(); itr != _bomSectionTableInfoMap.end(); itr++)
		{
			//AddNewBom::sectionInfo sectionInfoObj = itr->second;
			QTableWidget* sectionTable = itr->second;
			string sectionId = sectionTable->property("SectionId").toString().toStdString();
			for (int rowCount = 0; rowCount < sectionTable->rowCount(); rowCount++)
			{
				json attJson = json::object();
				string commonColorId;
				map<string, string> partMaterialColorsMap;
				for (int columnCount = 0; columnCount < sectionTable->columnCount(); columnCount++)
				{
					string fieldValue;

					QWidget* qcolumnWidget = (QWidget*)sectionTable->cellWidget(rowCount, columnCount)->children().last();
					string attInternalName = qcolumnWidget->property("rest_api_name").toString().toStdString();
					Logger::Debug("Create product CreateBom() attInternalName" + attInternalName);
					QString columnName = sectionTable->horizontalHeaderItem(columnCount)->text();
					Logger::Debug("Create product CreateBom() columnName" + columnName.toStdString());

					if (_mappedColorways.contains(columnName))
					{
						Logger::Debug("Create product CreateBom() colorways1");

						//QWidget *widget = gridLayout->itemAt(i)->widget();
						//auto gridLayout = dynamic_cast<QGridLayout*>(qcolumnWidget->layout());
						//QWidget *widget = gridLayout->itemAtPosition(0,0)->widget();
						//attInternalName = columnName.toStdString();
						//fieldValue = widget->property("colorId").toString().toStdString();
						//Logger::Debug("Create product CreateBom() colorId" + fieldValue);

						if (QWidget* widget = sectionTable->cellWidget(rowCount, columnCount))// Half cooked code for part material color
						{

							string colorId, colorId2;
							string colorId1 = widget->property("colorId").toString().toStdString();


							Logger::Debug("Create product CreateBom() colorId1" + colorId1);
							Logger::Debug("CreateProduct -> CreateBom () 8");
							if (QLayout* layout = widget->layout())
							{
								Logger::Debug("CreateProduct -> CreateBom () 9");
								{
									auto gridLayout = dynamic_cast<QGridLayout*>(widget->layout());
									QWidget *childwidget = gridLayout->itemAtPosition(0, 0)->widget();
									//attInternalName = columnName.toStdString();
									colorId2 = childwidget->property("colorId").toString().toStdString();
									Logger::Debug("Create product CreateBom() colorId2" + colorId2);

								}
							}

							if (FormatHelper::HasContent(colorId1))
								colorId = colorId1;
							else
								colorId = colorId2;

							if (FormatHelper::HasContent(colorId))
								partMaterialColorsMap.insert(make_pair(columnName.toStdString(), colorId));
						}

					}
					else if (QLineEdit* qLineEditC1 = qobject_cast<QLineEdit*>(qcolumnWidget))
					{

						fieldValue = qLineEditC1->text().toStdString();
						//QString columnName = sectionTable->horizontalHeaderItem(columnCount)->text();
					}
					else if (QTextEdit* qTextC1 = qobject_cast<QTextEdit*>(qcolumnWidget))
					{

						fieldValue = qTextC1->toPlainText().toStdString();
					}
					else if (QDateEdit* qDateC1 = qobject_cast<QDateEdit*>(qcolumnWidget))
					{
						fieldValue = FormatHelper::RetrieveDate(qDateC1);
						if (fieldValue.find(DATE_FORMAT_TEXT.toStdString()) != string::npos)
						{
							fieldValue = "";
						}
						else
						{
							//UTILITY_API->DisplayMessageBox("fieldValue::" + fieldValue);
							fieldValue = fieldValue + "T00:00:00Z";
						}
					}
					else if (QListWidget* listC1 = qobject_cast<QListWidget*>(qcolumnWidget))
					{
						string tempValue = "";
						QListWidgetItem* listItem = nullptr;
						for (int row = 0; row < listC1->count(); row++)
						{
							listItem = listC1->item(row);
							if (listItem->checkState())
							{
								tempValue = listItem->text().toStdString();
								if (FormatHelper::HasContent(tempValue))
								{
									tempValue = listC1->property(tempValue.c_str()).toString().toStdString();
									if (FormatHelper::HasContent(tempValue))
									{
										tempValue = tempValue + DELIMITER_NEGATION;
										fieldValue = fieldValue + tempValue;
									}
								}
							}
						}
					}
					else if (QPushButton* pushButton = qobject_cast<QPushButton*>(qcolumnWidget))
					{
						if (attInternalName == "Delete")
						{
							fieldValue = pushButton->property("materialId").toString().toStdString();
							commonColorId = pushButton->property("commonColorId").toString().toStdString();
							Logger::Debug("Create product CreateBom() QComboBox->materialId" + fieldValue);
							Logger::Debug("Create product CreateBom() QComboBox->commonColorId" + commonColorId);
							attInternalName = "actual";
						}
					}
					else if (QSpinBox* SpinC1 = qobject_cast<QSpinBox*>(qcolumnWidget))
					{
						if (SpinC1->value() != 0)
						{
							fieldValue = to_string(SpinC1->value());
						}
					}
					else if (QComboBox* qComboBoxC1 = qobject_cast<QComboBox*>(qcolumnWidget))
					{
						fieldValue = qComboBoxC1->currentText().toStdString();

						Logger::Debug("Create product CreateBom() QComboBox->fieldLabel" + attInternalName);
						//Logger::Debug("Create product ReadVisualUIFieldValue() QComboBox->labelText" + labelText);

						string fieldVal = qComboBoxC1->property(fieldValue.c_str()).toString().toStdString();
						Logger::Debug("Create product CreateBom() QComboBox->fieldVal" + fieldVal);
						if (!fieldVal.empty())
						{
							fieldValue = fieldVal;
						}
						Logger::Debug("Create product CreateBom() QComboBox->fieldValue" + fieldValue);
					}
					if (!attInternalName.empty() && !fieldValue.empty())
					{
						if (attInternalName == "qty_default")
							attJson[attInternalName] = atof(fieldValue.c_str());
						else
							attJson[attInternalName] = fieldValue;
					}
					Logger::Debug("Create product CreateBom() fieldValue" + fieldValue);
				}
				attJson["ds_section"] = sectionId;
				if (FormatHelper::HasContent(commonColorId))
					attJson["common_color"] = commonColorId;

				Logger::Debug("Create product CreateBom() attJson" + to_string(attJson));
				string materialId = Helper::GetJSONValue<string>(attJson, "actual", true);
				Logger::Debug("Create product CreateBom() materialId" + materialId);
				string materialType = Helper::GetJSONValue<string>(attJson, "Type", true);
				Logger::Debug("Create product CreateBom() materialType" + materialType);
				string materialName = Helper::GetJSONValue<string>(attJson, "material_name", true);
				Logger::Debug("Create product CreateBom() mayerialName" + materialName);
				if (FormatHelper::HasContent(bomLatestRevision))
				{
					string partMaterialResponse;
					if (FormatHelper::HasContent(materialId))
					{

						attJson.erase("Type");
						attJson.erase("material_name");
						attJson.erase("uom");
						string placementData = to_string(attJson);
						Logger::Debug("Create product CreateBom() placementData" + placementData);
						partMaterialResponse = RESTAPI::PostRestCall(placementData, Configuration::GetInstance()->GetPLMServerURL() + RESTAPI::BOM_REVISION_API_V3 + "/" + bomLatestRevision + "/items/part_materials", "content-type: application/json; boundary=----WebKitFormBoundary7MA4YWxkTrZu0gW");
						//response = REST_API->CallRESTPost(Configuration::GetInstance()->GetPLMServerURL() + RESTAPI::BOM_REVISION_API_V3 + "/" + bomLatestRevision + "/items/part_materials", &placementData, headerNameAndValueList, "Loading");
						Logger::Debug("Create product CreateBom() response material" + partMaterialResponse);

					}
					else
					{
						attJson.erase("Type");
						attJson.erase("material_name");
						attJson.erase("actual");
						attJson.erase("uom");
						string placementData = to_string(attJson);

						string queryParam;
						if (FormatHelper::HasContent(materialType))
							queryParam = queryParam + "material_type=" + materialType;
						if (FormatHelper::HasContent(queryParam))
							queryParam = "?" + queryParam;
						if (FormatHelper::HasContent(materialName))
						{
							materialName = QString::fromStdString(materialName).replace(" ", "%20").toStdString();
							queryParam = queryParam + "&material_name=" + materialName;
						}
						string api = Configuration::GetInstance()->GetPLMServerURL() + RESTAPI::BOM_REVISION_API_V3 + "/" + bomLatestRevision + "/items/special_part_materials" + queryParam;
						Logger::Debug("Create product CreateBom() queryParam" + queryParam);
						Logger::Debug("Create product CreateBom() placementData" + placementData);
						Logger::Debug("Create product CreateBom() queryParam" + api);

						partMaterialResponse = RESTAPI::PostRestCall(placementData, api, "content-type: application/json; boundary=----WebKitFormBoundary7MA4YWxkTrZu0gW");
						//response = REST_API->CallRESTPost(api, &placementData, headerNameAndValueList, "Loading");
						Logger::Debug("Create product CreateBom() response Special" + partMaterialResponse);

					}

					json detailJson = Helper::GetJsonFromResponse(partMaterialResponse, "{");

					Logger::Debug("Create product CreateBom() detailJson" + to_string(detailJson));

					string partMaterialId = Helper::GetJSONValue<string>(detailJson, ATTRIBUTE_ID, true);
					Logger::Debug("Create product CreateBom() partMaterialId" + partMaterialId);
					if (FormatHelper::HasContent(partMaterialId) && partMaterialColorsMap.size())
					{
						json partMaterailColorJson = Helper::GetJSONParsedValue<string>(detailJson, "part_material_colors", false);
						//	json partMaterailColorJson = Helper::GetJSONParsedValue<string>(partMaterialResponse, "part_material_colors", false);
						string partMaterailColorIds;
						for (int partMaterailColorCount = 0; partMaterailColorCount < partMaterailColorJson.size(); partMaterailColorCount++)
						{
							string partMaterailColorId = Helper::GetJSONValue<int>(partMaterailColorJson, partMaterailColorCount, true);
							Logger::Debug("CreateProduct -> CreateBom() -> apiMetadataStr" + sectionId);
							partMaterailColorIds += "id=" + partMaterailColorId + "&";
						}
						partMaterailColorIds = partMaterailColorIds.substr(0, partMaterailColorIds.length() - 1);

						string partMaterialDefinitions = RESTAPI::CentricRestCallGet(Configuration::GetInstance()->GetPLMServerURL() + RESTAPI::PART_MATERIAL_COLOR_API + "?" + partMaterailColorIds + "&limit=1000", APPLICATION_JSON_TYPE, "");
						Logger::Debug("CreateProduct -> CreateBom() -> partMaterialDefinitions" + partMaterialDefinitions);

						json partMaterialDefinitionsJson = json::parse(partMaterialDefinitions);

						for (int partMaterailColorCount = 0; partMaterailColorCount < partMaterialDefinitionsJson.size(); partMaterailColorCount++)
						{
							Logger::Debug("CreateProduct -> CreateBom() -> 1");
							json partMaterailColor = Helper::GetJSONParsedValue<int>(partMaterialDefinitionsJson, partMaterailColorCount, false);;///use new method
							Logger::Debug("CreateProduct -> CreateBom() -> 1");
							Logger::Debug("CreateProduct -> CreateBom() -> sectionCountJson" + to_string(partMaterailColor));
							string partMaterailColorId = Helper::GetJSONValue<string>(partMaterailColor, ATTRIBUTE_ID, true);
							Logger::Debug("CreateProduct -> CreateBom() -> sectionId" + partMaterailColorId);
							string partMaterailColorName = Helper::GetJSONValue<string>(partMaterailColor, "node_name", true);
							Logger::Debug("CreateProduct -> CreateBom()-> partMaterailColorName" + partMaterailColorName);
							Logger::Debug("CreateProduct -> CreateBom()-> partMaterialColorsMap.size()" + partMaterialColorsMap.size());
							auto it = _CloAndPLMColorwayMap.find(partMaterailColorName);
							if (it != _CloAndPLMColorwayMap.end())
							{

								string cloColorwayName = it->second;
								auto itr = partMaterialColorsMap.find(cloColorwayName);
								if (itr != partMaterialColorsMap.end())
								{
									string colorid = itr->second;
									Logger::Debug("CreateProduct -> CreateBom()-> colorid" + colorid);
									if (FormatHelper::HasContent(colorid))
									{
										string data = "{\"pmc_color\":\"" + colorid + "\"}";
										Logger::Debug("AddNewBom -> CreateTableforEachSection() -> data" + data);
										string resultJsonString = RESTAPI::PutRestCall(data, Configuration::GetInstance()->GetPLMServerURL() + RESTAPI::PART_MATERIAL_COLOR_API + "/" + partMaterailColorId, "content-type: application/json");

									}
								}
							}

						}

					}

				}

				//UTILITY_API->DisplayMessageBox("attJson" + to_string(attJson));
			}



		}
		Logger::Debug("CreateProduct -> CreateBom() -> End");
	}

	inline void ClearBomSectionLayout(QVBoxLayout *_sectionLayout)
	{
		Logger::Debug("CreateProduct -> ClearBomSectionLayout -> Start");
		while (_sectionLayout->count() > 0)
		{
			Logger::Debug("CreateProduct -> ClearBomSectionLayout -> 2");
			QWidget *item = _sectionLayout->itemAt(0)->widget();
			Logger::Debug("CreateProduct -> ClearBomSectionLayout -> 3");
			if (item != nullptr)
				delete item;
			Logger::Debug("CreateProduct -> ClearBomSectionLayout -> 4");
		}

		Logger::Debug("CreateProduct -> ClearBomSectionLayout -> End");
	}
	inline map<string, json> BackupBomDetails(map<string, QTableWidget*> _bomSectionTableInfoMap, QStringList _mappedColorways)
	{
		Logger::Debug("AddNewBom -> BackupBomDetails() -> Start");
		map<string, json> backupBomDataMap;
			for (auto itr = _bomSectionTableInfoMap.begin(); itr != _bomSectionTableInfoMap.end(); itr++)
			{
				//sectionInfo sectionInfoObj = 
				QTableWidget* sectionTable = itr->second;
				string sectionId = sectionTable->property("SectionId").toString().toStdString();
				for (int rowCount = 0; rowCount < sectionTable->rowCount(); rowCount++)
				{
					json attJson = json::object();
					string isRowAddedByUser;
					string commonColorId;
					for (int columnCount = 0; columnCount < sectionTable->columnCount(); columnCount++)
					{
						string fieldValue;

						QWidget* qcolumnWidget = (QWidget*)sectionTable->cellWidget(rowCount, columnCount)->children().last();
						string attInternalName = qcolumnWidget->property("rest_api_name").toString().toStdString();
						Logger::Debug("Create product CreateBom() attInternalName" + attInternalName);
						QString columnName = sectionTable->horizontalHeaderItem(columnCount)->text();
						Logger::Debug("Create product CreateBom() columnName" + columnName.toStdString());

						if (_mappedColorways.contains(columnName))
						{
							Logger::Debug("Create product CreateBom() colorways1");

							if (QWidget* widget = sectionTable->cellWidget(rowCount, columnCount))
							{

								string colorId, colorId2;
								string colorId1 = widget->property("colorId").toString().toStdString();


								Logger::Debug("Create product CreateBom() colorId1" + colorId1);
								Logger::Debug("CreateProduct -> CreateBom () 8");
								if (QLayout* layout = widget->layout())
								{
									Logger::Debug("CreateProduct -> CreateBom () 9");
									{
										auto gridLayout = dynamic_cast<QGridLayout*>(widget->layout());
										QWidget *childwidget = gridLayout->itemAtPosition(0, 0)->widget();
										//attInternalName = columnName.toStdString();
										colorId2 = childwidget->property("colorId").toString().toStdString();
										Logger::Debug("Create product CreateBom() colorId2" + colorId2);

									}
								}

								if (FormatHelper::HasContent(colorId1))
									colorId = colorId1;
								else
									colorId = colorId2;

								attInternalName = columnName.toStdString();
								fieldValue = colorId;
							}

						}
						else if (QLineEdit* qLineEditC1 = qobject_cast<QLineEdit*>(qcolumnWidget))
						{

							fieldValue = qLineEditC1->text().toStdString();
							//QString columnName = sectionTable->horizontalHeaderItem(columnCount)->text();
						}
						else if (QTextEdit* qTextC1 = qobject_cast<QTextEdit*>(qcolumnWidget))
						{

							fieldValue = qTextC1->toPlainText().toStdString();
						}
						else if (QDateEdit* qDateC1 = qobject_cast<QDateEdit*>(qcolumnWidget))
						{
							fieldValue = FormatHelper::RetrieveDate(qDateC1);
							if (fieldValue.find(DATE_FORMAT_TEXT.toStdString()) != string::npos)
							{
								fieldValue = "";
							}
							else
							{
								//UTILITY_API->DisplayMessageBox("fieldValue::" + fieldValue);
								fieldValue = fieldValue + "T00:00:00Z";
							}
						}
						else if (QListWidget* listC1 = qobject_cast<QListWidget*>(qcolumnWidget))
						{
							string tempValue = "";
							QListWidgetItem* listItem = nullptr;
							for (int row = 0; row < listC1->count(); row++)
							{
								listItem = listC1->item(row);
								if (listItem->checkState())
								{
									tempValue = listItem->text().toStdString();
									if (FormatHelper::HasContent(tempValue))
									{
										tempValue = listC1->property(tempValue.c_str()).toString().toStdString();
										if (FormatHelper::HasContent(tempValue))
										{
											tempValue = tempValue + DELIMITER_NEGATION;
											fieldValue = fieldValue + tempValue;
										}
									}
								}
							}
						}
						else if (QPushButton* pushButton = qobject_cast<QPushButton*>(qcolumnWidget))
						{
							if (attInternalName == "Delete")
							{
								fieldValue = pushButton->property("materialId").toString().toStdString();
								commonColorId = pushButton->property("commonColorId").toString().toStdString();
								isRowAddedByUser = pushButton->property("RowAddedByUser").toString().toStdString();
								Logger::Debug("AddNewBom BackupBomDetails() isRowAddedByUser" + isRowAddedByUser);
								attInternalName = "materialId";
							}
						}
						else if (QSpinBox* SpinC1 = qobject_cast<QSpinBox*>(qcolumnWidget))
						{
							if (SpinC1->value() != 0)
							{
								fieldValue = to_string(SpinC1->value());
							}
						}
						else if (QComboBox* qComboBoxC1 = qobject_cast<QComboBox*>(qcolumnWidget))
						{
							fieldValue = qComboBoxC1->currentText().toStdString();

							Logger::Debug("AddNewBom BackupBomDetails() QComboBox->fieldLabel" + attInternalName);
							//Logger::Debug("Create product ReadVisualUIFieldValue() QComboBox->labelText" + labelText);

							string fieldVal = qComboBoxC1->property(fieldValue.c_str()).toString().toStdString();
							Logger::Debug("Create product BackupBomDetails() QComboBox->fieldVal" + fieldVal);
							if (!fieldVal.empty())
							{
								fieldValue = fieldVal;
							}
							Logger::Debug("AddNewBom BackupBomDetails() QComboBox->fieldValue" + fieldValue);
						}
						if (!attInternalName.empty() && !fieldValue.empty())
						{
							if (attInternalName == "qty_default")
								attJson[attInternalName] = atoi(fieldValue.c_str());
							else
								attJson[attInternalName] = fieldValue;
						}
						Logger::Debug("AddNewBom BackupBomDetails() fieldValue" + fieldValue);
					}
					attJson["ds_section"] = sectionId;
					attJson["common_color"] = commonColorId;

					if (isRowAddedByUser == "true")
						backupBomDataMap.insert(make_pair(itr->first, attJson));
					Logger::Debug("AddNewBom BackupBomDetails() attJson" + to_string(attJson));


					//UTILITY_API->DisplayMessageBox("attJson" + to_string(attJson));
				}
				//sectionTable->model()->removeRows(0, sectionTable->rowCount());
				sectionTable->clearContents();
				sectionTable->setRowCount(0);
				//sectionTable->clear();

			}
			Logger::Debug("AddNewBom -> BackupBomDetails() -> End");
			return backupBomDataMap;
	}
}
