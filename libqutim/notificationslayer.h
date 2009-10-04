/****************************************************************************
 *  notificationslayer.h
 *
 *  Copyright (c) 2009 by Sidorov Aleksey <sauron@citadelspb.com>
 *  and Nigmatullin Ruslan <euroelessar@gmail.com>
 *
 ***************************************************************************
 *                                                                         *
 *   This library is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
*****************************************************************************/

#ifndef NOTIFICATIONSLAYER_H
#define NOTIFICATIONSLAYER_H

#include "libqutim_global.h"
#include <QPointer>

namespace qutim_sdk_0_3
{
	enum NotificationType
	{
		NotifyOnline = 0,
		NotifyOffline,
		NotifyStatusChange,
		NotifyBirthday,
		NotifyStartup,
		NotifyMessageGet,
		NotifyMessageSend,
		NotifySystem,
		NotifyTyping,
		NotifyBlockedMessage,
		NotifyCustom,
		NotifyCount
	};

	struct NotificationsLayerPrivate;
	class LIBQUTIM_EXPORT PopupBackend : public QObject
	{
		Q_OBJECT
	public:
		virtual void show(NotificationType type,
						  QObject *sender,
						  const QString &body,
						  const QString &customTitle
						  ) = 0;
	};

	class LIBQUTIM_EXPORT SoundBackend : public QObject
	{
		Q_OBJECT
	public:
		virtual void playSound (const QString &filename) = 0;
	protected:
		virtual void virtual_hook(int type, void *data);
	};

	class LIBQUTIM_EXPORT SoundThemeBackend : public QObject
	{
		//TODO
		Q_OBJECT
	public:
		virtual bool loadTheme(const QString &path) = 0;
	};

	class LIBQUTIM_EXPORT SoundThemeProvider : public QObject
	{
		Q_OBJECT
	public:
		virtual bool init(const QString &path) = 0;
		virtual void setSound(NotificationType sound, const QString &file) = 0;
		virtual QString sound(NotificationType sound) = 0;
	};

	class LIBQUTIM_EXPORT NotificationsLayer : public QObject
	{
		Q_OBJECT
	public:
		//note: title is set on type and sender, customTitle override this
		void sendNotification(const QString &body,const QString &customTitle = QString());
		void sendNotification(NotificationType type,
									QObject *sender,
									const QString &body = QString(),
									const QString &customTitle = QString()
									);
	protected:
		NotificationsLayer();
		~NotificationsLayer();
	private:
		NotificationsLayerPrivate *p;
	};

}

#endif // NOTIFICATIONSLAYER_H
