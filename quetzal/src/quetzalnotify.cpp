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
#include <qutim/notificationslayer.h>
#include <QDesktopServices>
#include <QUrl>
#include <QStringList>
#include <QDebug>

using namespace qutim_sdk_0_3;

void *quetzal_notify_message(PurpleNotifyMsgType type, const char *title,
							 const char *primary, const char *secondary)
{
	QString text = primary;
	if (secondary && *secondary) {
		text += QLatin1Char('\n');
		text += primary;
	}
	Notifications::send(text, QString::fromUtf8(title));
	return NULL;
}

void *quetzal_notify_email(PurpleConnection *gc,
						   const char *subject, const char *from,
						   const char *to, const char *url)
{
	return NULL;
}

void *quetzal_notify_emails(PurpleConnection *gc,
							size_t count, gboolean detailed,
							const char **subjects, const char **froms,
							const char **tos, const char **urls)
{
	return NULL;
}

void *quetzal_notify_formatted(const char *title, const char *primary,
							   const char *secondary, const char *text)
{
	QStringList lines = (QStringList() << primary << secondary << text);
	lines.removeAll(QString());
	Notifications::send(lines.join(QLatin1String("\n")).toUtf8().constData(),
						QString::fromUtf8(title));
	return NULL;
}

void *quetzal_notify_searchresults(PurpleConnection *gc, const char *title,
								   const char *primary, const char *secondary,
								   PurpleNotifySearchResults *results, gpointer user_data)
{
	return NULL;
}

void quetzal_notify_searchresults_new_rows(PurpleConnection *gc,
										   PurpleNotifySearchResults *results,
										   void *data)
{
}

void *quetzal_notify_userinfo(PurpleConnection *gc, const char *who,
							  PurpleNotifyUserInfo *user_info)
{
	return NULL;
}

void *quetzal_notify_uri(const char *uri)
{
	QDesktopServices::openUrl(QUrl::fromUserInput(uri));
	return NULL;
}

void quetzal_close_notify(PurpleNotifyType type, void *ui_handle)
{
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
