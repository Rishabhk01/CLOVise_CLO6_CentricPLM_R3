/*
* Copyright 2021-2022 CLO-Vise. All rights reserved
*
* @file CreateProductBOMHandler.cpp
*
* @brief Class implementation for create Bom table on tab.
* This class has all the variable and methods implementation which are used to create bom table and publish bom lines from CLO to PLM.
*
* @author GoVise
*
* @date 18-JAN-2021
*/
#include "CreateProductBOMHandler.h"

#include <cstring>

#include <QTextEdit>
#include <QComboBox>
#include <QLabel>
#include "qtreewidget.h"
#include <QFile>
#include "qdir.h"

#include "CLOAPIInterface.h"
#include "CLOVise/PLM/Helper/Util/Helper.h"
#include "CLOVise/PLM/Helper/Util/Configuration.h"
#include "CLOVise/PLM/Inbound/Document/DocumentConfig.h"
#include "CLOVise/PLM/Helper/Util/FlexTypeHelper.h"
#include "CLOVise/PLM/Helper/UIHelper/UIHelper.h"
#include "CLOVise/PLM/Helper/Util/Logger.h"
#include "CLOVise/PLM/CLOViseSuite/CLOViseSuite.h"
#include "CLOVise/PLM/Helper/UIHelper/CVHoverDelegate.h"
#include "CLOVise/PLM/Inbound/Color/PLMColorSearch.h"
#include "CLOVise/PLM/Inbound/Color/ColorConfig.h"
#include "CLOVise/PLM/Outbound/PublishToPLM/Section.h"
#include "CLOVise/PLM/Outbound/PublishToPLM/CreateProduct.h"
#include "CLOVise/PLM/Inbound/Material/PLMMaterialSearch.h"
#include "CLOVise/PLM/Inbound/Material/MaterialConfig.h"

using namespace std;

namespace CLOVise
{
	CreateProductBOMHandler* CreateProductBOMHandler::_instance = NULL;

	CreateProductBOMHandler* CreateProductBOMHandler::GetInstance()
	{
		if (_instance == NULL) {
			_instance = new CreateProductBOMHandler();
		}

		return _instance;
	}

	/*Description - CreateBom(json) method used to create BOM on bom tab in create style
		* Parameter -json
		*Exception -
		*Return -
		*/
	void CreateProductBOMHandler::CreateBom(json _sectionIdsJson)
	{
		//json _sectionIdsjson;
		BOMUtility::GetCentricMaterialTypes();
		readBomTableColumnJson();
		BOMUtility::CreateTableforEachSection(CreateProduct::GetInstance()->ui_sectionLayout, _sectionIdsJson, m_bomSectionTableInfoMap, m_bomSectionNameAndTypeMap, m_sectionMaterialTypeMap, m_addMaterialButtonAndTableMap, m_addSpecialMaterialButtonAndTableMap, CreateProduct::GetInstance()->m_mappedColorways, CreateProduct::GetInstance()->m_currentlySelectedStyleTypeId, m_bomTableColumnlist, m_bomTableColumnKeys);

		Logger::Debug("CreateProductBOMHandler -> CreateBom() -> m_bomSectionTableInfoMap" + to_string(m_bomSectionTableInfoMap.size()));

		populateTechPackDataInBom();

		for (auto itr = m_bomSectionTableInfoMap.begin(); itr != m_bomSectionTableInfoMap.end(); itr++)
		{
			Logger::Debug("CreateProductBOMHandler -> CreateBom loop");
			QTableWidget* key = itr->second;
			Logger::Debug("CreateProductBOMHandler -> CreateBom () m_addMaterialButtonAndTableMap.size()" + to_string(m_addMaterialButtonAndTableMap.size()));
#ifdef __APPLE__
			auto result = std::find_if(std::begin(m_addMaterialButtonAndTableMap), std::end(m_addMaterialButtonAndTableMap), [&](const std::pair<QPushButton*, QTableWidget*> &pair) { return pair.second == key; });
#else
			auto result = std::find_if(m_addMaterialButtonAndTableMap.begin(), m_addMaterialButtonAndTableMap.end(), [key](const auto& mo) {return mo.second == key; });
#endif

#ifdef __APPLE__
			auto result1 = std::find_if(std::begin(m_addSpecialMaterialButtonAndTableMap), std::end(m_addSpecialMaterialButtonAndTableMap), [&](const std::pair<QPushButton*, QTableWidget*> &pair) { return pair.second == key; });
#else
			auto result1 = std::find_if(m_addSpecialMaterialButtonAndTableMap.begin(), m_addSpecialMaterialButtonAndTableMap.end(), [key](const auto& mo) {return mo.second == key; });
#endif
			Logger::Debug("CreateProductBOMHandler -> CreateBom () 2");

			QPushButton* materialbutton = result->first;
			if (materialbutton != nullptr)
				connect(materialbutton, SIGNAL(clicked()), this, SLOT(onClickAddFromMaterialButton()));

			QPushButton* specialbutton = result1->first;
			if (specialbutton != nullptr)
				connect(specialbutton, SIGNAL(clicked()), this, SLOT(onClickAddSpecialMaterialButton()));
		}

	}
	CreateProductBOMHandler::CreateProductBOMHandler(QObject* parent)
	{
		m_addColorButtonSignalMapper = new QSignalMapper();
		m_deleteButtonSignalMapper = new QSignalMapper();
		m_materialTypeNameIdMap = BOMUtility::GetCentricMaterialTypes();
		connectSignalSlots(true);
		m_bomCreated = false;

	}


	/*Description - IsBomCreated() method is to get information BOM created or not
		* Parameter -
		*Exception -
		*Return -
		*/
	bool CreateProductBOMHandler::IsBomCreated()
	{
		return m_bomCreated;
	}


	/* Description - connectSignalSlots() method used to connect and disconnect signal and slots.
	* Parameter -  bool.
	* Exception -
	* Return -
	*/
	void CreateProductBOMHandler::connectSignalSlots(bool _b)
	{

		if (_b)
		{


			QObject::connect(m_addColorButtonSignalMapper, SIGNAL(mapped(const QString &)), this, SLOT(OnClickAddColorButton(const QString &)));
			QObject::connect(m_deleteButtonSignalMapper, SIGNAL(mapped(const QString &)), this, SLOT(OnClickDeleteButton(const QString &)));

		}
		else
		{
			QObject::disconnect(m_addColorButtonSignalMapper, SIGNAL(mapped(const QString &)), this, SLOT(OnClickAddColorButton(const QString &)));
			QObject::disconnect(m_deleteButtonSignalMapper, SIGNAL(mapped(const QString &)), this, SLOT(OnClickDeleteButton(const QString &)));
		}
	}

	/* Description - readBomTableColumnJson() method used read json file to get bom table columns
	* Parameter -
	* Exception -
	* Return -
	*/
	void CreateProductBOMHandler::readBomTableColumnJson()
	{
		m_bomTableColumnlist.clear();
		m_bomTableColumnKeys.clear();
		json bomConfigjson = json::object();
		string attJson = DirectoryUtil::GetPLMPluginDirectory() + "BomConfig.json";//Reading Columns from json
		bomConfigjson = Helper::ReadJSONFile(attJson);
		m_bomTableColumnJson = json::object();
		m_bomTableColumnJson = Helper::GetJSONParsedValue<string>(bomConfigjson, "BomTableColumns", false);
		for (int i = 0; i < m_bomTableColumnJson.size(); i++)
		{
			json fieldsJson = Helper::GetJSONParsedValue<int>(m_bomTableColumnJson, i, false);
			string displayName = Helper::GetJSONValue<string>(fieldsJson, "display_name", true);
			string internalName = Helper::GetJSONValue<string>(fieldsJson, "internal_name", true);
			m_bomTableColumnlist.append(QString::fromStdString(displayName));
			m_bomTableColumnKeys.append(QString::fromStdString(internalName));
		}
	}


