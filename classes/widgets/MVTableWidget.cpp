#include "MVTableWidget.h"

MVTableWidget::MVTableWidget(QWidget* parent, STYLE _style)
    :QTableWidget(parent)
{
    setDefaultProperty(_style);
}

MVTableWidget::~MVTableWidget()
{

}

void MVTableWidget::setDefaultProperty(STYLE _style)
{
    if (_style == RESULT_STYLE)
    {
        setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);        
        
        setLineWidth(50);
        setMidLineWidth(40);
        
        setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        setAutoScrollMargin(50);
        setDragEnabled(false);
        
        setDropIndicatorShown(false);        
        
        setWordWrap(true);

        setEditTriggers(QAbstractItemView::NoEditTriggers);
        setFocusPolicy(Qt::NoFocus);
        setSelectionMode(QAbstractItemView::NoSelection);
    }
}