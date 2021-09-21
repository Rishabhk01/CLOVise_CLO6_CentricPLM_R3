#pragma once

#include <QFrame>

class MVSubInnerFrame : public QFrame
{
public:
    MVSubInnerFrame(QWidget *parent);
    ~MVSubInnerFrame();

	void setObjectName(const QString &name);

private:
    void Initialize();
    void Clear();
	void SetObjectName(const QString &name);
};