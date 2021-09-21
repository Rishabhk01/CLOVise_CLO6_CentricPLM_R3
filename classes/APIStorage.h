#pragma once

#include <vector>

#include "APIDefine.h"

using namespace std;

namespace CLOAPISample
{
	class APIStorage
	{
	private:
		static APIStorage* _instance;
		APIStorage();
		~APIStorage();
	
	public:
		static APIStorage*	getInstance();
		static void			destroy();

		vector<LibraryAPIItem*> m_LibraryAPIItemList;
		vector<LibraryAPIItem*> m_MaterialAPIItemList;
		vector<LibraryAPIItem*> m_TrimAPIItemList;
		vector<LibraryAPIItem*> m_ProductAPIItemList;

	private:
		void initialize();
		void clear();
		string m_FileName;

		void setFileSizeForAllItems();
		unsigned int getFileSize(const QString& filePath);

	public:
		void GenerateAPIItemListForSample();
		void SendFileName(string);

		vector<LibraryAPIItem> GetSearchList(const QString& searchText, const QVariantMap& searchKeyValues);
		
		bool GetItemWithID(const QString& itemId, LibraryAPIItem& result);
		
		bool GetFilePathWithID(const QString& itemId, QString& resultFilePath);
		bool GetIconThumbnailPathWithID(const QString& itemId, QString& resultFilePath);
		bool GetPreviewThumbnailPathWithID(const QString& itemId, QString& resultFilePath);

		//initialize metod for showning downloaded items from previous login session.
		void SetPLMAssetItemList(QString _itemId);

	private:
		void generateAPIItemListForPatterns();
		void generateAPIItemListForTrims();
		void generateAPIItemListForFabrics();
		void generateAPIItemListForProjectFiles();
		void generateAPIItemListForAvatars();
		void generateAPIItemListForGarments();
		void generateAPIItemListForTopstitches();
		void generateAPIItemListForButtonHeads();
		void generateAPIItemListForButtonHoles();
		void generateAPIItemListForZipperPulers(); // generate many items to show Scrolling Page sample
		void generateAPIItemListForZipperSliders();
		void generateAPIItemListForZipperTopStoppers();
		void generateAPIItemListForZipperBottomStoppers();

		//This method for showing downloaded items from previous login session.
		void generateAPIItemListForPLMProduct(string _directoryPath, QString _itemDataType);
		void generateAPIItemListForPLMMaterial(string _directoryPath, QString _itemDataType);
		void generateAPIItemListForPLMColor(string _directoryPath, QString _itemDataType);
		void generateAPIItemListForPLMTrims(string _directoryPath, QString _itemDataType);
		//End.

		LibraryAPIItem* newLibraryAPIItemWithRandomizedDate(); // constructing an instance for LibraryAPIItem with radomly set in 'Date' and return the pointer
	};

}

#define API_STORAGE (CLOAPISample::APIStorage::getInstance())
#define DESTROY_API_STORAGE (CLOAPISample::APIStorage::destroy())