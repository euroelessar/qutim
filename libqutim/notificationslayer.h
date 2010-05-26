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
#include <QVariant>
#include <QSharedDataPointer>

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
		LIBQUTIM_EXPORT void sendNotification(const QString &body,const QVariant &data = QVariant());
		LIBQUTIM_EXPORT void sendNotification(Type type, QObject *sender,
											  const QString &body = QString(),
											  const QVariant &data = QVariant());
		LIBQUTIM_EXPORT void sendNotification(const Message &message);
		LIBQUTIM_EXPORT QString toString(Notifications::Type type);
	}

	class LIBQUTIM_EXPORT PopupBackend : public QObject
	{
		Q_OBJECT
		Q_CLASSINFO("Service", "Popup")
	public:
		virtual void show(Notifications::Type type,
						  QObject *sender,
						  const QString &body,
						  const QVariant &data) = 0;
	protected:
		virtual void virtual_hook(int type, void *data);
	};

	class LIBQUTIM_EXPORT SoundBackend : public QObject
	{
		Q_OBJECT
		Q_CLASSINFO("Service", "Sound")
	public:
		SoundBackend();
		virtual ~SoundBackend() {};
		virtual void playSound(const QString &filename) = 0;
		virtual QStringList supportedFormats() = 0;
	protected:
		virtual void virtual_hook(int type, void *data);
	};

	class SoundThemeData;
	class LIBQUTIM_EXPORT SoundTheme
	{
	public:
		SoundTheme(const QString name = QString());
		SoundTheme(const SoundTheme &other);
		SoundTheme(SoundThemeData *data);
		~SoundTheme();
		SoundTheme &operator =(const SoundTheme &other);
		QString path(Notifications::Type type) const;
		void setPath(Notifications::Type type, QString path);
		void play(Notifications::Type type) const;
		bool isNull() const;
		QString themeName() const;
		bool save();
	protected:
		QSharedDataPointer<SoundThemeData> d;
	};

	class SoundThemeProviderPrivate;
	class LIBQUTIM_EXPORT SoundThemeProvider
	{
		Q_DISABLE_COPY(SoundThemeProvider)
 	public:
		SoundThemeProvider();
		virtual ~SoundThemeProvider();
 		virtual bool setSoundPath(Notifications::Type sound, const QString &file);
 		virtual QString soundPath(Notifications::Type sound) = 0;
		virtual QString themeName() = 0;
		virtual bool saveTheme();
 	protected:
 		virtual void virtual_hook(int type, void *data);
		QScopedPointer<SoundThemeProviderPrivate> p;
	};

	class LIBQUTIM_EXPORT SoundThemeBackend : public QObject
	{
		Q_OBJECT
	public:
		virtual QStringList themeList() = 0;
		virtual SoundThemeProvider *loadTheme(const QString &name) = 0;
	};
	
	namespace Sound
	{
		LIBQUTIM_EXPORT SoundTheme theme(const QString &name = QString());
		LIBQUTIM_EXPORT void play(Notifications::Type type);
		LIBQUTIM_EXPORT QString currentThemeName();
		LIBQUTIM_EXPORT QStringList themeList();
		LIBQUTIM_EXPORT void setTheme(const QString &name);
		LIBQUTIM_EXPORT void setTheme(const SoundTheme &theme);
	}
}

#endif // NOTIFICATIONSLAYER_H
