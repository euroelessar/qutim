/****************************************************************************
 *
 *  This file is part of qutIM
 *
 *  Copyright (c) 2011 by Nigmatullin Ruslan <euroelessar@gmail.com>
 *
 ***************************************************************************
 *                                                                         *
 *   This file is part of free software; you can redistribute it and/or    *
 *   modify it under the terms of the GNU General Public License as        *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************
 ****************************************************************************/

#include "quetzalnotify.h"
#include <qutim/notification.h>
#include <QDesktopServices>
#include <QUrl>
#include <QStringList>
#include <QDebug>

using namespace qutim_sdk_0_3;

void *quetzal_notify_message(PurpleNotifyMsgType type, const char *title,
							 const char *primary, const char *secondary)
{
	Q_UNUSED(type);
	QString text = primary;
	if (secondary && *secondary) {
		text += QLatin1Char('\n');
		text += primary;
	}
	NotificationRequest request;
	request.setText(text);
	request.setTitle(title);
	request.send();
	return NULL;
}

void *quetzal_notify_email(PurpleConnection *gc,
						   const char *subject, const char *from,
						   const char *to, const char *url)
{
	Q_UNUSED(gc);
	Q_UNUSED(subject);
	Q_UNUSED(from);
	Q_UNUSED(to);
	Q_UNUSED(url);
	return NULL;
}

void *quetzal_notify_emails(PurpleConnection *gc,
							size_t count, gboolean detailed,
							const char **subjects, const char **froms,
							const char **tos, const char **urls)
{
	Q_UNUSED(gc);
	Q_UNUSED(count);
	Q_UNUSED(detailed);
	Q_UNUSED(subjects);
	Q_UNUSED(froms);
	Q_UNUSED(tos);
	Q_UNUSED(urls);
	return NULL;
}

void *quetzal_notify_formatted(const char *title, const char *primary,
							   const char *secondary, const char *text)
{
	QStringList lines = (QStringList() << primary << secondary << text);
	lines.removeAll(QString());
	NotificationRequest request;
	request.setText(lines.join(QLatin1String("\n")));
	request.setTitle(QString::fromUtf8(title));
	request.send();
	return NULL;
}

void *quetzal_notify_searchresults(PurpleConnection *gc, const char *title,
								   const char *primary, const char *secondary,
								   PurpleNotifySearchResults *results, gpointer user_data)
{
	Q_UNUSED(gc);
	Q_UNUSED(title);
	Q_UNUSED(primary);
	Q_UNUSED(secondary);
	Q_UNUSED(results);
	Q_UNUSED(user_data);
	return NULL;
}

void quetzal_notify_searchresults_new_rows(PurpleConnection *gc,
										   PurpleNotifySearchResults *results,
										   void *data)
{
	Q_UNUSED(gc);
	Q_UNUSED(results);
	Q_UNUSED(data);
}

void *quetzal_notify_userinfo(PurpleConnection *gc, const char *who,
							  PurpleNotifyUserInfo *user_info)
{
	Q_UNUSED(gc);
	Q_UNUSED(who);
	Q_UNUSED(user_info);
	return NULL;
}

void *quetzal_notify_uri(const char *uri)
{
	QDesktopServices::openUrl(QUrl::fromUserInput(uri));
	return NULL;
}

void quetzal_close_notify(PurpleNotifyType type, void *ui_handle)
{
	Q_UNUSED(type);
	Q_UNUSED(ui_handle);
}

PurpleNotifyUiOps quetzal_notify_uiops = {
	quetzal_notify_message,
	quetzal_notify_email,
	quetzal_notify_emails,
	quetzal_notify_formatted,
	quetzal_notify_searchresults,
	quetzal_notify_searchresults_new_rows,
	quetzal_notify_userinfo,
	quetzal_notify_uri,
	quetzal_close_notify,
	NULL,
	NULL,
	NULL,
	NULL
};