	/*
Description - OnClickDeleteButton() method used to delete a bom line from table.
	* Parameter -QString
	* Exception -
	*Return -
	*/
	void CreateProductBOMHandler::OnClickDeleteButton(const QString &position)
	{
		Logger::Debug("CreateProductBOMHandler -> OnClickDeleteButton () Start");
		QSignalMapper* button = (QSignalMapper*)sender();
		//	QPushButton* button = (QPushButton*)sender();
		QStringList coordinates = position.split("-");
		int _rowCount = coordinates[0].toInt();
		string Tablename = coordinates[1].toStdString();


		string tableName = button->property("TableName").toString().toStdString();

		Logger::Debug("CreateProductBOMHandler -> OnClickDeleteButton () tableName" + Tablename);
		auto itr = m_bomSectionTableInfoMap.find(Tablename);
		if (itr != m_bomSectionTableInfoMap.end())
		{
			QTableWidget* sectionTable = itr->second;


			sectionTable->removeRow(_rowCount);
			if (m_deleteButtonSignalMapper != nullptr)
			{
				for (int i = 0; i < sectionTable->rowCount(); i++)
				{
					QPushButton *deleteButton = static_cast<QPushButton*>(sectionTable->cellWidget(i, DELETE_BUTTON_COLUMN)->children().last());
					if (deleteButton != nullptr)
						m_deleteButtonSignalMapper->setMapping(deleteButton, QString("%1-%2").arg(i).arg(coordinates[1]));
				}
			}

			for (int i = 0; i < sectionTable->rowCount(); i++)
			{
				QWidget* widget = sectionTable->cellWidget(i, 5);
				if (QLayout* layout = widget->layout())
				{
					Logger::Debug("CreateProductBOMHandler -> OnClickDeleteButton () 10");
					{
						auto gridLayout = dynamic_cast<QGridLayout*>(widget->layout());
						if (gridLayout != nullptr)
						{
							Logger::Debug("CreateProductBOMHandler -> OnClickDeleteButton () 12");
							QWidget* widget1 = gridLayout->itemAtPosition(0, 0)->widget();
							QPushButton *addButton = static_cast<QPushButton*>(widget1->children().last());
							Logger::Debug("CreateProductBOMHandler -> OnClickDeleteButton () 13");
							if (m_addColorButtonSignalMapper != nullptr && addButton != nullptr)
							{
								Logger::Debug("CreateProductBOMHandler -> OnClickDeleteButton () 14");
								m_addColorButtonSignalMapper->setMapping(addButton, QString("%1-%2-%3").arg(i).arg(5).arg(QString::fromStdString(Tablename)));
							}

							Logger::Debug("CreateProductBOMHandler -> OnClickDeleteButton () 15");
						}

					}
				}
			}


			int columnCount = sectionTable->columnCount();
			int columnInJson = m_bomTableColumnlist.size();
			for (int row = 0; row < sectionTable->rowCount(); row++)
			{
				for (int col = columnInJson; col < (columnCount - columnInJson); col++)
				{
					QWidget* widget = sectionTable->cellWidget(row, col);
					if (QLayout* layout = widget->layout())
					{
						Logger::Debug("CreateProductBOMHandler -> OnClickDeleteButton () 1");
						{
							auto gridLayout = dynamic_cast<QGridLayout*>(widget->layout());
							if (gridLayout != nullptr)
							{
								Logger::Debug("CreateProductBOMHandler -> OnClickDeleteButton () 2");
								QWidget* widget1 = gridLayout->itemAtPosition(0, 0)->widget();
								QPushButton *addButton = static_cast<QPushButton*>(widget1->children().last());
								Logger::Debug("CreateProductBOMHandler -> OnClickDeleteButton () 3");
								if (m_addColorButtonSignalMapper != nullptr && addButton != nullptr)
								{
									Logger::Debug("CreateProductBOMHandler -> OnClickDeleteButton () 4");
									m_addColorButtonSignalMapper->setMapping(addButton, QString("%1-%2-%3").arg(row).arg(col).arg(QString::fromStdString(Tablename)));
								}

								Logger::Debug("CreateProductBOMHandler -> OnClickDeleteButton () 5");
							}

						}
					}
				}
			}


		}

		Logger::Debug("CreateProductBOMHandler -> OnClickDeleteButton () End");
	}


	/*
* Description - onClickAddFromMaterialButton() method is to add inLibrary material in bom table
* Parameter -
* Exception -
* Return -
*/
	void CreateProductBOMHandler::onClickAddFromMaterialButton()
	{
		Logger::Debug("CreateProductBOMHandler -> onClickAddFromMaterialButton () Start");
		QPushButton* button = (QPushButton*)sender();
		QTableWidget* sectionTable;
		m_currentAddMaterialButtonClicked = button;
		auto it = m_addMaterialButtonAndTableMap.find(button);
		if (it != m_addMaterialButtonAndTableMap.end())
		{
			sectionTable = it->second;
			QString tableName = sectionTable->property("TableName").toString();
			auto itr = m_bomSectionTableInfoMap.find(tableName.toStdString());
			json placementProductTypeJson;
			string queryParamsForMaterial = "";

			if (itr != m_bomSectionTableInfoMap.end() && tableName != "Blank")
			{
				placementProductTypeJson = BOMUtility::GetMaterialTypeForSection(m_bomSectionNameAndTypeMap, tableName.toStdString());
				for (int i = 0; i < placementProductTypeJson.size(); i++)
				{
					string bomPlacementProductTypeId = Helper::GetJSONValue<int>(placementProductTypeJson, i, true);
					queryParamsForMaterial = queryParamsForMaterial + "&product_type=" + bomPlacementProductTypeId;

				}

			}
			Configuration::GetInstance()->SetQueryParameterForMaterial(queryParamsForMaterial);
		}
		Logger::Debug("CreateProductBOMHandler -> onClickAddFromMaterialButton () button" + to_string(long(button)));
		CreateProduct::GetInstance()->hide();
		UTILITY_API->CreateProgressBar();
		Configuration::GetInstance()->SetProgressBarProgress(qrand() % 101);
		//		MaterialConfig::GetInstance()->SetMaximumLimitForMaterialResult();
		RESTAPI::SetProgressBarData(Configuration::GetInstance()->GetProgressBarProgress(), "Loading " + Configuration::GetInstance()->GetLocalizedMaterialClassName() + " Search", true);
		int isFromConstructor = false;
		if (!MaterialConfig::GetInstance()->GetIsModelExecuted())
		{
			MaterialConfig::GetInstance()->InitializeMaterialData();
			isFromConstructor = true;
		}
		Logger::Debug("CreateProductBOMHandler -> onClickAddFromMaterialButton () End");
		PLMMaterialSearch::GetInstance()->DrawSearchWidget(isFromConstructor);
		Logger::Debug("CreateProductBOMHandler -> onClickAddFromMaterialButton () End");
		MaterialConfig::GetInstance()->SetIsModelExecuted(true);
		Logger::Debug("CreateProductBOMHandler -> onClickAddFromMaterialButton () End");
		MaterialConfig::GetInstance()->SetIsRadioButton(true);
		Logger::Debug("CreateProductBOMHandler -> onClickAddFromMaterialButton () End");
		PLMMaterialSearch::GetInstance()->setModal(true);
		Logger::Debug("CreateProductBOMHandler -> onClickAddFromMaterialButton () End");
		UTILITY_API->DeleteProgressBar(true);
		PLMMaterialSearch::GetInstance()->exec();
		Logger::Debug("CreateProductBOMHandler -> onClickAddFromMaterialButton () End");

		Logger::Debug("CreateProductBOMHandler -> onClickAddFromMaterialButton () End");
	}


	/*
	Description - UpdateColorwayColumns() method used to add color to the colorways.
		* Parameter -QString
		* Exception -
		*Return -
		*/
	void CreateProductBOMHandler::OnClickAddColorButton(const QString &position)
	{
		Logger::Debug("CreateProductBOMHandler -> OnClickAddColorButton () Start");
		CreateProduct::GetInstance()->hide();
		QSignalMapper* button = (QSignalMapper*)sender();
		Logger::Debug("CreateProductBOMHandler -> OnClickAddColorButton () string" + position.toStdString());
		QStringList coordinates = position.split("-");
		int row = coordinates[0].toInt();
		int col = coordinates[1].toInt();
		string tableName = coordinates[2].toStdString();
		Logger::Debug("CreateProductBOMHandler -> OnClickAddColorButton () row" + to_string(row));
		Logger::Debug("CreateProductBOMHandler -> OnClickAddColorButton () column" + to_string(col));

		m_currentTableName = tableName;
		m_currentRow = row;
		m_currentColumn = col;
		Logger::Debug("CreateProductBOMHandler -> OnClickAddColorButton () tableName" + m_currentTableName);


		Configuration::GetInstance()->SetIsUpdateColorClicked(true);


		ColorConfig::GetInstance()->m_mode = "Search";

		bool  isFromConstructor = false;
		if (!ColorConfig::GetInstance()->GetIsModelExecuted())
		{
			ColorConfig::GetInstance()->InitializeColorData();
			isFromConstructor = true;
		}

		ColorConfig::GetInstance()->InitializeColorData();
		ColorConfig::GetInstance()->m_isSearchColor = true;
		PLMColorSearch::GetInstance()->setModal(true);
		PLMColorSearch::GetInstance()->DrawSearchWidget(true);
		UTILITY_API->DeleteProgressBar(true);
		ColorConfig::GetInstance()->SetIsModelExecuted(true);

		PLMColorSearch::GetInstance()->exec();
		RESTAPI::SetProgressBarData(0, "", false);

		Logger::Debug("CreateProductBOMHandler -> OnClickAddColorButton () End");

	}


	/*
	Description - populateTechPackDataInBom() method used read tech pack and show data on bom table
		* Parameter -QString
		* Exception -
		*Return -
		*/
	void CreateProductBOMHandler::populateTechPackDataInBom()
	{

		getMaterialDetails("fabricList", Configuration::GetInstance()->GetTechPackJson(), true);
		getMaterialDetails("buttonHeadList", Configuration::GetInstance()->GetTechPackJson(), false);
		getMaterialDetails("buttonHoleList", Configuration::GetInstance()->GetTechPackJson(), false);
		getMaterialDetails("zipperList", Configuration::GetInstance()->GetTechPackJson(), false);

	}

