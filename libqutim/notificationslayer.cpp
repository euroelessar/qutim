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

#include "notificationslayer.h"
#include "libqutim_global.h"
#include "objectgenerator.h"

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

	void SoundThemeBackend::virtual_hook(int type, void *data)
	{
		Q_UNUSED(type);
		Q_UNUSED(data);
	}

	void SoundThemeProvider::virtual_hook(int type, void *data)
	{
		Q_UNUSED(type);
		Q_UNUSED(data);
	}

	namespace Notifications
	{
		struct NotificationsLayerPrivate
		{
			QPointer<PopupBackend> popup_backend;
			QPointer<SoundBackend> sound_backend;
			QList<QPointer<SoundThemeBackend> > sound_theme_backends;
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
		{ if(!p) ensure_notifications_private_helper(); }


		void sendNotification(NotificationType type, QObject *sender,
												  const QString &body, const QString &custom_title)
		{
			ensure_notifications_private();
			//TODO add checks
			if (!isCoreInited())
				return;
			if(!p->inited)
			{
				GeneratorList popup_backends = moduleGenerators<PopupBackend>();
				p->popup_gen = popup_backends.size() ? popup_backends.first() : 0;

				GeneratorList sound_backends = moduleGenerators<SoundBackend>();
				p->sound_gen = sound_backends.size() ? sound_backends.first() : 0;
				p->inited = true;
			}
			if(p->popup_backend.isNull() && p->popup_gen)
				p->popup_backend = p->popup_gen->generate<PopupBackend>();

			if(p->sound_backend.isNull() && p->sound_gen)
				p->sound_backend = p->sound_gen->generate<SoundBackend>();

			if(p->popup_backend)
				p->popup_backend->show(type, sender, body, custom_title);
		}


		void sendNotification(const QString &body, const QString &custom_title)
		{
			sendNotification(NotifySystem, 0, body, custom_title);
		}
	}
}
