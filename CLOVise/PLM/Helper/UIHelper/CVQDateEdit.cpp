/**************************************************************************
**
** Copyright (c) 2013 Qualiant Software GmbH
**
** Author: Andreas Holzammer, KDAB (andreas.holzammer@kdab.com)
**
** Contact: Qualiant Software (d.oberkofler@qualiant.at)
**
** GNU Lesser General Public License Usage
**
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** If you have questions regarding the use of this file, please contact
** Qualiant Software at d.oberkofler@qualiant.at.
**
**************************************************************************/

#include "CVQDateEdit.h"
#include <QStyle>
#include <QToolButton>
#include <QLineEdit>
#include <QStyleOptionSpinBox>
#include <QKeyEvent>
#include <QCalendarWidget>
#include <iostream>
#include "CLOVise/PLM/Helper/UIHelper/CVQDateEdit.h"
#include "CLOVise/PLM/Helper/Util/FormatHelper.h"
#include "CLOVise/PLM/Helper/Util/Definitions.h"

using namespace std;

/*!
  \class DateEditEx dateeditex.h
  \brief A DateEdit with a nullable date

  This is a subclass of QDateEdit that has the additional feature
  of allowing to select a empty date. This can be achived with the
  clear button or by selecting the whole date and press backspace.

  To set an empty date from code, use the setter with an invalid
  date. To check weather the date is empty check if the date is valid,
  which comes from the getter.

  \sa QDateEdit
*/

class CVQDateEdit::Private {
public:
	Private(CVQDateEdit* qq) : q(qq), clearButton(0), null(false), nullable(false), specialText(DATE_FORMAT_TEXT.toStdString()) {

		QLineEdit *edit = q->findChild<QLineEdit*>("qt_spinbox_lineedit");
		//edit->setPlaceholderText("mm/dd/yyyy");
		//edit->setMaxLength(10);

		q->setStyleSheet("QDateEdit { background-color: #222224;font-size: 10px; font-face: ArialMT; min-height: 20px; border-bottom: 1px inset #262626; border-left: 1px inset #0D0D0D; border-top: 1px inset #0D0D0D; border-right: 1px inset #262626; padding: 1px; spacing: 1px; color: #808080; font-size: 10px; }"
			"QDateEdit::drop-down { image: url(:/CLOVise/PLM/Images/ui_dropbox_none.svg); width: 18px; height: 18px; subcontrol-position: right center; subcontrol-origin:margin;}"
			"QDateEdit::drop-down:hover { image: url(:/CLOVise/PLM/Images/ui_dropbox_over.svg); width: 18px; height: 18px; subcontrol-position: right center; subcontrol-origin:margin;}");
	}

	CVQDateEdit* const q;

	QToolButton* clearButton = new QToolButton();

	bool null;
	bool nullable;
	string specialText;

	void setNull(bool n) {
		null = n;
		if (null) {
			//QLineEdit *edit = qFindChild<QLineEdit *>(q, "qt_spinbox_lineedit");
			QLineEdit *edit = q->findChild<QLineEdit*>("qt_spinbox_lineedit");
			if (!edit->text().isEmpty()) {
				edit->clear();
				q->setMinimumDate(QDate(1899, 01, 01));
				q->setSpecialValueText(QString::fromStdString(specialText));
				q->setDate(QDate(0001, 01, 01));
				q->setStyleSheet("QDateEdit { background-color: #222224;font-size: 10px; font-face: ArialMT; min-height: 20px; border-bottom: 1px inset #262626; border-left: 1px inset #0D0D0D; border-top: 1px inset #0D0D0D; border-right: 1px inset #262626; padding: 1px; spacing: 1px; color: #808080; font-size: 10px; }"
					"QDateEdit::drop-down { image: url(:/CLOVise/PLM/Images/ui_dropbox_none.svg); width: 18px; height: 18px; subcontrol-position: right center; subcontrol-origin:margin;}"
					"QDateEdit::drop-down:hover { image: url(:/CLOVise/PLM/Images/ui_dropbox_over.svg); width: 18px; height: 18px; subcontrol-position: right center; subcontrol-origin:margin;}");
				//clearButton->setStyleSheet("QToolButton { margin-right: 5px; margin-left: 5px; }");
				//q->setDateTime(QDateTime::currentDateTime());
				//q->calendarWidget()->showToday();
			}
		}
		if (nullable) {
			clearButton->setVisible(!null);
		}

	}
};

