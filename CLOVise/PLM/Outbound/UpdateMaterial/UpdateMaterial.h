#pragma once
/*
* Copyright 2020-2021 CLO-Vise. All rights reserved
*
* @file UpdateMaterial.h
*
* @brief Class declaration for publish Product and Document from CLO to PLM.
* This class has all the variable declarations and function declarations which are used to PLM Product and Document instance data update and Publish from CLO to PLM.
*
* @author GoVise
*
* @date 14-AUG-2020
*/
#include "ui_UpdateMaterial.h"
#include <string>
#include <iostream>

#include<qstring.h>
#include <QDialog>
#include <QtGui>
#include <qtreewidget.h>
#include <QtCore>
#include <QtWidgets>

#include "CLOVise/PLM/Libraries/json.h"
#include "classes/widgets/MVDialog.h"

using json = nlohmann::json;
using namespace std;

namespace CLOVise
{
	class UpdateMaterial : public MVDialog, public Ui::UpdateMaterial
	{
		Q_OBJECT

	public:
		static UpdateMaterial* GetInstance();
		void UpdateMaterialWidget(bool _isFromConstructor);

	private:
		UpdateMaterial(QWidget* parent = nullptr);
		virtual ~UpdateMaterial();

		void addUpdateMaterialDetailsWidgetData();
		void drawCriteriaWidget(json _documentJson, QTreeWidget* _documentPushToPLMTree_1, QTreeWidget* _documentPushToPLMTree_2, json _metadataJson);
		void drawWidget(json _attributesJsonArray, QTreeWidget* _treeWidget, int _start, int _end);
		string collectCriteriaFields(QTreeWidget* _documentPushToPLMTree_1, QTreeWidget* _documentPushToPLMTree_2);
		void collectUpdateMaterialFieldsData();
		void collectUpdateMaterialTreeData();
		string getPublishRequestParameter(string _path, string _imageName);
		void addAttachmentClicked();
		void connectSignalSlots(bool _b) override;
		void DeletePointer(QWidget*_ptr);
		void UploadMaterialThumbnail(string _productId , string _fabricFileId);
		void ExtractThumbnailFromAttachment();
		string uploadDocument(string _productId);
		string exportFabricFile(string _revisionId, string _fabricFileId);
		void backButtonClicked();
		void publishToPLMClicked();
		void reject();
		void ClearAllFields(QTreeWidget* _documentPushToPLMTree);
		string getLatestVersionOfDocument(string _materialId);

		static UpdateMaterial* _instance;
		string m_MaterialThumbnailName = "";
		string m_MaterialThumbnailPath = "";
		QString m_zfabFilePath;
		string m_attachmentName;
		string m_zfabPath;
		string m_MaterialMetaData = "";
		json m_downloadedMaterialJson = json::object();

		QTreeWidget* m_UpdateMaterialTreeWidget_1;
		QTreeWidget* m_UpdateMaterialTreeWidget_2;
		QPushButton* m_cancelButton;
		QPushButton* m_publishButton;
		QPushButton* m_SaveAndCloseButton;
		QPushButton* m_addAttachmentButton;
		QTabWidget* m_tabWidget;
		QWidget* m_overviewTab;
		QWidget* m_attachmentTab;
		QLabel* m_attachmentNameLabel;
		QListWidget *m_attachmentsList;
		bool m_isSaveClicked = false;

	private slots:
		void onBackButtonClicked();
		void onPublishToPLMClicked();
		void onAddAttachmentClicked();
		void onSaveAndCloseClicked();
	};
}

