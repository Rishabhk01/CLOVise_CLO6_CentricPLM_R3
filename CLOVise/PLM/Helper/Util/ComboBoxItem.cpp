/*
* Copyright 2020-2021 CLO-Vise. All rights reserved
*
* @file ComboBoxItem.cpp
*
* @brief Class implementation for custom functionality on the QComboBoxItem.
* This class has all the variable and function implementation which are used custom functionality on the QComboBoxItem.
*
* @author GoVise
*
* @date 27-MAY-2020
*/
#include "ComboBoxItem.h"
#include "CLOVise/PLM/Helper/Util/Helper.h"
#include "CLOAPIInterface/CLOAPIInterface.h"

ComboBoxItem::ComboBoxItem()
{
#ifdef __APPLE__	// Settinng Windows OS style to QComboBox on MAC OS

	this->setStyle(QStyleFactory::create("Windows"));
	this->setItemDelegate(new QStyledItemDelegate());;

#endif

	this->setAttribute(Qt::WA_MacShowFocusRect, false);
	m_defaultValue = "";
}
/*
* Private Slot, which gets called in Connect statments
*/
void ComboBoxItem::onChangeText(const QString& _text)
{

}

void ComboBoxItem::fillItemListAndDefaultValue(QStringList _stringList, QString _defaultValue)
{
	m_itemList = _stringList;
	m_defaultValue = _defaultValue;
}


//void ComboBoxItem::leaveEvent(QEvent *event)
//{
//	__super::leaveEvent(event);
//	UTILITY_API->DisplayMessageBox("Hi");
//	
//}

void ComboBoxItem::focusOutEvent(QFocusEvent* event)
{
	QString string = this->currentText();
	if (this->isEditable())
	{
		if (!string.isEmpty())
		{
			if (!m_itemList.contains(string))
			{
				int indexOfDefaultValue = this->findText(m_defaultValue);
				this->setCurrentIndex(indexOfDefaultValue);
				////this->setEditable(false);
				//UTILITY_API->DisplayMessageBox("inside 1");


			}
			else
			{
				//UTILITY_API->DisplayMessageBox("inside 2");
			}
		}
		else
		{
			int indexOfBlankValue = this->findText(QString::fromStdString(BLANK));
			this->setCurrentIndex(indexOfBlankValue);
			//UTILITY_API->DisplayMessageBox("inside 3");
		}
		this->lineEdit()->setReadOnly(true);
#ifdef __APPLE__
		QComboBox::focusOutEvent(event);
#else
        __super::focusOutEvent(event);
#endif
		this->lineEdit()->setReadOnly(false);
		this->setStyleSheet(COMBOBOX_STYLE);
	}
	

}

//void ComboBoxItem::mousePressEvent(QMouseEvent *e)
//{
//	UTILITY_API->DisplayMessageBox("Hi mouse");
//	this->setEditable(true);
//}