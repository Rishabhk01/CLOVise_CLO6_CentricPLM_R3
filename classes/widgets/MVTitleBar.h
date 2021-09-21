#pragma once

#include <QFrame>
#include <QHBoxLayout>
#include <QLabel>
#include <QMouseEvent>
#include <QToolButton>
#include <QWidget>

class MVTitleBar : public QWidget
{
	Q_OBJECT
public:
	MVTitleBar(const QString& _title, QWidget* _pParent);
	virtual ~MVTitleBar();

	void SetTitle(const QString& _title);
	QString GetTitle() const;

protected:
	virtual void mousePressEvent(QMouseEvent* e) override;
	virtual void mouseMoveEvent(QMouseEvent* e) override;
	virtual void mouseReleaseEvent(QMouseEvent* e) override;

private slots:
	void onClickedCloseBtn();

private:
	QLabel*				m_pTitleLabel;
	QFrame*				m_pTitleFrame;
	QToolButton*		m_pCloseBtn;

	QString				m_titleStr;
	QPoint				m_clickedPos;
	bool				m_bPressed;

	void clear();
	void createDefaultWidget();
	void initialize();
	void initStyleSheet();
};