#include "MVGroupFrame.h"

#include <QVBoxLayout>

MVGroupFrame::MVGroupFrame(QWidget *parent) : QFrame(parent)
{
    Initialize();
}

MVGroupFrame::~MVGroupFrame()
{
    Clear();
}

void MVGroupFrame::Initialize()
{
	this->SetObjectName(QString::fromUtf8("MVGroupFrame"));
			
    ConnectControl(true);
    m_bVisible = false;
}

void MVGroupFrame::Clear()
{
}

void MVGroupFrame::ConnectControl(bool b)
{

}

void MVGroupFrame::SetTitle(QString str)
{
    QBoxLayout *baseLayout = dynamic_cast<QBoxLayout*>(layout());
    if(baseLayout)
    {
        QHBoxLayout *titleLayout = new QHBoxLayout();
        titleLayout->setSpacing(6);
        titleLayout->setObjectName(QString::fromUtf8("titleLayout"));
        titleLayout->setContentsMargins(0, 0, 0, 0);

        QFont font;
        font.setPointSize(9);
        font.setBold(true);
        font.setWeight(75);

        QLabel *imgLb = new QLabel(this);
        imgLb->setObjectName(QString::fromUtf8("imgLabel"));
        titleLayout->addWidget(imgLb);

		QCheckBox *checkBox = new QCheckBox(this);
		checkBox->setObjectName(QString::fromUtf8("checkBox"));
		titleLayout->addWidget(checkBox);
		checkBox->setChecked(true);
		checkBox->hide();

        QLabel *titleLb = new QLabel(this);
        titleLb->setObjectName(QString::fromUtf8("titleLabel"));
        titleLb->setFont(font);
        titleLb->setText(str);
        titleLayout->addWidget(titleLb);

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
        subLabel->setPixmap(QPixmap(QString::fromUtf8(":/CLO_PLUGIN/INFOR/images/pop_btn_section_close_none.svg")));
        titleLayout->addWidget(subLabel);
        m_bVisible = true;
        SetVisibleSubMenu(m_bVisible);
        subLabel->installEventFilter(this);

        baseLayout->insertLayout(0, titleLayout);
    }
}

void MVGroupFrame::setObjectName(const QString &name)
{
}

void MVGroupFrame::SetVisibleSubMenu(bool b)
{
	QObjectList childList = this->children();
	for (int i = 0; i < childList.size(); i++)
	{
		QFrame *subFrame = dynamic_cast<QFrame*>(childList[i]);
		if (subFrame)
		{
			QLabel *label = dynamic_cast<QLabel*>(childList[i]);
			if (!label)
				subFrame->setVisible(b);
		}
	}
}

void MVGroupFrame::SetObjectName(const QString &name)
{
	QFrame::setObjectName(name);
}

bool MVGroupFrame::eventFilter(QObject *obj, QEvent *event)
{
    if(obj->objectName() == "subLabel")
    {
        if(event->type() == QEvent::MouseButtonPress)
        {
            QLabel *objLabel = dynamic_cast<QLabel*>(obj);
            if(m_bVisible)
            {
                m_bVisible = false;
                objLabel->setPixmap(QPixmap(QString::fromUtf8(":/CLO_PLUGIN/INFOR/images/pop_btn_section_open_none.svg")));
            }
            else
            {
                m_bVisible = true;
                objLabel->setPixmap(QPixmap(QString::fromUtf8(":/CLO_PLUGIN/INFOR/images/pop_btn_section_close_none.svg")));
            }
            SetVisibleSubMenu(m_bVisible);
        }
    }
    return false;
}

void MVGroupFrame::ShowImageLabel(bool bShow)
{
	QLabel *imgLb = findChild<QLabel*>("imgLabel");
	if (imgLb)
		imgLb->setVisible(bShow);
}

void MVGroupFrame::ShowCheckBox(bool bShow)
{
	QCheckBox *checkBox = findChild<QCheckBox*>("checkBox");

	if (checkBox)
	{	
		if (!checkBox->isVisible() && bShow)
			connect(checkBox, SIGNAL(toggled(bool)), this, SIGNAL(toggled(bool)));
		else if (checkBox->isVisible() && !bShow)
			disconnect(checkBox, SIGNAL(toggled(bool)), this, SIGNAL(toggled(bool)));

		checkBox->setVisible(bShow);
	}
}

void MVGroupFrame::SetChecked(bool b)
{
	QCheckBox *checkBox = findChild<QCheckBox*>("checkBox");

	disconnect(checkBox, SIGNAL(toggled(bool)), this, SIGNAL(toggled(bool)));

	if (b)
		checkBox->setChecked((bool)Qt::Checked);
	else
		checkBox->setChecked((bool)Qt::Unchecked);

	connect(checkBox, SIGNAL(toggled(bool)), this, SIGNAL(toggled(bool)));
}

bool MVGroupFrame::IsChecked()
{
	QCheckBox *checkBox = findChild<QCheckBox*>("checkBox");
	if (checkBox)
		return checkBox->isVisible();

	return false;
}

