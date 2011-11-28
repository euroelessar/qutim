/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright (C) 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
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
#ifndef QUETZALINPUTDIALOG_H
#define QUETZALINPUTDIALOG_H

#include "quetzalrequestdialog.h"

class QuetzalInputDialog : public QuetzalRequestDialog
{
	Q_OBJECT
public:
	explicit QuetzalInputDialog(const char *title, const char *primary,
								const char *secondary, const char *default_value,
								gboolean multiline, gboolean masked, gchar *hint,
								const char *ok_text, GCallback ok_cb,
								const char *cancel_text, GCallback cancel_cb,
								void *user_data, QWidget *parent = 0);
	virtual void onClicked(int button);
protected:
	virtual void closeRequest();
private:
	PurpleRequestInputCb m_ok_cb;
	PurpleRequestInputCb m_cancel_cb;
};

#endif // QUETZALINPUTDIALOG_H

