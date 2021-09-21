#pragma once

#include <QDialog>

#include <string>
#include <iostream>

#include "ui_PLMSettingsSample.h"
#include "CLOVise/PLM/Libraries/json.h"
using json = nlohmann::json;

using namespace std;

namespace CLOAPISample
{

	class PLMSettingsSample : public QWidget, public Ui::PLMSettings
	{
		Q_OBJECT

	public:
		PLMSettingsSample(QWidget* parent = 0);
		~PLMSettingsSample();

		void Reset();

	private:
		string m_serverName = "";
		void UpdatePLMSettingsFileWithSelectedServer();
		void GeneratePLMSettingsUIFromFile();
		void ShowOrHidePLMSettingsOfSelectedServer(string selectedServer);
		void SetPLMSettingsOfSelectedServer();
		void ShowPLMSettingsOfSelectedServer();
		void HidePLMSettingsOfSelectedServer();

	private slots:
		void CurrentPLMServerChanged(const QString& changedServerName);
	};
}