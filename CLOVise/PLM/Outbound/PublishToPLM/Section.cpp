/*
    Elypson/qt-collapsible-section
    (c) 2016 Michael A. Voelkel - michael.alexander.voelkel@gmail.com

    This file is part of Elypson/qt-collapsible section.

    Elypson/qt-collapsible-section is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Elypson/qt-collapsible-section is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Elypson/qt-collapsible-section. If not, see <http://www.gnu.org/licenses/>.
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

    contentArea->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
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


void Section::toggle(bool collapsed) {
    toggleButton->setArrowType(collapsed ? Qt::ArrowType::DownArrow : Qt::ArrowType::RightArrow);
    toggleAnimation->setDirection(collapsed ? QAbstractAnimation::Forward : QAbstractAnimation::Backward);
    toggleAnimation->start();
}

QString Section::GetSectionName()
{
	return m_sectionName;
}
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
