/*
* Copyright 2020-2021 CLO-Vise. All rights reserved
*
* @file CVTitleBar.cpp
*
* @brief Class implementation for adding the frameless title bar with close button.
*
* @author GoVise
*
* @date 27-MAY-2020
*/
#include "CVTitleBar.h"

CVTitleBar::CVTitleBar(const QString& _title, QWidget* _pParent) : QWidget(_pParent)
{
	initialize();

	SetTitle(_title);

	createDefaultWidget();

	initStyleSheet();
	m_pCloseBtn->hide();
}

CVTitleBar::~CVTitleBar()
{
	clear();
}

/*
* Description - SetTitle() method used to set title to the widget.
* Parameter -  QString.
* Exception -
* Return - 
*/
void CVTitleBar::SetTitle(const QString& _title)
{
	m_titleStr = _title;

	if (m_pTitleLabel != nullptr) {
		m_pTitleLabel->setText(_title);
	}
}

/*
* Description - SetTitle() method used to get  title for the widget.
* Parameter - 
* Exception -
* Return - QString.
*/
QString CVTitleBar::GetTitle() const
{
	return m_titleStr;
}

/*
* Description - mousePressEvent() method used to mouse press event of the widget.
* Parameter -  QMouseEvent.
* Exception -
* Return - 
*/
void CVTitleBar::mousePressEvent(QMouseEvent* e)
{
	m_bPressed = true;
	m_clickedPos = mapToParent(e->pos());
}

/*
* Description - mouseMoveEvent() -
* Parameter -  QMouseEvent.
* Exception -
* Return - json.
*/
void CVTitleBar::mouseMoveEvent(QMouseEvent* e)
{
	if (m_bPressed == false) {
		return;
	}

	parentWidget()->move(e->globalPos() - m_clickedPos);
}

/*
* Description - mouseReleaseEvent() method used mouse event of the widget.
* Parameter -  QMouseEvent.
* Exception -
* Return - 
*/
void CVTitleBar::mouseReleaseEvent(QMouseEvent* e)
{
	m_bPressed = false;
}

/*
* Description - clear() method used to clear actions of the widget.
* Parameter - 
* Exception -
* Return - 
*/
void CVTitleBar::clear()
{
	if (m_pCloseBtn != nullptr)	{
		delete m_pCloseBtn;
		m_pCloseBtn = nullptr;
	}

	if (m_pTitleLabel != nullptr) {
		delete m_pTitleLabel;
		m_pTitleLabel = nullptr;
	}

	if (m_pTitleFrame != nullptr) {
		delete m_pTitleFrame;
		m_pTitleFrame = nullptr;
	}
}

/*
* Description - createDefaultWidget() method used to create default widget to the widget.
* Parameter - 
* Exception -
* Return - 
*/
void CVTitleBar::createDefaultWidget()
{
	// Create Frame
	m_pTitleFrame = new QFrame(this);
	m_pTitleFrame->setObjectName("TitleFrame");
	m_pTitleFrame->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	m_pTitleFrame->setMinimumHeight(30);

	layout()->addWidget(m_pTitleFrame);

	// Create Horizontal Layout for Frame
	QHBoxLayout* pLayout = new QHBoxLayout(m_pTitleFrame);
	pLayout->setContentsMargins(4, 0, 4, 0);
	pLayout->setSpacing(0);

	// Create Title Label
	m_pTitleLabel = new QLabel(m_pTitleFrame);
	m_pTitleLabel->setObjectName("TitleLabel");
	m_pTitleLabel->setText(GetTitle());
	m_pTitleLabel->setAlignment(Qt::AlignCenter);

	// Create Close Button
	m_pCloseBtn = new QToolButton(m_pTitleFrame);
	m_pCloseBtn->setObjectName("CloseButton");
	m_pCloseBtn->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	QSize iconSize(15, 16);
	QIcon iconClose;
	iconClose.addFile(":/CLOVise/PLM/Images/icon_cancel_none.svg", iconSize, QIcon::Normal, QIcon::Off);
	iconClose.addFile(":/CLOVise/PLM/Images/icon_cancel_over.svg", iconSize, QIcon::Normal, QIcon::On);
	iconClose.addFile(":/CLOVise/PLM/Images/icon_cancel_over.svg", iconSize, QIcon::Active, QIcon::Off);
	iconClose.addFile(":/CLOVise/PLM/Images/icon_cancel_over.svg", iconSize, QIcon::Active, QIcon::On);
	m_pCloseBtn->setFixedSize(iconSize);
	m_pCloseBtn->setIcon(iconClose);
	m_pCloseBtn->setIconSize(iconSize);
	m_pCloseBtn->setAutoRaise(true);

	pLayout->addWidget(m_pTitleLabel);
	pLayout->addWidget(m_pCloseBtn);

	connect(m_pCloseBtn, SIGNAL(clicked()), this, SLOT(onClickedCloseBtn()));
}

/*
* Description - initialize() method used to initialize the widget.
* Parameter - 
* Exception -
* Return - 
*/
void CVTitleBar::initialize()
{
	if (layout() == nullptr) {
		QHBoxLayout* pLayout = new QHBoxLayout(this);
		pLayout->setContentsMargins(0, 0, 0, 0);
		pLayout->setSpacing(0);
		setLayout(pLayout);
	}

	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

	m_pTitleFrame = nullptr;
	m_pTitleLabel = nullptr;
	m_pCloseBtn = nullptr;

	m_titleStr.clear();
	m_clickedPos = QPoint(0, 0);
	m_bPressed = false;
}

/*
* Description - initStyleSheet() method used to set StyleSheet to the widget.
* Parameter - 
* Exception -
* Return - 
*/
void CVTitleBar::initStyleSheet()
{
	m_pTitleFrame->setStyleSheet("backgrouund: darkblue; border: none;");
	m_pTitleLabel->setStyleSheet("background: transparent; color: white;");
}
 
/*
* Description - onClickedCloseBtn() method is a slot for close button.
* Parameter - 
* Exception -
* Return - 
*/
void CVTitleBar::onClickedCloseBtn()
{
	if (parentWidget() != nullptr) {		
		QString objectName = parentWidget()->objectName();		
		parentWidget()->close();
	}
}