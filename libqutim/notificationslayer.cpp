#include "notificationslayer.h"
#include "libqutim_global.h"
#include "objectgenerator.h"

namespace qutim_sdk_0_3 {

	void SoundBackend::virtual_hook(int type, void *data)
	{
		Q_UNUSED(type);
		Q_UNUSED(data);
	}

	struct NotificationsLayerPrivate {
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
	{ if(!p) ensure_notifications_private_helper();}

	NotificationsLayer::NotificationsLayer()
	{

	}

	NotificationsLayer::~NotificationsLayer()
	{

	}

	
	void NotificationsLayer::sendNotification ( NotificationType type, QObject* sender, const QString& title, const QString& body )
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
			p->popup_backend->show(type,sender,title,body);
	}


	void NotificationsLayer::sendSimpleNotification ( const QString& title, const QString& body )
	{
		sendNotification(System,
						 0,
						 title,
						 body);
	}

	
	
}