	/*
* Description - getMaterialDetails() method is to populate CLO techpack data in Bom table.
* Parameter -string , json , bool , QTableWidget* , QString , bool
* Exception -
* Return -
*/
	void CreateProductBOMHandler::getMaterialDetails(string _str, json _techPackJson, bool _isFabric)
	{

		string fabStrValue = _techPackJson[_str].dump();
		string materialCount, cwlStrVal, costInfoStrVal;
		json fabJson = json::parse(fabStrValue);
		json bomLineJson;
		string Id, Name, costVal, quantityVal, apiMetaDataVal, costUnitVal, quantityUnitVal;
		int cwlIndex = 0;
		int count = 0;
		json placementProductTypeJson;

		string code, name, materialType, description, objectId = "";
		for (int fabCount = 0; fabCount < fabJson.size(); fabCount++)
		{

			materialCount = fabJson[fabCount].dump();
			// Utility::InfoLogger("getFabricTrimDetails ::: JSON :: " + materialCount);
			json materialCountJson = json::parse(materialCount);
			cwlStrVal = materialCountJson["colorwayList"][cwlIndex].dump();
			json colorwayListJson = json::parse(cwlStrVal);
			json rowDataJson = json::object();
			QString tableName;
			QTableWidget* table = nullptr;
			if (materialCountJson.contains("apiMetaData"))// For In Library Materials
			{

				string apiMetadataStr = Helper::GetJSONValue<string>(materialCountJson, "apiMetaData", false);
				json apimetaDataJson = json::parse(apiMetadataStr);

				code = Helper::GetJSONValue<string>(apimetaDataJson, "Code", true);
				//string dataType = Helper::GetJSONValue<string>(apimetaDataJson, "Data Type", true);
				name = Helper::GetJSONValue<string>(apimetaDataJson, "Name", true);
				materialType = Helper::GetJSONValue<string>(apimetaDataJson, "Type", true);
				description = Helper::GetJSONValue<string>(apimetaDataJson, "Description", true);
				objectId = Helper::GetJSONValue<string>(apimetaDataJson, "objectId", true);

				Logger::Debug("BomUtility -> getMaterialDetails() -> materialType: " + materialType);

				BOMUtility::getColorInfo(materialCountJson, rowDataJson, objectId, _isFabric, m_colorwayMapForBom);
				Logger::Debug("BomUtility -> getMaterialDetails() -> m_materialTypeNameIdMap: " + to_string(m_materialTypeNameIdMap.size()));
#ifdef __APPLE__
				auto result = std::find_if(std::begin(m_materialTypeNameIdMap), std::end(m_materialTypeNameIdMap), [&](const std::pair<string, string> &pair) { return pair.second == materialType; });
#else
				auto result = std::find_if(m_materialTypeNameIdMap.begin(), m_materialTypeNameIdMap.end(), [materialType](const auto& mo) {return mo.second == materialType; });
#endif

				if (result != m_materialTypeNameIdMap.end())
				{
					Logger::Debug("BomUtility -> getMaterialDetails() -> label: " + result->first);
					Logger::Debug("BomUtility -> getMaterialDetails() -> m_sectionMaterialTypeMap: " + to_string(m_sectionMaterialTypeMap.size()));

					auto it = m_sectionMaterialTypeMap.find(result->first);

					if (it != m_sectionMaterialTypeMap.end())
					{
						Logger::Debug("BomUtility -> getMaterialDetails() -> material type: " + it->first);
						Logger::Debug("BomUtility -> getMaterialDetails() -> supported sections: " + it->second.join(',').toStdString());
						auto itr = m_bomSectionTableInfoMap.find(it->second[0].toStdString());
						tableName = it->second[0];
						if (itr != m_bomSectionTableInfoMap.end())
						{
							table = BOMUtility::GetSectionTable(m_bomSectionTableInfoMap, it->second[0].toStdString());
							placementProductTypeJson = BOMUtility::GetMaterialTypeForSection(m_bomSectionNameAndTypeMap, tableName.toStdString());
						}

					}
					else
					{
						table = BOMUtility::GetSectionTable(m_bomSectionTableInfoMap, "Blank");
						tableName = "Blank";
					}


				}
				else
				{
					name = Helper::GetJSONValue<string>(materialCountJson, "name", true);
					table = BOMUtility::GetSectionTable(m_bomSectionTableInfoMap, "Blank");
					tableName = "Blank";
				}


			}
			else
			{
				name = Helper::GetJSONValue<string>(materialCountJson, "name", true);
				table = BOMUtility::GetSectionTable(m_bomSectionTableInfoMap, "Blank");
				tableName = "Blank";
			}

			if (table == nullptr)
				continue;

			costInfoStrVal = colorwayListJson["colorwayCostInfo"].dump();
			json costInfoJson = json::parse(costInfoStrVal);
			quantityVal = Helper::GetJSONValue<string>(costInfoJson, "rollLength", true);
			string uom = Helper::GetJSONValue<string>(costInfoJson, "rollLengthUnit", true);
			rowDataJson["Code"] = code;
			rowDataJson["material_name"] = name;
			rowDataJson["Type"] = materialType;
			rowDataJson["comment"] = description;
			rowDataJson["qty_default"] = quantityVal;
			rowDataJson["uom"] = uom;
			rowDataJson["materialId"] = objectId;
			AddBomRows(table, rowDataJson, tableName, placementProductTypeJson);
		}


	}

