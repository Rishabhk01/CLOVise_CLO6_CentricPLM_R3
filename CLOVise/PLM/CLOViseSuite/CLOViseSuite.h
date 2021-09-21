#pragma once
/*
* Copyright 2020-2021 CLO-Vise. All rights reserved
*
* @file CLOViseSuite.h
*
* @brief Class declaration for setting search mode in CLO.
* This class has all the variable declarations and function declarations which are used to set search/publish mode in CLO.
*
* @author GoVise
*
* @date  26-MAY-2020
*/
#include "ui_CLOViseSuite.h"

#include <string>
#include <iostream>

#include <QDialog>

#include "classes/widgets/MVDialog.h"

using namespace std;

namespace CLOVise {

	class CLOViseSuite : public MVDialog, public Ui::CLOViseSuite
	{
		Q_OBJECT			

	public:
		static CLOViseSuite* GetInstance();
		static void Destroy();
		void DisableDesignSuiteButtons();
		void SetSplashImage(QString _imagePath);
		void SetImageOnLabel(QLabel* _label, QString _objectName, QString _ImagePath, QString _width, QString _height);
		bool IsModuleExecutable(QString _module);
		bool ValidateColorSearch();
		bool ValidateMaterialSearch();
		//bool ValidateDocumentSearch();
		//bool ValidateSampleSearch();
		//bool ValidateSubmitSample();
		bool ValidateProductSearch();
		bool ValidateCreateProduct();
		bool ValidateCreateMaterial();
		bool ValidateCopyProduct();
		bool ValidateUpdateMaterial();
		bool ValidateUpdateProduct();
		void ExecuteColorModule();
		void ExecuteMaterialModule();
		//void ExecuteDocumentModule();
		//void ExecuteSampleModule();
		//void ExecuteSubmitSampleModule();
		void ExecuteProductModule();
		void ExecuteCreateProductModule();
		void ExecuteCopyProductModule();
		void ExecuteCreateMaterialModule();
		void ExecuteUpdateMaterialModule();
		void ExecuteUpdateProductModule();
	private:		
		CLOViseSuite(QWidget* parent = nullptr);
		virtual ~CLOViseSuite();

		static CLOViseSuite* _instance;

		void connectSignalSlots(bool _b) override;

	private slots:
		void onMaterialSearch();
		void onClickedSearchProd();
		void onClickedSearchColors();
		//void onClickedSearchDocument(); 
		void onClickedCreateProduct();		
		void onClickedCreateMaterial();		
		void onClickedUpdateMaterial();
		void onClickedUpdateProduct();
		void onClickedLogout();
		void onClickedSettings();
		//void onClickedSearchSamples();
		void onClickedCopyProduct();
		//void onClickedSubmitSample();
	};
}