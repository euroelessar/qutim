/****************************************************************************
 *  notificationslayer.h
 *
 *  Copyright (c) 2010 by Sidorov Aleksey <sauron@citadelspb.com>
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
	class Message;
	struct NotificationsLayerPrivate;

	namespace Notifications
	{
		//note: title is set on type and sender, customTitle override this
		enum Type
		{			
			Online = 0x001,
			Offline = 0x002,
			StatusChange = 0x004,
			Birthday = 0x008,
			Startup = 0x010,
			MessageGet = 0x020,
			MessageSend = 0x040,
			System = 0x080,
			Typing = 0x100,
			BlockedMessage = 0x200,
			Count = 0x400
		};
		LIBQUTIM_EXPORT void sendNotification(const QString &body,const QString &custom_title = QString());
		LIBQUTIM_EXPORT void sendNotification(Type type, QObject *sender,
											  const QString &body = QString(),
											  const QString &custom_title = QString());
		LIBQUTIM_EXPORT void sendNotification(const Message &message);
		LIBQUTIM_EXPORT QString toString(Notifications::Type type);
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

	class LIBQUTIM_EXPORT SoundBackend
	{
	public:
		virtual void playSound(const QString &filename) = 0;
		virtual QStringList supportedFormats() = 0;
		virtual ~SoundBackend() {};
	protected:
		virtual void virtual_hook(int type, void *data);
	};

	class SoundThemePrivate;
	class SoundTheme
	{
	public:
		SoundTheme (const QString name = QString());
		QString path(Notifications::Type type);
		void setPath(Notifications::Type  type, QString path);
		void play(Notifications::Type type);
		QString title();
		void load();
		void save();
	};

	class LIBQUTIM_EXPORT SoundThemeProvider : public QObject
	{
		//TODO
// 		Q_OBJECT
// 	public:
// 		virtual bool init(const QString &path) = 0;
// 		virtual void setSound(Notifications::Type sound, const QString &file) = 0;
// 		virtual QString sound(Notifications::Type sound) = 0;
// 	protected:
// 		virtual void virtual_hook(int type, void *data);
	};

}

#endif // NOTIFICATIONSLAYER_H
