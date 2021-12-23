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
#include "ui_CreateProduct.h"

#include <string>
#include <iostream>

#include<qstring.h>
#include <QDialog>
#include <QtGui>
#include <qtreewidget.h>
#include <QtCore>
#include <QtWidgets>
#include "classes/widgets/MVTableWidget.h"
#include "CLOVise/PLM/Libraries/json.h"
#include "classes/widgets/MVDialog.h"

using json = nlohmann::json;
using namespace std;

namespace CLOVise
{
	class CreateProduct : public MVDialog, public Ui::CreateProduct
	{
		Q_OBJECT

	public:
		static CreateProduct* GetInstance();
		static void	Destroy();
		void AddColorwayDetails();
		void ClearAllFields(QTreeWidget* _documentPushToPLMTree);
		//void SetColorwayDetails(QStringList _downloadIdList, string _module, json _jsonarray);
		void ExtractColorwayDetails(string _productId);
		QListWidget* m_ImageIntentList;
		void SetTotalImageCount();
		void uploadColorwayImages();
		void LinkImagesToColorways(string _productId);
		void UploadStyleThumbnail(string _productId);
		void GetUpdatedColorwayNames();
		bool UpdateColorInColorways(QStringList _downloadIdList, json _jsonarray);
		//void DrawCriteriaWidget(bool _isFromConstructor);
		void addCreateProductDetailsWidgetData();
		void AddMaterialInBom();
		void GetMappedColorway();
		void UpdateColorwayColumnsInBom();
		void ExportTechPack();
		void ClearBomSectionLayout();
		void CreateBom(string _productId);
		struct ColorwayViews
		{
			string viewUploadId[4];
			int defaultImage = -1;
			string colorwayId;
		};
		MVTableWidget *ui_colorwayTable;
		int m_selectedRow;
		QStringList m_colorSpecList;
		void ResetCreateProductData();
		QLabel *m_bomName;
		QLabel *m_bomTemplateName;
		QPushButton* m_bomAddButton;
		json m_techPackJson;
		bool m_updateBomTab;
		void SetUpdateBomFlag(bool _flag);
		QStringList m_mappedColorways;
		string m_currentlySelectedStyleTypeId;
		json collectCriteriaFields(QTreeWidget* _documentPushToPLMTree_1, QTreeWidget* _documentPushToPLMTree_2);
	private:
		CreateProduct(QWidget* parent = nullptr);
		virtual ~CreateProduct();

		
		void drawCriteriaWidget(json _documentJson, QTreeWidget* _documentPushToPLMTree_1, QTreeWidget* _documentPushToPLMTree_2, json _metadataJson);
		void drawWidget(json _feildsJson, QTreeWidget* m_TreeWidget, int _start, int _end);
		
		/*void PutRestCall(string _parameter, string _api, string _contentType);
		 size_t  WriteCallback(void* _contents, size_t _size, size_t _nmemb, string* _userp);
		size_t GetFileSize(const std::string& _path);*/
		void collectPublishToPLMFieldsData();
		void collectProductFieldsData();
		//void collect3DVisualFieldsData();
		//void collect3DTurntablesData();
		void collectCreateProductFieldsData();
		void exportZPRJ(string _productId);
		void exportGLB(bool _isGLBExportable);
		string getPublishRequestParameter(string _path, string _imageName);
		void connectSignalSlots(bool _b) override;
		bool ValidateColorwayNameField();
		void AddRows(int _count, string _objectId, string _objectName, string _rgbValue, string _code, string _pantone, QStringList _colorwayNamesList);
		json ReadVisualUIFieldValue(QTreeWidget* _searchTree, int _rowIndex);
		string uploadDocument(string _productId);
		void clearDependentComboBox(QString _lable, QTreeWidget* _searchTree);
		void reject();
		void exportTurntableImages();
		void uploadGLBFile(string _productId);

		static CreateProduct* _instance;
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
		json m_ProductMetaData = json::object();
		json m_BomMetaData = json::object();
		json m_downloadJson = json::object();
		json m_checkedIds = json::object();

		QTreeWidget* m_createProductTreeWidget_1;
		QTreeWidget* m_createProductTreeWidget_2;
		//QToolButton* m_dateResetButton;
		QPushButton* m_cancelButton;
		QSignalMapper* m_colorwayDeleteSignalMapper;
		QPushButton* m_publishButton;
		QPushButton* m_SaveAndCloseButton;
		QPushButton* m_addImageIntentButton;
	
		MVTableWidget *m_imageIntentTable;
		QPushButton* m_colorwayAddButton;
		

		int m_colorwayRowcount;
		QStringList m_modifiedColorwayNames;
		QStringList m_selectedColorwayNames;
		QStringList m_localAttributesList;
		QStringList m_plmAttributesList;

		
		bool m_addClicked;
		map<string, string > m_seasonNameIdMap;
		map<string, string> m_brandNameIdMap;
		map<string, string> m_dipartmentNameIdMap;
		map<string, string> m_collectionNameIdMap;
		map<string, string> m_shapeNameIdMap;
		map<string, string> m_themeNameIdMap;
		map<string, string> m_styleTypeNameIdMap;
		map<string, string> m_developmentTypeNameIdMap;
		map<string, string> m_curPriorityNameIdMap;
		map<string, string> m_styleTypeBomPermissionMap;
		std::map<QString, QString> m_colorwayImageLabelsMap;
		std::map<QString, QString> m_styleImageLabelsMap;
		std::map<string, string> m_digiCodeNamesMap;
		std::map<string, string> m_CloAndPLMColorwayMap;
		QStringList m_digiCodeValue;
		string m_collectionId;
		QLabel* m_totalCountLabel;
		json m_creatProductFieldsJson = json::object();
		QSignalMapper *m_buttonSignalMapper;
		QSignalMapper *m_createActionSignalMapper;
		QSignalMapper *m_printActionSignalMapper;
		QStringList m_ColorwayTableColumnNames;
		string m_currentColorSpec;
		json m_clientSpecificJson;
		bool m_2DigiCodeActive;
		QLineEdit* m_plmColowayName;
		bool m_isCreateColorSpec = false;
		int  m_selectedStyleTypeIndex = 0;
		int  m_prevSelectedStyleTypeIndex = 0;

	private slots:
		void onBackButtonClicked();
		void onPublishToPLMClicked();
		void onAddColorwaysClicked();
		void onContextMenuClicked(const QPoint & _pos);
		void onSaveAndCloseClicked();
		void OnHandleDropDownValue(const QString& _item);
		void onAddImageIntentClicked();
		void onTabClicked(int _index);
		void OnClickAddColorButton(int i);
		void OnUni2CodeSelected(const QString& _str);
		void OnplmColorwayNameEntered();
		void onHorizontalHeaderClicked(int column);
		void OnCreateColorSpecClicked(int _row);
		void OnSearchPrintClicked(int _row);
		void OnColorwaysTableDeleteButtonClicked(int _row);
		void onAddNewBomClicked();

	};
}
