#include "notificationslayer.h"
#include "libqutim_global.h"
#include "objectgenerator.h"

namespace qutim_sdk_0_3 {

	struct NotificationsLayerPrivate {
		QPointer<PopupBackend> popup_backend;
		QPointer<SoundBackend> sound_backend;
		QList<QPointer<SoundThemeBackend> > sound_theme_backends;
		const ObjectGenerator *gen;
	};

	static NotificationsLayerPrivate *p = 0;

	void ensure_notifications_private_helper()
	{
		p = new NotificationsLayerPrivate;
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
		if (p->popup_backend.isNull())
		{
			GeneratorList popup_backends = moduleGenerators<PopupBackend>();
			if(p->gen || p->gen = popup_backends.size() ? popup_backends.first() : 0)
			{
				p->popup_backend = p->gen->generate<PopupBackend>();
			}
		}
		p->popup_backend->show(type,sender,title,body);
	}


	void NotificationsLayer::sendSimpleNotification ( const QString& title, const QString& body )
	{
		sendNotification(NotificationType::System,
						 0,
						 title,
						 body);
	}

	
	
}