/*!
  \reimp
*/
CVQDateEdit::CVQDateEdit(QWidget *parent) :
	QDateEdit(parent), d(new Private(this))
{
	installEventFilter(this);
	QLineEdit *edit = this->findChild<QLineEdit*>("qt_spinbox_lineedit");
	//edit->installEventFilter(this);
	//edit->setMaxLength(10);
	edit->setReadOnly(true);
	this->setSpecialValueText(QString::fromStdString(d->specialText));
	this->setMinimumDate(QDate(1899, 01, 01));
	this->setDate(QDate(0001, 01, 01));
	this->setStyleSheet("QDateEdit { background-color: #222224;font-size: 10px; font-face: ArialMT; min-height: 20px; border-bottom: 1px inset #262626; border-left: 1px inset #0D0D0D; border-top: 1px inset #0D0D0D; border-right: 1px inset #262626; padding: 1px; spacing: 1px; color: #FFFFFF; font-size: 10px; }"
		"QDateEdit::drop-down { image: url(:/CLOVise/PLM/Images/ui_dropbox_none.svg); width: 18px; height: 18px; subcontrol-position: right center; subcontrol-origin:margin;}"
		"QDateEdit::drop-down:hover { image: url(:/CLOVise/PLM/Images/ui_dropbox_over.svg); width: 18px; height: 18px; subcontrol-position: right center; subcontrol-origin:margin;}");
}

/*!
 * \brief returns date, if empty date is invalid
 * \return date, if empty date is invalid
 */
QDateTime CVQDateEdit::dateTime() const
{
	if (d->nullable && d->null) {
		return QDateTime();
	}
	else {
		return QDateEdit::dateTime();
	}
}

/*!
 * \brief returns date, if empty date is invalid
 * \return date, if empty date is invalid
 */
QDate CVQDateEdit::date() const
{
	if (d->nullable && d->null) {
		return QDate();
	}
	else {
		return QDateEdit::date();
	}
}

/*!
 * \brief returns date, if empty date is invalid
 * \return date, if empty date is invalid
 */
QTime CVQDateEdit::time() const
{
	if (d->nullable && d->null) {
		return QTime();
	}
	else {
		return QDateEdit::time();
	}
}

/*!
 * \brief sets a date, if date is invalid a
 * empty date is shown
 */
void CVQDateEdit::setDateTime(const QDateTime &dateTime)
{
	if (d->nullable && !dateTime.isValid()) {
		d->setNull(true);
	}
	else {
		d->setNull(false);
		QDateEdit::setDateTime(dateTime);
	}
}

/*!
 * \brief sets a date, if date is invalid a
 * empty date is shown
 */
void CVQDateEdit::setDate(const QDate &date)
{
	if (d->nullable && !date.isValid()) {
		d->setNull(true);
		QDateEdit::setDate(date);
	}
	else {
		setStyleSheet("QDateEdit { background-color: #222224;font-size: 10px; font-face: ArialMT; min-height: 20px; border-bottom: 1px inset #262626; border-left: 1px inset #0D0D0D; border-top: 1px inset #0D0D0D; border-right: 1px inset #262626; padding: 1px; spacing: 1px; color: #FFFFFF; font-size: 10px; }"
			"QDateEdit::drop-down { image: url(:/CLOVise/PLM/Images/ui_dropbox_none.svg); width: 18px; height: 18px; subcontrol-position: right center; subcontrol-origin:margin;}"
			"QDateEdit::drop-down:hover { image: url(:/CLOVise/PLM/Images/ui_dropbox_over.svg); width: 18px; height: 18px; subcontrol-position: right center; subcontrol-origin:margin;}");
		d->setNull(false);
		QDateEdit::setDate(date);
	}
}

