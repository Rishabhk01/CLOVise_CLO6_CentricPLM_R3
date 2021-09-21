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
#ifndef COMBOBOXITEM_H
#define COMBOBOXITEM_H

#include <QtGui>
#include <QtWidgets>
#include <qcombobox.h>

class ComboBoxItem : public QComboBox
{
    Q_OBJECT

private:
	QStringList m_itemList;
	QString m_defaultValue;

public:
	ComboBoxItem();
	void fillItemListAndDefaultValue(QStringList _stringList, QString _defaultValue);
	void focusOutEvent(QFocusEvent* event) override;
	//void leaveEvent(QEvent *event)override;
	//void mousePressEvent(QMouseEvent *e) override;

public slots:
    void onChangeText(const QString& _text);

	void wheelEvent(QWheelEvent *e)
	{
		if (hasFocus())
			QComboBox::wheelEvent(e);
	}
};

#endif 