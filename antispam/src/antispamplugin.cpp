#include "antispamplugin.h"
#include <qutim/debug.h>
#include <qutim/messagesession.h>
#include "handler.h"
#include <qutim/settingslayer.h>
#include <qutim/icon.h>
#include "settingswidget.h"

namespace Antispam
{

using namespace qutim_sdk_0_3;

inline SettingsItem *item()
{
	static GeneralSettingsItem<SettingsWidget> item(Settings::General,
													Icon("mail-signature-unknown"),
													QT_TRANSLATE_NOOP("Antispam", "Antispam"));
	return &item;
}

void AntispamPlugin::init()
{
	debug() << Q_FUNC_INFO;
	addAuthor(QT_TRANSLATE_NOOP("Author","Sidorov Aleksey"),
			  QT_TRANSLATE_NOOP("Task","Author"),
			  QLatin1String("sauron@citadelspb.com"),
			  QLatin1String("sauron.me"));
	setInfo(QT_TRANSLATE_NOOP("Plugin", "Antispam"),
			QT_TRANSLATE_NOOP("Plugin", "Blocks messages from unknown contacts by question-answer pair"),
			QUTIM_VERSION_CHECK(0, 0, 1, 0));
}
bool AntispamPlugin::load()
{
	if (!m_handler)
		m_handler = new Handler;
	MessageHandler::registerHandler(m_handler.data(), 0, MessageHandler::HighPriority + 100500);
	Settings::registerItem(item());
	item()->connect(SIGNAL(saved()), m_handler, SLOT(loadSettings()));
	return true;
}
bool AntispamPlugin::unload()
{
	if (m_handler) {
		m_handler->deleteLater();
		MessageHandler::unregisterHandler(m_handler.data());
	}
	Settings::removeItem(item());
	return true;
}

}

QUTIM_EXPORT_PLUGIN(Antispam::AntispamPlugin)
