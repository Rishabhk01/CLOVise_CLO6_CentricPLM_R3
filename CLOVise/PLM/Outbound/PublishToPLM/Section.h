/*
* Copyright 2021-2022 CLO-Vise. All rights reserved
*
* @file AddNewBom.h
*
* @brief Class implementation for create Bom table on tab.
* This class has all the variable declarations and function declarations which are used to create section.
*
* @author GoVise
*
* @date 10-OCT-2021
*/

#ifndef SECTION_H
#define SECTION_H

#include <QFrame>
#include <QGridLayout>
#include <QParallelAnimationGroup>
#include <QScrollArea>
#include <QToolButton>
#include <QWidget>

class Section : public QWidget {

private:

	QGridLayout* mainLayout;
	QToolButton* toggleButton;
	QFrame* headerLine;
	QParallelAnimationGroup* toggleAnimation;
	QScrollArea* contentArea;
	int animationDuration;
	QString m_sectionName;
	QString GetSectionName();

public slots:

	void toggle(bool collapsed);


public:
	explicit Section(const QString & title = "", const int animationDuration = 100, QWidget* parent = 0);

	void setContentLayout(QLayout & contentLayout);
};

#endif // SECTION_H
