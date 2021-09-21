#pragma once

#include <QFrame>

class MVOutterFrame : public QFrame
{
public:
    MVOutterFrame(QWidget *parent);
    ~MVOutterFrame();

	void setObjectName(const QString &name);

private:
    void Initialize();
    void Clear();
	void SetObjectName(const QString &name);
};