	/*
Description - AddBomRows(QTableWidget* _sectionTable, json _rowDataJson, QString _tableName, json _placementMateriaTypeJson, bool _userAddedRow) method used to add row in bom table.
	* Parameter -string
	* Exception -
	*Return -
	*/
	void CreateProductBOMHandler::AddBomRows(QTableWidget* _sectionTable, json _rowDataJson, QString _tableName, json _placementMateriaTypeJson, bool _userAddedRow)
	{

		Logger::Debug("CreateProductBOMHandler -> AddBomRows() -> Start");
		Logger::Debug("CreateProductBOMHandler -> AddBomRows() -> _sectionTable" + to_string(long(_sectionTable)));
        
#ifdef __APPLE__    // Settinng Windows OS style to QComboBox on MAC OS
        _sectionTable->setStyle(QStyleFactory::create("Windows"));
        _sectionTable->setItemDelegate(new QStyledItemDelegate());
#endif
		int rowCount = _sectionTable->rowCount();
		Logger::Debug("CreateProductBOMHandler -> AddBomRows() -> test1");
		_sectionTable->setRowCount(rowCount);
		Logger::Debug("CreateProductBOMHandler -> AddBomRows() -> test2");
		_sectionTable->insertRow(rowCount);
		//bool colorChip = false;
		//Logger::Debug("AddNewBom -> AddBomRows() -> Code"+ _code);
		//Logger::Debug("AddNewBom -> AddBomRows() -> Name"+ _objectName);
		//Logger::Debug("AddNewBom -> AddBomRows() -> Quantity"+ _quantity);
		Logger::Debug("CreateProductBOMHandler -> AddBomRows() -> test3");
		int columnCount = _sectionTable->columnCount();
		//" << "Code" <<"Name"<< "Type"<< "Placement"<< "Quantity"<<"Comment";
		QStringList bomTableColumnKeys;
		bomTableColumnKeys = m_bomTableColumnKeys;
		Logger::Debug("CreateProductBOMHandler -> AddBomRows() -> test4");
		bomTableColumnKeys.append(CreateProduct::GetInstance()->m_mappedColorways);
		Logger::Debug("CreateProductBOMHandler -> AddBomRows() -> test5");
		string materialId = Helper::GetJSONValue<string>(_rowDataJson, "materialId", true);
		Logger::Debug("CreateProductBOMHandler -> AddBomRows() -> materialId" + materialId);
		for (int columnIndex = 0; columnIndex < bomTableColumnKeys.size(); columnIndex++)
		{
			bool colorChip = false;
			Logger::Debug("CreateProductBOMHandler -> AddBomRows() -> 1");

			string text;
			if (_rowDataJson.contains(bomTableColumnKeys[columnIndex].toStdString()))
			{
				text = Helper::GetJSONValue<string>(_rowDataJson, bomTableColumnKeys[columnIndex].toStdString(), true);
				if (bomTableColumnKeys[columnIndex] == "Type")
				{
					ComboBoxItem* comboType = new ComboBoxItem();
					comboType->setStyleSheet("QComboBox{max-height: 25px; min-width: 80px;}");
					comboType->setAttribute(Qt::WA_MacShowFocusRect, false);
					comboType->setFocusPolicy(Qt::StrongFocus);
					QStringList materialType;
					for (int i = 0; i < _placementMateriaTypeJson.size(); i++)
					{
						string bomPlacementProductTypeId = Helper::GetJSONValue<int>(_placementMateriaTypeJson, i, true);
						auto itr = m_materialTypeNameIdMap.find(bomPlacementProductTypeId);
						if (itr != m_materialTypeNameIdMap.end())
						{
							itr->first;
							materialType.append(QString::fromStdString(itr->second));
						}
						comboType->setProperty(itr->second.c_str(), QString::fromStdString(itr->first));

					}
					if (_tableName == "Blank")
					{
						for (auto itr = m_materialTypeNameIdMap.begin(); itr != m_materialTypeNameIdMap.end(); itr++)
						{
							itr->first;
							materialType.append(QString::fromStdString(itr->second));
							comboType->setProperty(itr->second.c_str(), QString::fromStdString(itr->first));
						}
					}
					if (_userAddedRow && FormatHelper::HasContent(text))
					{
						auto itr = m_materialTypeNameIdMap.find(text);
						if (itr != m_materialTypeNameIdMap.end())
						{
							text = itr->second;
						}
						comboType->setProperty(itr->second.c_str(), QString::fromStdString(itr->first));
					}
					comboType->addItems(materialType);
					comboType->setProperty("materialId", QString::fromStdString(materialId));
					int valueIndex = comboType->findText(QString::fromStdString(text));

					if (valueIndex == -1) // -1 for not found
					{
						int index = comboType->findText(QString::fromStdString(BLANK));
						comboType->setCurrentIndex(index);
					}
					else
					{
						comboType->setCurrentIndex(valueIndex);
					}
					comboType->setProperty("rest_api_name", bomTableColumnKeys[columnIndex]);

					//comboType->setProperty("Id", QString::fromStdString(_objectId));
					QWidget *pWidget = CVWidgetGenerator::InsertWidgetInCenter(comboType);
					//comboColorwayItem->clear();
					//comboColorwayItem->addItems(_colorwayNamesList);
					_sectionTable->setCellWidget(rowCount, columnIndex, pWidget);

					//QObject::connect(comboColorwayItem, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(OnHandleColorwayNameComboBox(const QString&)));
				}
				else if (CreateProduct::GetInstance()->m_mappedColorways.contains(bomTableColumnKeys[columnIndex]) || bomTableColumnKeys[columnIndex] == "common_color")
				{
					string colorId = text;
					if (FormatHelper::HasContent(colorId))
					{
						//json dependentFieldJson = Helper::makeRestcallGet("csi-requesthandler/api/v2/color_specifications/", "&skip=0&limit=100", "" + colorId, "");
						string resultResponse = RESTAPI::CentricRestCallGet(Configuration::GetInstance()->GetPLMServerURL() + RESTAPI::SEARCH_COLOR_API + "/" + colorId, APPLICATION_JSON_TYPE, "");
						Logger::Debug("CreateProductBOMHandler -> AddBomRows() -> resultResponse" + resultResponse);
						json ColoreResultJson = json::parse(resultResponse);
						Logger::Debug("CreateProductBOMHandler -> AddBomRows() -> ColoreResultJson" + to_string(ColoreResultJson));
						string rgbValue = Helper::GetJSONValue<string>(ColoreResultJson, RGB_VALUE_KEY, true);
						string colorName = Helper::GetJSONValue<string>(ColoreResultJson, ATTRIBUTE_NAME, true);
						Logger::Debug("CreateProductBOMHandler -> AddBomRows() -> rgbValue" + rgbValue);

						rgbValue = Helper::FindAndReplace(rgbValue, "(", "");
						rgbValue = Helper::FindAndReplace(rgbValue, ")", "");
						rgbValue = Helper::FindAndReplace(rgbValue, " ", "");
						Logger::Debug("CreateProductBOMHandler -> AddBomRows() -> 2");
						if (FormatHelper::HasContent(rgbValue))
						{
							QStringList listRGB;
							QString colorRGB = QString::fromStdString(rgbValue);
							listRGB = colorRGB.split(',');
							int red = listRGB.at(0).toInt();
							int green = listRGB.at(1).toInt();
							int blue = listRGB.at(2).toInt();
							QColor color(red, green, blue);
							QImage image(20, 20, QImage::Format_ARGB32);
							image.fill(color);
							QPixmap pixmap;
							Logger::Debug("CreateProductBOMHandler -> AddBomRows() -> 2");
							QLabel* label = new QLabel();
							label->setToolTip(QString::fromStdString(colorName));
							pixmap = QPixmap::fromImage(image);
							label->setPixmap(QPixmap(pixmap));


							QWidget* p_widget = new QWidget(_sectionTable);
							QGridLayout* gridLayout = new QGridLayout(_sectionTable);
							gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
							gridLayout->setContentsMargins(0, 0, 0, 0);

							QWidget *colorchip = CVWidgetGenerator::InsertWidgetInCenter(label);

							gridLayout->addWidget(colorchip, 0, 0, 1, 1, Qt::AlignHCenter);
							QPushButton* pushButton_2 = CVWidgetGenerator::CreatePushButton("", ADD_HOVER_ICON_PATH, "", PUSH_BUTTON_STYLE, 30, true);
							pushButton_2->setFixedHeight(20);
							pushButton_2->setFixedWidth(20);
							if (m_addColorButtonSignalMapper != nullptr)
							{
								m_addColorButtonSignalMapper->setProperty("TableName", _tableName);
								connect(pushButton_2, SIGNAL(clicked()), m_addColorButtonSignalMapper, SLOT(map()));
								m_addColorButtonSignalMapper->setMapping(pushButton_2, QString("%1-%2-%3").arg(rowCount).arg(columnIndex).arg(_tableName));
							}
							pushButton_2->setProperty("TableName", _tableName);
							gridLayout->addWidget(pushButton_2, 0, 1, 1, 1, Qt::AlignHCenter);
							p_widget->setLayout(gridLayout);
							p_widget->setProperty("rest_api_name", bomTableColumnKeys[columnIndex]);
							p_widget->setProperty("colorId", colorId.c_str());
							Logger::Debug("CreateProductBOMHandler -> AddBomRows() -> 10");
							_sectionTable->setCellWidget(rowCount, columnIndex, p_widget);

							colorChip = true;
						}
					}
					else
					{
						QWidget* p_widget = new QWidget(_sectionTable);
						QGridLayout* gridLayout = new QGridLayout(_sectionTable);
						gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
						gridLayout->setContentsMargins(0, 0, 0, 0);

						QPixmap pixmap;
						Logger::Debug("CreateProductBOMHandler -> AddBomRows() -> 2");
						QLabel* label = new QLabel();

						QImage styleIcon;
						QImageReader imageReader(":/CLOVise/PLM/Images/NoImage.png");
						imageReader.setDecideFormatFromContent(true);
						styleIcon = imageReader.read();
						pixmap = QPixmap::fromImage(styleIcon);

						label->setMaximumSize(QSize(20, 20));
						int width = label->width();
						int height = label->height();
						label->setPixmap(QPixmap(pixmap.scaled(width, height, Qt::KeepAspectRatio)));
						Logger::Debug("CreateProductBOMHandler -> AddBomRows() -> 3");
						QWidget *colorchip = CVWidgetGenerator::InsertWidgetInCenter(label);

						gridLayout->addWidget(colorchip, 0, 0, 1, 1, Qt::AlignHCenter);
						QPushButton* pushButton_2 = CVWidgetGenerator::CreatePushButton("", ADD_HOVER_ICON_PATH, "", PUSH_BUTTON_STYLE, 30, true);
						pushButton_2->setFixedHeight(20);
						pushButton_2->setFixedWidth(20);
						if (m_addColorButtonSignalMapper != nullptr)
						{
							m_addColorButtonSignalMapper->setProperty("TableName", _tableName);
							connect(pushButton_2, SIGNAL(clicked()), m_addColorButtonSignalMapper, SLOT(map()));
							//int number = rowCount * 10 + columnIndex;
							m_addColorButtonSignalMapper->setMapping(pushButton_2, QString("%1-%2-%3").arg(rowCount).arg(columnIndex).arg(_tableName));
							//m_buttonSignalMapper->setMapping(pushButton_2, number);
						}
						pushButton_2->setProperty("TableName", _tableName);
						gridLayout->addWidget(pushButton_2, 0, 1, 1, 1, Qt::AlignHCenter);
						p_widget->setLayout(gridLayout);
						p_widget->setProperty("rest_api_name", bomTableColumnKeys[columnIndex]);
						Logger::Debug("CreateProductBOMHandler -> AddBomRows() -> 10");
						_sectionTable->setCellWidget(rowCount, columnIndex, p_widget);
						colorChip = true;

					}
				}
				else {

					QLineEdit* newColumn = new QLineEdit();
					newColumn->setStyleSheet(LINEEDIT_STYLE);
					
					if (FormatHelper::HasContent(text))
						newColumn->setText(QString::fromStdString(text));
					else
						newColumn->setText("");
					newColumn->home(true);
					Logger::Debug("CreateProductBOMHandler -> AddBomRows() -> 6");
					QWidget *pLineEditWidget = CVWidgetGenerator::InsertWidgetInCenter(newColumn);
					newColumn->setProperty("row", rowCount);
					Logger::Debug("CreateProductBOMHandler -> AddBomRows() -> 7");
					newColumn->setProperty("rest_api_name", bomTableColumnKeys[columnIndex]);
					_sectionTable->setCellWidget(rowCount, columnIndex, pLineEditWidget);
					Logger::Debug("CreateProductBOMHandler -> AddBomRows() -> 8");
				}
			}
			else
			{
				text = "";

				if (CreateProduct::GetInstance()->m_mappedColorways.contains(bomTableColumnKeys[columnIndex]))
				{
					string colorData = _rowDataJson["color"].dump();

					json colorDataJson = json::parse(colorData);
					Logger::Debug("CreateProductBOMHandler -> AddBomRows() -> colorDataJson" + to_string(colorDataJson));
					if (!colorDataJson.empty())
					{
						for (int colorwayListCount = 0; colorwayListCount < colorDataJson.size(); colorwayListCount++)
						{
							json colorwayJson = json::object();
							json colorJson = json::object();
							string cloColorwayStr = colorDataJson[colorwayListCount].dump();

							json cloColorwayJson = json::parse(cloColorwayStr);
							Logger::Debug("CreateProductBOMHandler -> AddBomRows() -> cloColorwayJson" + to_string(cloColorwayJson));
							string colorId = Helper::GetJSONValue<string>(cloColorwayJson, "colorObjectId", true);
							Logger::Debug("CreateProductBOMHandler -> AddBomRows() -> colorId" + colorId);

							string colorwayName = Helper::GetJSONValue<string>(cloColorwayJson, "colorwayName", true);
							Logger::Debug("CreateProductBOMHandler -> AddBomRows() -> colorwayName" + colorwayName);

							Logger::Debug("CreateProductBOMHandler -> AddBomRows() -> horizontalHeaderItem Text" + _sectionTable->horizontalHeaderItem(columnIndex)->text().toStdString());
							if (colorwayName == _sectionTable->horizontalHeaderItem(columnIndex)->text().toStdString() &&  (FormatHelper::HasContent(colorId)))
							{


								//json dependentFieldJson = Helper::makeRestcallGet("csi-requesthandler/api/v2/color_specifications/", "&skip=0&limit=100", "" + colorId, "");
								string resultResponse = RESTAPI::CentricRestCallGet(Configuration::GetInstance()->GetPLMServerURL() + RESTAPI::SEARCH_COLOR_API + "/" + colorId, APPLICATION_JSON_TYPE, "");
								Logger::Debug("CreateProductBOMHandler -> AddBomRows() -> resultResponse" + resultResponse);
								json ColoreResultJson = json::parse(resultResponse);
								Logger::Debug("CreateProductBOMHandler -> AddBomRows() -> ColoreResultJson" + to_string(ColoreResultJson));
								string rgbValue = Helper::GetJSONValue<string>(ColoreResultJson, RGB_VALUE_KEY, true);
								string colorName = Helper::GetJSONValue<string>(ColoreResultJson, ATTRIBUTE_NAME, true);
								Logger::Debug("CreateProductBOMHandler -> AddBomRows() -> rgbValue" + rgbValue);

								rgbValue = Helper::FindAndReplace(rgbValue, "(", "");
								rgbValue = Helper::FindAndReplace(rgbValue, ")", "");
								rgbValue = Helper::FindAndReplace(rgbValue, " ", "");
								Logger::Debug("CreateProductBOMHandler -> AddBomRows() -> 2");
								if (FormatHelper::HasContent(rgbValue))
								{
									QStringList listRGB;
									QString colorRGB = QString::fromStdString(rgbValue);
									listRGB = colorRGB.split(',');
									int red = listRGB.at(0).toInt();
									int green = listRGB.at(1).toInt();
									int blue = listRGB.at(2).toInt();
									QColor color(red, green, blue);
									QImage image(20, 20, QImage::Format_ARGB32);
									image.fill(color);
									QPixmap pixmap;
									Logger::Debug("CreateProductBOMHandler -> AddBomRows() -> 2");
									QLabel* label = new QLabel();
									label->setToolTip(QString::fromStdString(colorName));
									pixmap = QPixmap::fromImage(image);
									label->setPixmap(QPixmap(pixmap));


									QWidget* p_widget = new QWidget(_sectionTable);
									QGridLayout* gridLayout = new QGridLayout(_sectionTable);
									gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
									gridLayout->setContentsMargins(0, 0, 0, 0);

									QWidget *colorchip = CVWidgetGenerator::InsertWidgetInCenter(label);
									colorchip->setProperty("colorId", colorId.c_str());
									gridLayout->addWidget(colorchip, 0, 0, 1, 1, Qt::AlignHCenter);
									QPushButton* pushButton_2 = CVWidgetGenerator::CreatePushButton("", ADD_HOVER_ICON_PATH, "", PUSH_BUTTON_STYLE, 30, true);
									pushButton_2->setFixedHeight(20);
									pushButton_2->setFixedWidth(20);
									if (m_addColorButtonSignalMapper != nullptr)
									{
										m_addColorButtonSignalMapper->setProperty("TableName", _tableName);
										connect(pushButton_2, SIGNAL(clicked()), m_addColorButtonSignalMapper, SLOT(map()));
										m_addColorButtonSignalMapper->setMapping(pushButton_2, QString("%1-%2-%3").arg(rowCount).arg(columnIndex).arg(_tableName));
									}
									pushButton_2->setProperty("TableName", _tableName);
									gridLayout->addWidget(pushButton_2, 0, 1, 1, 1, Qt::AlignHCenter);
									p_widget->setLayout(gridLayout);
									p_widget->setProperty("rest_api_name", bomTableColumnKeys[columnIndex]);
									p_widget->setProperty("colorId", colorId.c_str());
									Logger::Debug("CreateProductBOMHandler -> AddBomRows() -> 10");
									_sectionTable->setCellWidget(rowCount, columnIndex, p_widget);

									colorChip = true;
								}
							}
								else
								{
									QWidget* p_widget = new QWidget(_sectionTable);
									QGridLayout* gridLayout = new QGridLayout(_sectionTable);
									gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
									gridLayout->setContentsMargins(0, 0, 0, 0);

									QPixmap pixmap;
									Logger::Debug("CreateProductBOMHandler -> AddBomRows() -> 2");
									QLabel* label = new QLabel();

									QImage styleIcon;
									QImageReader imageReader(":/CLOVise/PLM/Images/NoImage.png");
									imageReader.setDecideFormatFromContent(true);
									styleIcon = imageReader.read();
									pixmap = QPixmap::fromImage(styleIcon);

									label->setMaximumSize(QSize(20, 20));
									int width = label->width();
									int height = label->height();
									label->setPixmap(QPixmap(pixmap.scaled(width, height, Qt::KeepAspectRatio)));
									Logger::Debug("CreateProductBOMHandler -> AddBomRows() -> 3");
									QWidget *colorchip = CVWidgetGenerator::InsertWidgetInCenter(label);

									gridLayout->addWidget(colorchip, 0, 0, 1, 1, Qt::AlignHCenter);
									QPushButton* pushButton_2 = CVWidgetGenerator::CreatePushButton("", ADD_HOVER_ICON_PATH, "", PUSH_BUTTON_STYLE, 30, true);
									pushButton_2->setFixedHeight(20);
									pushButton_2->setFixedWidth(20);
									if (m_addColorButtonSignalMapper != nullptr)
									{
										m_addColorButtonSignalMapper->setProperty("TableName", _tableName);
										connect(pushButton_2, SIGNAL(clicked()), m_addColorButtonSignalMapper, SLOT(map()));
										//int number = rowCount * 10 + columnIndex;
										m_addColorButtonSignalMapper->setMapping(pushButton_2, QString("%1-%2-%3").arg(rowCount).arg(columnIndex).arg(_tableName));
										//m_buttonSignalMapper->setMapping(pushButton_2, number);
									}
									pushButton_2->setProperty("TableName", _tableName);
									gridLayout->addWidget(pushButton_2, 0, 1, 1, 1, Qt::AlignHCenter);
									p_widget->setLayout(gridLayout);
									p_widget->setProperty("rest_api_name", bomTableColumnKeys[columnIndex]);
									Logger::Debug("CreateProductBOMHandler -> AddBomRows() -> 10");
									_sectionTable->setCellWidget(rowCount, columnIndex, p_widget);
									colorChip = true;

								}
							
						}
					}
					else
					{
						QWidget* p_widget = new QWidget(_sectionTable);
						QGridLayout* gridLayout = new QGridLayout(_sectionTable);
						gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
						gridLayout->setContentsMargins(0, 0, 0, 0);

						QPixmap pixmap;
						Logger::Debug("CreateProductBOMHandler -> AddBomRows() -> 2");
						QLabel* label = new QLabel();

						QImage styleIcon;
						QImageReader imageReader(":/CLOVise/PLM/Images/NoImage.png");
						imageReader.setDecideFormatFromContent(true);
						styleIcon = imageReader.read();
						pixmap = QPixmap::fromImage(styleIcon);

						label->setMaximumSize(QSize(20, 20));
						int width = label->width();
						int height = label->height();
						label->setPixmap(QPixmap(pixmap.scaled(width, height, Qt::KeepAspectRatio)));
						Logger::Debug("CreateProductBOMHandler -> AddBomRows() -> 3");
						QWidget *colorchip = CVWidgetGenerator::InsertWidgetInCenter(label);


						gridLayout->addWidget(colorchip, 0, 0, 1, 1, Qt::AlignHCenter);
						QPushButton* pushButton_2 = CVWidgetGenerator::CreatePushButton("", ADD_HOVER_ICON_PATH, "", PUSH_BUTTON_STYLE, 30, true);
						pushButton_2->setFixedHeight(20);
						pushButton_2->setFixedWidth(20);
						if (m_addColorButtonSignalMapper != nullptr)
						{
							m_addColorButtonSignalMapper->setProperty("TableName", _tableName);
							connect(pushButton_2, SIGNAL(clicked()), m_addColorButtonSignalMapper, SLOT(map()));
							//int number = rowCount * 10 + columnIndex;
							m_addColorButtonSignalMapper->setMapping(pushButton_2, QString("%1-%2-%3").arg(rowCount).arg(columnIndex).arg(_tableName));
							//m_buttonSignalMapper->setMapping(pushButton_2, number);
						}
						pushButton_2->setProperty("TableName", _tableName);
						gridLayout->addWidget(pushButton_2, 0, 1, 1, 1, Qt::AlignHCenter);
						p_widget->setLayout(gridLayout);
						p_widget->setProperty("rest_api_name", bomTableColumnKeys[columnIndex]);
						Logger::Debug("CreateProductBOMHandler -> AddBomRows() -> 10");
						_sectionTable->setCellWidget(rowCount, columnIndex, p_widget);
						colorChip = true;
					}
				}
				else
				{


					Logger::Debug("AddNewBom -> AddBomRows() -> m_bomTableColumnKeys[columnIndex]" + bomTableColumnKeys[columnIndex].toStdString());
					if (bomTableColumnKeys[columnIndex] == "common_color")
					{
						Logger::Debug("CreateProductBOMHandler -> AddBomRows() -> 9");

						QPushButton* buttonColumn = new QPushButton("Add color");

						//newColumn->setProperty("row", rowCount);

						QWidget* p_widget = new QWidget(_sectionTable);
						QGridLayout* gridLayout = new QGridLayout(_sectionTable);
						gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
						gridLayout->setContentsMargins(0, 0, 0, 0);

						QPixmap pixmap;
						Logger::Debug("CreateProductBOMHandler -> AddBomRows() -> 2");
						QLabel* label = new QLabel();

						QImage styleIcon;
						QImageReader imageReader(":/CLOVise/PLM/Images/NoImage.png");
						imageReader.setDecideFormatFromContent(true);
						styleIcon = imageReader.read();
						pixmap = QPixmap::fromImage(styleIcon);

						label->setMaximumSize(QSize(20, 20));
						int width = label->width();
						int height = label->height();
						label->setPixmap(QPixmap(pixmap.scaled(width, height, Qt::KeepAspectRatio)));
						Logger::Debug("CreateProductBOMHandler -> AddBomRows() -> 3");
						QWidget *colorchip = CVWidgetGenerator::InsertWidgetInCenter(label);

						//QPushButton* pushButton = new QPushButton(_sectionTable);
						//pushButton->setObjectName(QString::fromUtf8("Add"));
						gridLayout->addWidget(colorchip, 0, 0, 1, 1, Qt::AlignHCenter);
						QPushButton* pushButton_2 = CVWidgetGenerator::CreatePushButton("", ADD_HOVER_ICON_PATH, "", PUSH_BUTTON_STYLE, 30, true);
						pushButton_2->setFixedHeight(20);
						pushButton_2->setFixedWidth(20);
						if (m_addColorButtonSignalMapper != nullptr)
						{
							m_addColorButtonSignalMapper->setProperty("TableName", _tableName);
							connect(pushButton_2, SIGNAL(clicked()), m_addColorButtonSignalMapper, SLOT(map()));
							//int number = rowCount * 10 + columnIndex;
							//m_buttonSignalMapper->setMapping(pushButton_2, number);
							m_addColorButtonSignalMapper->setMapping(pushButton_2, QString("%1-%2-%3").arg(rowCount).arg(columnIndex).arg(_tableName));
						}
						pushButton_2->setProperty("TableName", _tableName);
						//pushButton_2->setObjectName(QString::fromUtf8(""));
						//QObject::connect(pushButton_2, SIGNAL(clicked()), this, SLOT(OnClickAddColorButton()));
						//connect(updateColorButton, SIGNAL(clicked()), m_buttonSignalMapper, SLOT(map()));
						gridLayout->addWidget(pushButton_2, 0, 1, 1, 1, Qt::AlignHCenter);
						//QLabel* label1 = new QLabel(_sectionTable);
						//label1->setText("Color Name Demo");
						//label->setObjectName(QString::fromUtf8("pushButton_3"));
						//gridLayout->addWidget(label1, 1, 0, 1, 2, Qt::AlignHCenter);
						p_widget->setLayout(gridLayout);
						p_widget->setProperty("rest_api_name", bomTableColumnKeys[columnIndex]);
						//QWidget *pLineEditWidget = CVWidgetGenerator::InsertWidgetInCenter(p_widget);
						Logger::Debug("CreateProductBOMHandler -> AddBomRows() -> 10");
						_sectionTable->setCellWidget(rowCount, columnIndex, p_widget);
					}
					else if (bomTableColumnKeys[columnIndex] == "Delete")
					{
						QPushButton *deleteButton = CVWidgetGenerator::CreatePushButton("", ":/CLOVise/PLM/Images/icon_delete_over.svg", "Delete", PUSH_BUTTON_STYLE, 30, true);
						deleteButton->setStyleSheet(BUTTON_STYLE);
						deleteButton->setProperty("materialId", QString::fromStdString(materialId));
						Logger::Debug("CreateProductBOMHandler -> AddBomRows() -> _tableName" + _tableName.toStdString());
						if (m_deleteButtonSignalMapper != nullptr)
						{
							m_deleteButtonSignalMapper->setProperty("TableName", _tableName);
							connect(deleteButton, SIGNAL(clicked()), m_deleteButtonSignalMapper, SLOT(map()));
							m_deleteButtonSignalMapper->setMapping(deleteButton, QString("%1-%2").arg(rowCount).arg(_tableName));
						}
						deleteButton->setProperty("RowAddedByUser", _userAddedRow);
						//QObject::connect(deleteButton, SIGNAL(clicked()), this, SLOT(OnClickDeleteButton()));
						Logger::Debug("CreateProductBOMHandler -> AddBomRows() -> 9");
						QWidget *pPushButton = CVWidgetGenerator::InsertWidgetInCenter(deleteButton);
						deleteButton->setProperty("rest_api_name", bomTableColumnKeys[columnIndex]);
						Logger::Debug("CreateProductBOMHandler -> AddBomRows() -> 10");
						_sectionTable->setCellWidget(rowCount, columnIndex, pPushButton);
						Logger::Debug("CreateProductBOMHandler -> AddBomRows() -> 11");
					}
					else
					{
						QLineEdit* newColumn = new QLineEdit();
						newColumn->setAttribute(Qt::WA_MacShowFocusRect, false);
						newColumn->setStyleSheet(LINEEDIT_STYLE);
						
						if (FormatHelper::HasContent(text))
							newColumn->setText(QString::fromStdString(text));
						else
							newColumn->setText("");

						newColumn->home(true);
						Logger::Debug("CreateProductBOMHandler -> AddBomRows() -> 6");
						QWidget *pLineEditWidget = CVWidgetGenerator::InsertWidgetInCenter(newColumn);
						newColumn->setProperty("row", rowCount);
						Logger::Debug("CreateProductBOMHandler -> AddBomRows() -> 7");
						newColumn->setProperty("rest_api_name", bomTableColumnKeys[columnIndex]);
						_sectionTable->setCellWidget(rowCount, columnIndex, pLineEditWidget);
						Logger::Debug("CreateProductBOMHandler -> AddBomRows() -> 8");
					}

				}



			}


			Logger::Debug("CreateProductBOMHandler -> AddBomRows() -> End");
		}
	}


