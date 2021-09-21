#pragma once
#include <QTableWidget>

class MVTableWidget : public QTableWidget
{
	enum STYLE
	{
		RESULT_STYLE,
		END_OF_STYLE
	};

public:
	explicit MVTableWidget(QWidget* parent = 0, STYLE _style = RESULT_STYLE);
	virtual ~MVTableWidget();

private:
	void setDefaultProperty(STYLE _style);
};