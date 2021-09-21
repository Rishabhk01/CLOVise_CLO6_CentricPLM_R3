#pragma once
/*
* Copyright 2020-2021 CLO-Vise. All rights reserved
*
* @file ComboBoxItem.h
*
* @brief Class declaration for custom functionality on the QComboBoxItem.
* This class has all the variable and function declaration which are used custom functionality on the QComboBoxItem.
*
* @author GoVise
*
* @date 27-MAY-2020
*/
#ifndef CUSTOMDOUBLESPINBOX_H
#define CUSTOMDOUBLESPINBOX_H

#include <QtGui>
#include <QtWidgets>
#include <qcombobox.h>

class CustomDoubleSpinBox : public QDoubleSpinBox
{
	Q_OBJECT

public:
	CustomDoubleSpinBox();

public slots:
	void wheelEvent(QWheelEvent *e)
	{
		if (hasFocus())
			QDoubleSpinBox::wheelEvent(e);
	}
};
#endif 