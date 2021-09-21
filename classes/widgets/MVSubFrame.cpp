#include "MVSubFrame.h"

MVSubFrame::MVSubFrame(QWidget *parent) : QFrame(parent)
{
	Initialize();
}

MVSubFrame::~MVSubFrame()
{

	Clear();
}

void MVSubFrame::Initialize()
{
	this->SetObjectName(QString::fromUtf8("frame_Sub"));
}

void MVSubFrame::Clear()
{
}

void MVSubFrame::setObjectName(const QString &name)
{
}

void MVSubFrame::SetObjectName(const QString &name)
{
	QFrame::setObjectName(name);
}