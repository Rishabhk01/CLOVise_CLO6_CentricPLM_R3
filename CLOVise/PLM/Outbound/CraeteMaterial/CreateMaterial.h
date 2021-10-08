#pragma once
/*
* Copyright 2020-2021 CLO-Vise. All rights reserved
*
* @file CreateMaterial.h
*
* @brief Class declaration for publish Product and Document from CLO to PLM.
* This class has all the variable declarations and function declarations which are used to PLM Product and Document instance data update and Publish from CLO to PLM.
*
* @author GoVise
*
* @date 14-AUG-2020
*/
#include "ui_CreateMaterial.h"

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
	class CreateMaterial : public MVDialog, public Ui::CreateMaterial
	{
		Q_OBJECT

	public:
		static CreateMaterial* GetInstance();
		QTreeWidget* GetTreewidget(int _index);
		void ClearAllFields(QTreeWidget* _documentPushToPLMTree);

		bool m_isSaveClicked;
		void DrawCriteriaWidget(bool _isFromConstructor);

	private:
		CreateMaterial(QWidget* parent = nullptr);
		virtual ~CreateMaterial();

		void addCreateMaterialDetailsWidgetData();
		void drawCriteriaWidget(json _documentJson, QTreeWidget* _documentPushToPLMTree_1, QTreeWidget* _documentPushToPLMTree_2, json _metadataJson);
		//void drawWidget(json _attJson, QTreeWidget* _documentPushToPLMTree, string _attValue, string _attKey, bool _isEditable);
		string collectCriteriaFields(QTreeWidget* _documentPushToPLMTree_1, QTreeWidget* _documentPushToPLMTree_2);
		void collectCreateMaterialFieldsData();
		void collectCreateMaterialTreeData();
		string getPublishRequestParameter(string _path, string _imageName);
		void addAttachmentClicked();
		void connectSignalSlots(bool _b) override;
		void DeletePointer(QWidget*_ptr);
		void UploadMaterialThumbnail(string _productId,string _fabricFileId);
		void ExtractThumbnailFromAttachment();
		string uploadDocument(string _productId);
		string exportFabricFile(string _revisionId);
		void backButtonClicked();
		void publishToPLMClicked();
		void reject();
		void connectOnHandleDropDownWidget(QTreeWidget* _documentPushToPLMTree);
		void processTreeWidgetForDependency(QString _item, QString _senderCombo, QTreeWidget* _documentPushToPLMTree);
		void clearDependentFields(QString _lable, QTreeWidget* _documentPushToPLMTree);

		static CreateMaterial* _instance;
		string m_MaterialThumbnailName = "";
		string m_MaterialThumbnailPath = "";
		QString m_zfabFilePath;
		string m_attachmentName;
		string m_zfabPath;
		string m_MaterialMetaData = "";		

		QTreeWidget* m_CreateMaterialTreeWidget_1;
		QTreeWidget* m_CreateMaterialTreeWidget_2;
		QPushButton* m_cancelButton;
		QPushButton* m_publishButton;
		QPushButton* m_SaveAndCloseButton;
		QPushButton* m_addAttachmentButton;
		QTabWidget* m_tabWidget;
		QWidget* m_overviewTab;
		QWidget* m_attachmentTab;
		QLabel* m_attachmentNameLabel;
		QListWidget *m_attachmentsList;
	private slots:
		void onBackButtonClicked();
		void onPublishToPLMClicked();
		void onAddAttachmentClicked();
		void onSaveAndCloseClicked();
		void OnHandleDropDownValue(const QString& _item);
	};
}
