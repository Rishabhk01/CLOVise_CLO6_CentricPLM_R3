/*
* Copyright 2020-2021 CLO-Vise. All rights reserved
*
* @file CVHoverDelegate.cpp
*
* @brief Class implementation for custom functionality on the widget.
* This class has all the variable and function implementation which are used custom functionality on the widget.
*
* @author GoVise
*
* @date 27-MAY-2020
*/
#include <qitemdelegate.h>

#include "CVHoverDelegate.h"
#include "CLOVise/PLM/Helper/Util/Helper.h"

CVHoverDelegate::CVHoverDelegate(MVTableWidget* _tableWidget, QObject* _parent)
	: QItemDelegate(_parent), m_tableWdt(_tableWidget), m_hoveredRow(-1)
{
	// mouse tracking have to be true, otherwise item entered won't emit
	m_tableWdt->setMouseTracking(true);
	m_pen = QPen(QColor(0, 0, 0), 1, _tableWidget->gridStyle());
	m_tableWdt = _tableWidget;
	connect(m_tableWdt, SIGNAL(itemEntered(QTableWidgetItem*)), this, SLOT(onItemEntered(QTableWidgetItem*)));
}

CVHoverDelegate::~CVHoverDelegate()
{

}

void CVHoverDelegate::onItemEntered(QTableWidgetItem* _item)
{
	m_hoveredRow = _item->row();
	m_tableWdt->viewport()->update();  // force update
}

void CVHoverDelegate::paint(QPainter* _painter, const QStyleOptionViewItem& _option,
	const QModelIndex& _index) const
{
	QStyleOptionViewItemV4 viewItemOption(_option);

	if (_index.row() == m_hoveredRow)
	{		
		
		QColor ItemForegroundColor = _index.data(Qt::ForegroundRole).value<QColor>();
		if (ItemForegroundColor != _option.palette.color(QPalette::WindowText))
		{
			viewItemOption.palette.setColor(QPalette::Text, highlightedTextColor);
			_painter->fillRect(_option.rect, hoverHighlightColor);
		}
	
		
	}	
	
	QPen oldPen = _painter->pen();
	_painter->setPen(m_pen);
	QPoint p1 = QPoint(_option.rect.bottomLeft().x() - 1, _option.rect.bottomLeft().y());
	QPoint p2 = QPoint(_option.rect.bottomRight().x() + 1, _option.rect.bottomRight().y());
	_painter->drawLine(p1, p2);
	_painter->setPen(oldPen);
	QItemDelegate::paint(_painter, viewItemOption, _index);
}