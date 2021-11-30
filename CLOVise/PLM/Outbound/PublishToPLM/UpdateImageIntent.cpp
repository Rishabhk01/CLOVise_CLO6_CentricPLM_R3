/*
* Copyright 2020-2021 CLO-Vise. All rights reserved
*
* @file PublishToPLM.cpp
*
* @brief Class implementation for publish Product and Document from CLO to PLM.
* This class has all the variable and methods implementation which are used to PLM Product and Document instance data update and Publish from CLO to PLM.
*
* @author GoVise
*
* @date 14-AUG-2020
*/
#include "UpdateImageIntent.h"

#include <cstring>

#include <QTextEdit>
#include <QComboBox>
#include <QLabel>
#include "qtreewidget.h"
#include <QFile>
#include "qdir.h"

#ifdef __APPLE__

#include "zlib.h"
#include "CLOVise/PLM/Libraries/zlib/include/zip.h"
#include "CLOVise/PLM/Libraries/zlib/zipper/zipper.h"

#if defined(MSDOS) || defined(OS2) || defined(WIN32) || defined(__CYGWIN__)
#  include <fcntl.h>
#  include <io.h>
#  define SET_BINARY_MODE(file) setmode(fileno(file), O_BINARY)
#else
#  define SET_BINARY_MODE(file)
#endif

#define CHUNK 16384

using namespace zipper;

#else

#include "CLOVise/PLM/Libraries/ZipLib/ZipFile.h"
#include "CLOVise/PLM/Libraries/ZipLib/streams/memstream.h"
#include "CLOVise/PLM/Libraries/ZipLib/methods/Bzip2Method.h"

#endif

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
#include "CLOVise/PLM/Outbound/PublishToPLM/UpdateProduct.h"

using namespace std;

namespace CLOVise
{
	UpdateImageIntent* UpdateImageIntent::_instance = NULL;

	UpdateImageIntent* UpdateImageIntent::GetInstance()
	{
		if (_instance == NULL) {
			_instance = new UpdateImageIntent();
		}

		return _instance;
	}

	void UpdateImageIntent::Destroy()
	{
		if (_instance) {
			delete _instance;
			_instance = NULL;
		}
	}

	UpdateImageIntent::UpdateImageIntent(QWidget* parent) : MVDialog(parent)
	{
		setupUi(this);
		Logger::Debug("UpdateImageIntent -> Constructor() -> Start");
		/*
		if (!PublishToPLMData::GetInstance()->isModelExecuted)
			RESTAPI::SetProgressBarData(15, "Loading Update product", true);*/

		QString windowTitle = "Add Image Intents ";
		this->setWindowTitle(windowTitle);

#ifdef __APPLE__
		this->setWindowFlags(Qt::Dialog | Qt::CustomizeWindowHint | Qt::WindowMinMaxButtonsHint | Qt::WindowCloseButtonHint);
#else
		this->setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint | Qt::CustomizeWindowHint);
		m_pTitleBar = new MVTitleBar(windowTitle, this);
		layout()->setMenuBar(m_pTitleBar);
#endif // !__APPLE__

		m_backButton = nullptr;
		m_finishButton = nullptr;
		m_imageQueueTableRowCount = 0;

		m_updateProductTreeWidget = CVWidgetGenerator::CreatePublishTreeWidget("QTreeWidget { background-color: #262628; border: 1px solid #000;}""QTreeWidget::item { padding :5px; height: 20px; color: #FFFFFF; font-face: ArialMT; font-size: 10px; width: 90px; margin-left: 5px; margin-right: 5px; margin-top: 5px; margin-bottom: 5px; border: none;}""QTreeWidget::item:hover{background-color: #262628;}", true);
		m_updateProductTreeWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

		m_backButton = CVWidgetGenerator::CreatePushButton("Back", BACK_HOVER_PATH, "Back", PUSH_BUTTON_STYLE, 30, true);
		//m_finishButton = CVWidgetGenerator::CreatePushButton("Finish", SEARCH_HOVER_ICON_PATH, "Finish", PUSH_BUTTON_STYLE, 30, true);
		//m_addToQueueButton = CVWidgetGenerator::CreatePushButton("Add to Upload Queue", SEARCH_HOVER_ICON_PATH, "Add to Upload Queue", PUSH_BUTTON_STYLE, 30, true);
		m_finishButton = CVWidgetGenerator::CreatePushButton("Finish", OK_HOVER_ICON_PATH, "Finish", PUSH_BUTTON_STYLE, 30, true);
		m_addToQueueButton = CVWidgetGenerator::CreatePushButton("Add to Upload Queue", ADD_HOVER_ICON_PATH, "Add to Upload Queue", PUSH_BUTTON_STYLE, 30, true);

		QLabel* selectColorwayLabel = CVWidgetGenerator::CreateLabel("1: Select Style/Colorway", QString::fromStdString(BLANK), HEADER_STYLE, true);
		selectColorwayLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
		QLabel* assignLabel = CVWidgetGenerator::CreateLabel("2: Assign label(s)", QString::fromStdString(BLANK), HEADER_STYLE, true);
		assignLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
		QLabel* imageQueueLabel = CVWidgetGenerator::CreateLabel("Image Intent Queue", QString::fromStdString(BLANK), HEADER_STYLE, true);
		imageQueueLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

		m_setDefaultCheckBox = CVWidgetGenerator::CreateCheckBox("", "", true);
		m_setDefaultCheckBox->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

		//m_includeAvatarCheckBox = CVWidgetGenerator::CreateCheckBoxWidget(false, "", "", "");
	   // m_includeAvatarCheckBox->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

		QLabel* defaultImageLabel = CVWidgetGenerator::CreateLabel("Set as Default Image", QString::fromStdString(BLANK), HEADER_STYLE, true);
		defaultImageLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

		//QLabel* includeAvatarLabel = CVWidgetGenerator::CreateLabel("Include Avatar", QString::fromStdString(BLANK), HEADER_STYLE, true);
		//includeAvatarLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

