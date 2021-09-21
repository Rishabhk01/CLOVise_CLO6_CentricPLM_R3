#pragma once
#include <QDialog>
#include "classes/widgets/MVTitleBar.h"

#include "CLOVise/PLM/Helper/UIHelper/CVTitleBar.h"

class MVDialog : public QDialog
{
public:
	explicit MVDialog(QWidget* parent = 0, Qt::WindowFlags f = 0);
	virtual ~MVDialog();

	MVTitleBar* m_pTitleBar;
	CVTitleBar* m_CVTitleBar;
	
protected:
	void ConnectSignalSlots();
	void DisconnectSignalSlots();

protected:
	virtual void connectSignalSlots(bool b) {}
};

