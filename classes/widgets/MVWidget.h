#pragma once
#include <QWidget>

class MVWidget : public QWidget
{
public:
	explicit MVWidget(QWidget* parent = 0);
	virtual ~MVWidget();

protected:
	void ConnectSignalSlots();
	void DisconnectSignalSlots();

protected:
	virtual void connectSignalSlots(bool b) {}
};