		m_colorwayComboBox = CVWidgetGenerator::CreateComboBox(QString::fromStdString("Style & Colorway"), "", true);
		m_colorwayComboBox->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
		m_colorwayComboBox->setMaximumWidth(200);
		m_colorwayComboBox->setMinimumWidth(200);

		QSpacerItem *horizontalSpacer = new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Expanding);
		QSpacerItem *horizontalSpacer1 = new QSpacerItem(1, 1, QSizePolicy::Fixed, QSizePolicy::Fixed);
		QSpacerItem *horizontalSpacer2 = new QSpacerItem(5, 1, QSizePolicy::Fixed, QSizePolicy::Fixed);

		m_imageQueueTable = new MVTableWidget();
		m_imageQueueTable->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
		//m_imageQueueTable->horizontalHeader()->setStyleSheet("QHeaderView { font-face: ArialMT; font-size: 10px;background-color:#2D2D2F; color: #FFFFFF; font-weight: bold;}" "QHeaderView::section:horizontal{border: 1px solid #000000;} QHeaderView::down-arrow{ width: 5px; height:4px; }");
		m_imageQueueTable->horizontalHeader()->setStretchLastSection(true);
		//->verticalHeader()->setStyleSheet("QHeaderView { color: #FFFFFF;font-weight: bold; background-color:#2D2D2F;}" "QHeaderView::section:vertical{border: 1px solid #000000;}");

		m_imageQueueTable->setStyleSheet("QTableWidget{ background-color: #262628; border-right: 1px solid #000000; border-top: 1px solid #000000; border-left: 1px solid #000000; font-face: ArialMT; font-size: 12px; color: #FFFFFF; }"
			"QTableCornerButton::section{border: 1px solid #000000; background-color: #262628; }""QTableWidget::item{ border-bottom: 2px solid #000000; font-face: ArialMT; font-size: 12px; color: #FFFFFF; }"
			"QToolTip{ color: #46C8FF; background-color: #33414D; border: 1px #000000; }""QTableWidget::item:selected{ background-color: #33414D; color: #46C8FF; }""QScrollBar::add-page:vertical { background: #000000; }"
			"QScrollBar::sub-page:vertical {background: #000000;}""QScrollBar::add-page:horizontal { background: #000000; }""QScrollBar::sub-page:horizontal { background: #000000; }");
		//CVWidgetGenerator::InitializeTableView(m_imageQueueTable);
		//m_imageQueueTable->hide();
		m_imageQueueTable->horizontalHeader()->hide();
		m_imageQueueTable->verticalHeader()->hide();
		CVWidgetGenerator::InitializeTableView(m_imageQueueTable);
		CVHoverDelegate* customHoverDelegate = new CVHoverDelegate(m_imageQueueTable);
		m_imageQueueTable->setItemDelegate(customHoverDelegate);

		horizontalLayout->insertWidget(1, assignLabel);
		horizontalLayout->insertWidget(0, selectColorwayLabel);

		QHBoxLayout* checkBoxLayout = new QHBoxLayout();

		horizontalLayout_2->insertWidget(0, m_colorwayComboBox);
		checkBoxLayout->insertWidget(0, m_setDefaultCheckBox);
		checkBoxLayout->insertWidget(1, defaultImageLabel);
		checkBoxLayout->insertSpacerItem(2, horizontalSpacer1);
		//checkBoxLayout->insertWidget(3, m_includeAvatarCheckBox);
		//checkBoxLayout->insertWidget(4, includeAvatarLabel);

		horizontalLayout_2->insertLayout(1, checkBoxLayout);


		m_viewList = new QListWidget();
		m_labelList = new QListWidget();
		QString listStyleSheet = "QListWidget{ border-bottom: 1px inset #262626; font-size: 10px; font-face: ArialMT; border-left: 1px inset #0D0D0D; border-top: 1px inset #0D0D0D; border-right: 1px inset #262626; background-color: #222224; } QListWidget::item{ background-color: #222224; margin: 5px; height: 20px;}";//max-height:100px;
		m_viewList->setStyleSheet(listStyleSheet);
		m_labelList->setStyleSheet(listStyleSheet);
		m_viewList->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);



		m_colorwayImageLabelsMap = Configuration::GetInstance()->GetColorwayImageLabels();
		m_styleImageLabelsMap = Configuration::GetInstance()->GetStyleImageLabels();
		Logger::Debug("UpdateImageIntent -> Constructor() -> m_colorwayImageLabelsMap size " + to_string(m_colorwayImageLabelsMap.size()));

		QStringList snapshotList{ "Front",  "Right" , "Back","Left" };
		for (int i = 0; i < snapshotList.size(); i++)
		{
			QRadioButton* radioButton = new QRadioButton;
			QListWidgetItem* listItem = new QListWidgetItem();
			radioButton->setCheckable(true);
			radioButton->setText(snapshotList[i]);
			radioButton->setAttribute(Qt::WA_MacShowFocusRect, false);
			radioButton->setParent(m_viewList);
			radioButton->setStyleSheet("QRadioButton{spacing: 7px; margin-right: 5px;}");
			/*	radioButton->setProperty(COLOR_ID_KEY.c_str(), QString::fromStdString(objectId));
				radioButton->setProperty(OBJECT_ID_KEY.c_str(), QString::fromStdString(objectId));*/
			m_viewList->addItem(listItem);
			m_viewList->setItemWidget(listItem, radioButton);

		}

		Logger::Debug("UpdateImageIntent -> Constructor() -> m_styleImageLabelsMap size " + to_string(m_styleImageLabelsMap.size()));
		for (auto it = m_styleImageLabelsMap.begin(); it != m_styleImageLabelsMap.end(); it++)
		{
			QCheckBox* checkbox = new QCheckBox;
			QListWidgetItem* listItem = new QListWidgetItem();
			checkbox->setCheckable(true);
			checkbox->setText(it->first);
			checkbox->setProperty(IMAGE_LABEL_KEY.c_str(), it->second);
			checkbox->setAttribute(Qt::WA_MacShowFocusRect, false);
			checkbox->setParent(m_viewList);
			checkbox->setStyleSheet("QRadioButton{spacing: 7px; margin-right: 5px;}");
			/*	radioButton->setProperty(COLOR_ID_KEY.c_str(), QString::fromStdString(objectId));
				radioButton->setProperty(OBJECT_ID_KEY.c_str(), QString::fromStdString(objectId));*/
			m_labelList->addItem(listItem);
			m_labelList->setItemWidget(listItem, checkbox);
		}

		//	m_ImageIntentList = new QListWidget();
		m_labelList->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);


		horizontalLayout_3->insertWidget(0, m_viewList);
		horizontalLayout_3->insertSpacerItem(1, horizontalSpacer2);
		horizontalLayout_3->insertWidget(2, m_labelList);


		horizontalLayout_4->insertSpacerItem(0, horizontalSpacer);
		horizontalLayout_4->insertWidget(1, m_addToQueueButton);


		horizontalLayout_5->insertWidget(0, imageQueueLabel);
		horizontalLayout_5->insertSpacerItem(1, horizontalSpacer);

		horizontalLayout_8->addWidget(m_imageQueueTable);

		ui_buttonsLayout->insertSpacerItem(0, horizontalSpacer);
		ui_buttonsLayout->insertWidget(1, m_backButton);
		ui_buttonsLayout->insertSpacerItem(2, horizontalSpacer1);
		ui_buttonsLayout->insertWidget(3, m_finishButton);
		ui_buttonsLayout->insertSpacerItem(4, horizontalSpacer);

		/*gridLayout_3->addWidget(m_colorwayComboBox, 1, 0);
		gridLayout_3->addWidget(m_colorwayComboBox, 1, 0);*/
		connectSignalSlots(true);

		/*int colorwayIndex = 0;
		int colorwayCount = UTILITY_API->GetColorwayCount();
		string temporaryPath = UTILITY_API->GetCLOTemporaryFolderPath();
		for (colorwayIndex = 0; colorwayIndex < colorwayCount; colorwayIndex++)
		{
			string colorwayName = UTILITY_API->GetColorwayName(colorwayIndex);
			UTILITY_API->SetCurrentColorwayIndex(colorwayIndex);
			string filepath = temporaryPath + "CLOViseTurntableImages/" + colorwayName + ".png";
			EXPORT_API->ExportTurntableImages(filepath, 4, 480, 640);
		}*/

		//gridLayout_3->addItem();

		Logger::Debug("UpdateImageIntent -> Constructor() -> End");
		/*if (!PublishToPLMData::GetInstance()->isModelExecuted)*/
		//RESTAPI::SetProgressBarData(0, "", false);
