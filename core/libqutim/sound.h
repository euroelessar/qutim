/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Aleksey Sidorov <gorthauer87@yandex.ru>
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

class LIBQUTIM_EXPORT Sound : public QObject
{
	Q_OBJECT
public:
	virtual ~Sound();
	static Sound *instance();
	static SoundTheme theme(const QString &name = QString());
	static void play(Notification::Type type);
	static QString currentThemeName();
	static QStringList themeList();
	static void setTheme(const QString &name);
	static void setTheme(const SoundTheme &theme);
signals:
	void currentThemeChanged(const QString &themeName);
private:
	Sound();
};

}

#endif // SOUND_H

