#include "MVTitleBar.h"
//#include "CLO_PLUGIN/INFOR/Utilities/Utility.h"

MVTitleBar::MVTitleBar(const QString& _title, QWidget* _pParent) : QWidget(_pParent)
{
	initialize();

	SetTitle(_title);

	createDefaultWidget();

	initStyleSheet();
}

MVTitleBar::~MVTitleBar()
{
	clear();
}

void MVTitleBar::SetTitle(const QString& _title)
{
	m_titleStr = _title;

	if (m_pTitleLabel != nullptr) {
		m_pTitleLabel->setText(_title);
	}
}

QString MVTitleBar::GetTitle() const
{
	return m_titleStr;
}

void MVTitleBar::mousePressEvent(QMouseEvent* e)
{
	m_bPressed = true;
	m_clickedPos = mapToParent(e->pos());
}

void MVTitleBar::mouseMoveEvent(QMouseEvent* e)
{
	if (m_bPressed == false) {
		return;
	}

	parentWidget()->move(e->globalPos() - m_clickedPos);
}

void MVTitleBar::mouseReleaseEvent(QMouseEvent* e)
{
	m_bPressed = false;
}

void MVTitleBar::clear()
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

void MVTitleBar::createDefaultWidget()
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

void MVTitleBar::initialize()
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

void MVTitleBar::initStyleSheet()
{
	//m_pTitleFrame->setStyleSheet("background: darkblue; border: none;");
	//m_pTitleLabel->setStyleSheet("background: transparent; ""font: 75 10pt;" "font-family:Tahoma;" "font-weight: bold;");
	m_pTitleFrame->setStyleSheet("backgrouund: darkblue; border: none;");
	m_pTitleLabel->setStyleSheet("background: transparent; color: white;");
}
 
void MVTitleBar::onClickedCloseBtn()
{
	if (parentWidget() != nullptr) {
		
		QString objectName = parentWidget()->objectName();
		
		//Utility::Logger(objectName.toStdString() + " -> closeWindowPressed() -> Start");
		parentWidget()->close();
		//Utility::Logger(objectName.toStdString() + " -> closeWindowPressed() -> End");
	}
}