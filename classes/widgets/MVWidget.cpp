#include "MVWidget.h"

MVWidget::MVWidget(QWidget* parent)
	:QWidget(parent)
{

}
	
MVWidget::~MVWidget()
{

}

void MVWidget::ConnectSignalSlots()
{
	connectSignalSlots(true);
}

void MVWidget::DisconnectSignalSlots()
{
	connectSignalSlots(false);
}

