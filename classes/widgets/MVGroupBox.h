#pragma once

#include <QCheckBox>
#include <QEvent>
#include <QFrame>
#include <QLabel>

class MVGroupBox : public QFrame
{
	Q_OBJECT
public:
	MVGroupBox(QWidget *parent);
	~MVGroupBox();

	void SetTitle(QString str);
	void setObjectName(const QString &name);
	void SetCheckedCheckBox(bool b);
	void SetVisibleSubMenu(bool b);

	bool GetCheckedCheckBox();
signals:
	void ChangeCheckStatus(int state);
	void ChangeSubMenuVisibleStatus(bool b);
	void ChangeVisibleStatus(bool b);

public slots:
	void OnChangeCheckStatus(int state);

private:
	void Initialize();
	void Clear();
	void ConnectControl(bool b);
	void SetObjectName(const QString &name);
	bool eventFilter(QObject *obj, QEvent *event);

	bool m_bVisible;
	QCheckBox *m_pTitleCheckBox;
};
