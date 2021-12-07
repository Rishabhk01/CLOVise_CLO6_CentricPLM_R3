/*
* Copyright 2020-2021 CLO-Vise. All rights reserved
*
* @file CLOViseSuite.cpp
*
* @brief Class implementation for setting search/publish mode in CLO.
* This class has all the variable and methods implementation which are used to set search mode in CLO.
*
* @author GoVise
*
* @date 26-MAY-2020
*/
#include "CLOViseSuite.h"

#include <exception>
#include <time.h>

#include <QDebug>
#include <qmessagebox.h>

#include <LibraryWindowImplPlugin.h>
#include <CLOAPIInterface.h>
#include "classes/PLMSettingsSample.h"

#include "CLOVise/PLM/Authentication/CLOPLMSignIn.h"
#include "CLOVise/PLM/Authentication/CLOPLMImplementation.h"

#include "CLOVise/PLM/Helper/UIHelper/CVDisplayMessageBox.h"
#include "CLOVise/PLM/Helper/UIHelper/UIHelper.h"

#include "CLOVise/PLM/Inbound/Color/PLMColorSearch.h"
#include "CLOVise/PLM/Inbound/Color/PLMColorResults.h"
#include "CLOVise/PLM/Inbound/Color/ColorConfig.h"

#include "CLOVise/PLM/Inbound/Print/PLMPrintSearch.h"
#include "CLOVise/PLM/Inbound/Print/PLMPrintResults.h"
#include "CLOVise/PLM/Inbound/Print/PrintConfig.h"

#include "CLOVise/PLM/Inbound/Material/PLMMaterialSearch.h"
#include "CLOVise/PLM/Inbound/Material/MaterialConfig.h"
#include "CLOVise/PLM/Inbound/Material/PLMMaterialResult.h"
#include "CLOVise/PLM/Helper/Util/RestAPIUtility.h"

#include "CLOVise/PLM/Inbound/Document/PLMDocumentSearch.h"
#include "CLOVise/PLM/Inbound/Document/PLMDocumentResults.h"
#include "CLOVise/PLM/Inbound/Document/DocumentConfig.h"

#include "CLOVise/PLM/Inbound/Product/PLMProductSearch.h"
#include "CLOVise/PLM/Inbound/Product/PLMProductResults.h"
#include "CLOVise/PLM/Inbound/Product/ProductConfig.h"

#include "CLOVise/PLM/Inbound/Sample/PLMSampleSearch.h"
#include "CLOVise/PLM/Inbound/Sample/PLMSampleResult.h"
#include "CLOVise/PLM/Inbound/Sample/SampleConfig.h"

#include "CLOVise/PLM/Outbound/PublishToPLM/CreateProduct.h"
#include "CLOVise/PLM/Outbound/PublishToPLM/CopyProduct.h"
#include "CLOVise/PLM/Outbound/PublishToPLM/PublishToPLMData.h"

#include "CLOVise/PLM/Outbound/PublishToPLM/UpdateProduct.h"
//#include "CLOVise/PLM/Outbound/PublishToPLM/Create3DModel.h"

//#include "CLOVise/PLM/Outbound/PublishToPLM/Update3DModel.h"
//#include "CLOVise/PLM/Outbound/PublishToPLM/Update3DModel.h"

#include "CLOVise/PLM/Outbound/CraeteMaterial/CreateMaterialConfig.h"
#include "CLOVise/PLM/Outbound/UpdateMaterial/UpdateMaterialConfig.h"
#include "CLOVise/PLM/Outbound/CraeteMaterial/CreateMaterial.h"
#include "CLOVise/PLM/Outbound/UpdateMaterial/UpdateMaterial.h"

#include "CLOVise/PLM/Helper/Util/Logger.h"
#include "CLOVise/PLM/Helper/Util/CVLicenseHelper.h"
#include "CLOVise/PLM/Helper/Util/Configuration.h"
#include "CLOVise/PLM/Helper/Util/CVErrorMessage.h"

using namespace std;

namespace CLOVise
{
	CLOViseSuite* CLOViseSuite::_instance = NULL;

	CLOViseSuite* CLOViseSuite::GetInstance()
	{
		if (_instance == NULL) {
			_instance = new CLOViseSuite();
		}
		return _instance;
	}

	void CLOViseSuite::Destroy()
	{
		if (_instance) {
			delete _instance;
			_instance = NULL;
		}
	}

	CLOViseSuite::CLOViseSuite(QWidget* parent)
		: MVDialog(parent)
	{
		setupUi(this);

		this->setWindowTitle("CLO-Vise Suite for PLM");

#ifdef __APPLE__
        this->setWindowFlags(Qt::Dialog | Qt::CustomizeWindowHint | Qt::WindowMinMaxButtonsHint | Qt::WindowCloseButtonHint);
		this->setStyleSheet("QDialog { min-width: 712px; min-height: 485px; max-width: 712px; max-height: 485px; width: 712px; height: 485px; }");
		this->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
#else
		this->setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint | Qt::CustomizeWindowHint);
		QString windowTitle = "CLO-Vise Suite for " + PLM_NAME + " PLM";
        m_pTitleBar = new MVTitleBar(windowTitle, this);
        layout()->setMenuBar(m_pTitleBar);
#endif // !__APPLE__

		this->setSizeGripEnabled(false);
		this->setStyleSheet("QDialog { width: 712px; height: 485px; }");
		Configuration::GetInstance()->CreateTemporaryFolderPath();

		string userName = Configuration::GetInstance()->GetLoggedInUserName();
		
		ui_userNameLabel->setStyleSheet(USER_NAME_LABEL_STYLE);
		ui_userNameLabel->setText(CVWidgetGenerator::AddNewLinesToString(userName, 20));
		ui_userNameLabel->setAlignment(Qt::AlignRight);
		ui_userNameLabel->setToolTip("Logged-in User");
		ui_userNameLabel->setMinimumWidth(100);
		ui_userNameLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

		SetSplashImage(SPLASH_IMAGE_PATH);
		SetImageOnLabel(ui_CloViseLogoLabel, "ui_CloViseLogoLabel", CLOVISE_LOGO_PATH, "140px", "25px");
		SetImageOnLabel(ui_PlmLabel, "ui_PlmLabel", ":/CLOVise/PLM/Images/img_bi_black_centricPLM.svg", "140px", "20px");
		SetImageOnLabel(ui_PoweredByLabel, "ui_PoweredByLabel", POWERED_BY_IMAGE_PATH, "140px", "35px");
		
		ui_searchProduct->setText(QString::fromStdString(Configuration::GetInstance()->GetLocalizedStyleClassName()));
		ui_searchMaterial->setText(QString::fromStdString(Configuration::GetInstance()->GetLocalizedMaterialClassName()));
		ui_searchColor->setText(QString::fromStdString(Configuration::GetInstance()->GetLocalizedColorClassName()));
		ui_searchPrint->setText("Print");

