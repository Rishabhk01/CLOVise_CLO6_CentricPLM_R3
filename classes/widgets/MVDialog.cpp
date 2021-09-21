#include "MVDialog.h"

MVDialog::MVDialog(QWidget* parent, Qt::WindowFlags f)
	:QDialog(parent, f)
{
	m_pTitleBar = nullptr;
	m_CVTitleBar = nullptr;
}

MVDialog::~MVDialog()
{
	if (m_pTitleBar != nullptr) {
		delete m_pTitleBar;
		m_pTitleBar = nullptr;
	}
	if (m_CVTitleBar != nullptr) {
		delete m_CVTitleBar;
		m_CVTitleBar = nullptr;
	}
}

void MVDialog::ConnectSignalSlots()
{
	connectSignalSlots(true);
}

void MVDialog::DisconnectSignalSlots()
{
	connectSignalSlots(false);
}
