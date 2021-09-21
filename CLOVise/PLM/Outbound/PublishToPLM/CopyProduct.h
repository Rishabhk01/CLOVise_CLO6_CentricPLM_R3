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
#include "ui_CopyProduct.h"

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
	class CopyProduct : public MVDialog, public Ui::CopyProduct
	{
		Q_OBJECT

	public:
		static CopyProduct* GetInstance();
		//static void	Destroy();
		void AddColorwayDetails(json _downloadJson);
		void ClearAllFields(QTreeWidget* _treeWidget);
		void SetDownloadedColorwayDetails();
		//void ExtractColorwayDetails(string _productId);
		QListWidget* m_ImageIntentList;
		//void SetTotalImageCount();
		//void uploadColorwayImages();
		//void LinkImagesToColorways();
		void UploadStyleThumbnail(string _productId);
		void AddStyleDetails(json _downloadJSON);
		struct ColorwayViews
		{
			string viewUploadId[4];
			int defaultImage = -1;
			string colorwayId;
		};
		bool IsCopyCreated();
		void SetCopyCreated(bool _copy);
		bool  m_copyCreated = false;
	private:
		CopyProduct(QWidget* parent = nullptr);
		virtual ~CopyProduct();

		void addCopyProductDetailsWidgetData();
		void drawCriteriaWidget(json _documentJson, QTreeWidget* _documentPushToPLMTree_1, QTreeWidget* _documentPushToPLMTree_2, json _metadataJson);
		void drawWidget(json _fieldJson, QTreeWidget* m_TreeWidget, int _start, int _end);
		string collectCriteriaFields(QTreeWidget* _documentPushToPLMTree_1, QTreeWidget* _documentPushToPLMTree_2);
		/*void PutRestCall(string _parameter, string _api, string _contentType);
		 size_t  WriteCallback(void* _contents, size_t _size, size_t _nmemb, string* _userp);
		size_t GetFileSize(const std::string& _path);*/
		void collectPublishToPLMFieldsData();
		void collectProductFieldsData();
		//void collect3DVisualFieldsData();
		//void collect3DTurntablesData();
		void collectCopyProductFieldsData();
		void exportZPRJ(string _productId);
		void exportGLB(bool _isGLBExportable);
		string getPublishRequestParameter(string _path, string _imageName);
		void connectSignalSlots(bool _b) override;
		void GetUpdatedColorwayNames();
		bool ValidateColorwayNameField();
		void AddRows(int _count, string _objectId, string _objectName, string _rgbValue, string _code, string _pantone, QStringList _colorwayNamesList);
		void ReadVisualUIFieldValue(QTreeWidget* _searchTree, int _rowIndex, string& _attJson);
		string uploadDocument(string _productId);
		void clearDependentComboBox(QString _lable, QTreeWidget* _searchTree);
		
		

		static CopyProduct* _instance;
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
		string m_ProductMetaData = "";
		json m_downloadJson = json::object();
		json m_checkedIds = json::object();
		string m_copydataRequest;
		QTreeWidget* m_CopyProductTreeWidget_1;
		QTreeWidget* m_CopyProductTreeWidget_2;
		QTreeWidget* m_HeirarchyTreeWidget;
		//QToolButton* m_dateResetButton;
		QPushButton* m_cancelButton;
		QPushButton* m_publishButton;
		QPushButton* m_CopyButton;
		QPushButton* m_addImageIntentButton;
		MVTableWidget *ui_ColorwayTable;
		MVTableWidget *m_imageIntentTable;
		QPushButton* m_searchStyleButton;

		int m_colorwayRowcount;
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
		QLabel* m_totalCountLabel;
		QStringList m_localAttributesList;
		string m_collectionId;
		//	QTabWidget* m_tabWidget;
		//	QWidget* m_overviewTab;

	private slots:
		void onBackButtonClicked();
		void onPublishToPLMClicked();
		void onAddColorwaysClicked();
		void onContextMenuClicked(const QPoint & _pos);
		void onSearchStyleClicked();
		void OnHandleDropDownValue(const QString& _item);
	//	void onAddImageIntentClicked();
		//void onTabClicked(int _index);

	};
}
