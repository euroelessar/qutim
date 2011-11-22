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
#include "quetzalinputdialog.h"
#include <QTextEdit>
#include <QPlainTextEdit>
#include <QLineEdit>
#include <QPushButton>

using namespace qutim_sdk_0_3;

QuetzalInputDialog::QuetzalInputDialog(const char *title, const char *primary,
									   const char *secondary, const char *default_value,
									   gboolean multiline, gboolean masked, gchar *hint,
									   const char *ok_text, GCallback ok_cb,
									   const char *cancel_text, GCallback cancel_cb,
									   void *user_data, QWidget *parent)
				: QuetzalRequestDialog(title, primary, secondary, PURPLE_REQUEST_INPUT, user_data, parent)
{
	m_ok_cb = (PurpleRequestInputCb) ok_cb;
	m_cancel_cb = (PurpleRequestInputCb) cancel_cb;
	DataItem item = createItem(title, primary, secondary);
	{
		DataItem data(QLatin1String("data"), LocalizedString(), QString::fromUtf8(default_value));
		data.setProperty("hideTitle", true);
		data.setProperty("multiline", multiline);
		data.setProperty("password", bool(masked));
		data.setProperty("html", !qstrcmp(hint, "html"));
		item.addSubitem(data);
	}
	createItem(item, ok_text, cancel_text);
}

void QuetzalInputDialog::onClicked(int button)
{
	// Ok button was created first
	PurpleRequestInputCb cb = button == 0 ? m_ok_cb : m_cancel_cb;
	const DataItem item = form()->item().subitem(QLatin1String("data"));
	QString value = item.data().toString();
	if (item.property("html").toBool())
		value = Qt::escape(value);
	cb(userData(), value.toUtf8().constData());
	quetzal_request_close(PURPLE_REQUEST_INPUT, this);
}

void QuetzalInputDialog::closeRequest()
{
	onClicked(1);
}