	/*
Description - AddMaterialInBom method used to add a material in bom table.
	* Parameter -string
	* Exception -
	*Return -
	*/
	void CreateProductBOMHandler::AddMaterialInBom()
	{
		QTableWidget* sectionTable;
		Logger::Debug("CreateProductBOMHandler -> AddMaterialInBom () button" + to_string(long(m_currentAddMaterialButtonClicked)));
		auto it = m_addMaterialButtonAndTableMap.find(m_currentAddMaterialButtonClicked);
		Logger::Debug("CreateProductBOMHandler -> AddMaterialInBom () m_addMaterialButtonAndTableMap.size" + to_string(m_addMaterialButtonAndTableMap.size()));
		if (it != m_addMaterialButtonAndTableMap.end())
		{
			Logger::Debug("CreateProductBOMHandler -> AddMaterialInBom() -> 1");
			sectionTable = it->second;
			QString tableName = sectionTable->property("TableName").toString();
			json rowDataJson = BOMUtility::AddMaterialInBom();
			json placementMateriaTypeJson;
			Logger::Debug("CreateProductBOMHandler -> AddMaterialInBom() -> rowDataJson" + to_string(rowDataJson));
			placementMateriaTypeJson = BOMUtility::GetMaterialTypeForSection(m_bomSectionNameAndTypeMap, tableName.toStdString());
			AddBomRows(sectionTable, rowDataJson, tableName, placementMateriaTypeJson, true);
		}
	}

