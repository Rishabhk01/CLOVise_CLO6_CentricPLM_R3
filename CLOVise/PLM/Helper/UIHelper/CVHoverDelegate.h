#pragma once
/*
* Copyright 2020-2021 CLO-Vise. All rights reserved
*
* @file CVHoverDelegate.h
*
* @brief Class declaration for hover functionality on the widget.
* This class has all the variable and function declarations which are used hover functionality on the widget.
*
* @author GoVise
*
* @date 27-MAY-2020
*/
#ifndef CUSTOMHOVERDELEGATE_H
#define CUSTOMHOVERDELEGATE_H

#include <QtGui>
#include <QtWidgets>
#include <qtablewidget.h>

#include "classes/widgets/MVTableWidget.h"

class CVHoverDelegate : public QItemDelegate
{
    Q_OBJECT 

public:
    CVHoverDelegate(MVTableWidget* _tableWidget, QObject* _parent = 0);
	virtual ~CVHoverDelegate();

    void paint(QPainter* _painter, const QStyleOptionViewItem& _option,
        const QModelIndex& _index) const;

private:
	QTableWidget* m_tableWdt;
	int m_hoveredRow;
	QPen m_pen;

protected slots:
    void onItemEntered(QTableWidgetItem* _item);

};

#endif 