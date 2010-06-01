#include "antispamplugin.h"
#include "captchasender.h"
#include <qutim/debug.h>
#include <qutim/messagesession.h>

namespace Antispam
{
	
	void AntispamPlugin::init()
	{
		debug() << Q_FUNC_INFO;
		addAuthor(QT_TRANSLATE_NOOP("Author","Aleksey Sidorov"),
				  QT_TRANSLATE_NOOP("Task","Author"),
				  QLatin1String("sauron@citadelspb.com"),
				  QLatin1String("sauron.me"));
	}
	bool AntispamPlugin::load()
	{
		m_captcha_sender = new CaptchaSender(this);
		ChatLayer *layer = ChatLayer::instance();
		connect(layer,
				SIGNAL(sessionCreated(qutim_sdk_0_3::ChatSession*)),
				SLOT(sessionCreated(qutim_sdk_0_3::ChatSession*))
				);
		return true;
	}
	bool AntispamPlugin::unload()
	{
		return true;
	}
		
	void AntispamPlugin::sessionCreated(qutim_sdk_0_3::ChatSession* session)
	{
		connect(session,
				SIGNAL(messageReceived(qutim_sdk_0_3::Message*)),
				m_captcha_sender,
				SLOT(messageReceived(qutim_sdk_0_3::Message*))
				);
	}

}

QUTIM_EXPORT_PLUGIN(Antispam::AntispamPlugin)