	/*
* Description - onClickAddSpecialMaterialButton() method is to add special material in bom table
* Parameter -
* Exception -
* Return -
*/
	void CreateProductBOMHandler::onClickAddSpecialMaterialButton()
	{
		Logger::Debug("CreateProductBOMHandler -> onClickAddSpecialMaterialButton () Start");
		QTableWidget* sectionTable;
		QPushButton* button = (QPushButton*)sender();
		auto it = m_addSpecialMaterialButtonAndTableMap.find(button);
		if (it != m_addSpecialMaterialButtonAndTableMap.end())
		{
			sectionTable = it->second;
			QString tableName = sectionTable->property("TableName").toString();
			json rowDataJson = json::object();
			rowDataJson["Code"] = "";
			rowDataJson["material_name"] = "";
			rowDataJson["Type"] = "";
			rowDataJson["comment"] = "";
			rowDataJson["qty_default"] = "";
			rowDataJson["uom"] = "";
			rowDataJson["materialId"] = "";
			json placementMateriaTypeJson;
			placementMateriaTypeJson = BOMUtility::GetMaterialTypeForSection(m_bomSectionNameAndTypeMap, tableName.toStdString());
			AddBomRows(sectionTable, rowDataJson, tableName, placementMateriaTypeJson, true);
		}

		Logger::Debug("CreateProductBOMHandler -> onClickAddSpecialMaterialButton () End");
	}

