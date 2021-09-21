#pragma once
#include <QTreeWidget>

class MVTreeWidget : public QTreeWidget
{
	enum STYLE
	{
		SEARCH_STYLE,
		END_OF_STYLE
	};

public:
	explicit MVTreeWidget(QWidget* parent = 0, STYLE _style = SEARCH_STYLE);
	virtual ~MVTreeWidget();

private:
	void setDefaultProperty(STYLE _style);
};