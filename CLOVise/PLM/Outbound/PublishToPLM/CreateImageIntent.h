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
#include "ui_CreateImageIntent.h"

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
	class CreateImageIntent : public MVDialog, public Ui::CreateImageIntent
	{
		Q_OBJECT

	public:
		static CreateImageIntent* GetInstance();
		static void	Destroy();
		int m_imageQueueTableRowCount;
		MVTableWidget *m_imageQueueTable;
		/*void AddColorwayDetails(json _downloadJson);
		void SetDownloadedColorwayDetails();*/

		void fillSelectedList(QStringList _colorwayNames);
		struct ColorwayViews
		{
			string viewUploadId[4];
			int defaultImage = -1;
			string colorwayId;
			QStringList imageLabels;
			map<int, QStringList> viewLabelMap;
		}colorwayViewStruct;
		map<string, ColorwayViews> m_ColorwayViewMap;
		QStringList m_colorwayselectedList;
		QStringList m_colorwayViewQueue;
		void ClearAllFields();
	private:
		CreateImageIntent(QWidget* parent = nullptr);
		virtual ~CreateImageIntent();
		void connectSignalSlots(bool _b) override;
		
		QStringList m_exportedColorwayImages;
		
		//void addProductWidgetData();
		//void drawCriteriaWidget(json _documentJson, QTreeWidget* _documentPushToPLMTree, json _metadataJson);
		//void drawWidget(json _attJson, QTreeWidget* _documentPushToPLMTree, string _attValue, string _attKey, bool _isEditable);
		//json collectCriteriaFields(QTreeWidget* _documentPushToPLMTree);

		//void collectPublishToPLMFieldsData();
		//void collectProductFieldsData();
		//void reWriteUpdatedMetadataInto3dGarment();
		////void collect3DVisualFieldsData();
		////void collect3DTurntablesData();
		//void collectCreateProductFieldsData();
		//void exportZPRJ();
		//void getPublishRequestParameter();
		//void connectSignalSlots(bool _b) override;
		//void GetUpdatedColorwayNames();
		//bool ValidateColorwayNameField();
		//void AddRows(int _count, string _objectId, string _objectName, string _rgbValue, string _objectStatus, QStringList _colorwayNamesList);
		//

		static CreateImageIntent* _instance;
		json m_parameterJson;
		json m_productDetailsJson = json::object();
		json m_imagePagesDetailsJson = json::object();
		json m_turnTableDetailsJson = json::object();
		json m_documentDetailsJson = json::object();
		string m_3DModelFileName = "";
		string m_3DModelThumbnailName = "";
		string m_3DModelName = "";
		string m_imagePagesImageWidth = "";
		string m_imagePagesImageHeight = "";
		string m_noOfimagePageImages = "";
		string m_imagePagesName = "";
		string m_imagePagesZipName = "";

		string m_turntableImageWidth = "";
		string m_turntableImageHeight = "";
		string m_noOfturntableImages = "";
		string m_turntableName = "";
		string m_turntableZipName = "";
		string m_imagePagesImagesPath = "";
		string m_turntableImagesPath = "";
		string m_3DModelFilePath = "";
		string m_3DModelThumbnailPath = "";
		string m_multipartFilesParams = "";
		json m_documentsMetaDataJson = "";		

		QTreeWidget* m_updateProductTreeWidget;
		//QToolButton* m_dateResetButton;
		QPushButton* m_backButton;
		QPushButton* m_finishButton;
		QPushButton* m_addToQueueButton;
		QTabWidget* m_tabWidget;
		QWidget* m_overviewTab;
		QPushButton* m_SaveAndCloseButton;

		MVTableWidget *ui_ColorwayTable;
		QPushButton* m_colorwayAddButton;
		int m_colorwayRowcount;
		QStringList m_modifiedColorwayNames;
		QStringList m_selectedColorwayNames;
		bool m_isSaveClicked;
		bool m_addClicked;


		QComboBox*  m_colorwayComboBox;
		QListWidget* m_viewList;
		QListWidget* m_labelList;
		
		
		QCheckBox* m_setDefaultCheckBox;
		//QWidget* m_includeAvatarCheckBox;
		map<string, string > m_ImageDataMap;
		QStringList m_SelectedColorwayList;
		QStringList m_ColorwayImageLabelList;
		QStringList m_StyleImageLabelList;
		map<QString, QString> m_colorwayImageLabelsMap;
		map<QString, QString> m_styleImageLabelsMap;

	private slots:
		void onAddToQueueButtonClicked();
		void onFinishButtonClicked();
		void onBackButtonClicked();
		void OnStyleOrColorwayDropdownClicked(const QString& _item);
	//	void onUpdateButtonClicked();
	//	void onAddColorwaysClicked();
	//	void onContextMenuClicked(const QPoint & _pos);
	//	void onSaveAndCloseClicked();
	};
}
