/****************************************************************************
 *  sound.h
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

#ifndef SOUND_H
#define SOUND_H

#include "libqutim_global.h"
#include <QPointer>
#include <QVariant>
#include <QSharedDataPointer>
#include "notification.h"

namespace qutim_sdk_0_3
{
class Message;

class LIBQUTIM_EXPORT SoundBackend : public QObject
{
	Q_OBJECT
	Q_CLASSINFO("Service", "Sound")
public:
	SoundBackend();
	virtual ~SoundBackend() {}
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
	QString path(Notification::Type type) const;
	void setPath(Notification::Type type, QString path);
	void play(Notification::Type type) const;
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
	virtual bool setSoundPath(Notification::Type sound, const QString &file);
	virtual QString soundPath(Notification::Type sound) = 0;
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
LIBQUTIM_EXPORT void play(Notification::Type type);
LIBQUTIM_EXPORT QString currentThemeName();
LIBQUTIM_EXPORT QStringList themeList();
LIBQUTIM_EXPORT void setTheme(const QString &name);
LIBQUTIM_EXPORT void setTheme(const SoundTheme &theme);

}
}

#endif // SOUND_H
