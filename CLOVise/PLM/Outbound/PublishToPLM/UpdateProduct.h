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
#include "ui_UpdateProduct.h"

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
	class UpdateProduct : public MVDialog, public Ui::UpdateProduct
	{
		Q_OBJECT

	public:
		static UpdateProduct* GetInstance();
		void AddColorwayDetails(QStringList _downloadIdList, json _jsonarray);
		bool UpdateColorInColorways(QStringList _downloadIdList, json _jsonarray);
		void ClearAllFields(QTreeWidget* _documentPushToPLMTree);
		//void SetColorwayDetails(QStringList _downloadIdList, string _module, json _jsonarray);
		void CreateAndUpdateColorways(string _productId);
		void addCreateProductDetailsWidgetData();
		QListWidget* m_ImageIntentList;
		void SetTotalImageCount();
		void uploadColorwayImages();
		void LinkImagesToColorways(string _productId);
		void UploadStyleThumbnail(string _productId);
		void GetUpdatedColorwayNames();
		void DeleteColorwayFromPLM();
		void GetcolorwayDetails();
		void GetMappedColorway();
		bool GetIsSaveClicked();
		void AddMaterialInBom();
		void FillImageIntentIdAndLabeMap(json _imageJson, string _module);
		void FillNonCloImageMap(json _imageJson,string _id);
		void FillNonCloColorwayImageMap(json _imageJson, string _imageId, QString _colorwayId);
		struct ColorwayViews
		{
			string viewUploadId[4];
			int defaultImage = -1;
			string colorwayId;
			QStringList imageLabels;
			map<int, QStringList> viewLabelMap;
		};
		struct ImageIntentsDetails
		{
			QString colorwayName;
			QString view;
			QString viewName;
			QString labels;
			QString isdefault;

		} m_imageIntentsDetails;
		void FillImageIntentsStruct(string _colorway, string _view, string _viewName, string _labels, string _default);

		void AddRowInImageIntentTab(QPixmap _pixMap, ImageIntentsDetails _imageIntentsDetails, string _imageId);
		void SetUpdateBomFlag(bool _flag);
		MVTableWidget *ui_colorwayTable;
		std::map<QString, QString> m_colorwayImageLabelsMap;
		std::map<QString, QStringList> m_nonCloStyleImageLabelsMap;
		std::map<QString, QString> m_styleNodeNamedisplayNameMap;
		std::map<QString, map<QString, QStringList>>m_nonCloColorWayImageLabelsMap;
		map<QString, QStringList > m_nonCloColorwayImagesMap;
		std::map<QString, QString> m_styleImageLabelsMap;
		std::map<string, string> m_digiCodeNamesMap;
		std::map<string, string> m_CloAndPLMColorwayMap;
		QStringList m_digiCodeValue;

		QSignalMapper *m_updateColorButtonSignalMapper;
		QSignalMapper *m_editButtonSignalMapper;
		QSignalMapper *m_deleteSignalMapper;
		QSignalMapper *m_updateColorwayDeleteSignalMapper;
		QSignalMapper *m_createActionSignalMapper;
		QSignalMapper *m_printActionSignalMapper;

		int m_selectedRow;
		QStringList m_colorSpecList;
		json m_downloadedStyleJson;
		QStringList m_ColorwayTableColumnNames;
		QStringList m_ImageIntentsColumnsNames;
		int m_ImageIntentRowIndexForEdit;
		bool m_editButtonClicked;
		MVTableWidget *m_imageIntentTable;
		int m_imageIntentRowcount;
		int m_currentViewSelected;
		int  m_selectedStyleTypeIndex = 0;
		QPushButton* m_bomAddButton;
		QLabel *m_bomName;
		QLabel *m_bomTemplateName;
		QStringList m_mappedColorways;
		string m_currentlySelectedStyleTypeId;
		bool m_updateBomTab;
	private:
		UpdateProduct(QWidget* parent = nullptr);
		virtual ~UpdateProduct();


		void drawCriteriaWidget(json _documentJson, QTreeWidget* _documentPushToPLMTree_1, QTreeWidget* _documentPushToPLMTree_2, json _metadataJson);
		void drawWidget(json _feildsJson, QTreeWidget* m_TreeWidget, int _start, int _end);
		string collectCriteriaFields(QTreeWidget* _documentPushToPLMTree_1, QTreeWidget* _documentPushToPLMTree_2);
		/*void PutRestCall(string _parameter, string _api, string _contentType);
		 size_t  WriteCallback(void* _contents, size_t _size, size_t _nmemb, string* _userp);
		size_t GetFileSize(const std::string& _path);*/
		void collectPublishToPLMFieldsData();
		void collectProductFieldsData();
		//void collect3DVisualFieldsData();
		//void collect3DTurntablesData();
		void collectCreateProductFieldsData();
		void exportZPRJ(string _productId);
		void exportGLBFile(string _productId);
		void exportGLB(bool _isGLBExportable);
		string getPublishRequestParameter(string _path, string _imageName);
		void connectSignalSlots(bool _b) override;
		bool ValidateColorwayNameField();
		void AddRows(int _count, string _objectId, string _objectName, string _rgbValue, string _code, string _pantone, QStringList _colorwayNamesList, json _colorwayJson = NULL, string _colorwayId = "");
		json ReadVisualUIFieldValue(QTreeWidget* _searchTree, int _rowIndex);
		string uploadDocument(string _productId);
		string uploadGLBFile(string _productId);
		void clearDependentComboBox(QString _lable, QTreeWidget* _searchTree);
		void reject();
		string reviseDocument(string _revisionId);
		void ClearColorwayTable();
		//void hideButtonClicked(bool _hide);
		void drawColorwayImageList(string _selectedColorwayId, string _colorwayName, string _module);
		void showNoImageAvailable();
		void ShowImageIntent();
		void exportTurntableImages();

		//void UpdateColorwayDetails(string _productId);
		static UpdateProduct* _instance;
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
		string m_productMetaData;
		json m_downloadJson = json::object();
		json m_checkedIds = json::object();

		QTreeWidget* m_updateProductTreeWidget_1;
		QTreeWidget* m_updateProductTreeWidget_2;
		//QToolButton* m_dateResetButton;
		QPushButton* m_cancelButton;
		QPushButton* m_updateColorwayDeleteButton;
		QPushButton* m_publishButton;
		QPushButton* m_SaveAndCloseButton;
		QPushButton* m_addImageIntentButton;


		QPushButton* m_colorwayAddButton;
		bool m_downloadedColorway;
		json m_downloadedColorwayJson;
		int m_colorwayRowcount;
		bool m_isCreateColorSpec = false;
		QStringList m_modifiedColorwayNames;
		QStringList m_selectedColorwayNames;
		bool m_isSaveClicked;
		bool m_addClicked;
		map<string, string > m_seasonNameIdMap;
		map<string, string> m_brandNameIdMap;
		map<string, string> m_dipartmentNameIdMap;
		map<string, string> m_styleTypeNameIdMap;
		map<string, string> m_developmentTypeNameIdMap;
		map<string, string> m_curPriorityNameIdMap;
		map<string, string> m_downloadedColorwayIdMap;
		map<string, QStringList > m_imageIntentIdAndLabeMap;
		string m_collectionId;
		QLabel* m_totalCountLabel;
		

		QStringList m_DeletedColorwayList;
		QStringList m_NewlyAddedColorway;
		QStringList m_localAttributesList;
		QStringList m_plmAttributesList;
		QListWidget* m_colorwayImageList;

		string m_perveiouselySelectedId;
		json m_colorwayJson;
		json m_clientSpecificJson;
		bool m_isColorwayImageListHidden;
		string m_currentColorSpec;
		bool m_2DigiCodeActive;
		//	QTabWidget* m_tabWidget;
		//	QWidget* m_overviewTab;

	private slots:
		void onBackButtonClicked();
		void onPublishToPLMClicked();
		void onAddColorwaysClicked();
		void onSaveAndCloseClicked();
		void OnHandleDropDownValue(const QString& _item);
		void OnClickUpdateColorButton(int i);
		void onAddImageIntentClicked();
		void onTabClicked(int _index);
		void OnAddColorwayClicked();
		void OnColorwaysTableDeleteButtonClicked(int _row);
		//	void onTableRadioButtonSelected(int _row);
		//void onHideButtonClicked(bool _hide);
		void onImageIntentsTableEditButtonClicked(int row);
		void onImageIntentsTableDeleteButtonClicked(int row);
		void OnUni2CodeSelected(const QString& _str);
		void OnplmColorwayNameEntered();
		void onImageIntentsTableHorizontalHeaderClicked(int column);
		void onColorwayTableHorizontalHeaderClicked(int column);
		void OnCreateColorSpecClicked(int _row);
		void OnSearchPrintClicked(int _row);
		void onAddNewBomClicked();
	};
}
