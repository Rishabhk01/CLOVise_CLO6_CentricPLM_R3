#pragma once

#include <QString>
#include <QVariant>

const QString META_DATA_KEY_0_DATA_TYPE = QString("Data Type");
const QString META_DATA_KEY_1_DATA_TYPE = QString("Type");
const QString META_DATA_KEY_1_SERVICE_DIVISION = QString("CLO Service Division");
const QString META_DATA_KEY_2_BRAND = QString("Brand");
const QString META_DATA_KEY_3_PRODUCT_TYPE = QString("Product Type");
const QString META_DATA_KEY_3_SHAPE_TYPE = QString("Shape Type");
const QString META_DATA_KEY_4_SEASON_CREATED = QString("Season Created");
const QString META_DATA_KEY_5_NAME = QString("Name");
const QString META_DATA_KEY_6_ID = QString("Id");
const QString META_DATA_KEY_7_NUMBER = QString("Status");
const QString META_DATA_KEY_17_COLOR_SUBTYPE = QString("SubType");

namespace CLOAPISample
{
	struct LibraryAPISampleItem
	{
		QString itemPath; // for sample only
		QString iconThumbnailPath; // for sample only
		QString previewThumbnailPath; // for sample only

		LibraryAPISampleItem() : itemPath(""), iconThumbnailPath(""), previewThumbnailPath("") {}
	};

	struct LibraryAPIItem
	{
		QString itemID;
		QString itemName;
		QString itemType;	
		QString dateTime;
		unsigned int filesize;
		QVariantMap metaData; // user custom field (key - value list)

		LibraryAPISampleItem sampleItemData;
		// designated initializer 
		LibraryAPIItem() :itemID(""), itemName(""), itemType("FILE"), dateTime(""), filesize(0.0f), metaData(QVariantMap()) {}
	};
}

#ifdef __APPLE__
	const QString SAMPLE_STORAGE_DIRECTORY = QString("/Users/inoblaze/sample_assets/");
#else
	const QString SAMPLE_STORAGE_DIRECTORY = QString("C:/sample_assets/");
#endif  __APPLE__
