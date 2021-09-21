#pragma once
/*
* Copyright 2020-2021 CLO-Vise. All rights reserved
*
* @file CVTableWidgetItem.h
*
* @brief Class declaration for sorting of numbers in QTableWidget.
* This class has all the variable and function declarations which are used sorting of numbers in QTableWidget.
*
* @author GoVise
*
* @date 27-MAY-2020
*/
#include <QtGui>
#include <qtablewidget.h>

class CVTableWidgetItem : public QObject, public QTableWidgetItem
{
	Q_OBJECT
public:
	bool operator< (const QTableWidgetItem& _other) const
	{
		if (this->text().toInt() < _other.text().toInt())
			return true;
		else if (this->text().toFloat() < _other.text().toFloat())
			return true;
		else
			return false;
	}
};