/*!
 * \brief sets a date, if date is invalid a
 * empty date is shown
 */
void CVQDateEdit::setTime(const QTime &time)
{
	if (d->nullable && !time.isValid()) {
		d->setNull(true);
	}
	else {
		d->setNull(false);
		QDateEdit::setTime(time);
	}
}

/*!
 * \brief returns date can be empty
 * \return true, if date can be emtpy
 */
bool CVQDateEdit::isNullable() const
{
	return d->nullable;
}
void CVQDateEdit::setSpecialText(string _specialText)
{
	d->specialText = _specialText;
	this->setSpecialValueText(QString::fromStdString(d->specialText));
	this->setStyleSheet("QDateEdit { background-color: #222224;font-size: 10px; font-face: ArialMT; min-height: 20px; border-bottom: 1px inset #262626; border-left: 1px inset #0D0D0D; border-top: 1px inset #0D0D0D; border-right: 1px inset #262626; padding: 1px; spacing: 1px; color: #808080; font-size: 10px; }"
		"QDateEdit::drop-down { image: url(:/CLOVise/PLM/Images/ui_dropbox_none.svg); width: 18px; height: 18px; subcontrol-position: right center; subcontrol-origin:margin;}"
		"QDateEdit::drop-down:hover { image: url(:/CLOVise/PLM/Images/ui_dropbox_over.svg); width: 18px; height: 18px; subcontrol-position: right center; subcontrol-origin:margin;}");
}
/*!
 * \brief sets weahter the date can be empty
 */
void CVQDateEdit::setNullable(bool enable)
{
	d->nullable = enable;

	if (enable && !d->clearButton) {
		d->clearButton = new QToolButton(this);
		//d->clearButton->setFlat(true);
#if defined(WIDGETS_LIBRARY)
		static bool initres = false;
		if (!initres) {
			Q_INIT_RESOURCE(widgets);
			initres = true;
		}
#endif // defined(WIDGETS_LIBRARY)
		d->clearButton->setStyleSheet("QToolButton { icon-size:18px; image: url(:/CLOVise/PLM/Images/clear_none.svg); }""QToolButton:hover { image: url(:/CLOVise/PLM/Images/clear_over.svg); }""QToolTip{ color: #46C8FF; background-color: #33414D; border: 1px #000000; }");
		//d->clearButton->setIcon(QIcon(":/CLOVise/FlexPLM/Images/clear_over.svg"));
		d->clearButton->setToolTip("Clear");
		d->clearButton->setFocusPolicy(Qt::NoFocus);
		d->clearButton->setFixedSize(18, d->clearButton->sizeHint().height() - 6);
		connect(d->clearButton, SIGNAL(clicked()), this, SLOT(clearButtonClicked()));
		d->clearButton->setVisible(!d->null);
	}
	else if (d->clearButton) {
		disconnect(d->clearButton, SIGNAL(clicked()), this, SLOT(clearButtonClicked()));
		delete d->clearButton;
		d->clearButton = 0;
	}

	update();
}

/*!
  \reimp
*/
QSize CVQDateEdit::sizeHint() const
{
	const QSize sz = QDateEdit::sizeHint();
	if (!d->clearButton)
		return sz;
	return QSize(sz.width() + d->clearButton->width() + 3, sz.height());
}

/*!
  \reimp
*/
QSize CVQDateEdit::minimumSizeHint() const
{
	const QSize sz = QDateEdit::minimumSizeHint();
	if (!d->clearButton)
		return sz;
	return QSize(sz.width() + d->clearButton->width() + 3, sz.height());
}

void CVQDateEdit::showEvent(QShowEvent *event)
{
	QDateEdit::showEvent(event);
	d->setNull(d->null); // force empty string back in
}

