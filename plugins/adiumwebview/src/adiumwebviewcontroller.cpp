/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Sidorov Aleksey <sauron@citadelspb.com>
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

#include "adiumwebviewcontroller.h"
#include "../lib/webkitnetworkaccessmanager.h"
#include <qutim/message.h>
#include <qutim/adiumchat/chatsessionimpl.h>
#include <qutim/thememanager.h>
#include <qutim/debug.h>
#include <qutim/account.h>
#include <qutim/conference.h>
#include <qutim/history.h>
#include <qutim/emoticons.h>
#include <qutim/notification.h>
#include <qutim/servicemanager.h>
#include <qutim/adiumchat/abstractchatform.h>
#include <qutim/adiumchat/chatlayerimpl.h>
#include <QPlainTextEdit>
#include <QWebFrame>
#include <QWebInspector>
#include <QWebElement>

namespace Adium {

using namespace qutim_sdk_0_3;
using namespace Core::AdiumChat;

WebViewController::WebViewController(bool isPreview) :
	m_isLoading(false), m_isPreview(isPreview)
{
	m_topic.setProperty("topic", true);
	setNetworkAccessManager(new WebKitNetworkAccessManager(this));
	
	settings()->setAttribute(QWebSettings::DeveloperExtrasEnabled, true);
	QWebInspector *inspector = new QWebInspector;
	inspector->setPage(this);
	connect(this, SIGNAL(destroyed()), inspector, SLOT(deleteLater()));
	connect(this, SIGNAL(loadFinished(bool)), SLOT(onLoadFinished()));
	connect(this, SIGNAL(contentsChanged()), SLOT(onContentsChanged()));
}

WebViewController::~WebViewController()
{
}

void WebViewController::setChatSession(ChatSessionImpl* session)
{
	if (m_session.data() == session)
		return;
	
	m_session = session;
	if (qobject_cast<Conference*>(m_session.data()->unit())) {
		connect(m_session.data()->unit(), SIGNAL(topicChanged(QString,QString)),
		        this, SLOT(onTopicChanged(QString)));
	}
	if (!m_isPreview) {
		loadSettings();
		clearChat();
		loadHistory();
	}
}

ChatSessionImpl *WebViewController::getSession() const
{
	return m_session.data();
}

static bool isContentSimiliar(const Message &a, const Message &b)
{
	if (a.chatUnit() == b.chatUnit()
	        && a.isIncoming() == b.isIncoming()
	        && a.property("senderName", QString()) == b.property("senderName", QString())
	        && a.property("service", false) == b.property("service", false)
	        && a.property("history", false) == b.property("history", false)
	        && a.property("mention", false) == b.property("mention", false)) {
		return qAbs(a.time().secsTo(b.time())) < 300;
	}
	return false;
}

void WebViewController::appendMessage(const qutim_sdk_0_3::Message &msg)
{
	if (msg.property("topic", false)) {
		m_topic = msg;
		if (!m_isLoading)
			updateTopic();
		return;
	}
	bool similiar = isContentSimiliar(m_last, msg);
	QString script = m_style.scriptForAppendingContent(msg, similiar, false, false);
	m_last = msg;
	m_pendingScripts << script;
	if (!m_isLoading) {
		for (int i = 0; i < m_pendingScripts.size(); ++i)
			evaluateJavaScript(m_pendingScripts.at(i));
		m_pendingScripts.clear();
	}
}

void WebViewController::clearChat()
{
	Q_ASSERT(!m_session.isNull());
	m_last = Message();
	m_isLoading = true;
	mainFrame()->setHtml(m_style.baseTemplateForChat(m_session.data()));
}

QString WebViewController::quote()
{
	QString quote = selectedText();
	if (quote.isEmpty())
		quote = m_last.text();
	return quote;
}

WebKitMessageViewStyle *WebViewController::style()
{
	return &m_style;
}

void WebViewController::evaluateJavaScript(const QString &script)
{
	if (!m_session || m_isLoading)
		m_pendingScripts << script;
	else
		mainFrame()->evaluateJavaScript(script);
}

void WebViewController::loadSettings()
{
	Config config("appearance/adiumChat");
	config.beginGroup("style");
	QString styleName = config.value(QLatin1String("name"), QLatin1String("default"));
	m_style.setStylePath(ThemeManager::path(QLatin1String("webkitstyle"), styleName));
	QString variant = config.value(QLatin1String("variant"), m_style.defaultVariant());
	m_style.setActiveVariant(variant);
}

void WebViewController::loadHistory()
{
	Config config = Config(QLatin1String("appearance")).group(QLatin1String("chat/history"));
	int max_num = config.value(QLatin1String("maxDisplayMessages"), 5);
	MessageList messages = History::instance()->read(m_session.data()->unit(), max_num);
	foreach (Message mess, messages) {
		mess.setProperty("silent", true);
		mess.setProperty("store", false);
		mess.setProperty("history", true);
		if (!mess.chatUnit()) //TODO FIXME
			mess.setChatUnit(m_session.data()->unit());
		appendMessage(mess);
	}
}

void WebViewController::onLoadFinished()
{
	foreach (const QString &script, m_pendingScripts) {
		mainFrame()->evaluateJavaScript(script);
	}
	m_isLoading = false;
	m_pendingScripts.clear();
	if (qobject_cast<Conference*>(m_session.data()->unit())) {
		updateTopic();
	}
}

void WebViewController::onTopicChanged(const QString &topic)
{
	if (m_topic.text() == topic)
		return;
	m_topic.setText(topic);
	m_topic.setProperty("html", QString());
	if (!m_isLoading)
		updateTopic();
}

void WebViewController::updateTopic()
{
	QWebElement element = mainFrame()->findFirstElement(QLatin1String("#topic"));
	if (element.isNull())
		return;
	if (m_topic.text().isEmpty()) {
		Conference *conference = qobject_cast<Conference*>(m_session.data()->unit());
		m_topic.setText(conference->topic());
		m_topic.setTime(QDateTime::currentDateTime());
	}
	element.setInnerXml(m_style.templateForContent(m_topic, false));
}

void WebViewController::onContentsChanged()
{
	qDebug() << Q_FUNC_INFO;
}

} // namespace Adium