	void CreateProductBOMHandler::CreateBom(string _productId, json _BomMetaData, map<string, string> _CloAndPLMColorwayMap)
	{
		BOMUtility::CreateBom(_productId, _BomMetaData, m_bomSectionTableInfoMap, CreateProduct::GetInstance()->m_mappedColorways, _CloAndPLMColorwayMap);
	}

	/*
Description - BackupBomDetails() used to store bom data on click of save button
	* Parameter -
	* Exception -
	*Return -
	*/
	void CreateProductBOMHandler::BackupBomDetails()
	{
		m_backupBomDataMap = BOMUtility::BackupBomDetails(m_bomSectionTableInfoMap, CreateProduct::GetInstance()->m_mappedColorways);
	}

	/*
Description - RestoreBomDetails() method used to store bom data.
	* Parameter -
	* Exception -
	*Return -
	*/
	void CreateProductBOMHandler::RestoreBomDetails()
	{
		Logger::Debug("CreateProductBOMHandler -> RestoreBomDetails() -> Start");
		getMaterialDetails("fabricList", Configuration::GetInstance()->GetTechPackJson(), true);
		getMaterialDetails("buttonHeadList", Configuration::GetInstance()->GetTechPackJson(), false);
		getMaterialDetails("buttonHoleList", Configuration::GetInstance()->GetTechPackJson(), false);
		getMaterialDetails("zipperList", Configuration::GetInstance()->GetTechPackJson(), false);
		Logger::Debug("CreateProductBOMHandler -> RestoreBomDetails() -> m_backupBomDataMap size" + to_string(m_backupBomDataMap.size()));
		for (auto itr = m_backupBomDataMap.begin(); itr != m_backupBomDataMap.end(); itr++)
		{
			json rowDataJson = itr->second;
			Logger::Debug("CreateProductBOMHandler -> RestoreBomDetails() -> TableName" + itr->first);
			Logger::Debug("CreateProductBOMHandler -> RestoreBomDetails() -> rowDataJson" + to_string(rowDataJson));
			QTableWidget* table = BOMUtility::GetSectionTable(m_bomSectionTableInfoMap, itr->first);
			json placementMateriaTypeJson;
			placementMateriaTypeJson = BOMUtility::GetMaterialTypeForSection(m_bomSectionNameAndTypeMap, itr->first);
			if (table != nullptr)
			{
				for (int i = 0; i < rowDataJson.size(); i++)
				{
					json rowData = rowDataJson.at(i);
					Logger::Debug("CreateProductBOMHandler -> RestoreBomDetails() -> rowData" + to_string(rowData));
					AddBomRows(table, rowData, QString::fromStdString(itr->first), placementMateriaTypeJson, true);
				}
			}
		}
		Logger::Debug("CreateProductBOMHandler -> RestoreBomDetails() -> End");
	}

