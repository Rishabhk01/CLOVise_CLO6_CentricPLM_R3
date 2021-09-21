#include "PLMSettingsSample.h"
#include "PLMSettingDataSample.h"
#include "CLOVise/PLM/Helper/Util/Definitions.h"
#include <QDebug>
#include <QFileDialog>
#include "CLOVise/PLM/Helper/Util/Configuration.h"
#include "CLOVise/PLM/Helper/Util/Helper.h"
#include <fstream>
#include "CLOVise/PLM/Libraries/json.h"
#include "CLOVise/PLM/Helper/Util/DirectoryUtil.h"
#include "CLOVise/PLM/Helper/Util/Logger.h"
#include <CLOVise/PLM/CLOViseSuite/CLOViseSuite.h>

using json = nlohmann::json;

namespace CLOAPISample
{
	PLMSettingsSample::PLMSettingsSample(QWidget* parent)
		: QWidget(parent)
	{
		setupUi(this);
		//Generates PLM Settings UI, reading PLM Settings JSON File.
		GeneratePLMSettingsUIFromFile();
		//This is a slot, which triggers on change of PLM Environment value.
		QObject::connect(comboBox_PLMServer, SIGNAL(currentIndexChanged(const QString&)), this,
			SLOT(CurrentPLMServerChanged(const QString&)));
	}

	PLMSettingsSample::~PLMSettingsSample()
	{
		//DESTROY_PLM_SETTING_DATA_SAMPLE 
		// do not use this here. please release plm setting data when unloading the library windows sample plug-in dll.
		//Update PLM Settings JSON File, on change of PLM Environment value.
		UpdatePLMSettingsFileWithSelectedServer();
	}

