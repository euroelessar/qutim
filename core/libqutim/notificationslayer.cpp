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

#include "notificationslayer.h"
#include "libqutim_global.h"
#include "objectgenerator.h"
#include "servicemanager.h"
#include "contact.h"
#include "message.h"
#include "configbase.h"
#include <QFileInfo>
#include "notificationslayer_p.h"

namespace qutim_sdk_0_3
{

SoundBackend::SoundBackend()
{
	Q_UNUSED(QT_TRANSLATE_NOOP("Service","Popup"));
	Q_UNUSED(QT_TRANSLATE_NOOP("Service","Sound"));
}

void SoundBackend::virtual_hook(int type, void *data)
{
	Q_UNUSED(type);
	Q_UNUSED(data);
}

struct NotificationsLayerPrivate
{
	ServicePointer<SoundBackend> soundBackend;
	QList<SoundThemeBackend*> soundThemeBackends;
	QHash<QString, SoundThemeData*> soundThemeCache;
	bool soundIsInited;
	void initSound()
	{
		if (!ObjectGenerator::isInited())
			return;
		GeneratorList exts = ObjectGenerator::module<SoundThemeBackend>();
		foreach (const ObjectGenerator *gen, exts)
			soundThemeBackends << gen->generate<SoundThemeBackend>();
		soundIsInited = true;
	}