		CVWidgetGenerator::SetButtonProperty(ui_searchMaterial, FABRIC_HOVER_ICON_PATH, SEARCH_TOOLTIP, CLOVISESUITE_BUTTON_STYLE);
		CVWidgetGenerator::SetButtonProperty(ui_searchColor, COLOR_HOVER_ICON_PATH, SEARCH_TOOLTIP, CLOVISESUITE_BUTTON_STYLE);
		CVWidgetGenerator::SetButtonProperty(ui_searchPrint, PRINT_HOVER_ICON_PATH, SEARCH_TOOLTIP, CLOVISESUITE_BUTTON_STYLE);
		//CVWidgetGenerator::SetButtonProperty(ui_searchDocument, DOCUMENT_HOVER_ICON_PATH, SEARCH_TOOLTIP, CLOVISESUITE_BUTTON_STYLE);
		CVWidgetGenerator::SetButtonProperty(ui_searchProduct, PRODUCT_HOVER_ICON_PATH, SEARCH_TOOLTIP, CLOVISESUITE_BUTTON_STYLE);
		CVWidgetGenerator::SetButtonProperty(ui_createProductButton, ADD_HOVER_ICON_PATH, CREATE_TOOLTIP, CLOVISESUITE_BUTTON_STYLE);
		CVWidgetGenerator::SetButtonProperty(ui_createMaterialButton, ADD_HOVER_ICON_PATH, CREATE_TOOLTIP, CLOVISESUITE_BUTTON_STYLE);
		Logger::Logger("Before CLOVise suit ....");
		CVWidgetGenerator::SetButtonProperty(ui_updateMaterialButton, UPDATE_HOVER_ICON_PATH, UPDATE_TOOLTIP, CLOVISESUITE_BUTTON_STYLE);
		Logger::Logger("After CLOVise suit ....");
		CVWidgetGenerator::SetButtonProperty(ui_updateProductButton, UPDATE_HOVER_ICON_PATH, UPDATE_TOOLTIP, CLOVISESUITE_BUTTON_STYLE);
		//CVWidgetGenerator::SetButtonProperty(ui_searchSampleButton, SAMPLE_HOVER_ICON_PATH, SEARCH_TOOLTIP, CLOVISESUITE_BUTTON_STYLE);
		CVWidgetGenerator::SetButtonProperty(ui_copyProductButton, COPY_HOVER_ICON_PATH, COPY_TOOLTIP, CLOVISESUITE_BUTTON_STYLE);
		ui_copyProductButton->setIconSize(QSize(12, 12));
		//"QPushButton{ icon-size: 12px; font-size: 10px; font-face: ArialMT; height: 27px; width: 175px; }""QToolTip { color: #46C8FF; background-color: #33414D; border: 1px #000000; }"
		//CVWidgetGenerator::SetButtonProperty(SampleSubmit, SUBMIT_ICON_PATH, SUBMIT_TOOLTIP, "QPushButton{ icon-size: 12px; font-size: 10px; font-face: ArialMT; height: 27px; width: 175px; padding: 5px; }""QToolTip { color: #46C8FF; background-color: #33414D; border: 1px #000000; }");
		CVWidgetGenerator::SetButtonProperty(logout, LOGOUT_HOVER_ICON_PATH, LOGOUT_TOOLTIP, TOOL_BUTTON_STYLE);
		connectSignalSlots(true);

		
	}

	CLOViseSuite::~CLOViseSuite()
	{

	}

	/*
	* Description - SetSplashImage() method used to add Splash image on CLO-Vise Suite.
	* Parameter - QString
	* Exception - 
	* Return -
	*/
	void CLOViseSuite::SetSplashImage(QString _imagePath)
	{
		ui_SplashFrame->setStyleSheet("border-image: url(" + _imagePath + "); background-position: centre;");
	}

	/*
	* Description - SetCloViseImagesOnSplash() method used to add plm images on CLO-Vise Suite.
	* Parameter - QString, QString, QString
	* Exception -
	* Return - 
	*/
	void CLOViseSuite::SetImageOnLabel(QLabel* _label, QString _objectName, QString _ImagePath, QString _width, QString _height)
	{
		_label->setStyleSheet("#" + _objectName + " { border-image:url(" + _ImagePath + "); width: " + _width + "; height: " + _height + "; }");
	}

	/*
	* Description - DisableDesignSuiteButtons() method used to disable the navigation options like color, material, product, document, publishToPLM.
	* Parameter - 
	* Exception - exception, Char *
	* Return -
	*/
	void CLOViseSuite::DisableDesignSuiteButtons()
	{
		Logger::Info("DesignSuite -> DisableDesignSuiteButtons() -> Start");
		ui_searchMaterial->setEnabled(false);
		ui_searchMaterial->setIcon(QIcon(FABRIC_NONE_ICON_PATH));
		ui_searchColor->setEnabled(false);
		ui_searchColor->setIcon(QIcon(COLOR_NONE_ICON_PATH));
		/*ui_searchDocument->setEnabled(false);
		ui_searchDocument->setIcon(QIcon(DOCUMENT_NONE_ICON_PATH));*/
		ui_searchProduct->setEnabled(false);
		ui_searchProduct->setIcon(QIcon(PRODUCT_NONE_ICON_PATH));
		ui_createProductButton->setEnabled(false);
		ui_createProductButton->setIcon(QIcon(ADD_NONE_ICON_PATH));
		ui_createMaterialButton->setEnabled(false);
		ui_updateMaterialButton->setEnabled(false);
		ui_createMaterialButton->setIcon(QIcon(ADD_NONE_ICON_PATH));
		ui_updateMaterialButton->setIcon(QIcon(ADD_NONE_ICON_PATH));
		ui_updateProductButton->setEnabled(false);
		ui_updateProductButton->setIcon(QIcon(UPDATE_NONE_ICON_PATH));
		ui_copyProductButton->setEnabled(false);
		ui_copyProductButton->setIcon(QIcon(COPY_NONE_ICON_PATH));
		Logger::Info("DesignSuite -> DisableDesignSuiteButtons() -> End");
	}

	bool CLOViseSuite::ValidateColorSearch()
	{
		return true;
	}

	bool CLOViseSuite::ValidateMaterialSearch()
	{
		return true;
	}

	/*bool CLOViseSuite::ValidateDocumentSearch()
	{
		return true;
	}*/
	
	bool CLOViseSuite::ValidateCreateMaterial()
	{
		return true;
	}
	bool CLOViseSuite::ValidateUpdateMaterial()
	{
		return true;
	}
	bool CLOViseSuite::ValidatePrintSearch()
	{
		return true;
	}
	/*bool CLOViseSuite::ValidateSampleSearch()
	{
		bool isSampleValidated = false;
		if (!FormatHelper::HasContent(UTILITY_API->GetProjectName()))
		{
			throw logic_error(NO_3D_SAMPLE_MSG);
		}
		if (DEFAULT_3DMODEL_NAME.find(UTILITY_API->GetProjectName()) != string::npos)
		{
			throw logic_error(NO_3D_SAMPLE_MSG);
		}
		isSampleValidated = true;
		return isSampleValidated;
	}*/

	//bool CLOViseSuite::ValidateSubmitSample()
	//{
	//	bool isSubmitSampleValidated = false;
	//	if (!FormatHelper::HasContent(UTILITY_API->GetProjectName()))
	//	{
	//		throw logic_error(NO_3D_SAMPLE_TO_SUBMIT_MSG);
	//	}
	//	if (DEFAULT_3DMODEL_NAME.find(UTILITY_API->GetProjectName()) != string::npos)
	//	{
	//		throw logic_error(NO_3D_SAMPLE_TO_SUBMIT_MSG);
	//	}
	//	if (!FormatHelper::HasContent(SubmitSampleData::GetInstance()->GetActiveSampleObjectId()))
	//	{
	//		throw logic_error(NO_ACTIVE_SAMPLE_MSG);
	//	}
	//	isSubmitSampleValidated = true;
	//	return isSubmitSampleValidated;
	//}

	bool CLOViseSuite::ValidateProductSearch()
	{
		bool isProductValidated = false;

		/*if (!FormatHelper::HasContent(UTILITY_API->GetProjectName()))
		{
			throw logic_error(NO_ACTIVE_3D_GARMENT_PRODUCT_MSG);
		}
		else if (DEFAULT_3DMODEL_NAME.find(UTILITY_API->GetProjectName()) != string::npos)
		{
			throw logic_error(NO_ACTIVE_3D_GARMENT_PRODUCT_MSG);
		}*/

		isProductValidated = true;
		return isProductValidated;
	}

	bool CLOViseSuite::ValidateUpdateProduct()
	{
		bool isUpadteProductValid = false;
		json upadtestyleMetadata = PublishToPLMData::GetInstance()->GetUpdateStyleCacheData();
		string attkey = Helper::GetJSONValue<string>(upadtestyleMetadata, ATTRIBUTE_ID, true);

		if (FormatHelper::HasContent(attkey))
			isUpadteProductValid = true;
		else
		{	
				throw logic_error("No active " + Configuration::GetInstance()->GetLocalizedStyleClassName() + ". Please download a " + Configuration::GetInstance()->GetLocalizedStyleClassName() + " from PLM.");
		}
		if (!FormatHelper::HasContent(UTILITY_API->GetProjectName()))
		{
			throw logic_error(NO_ACTIVE_3D_GARMENT_PUBLISH_MSG);
		}
		if (DEFAULT_3DMODEL_NAME.find(UTILITY_API->GetProjectName()) != string::npos)
		{
			throw logic_error(NO_ACTIVE_3D_GARMENT_PUBLISH_MSG);
		}
		return isUpadteProductValid;
	}

	bool CLOViseSuite::ValidateCreateProduct()
	{
		bool isPublishToPLMValidated = false;

		if (!FormatHelper::HasContent(UTILITY_API->GetProjectName()))
		{
			throw logic_error(NO_ACTIVE_3D_GARMENT_PUBLISH_MSG);
		}
		if (DEFAULT_3DMODEL_NAME.find(UTILITY_API->GetProjectName()) != string::npos)
		{
			throw logic_error(NO_ACTIVE_3D_GARMENT_PUBLISH_MSG);
		}
		//UIHelper::LookFor3DGarmentChange(/*PublishToPLMData::GetInstance()->Get3DModelObjectId()*/);
		//if (!FormatHelper::HasContent(PublishToPLMData::GetInstance()->GetActiveProductObjectId()) /*&& !PublishToPLMData::GetInstance()->GetIsProductOverridden()*/)
		//{
		//	string metaData = UTILITY_API->GetAPIMetaData(UTILITY_API->GetProjectFilePath());
		//	if (!FormatHelper::HasContent(metaData))
		//	{
		//		throw logic_error(NO_PRODUCT_EXIST_MSG);
		//	}
		//	json metadataJSON = json::parse(metaData);
		//	string objectId = Helper::GetJSONValue<string>(metadataJSON, DOCUMENTID_KEY, true);
		//	if (!FormatHelper::HasContent(objectId))
		//	{
		//		throw logic_error(NO_PRODUCT_EXIST_MSG);
		//	}
		//	json downloadJson = UIHelper::Cache3DModelDataFromFile(metadataJSON, DOCUMENT_MODULE);
		//	json resultsJSON = Helper::GetJSONParsedValue<string>(downloadJson, ATTACHMENTS_KEY, false);
		//	json downloadedResultsJSON = Helper::GetJSONParsedValue<int>(resultsJSON, 0, false);
		//	string productObjectId = Helper::GetJSONValue<string>(metadataJSON, PRODUCT_OBJ_ID, true);
		//	string downloadedProductObjectId = Helper::GetJSONValue<string>(downloadedResultsJSON, PRODUCT_OBJ_ID, true);
		//	if (productObjectId != downloadedProductObjectId)
		//	{
		//		throw logic_error(NO_ACTIVE_PRODUCT_MSG);
		//	}
		//	PublishToPLMData::GetInstance()->SetActive3DModelMetaData(downloadedResultsJSON);
		//	PLMDocumentData::GetInstance()->Set3DModelFileMetaDataJSONList(Helper::GetJSONParsedValue<string>(downloadJson, _3DMODEL_METADATALIST_KEY, false));
		//	metadataJSON = UIHelper::AddMetaDataIn3DModelFile(metadataJSON, resultsJSON, PLMDocumentData::GetInstance()->Get3DModelFileMetaDataJSONList(), PLMDocumentData::GetInstance()->GetAttScopes());
		//	Logger::Debug("metadataJSON 2 - " + to_string(metadataJSON));
		//	//UTILITY_API->SetMetaDataForCurrentGarment(to_string(metadataJSON));
		//	//PublishToPLMData::GetInstance()->SetIsProductOverridden(false);
		//	UIHelper::Rename3DWindowTitle(UTILITY_API->GetProjectFilePath(), UTILITY_API->GetProjectName(), PublishToPLMData::GetInstance()->GetActiveProductName());
		//	Logger::Debug("After rename end");
		//}
		//if (!FormatHelper::HasContent(PublishToPLMData::GetInstance()->GetActiveProductObjectId()))
		//{
		//	throw logic_error(NO_ACTIVE_PRODUCT_MSG);
		//}

		isPublishToPLMValidated = true;
		return isPublishToPLMValidated;
	}

	bool CLOViseSuite::ValidateCopyProduct()
	{
		bool isPublishToPLMValidated = false;

		 
		return true;
	}
	/*
	* Description - IsModuleExecutable() for check whether module is executable
	* Parameter -
	* Exception - exception, Char *
	* Return -
	*/
	bool CLOViseSuite::IsModuleExecutable(QString _module)
	{
		bool validate = false;
		
		if (_module == "Color")
			validate = ValidateColorSearch();

		if (_module == "Material")
			validate = ValidateMaterialSearch();

		/*if (_module == "Document")
			validate = ValidateDocumentSearch();*/

		/*if (_module == "Sample")
		validate = ValidateSampleSearch();*/

		//if (_module == "SubmitSample")
		//	validate = ValidateSubmitSample();

		if (_module == "Product")
			validate = ValidateProductSearch();

		if (_module == "CreateProduct")
			validate = ValidateCreateProduct();

		if (_module == "CopyProduct")
			validate = ValidateCopyProduct();

		if (_module == "UpdateProduct")
			validate = ValidateUpdateProduct();

		if (_module == "CreateMaterial")
			validate = ValidateCreateMaterial();

		if (_module == "UpdateMaterial")
			validate = ValidateUpdateMaterial();

		if (_module == "Print")
			validate = ValidatePrintSearch();

		return validate;
	}

	void CLOViseSuite::ExecuteColorModule()  
	{
		this->hide();
		try
		{
			if (IsModuleExecutable("Color")) 
			{
				int isFromConstructor = false;
				ColorConfig::GetInstance()->m_mode = "Search";
				if (!ColorConfig::GetInstance()->GetIsModelExecuted())
				{
					ColorConfig::GetInstance()->InitializeColorData();
					isFromConstructor = true;
				}
				else
				{
					PLMColorSearch::GetInstance()->DrawSearchWidget(true);
				}

				if (ColorConfig::GetInstance()->m_colorLoggedOut && isFromConstructor)
				{
					ColorConfig::GetInstance()->m_colorLoggedOut = false;
					ColorConfig::GetInstance()->m_resultAfterLogout = true;
					PLMColorSearch::GetInstance()->DrawSearchWidget(isFromConstructor);
				}
				PLMColorSearch::GetInstance()->setModal(true);
				
				UTILITY_API->DeleteProgressBar(true);
				PLMColorSearch::GetInstance()->exec();
				ColorConfig::GetInstance()->SetIsModelExecuted(true);
			}
		}
		catch (string msg)
		{
			UTILITY_API->DeleteProgressBar(true);
			Logger::Error("DesignSuite -> Color Exception - " + msg);
			UTILITY_API->DisplayMessageBox(msg);
			this->show();
		}
		catch (exception & e)
		{
			UTILITY_API->DeleteProgressBar(true);
			Logger::Error("DesignSuite -> Color Exception - " + string(e.what()));
			UTILITY_API->DisplayMessageBox(string(e.what()));
			this->show();
		}
		catch (const char* msg)
		{
			UTILITY_API->DeleteProgressBar(true);
			wstring wstr(msg, msg + strlen(msg));
			UTILITY_API->DisplayMessageBoxW(wstr);
			Logger::Error("DesignSuite -> Color Exception - " + string(msg));
			this->show();
		}
	}

	void CLOViseSuite::ExecutePrintModule()
	{
		this->hide();
		try
		{
			Logger:: GetFileSize("design suite-> FileSize");
			if (IsModuleExecutable("Print"))
			{
				int isFromConstructor = false;

				if (!PrintConfig::GetInstance()->GetIsModelExecuted())
				{
					PrintConfig::GetInstance()->InitializePrintData();
					isFromConstructor = true;
				}
				else
					PLMPrintSearch::GetInstance()->DrawSearchWidget(isFromConstructor);
				if (PrintConfig::GetInstance()->m_printLoggedOut && isFromConstructor)
				{
					PrintConfig::GetInstance()->m_printLoggedOut = false;
					//PrintConfig::GetInstance()->m_resultAfterLogout = true; need to update the implementation
					PLMPrintSearch::GetInstance()->DrawSearchWidget(isFromConstructor);
				}
				PLMPrintSearch::GetInstance()->setModal(true);

				UTILITY_API->DeleteProgressBar(true);
				PLMPrintSearch::GetInstance()->exec();
				PrintConfig::GetInstance()->SetIsModelExecuted(true);
			}
			
		}
		catch (string msg)
		{
			UTILITY_API->DeleteProgressBar(true);
			Logger::Error("DesignSuite -> Print Exception - " + msg);
			UTILITY_API->DisplayMessageBox(msg);
			this->show();
		}
		catch (exception & e)
		{
			UTILITY_API->DeleteProgressBar(true);
			Logger::Error("DesignSuite -> Print Exception - " + string(e.what()));
			UTILITY_API->DisplayMessageBox(string(e.what()));
			this->show();
		}
		catch (const char* msg)
		{
			UTILITY_API->DeleteProgressBar(true);
			wstring wstr(msg, msg + strlen(msg));
			UTILITY_API->DisplayMessageBoxW(wstr);
			Logger::Error("DesignSuite -> Print Exception - " + string(msg));
			this->show();
		}
	}

	void CLOViseSuite::ExecuteMaterialModule()
	{
		this->hide();
		try
		{
			auto startTime = std::chrono::high_resolution_clock::now();
			if (IsModuleExecutable("Material"))
			{
				int isFromConstructor = false;
				Configuration::GetInstance()->SetProgressBarProgress(0);
				UTILITY_API->CreateProgressBar();
				Configuration::GetInstance()->SetProgressBarProgress(qrand() % 101);
				RESTAPI::SetProgressBarData(10, "Loading "+ Configuration::GetInstance()->GetLocalizedMaterialClassName() +" Search", true);
				
				if (!MaterialConfig::GetInstance()->GetIsModelExecuted())
				{
					MaterialConfig::GetInstance()->InitializeMaterialData();
					isFromConstructor = true;
				}
				else
					PLMMaterialSearch::GetInstance()->DrawSearchWidget(isFromConstructor);

				if (MaterialConfig::GetInstance()->m_materialLoggedOut && isFromConstructor)
				{
					MaterialConfig::GetInstance()->m_materialLoggedOut = false;
					MaterialConfig::GetInstance()->m_resultAfterLogout = true;
					PLMMaterialSearch::GetInstance()->DrawSearchWidget(isFromConstructor);
				}
				MaterialConfig::GetInstance()->SetIsRadioButton(false);				
				PLMMaterialSearch::GetInstance()->setModal(true);				
				UTILITY_API->DeleteProgressBar(true);
				PLMMaterialSearch::GetInstance()->exec();
				auto finishTime = std::chrono::high_resolution_clock::now();
				std::chrono::duration<double> totalDuration = finishTime - startTime;
				Logger::perfomance(PERFOMANCE_KEY + "Time for Material Search UI:: " + to_string(totalDuration.count()));				
				MaterialConfig::GetInstance()->SetIsModelExecuted(true);
				
			}
		}
		catch (string msg)
		{
			UTILITY_API->DeleteProgressBar(true);
			Logger::Error("DesignSuite -> Material Exception - " + msg);
			UTILITY_API->DisplayMessageBox(msg);
			this->show();
		}
		catch (exception & e)
		{
			UTILITY_API->DeleteProgressBar(true);
			Logger::Error("DesignSuite -> Material Exception - " + string(e.what()));
			UTILITY_API->DisplayMessageBox(string(e.what()));
			this->show();
		}
		catch (const char* msg)
		{
			UTILITY_API->DeleteProgressBar(true);
			wstring wstr(msg, msg + strlen(msg));
			UTILITY_API->DisplayMessageBoxW(wstr);
			Logger::Error("DesignSuite -> Material Exception - " + string(msg));
			this->show();
		}
	}

	//void CLOViseSuite::ExecuteSampleModule()
	//{
	//	this->hide();
	//	try
	//	{
	//		if (IsModuleExecutable("Sample"))
	//		{
	//			SampleConfig::GetInstance()->InitializeSampleData();
	//			/*if (SampleConfig::GetInstance()->isModelExecuted)
	//			{*/
	//			UTILITY_API->CreateProgressBar();
	//			RESTAPI::SetProgressBarData(10, "Loading Sample Search", true);
	//			UTILITY_API->SetProgress("Loading Sample Search", (qrand() % 101));
	//			//}
	//			//PLMSampleSearch::Destroy();
	//			PLMSampleSearch::GetInstance()->setModal(true);
	//			//if (SampleConfig::GetInstance()->isModelExecuted)
	//				//UTILITY_API->SetProgress("Loading Sample Search", 100);
	//			PLMSampleSearch::GetInstance()->exec();
	//			RESTAPI::SetProgressBarData(0, "", false);
	//			SampleConfig::GetInstance()->isModelExecuted = true;
	//		}
	//	}
	//	catch (exception & e)
	//	{
	//		RESTAPI::SetProgressBarData(0, "", false);
	//		Logger::Error("DesignSuite -> Sample Exception - " + string(e.what()));
	//		UTILITY_API->DisplayMessageBox(string(e.what()));
	//		this->show();
	//	}
	//	catch (const char* msg)
	//	{
	//		RESTAPI::SetProgressBarData(0, "", false);
	//		wstring wstr(msg, msg + strlen(msg));
	//		UTILITY_API->DisplayMessageBoxW(wstr);
	//		Logger::Error("DesignSuite -> Sample Exception - " + string(msg));
	//		this->show();
	//	}
	//}

	/*void CLOViseSuite::ExecuteSubmitSampleModule()
	{
		this->hide();
		try
		{
			if (IsModuleExecutable("SubmitSample"))
			{
				if (SubmitSampleData::GetInstance()->isModelExecuted)
				{
					RESTAPI::SetProgressBarData(20, "Loading submit to PLM", true);
					UTILITY_API->SetProgress("Loading submit to PLM", 50);
				}

				SubmitSample::GetInstance()->Destroy();
				SubmitSample::GetInstance()->setModal(true);
				if (SubmitSampleData::GetInstance()->isModelExecuted)
					UTILITY_API->SetProgress("Loading submit to PLM", 95);
				SubmitSample::GetInstance()->exec();
				RESTAPI::SetProgressBarData(0, "", false);
				SubmitSampleData::GetInstance()->isModelExecuted = true;
			}
		}
		catch (exception & e)
		{
			RESTAPI::SetProgressBarData(0, "", false);
			Logger::Error("DesignSuite -> SubmitSample Exception - " + string(e.what()));
			UTILITY_API->DisplayMessageBox(string(e.what()));
			this->show();
		}
	}*/

	//void CLOViseSuite::ExecuteDocumentModule()
	//{
	//	this->hide();
	//	try
	//	{
	//		if (IsModuleExecutable("Document"))
	//		{
	//			PLMDocumentData::GetInstance()->InitializeDocumentData();
	//			if (PLMDocumentData::GetInstance()->isModelExecuted)
	//			{
	//				RESTAPI::SetProgressBarData(20, "Loading 3D Model Search", true);
	//				UTILITY_API->SetProgress("Loading 3D Model Search", 50);
	//			}
	//			//PLMDocumentSearch::Destroy();
	//			PLMDocumentSearch::GetInstance()->setModal(true);
	//			if (PLMDocumentData::GetInstance()->isModelExecuted)
	//				UTILITY_API->SetProgress("Loading 3D Model Search", 100);
	//			PLMDocumentSearch::GetInstance()->exec();
	//			RESTAPI::SetProgressBarData(0, "", false);
	//			PLMDocumentData::GetInstance()->isModelExecuted = true;
	//		}
	//	}
	//	catch (exception & e)
	//	{
	//		RESTAPI::SetProgressBarData(0, "", false);
	//		Logger::Error("DesignSuite -> Document Exception - " + string(e.what()));
	//		UTILITY_API->DisplayMessageBox(string(e.what()));
	//		this->show();
	//	}
	//}

	void CLOViseSuite::ExecuteCreateProductModule()
	{
		this->hide();
		try
		{
			if (IsModuleExecutable("CreateProduct"))
			{
				
				/*if (PublishToPLMData::GetInstance()->isModelExecuted)
				{*/
				UTILITY_API->CreateProgressBar();
				RESTAPI::SetProgressBarData(20, "Loading Create " + Configuration::GetInstance()->GetLocalizedStyleClassName(), true);
				UTILITY_API->SetProgress("Loading Create " + Configuration::GetInstance()->GetLocalizedStyleClassName(), (qrand() % 101));
				//}
				//CreateProduct::Destroy();
				if (CreateProduct::GetInstance()->ui_colorwayTable->rowCount() != 0)
				{
					CreateProduct::GetInstance()->GetUpdatedColorwayNames();
				}
				CreateProduct::GetInstance()->addCreateProductDetailsWidgetData();
				CreateProduct::GetInstance()->ExportTechPack();
				CreateProduct::GetInstance()->setModal(true);
				CreateProduct::GetInstance()->addCreateProductDetailsWidgetData();
				//if (PublishToPLMData::GetInstance()->isModelExecuted)
				//	UTILITY_API->SetProgress("Loading Create Product to PLM", 100);				
				UTILITY_API->DeleteProgressBar(true);
				
				/*if (PublishToPLMData::GetInstance()->m_createProductLoggedOut)
				{
					PublishToPLMData::GetInstance()->m_createProductLoggedOut = false;
					CreateProduct::GetInstance()->DrawCriteriaWidget(true);
				}
				if (PublishToPLMData::GetInstance()->GetIsModelExecuted())
					CreateProduct::GetInstance()->DrawCriteriaWidget(false);*///implemented for destroy issue.
				CreateProduct::GetInstance()->exec();
				//UTILITY_API->DeleteProgressBar(true);
				PublishToPLMData::GetInstance()->SetIsModelExecuted(true);
				//PublishToPLMData::GetInstance()->isModelExecuted = true;
			}
		}
		catch (string msg)
		{
			UTILITY_API->DeleteProgressBar(true);
			Logger::Error("DesignSuite -> Create Product Exception - " + msg);
			UTILITY_API->DisplayMessageBox(msg);
			this->show();
		}
		catch (exception & e)
		{
			UTILITY_API->DeleteProgressBar(true);
			Logger::Error("DesignSuite -> Create Product Exception - " + string(e.what()));
			UTILITY_API->DisplayMessageBox(string(e.what()));
			this->show();
		}
		catch (const char* msg)
		{
			UTILITY_API->DeleteProgressBar(true);
			wstring wstr(msg, msg + strlen(msg));
			UTILITY_API->DisplayMessageBoxW(wstr);
			Logger::Error("DesignSuite -> Create Product Exception - " + string(msg));
			this->show();
		}
	}

	void CLOViseSuite::ExecuteCopyProductModule()
	{
		Logger::Logger("ExecuteCopyProductModule Started");
		this->hide();
		try
		{
			if (IsModuleExecutable("CopyProduct"))
			{
				Configuration::GetInstance()->SetProgressBarProgress(0);
				//UpdateProduct::Destroy();
				UTILITY_API->CreateProgressBar();
				Configuration::GetInstance()->SetProgressBarProgress(qrand() % 101);
				RESTAPI::SetProgressBarData(Configuration::GetInstance()->GetProgressBarProgress(), "Loading " + Configuration::GetInstance()->GetLocalizedStyleClassName() + " Search", true);
				ProductConfig::GetInstance()->SetMaximumLimitForStyleResult();
				Configuration::GetInstance()->SetCurrentScreen(SEARCH_PRODUCT_CLICKED);
				if (!ProductConfig::GetInstance()->GetIsModelExecuted())
					ProductConfig::GetInstance()->InitializeProductData();
				Configuration::GetInstance()->SetCurrentScreen(COPY_PRODUCT_CLICKED);
				CopyProduct::GetInstance()->SetCopyCreated(false);
				ProductConfig::GetInstance()->m_isShow3DAttWidget = false;	
				PLMProductSearch::GetInstance()->ClearAllFields();
				PLMProductSearch::GetInstance()->setModal(true);
				//UIHelper::ClearAllFieldsForSearch(PLMProductSearch::GetInstance()->GetTreewidget(0));
				//UIHelper::ClearAllFieldsForSearch(PLMProductSearch::GetInstance()->GetTreewidget(1));
				UTILITY_API->DeleteProgressBar(true);
				PLMProductSearch::GetInstance()->exec();
				PublishToPLMData::GetInstance()->SetIsModelExecuted(true);
				
			}
		}
		catch (string msg)
		{
			UTILITY_API->DeleteProgressBar(true);
			Logger::Error("DesignSuite -> Copy Product Exception - " + msg);
			UTILITY_API->DisplayMessageBox(msg);
			this->show();
		}
		catch (exception & e)
		{
			UTILITY_API->DeleteProgressBar(true);
			Logger::Error("DesignSuite -> Copy Product Exception - " + string(e.what()));
			UTILITY_API->DisplayMessageBox(string(e.what()));
			this->show();
		}
		catch (const char* msg)
		{
			UTILITY_API->DeleteProgressBar(true);
			wstring wstr(msg, msg + strlen(msg));
			UTILITY_API->DisplayMessageBoxW(wstr);
			Logger::Error("DesignSuite -> Copy Product Exception - " + string(msg));
			this->show();
		}
	}

	void CLOViseSuite::ExecuteCreateMaterialModule()
	{
		this->hide();
		try
		{
			if (IsModuleExecutable("CreateMaterial"))
			{
				int isFromConstructor = false;
				Configuration::GetInstance()->SetProgressBarProgress(0);
				UTILITY_API->CreateProgressBar();
				RESTAPI::SetProgressBarData(20, "Loading Create " + Configuration::GetInstance()->GetLocalizedMaterialClassName() + "..", true);
				UTILITY_API->SetProgress("Loading Create " + Configuration::GetInstance()->GetLocalizedMaterialClassName() + "..", (qrand() % 101));
				
				if (!CreateMaterialConfig::GetInstance()->GetIsModelExecuted())
				{
					MaterialConfig::GetInstance()->GetMaterialConfigJSON();
					isFromConstructor = true;
				}
				else
					CreateMaterial::GetInstance()->DrawCriteriaWidget(isFromConstructor);

				if (CreateMaterialConfig::GetInstance()->m_createMaterialLoggedOut && isFromConstructor)
				{
					CreateMaterialConfig::GetInstance()->m_createMaterialLoggedOut = false;
					CreateMaterial::GetInstance()->DrawCriteriaWidget(isFromConstructor);
				}
				CreateMaterial::GetInstance()->setModal(true);
				UTILITY_API->DeleteProgressBar(true);
				CreateMaterial::GetInstance()->exec();
				CreateMaterialConfig::GetInstance()->SetIsModelExecuted(true);
			}
		}
		catch (string msg)
		{
			UTILITY_API->DeleteProgressBar(true);
			Logger::Error("DesignSuite -> Create Material Exception - " + msg);
			UTILITY_API->DisplayMessageBox(msg);
			this->show();
		}
		catch (exception & e)
		{
			UTILITY_API->DeleteProgressBar(true);
			Logger::Error("DesignSuite -> Create Material Exception - " + string(e.what()));
			UTILITY_API->DisplayMessageBox(string(e.what()));
			this->show();
		}
		catch (const char* msg)
		{
			UTILITY_API->DeleteProgressBar(true);
			wstring wstr(msg, msg + strlen(msg));
			UTILITY_API->DisplayMessageBoxW(wstr);
			Logger::Error("DesignSuite -> Create Material Exception - " + string(msg));
			this->show();
		}
	}
	void CLOViseSuite::ExecuteUpdateMaterialModule()
	{
		Logger::Info("DesignSuite -> ExecuteUpdateMaterialModule -> Start");
		this->hide();
		try
		{
			if (IsModuleExecutable("UpdateMaterial"))
			{
				if (MaterialConfig::GetInstance()->GetIsSaveAndCloseClicked())
				{
					RESTAPI::SetProgressBarData(20, "Loading Update " + Configuration::GetInstance()->GetLocalizedMaterialClassName() + "..", true);
					UTILITY_API->SetProgress("Loading Update " + Configuration::GetInstance()->GetLocalizedMaterialClassName() + "..", (qrand() % 101));
					UpdateMaterial::GetInstance()->setModal(true);
					UTILITY_API->DeleteProgressBar(true);
					UpdateMaterial::GetInstance()->exec();
				}
				else
				{
					
					Configuration::GetInstance()->SetProgressBarProgress(0);
					UTILITY_API->CreateProgressBar();
					Configuration::GetInstance()->SetProgressBarProgress(qrand() % 101);
					MaterialConfig::GetInstance()->SetMaximumLimitForMaterialResult();
					RESTAPI::SetProgressBarData(Configuration::GetInstance()->GetProgressBarProgress(), "Loading " + Configuration::GetInstance()->GetLocalizedMaterialClassName() + " Search", true);
					if (!MaterialConfig::GetInstance()->GetIsModelExecuted())
					{
						Configuration::GetInstance()->SetCurrentScreen(SEARCH_MATERIAL_CLICKED);
						MaterialConfig::GetInstance()->InitializeMaterialData();
					}
					MaterialConfig::GetInstance()->SetIsRadioButton(true);
					Configuration::GetInstance()->SetCurrentScreen(UPDATE_MATERIAL_CLICKED);

					PLMMaterialSearch::GetInstance()->setModal(true);					
					PLMMaterialSearch::GetInstance()->DrawSearchWidget(false);
					UTILITY_API->DeleteProgressBar(true);
					PLMMaterialSearch::GetInstance()->exec();
				}
			}
		}
		catch (string msg)
		{
			UTILITY_API->DeleteProgressBar(true);
			Logger::Error("DesignSuite -> Create Material Exception - " + msg);
			UTILITY_API->DisplayMessageBox(msg);
			this->show();
		}
		catch (exception & e)
		{
			UTILITY_API->DeleteProgressBar(true);
			Logger::Error("DesignSuite -> Create Material Exception - " + string(e.what()));
			UTILITY_API->DisplayMessageBox(string(e.what()));
			this->show();
		}
		catch (const char* msg)
		{
			UTILITY_API->DeleteProgressBar(true);
			wstring wstr(msg, msg + strlen(msg));
			UTILITY_API->DisplayMessageBoxW(wstr);
			Logger::Error("DesignSuite -> Create Material Exception - " + string(msg));
			this->show();
		}
		Logger::Info("DesignSuite -> ExecuteUpdateMaterialModule -> Start");
	}

	void CLOViseSuite::ExecuteUpdateProductModule()
	{
		Logger::Info("DesignSuite -> ExecuteUpdateProductModule -> Start");
		this->hide();
		try
		{
			if (IsModuleExecutable("UpdateProduct"))
			{
				UTILITY_API->CreateProgressBar();
				RESTAPI::SetProgressBarData(20, "Loading Update " + Configuration::GetInstance()->GetLocalizedStyleClassName(), true);
				UTILITY_API->SetProgress("Loading Update " + Configuration::GetInstance()->GetLocalizedStyleClassName(), (qrand() % 101));
				//}
				//CreateProduct::Destroy();
				if (UpdateProduct::GetInstance()->ui_colorwayTable->rowCount() != 0)
				{
					UpdateProduct::GetInstance()->GetUpdatedColorwayNames();
				}
				//UpdateProduct::GetInstance()->m_downloadedStyleJson = PublishToPLMData::GetInstance()->GetUpdateStyleCacheData();
				if(Configuration::GetInstance()->GetIsNewStyleDownloaded())
					UpdateProduct::GetInstance()->addCreateProductDetailsWidgetData();
				else
				{
					if(!UpdateProduct::GetInstance()->GetIsSaveClicked())
						UpdateProduct::GetInstance()->GetcolorwayDetails();
				}

				UpdateProduct::GetInstance()->setModal(true);					
				Configuration::GetInstance()->SetIsNewStyleDownloaded(false);
				UTILITY_API->DeleteProgressBar(true);
				UpdateProduct::GetInstance()->exec();				
				PublishToPLMData::GetInstance()->SetIsModelExecuted(true);
			}
		}
		catch (string msg)
		{
			UTILITY_API->DeleteProgressBar(true);
			Logger::Error("DesignSuite -> UpdateProduct Exception - " + msg);
			UTILITY_API->DisplayMessageBox(msg);
			this->show();
		}
		catch (exception & e)
		{
			UTILITY_API->DeleteProgressBar(true);
			Logger::Error("DesignSuite -> UpdateProduct Exception - " + string(e.what()));
			UTILITY_API->DisplayMessageBox(string(e.what()));
			this->show();
		}
		catch (const char* msg)
		{
			UTILITY_API->DeleteProgressBar(true);
			wstring wstr(msg, msg + strlen(msg));
			UTILITY_API->DisplayMessageBoxW(wstr);
			Logger::Error("DesignSuite -> PublishToPLM Exception - " + string(msg));
			this->show();
		}
	}
	void CLOViseSuite::ExecuteProductModule()
	{
		this->hide();
		try
		{
			if (IsModuleExecutable("Product"))
			{
				//if (!Configuration::GetInstance()->GetIsStylePublished())
				//{
					//UpdateProduct::GetInstance()->Destroy();
				//}
				bool isFromConstructor = false;
				Configuration::GetInstance()->SetProgressBarProgress(0);
				UTILITY_API->CreateProgressBar();
				Configuration::GetInstance()->SetProgressBarProgress(qrand() % 101);
				RESTAPI::SetProgressBarData(20, "Loading " + Configuration::GetInstance()->GetLocalizedStyleClassName() + " Search", true);
				if (!ProductConfig::GetInstance()->GetIsModelExecuted())
				{
					ProductConfig::GetInstance()->InitializeProductData();
					isFromConstructor = true;
				}
				else
					PLMProductSearch::GetInstance()->DrawSearchWidget(isFromConstructor);

				if (ProductConfig::GetInstance()->m_productlLoggedOut && isFromConstructor)
				{
					ProductConfig::GetInstance()->m_productlLoggedOut = false;
					ProductConfig::GetInstance()->m_resultAfterLogout = true;
					PLMProductSearch::GetInstance()->DrawSearchWidget(isFromConstructor);
				}
				ProductConfig::GetInstance()->m_isShow3DAttWidget = true;
				/*	if (ProductConfig::GetInstance()->isModelExecuted)
					{*/
					//UTILITY_API->CreateProgressBar();
					//UTILITY_API->SetProgress("Loading Style Search", (qrand() % 101));
					//}
				PLMProductSearch::GetInstance()->setModal(true);
				//PLMProductSearch::GetInstance()->ClearAllFields();
				//UIHelper::ClearAllFieldsForSearch(PLMProductSearch::GetInstance()->GetTreewidget(0));
				//UIHelper::ClearAllFieldsForSearch(PLMProductSearch::GetInstance()->GetTreewidget(1));
				UTILITY_API->DeleteProgressBar(true);
				//if (ProductConfig::GetInstance()->isModelExecuted)
				PLMProductSearch::GetInstance()->exec();
				ProductConfig::GetInstance()->SetIsModelExecuted(true);
				//ProductConfig::GetInstance()->isModelExecuted = true;
			}
		}
		catch (string msg)
		{
			UTILITY_API->DeleteProgressBar(true);
			Logger::Error("DesignSuite -> Product Exception - " + msg);
			UTILITY_API->DisplayMessageBox(msg);
			this->show();
		}
		catch (exception & e)
		{
			UTILITY_API->DeleteProgressBar(true);
			Logger::Error("DesignSuite -> Product Exception - " + string(e.what()));
			UTILITY_API->DisplayMessageBox(string(e.what()));
			this->show();
		}
		catch (const char* msg)
		{
			UTILITY_API->DeleteProgressBar(true);
			wstring wstr(msg, msg + strlen(msg));
			UTILITY_API->DisplayMessageBoxW(wstr);
			Logger::Error("DesignSuite -> Product Exception - " + string(msg));
			this->show();
		}
		Logger::Info("DesignSuite -> ExecuteUpdateProductModule -> End");
	}

	/*
	* Description - ClickedSearchColors() for execute the functionality, when the click of Color button.
	* Clear all fields items in the ColorSearch UI.
	* Parameter - 
	* Exception - exception, Char *
	* Return -
	*/	
	void CLOViseSuite::onClickedSearchColors() 
	{
		Logger::Info("DesignSuite -> Initialize ColorSearch-> Start");
		Configuration::GetInstance()->SetCurrentScreen(COLOR_SEARCH_CLICKED);
		Configuration::GetInstance()->SetIsUpdateColorClicked(false);
		ExecuteColorModule();
		Logger::Info("DesignSuite -> Initialize ColorSearch-> Start");
	}

	/*
	* Description - ClickedSearchPrints() for execute the functionality, when the click of Color button.
	* Clear all fields items in the ColorSearch UI.
	* Parameter -
	* Exception - exception, Char *
	* Return -
	*/
	void CLOViseSuite::onClickedSearchPrints()
	{
		Logger::Info("DesignSuite -> onClickedSearchPrints() -> Start");
		Configuration::GetInstance()->SetCurrentScreen(PRINT_SEARCH_CLICKED);
		Configuration::GetInstance()->SetIsUpdateColorClicked(false);
		ExecutePrintModule();
		Logger::Info("DesignSuite -> onClickedSearchPrints() -> Start");
	}
	/*
	* Description - ClickedSearchSamples() for execute the functionality, when the click of Sample button.
	* Clear all fields items in the ColorSearch UI.
	* Parameter -
	* Exception - exception, Char *
	* Return -
	*/
	/*void CLOViseSuite::onClickedSearchSamples()
	{
		Logger::Info("DesignSuite -> Initialize SampleSearch-> Start");
		ExecuteCopyProductModule();
		Logger::Info("DesignSuite -> Initialize SampleSearch-> End");
	}*/
	
	void CLOViseSuite::onClickedCopyProduct()
	{
		Logger::Info("DesignSuite -> Initialize onClickedCopyProduct-> Start");
		Configuration::GetInstance()->SetCurrentScreen(COPY_PRODUCT_CLICKED);
		ExecuteCopyProductModule();
		Logger::Info("DesignSuite -> Initialize onClickedCopyProduct-> End");
	}

	/*
	* Description - ClickedSubmitSample() method used to execute the functionality, when the click of 3D Model & Visual button.
	* Clear all fields items in the publish UI.
	* Parameter -
	* Exception - exception, Char *
	* Return -
	*/
	//void CLOViseSuite::onClickedSubmitSample()
	//{
	//	Logger::Info("DesignSuite -> onClickedSubmitSample -> Start");
	//	ExecuteSubmitSampleModule();
	//	Logger::Info("DesignSuite -> onClickedSubmitSample -> End");
	//}

	/*
	* Description - ClickedSearchDocument() for execute the functionality, when the click of 3D Module button.
	* Clear all fields items in the document search UI.
	* Parameter - 
	* Exception - exception, Char *
	* Return -
	*/
	//void CLOViseSuite::onClickedSearchDocument()
	//{
	//	Logger::Info("DesignSuite -> Initialize TrimsSearch -> Start...");
	//	//ExecuteDocumentModule();
	//	Configuration::GetInstance()->SetCurrentScreen(SEARCH_PRODUCT_CLICKED);
	//	ExecuteStyleModule();
	//	Logger::Info("DesignSuite -> Initialize TrimsSearch -> End");
	//}

	/*
	* Description - ClickedPublishToPLM() method used to execute the functionality, when the click of 3D Model & Visual button.
	* Clear all fields items in the publish UI.
	* Parameter -
	* Exception - exception, Char *
	* Return -
	*/
	void CLOViseSuite::onClickedCreateProduct()
	{
		Logger::Info("DesignSuite -> Initialize 3D Model&Visual -> Start");
		Configuration::GetInstance()->SetIsUpdateColorClicked(false);
		Configuration::GetInstance()->SetCurrentScreen(CREATE_PRODUCT_CLICKED);
		
		ExecuteCreateProductModule();
		Logger::Info("DesignSuite -> Initialize 3D Model&Visual -> End");
	}

	void CLOViseSuite::onClickedUpdateProduct()
	{
		Logger::Info("DesignSuite -> onClickedUpdateProduct -> Start");
		Configuration::GetInstance()->SetCurrentScreen(UPDATE_PRODUCT_CLICKED);
		ExecuteUpdateProductModule();
		Logger::Info("DesignSuite -> onClickedUpdateProduct -> End");
	}

	/*
	* Description - MaterialSearch() method used to execute the functionality, when the click of Material button.
	* Clear all fields items in the MaterialSearch UI.
	* Parameter - 
	* Exception - exception, Char *
	* Return -
	*/
	void CLOViseSuite::onMaterialSearch()
	{
		Logger::Info("DesignSuite -> Initialize MaterialSearch -> Start...");
		Configuration::GetInstance()->SetCurrentScreen(SEARCH_MATERIAL_CLICKED);
		ExecuteMaterialModule();
		Logger::Info("DesignSuite -> Initialize MaterialSearch -> End...");
	}

	/*
	* Description - ClickedSearchProd() method used to execute the functionality when the click of product button.
	* Clear all fields items in the ProductSearch UI.
	* Parameter - 
	* Exception - exception, Char *
	* Return -
	*/
	void CLOViseSuite::onClickedSearchProd()
	{
		Logger::Info("DesignSuite -> Initialize ProdSearch -> Start");
		Configuration::GetInstance()->SetCurrentScreen(SEARCH_PRODUCT_CLICKED);
	
		ExecuteProductModule();
		Logger::Info("DesignSuite -> Initialize ProdSearch -> End");
	}

	/*
	* Description - onClickedCreateMaterial() method used to execute the functionality, when the click of Create Material button.
	* Clear all fields items in the publish UI.
	* Parameter -
	* Exception - exception, Char *
	* Return -
	*/
	void CLOViseSuite::onClickedCreateMaterial()
	{
		Logger::Info("DesignSuite -> Initialize Create Material -> Start");
		Configuration::GetInstance()->SetCurrentScreen(CREATE_MATERIAL_CLICKED);
		ExecuteCreateMaterialModule();
		Logger::Info("DesignSuite -> Initialize Create Material -> End");
	}

	/*
	* Description - onClickedUpdateMaterial() method used to execute the functionality, when the click of Create Material button.
	* Clear all fields items in the publish UI.
	* Parameter -
	* Exception - exception, Char *
	* Return -
	*/
	void CLOViseSuite::onClickedUpdateMaterial()
	{
		Logger::Info("DesignSuite -> Initialize Create Material -> Start");
		ExecuteUpdateMaterialModule();
		Logger::Info("DesignSuite -> Initialize Create Material -> End");
	}

	/*
	* Description - ClickedLogout() method used to execute the functionality when the click of logout button.
	* Destroy all the UI's.
	* Parameter - 
	* Exception - 
	* Return -
	*/
	void CLOViseSuite::onClickedLogout()
	{
		bool loggedOut = false;
		Logger::Info("CLOViseSuite::onClickedLogout -> Start");
		try {
			if (CVLicenseHelper::GetInstance()->ValidateCVLicenseLogOut(CLOPLMSignIn::GetInstance()->GetID().toStdString()))
			{
				this->hide();
				Configuration::GetInstance()->SetPLMSignin(false);
				Configuration::GetInstance()->SetBearerToken("");
				Configuration::GetInstance()->SetConnectionStatus("Not Connected");
				Configuration::GetInstance()->SetPLMVersion("Not Available");
				PLMSignin::Destroy();
				CLOPLMSignIn::Destroy();
				Logger::Info("DesignSuite -> CLOPLMSignIn -> after");
				ColorConfig::GetInstance()->ResetColorConfig();
				PrintConfig::GetInstance()->ResetPrintConfig();
				Logger::Info("DesignSuite -> ResetColorConfig -> after");
				MaterialConfig::GetInstance()->ResetMaterialConfig();
				Logger::Info("DesignSuite -> ResetMaterialConfig -> after");
				CreateMaterialConfig::GetInstance()->ResetCreateConfig();
				Logger::Info("DesignSuite -> ResetCreateConfig -> after");
				ProductConfig::GetInstance()->ResetProductConfig();
				Logger::Info("DesignSuite -> ResetProductConfig -> after");
				//CreateProduct::GetInstance()->ResetCreateProductData(); //implemented for cdestroy issue. 
				Logger::Info("DesignSuite -> CreateProduct -> after");
				//CopyProduct::Destroy();
				//UpdateProduct::GetInstance()->Destroy();
				
				PublishToPLMData::GetInstance()->ResetPublishData();
				Logger::Info("DesignSuite -> ResetPublishData -> after");
				/*ColorConfig::Destroy();
				PLMDocumentSearch::Destroy();
				PLMDocumentResults::Destroy();
				PLMDocumentData::Destroy();
				PLMSampleSearch::Destroy();
				PLMSampleResult::Destroy();
				SampleConfig::Destroy();
				CreateProduct::Destroy();
				PublishToPLMData::Destroy();*/
				CreateProduct::Destroy(); //uncommented because of destroy issue not fixed.
				if (FormatHelper::HasContent(UTILITY_API->GetProjectName()) && DEFAULT_3DMODEL_NAME.find(UTILITY_API->GetProjectName()) == string::npos)
				{
					string currentProjectPath = UTILITY_API->GetProjectFilePath();
					string currentProjectName = UTILITY_API->GetProjectName();
					UIHelper::Rename3DWindowTitle(currentProjectPath, currentProjectName, "");
				}
				UTILITY_API->DeleteProgressBar(false);
				CVDisplayMessageBox DownloadDialogObject;
				DownloadDialogObject.DisplyMessage("Logged-out Successfully.");
				DownloadDialogObject.setModal(true);
				DownloadDialogObject.exec();
				Logger::Logger("DesignSuite -> Logged-out Successfully.");
				Logger::Info("CLOViseSuite::onClickedLogout -> End");
				Destroy();
			}
		}
		catch (string msg)
		{
			UTILITY_API->DeleteProgressBar(true);
			Logger::Error("Logg Out Exception - " + msg);
			UTILITY_API->DisplayMessageBox(msg);
			this->show();
		}
		catch (exception & e)
		{
			UTILITY_API->DeleteProgressBar(true);
			Logger::Error("Logg Out Exception - " + string(e.what()));
			UTILITY_API->DisplayMessageBox(string(e.what()));
			this->show();
		}
		catch (const char* msg)
		{
			UTILITY_API->DeleteProgressBar(true);
			wstring wstr(msg, msg + strlen(msg));
			UTILITY_API->DisplayMessageBoxW(wstr);
			Logger::Error("Logg Out Exception -" + string(msg));
			this->show();
		}
	}

	void CLOViseSuite::onClickedSettings()
	{
		// get library access from CLO Team.
		
	}

	/*
	* Description - connectSignalSlots() method used to connect and disconnect the signal and slot.
	* Parameter -   bool.
	* Exception -
	* Return -
	*/
	void CLOViseSuite::connectSignalSlots(bool _b)
	{
		Logger::Info("PLMMaterialSearch -> connectSignalSlots() -> Start");
		if (_b)
		{
			QObject::connect(ui_searchMaterial, SIGNAL(clicked()), this, SLOT(onMaterialSearch()));
			QObject::connect(ui_searchProduct, SIGNAL(clicked()), this, SLOT(onClickedSearchProd()));
			QObject::connect(ui_searchColor, SIGNAL(clicked()), this, SLOT(onClickedSearchColors()));
			QObject::connect(ui_searchPrint, SIGNAL(clicked()), this, SLOT(onClickedSearchPrints()));
			//QObject::connect(ui_searchDocument, SIGNAL(clicked()), this, SLOT(onClickedSearchDocument()));
			//QObject::connect(PublishProduct, SIGNAL(clicked()), this, SLOT(onClickedPublishToPLM()));
			QObject::connect(ui_createProductButton, SIGNAL(clicked()), this, SLOT(onClickedCreateProduct()));
			QObject::connect(ui_createMaterialButton, SIGNAL(clicked()), this, SLOT(onClickedCreateMaterial()));
			QObject::connect(ui_updateMaterialButton, SIGNAL(clicked()), this, SLOT(onClickedUpdateMaterial()));
			QObject::connect(ui_updateProductButton, SIGNAL(clicked()), this, SLOT(onClickedUpdateProduct()));
			//QObject::connect(ui_searchSampleButton, SIGNAL(clicked()), this, SLOT(onClickedSearchSamples()));
			QObject::connect(ui_copyProductButton, SIGNAL(clicked()), this, SLOT(onClickedCopyProduct()));
			//QObject::connect(SampleSubmit, SIGNAL(clicked()), this, SLOT(onClickedSubmitSample()));
			QObject::connect(logout, SIGNAL(clicked()), this, SLOT(onClickedLogout()));
		}
		else
		{
			QObject::disconnect(ui_searchMaterial, SIGNAL(clicked()), this, SLOT(onMaterialSearch()));
			QObject::disconnect(ui_searchProduct, SIGNAL(clicked()), this, SLOT(onClickedSearchProd()));
			QObject::disconnect(ui_searchColor, SIGNAL(clicked()), this, SLOT(onClickedSearchColors()));
			QObject::disconnect(ui_searchPrint, SIGNAL(clicked()), this, SLOT(onClickedSearchPrints()));
			//QObject::disconnect(ui_searchDocument, SIGNAL(clicked()), this, SLOT(onClickedSearchDocument()));
			//QObject::disconnect(PublishProduct, SIGNAL(clicked()), this, SLOT(onClickedPublishToPLM()));
			QObject::disconnect(ui_createProductButton, SIGNAL(clicked()), this, SLOT(onClickedCreateProduct()));
			QObject::disconnect(ui_createMaterialButton, SIGNAL(clicked()), this, SLOT(onClickedCreateMaterial()));
			QObject::disconnect(ui_updateProductButton, SIGNAL(clicked()), this, SLOT(onClickedUpdateProduct()));
			//QObject::disconnect(ui_searchSampleButton, SIGNAL(clicked()), this, SLOT(onClickedSearchSamples()));
			QObject::disconnect(ui_copyProductButton, SIGNAL(clicked()), this, SLOT(onClickedCopyProduct()));
			//QObject::disconnect(SampleSubmit, SIGNAL(clicked()), this, SLOT(onClickedSubmitSample()));
			QObject::disconnect (logout, SIGNAL(clicked()), this, SLOT(onClickedLogout()));
		}
		Logger::Info("PLMMaterialSearch -> connectSignalSlots() -> End");
	}
}

