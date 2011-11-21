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
#ifndef QUETZALFILEDIALOG_H
#define QUETZALFILEDIALOG_H

#include <QFileDialog>
#include <purple.h>

extern void quetzal_request_close(PurpleRequestType type, QObject *dialog);

class QuetzalFileDialog : public QObject
{
	Q_OBJECT
public:
	explicit QuetzalFileDialog(const char *title, const QString &dirname,
							   GCallback ok_cb, GCallback cancel_cb,
							   void *user_data, QFileDialog *parent);
protected slots:
	void onAccept();
	void onReject();
private:
	PurpleRequestFileCb m_ok_cb;
	PurpleRequestFileCb m_cancel_cb;
	void *m_user_data;
	QFileDialog *m_dialog;
};

#endif // QUETZALFILEDIALOG_H