	/*
Description - UpdateColorwayColumns() method used to add or remove colorways columns from bom table
	* Parameter -
	* Exception -
	*Return -
	*/
	void CreateProductBOMHandler::UpdateColorwayColumnsInBom()
	{

		Logger::Debug("CreateProductBOMHandler -> UpdateColorwayColumns () Start");


		QStringList tablecolumnList;
		QStringList bomTableColumnKeys;
		tablecolumnList = m_bomTableColumnlist;
		bomTableColumnKeys = m_bomTableColumnKeys;
		QStringList bomColorwayColumns;


		if (CreateProduct::GetInstance()->m_mappedColorways.size() && m_bomSectionTableInfoMap.size() > 0)
		{

			tablecolumnList.append(CreateProduct::GetInstance()->m_mappedColorways);
			bomTableColumnKeys.append(CreateProduct::GetInstance()->m_mappedColorways);

		}
		int rowCount = 0;
		for (auto itr = m_bomSectionTableInfoMap.begin(); itr != m_bomSectionTableInfoMap.end(); itr++)// map contain section name and corresponding table pointer  
		{
			Logger::Debug("CreateProductBOMHandler -> UpdateColorwayColumns () 1");
			QTableWidget* sectionTable = itr->second;
			int columnCount = sectionTable->columnCount();
			int bomColumnCountWOColorway = m_bomTableColumnlist.size();
			if (columnCount > bomColumnCountWOColorway)// means colorway column in table
			{
				int diff = columnCount - bomColumnCountWOColorway;
				Logger::Debug("CreateProductBOMHandler -> UpdateColorwayColumns () diff" + to_string(diff));
				while (diff)
				{
					int columnNumber = bomColumnCountWOColorway + diff;
					Logger::Debug("CreateProductBOMHandler -> UpdateColorwayColumns () columnNumber" + to_string(columnNumber));
					QString columnName = sectionTable->horizontalHeaderItem(columnNumber - 1)->text();
					bomColorwayColumns.append(columnName);
					if (!CreateProduct::GetInstance()->m_mappedColorways.contains(columnName))
						sectionTable->removeColumn(columnNumber - 1);
					diff--;
				}
			}
		}
		Logger::Debug("CreateProductBOMHandler -> UpdateColorwayColumns () m_bomSectionTableInfoMap.size()" + to_string(m_bomSectionTableInfoMap.size()));
		for (auto itr = m_bomSectionTableInfoMap.begin(); itr != m_bomSectionTableInfoMap.end(); itr++)// map contain section name and corresponding table pointer  
		{
			QTableWidget* sectionTable = itr->second;
			Logger::Debug("CreateProductBOMHandler -> UpdateColorwayColumns () itr->first" + itr->first);
			QString tableName = QString::fromStdString(itr->first);
			//Logger::Debug("AddNewBom -> UpdateColorwayColumns () 11");
			sectionTable->setColumnCount(bomTableColumnKeys.size());
			sectionTable->setHorizontalHeaderLabels(tablecolumnList);

			Logger::Debug("CreateProductBOMHandler -> UpdateColorwayColumns () sectionTable->rowCount()" + to_string(sectionTable->rowCount()));
			for (int rowCount = 0; rowCount < sectionTable->rowCount(); rowCount++)
			{
				QComboBox* typeCombo;
				typeCombo = static_cast<QComboBox*>(sectionTable->cellWidget(rowCount, MATERIAL_TYPE_COLUMN)->children().last());
				if (typeCombo == nullptr)
					continue;
				QString matrialId = typeCombo->property("materialId").toString();
				Logger::Debug("CreateProductBOMHandler -> UpdateColorwayColumns () matrialId" + matrialId.toStdString());

				Logger::Debug("CreateProductBOMHandler -> UpdateColorwayColumns () sectionTable->columnCount()" + to_string(sectionTable->columnCount()));
				for (int columnIndex = 0; columnIndex < sectionTable->columnCount(); columnIndex++)
				{
					QString columnName = sectionTable->horizontalHeaderItem(columnIndex)->text();
					Logger::Debug("CreateProductBOMHandler -> UpdateColorwayColumns1 () columnName" + columnName.toStdString());
					Logger::Debug("CreateProductBOMHandler -> UpdateColorwayColumns1 () bomColorwayColumns" + bomColorwayColumns.join(',').toStdString());

					if (!bomColorwayColumns.contains(columnName))
					{
						if (FormatHelper::HasContent(matrialId.toStdString()))
						{

							//sectionTabl
							Logger::Debug("CreateProductBOMHandler -> UpdateColorwayColumns () 2");
							if (CreateProduct::GetInstance()->m_mappedColorways.contains(columnName))
							{
								Logger::Debug("CreateProductBOMHandler -> UpdateColorwayColumns () columnName" + columnName.toStdString());
								auto colorwayJsonItr = m_colorwayMapForBom.find(matrialId.toStdString());
								if (colorwayJsonItr != m_colorwayMapForBom.end())
								{
									json colorwayJson = colorwayJsonItr->second;
									Logger::Debug("CreateProductBOMHandler -> UpdateColorwayColumns () colorwayJson" + to_string(colorwayJson));
									string colorwayNameStr = Helper::GetJSONValue<string>(colorwayJson, columnName.toStdString(), false);
									json colorwayNameJson1 = json::parse(colorwayNameStr);
									Logger::Debug("CreateProductBOMHandler -> UpdateColorwayColumns () colorwayNameJson1" + to_string(colorwayNameJson1));
									string colorObjId = Helper::GetJSONValue<string>(colorwayNameJson1, "colorObjectId", true);
									Logger::Debug("CreateProductBOMHandler -> UpdateColorwayColumns () colorObjId" + colorObjId);
									if (FormatHelper::HasContent(colorObjId))
									{
										//json dependentFieldJson = Helper::makeRestcallGet("csi-requesthandler/api/v2/color_specifications/", "&skip=0&limit=100", "" + colorId, "");
										string resultResponse = RESTAPI::CentricRestCallGet(Configuration::GetInstance()->GetPLMServerURL() + RESTAPI::SEARCH_COLOR_API + "/" + colorObjId, APPLICATION_JSON_TYPE, "");
										Logger::Debug("CreateProductBOMHandler -> UpdateColorwayColumns() -> resultResponse" + resultResponse);
										json ColoreResultJson = json::parse(resultResponse);
										Logger::Debug("CreateProductBOMHandler -> UpdateColorwayColumns() -> ColoreResultJson" + to_string(ColoreResultJson));
										string rgbValue = Helper::GetJSONValue<string>(ColoreResultJson, RGB_VALUE_KEY, true);
										string colorName = Helper::GetJSONValue<string>(ColoreResultJson, ATTRIBUTE_NAME, true);
										Logger::Debug("CreateProductBOMHandler -> UpdateColorwayColumns() -> rgbValue" + rgbValue);

										rgbValue = Helper::FindAndReplace(rgbValue, "(", "");
										rgbValue = Helper::FindAndReplace(rgbValue, ")", "");
										rgbValue = Helper::FindAndReplace(rgbValue, " ", "");
										Logger::Debug("CreateProductBOMHandler -> UpdateColorwayColumns() -> 3");
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
											Logger::Debug("CreateProductBOMHandler -> UpdateColorwayColumns() -> 4");
											QLabel* label = new QLabel();
											label->setToolTip(QString::fromStdString(colorName));
											pixmap = QPixmap::fromImage(image);
											label->setPixmap(QPixmap(pixmap));
											Logger::Debug("CreateProductBOMHandler -> UpdateColorwayColumns() -> 5");

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
												connect(pushButton_2, SIGNAL(clicked()), m_addColorButtonSignalMapper, SLOT(map()));
												//int number = rowCount * 10 + columnIndex;
												m_addColorButtonSignalMapper->setMapping(pushButton_2, QString("%1-%2-%3").arg(rowCount).arg(columnIndex).arg(tableName));
												//m_buttonSignalMapper->setMapping(pushButton_2, number);
											}
											pushButton_2->setProperty("TableName", tableName);
											gridLayout->addWidget(pushButton_2, 0, 1, 1, 1, Qt::AlignHCenter);
											p_widget->setLayout(gridLayout);
											p_widget->setProperty("colorId", colorObjId.c_str());
											sectionTable->setCellWidget(rowCount, columnIndex, p_widget);
											Logger::Debug("CreateProductBOMHandler -> AddBomRows() -> 7");
											//colorChip = true;
										}
									}
								}
								else
								{
									if (CreateProduct::GetInstance()->m_mappedColorways.contains(columnName))
									{
										QWidget* p_widget = new QWidget(sectionTable);
										QGridLayout* gridLayout = new QGridLayout(sectionTable);
										gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
										gridLayout->setContentsMargins(0, 0, 0, 0);

										QPixmap pixmap;
										Logger::Debug("CreateProductBOMHandler -> AddBomRows() -> 2");
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
										Logger::Debug("CreateProductBOMHandler -> AddBomRows() -> 3");
										QWidget *colorchip = nullptr;
										colorchip = CVWidgetGenerator::InsertWidgetInCenter(label);

										gridLayout->addWidget(colorchip, 0, 0, 1, 1, Qt::AlignHCenter);
										QPushButton* pushButton_2 = CVWidgetGenerator::CreatePushButton("", ADD_HOVER_ICON_PATH, "", PUSH_BUTTON_STYLE, 30, true);
										pushButton_2->setFixedHeight(20);
										pushButton_2->setFixedWidth(20);
										if (m_addColorButtonSignalMapper != nullptr)
										{
											m_addColorButtonSignalMapper->setProperty("TableName", tableName);
											connect(pushButton_2, SIGNAL(clicked()), m_addColorButtonSignalMapper, SLOT(map()));
											//int number = rowCount * 10 + columnIndex;
											m_addColorButtonSignalMapper->setMapping(pushButton_2, QString("%1-%2-%3").arg(rowCount).arg(columnIndex).arg(tableName));
											//m_buttonSignalMapper->setMapping(pushButton_2, number);
										}
										pushButton_2->setProperty("TableName", tableName);
										gridLayout->addWidget(pushButton_2, 0, 1, 1, 1, Qt::AlignHCenter);
										p_widget->setLayout(gridLayout);

										Logger::Debug("CreateProductBOMHandler -> AddBomRows() -> 10");
										sectionTable->setCellWidget(rowCount, columnIndex, p_widget);
									}
								}

							}

						}
						else
						{
							if (CreateProduct::GetInstance()->m_mappedColorways.contains(columnName))
							{
								QWidget* p_widget = new QWidget(sectionTable);
								QGridLayout* gridLayout = new QGridLayout(sectionTable);
								gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
								gridLayout->setContentsMargins(0, 0, 0, 0);

								QPixmap pixmap;
								Logger::Debug("CreateProductBOMHandler -> AddBomRows() -> 2");
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
								Logger::Debug("CreateProductBOMHandler -> AddBomRows() -> 3");
								QWidget *colorchip = nullptr;
								colorchip = CVWidgetGenerator::InsertWidgetInCenter(label);

								gridLayout->addWidget(colorchip, 0, 0, 1, 1, Qt::AlignHCenter);
								QPushButton* pushButton_2 = CVWidgetGenerator::CreatePushButton("", ADD_HOVER_ICON_PATH, "", PUSH_BUTTON_STYLE, 30, true);
								pushButton_2->setFixedHeight(20);
								pushButton_2->setFixedWidth(20);
								if (m_addColorButtonSignalMapper != nullptr)
								{
									m_addColorButtonSignalMapper->setProperty("TableName", tableName);
									connect(pushButton_2, SIGNAL(clicked()), m_addColorButtonSignalMapper, SLOT(map()));
									//int number = rowCount * 10 + columnIndex;
									m_addColorButtonSignalMapper->setMapping(pushButton_2, QString("%1-%2-%3").arg(rowCount).arg(columnIndex).arg(tableName));
									//m_buttonSignalMapper->setMapping(pushButton_2, number);
								}
								pushButton_2->setProperty("TableName", tableName);
								gridLayout->addWidget(pushButton_2, 0, 1, 1, 1, Qt::AlignHCenter);
								p_widget->setLayout(gridLayout);

								Logger::Debug("CreateProductBOMHandler -> AddBomRows() -> 10");
								sectionTable->setCellWidget(rowCount, columnIndex, p_widget);
							}
						}
					}
				}

			}
		}

		Logger::Debug("CreateProductBOMHandler -> UpdateColorwayColumns () End");
	}

	/*
Description - ClearBomData() method used to clear all the bom related variables.
	* Parameter -
	* Exception -
	*Return -
	*/
	void CreateProductBOMHandler::ClearBomData()
	{
		Logger::Debug("CreateProductBOMHandler -> ClearBomData() -> Start");
		m_addMaterialButtonAndTableMap.clear();
		m_addSpecialMaterialButtonAndTableMap.clear();
		m_colorwayMapForBom.clear();
		m_bomSectionTableInfoMap.clear();
		BOMUtility::m_mappedColorwaysArr.clear();
		AddNewBom::GetInstance()->m_BOMMetaData.clear();
		m_sectionMaterialTypeMap.clear();
		m_bomCreated = false;
		CreateProduct::GetInstance()->m_bomName->setText("");
		CreateProduct::GetInstance()->m_bomTemplateName->setText("");
		Logger::Debug("CreateProductBOMHandler -> ClearBomData() -> End");
	}

	bool CreateProductBOMHandler::ValidateBomFields()
	{
		return BOMUtility::ValidateBomFields(m_bomSectionTableInfoMap);
	}
}
