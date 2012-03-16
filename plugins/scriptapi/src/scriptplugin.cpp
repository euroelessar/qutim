/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
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

#include "scriptplugin.h"
#include "scriptengine.h"
#include "scriptpluginwrapper.h"
#include <QDebug>
#include <QLatin1Literal>
#include <qutim/thememanager.h>
#include <qutim/chatunit.h>
#include <qutim/chatsession.h>
#include <qutim/notification.h>

ScriptMessageHandler::ScriptMessageHandler(ScriptPlugin *parent)
{
	QObject::connect(&m_engine, SIGNAL(signalHandlerException(QScriptValue)),
	                 parent, SLOT(onException(QScriptValue)));
    m_engine.importExtension(QLatin1String("qt.core"));
	m_engine.importExtension(QLatin1String("qt.gui"));
	m_engine.importExtension(QLatin1String("qutim"));
}

MessageHandler::Result ScriptMessageHandler::doHandle(Message &message, QString *reason)
{
	Q_UNUSED(reason);
	if (message.isIncoming())
		return MessageHandler::Accept;
	QLatin1Literal command("/script");
	QString text = message.text();
	if (text.size() < command.size() + 2
	        || !text.startsWith(QLatin1String(command.data()))
	        || !text.at(command.size()).isSpace()) {
		static QRegExp regexp("\\[\\[(.*)\\]\\]", Qt::CaseInsensitive);
		Q_ASSERT(regexp.isValid());
	    int pos = 0;
		bool first = true;
	    while ((pos = regexp.indexIn(text, pos)) != -1) {
			if (first) {
				first = false;
				openContext(message.chatUnit());
			}
			QString result = m_engine.evaluate(regexp.cap(1)).toString();
			debug() << regexp.cap(1) << result;
			text.replace(pos, regexp.matchedLength(), result);
	        pos += result.length();
	    }
		if (!first) {
			closeContext();
			message.setText(text);
		}
		return MessageHandler::Accept;
	}
	openContext(message.chatUnit());
	m_engine.evaluate(message.text().mid(command.size() + 1));
	closeContext();
	return MessageHandler::Reject;
}

void ScriptMessageHandler::openContext(ChatUnit *unit)
{
	QScriptContext *context = m_engine.pushContext();
	QScriptValue object = context->activationObject();
	if (ChatSession *session = ChatLayer::get(unit, false))
		object.setProperty(QLatin1String("session"), m_engine.newQObject(session));
	object.setProperty(QLatin1String("unit"), m_engine.newQObject(unit));
}

void ScriptMessageHandler::closeContext()
{
	if (m_engine.hasUncaughtException())
		handleException();
	m_engine.popContext();
}

void ScriptMessageHandler::handleException()
{
	QString error;
	error = m_engine.uncaughtException().toString();
	error += QLatin1Char('\n');
	error += m_engine.uncaughtExceptionBacktrace().join("\n");
	debug() << error;
	Notification::send(error);
}

ScriptPlugin::ScriptPlugin()
{
}

void ScriptPlugin::init()
{
	addAuthor(QLatin1String("euroelessar"));
	setInfo(QT_TRANSLATE_NOOP("Plugin", "Script API"),
			QT_TRANSLATE_NOOP("Plugin", "Added ability to create plugins at JavaScript"),
			PLUGIN_VERSION(0, 0, 1, 0), ExtensionIcon("network-wireless"));
	setCapabilities(Loadable);
}

bool ScriptPlugin::load()
{
	debug() << Q_FUNC_INFO << ThemeManager::list("scripts");
	if (m_handler)
		return true;
	m_handler.reset(new ScriptMessageHandler(this));
	MessageHandler::registerHandler(m_handler.data(),
	                                MessageHandler::NormalPriortity,
	                                MessageHandler::HighPriority);
	return true;
}

QList<Plugin*> ScriptPlugin::loadPlugins()
{
	QList<Plugin*> plugins;
	foreach (const QString &name, ThemeManager::list("scripts"))
		plugins << new ScriptPluginWrapper(name);
	return plugins;
}

bool ScriptPlugin::unload()
{
	if (m_handler) {
		m_handler.reset(0);
		return true;
	}
	return false;
}

void ScriptPlugin::onException(const QScriptValue &exception)
{
	Q_UNUSED(exception);
	m_handler->handleException();
}

QUTIM_EXPORT_PLUGIN(ScriptPlugin)

