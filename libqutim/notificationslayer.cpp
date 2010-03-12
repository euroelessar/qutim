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
#include "contact.h"
#include "message.h"
#include "configbase.h"

namespace qutim_sdk_0_3
{
	void PopupBackend::virtual_hook(int type, void *data)
	{
		Q_UNUSED(type);
		Q_UNUSED(data);
	}

	void SoundBackend::virtual_hook(int type, void *data)
	{
		Q_UNUSED(type);
		Q_UNUSED(data);
	}

	struct NotificationsLayerPrivate
	{
		QPointer<PopupBackend> popup_backend;
		QPointer<SoundBackend> sound_backend;
		const ObjectGenerator *popup_gen;
		const ObjectGenerator *sound_gen;
		bool inited;
	};

	static NotificationsLayerPrivate *p = 0;

	void ensure_notifications_private_helper()
	{
		p = new NotificationsLayerPrivate;
		p->inited = false;
		p->popup_gen = 0;
		p->sound_gen = 0;
	}

	inline void ensure_notifications_private()
	{
		if (!p) ensure_notifications_private_helper();
	}

	namespace Notifications
	{
		void sendNotification(Type type, QObject *sender,
							const QString &body, const QVariant &data)
		{
			ensure_notifications_private();
			//TODO add checks
			if (!isCoreInited())
				return;
			if (!p->inited)
			{
				GeneratorList popup_backends = moduleGenerators<PopupBackend>();
				p->popup_gen = popup_backends.size() ? popup_backends.first() : 0;

				GeneratorList sound_backends = moduleGenerators<SoundBackend>();
				p->sound_gen = sound_backends.size() ? sound_backends.first() : 0;
				p->inited = true;
			}
			if (p->popup_backend.isNull() && p->popup_gen)
				p->popup_backend = p->popup_gen->generate<PopupBackend>();

			if (p->popup_backend)
				p->popup_backend->show(type, sender, body, data);
		}


		void sendNotification(const QString &body, const QVariant &data)
		{
			sendNotification(System, 0, body, data);
		}


		void sendNotification(const Message& message)
		{
			Type type = static_cast<Type>(message.property("service").toInt());
			if (!type)
				type = message.isIncoming() ? MessageGet : MessageSend;
			sendNotification(type, const_cast<ChatUnit *>(message.chatUnit()),message.text(),QVariant::fromValue(message));
		}

		QString toString(Notifications::Type type)
		{
			QString title;
			switch ( type )
			{
			case Notifications::System:
				title = QObject::tr("System message from %1:");
				break;
			case Notifications::StatusChange:
				title = QObject::tr("%1 changed status");
				break;
			case Notifications::MessageGet:
				title = QObject::tr("Message from %1:");
				break;
			case Notifications::MessageSend:
				title = QObject::tr("Message to %1:");
				break;
			case Notifications::Typing:
				title = QObject::tr("%1 is typing");
				break;
			case Notifications::BlockedMessage:
				title = QObject::tr("Blocked message from %1");
				break;
			case Notifications::Birthday:
				title = QObject::tr("%1 has birthday today!!");
				break;
			case Notifications::Online:
				title = QObject::tr("%1 is online");
				break;
			case Notifications::Offline:
				title = QObject::tr("%1 is offline");
				break;
			case Notifications::Startup:
				title = QObject::tr("qutIM launched");
				break;
			case Notifications::Count:
				title = QObject::tr("Count");
				break;
			default:
				return title;
			}
			return title;
		}

	}

	class SoundThemePrivate
	{
// 	public:
// 		QString themeName;
// 		QHash<Notifications::Type , QString> themeHash;
// 		static SoundThemePrivate *self()
// 		{
// 			if ( !instance )
// 				instance = new SoundThemePrivate();
// 			return instance;
// 		};
// 	private:
// 		static SoundThemePrivate *instance;
	};

	SoundTheme::SoundTheme(const QString name)
	{
		Q_UNUSED(name);
// 		QString themeName = name;
// 		if (themeName .isEmpty()) {
// 			themeName  = Config().group("sounds").value<QString>("theme", QString());
// 		}
// 		if (SoundThemePrivate::self()->themeName != themeName) {
// 			SoundThemePrivate::self()->themeName = themeName;
// 			load();
// 		}
	}

	void SoundTheme::load()
	{
		//TODO
	}

	QString SoundTheme::path(Notifications::Type type)
	{
		Q_UNUSED(type);
		return QString();
	}

	void SoundTheme::play(Notifications::Type type)
	{
		Q_UNUSED(type);
// 		ensure_notifications_private();
// 		if (!isCoreInited())
// 			return;
// 		if (!p->inited)
// 		{
// 			GeneratorList popup_backends = moduleGenerators<PopupBackend>();
// 			p->popup_gen = popup_backends.size() ? popup_backends.first() : 0;
// 
// 			GeneratorList sound_backends = moduleGenerators<SoundBackend>();
// 			p->sound_gen = sound_backends.size() ? sound_backends.first() : 0;
// 			p->inited = true;
// 		}
// 		if (p->sound_backend.isNull() && p->sound_gen)
// 			p->sound_backend = p->sound_gen->generate<SoundBackend>();
// 
// 		if (p->sound_backend)
// 			p->sound_backend->playSound(SoundThemePrivate::self()->themeHash.value(type));
	}

	void SoundTheme::save()
	{

	}

	void SoundTheme::setPath(Notifications::Type type, QString path)
	{
		Q_UNUSED(type);
		Q_UNUSED(path);
	}

	QString SoundTheme::title()
	{
		return QString();
	}

}

