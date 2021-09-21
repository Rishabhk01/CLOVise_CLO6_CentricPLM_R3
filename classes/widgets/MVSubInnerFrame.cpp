#include "MVSubInnerFrame.h"

MVSubInnerFrame::MVSubInnerFrame(QWidget *parent) : QFrame(parent)
{
    Initialize();
}

MVSubInnerFrame::~MVSubInnerFrame()
{
    Clear();
}

void MVSubInnerFrame::Initialize()
{
	this->SetObjectName(QString::fromUtf8("frame_SubInner"));
}

void MVSubInnerFrame::Clear()
{
}

void MVSubInnerFrame::setObjectName(const QString &name)
{
}

void MVSubInnerFrame::SetObjectName(const QString &name)
{
	QFrame::setObjectName(name);
}