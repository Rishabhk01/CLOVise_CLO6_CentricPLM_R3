#pragma once

#include <QFrame>

class MVSubFrame : public QFrame
{
public:
    MVSubFrame(QWidget *parent);
    ~MVSubFrame();

	void setObjectName(const QString &name);

private:
    void Initialize();
    void Clear();
	void SetObjectName(const QString &name);
};