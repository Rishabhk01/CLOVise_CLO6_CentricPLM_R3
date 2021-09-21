#include "MVGroupBox.h"

#include <QVBoxLayout>

MVGroupBox::MVGroupBox(QWidget *parent) : QFrame(parent)
{
	Initialize();
}

MVGroupBox::~MVGroupBox()
{
	Clear();
}

void MVGroupBox::Initialize()
{
	this->SetObjectName(QString::fromUtf8("MVGroupBox"));
	this->setStyleSheet(QString(
		"QFrame#%1 {border: none;}").arg(objectName()));

	ConnectControl(true);
	m_bVisible = false;

	m_pTitleCheckBox = new QCheckBox(this);
	m_pTitleCheckBox->setObjectName(QString::fromUtf8("titleCheckBox"));
	m_pTitleCheckBox->setMinimumSize(150, 15);
	m_pTitleCheckBox->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	m_pTitleCheckBox->setChecked(false);
	connect(m_pTitleCheckBox, SIGNAL(stateChanged(int)), this, SLOT(OnChangeCheckStatus(int)));
}

void MVGroupBox::Clear()
{
}

void MVGroupBox::ConnectControl(bool b)
{
}

void MVGroupBox::SetTitle(QString str)
{
	QBoxLayout *baseLayout = dynamic_cast<QBoxLayout*>(layout());
	if(baseLayout)
	{
		this->SetObjectName(str);

		QHBoxLayout *titleLayout = new QHBoxLayout();
		titleLayout->setSpacing(6);
		titleLayout->setObjectName(QString::fromUtf8("titleLayout"));
		titleLayout->setContentsMargins(0, 0, 0, 0);

		QFont font;
		font.setPointSize(7);
		font.setBold(true);
		font.setWeight(75);

		//QCheckBox *titleCheckBox = new QCheckBox(this);
		//titleCheckBox->setObjectName(QString::fromUtf8("titleCheckBox"));
		//titleCheckBox->setMinimumSize(150, 15);
		//titleCheckBox->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
		////titleCheckBox->setFont(font);
		//titleCheckBox->setText(str);
		//connect(titleCheckBox, SIGNAL(stateChanged(int)), this, SLOT(OnChangeCheckStatus(int)));
		m_pTitleCheckBox->setText(str);
		titleLayout->addWidget(m_pTitleCheckBox);

		QSpacerItem *titleSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
		titleLayout->addItem(titleSpacer);

		QLabel *subLabel = new QLabel(this);
		subLabel->setObjectName(QString::fromUtf8("subLabel"));
		QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
		sizePolicy.setHorizontalStretch(0);
		sizePolicy.setVerticalStretch(0);
		sizePolicy.setHeightForWidth(subLabel->sizePolicy().hasHeightForWidth());
		subLabel->setSizePolicy(sizePolicy);
		subLabel->setMinimumSize(QSize(15, 10));
		subLabel->setMaximumSize(QSize(20, 12));
		subLabel->setPixmap(QPixmap(QString::fromUtf8(":/CLO_PLUGIN/INFOR/images/pop_btn_section_open_none.svg")));		
		titleLayout->addWidget(subLabel);
		//m_bVisible = true;
		SetVisibleSubMenu(m_bVisible);
		subLabel->installEventFilter(this);

		baseLayout->insertLayout(0, titleLayout);
	}
}

void MVGroupBox::SetVisibleSubMenu(bool b)
{
	QObjectList childList = this->children();
	for(int i = 0; i < childList.size(); i++)
	{
		QFrame *subFrame = dynamic_cast<QFrame*>(childList[i]);
		if(subFrame)
		{
			m_bVisible = b;
			QLabel *label = dynamic_cast<QLabel*>(childList[i]);
			if(!label)
				subFrame->setVisible(m_bVisible);

			if(label)
			{
				if(m_bVisible)
					label->setPixmap(QPixmap(QString::fromUtf8(":/CLO_PLUGIN/INFOR/images/pop_btn_section_close_none.svg")));
				else
					label->setPixmap(QPixmap(QString::fromUtf8(":/CLO_PLUGIN/INFOR/images/pop_btn_section_open_none.svg")));
			}
		}
	}

	emit ChangeSubMenuVisibleStatus(b);
}

// [James: 2013-10-22] MVIS-4010 => .ui파일에서 objectName 설정하지 못하도록 하기위해
void MVGroupBox::setObjectName(const QString &name)
{
}

// [James: 2013-10-22] MVIS-4010 => Initialize()에서 objectName 설정하기 위해
void MVGroupBox::SetObjectName(const QString &name)
{
	QFrame::setObjectName(name);
}

void MVGroupBox::SetCheckedCheckBox(bool b)
{
	m_pTitleCheckBox->setChecked(b);
}
bool MVGroupBox::GetCheckedCheckBox()
{
	return m_pTitleCheckBox->isChecked();
}


void MVGroupBox::OnChangeCheckStatus(int state)
{
	emit ChangeCheckStatus(state);
}

bool MVGroupBox::eventFilter(QObject *obj, QEvent *event)
{
	if(obj->objectName() == "subLabel")
	{
		if(event->type() == QEvent::MouseButtonPress)
		{
			QLabel *objLabel = dynamic_cast<QLabel*>(obj);
			if(m_bVisible)
			{
				m_bVisible = false;
			}
			else
			{
				m_bVisible = true;
			}
			SetVisibleSubMenu(m_bVisible);
			emit ChangeVisibleStatus(m_bVisible);

			return true;
		}
	}
	return false;
}
