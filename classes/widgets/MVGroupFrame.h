#pragma once

#include <QCheckBox>
#include <QEvent>
#include <QFrame>
#include <QLabel>

class MVGroupFrame : public QFrame
{
    Q_OBJECT
public:
    MVGroupFrame(QWidget *parent);
    ~MVGroupFrame();

    void SetTitle(QString str);
	void setObjectName(const QString &name); // [James: 2013-10-22] MVIS-4010
	void SetVisibleSubMenu(bool b);

private:
    void Initialize();
    void Clear();
    void ConnectControl(bool b);    
	void SetObjectName(const QString &name); // [James: 2013-10-22] MVIS-4010
    bool eventFilter(QObject *obj, QEvent *event);

    bool m_bVisible;

public:
	void ShowImageLabel(bool bShow);
	void ShowCheckBox(bool bShow);
	void SetChecked(bool b);
	bool IsChecked();
};
