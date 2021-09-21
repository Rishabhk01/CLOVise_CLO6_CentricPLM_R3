#pragma once
/*
* Copyright 2020-2021 CLO-Vise. All rights reserved
*
* @file FlexTypeHelper.h
*
* @brief Class Flex PLM objects configuration helper used in CLO draw object UI's.
* This class has all the variable and function implementation which are Flex PLM objects configuration helper used in CLO draw object UI's.
* like search, results UI's.
*
* @author GoVise
*
* @date 27-MAY-2020
*/
#include <string>
#include <set>

#include <QTreeWidgetItem>
#include <qstring.h>

#include "CLOVise/PLM/Helper/Util/Helper.h"
#include "CLOVise/PLM/Libraries/json.h"
#include "CLOVise/PLM/Helper/UIHelper/CVWidgetGenerator.h"

using namespace std;
using json = nlohmann::json;

class FlexTypeHelper {

public:
	static QString CreateFlexTypeTree(json _hierarchyJson,
		QTreeWidget* _treeWidget_H, set<string>& _flextypeTree)
	{
		QString selectedRoot;
		try {
			QTreeWidgetItem* dataitem = new QTreeWidgetItem(_treeWidget_H);
			_treeWidget_H->addTopLevelItem(dataitem);

			string displayName = Helper::GetJSONValue<string>(_hierarchyJson, TYPEDISPLAYNAME_JSON_KEY, true);
			string typeName = Helper::GetJSONValue<string>(_hierarchyJson, TYPENAME_JSON_KEY, true);
			string subType = Helper::GetJSONValue<string>(_hierarchyJson, SUB_FLEX_TYPE, false);
			
			dataitem->setText(0, QString::fromStdString(displayName));
			dataitem->setData(0, Qt::UserRole, QString::fromStdString(to_string(_hierarchyJson)));
			dataitem->setData(1, Qt::UserRole, QString::fromStdString(typeName));
			selectedRoot = QString::fromStdString(typeName);
			_flextypeTree.insert(typeName);

			json subTypeJson = json::parse(subType);
			if (subTypeJson.size() != 0)
			{
			CreateChildNodes(subTypeJson, dataitem, _flextypeTree);
			}
		}
		catch (const char* e)
		{
			throw e;
		}
		catch (string e)
		{
			throw e;
		}
		catch (exception& e)
		{
			throw e;
		}

		return selectedRoot;
	};

	/*
	* Description - CreateChildNodes() method used to create child nodes of the hierarchy.
	* Parameter -  json, QTreeWidgetItem, string, vector<string>.
	* Exception - 
	* Return -
	*/
	static void CreateChildNodes(json& _subTypeJson, QTreeWidgetItem* _parentNode,
		set<string>& _flextypeTree)
	{
		for (int i = 0; i < _subTypeJson.size(); i++)
		{
			json subType = _subTypeJson[i];
			string typeName = Helper::GetJSONValue<string>(subType, TYPENAME_JSON_KEY, true);
			if (std::find(_flextypeTree.begin(), _flextypeTree.end(), typeName) == _flextypeTree.end())
			{
				string displayName = Helper::GetJSONValue<string>(subType, TYPEDISPLAYNAME_JSON_KEY, true);
				string subTypeStr = Helper::GetJSONValue<string>(subType, SUB_FLEX_TYPE, false);
				QTreeWidgetItem* dataitem = new QTreeWidgetItem(QStringList(QObject::tr(displayName.c_str())));
				dataitem->setText(0, QString::fromStdString(displayName));
				dataitem->setData(0, Qt::UserRole, QString::fromStdString(subType.dump()));
				dataitem->setData(1, Qt::UserRole, QString::fromStdString(typeName));
				_parentNode->addChild(dataitem);
				_flextypeTree.insert(typeName);

				json subTypeJson = json::parse(subTypeStr);
				if (subTypeJson.size() != 0)
				{
					CreateChildNodes(subTypeJson, dataitem, _flextypeTree);
				}
			}
		}
		if (_parentNode->parent() != nullptr)
		{
			json p2pJson = json::parse(_parentNode->parent()->data(0, Qt::UserRole).toString().toStdString());
			string subTypeStr = Helper::GetJSONValue<string>(p2pJson, SUB_FLEX_TYPE, false);
			json subTypeJson = json::parse(subTypeStr);
			CreateChildNodes(subTypeJson, _parentNode->parent(), _flextypeTree);
		}
	}

	/*
	* Description - DrawDefaultSearchCriteriaWidget() method used to draw default search criteria widget.
	* Parameter -  json, string, QTreeWidget, QStringList.
	* Exception - 
	* Return -
	*/
	static void DrawDefaultSearchCriteriaWidget(json _searchjson, string _selectedType, QTreeWidget* _SearchTree_1, QTreeWidget* _SearchTree_2, QStringList _attScops)
	{
		json feildsJson = json::object();
		json attributesJson = json::object();
		string typeName = "", attributesTemPType = "";
		for (int searchFeildsCount = 0; searchFeildsCount < _searchjson.size(); searchFeildsCount++)
		{
			feildsJson = Helper::GetJSONParsedValue<int>(_searchjson, searchFeildsCount, false);
			for (int searchFeildsCount = 0; searchFeildsCount < feildsJson.size(); searchFeildsCount++)
			{
				typeName = Helper::GetJSONValue<string>(feildsJson, TYPENAME_JSON_KEY, true);
				/*if (selectedType == typeName)
				{*/
					attributesJson = Helper::GetJSONParsedValue<string>(feildsJson, FILTER_ATTRIBUTES_KEY, false);
					CVWidgetGenerator::CreateSearchCriteriaWidget(_SearchTree_1, _SearchTree_2, attributesJson, true, true, _attScops);
				/*}*/
			}
		}

	}
};