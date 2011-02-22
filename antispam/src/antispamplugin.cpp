#include "antispamplugin.h"
#include "captchasender.h"
#include <qutim/debug.h>
#include <qutim/messagesession.h>

namespace Antispam
{

void AntispamPlugin::init()
{
	debug() << Q_FUNC_INFO;
	m_captcha_sender = 0;
	addAuthor(QT_TRANSLATE_NOOP("Author","Aleksey Sidorov"),
			  QT_TRANSLATE_NOOP("Task","Author"),
			  QLatin1String("sauron@citadelspb.com"),
			  QLatin1String("sauron.me"));
	setInfo(QT_TRANSLATE_NOOP("Plugin", "Antispam"),
			QT_TRANSLATE_NOOP("Plugin", "Blocks messages from unknown contacts by question-answer pair"),
			QUTIM_VERSION_CHECK(0, 0, 1, 0));
}
bool AntispamPlugin::load()
{
	m_captcha_sender = new CaptchaSender(this);
	ChatLayer *layer = ChatLayer::instance();
	connect(layer,
			SIGNAL(sessionCreated(qutim_sdk_0_3::ChatSession*)),
			SLOT(sessionCreated(qutim_sdk_0_3::ChatSession*))
			);
	foreach (qutim_sdk_0_3::ChatSession *session, layer->sessions())
		sessionCreated(session);
	return true;
}
bool AntispamPlugin::unload()
{
	disconnect(ChatLayer::instance(), 0, this, 0);
	delete m_captcha_sender;
	m_captcha_sender = 0;
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