	inline void ensureSound() { if (!soundIsInited) initSound(); }
};

static NotificationsLayerPrivate *p = 0;

void ensure_notifications_private_helper()
{
	p = new NotificationsLayerPrivate;
	p->soundIsInited = false;
}

inline void ensure_notifications_private()
{
	if (!p)
		ensure_notifications_private_helper();
}

namespace Notifications
{

void send(Notification::Type type, QObject *sender,
		  const QString &body, const QVariant &data)
{
	ensure_notifications_private();
	//new backends
	Notification *notification = 0;
	if (data.canConvert<Message>()) {
		notification = Notification::send(data.value<Message>());
	} else {
		NotificationRequest request(type);
		request.setObject(sender);
		request.setText(body);
		notification = request.send();
	}	
}


void send(const QString &body, const QVariant &data)
{
	Notification::send(body);
	Q_UNUSED(data);
}


void send(const Message &message)
{
	Notification::send(message);
}

QString toString(Notification::Type type)
{
	QString title;
	switch(type) {
	case Notification::IncomingMessage:
	case Notification::ChatIncomingMessage:
		title = QObject::tr("Message from %1:");
		break;
	case Notification::OutgoingMessage:
	case Notification::ChatOutgoingMessage:
		title = QObject::tr("Message to %1:");
		break;
	case Notification::AppStartup:
		title = QObject::tr("qutIM launched");
		break;
	case Notification::BlockedMessage:
		title = QObject::tr("Blocked message from %1");
		break;
	case Notification::ChatUserJoined:
	case Notification::UserOnline:
		title = QObject::tr("%1 is online");
		break;
	case Notification::ChatUserLeaved:
	case Notification::UserOffline:
		title = QObject::tr("%1 is offline");
		break;
	case Notification::UserChangedStatus:
		title = QObject::tr("%1 changed status");
		break;
	case Notification::UserHasBirthday:
		title = QObject::tr("%1 has birthday today!!");
		break;
	case Notification::UserTyping:
		title = QObject::tr("%1 is typing");
		break;
	case Notification::FileTransferCompleted:
	case Notification::System:
	default:
		title = QObject::tr("System notify");
	}
	return title;
}

}

class SoundThemeData : public QSharedData
{
public:
	SoundThemeData() : provider(0) {}
	SoundThemeData(const SoundThemeData &o) : QSharedData(o), provider(o.provider) {}
	~SoundThemeData() { delete provider; }
	SoundThemeProvider *provider;
};

SoundTheme::SoundTheme(const QString name) : d(Sound::theme(name).d)
{
}

SoundTheme::SoundTheme(SoundThemeData *data) : d(data)
{
}

SoundTheme::SoundTheme(const SoundTheme &other) : d(other.d)
{
}

SoundTheme::~SoundTheme()
{
}

SoundTheme &SoundTheme::operator =(const SoundTheme &other)
{
	d = other.d;
	return *this;
}

QString SoundTheme::path(Notification::Type type) const
{
	return isNull() ? QString() : d->provider->soundPath(type);
}

void SoundTheme::play(Notification::Type type) const
{
	QString filePath = path(type);
	if (filePath.isEmpty())
		return;
	QFileInfo info(filePath);
	if (p->soundBackend && p->soundBackend->supportedFormats().contains(info.suffix()))
		p->soundBackend->playSound(filePath);
}

bool SoundTheme::isNull() const
{
	return !d || !d->provider;
}

bool SoundTheme::save()
{
	return !isNull() && d->provider->saveTheme();
}

void SoundTheme::setPath(Notification::Type type, QString path)
{
	Q_UNUSED(type);
	Q_UNUSED(path);
}

QString SoundTheme::themeName() const
{
	return isNull() ? QString() : d->provider->themeName();
}

class SoundThemeProviderPrivate
{
public:
};

SoundThemeProvider::SoundThemeProvider()
{
}

SoundThemeProvider::~SoundThemeProvider()
{
}

bool SoundThemeProvider::setSoundPath(Notification::Type sound, const QString &file)
{
	Q_UNUSED(sound);
	Q_UNUSED(file);
	return false;
}

bool SoundThemeProvider::saveTheme()
{
	return false;
}

void SoundThemeProvider::virtual_hook(int type, void *data)
{
	Q_UNUSED(type);
	Q_UNUSED(data);
}

namespace Sound
{
SoundTheme theme(const QString &name)
{
	if (name.isEmpty()) {
		QString currentName = currentThemeName();
		if (currentName.isEmpty())
			return SoundTheme(0);
		else
			return theme(currentName);
	} else {
		p->ensureSound();
	}

	// Firstly look at cache
	if (SoundThemeData *data = p->soundThemeCache.value(name))
		return SoundTheme(data);

	// Then try a chance in different backends
	foreach (SoundThemeBackend *backend, p->soundThemeBackends) {
		if (backend->themeList().contains(name)) {
			SoundThemeData *data = new SoundThemeData;
			data->provider = backend->loadTheme(name);
			Q_ASSERT(data->provider);
			Q_ASSERT(data->provider->themeName() == name);
			data->ref.ref();
			p->soundThemeCache.insert(name, data);
			return SoundTheme(data);
		}
	}

	// So.. there is no such theme... create null one
	return SoundTheme(0);
}

void play(Notification::Type type)
{
	theme().play(type);
}

QString currentThemeName()
{
	//TODO rewrite!
	ensure_notifications_private();
	p->ensureSound();
	ConfigGroup config = Config("appearance").group("sound");
	QString name = config.value<QString>("theme", QString());
	if (name.isEmpty()) {
		QStringList themes = themeList();
		if (themes.isEmpty() || themes.contains(QLatin1String("default")))
			name = QLatin1String("default");
		else
			name = themes.first();
		config.setValue("theme", name);
		config.sync();
	}
	return name;
}

QStringList themeList()
{
	ensure_notifications_private();
	p->ensureSound();
	QSet<QString> themes;
	themes << QT_TRANSLATE_NOOP("Sound", "No sound");
	foreach (SoundThemeBackend *backend, p->soundThemeBackends) {
		foreach (const QString &theme, backend->themeList())
			themes << theme;
	}
	return themes.toList();
}

void setTheme(const QString &name)
{
	ConfigGroup group = Config("appearance").group("sound");
	group.setValue("theme", name);
	group.sync();
}

void setTheme(const SoundTheme &theme)
{
	setTheme(theme.themeName());
}
}

SoundHandler::SoundHandler() :
	NotificationBackend("Sound")
{

}

void SoundHandler::handleNotification(Notification *notification)
{
	ref(notification);
	Sound::play(notification->request().type());
	deref(notification);
}

}

