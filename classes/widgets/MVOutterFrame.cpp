#include "MVOutterFrame.h" 

MVOutterFrame::MVOutterFrame(QWidget *parent) : QFrame(parent)
{
    Initialize();
}

MVOutterFrame::~MVOutterFrame()
{
    Clear();
}

void MVOutterFrame::Initialize()
{
	this->SetObjectName(QString::fromUtf8("frame_Outer"));
}

void MVOutterFrame::Clear()
{
}

void MVOutterFrame::setObjectName(const QString &name)
{
}

void MVOutterFrame::SetObjectName(const QString &name)
{
	QFrame::setObjectName(name);
}