#ifdef __APPLE__
		RESTAPI::SetProgressBarData(0, "", false);
#endif
	}

	UpdateImageIntent::~UpdateImageIntent()
	{

	}
	//{
	//	Logger::Debug("UpdateProduct-> Destructor() Start....");
	//	UIHelper::DeletePointer(m_updateProductTreeWidget);
	//	UIHelper::DeletePointer(m_cancelButton);
	//	UIHelper::DeletePointer(m_publishButton);
	//	UIHelper::DeletePointer(m_tabWidget);
	//	UIHelper::DeletePointer(m_overviewTab);
	//	UIHelper::DeletePointer(m_SaveAndCloseButton);
	//	UIHelper::DeletePointer(ui_ColorwayTable);
	//	UIHelper::DeletePointer(m_colorwayAddButton);
	//	Logger::Debug("UpdateProduct-> Destructor() End....");
	//	
	//}

	/*
	* Description - ResetDateEditWidget() method used to Reset Date Widget.
	* Parameter -
	* Exception -
	* Return -
	*/
	//void CreateProduct::onResetDateEditWidget()
	//{
	//	//UIHelper::ResetDate(m_createProductTreeWidget);
	//}

	/*
	* Description - connectSignalSlots() method used to connect and disconnect signal and slots.
	* Parameter -  bool.
	* Exception -
	* Return -
	*/
	void UpdateImageIntent::connectSignalSlots(bool _b)
	{
		if (_b)
		{
			QObject::connect(m_addToQueueButton, SIGNAL(clicked()), this, SLOT(onAddToQueueButtonClicked()));
			QObject::connect(m_finishButton, SIGNAL(clicked()), this, SLOT(onFinishButtonClicked()));
			QObject::connect(m_backButton, SIGNAL(clicked()), this, SLOT(onBackButtonClicked()));
			QObject::connect(m_colorwayComboBox, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(OnStyleOrColorwayDropdownClicked(const QString&)));




		}
		else
		{
			QObject::disconnect(m_addToQueueButton, SIGNAL(clicked()), this, SLOT(onAddToQueueButtonClicked()));
			QObject::disconnect(m_finishButton, SIGNAL(clicked()), this, SLOT(onFinishButtonClicked()));
			QObject::disconnect(m_backButton, SIGNAL(clicked()), this, SLOT(onBackButtonClicked()));
			QObject::disconnect(m_colorwayComboBox, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(OnStyleOrColorwayDropdownClicked(const QString&)));
		}
	}

	void UpdateImageIntent::OnStyleOrColorwayDropdownClicked(const QString& _item)
	{
		StyleOrColorwayDropdownClicked(_item);
	}

	void UpdateImageIntent::StyleOrColorwayDropdownClicked(const QString& _item)
	{
		std::map<QString, QString> checkBoxItemListMap;
		if (_item.contains("No Colorway"))
		{
			checkBoxItemListMap = m_styleImageLabelsMap;
		}
		else
		{
			checkBoxItemListMap = m_colorwayImageLabelsMap;
		}
		m_labelList->clear();

		for (auto it = checkBoxItemListMap.begin(); it != checkBoxItemListMap.end(); it++)
		{
			QCheckBox* checkbox = new QCheckBox;
			QListWidgetItem* listItem = new QListWidgetItem();
			checkbox->setCheckable(true);
			checkbox->setText(it->first);
			checkbox->setProperty(IMAGE_LABEL_KEY.c_str(), it->second);
			checkbox->setAttribute(Qt::WA_MacShowFocusRect, false);
			checkbox->setParent(m_viewList);
			checkbox->setStyleSheet("QRadioButton{spacing: 7px; margin-right: 5px;}");

			m_labelList->addItem(listItem);
			m_labelList->setItemWidget(listItem, checkbox);
		}
	}

	void UpdateImageIntent::onAddToQueueButtonClicked()
	{
		Logger::Debug("UpdateImageIntent -> onAddToQueueButtonClicked() -> Start");


		QString selectedColorway = m_colorwayComboBox->currentText();
		m_colorwayselectedList.append(selectedColorway);
		string  throwValue = "";
		QString selectedSnapshot;
		QStringList selectedImageLabels;
		string tempValue = "";
		QListWidgetItem* listItem = nullptr;
		map<string, ColorwayViews>::iterator it;
		Logger::Debug("UpdateImageIntent -> onAddToQueueButtonClicked() -> m_viewList->count()" + to_string(m_viewList->count()));

		try {

			for (int index = 0; index < m_viewList->count(); index++)
			{
				auto radioButon = static_cast<QRadioButton*>(m_viewList->itemWidget(m_viewList->item(index)));
				if (radioButon->isChecked())
				{
					selectedSnapshot = radioButon->text();
					break;
				}

			}

			for (int index = 0; index < m_labelList->count(); index++)
			{
				auto checkbox = static_cast<QCheckBox*>(m_labelList->itemWidget(m_labelList->item(index)));
				if (checkbox->isChecked())
				{
					selectedImageLabels << checkbox->text();
				}
			}



			ColorwayViews colorwayView;
			int view;
			if (selectedSnapshot.contains("Back"))
				view = BACK_VIEW;
			else if (selectedSnapshot.contains("Front"))
				view = FRONT_VIEW;
			else if (selectedSnapshot.contains("Left"))
				view = LEFT_VIEW;
			else
				view = RIGHT_VIEW;
			if (IsValidImageIntent(selectedSnapshot, selectedImageLabels, selectedColorway, view))
			{
				

				Logger::Debug("UpdateImageIntent -> onAddToQueueButtonClicked() ->View1 : " + to_string(view));

				it = m_ColorwayViewMap.find(selectedColorway.toStdString());
				if (it != m_ColorwayViewMap.end())
				{
					Logger::Debug("UpdateImageIntent -> onAddToQueueButtonClicked() -> 1" + to_string(view));
					colorwayView = it->second;
					colorwayView.viewUploadId[view] = "1";
					if (m_setDefaultCheckBox->isChecked())
						colorwayView.defaultImage = view;
					colorwayView.viewLabelMap.insert(make_pair(view, selectedImageLabels));
					Logger::Debug("UpdateImageIntent -> onAddToQueueButtonClicked() -> colorwayView.viewUploadId[view]:" + colorwayView.viewUploadId[view]);
					it->second = colorwayView;
				}
				else
				{
					Logger::Debug("UpdateImageIntent -> onAddToQueueButtonClicked() -> 2");
					colorwayView.viewUploadId[view] = "1";
					if (m_setDefaultCheckBox->isChecked())
						colorwayView.defaultImage = view;

					colorwayView.viewLabelMap.insert(make_pair(view, selectedImageLabels));
					Logger::Debug("UpdateImageIntent -> onAddToQueueButtonClicked() -> colorwayView.viewUploadId[view]:" + colorwayView.viewUploadId[view]);
					m_ColorwayViewMap.insert(make_pair(selectedColorway.toStdString(), colorwayView));
				}

				m_imageQueueTable->setColumnCount(4);
				m_imageQueueTable->setRowCount(++m_imageQueueTableRowCount);
				m_imageQueueTable->show();

				//m_colorwayViewQueue.append(selectedColorway + selectedSnapshot);
				QTableWidgetItem* colorwayItem = new QTableWidgetItem("Colorway: " + selectedColorway);
				colorwayItem->setTextAlignment(Qt::AlignCenter);
				m_imageQueueTable->setItem(m_imageQueueTableRowCount - 1, 0, colorwayItem);

				QTableWidgetItem* nameWidget = new QTableWidgetItem("View: " + selectedSnapshot);
				nameWidget->setTextAlignment(Qt::AlignCenter);
				m_imageQueueTable->setItem(m_imageQueueTableRowCount - 1, 1, nameWidget);


				QTableWidgetItem* LabelWidget = new QTableWidgetItem("Label: " + selectedImageLabels.join(","));
				LabelWidget->setTextAlignment(Qt::AlignCenter);
				m_imageQueueTable->setItem(m_imageQueueTableRowCount - 1, 2, LabelWidget);



				QString defaultcheck;
				if (m_setDefaultCheckBox->isChecked())
					defaultcheck = "Default Image: Yes";
				else
					defaultcheck = "Default Image: No";

				QTableWidgetItem* defaultCheckbox = new QTableWidgetItem(defaultcheck);
				defaultCheckbox->setTextAlignment(Qt::AlignCenter);
				m_imageQueueTable->setItem(m_imageQueueTableRowCount - 1, 3, defaultCheckbox);

				Logger::Debug("UpdateImageIntent -> onAddToQueueButtonClicked() -> m_colorwayselectedList.size()" + to_string(m_colorwayselectedList.size()));
			}
			else
				UTILITY_API->DisplayMessageBox("Already selected");
			Logger::Debug("UpdateImageIntent -> onAddToQueueButtonClicked() -> End");
		}
		catch (string str)
		{
			RESTAPI::SetProgressBarData(0, "", false);
			Logger::Error("Create product-> Create product Exception - " + str);
			UTILITY_API->DisplayMessageBox(str);
			this->show();
		}
	}

	void UpdateImageIntent::onFinishButtonClicked()
	{
		Logger::Debug("UpdateImageIntent -> onFinishButtonClicked() -> Start");
		try {
			string temporaryPath = UTILITY_API->GetCLOTemporaryFolderPath();

			if (UpdateProduct::GetInstance()->m_editButtonClicked)// For edit image intent
			{
				int rowIndex = UpdateProduct::GetInstance()->m_ImageIntentRowIndexForEdit;
				QPushButton *editButton = static_cast<QPushButton*>(UpdateProduct::GetInstance()->m_imageIntentTable->cellWidget(rowIndex, EDIT_COLUMN)->children().last());
				string selectedColorwayName = m_colorwayComboBox->currentText().toStdString();
				Logger::Debug("UpdateImageIntent -> selectedColorwayName() " + selectedColorwayName);
				int cloColorwaySelectedIndex = UTILITY_API->GetCurrentColorwayIndex();
				int colorwayCount = UTILITY_API->GetColorwayCount();
				int colorwayIndex = 0;
				for (colorwayIndex = 0; colorwayIndex < colorwayCount; colorwayIndex++)
				{
					string colorwayName = UTILITY_API->GetColorwayName(colorwayIndex);
					if (selectedColorwayName.compare(colorwayName) == 0)
					{
						UTILITY_API->SetCurrentColorwayIndex(colorwayIndex);
						string filepath = temporaryPath + "CLOViseTurntableImages/" + selectedColorwayName + ".png";

						EXPORT_API->ExportTurntableImages(filepath, 4, 480, 640);
						break;
					}
				}

				// To generate turntable images for the style with the currently selected colorway (needed for style image labels)
				UTILITY_API->SetCurrentColorwayIndex(cloColorwaySelectedIndex);
				string filepath = temporaryPath + "CLOViseTurntableImages/" + "No Colorway(Default)" + ".png";
				EXPORT_API->ExportTurntableImages(filepath, 4, 480, 640);
				//////
				QString selectedSnapshot;
				QStringList selectedImageLabels;
				for (int index = 0; index < m_viewList->count(); index++)
				{
					Logger::Debug("UpdateImageIntent -> onFinishButtonClicked() -> 2");
					auto radioButon = static_cast<QRadioButton*>(m_viewList->itemWidget(m_viewList->item(index)));
					if (radioButon->isChecked())
					{
						Logger::Debug("UpdateImageIntent -> onFinishButtonClicked() -> 3");
						selectedSnapshot = radioButon->text();
						break;
					}

				}
				Logger::Debug("UpdateImageIntent -> onFinishButtonClicked() -> 4");
				for (int index = 0; index < m_labelList->count(); index++)
				{
					Logger::Debug("UpdateImageIntent -> onFinishButtonClicked() -> 5");
					auto checkbox = static_cast<QCheckBox*>(m_labelList->itemWidget(m_labelList->item(index)));
					if (checkbox->isChecked())
					{
						selectedImageLabels << checkbox->text();
					}
				}
				string defaultcheck;
				if (m_setDefaultCheckBox->isChecked())
					defaultcheck = "Yes";
				else
					defaultcheck = "No";

				int view;
				if (selectedSnapshot.contains("Back"))
					view = BACK_VIEW;
				else if (selectedSnapshot.contains("Front"))
					view = FRONT_VIEW;
				else if (selectedSnapshot.contains("Left"))
					view = LEFT_VIEW;
				else
					view = RIGHT_VIEW;

				if (IsValidImageIntent(selectedSnapshot, selectedImageLabels, QString::fromStdString(selectedColorwayName), view))
				{

					QString labels = selectedImageLabels.join(',');
					//QString displayDetails = "Colorway : " + QString::fromStdString(colorwayName) + "\nView : " + selectedSnapshot + "\nImage Name: " + QString::fromStdString("") + "\n Image Labels: " + labels;					
					UpdateProduct::GetInstance()->m_imageIntentTable->item(rowIndex, COLORWAY_COLUMN)->setText(QString::fromStdString(selectedColorwayName));
					UpdateProduct::GetInstance()->m_imageIntentTable->item(rowIndex, IMAGE_VIEW_COLUMN)->setText(selectedSnapshot);
					UpdateProduct::GetInstance()->m_imageIntentTable->item(rowIndex, LABELS_COLUMN)->setText(labels);
					UpdateProduct::GetInstance()->m_imageIntentTable->item(rowIndex, DEFAULT_COLUMN)->setText(QString::fromStdString(defaultcheck));

					Logger::Debug("UpdateImageIntent -> onFinishButtonClicked() -> 8");
					map<string, ColorwayViews> ::iterator mapIterator;
					ColorwayViews colorwayView;
					Logger::Debug("UpdateImageIntent -> onFinishButtonClicked() -> 9");
					mapIterator = m_ColorwayViewMap.find(selectedColorwayName);


					Logger::Debug("UpdateImageIntent -> onFinishButtonClicked() -> view" + to_string(view));


					editButton->setProperty("view", QString::fromStdString(to_string(view)));
					editButton->setProperty("defaultImage", QString::fromStdString(defaultcheck));

					QString filepath = QString::fromStdString(temporaryPath) + "CLOViseTurntableImages/" + QString::fromStdString(selectedColorwayName) + "_" + QString::fromStdString(to_string(view)) + ".png";
					Logger::Debug("UpdateImageIntent -> onFinishButtonClicked() -> filepath1" + filepath.toStdString());
					QPixmap pix(filepath);
					pix.scaled(QSize(80, 80), Qt::KeepAspectRatio);

					QWidget *pColorWidget = nullptr;
					QLabel* label = new QLabel();
					label->setMaximumSize(QSize(80, 80));
					int w = label->width();
					int h = label->height();
					label->setPixmap(QPixmap(pix.scaled(w, h, Qt::KeepAspectRatio)));
					pColorWidget = CVWidgetGenerator::InsertWidgetInCenter(label);
					UpdateProduct::GetInstance()->m_imageIntentTable->setCellWidget(rowIndex, IMAGE_INTENT_COLUMN, pColorWidget);
					Logger::Debug("UpdateImageIntent -> onFinishButtonClicked() -> UpdateProduct::GetInstance()->m_currentViewSelected " + to_string(UpdateProduct::GetInstance()->m_currentViewSelected));

					if (mapIterator != m_ColorwayViewMap.end())
					{
						if (m_setDefaultCheckBox->isChecked())
						{
							Logger::Debug("UpdateImageIntent -> onFinishButtonClicked() -> In side checked view" + to_string(view));
							mapIterator->second.defaultImage = view;
						}
						else
						{
							Logger::Debug("UpdateImageIntent -> onFinishButtonClicked() -> In side Not checked view" + to_string(view));
							if (mapIterator->second.defaultImage != -1)
								mapIterator->second.defaultImage = -1;
						}
						mapIterator->second.viewLabelMap[view] = selectedImageLabels;
						mapIterator->second.viewUploadId[view] = "1";
						if (UpdateProduct::GetInstance()->m_currentViewSelected != view)
							mapIterator->second.viewUploadId[UpdateProduct::GetInstance()->m_currentViewSelected] = "";

						m_ColorwayViewMap.insert(make_pair(selectedColorwayName, mapIterator->second));
						Logger::Debug("UpdateImageIntent -> onFinishButtonClicked() -> 11");
					}

					Logger::Debug("UpdateImageIntent -> onFinishButtonClicked() -> colorwayView.viewUploadId[0]" + mapIterator->second.viewUploadId[0]);
					Logger::Debug("UpdateImageIntent -> onFinishButtonClicked() -> colorwayView.viewUploadId[1]" + mapIterator->second.viewUploadId[1]);
					Logger::Debug("UpdateImageIntent -> onFinishButtonClicked() -> colorwayView.viewUploadId[2]" + mapIterator->second.viewUploadId[2]);
					Logger::Debug("UpdateImageIntent -> onFinishButtonClicked() -> colorwayView.viewUploadId[3]" + mapIterator->second.viewUploadId[3]);
					Logger::Debug("UpdateImageIntent -> onFinishButtonClicked() -> colorwayView.defaultImage" + to_string(mapIterator->second.defaultImage));

					UpdateProduct::GetInstance()->m_editButtonClicked = false;
					m_colorwayComboBox->setDisabled(false);
					m_colorwayComboBox->setStyleSheet(COMBOBOX_STYLE);
					m_addToQueueButton->show();
				}
				else
					UTILITY_API->DisplayMessageBox("Already selected");
			}
			else
			{
				m_colorwayselectedList.removeDuplicates();
				Logger::Debug("UpdateImageIntent -> onFinishButtonClicked() -> m_colorwayselectedList.size()" + to_string(m_colorwayselectedList.size()));
				int cloColorwaySelectedIndex = UTILITY_API->GetCurrentColorwayIndex();
				int colorwayCount = UTILITY_API->GetColorwayCount();
				for (int i = 0; i < m_colorwayselectedList.size(); i++)
				{
					if (!m_exportedColorwayImages.contains(m_colorwayselectedList[i]))
					{
						m_exportedColorwayImages.append(m_colorwayselectedList[i]);
						Logger::Debug("UpdateImageIntent -> onFinishButtonClicked() -> Exporting: " + m_colorwayselectedList[i].toStdString());
						int colorwayIndex = 0;
						for (colorwayIndex = 0; colorwayIndex < colorwayCount; colorwayIndex++)
						{
							string colorwayName = UTILITY_API->GetColorwayName(colorwayIndex);
							if (m_colorwayselectedList[i].toStdString().compare(colorwayName) == 0)
							{
								UTILITY_API->SetCurrentColorwayIndex(colorwayIndex);
								string filepath = temporaryPath + "CLOViseTurntableImages/" + m_colorwayselectedList[i].toStdString() + ".png";

								EXPORT_API->ExportTurntableImages(filepath, 4, 480, 640);
								break;
							}
						}


					}
				}

				// To generate turntable images for the style with the currently selected colorway (needed for style image labels)
				UTILITY_API->SetCurrentColorwayIndex(cloColorwaySelectedIndex);
				string filepath = temporaryPath + "CLOViseTurntableImages/" + "No Colorway(Default)" + ".png";
				EXPORT_API->ExportTurntableImages(filepath, 4, 480, 640);
				//////

				string clorwayname;
				string viewName;
				string lable;
				string defaultImage;
				int view;
				Logger::Debug("UpdateImageIntent -> onFinishButtonClicked() -> m_imageQueueTable->rowCount()" + to_string(m_imageQueueTable->rowCount()));
				Logger::Debug("UpdateImageIntent -> onFinishButtonClicked() -> m_imageQueueTable->columnCount()" + to_string(m_imageQueueTable->columnCount()));
				for (int index = 0; index < m_imageQueueTable->rowCount(); index++)
				{
					//	QListWidget* list = new QListWidget();
						//QListWidgetItem* listItem = new QListWidgetItem();
					for (int colIndex = 0; colIndex < m_imageQueueTable->columnCount(); colIndex++)
					{
						Logger::Debug("UpdateImageIntent -> onFinishButtonClicked() -> 1");
						QTableWidgetItem* item = m_imageQueueTable->item(index, colIndex);
						Logger::Debug("UpdateImageIntent -> onFinishButtonClicked() -> Item" + item->text().toStdString());
						if (colIndex == COLORWAY_NAME_COLUMN)
						{
							string text;
							text = item->text().toStdString();
							int length = text.length();
							int indexOfColon = text.find(":");
							clorwayname = text.substr(indexOfColon + 2, length);
							Logger::Debug("UpdateImageIntent -> onFinishButtonClicked() -> clorwayname" + clorwayname);

						}
						if (colIndex == VIEW_COLUMN)
						{
							string text;
							text = item->text().toStdString();
							int length = text.length();
							int indexOfColon = text.find(":");
							viewName = text.substr(indexOfColon + 1, length);
							Logger::Debug("UpdateImageIntent -> onFinishButtonClicked() -> viewName" + viewName);

							if (viewName.find("Back") != -1)
								view = BACK_VIEW;
							else if (viewName.find("Front") != -1)
								view = FRONT_VIEW;
							else if (viewName.find("Left") != -1)
								view = LEFT_VIEW;
							else
								view = RIGHT_VIEW;
						}
						if (colIndex == IMAGE_LABLE_COLUMN)
						{
							string text;
							text = item->text().toStdString();
							int length = text.length();
							int indexOfColon = text.find(":");
							lable = text.substr(indexOfColon + 1, length);
							Logger::Debug("UpdateImageIntent -> onFinishButtonClicked() -> label" + lable);
						}
						if (colIndex == DEFAULT_IMAGE_COLUMN)
						{
							string text;
							text = item->text().toStdString();
							int length = text.length();
							int indexOfColon = text.find(":");
							defaultImage = text.substr(indexOfColon + 1, length);
							Logger::Debug("UpdateImageIntent -> onFinishButtonClicked() -> label" + defaultImage);
						}

					}
					/*if (m_colorwayViewQueue.contains(QString::fromStdString(clorwayname) + QString::fromStdString(viewName).trimmed()))
						continue;
					else
					m_colorwayViewQueue.append(QString::fromStdString(clorwayname) + QString::fromStdString(viewName).trimmed());*/
					//string displayDetails = "Colorway : " + clorwayname + "\nView : " + viewName + "\n Image Labels: " + lable + "\n Default: " + defaultImage;
					UpdateProduct::GetInstance()->FillImageIntentsStruct(clorwayname, to_string(view), viewName, lable, defaultImage);

					QString filepath = QString::fromStdString(temporaryPath) + "CLOViseTurntableImages/" + QString::fromStdString(clorwayname) + "_" + QString::fromStdString(to_string(view)) + ".png";
					Logger::Debug("UpdateImageIntent -> onFinishButtonClicked() -> filepath" + filepath.toStdString());

					QPixmap pix(filepath);
					pix.scaled(QSize(80, 80), Qt::KeepAspectRatio);


					Logger::Debug("UpdateImageIntent -> onFinishButtonClicked() -> UpdateProduct::GetInstance()->m_imageIntentRowcount" + to_string(UpdateProduct::GetInstance()->m_imageIntentRowcount));

					UpdateProduct::GetInstance()->AddRowInImageIntentTab(pix, UpdateProduct::GetInstance()->m_imageIntentsDetails, "");

					UpdateProduct::GetInstance()->SetTotalImageCount();


				}
			}
			//m_colorwayViewQueue.clear();
			if (m_viewList->count() > 0)
			{
				for (int index = 0; index < m_viewList->count(); index++)
				{

					auto radioButon = static_cast<QRadioButton*>(m_viewList->itemWidget(m_viewList->item(index)));
					radioButon->setAutoExclusive(false);
					radioButon->setChecked(false);
					radioButon->setAutoExclusive(true);
				}

			}

			if (m_labelList->count() > 0)
			{
				for (int index = 0; index < m_labelList->count(); index++)
				{
					auto checkbox = static_cast<QCheckBox*>(m_labelList->itemWidget(m_labelList->item(index)));
					checkbox->setChecked(false);
				}
			}
			m_setDefaultCheckBox->setChecked(false);
			this->hide();
			UpdateProduct::GetInstance()->setModal(true);
			UpdateProduct::GetInstance()->show();
		}
		catch (string str)
		{
			RESTAPI::SetProgressBarData(0, "", false);
			Logger::Error("Create product-> Create product Exception - " + str);
			UTILITY_API->DisplayMessageBox(str);
			this->show();
		}

		Logger::Debug("UpdateImageIntent -> onFinishButtonClicked() -> End");
	}

	void UpdateImageIntent::onBackButtonClicked()
	{
		Logger::Debug("UpdateImageIntent -> onBackButtonClicked() -> Start");
		//m_ColorwayViewMap.clear();
		//m_colorwayViewQueue.clear();
		m_addToQueueButton->show();
		UpdateProduct::GetInstance()->m_editButtonClicked = false;
		if (m_viewList->count() > 0)
		{
			for (int index = 0; index < m_viewList->count(); index++)
			{

				auto radioButon = static_cast<QRadioButton*>(m_viewList->itemWidget(m_viewList->item(index)));
				radioButon->setAutoExclusive(false);
				radioButon->setChecked(false);
				radioButon->setAutoExclusive(true);
			}

		}

		if (m_labelList->count() > 0)
		{
			for (int index = 0; index < m_labelList->count(); index++)
			{
				auto checkbox = static_cast<QCheckBox*>(m_labelList->itemWidget(m_labelList->item(index)));
				checkbox->setChecked(false);
			}
		}
		m_setDefaultCheckBox->setChecked(false);
		this->hide();
		UpdateProduct::GetInstance()->setModal(true);
		UpdateProduct::GetInstance()->show();
		Logger::Debug("UpdateImageIntent -> onBackButtonClicked() -> End");
	}

	void UpdateImageIntent::fillSelectedList(QStringList _colorwayNames)
	{
		Logger::Debug("UpdateImageIntent -> fillSelectedList() -> Start");
		m_colorwayComboBox->clear();
		m_colorwayComboBox->addItem("No Colorway(Default)");
		m_colorwayComboBox->addItems(_colorwayNames);
		m_colorwayComboBox->setCurrentText(QString::fromStdString("No Colorway(Default)"));
		Logger::Debug("UpdateImageIntent -> fillSelectedList() -> End");
	}
	void UpdateImageIntent::ClearAllFields()
	{
		Logger::Debug("UpdateImageIntent -> ClearAllFields() -> Start");

		/*if (m_viewList->count() > 0)
		{
			for (int index = 0; index < m_viewList->count(); index++)
			{
				auto radioButon = static_cast<QRadioButton*>(m_viewList->itemWidget(m_viewList->item(index)));
				radioButon->setChecked(false);
			}

		}

		if (m_labelList->count() > 0)
		{
			for (int index = 0; index < m_labelList->count(); index++)
			{
				auto checkbox = static_cast<QCheckBox*>(m_labelList->itemWidget(m_labelList->item(index)));
				checkbox->setChecked(false);
			}

		}*/
		m_setDefaultCheckBox->setChecked(false);
		//m_colorwayViewQueue.clear();
		m_imageQueueTable->clear();
		m_colorwayselectedList.clear();
		m_exportedColorwayImages.clear();
		//m_ColorwayViewMap.clear();

		if (m_viewList->count() > 0)
		{
			for (int index = 0; index < m_viewList->count(); index++)
			{

				auto radioButon = static_cast<QRadioButton*>(m_viewList->itemWidget(m_viewList->item(index)));
				radioButon->setAutoExclusive(false);
				radioButon->setChecked(false);
				radioButon->setAutoExclusive(true);
			}

		}

		if (m_labelList->count() > 0)
		{
			for (int index = 0; index < m_labelList->count(); index++)
			{
				auto checkbox = static_cast<QCheckBox*>(m_labelList->itemWidget(m_labelList->item(index)));
				checkbox->setChecked(false);
			}
		}


		Logger::Debug("UpdateImageIntent -> ClearAllFields() -> End");
	}

	bool UpdateImageIntent::IsValidImageIntent(QString selectedSnapshot, QStringList selectedImageLabels, QString selectedColorway, int _view)
	{
		Logger::Debug("UpdateImageIntent -> IsValidImageIntent() -> Start selectedSnapshot selected colorway " + selectedSnapshot.toStdString() + selectedColorway.toStdString());
		string  throwValue = "";
		bool retVal = false;
		//m_colorwayViewQueue.removeDuplicates();
		map<string, ColorwayViews>::iterator it;
		if (!FormatHelper::HasContent(selectedSnapshot.toStdString()))
		{
			throwValue = "View" + REQUIRED_THROW_KEY;
			throw (throwValue);
		}
		if (selectedImageLabels.size() == 0)
		{
			throwValue = "Label" + REQUIRED_THROW_KEY;
			throw (throwValue);
		}
		if (!FormatHelper::HasContent(selectedColorway.toStdString()))
		{
			throwValue = "Colorway Name" + REQUIRED_THROW_KEY;
			throw (throwValue);
		}
		it = m_ColorwayViewMap.find(selectedColorway.toStdString());// Searching unit in map
		bool defaultselected = false;
		if (it != m_ColorwayViewMap.end())
		{
			Logger::Debug("UpdateImageIntent -> IsValidImageIntent() -> it->second.defaultImage" + to_string(it->second.defaultImage));
			
			if (UpdateProduct::GetInstance()->m_editButtonClicked)
			{
				if (it->second.defaultImage != _view && it->second.defaultImage != -1 && it->second.defaultImage != UpdateProduct::GetInstance()->m_currentViewSelected && m_setDefaultCheckBox->isChecked())
					defaultselected = true;
			}
			else
			{
				if(it->second.defaultImage != -1 && m_setDefaultCheckBox->isChecked())
				defaultselected = true;
			}
			if(defaultselected)
			{
				throwValue = "Default view already selected for this colorway";
				throw (throwValue);
			}
		}
		QString currentViewName;
		switch (UpdateProduct::GetInstance()->m_currentViewSelected)
		{
		case BACK_VIEW:
			currentViewName = "Back";
			break;
		case FRONT_VIEW:
			currentViewName = "Front";
			break;
		case LEFT_VIEW:
			currentViewName = "Left";
			break;
		case RIGHT_VIEW:
			currentViewName = "Right";
			break;
		}

		//for (int i = 0; i < m_colorwayViewQueue.size(); i++)
		//{
		//	Logger::Debug("UpdateImageIntent -> IsValidImageIntent() -> m_colorwayViewQueue[i]" + m_colorwayViewQueue[i].toStdString()); ;
		//}
		Logger::Debug("UpdateImageIntent -> IsValidImageIntent() -> it->second.m_currentViewSelected" + to_string(UpdateProduct::GetInstance()->m_currentViewSelected));
		Logger::Debug("UpdateImageIntent -> IsValidImageIntent() ->_view" + to_string(_view));
		if ((UpdateProduct::GetInstance()->m_editButtonClicked))
		{
			if (UpdateProduct::GetInstance()->m_currentViewSelected != _view)
			{
				if(it->second.viewUploadId[_view] != "")
					retVal = false;
				else
					retVal = true;
			}
			else
			{
				it->second.viewUploadId[UpdateProduct::GetInstance()->m_currentViewSelected] = "";
				//m_colorwayViewQueue.removeAll(selectedColorway + currentViewName);
				retVal = true;
				//if(it->second.defaultImage != -1 && m_setDefaultCheckBox->isChecked() || it->second.defaultImage != -1)
			}
			//if (!currentViewName.contains(selectedSnapshot))// View changed
			//{
			//	if (m_colorwayViewQueue.contains(selectedColorway + selectedSnapshot))
			//		retVal = false;
			//	else
			//	{
			//		it->second.viewUploadId[UpdateProduct::GetInstance()->m_currentViewSelected] = "";
			//		m_colorwayViewQueue.removeAll(selectedColorway + currentViewName);
			//		retVal = true;
			//	}
			//}
			//else
			//	retVal = true;
		}
		else
		{
			Logger::Debug("UpdateImageIntent -> IsValidImageIntent() -> 4");
			if (it->second.viewUploadId[_view] != "")
				retVal = false;
			else
				retVal = true;
		}

	
	Logger::Debug("UpdateProduct -> IsValidImageIntent() ->  _selectedColorway + selectedSnapshot  :   " + selectedColorway.toStdString() + selectedSnapshot.toStdString());
	Logger::Debug("UpdateImageIntent -> IsValidImageIntent() -> End");
	return retVal;
 }
}
