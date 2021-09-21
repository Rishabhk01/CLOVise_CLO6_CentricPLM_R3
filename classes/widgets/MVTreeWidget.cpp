#include "MVTreeWidget.h"

MVTreeWidget::MVTreeWidget(QWidget* parent, STYLE _style)
	:QTreeWidget(parent)
{
	setDefaultProperty(_style);
}

MVTreeWidget::~MVTreeWidget()
{

}

void MVTreeWidget::setDefaultProperty(STYLE _style)
{
    if (_style == SEARCH_STYLE)
    {
        setLineWidth(4);
        setDropIndicatorShown(false);        
        setSelectionMode(QAbstractItemView::NoSelection);
        setHorizontalScrollMode(QAbstractItemView::ScrollPerItem);
        setRootIsDecorated(false);
        setWordWrap(true);
        setColumnCount(2);
        setHeaderHidden(true);
    }
}