/*!
  \reimp
*/
void CVQDateEdit::resizeEvent(QResizeEvent *event)
{
	if (d->clearButton) {
		QStyleOptionSpinBox opt;
		initStyleOption(&opt);
		opt.subControls = QStyle::SC_SpinBoxUp;

		int left = style()->subControlRect(QStyle::CC_SpinBox, &opt, QStyle::SC_SpinBoxUp, this).left() - d->clearButton->width() - 3;
		d->clearButton->move(left, (height() - d->clearButton->height()) / 2);
	}

	QDateEdit::resizeEvent(event);
}

/*!
  \reimp
*/
void CVQDateEdit::paintEvent(QPaintEvent *event)
{
	QDateEdit::paintEvent(event);
}

/*!
  \reimp
*/
void CVQDateEdit::keyPressEvent(QKeyEvent *event)
{
	if (d->nullable &&
		(event->key() >= Qt::Key_0) &&
		(event->key() <= Qt::Key_9) &&
		d->null) {
		setStyleSheet("QDateEdit { background-color: #222224;font-size: 10px; font-face: ArialMT; min-height: 20px; border-bottom: 1px inset #262626; border-left: 1px inset #0D0D0D; border-top: 1px inset #0D0D0D; border-right: 1px inset #262626; padding: 1px; spacing: 1px; color: #FFFFFF; font-size: 10px; }"
			"QDateEdit::drop-down { image: url(:/CLOVise/PLM/Images/ui_dropbox_none.svg); width: 18px; height: 18px; subcontrol-position: right center; subcontrol-origin:margin;}"
			"QDateEdit::drop-down:hover { image: url(:/CLOVise/PLM/Images/ui_dropbox_over.svg); width: 18px; height: 18px; subcontrol-position: right center; subcontrol-origin:margin;}");

		setDateTime(QDateTime::currentDateTime());
		d->setNull(d->null);
	}
	if (event->key() == Qt::Key_Tab && d->nullable && d->null && event->key() == Qt::RightButton) {
		setStyleSheet("QDateEdit { background-color: #222224;font-size: 10px; font-face: ArialMT; min-height: 20px; border-bottom: 1px inset #262626; border-left: 1px inset #0D0D0D; border-top: 1px inset #0D0D0D; border-right: 1px inset #262626; padding: 1px; spacing: 1px; color: #FFFFFF; font-size: 10px; }"
			"QDateEdit::drop-down { image: url(:/CLOVise/PLM/Images/ui_dropbox_none.svg); width: 18px; height: 18px; subcontrol-position: right center; subcontrol-origin:margin;}"
			"QDateEdit::drop-down:hover { image: url(:/CLOVise/PLM/Images/ui_dropbox_over.svg); width: 18px; height: 18px; subcontrol-position: right center; subcontrol-origin:margin;}");

		d->setNull(d->null);
		QAbstractSpinBox::keyPressEvent(event);
		d->setNull(d->null);
		return;
	}
	if (event->key() == Qt::Key_Backspace && d->nullable) {
		setStyleSheet("QDateEdit { background-color: #222224;font-size: 10px; font-face: ArialMT; min-height: 20px; border-bottom: 1px inset #262626; border-left: 1px inset #0D0D0D; border-top: 1px inset #0D0D0D; border-right: 1px inset #262626; padding: 1px; spacing: 1px; color: #FFFFFF; font-size: 10px; }"
			"QDateEdit::drop-down { image: url(:/CLOVise/PLM/Images/ui_dropbox_none.svg); width: 18px; height: 18px; subcontrol-position: right center; subcontrol-origin:margin;}"
			"QDateEdit::drop-down:hover { image: url(:/CLOVise/PLM/Images/ui_dropbox_over.svg); width: 18px; height: 18px; subcontrol-position: right center; subcontrol-origin:margin;}");

		//QLineEdit *edit = qFindChild<QLineEdit *>(this, "qt_spinbox_lineedit");
		QLineEdit *edit = this->findChild<QLineEdit*>("qt_spinbox_lineedit");
		if (edit->selectedText() == edit->text()) {
			setDateTime(QDateTime());
			d->setNull(d->null);
			event->accept();
			return;
		}
	}
	d->setNull(d->null);
	QDateEdit::keyPressEvent(event);
}

