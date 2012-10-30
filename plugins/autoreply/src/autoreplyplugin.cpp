#include "autoreplyplugin.h"
#include <qutim/config.h>
#include <qutim/contact.h>
#include "autoreplybuttongenerator.h"
#include "autoreplydialog.h"
#include "autoreplysettingswidget.h"
#include <qutim/contact.h>
#include <QApplication>

using namespace qutim_sdk_0_3;

AutoReplyPlugin::AutoReplyPlugin()
	: m_idleManager("Idle"), m_active(false), m_deltaTime(0), m_idleTimeOut(0)
{
}

void AutoReplyPlugin::init()
{
	setInfo(QT_TRANSLATE_NOOP("Plugin", "Autoreply"),
			QT_TRANSLATE_NOOP("Plugin", "Plugin allows autoreplying messages during idle state"),
			PLUGIN_VERSION(0, 0, 1, 0));
	setCapabilities(Loadable);
	addAuthor(QLatin1String("euroelessar"));
}

bool AutoReplyPlugin::load()
{
	if (!m_handler) {
		connect(ServiceManager::instance(), SIGNAL(serviceChanged(QByteArray,QObject*,QObject*)),
				SLOT(onServiceChanged(QByteArray)));
		Config config("autoreply");
		m_handler.reset(new AutoReplyMessageHandler(this));
		MessageHandler::registerHandler(m_handler.data());
		m_settingsItem.reset(new GeneralSettingsItem<AutoReplySettingsWidget>(
								 Settings::Plugin, QIcon(),
								 QT_TRANSLATE_NOOP("AutoReply", "Auto reply")));
		m_settingsItem->connect(SIGNAL(saved()), this, SLOT(onSettingsChanged()));
		Settings::registerItem(m_settingsItem.data());
		m_active = false;
		onSettingsChanged();
	}
	return true;
}

void AutoReplyPlugin::onSettingsChanged()
{
	Q_ASSERT(m_handler);
	Config config("autoreply");
	ServicePointer<QObject> form("ChatForm");
	m_deltaTime = config.value("deltaTime", 15 * 60);
	if (config.value("automatic", true)) {
		if (form && m_button) {
			QMetaObject::invokeMethod(form, "removeAction",
									  Q_ARG(qutim_sdk_0_3::ActionGenerator*, m_button.data()));
		}
		m_active = false;
		m_button.reset(0);
		m_idleTimeOut = config.value("timeOut", 15 * 60);
		onServiceChanged("Idle");

		const QString message = config.value("message", QString());
		if (!message.isEmpty())
			setReplyText(message);
	} else if (!m_button) {
		if (m_idleManager)
			QObject::disconnect(m_idleManager.data(), 0, this, 0);
		m_button.reset(new AutoReplyButtonGenerator(this));
		if (form) {
			QMetaObject::invokeMethod(form, "addAction",
									  Q_ARG(qutim_sdk_0_3::ActionGenerator*, m_button.data()));
		}
	}
}

bool AutoReplyPlugin::unload()
{
	if (m_handler) {
		ServicePointer<QObject> form("ChatForm");
		if (form && m_button) {
			QMetaObject::invokeMethod(form, "removeAction",
									  Q_ARG(qutim_sdk_0_3::ActionGenerator*, m_button.data()));
		}
		Settings::removeItem(m_settingsItem.data());
		m_settingsItem.reset();
		m_handler.reset(0);
		setReplyText(QString());
		if (m_idleManager)
			QObject::disconnect(m_idleManager.data(), 0, this, 0);
		m_button.reset(0);
	}
	return true;
}

bool AutoReplyPlugin::isActive() const
{
	return m_active;
}

void AutoReplyPlugin::setActive(bool active)
{
	if (m_active == active)
		return;
	m_active = active;
	emit activeChanged(m_active);
	if (!m_active)
		setBackTime(QDateTime());
}

QString AutoReplyPlugin::replyText() const
{
	return m_replyText;
}

QDateTime AutoReplyPlugin::backTime() const
{
	return m_backTime;
}

int AutoReplyPlugin::deltaTime() const
{
	return m_deltaTime;
}

void AutoReplyPlugin::setReplyText(const QString &replyText)
{
	if (m_replyText == replyText)
		return;
	m_replyText = replyText;
	emit replyTextChanged(m_replyText);
}

void AutoReplyPlugin::setBackTime(const QDateTime &backTime)
{
	if (m_backTime == backTime)
		return;
	m_backTime = backTime;
	emit backTimeChanged(m_backTime);
}

void AutoReplyPlugin::onSecondsIdle(int idle)
{
	if (m_button)
		return;
	bool active = (idle > m_idleTimeOut);
	if (m_active != active)
		setActive(!m_active);
}

void AutoReplyPlugin::onServiceChanged(const QByteArray &name)
{
	if (name != "Idle")
		return;
	if (m_idleManager)
		connect(m_idleManager.data(), SIGNAL(secondsIdle(int)), SLOT(onSecondsIdle(int)));
}

void AutoReplyPlugin::onActionToggled(bool active)
{
	if (m_active == active)
		return;
	if (active) {
		sender()->setProperty("checked", m_active);
		AutoReplyDialog *dialog = new AutoReplyDialog(this, qApp->activeWindow());
		dialog->setAttribute(Qt::WA_DeleteOnClose);
		dialog->open();
	} else {
		m_active = active;
		emit activeChanged(m_active);
	}
}

QUTIM_EXPORT_PLUGIN(AutoReplyPlugin)
