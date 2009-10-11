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

	struct NotificationsLayerPrivate;

	namespace Notifications
	{
		//note: title is set on type and sender, customTitle override this
		enum Type
		{
			Online = 0,
			Offline = 1,
			StatusChange = 2,
			Birthday =3,
			Startup = 4,
			MessageGet = 5,
			MessageSend = 6,
			System = 7,
			Typing = 8,
			BlockedMessage = 9,
			Count = 10
		};
		LIBQUTIM_EXPORT void sendNotification(const QString &body,const QString &custom_title = QString());
		LIBQUTIM_EXPORT void sendNotification(Type type, QObject *sender,
											  const QString &body = QString(),
											  const QString &custom_title = QString());
	}

	class LIBQUTIM_EXPORT PopupBackend : public QObject
	{
		Q_OBJECT
	public:
		virtual void show(Notifications::Type type,
						  QObject *sender,
						  const QString &body,
						  const QString &customTitle) = 0;
	protected:
		virtual void virtual_hook(int type, void *data);
	};

	class LIBQUTIM_EXPORT SoundBackend : public QObject
	{
		Q_OBJECT
	public:
		virtual void playSound(const QString &filename) = 0;
	protected:
		virtual void virtual_hook(int type, void *data);
	};

	class LIBQUTIM_EXPORT SoundThemeBackend : public QObject
	{
		//TODO
		Q_OBJECT
	public:
		virtual bool loadTheme(const QString &path) = 0;
	protected:
		virtual void virtual_hook(int type, void *data);
	};

	class LIBQUTIM_EXPORT SoundThemeProvider : public QObject
	{
		Q_OBJECT
	public:
		virtual bool init(const QString &path) = 0;
		virtual void setSound(Notifications::Type sound, const QString &file) = 0;
		virtual QString sound(Notifications::Type sound) = 0;
	protected:
		virtual void virtual_hook(int type, void *data);
	};

}

#endif // NOTIFICATIONSLAYER_H