/*!
  \reimp
*/
void CVQDateEdit::mousePressEvent(QMouseEvent *event)
{
	bool saveNull = d->null;
	QLineEdit *edit = this->findChild<QLineEdit*>("qt_spinbox_lineedit");
	string editText = edit->text().toStdString();
	setStyleSheet("QDateEdit { background-color: #222224;font-size: 10px; font-face: ArialMT; min-height: 20px; border-bottom: 1px inset #262626; border-left: 1px inset #0D0D0D; border-top: 1px inset #0D0D0D; border-right: 1px inset #262626; padding: 1px; spacing: 1px; color: #FFFFFF; font-size: 10px; }"
		"QDateEdit::drop-down { image: url(:/CLOVise/PLM/Images/ui_dropbox_none.svg); width: 18px; height: 18px; subcontrol-position: right center; subcontrol-origin:margin;}"
		"QDateEdit::drop-down:hover { image: url(:/CLOVise/PLM/Images/ui_dropbox_over.svg); width: 18px; height: 18px; subcontrol-position: right center; subcontrol-origin:margin;}");

	if (event->buttons() == Qt::RightButton)
	{
		setStyleSheet("QDateEdit { background-color: #222224;font-size: 10px; font-face: ArialMT; min-height: 20px; border-bottom: 1px inset #262626; border-left: 1px inset #0D0D0D; border-top: 1px inset #0D0D0D; border-right: 1px inset #262626; padding: 1px; spacing: 1px; color: #FFFFFF; font-size: 10px; }"
			"QDateEdit::drop-down { image: url(:/CLOVise/PLM/Images/ui_dropbox_none.svg); width: 18px; height: 18px; subcontrol-position: right center; subcontrol-origin:margin;}"
			"QDateEdit::drop-down:hover { image: url(:/CLOVise/PLM/Images/ui_dropbox_over.svg); width: 18px; height: 18px; subcontrol-position: right center; subcontrol-origin:margin;}");
	}
	QDateEdit::mousePressEvent(event);

	if (!FormatHelper::HasContent(editText))
	{
		setDateTime(QDateTime::currentDateTime());
	}
	else if (editText == d->specialText)
	{
		setDateTime(QDateTime::currentDateTime());
	}
	else if (d->nullable && saveNull && calendarWidget()->isVisible())
	{
		setDateTime(QDateTime::currentDateTime());
	}
}

bool CVQDateEdit::eventFilter(QObject* object, QEvent* event)
{
	if (object == this || object == lineEdit())
	{
		if (event->type() == QEvent::MouseButtonRelease)
		{
			this->setStyleSheet("QDateEdit { background-color: #222224;font-size: 10px; font-face: ArialMT; min-height: 20px; border-bottom: 1px inset #262626; border-left: 1px inset #0D0D0D; border-top: 1px inset #0D0D0D; border-right: 1px inset #262626; padding: 1px; spacing: 1px; color: #FFFFFF; font-size: 10px; }"
				"QDateEdit::drop-down { image: url(:/CLOVise/PLM/Images/ui_dropbox_none.svg); width: 18px; height: 18px; subcontrol-position: right center; subcontrol-origin:margin;}"
				"QDateEdit::drop-down:hover { image: url(:/CLOVise/PLM/Images/ui_dropbox_over.svg); width: 18px; height: 18px; subcontrol-position: right center; subcontrol-origin:margin;}");
		}
	}
	return QDateEdit::eventFilter(object, event);
}

/*!
  \reimp
*/
bool CVQDateEdit::focusNextPrevChild(bool next)
{
	if (d->nullable && d->null) {
		return QAbstractSpinBox::focusNextPrevChild(next);
	}
	else {
		return QDateEdit::focusNextPrevChild(next);
	}
}

QValidator::State CVQDateEdit::validate(QString &input, int &pos) const
{
	if (d->nullable && d->null) {
		return QValidator::Acceptable;
	}
	return QDateEdit::validate(input, pos);
}

void CVQDateEdit::clearButtonClicked()
{
	d->setNull(true);
}

QString textFromDateTime(const QDateTime & dateTime)
{
	return "";
}