	void PLMSettingsSample::Reset()
	{
		// re-create the PLM settings data instance
		DESTROY_PLM_SETTING_DATA_SAMPLE;
	}
	/**
	*\reading the PLMSettings.json display inPLMSettingsUI.
	*/
	void PLMSettingsSample::GeneratePLMSettingsUIFromFile()
	{
		Logger::Logger("PLMSettingsSample::GeneratePLMSettingsUIFromFile() Started..");
		string plmSettingsFilePath = DirectoryUtil::GetPLMPluginDirectory() + PLMSETTINGS_FILE_NAME;
		try
		{
			json plmSettingsJSON = Helper::ReadJSONFile(plmSettingsFilePath);
			string availablePLMServers = plmSettingsJSON[PLMSERVER_JSON_KEY].dump();
			json availablePLMServersJSON = json::parse(availablePLMServers);
			Logger::Debug("PLMSettingsSample::GeneratePLMSettingsUIFromFile() availablePLMServersJSON - " + to_string(availablePLMServersJSON));
			string selectedPLMServer = Helper::GetJSONValue<string>(plmSettingsJSON, SELECTEDPLMSERVER_JSON_KEY, true);
			Logger::Debug("PLMSettingsSample::GeneratePLMSettingsUIFromFile() selectedPLMServer - " + selectedPLMServer);
			QStringList availablePLMServersList = Helper::ConvertJSONArrayQStringList(availablePLMServersJSON);
			int selectedPLMServerIndex = availablePLMServersList.indexOf(QString::fromStdString(selectedPLMServer));

			comboBox_PLMServer->addItems(availablePLMServersList);
			comboBox_PLMServer->setCurrentIndex(selectedPLMServerIndex);

			ShowOrHidePLMSettingsOfSelectedServer(selectedPLMServer);
		}
		catch (string msg)
		{
			Logger::Logger("PLMSettingsSample::GeneratePLMSettingsUIFromFile() exception - " + msg);
			UTILITY_API->DisplayMessageBox(PLMSETTINGS_EXCEPTION_MSG);
		}
		catch (exception& e)
		{
			Logger::Logger("PLMSettingsSample::GeneratePLMSettingsUIFromFile() exception - " + string(e.what()));
			UTILITY_API->DisplayMessageBox(PLMSETTINGS_EXCEPTION_MSG);
		}
		catch (const char* msg)
		{
			Logger::Logger("PLMSettingsSample::GeneratePLMSettingsUIFromFile() exception - " + string(msg));
			UTILITY_API->DisplayMessageBox(PLMSETTINGS_EXCEPTION_MSG);

		}
		Logger::Logger("PLMSettingsSample::GeneratePLMSettingsUIFromFile() Ended..");
	}
	/**
	*\Checks for current environment change and generates back PLM Settings UI.
	* \param changedServerName
	*/
	void PLMSettingsSample::CurrentPLMServerChanged(const QString& changedServer)
	{
		Logger::Logger("PLMSettingsSample::CurrentTenantChanged() Started..");
		string plmSettingsFilePath = DirectoryUtil::GetPLMPluginDirectory() + PLMSETTINGS_FILE_NAME;
		try
		{
			json plmSettingsJSON = Helper::ReadJSONFile(plmSettingsFilePath);
			string selectedPLMServer = Helper::GetJSONValue<string>(plmSettingsJSON, SELECTEDPLMSERVER_JSON_KEY, true);
			Logger::Logger("PLMSettingsSample::CurrentTenantChanged() changedServer - " + changedServer.toStdString());
			Logger::Logger("PLMSettingsSample::CurrentTenantChanged() selectedPLMServer - " + selectedPLMServer);
			if (changedServer == QString::fromStdString(SELECT))
			{
				HidePLMSettingsOfSelectedServer();
			}
			else
			{
				string plmSettingsDetails = Helper::GetJSONValue<string>(plmSettingsJSON, changedServer.toStdString(), false);
				json plmSettingsDetailsJSON = json::parse(plmSettingsDetails);

				if (selectedPLMServer != changedServer.toStdString())
				{
					string value = Helper::GetJSONValue<string>(plmSettingsDetailsJSON, PLMSERVERURL_JSON_KEY, true);
					if (!FormatHelper::HasContent(value))
						value = "";
					label_PLMServerURL_Value->setText(QString::fromStdString(value));
					value = Helper::GetJSONValue<string>(plmSettingsDetailsJSON, CLOPLUGINVERSION_JSON_KEY, true);
					if (!FormatHelper::HasContent(value))
						value = "";
					label_CLOVise_Value->setText(QString::fromStdString(value));
					value = Helper::GetJSONValue<string>(plmSettingsDetailsJSON, CLOPLMPLUGINVERSION_JSON_KEY, true);
					if (!FormatHelper::HasContent(value))
						value = "";
					lable_PLMVersion_Value->setText(QString::fromStdString(value));
					value = Helper::GetJSONValue<string>(plmSettingsDetailsJSON, COMPANY_NAME, true);
					if (!FormatHelper::HasContent(value))
						value = "";
					label_CompanyName_Value->setText(QString::fromStdString(value));
					label_ConnectionStatus_Value->setText(QString::fromStdString(SERVER_NOTCONNECTED_STATUS));
				}
				else
				{
					SetPLMSettingsOfSelectedServer();
				}
				ShowPLMSettingsOfSelectedServer();
			}
		}
		catch (string msg)
		{
			Logger::Logger("PLMSettingsSample::CurrentTenantChanged() exception - " + msg);
			UTILITY_API->DisplayMessageBox(PLMSETTINGS_EXCEPTION_MSG);
		}
		catch (const char* msg)
		{
			Logger::Logger("PLMSettingsSample::CurrentTenantChanged() exception - " + string(msg));
			UTILITY_API->DisplayMessageBox(PLMSETTINGS_EXCEPTION_MSG);
		}
		catch (exception& e)
		{
			Logger::Logger("PLMSettingsSample::CurrentTenantChanged() exception - " + string(e.what()));
			UTILITY_API->DisplayMessageBox(PLMSETTINGS_EXCEPTION_MSG);
		}
		Logger::Logger("PLMSettingsSample::CurrentTenantChanged() Ended..");
	}
	/**
	*\Shows or Hide PLM Settings values on PLM Settings UI.
	* \param selectedEnvironment, plmSettingsJSON
	*/
	void PLMSettingsSample::ShowOrHidePLMSettingsOfSelectedServer(string selectedServer)
	{
		Logger::Info("PLMSettingsSample::ShowOrHidePLMSettingsOfSelectedServer() Started..");
		if (selectedServer == SELECT)
		{
			HidePLMSettingsOfSelectedServer();
		}
		else
		{
			SetPLMSettingsOfSelectedServer();
			ShowPLMSettingsOfSelectedServer();
		}
		Logger::Info("PLMSettingsSample::ShowOrHidePLMSettingsOfSelectedServer() Ended..");
	}
	/**
	*\Set all PLM Settings values on PLM Settings UI.
	*/
	void PLMSettingsSample::SetPLMSettingsOfSelectedServer()
	{
		Logger::Info("PLMSettingsSample::SetPLMSettingsOfSelectedServer() Started..");
		
		label_PLMServerURL_Value->setText(QString::fromStdString(Configuration::GetInstance()->GetPLMServerURL()));
		label_CLOVise_Value->setText(QString::fromStdString(Configuration::GetInstance()->GetCLOViseVersion()));
		lable_PLMVersion_Value->setText(QString::fromStdString(Configuration::GetInstance()->GetPLMVersion()));
		label_ConnectionStatus_Value->setText(QString::fromStdString(Configuration::GetInstance()->GetConnectionStatus()));
		label_CompanyName_Value->setText(QString::fromStdString(Configuration::GetInstance()->GetCompanyName()));

		Logger::Info("PLMSettingsSample::SetPLMSettingsOfSelectedServer() Ended..");
	}
	/**
	*\Shows all PLM Settings values on PLM Settings UI.
	*/
	void PLMSettingsSample::ShowPLMSettingsOfSelectedServer()
	{
		Logger::Info("PLMSettingsSample::ShowPLMSettingsOfSelectedServer() Started..");

		label_PLMServerURL_Value->show();
		label_PLMServerURL_Key->show();

		lable_PLMVersion_Value->show();
		lable_PLMVersion_Key->show();

		label_CLOVise_Value->show();
		label_CLOVise_Key->show();

		label_ConnectionStatus_Value->show();
		label_ConnectionStatus_key->show();

		label_CompanyName_Value->show();
		label_CompanyName_Key->show();

		Logger::Info("PLMSettingsSample::ShowPLMSettingsOfSelectedServer() Ended..");
	}
	/**
	*\Hides all PLM Settings values on PLM Settings UI.
	*/
	void PLMSettingsSample::HidePLMSettingsOfSelectedServer()
	{
		Logger::Info("PLMSettingsSample::HidePLMSettingsOfSelectedServer() Started..");

		label_PLMServerURL_Value->hide();
		label_PLMServerURL_Key->hide();

		label_ConnectionStatus_Value->hide();
		label_ConnectionStatus_key->hide();

		lable_PLMVersion_Value->hide();
		lable_PLMVersion_Key->hide();

		label_CLOVise_Value->hide();
		label_CLOVise_Key->hide();

		label_CompanyName_Value->hide();
		label_CompanyName_Key->hide();

		Logger::Info("PLMSettingsSample::HidePLMSettingsOfSelectedServer() Ended..");
	}
	/**
	*\Updates selected environment on PLM Settings UI to File.
	*/
	void PLMSettingsSample::UpdatePLMSettingsFileWithSelectedServer()
	{
		Logger::Logger("PLMSettingsSample::UpdatePLMSettingsFileWithSelectedServer() Started..");
		string plmSettingsFilePath = DirectoryUtil::GetPLMPluginDirectory() + PLMSETTINGS_FILE_NAME;
		try
		{
			json plmSettingsJSON = Helper::ReadJSONFile(plmSettingsFilePath);
			string selectedPLMServer = Helper::GetJSONValue<string>(plmSettingsJSON, SELECTEDPLMSERVER_JSON_KEY, true);
			Logger::Logger("PLMSettingsSample::UpdatePLMSettingsFileWithSelectedServer() selectedPLMServer - " + selectedPLMServer);
			string changedPLMServer = comboBox_PLMServer->currentText().toStdString();

			if (selectedPLMServer != changedPLMServer)
			{
				if (SERVER_CONNECTED_STATUS == Configuration::GetInstance()->GetConnectionStatus())
				{
					UTILITY_API->DisplayMessageBox(RESTARTCLO_MSG);
					CLOVise::CLOViseSuite::GetInstance()->setModal(true);
					CLOVise::CLOViseSuite::GetInstance()->DisableDesignSuiteButtons();
					CLOVise::CLOViseSuite::GetInstance()->exec();
				}
				plmSettingsJSON[SELECTEDPLMSERVER_JSON_KEY] = changedPLMServer;
				Helper::WriteJSONFile(plmSettingsFilePath, true, plmSettingsJSON);
				Configuration::GetInstance()->SetConnectionStatus(SERVER_NOTCONNECTED_STATUS);
				Configuration::GetInstance()->SetBearerToken(BLANK);
			}
			Configuration::GetInstance()->CachePLMsettingsFromFile();
		}
		catch (string msg)
		{
			Logger::Logger("PLMSettingsSample::UpdatePLMSettingsFileWithSelectedServer() exception - " + msg);
			UTILITY_API->DisplayMessageBox(PLMSETTINGS_EXCEPTION_MSG);
		}
		catch (const char* msg)
		{

			Logger::Logger("PLMSettingsSample::UpdatePLMSettingsFileWithSelectedServer() exception - " + string(msg));
			UTILITY_API->DisplayMessageBox(PLMSETTINGS_EXCEPTION_MSG);
		}
		catch (exception& e)
		{
			Logger::Logger("PLMSettingsSample::UpdatePLMSettingsFileWithSelectedServer() exception - " + string(e.what()));
			UTILITY_API->DisplayMessageBox(PLMSETTINGS_EXCEPTION_MSG);
		}
		Logger::Logger("PLMSettingsSample::UpdatePLMSettingsFileWithSelectedServer() Ended..");

	}
}