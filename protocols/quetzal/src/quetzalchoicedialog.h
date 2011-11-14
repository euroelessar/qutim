/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright (C) 2011 Ruslan Nigmatullin euroelessar@yandex.ru
**
*****************************************************************************
**
** $QUTIM_BEGIN_LICENSE$
** This program is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
** See the GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program.  If not, see http://www.gnu.org/licenses/.
** $QUTIM_END_LICENSE$
**
****************************************************************************/
#ifndef QUETZALCHOICEDIALOG_H
#define QUETZALCHOICEDIALOG_H

#include "quetzalrequestdialog.h"
#include <QRadioButton>
#include <purple.h>

class QuetzalChoiceDialog : public QuetzalRequestDialog
{
Q_OBJECT
public:
	explicit QuetzalChoiceDialog(const char *title, const char *primary,
								 const char *secondary, int default_value,
								 const char *ok_text, GCallback ok_cb,
								 const char *cancel_text, GCallback cancel_cb,
								 void *user_data, va_list choices,
								 QWidget *parent = 0);
protected:
	virtual void closeRequest();
protected slots:
	void onOkClicked();
	void onCancelClicked();
private:
	void callBack(PurpleRequestChoiceCb cb);
	PurpleRequestChoiceCb m_ok_cb;
	PurpleRequestChoiceCb m_cancel_cb;
	QList<QRadioButton *> m_radios;
};

#endif // QUETZALCHOICEDIALOG_H

