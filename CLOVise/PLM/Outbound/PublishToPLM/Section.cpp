/*
* Copyright 2021-2022 CLO-Vise. All rights reserved
*
* @file Section.cpp
*
* @brief Class implementation for create  section on bom tab.
* This class has all the variable and methods implementation which are used to create Bom section.
*
* @author GoVise
*
* @date 10-OCT-2021
*/

#include <QPropertyAnimation>

#include "Section.h"
#include "CLOVise/PLM/Helper/Util/Logger.h"
Section::Section(const QString & title, const int animationDuration, QWidget* parent)
	: QWidget(parent), animationDuration(animationDuration)
{
	Logger::Debug("Section constructor() Start");
	toggleButton = new QToolButton(this);
	headerLine = new QFrame(this);
	toggleAnimation = new QParallelAnimationGroup(this);
	contentArea = new QScrollArea(this);
	mainLayout = new QGridLayout(this);
	m_sectionName = title;
	Logger::Debug("Section constructor() 1");
	toggleButton->setStyleSheet("QToolButton {border: none; color: #FFFFFF; font-size: 10px;}");
	toggleButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
	toggleButton->setArrowType(Qt::ArrowType::RightArrow);
	toggleButton->setText(title);
	toggleButton->setCheckable(true);
	toggleButton->setChecked(false);
	Logger::Debug("Section constructor() 2");
	headerLine->setFrameShape(QFrame::HLine);
	headerLine->setFrameShadow(QFrame::Sunken);
	headerLine->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);

	contentArea->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
	Logger::Debug("Section constructor() 3");
	// start out collapsed
	contentArea->setMaximumHeight(0);
	contentArea->setMinimumHeight(0);
	Logger::Debug("Section constructor() 4");
	// let the entire widget grow and shrink with its content
	toggleAnimation->addAnimation(new QPropertyAnimation(this, "minimumHeight"));
	toggleAnimation->addAnimation(new QPropertyAnimation(this, "maximumHeight"));
	toggleAnimation->addAnimation(new QPropertyAnimation(contentArea, "maximumHeight"));
	Logger::Debug("Section constructor() 5");
	mainLayout->setVerticalSpacing(0);
	mainLayout->setContentsMargins(0, 0, 0, 0);

	int row = 0;
	mainLayout->addWidget(toggleButton, row, 0, 1, 1, Qt::AlignLeft);
	mainLayout->addWidget(headerLine, row++, 2, 1, 1);
	mainLayout->addWidget(contentArea, row, 0, 1, 3);
	setLayout(mainLayout);
	Logger::Debug("Section constructor() 6");
	connect(toggleButton, &QToolButton::toggled, this, &Section::toggle);
	Logger::Debug("Section constructor() End");
}

/*
	* Description - toggle() method for collapse and expand the sections.
	* Parameter - bool collapsed
	* Exception -
	* Return -
	*/
void Section::toggle(bool collapsed) {
	toggleButton->setArrowType(collapsed ? Qt::ArrowType::DownArrow : Qt::ArrowType::RightArrow);
	toggleAnimation->setDirection(collapsed ? QAbstractAnimation::Forward : QAbstractAnimation::Backward);
	toggleAnimation->start();
}

/*
	* Description - GetSectionName() method used to get the section name.
	* Parameter -
	* Exception -
	* Return -
	*/
QString Section::GetSectionName()
{
	return m_sectionName;
}

/*
	* Description - setContentLayout() method set layout for section
	* Parameter -
	* Exception -
	* Return -
	*/
void Section::setContentLayout(QLayout & contentLayout)
{
	Logger::Debug("setContentLayout() Start");
	delete contentArea->layout();
	contentArea->setLayout(&contentLayout);
	const auto collapsedHeight = sizeHint().height() - contentArea->maximumHeight();
	auto contentHeight = contentLayout.sizeHint().height();
	Logger::Debug("setContentLayout() 1");
	for (int i = 0; i < toggleAnimation->animationCount() - 1; ++i)
	{
		QPropertyAnimation* SectionAnimation = static_cast<QPropertyAnimation *>(toggleAnimation->animationAt(i));
		SectionAnimation->setDuration(animationDuration);
		SectionAnimation->setStartValue(collapsedHeight);
		SectionAnimation->setEndValue(collapsedHeight + contentHeight);
		SectionAnimation->setDirection(QAbstractAnimation::Forward);
	}
	Logger::Debug("setContentLayout() 2");
	QPropertyAnimation* contentAnimation = static_cast<QPropertyAnimation *>(toggleAnimation->animationAt(toggleAnimation->animationCount() - 1));
	contentAnimation->setDuration(animationDuration);
	contentAnimation->setStartValue(0);
	contentAnimation->setEndValue(contentHeight);
	contentAnimation->setDirection(QAbstractAnimation::Forward);
	Logger::Debug("setContentLayout() End");
}
