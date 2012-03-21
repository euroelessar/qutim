/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Aleksey Sidorov <gorthauer87@yandex.ru>
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

#include "quickchatviewcontroller.h"
#include <qutim/message.h>
#include <qutim/adiumchat/chatsessionimpl.h>
#include <qutim/thememanager.h>
#include <QDeclarativeComponent>
#include <QStringBuilder>
#include <QDeclarativeItem>
#include <qutim/debug.h>
#include <qutim/account.h>
#include <QDeclarativeContext>
#include <QDeclarativeEngine>
#include <qutim/conference.h>
#include <qutim/history.h>
#include <qutim/emoticons.h>
#include <qutim/notification.h>
#include <QImageReader>
#include <qutim/servicemanager.h>
#include <QPlainTextEdit>
#include <qutim/adiumchat/abstractchatform.h>
#include <qutim/adiumchat/chatlayerimpl.h>

namespace Core {
namespace AdiumChat {

using namespace qutim_sdk_0_3;

static QString makeUrls (QString html) //TODO temporary
{
	html = Qt::escape(html);
	html.replace("\n", "<br />");
	static QRegExp linkRegExp("([a-zA-Z0-9\\-\\_\\.]+@([a-zA-Z0-9\\-\\_]+\\.)+[a-zA-Z]+)|"
							  "(([a-zA-Z]+://|www\\.)([\\w:/\\?#\\[\\]@!\\$&\\(\\)\\*\\+,;=\\._~-]|&amp;|%[0-9a-fA-F]{2})+)",
							  Qt::CaseInsensitive);
	Q_ASSERT(linkRegExp.isValid());
	int pos = 0;
	while(((pos = linkRegExp.indexIn(html, pos)) != -1))
	{
		QString link = linkRegExp.cap(0);
		QString tmplink = link;
		if (tmplink.toLower().startsWith("www."))
			tmplink.prepend("http://");
		else if(!tmplink.contains("//"))
			tmplink.prepend("mailto:");
		static const QString hrefTemplate( "<a href='%1' target='_blank'>%2</a>" );
		tmplink = hrefTemplate.arg(tmplink, link);
		html.replace(pos, link.length(), tmplink);
		pos += tmplink.count();
	}
	return html;
}

static QVariant messageToVariant(const Message &mes)
{
	//TODO Optimize if posible
	QVariantMap map;
	map.insert(QLatin1String("mid"), mes.id());
	map.insert(QLatin1String("time"), mes.time().isValid() ? mes.time() : QDateTime::currentDateTime());
	QObject *unit = mes.chatUnit();
	if (unit) {
		map.insert(QLatin1String("chatUnit"), qVariantFromValue<QObject*>(unit));
		QObject *account = mes.chatUnit()->account();
		map.insert(QLatin1String("account"), qVariantFromValue<QObject*>(account));
	}
	map.insert(QLatin1String("isIncoming"), mes.isIncoming());
	map.insert(QLatin1String("isDelivered"), mes.isIncoming());

	//define action and service property if it not defined
	map.insert(QLatin1String("action"), mes.property("action",false));
	map.insert(QLatin1String("service"), mes.property("service",false));

	//handle /me
	bool isMe = mes.text().startsWith(QLatin1String("/me "));
	QString body = isMe ? mes.text().mid(4) : mes.text();
	if (isMe)
		map.insert(QLatin1String("action"), true);
	map.insert(QLatin1String("body"), makeUrls(body));

	foreach(const QByteArray &name, mes.dynamicPropertyNames())
		map.insert(QString::fromUtf8(name), mes.property(name));

	//add correct sender name
	QString sender = mes.property("senderName", QString());
	if (sender.isEmpty()) {
		if (mes.isIncoming())
			map.insert(QLatin1String("sender"), mes.chatUnit()->title());
		else {
			Conference *c = qobject_cast<Conference*>(mes.chatUnit());
			if (c && c->me())
				map.insert(QLatin1String("sender"), c->me()->title());
			else
				map.insert(QLatin1String("sender"), mes.chatUnit()->account()->name());
		}
	} else
		map.insert(QLatin1String("sender"), sender);
	//add correct avatar
	if (!mes.property("avatar").isValid()) {
		if (mes.isIncoming()) {
			if (Buddy *b = qobject_cast<Buddy*>(mes.chatUnit()))
				map.insert(QLatin1String("avatar"), b->avatar());
		}
	}

	return map;
}

static QString chatStateToString(ChatState state)
{
	QString stateStr;
	switch (state) {
	case ChatStateActive:
		stateStr = QLatin1String("ChatStateActive");
		break;
	case ChatStateInActive:
		stateStr = QLatin1String("ChatStateInActive");
		break;
	case ChatStateGone:
		stateStr = QLatin1String("ChatStateGone");
		break;
	case ChatStateComposing:
		stateStr = QLatin1String("ChatStateComposing");
		break;
	case ChatStatePaused:
		stateStr = QLatin1String("ChatStatePaused");
		break;
	};
	return stateStr;
}

QuickChatController::QuickChatController(QDeclarativeEngine *engine, QObject *parent) :
	QGraphicsScene(parent),
	m_themeName(QLatin1String("default")),
	//	m_engine(engine) //TODO use one engine for all controllers
	m_engine(engine)
{
	m_context = new QDeclarativeContext(m_engine, this);
	m_context->setContextProperty("controller", this);
}

QuickChatController::~QuickChatController()
{
}

void QuickChatController::appendMessage(const qutim_sdk_0_3::Message& msg)
{
	if (msg.text().isEmpty())
		return;
	emit messageAppended(messageToVariant(msg));
}

void QuickChatController::clearChat()
{
	emit clearChatField();
}

ChatSessionImpl* QuickChatController::getSession() const
{
	return m_session.data();
}

void QuickChatController::loadHistory()
{
	debug() << Q_FUNC_INFO;
	Config config = Config(QLatin1String("appearance")).group(QLatin1String("chat/history"));
	int max_num = config.value(QLatin1String("maxDisplayMessages"), 5);
	MessageList messages = History::instance()->read(m_session.data()->getUnit(), max_num);
	foreach (Message mess, messages) {
		mess.setProperty("silent", true);
		mess.setProperty("store", false);
		mess.setProperty("history", true);
		if (!mess.chatUnit()) //TODO FIXME
			mess.setChatUnit(m_session.data()->getUnit());
		appendMessage(mess);
	}
}

void QuickChatController::setChatSession(ChatSessionImpl* session)
{
	if (m_session.data() == session)
		return;

	if(m_session) {
		m_session.data()->disconnect(this);
		m_session.data()->removeEventFilter(this);
	} else
		m_session = session;
	m_session.data()->installEventFilter(this);
	loadSettings();
	emit sessionChanged(session);

	connect(session->unit(), SIGNAL(chatStateChanged(qutim_sdk_0_3::ChatState,qutim_sdk_0_3::ChatState)),
			this, SLOT(onChatStateChanged(qutim_sdk_0_3::ChatState)));
}

QDeclarativeItem *QuickChatController::rootItem() const
{
	return m_item.data();
}

bool QuickChatController::eventFilter(QObject *obj, QEvent *ev)
{
	if (ev->type() == MessageReceiptEvent::eventType()) {
		MessageReceiptEvent *msgEvent = static_cast<MessageReceiptEvent *>(ev);
		emit messageDelivered(msgEvent->id());
		return true;
	}
	return QGraphicsScene::eventFilter(obj, ev);
}

void QuickChatController::loadSettings()
{
	ConfigGroup cfg = Config("appearance/quickChat").group("style");
	loadTheme(cfg.value<QString>("name","default"));
}

void QuickChatController::loadTheme(const QString &name)
{
	m_themeName = name;
	QString path = ThemeManager::path(QLatin1String("qmlchat"), m_themeName);
	QString main = path % QLatin1Literal("/main.qml");

	QDeclarativeComponent component (m_engine, main);
	QObject *obj = component.create(m_context);

	setRootItem(qobject_cast<QDeclarativeItem*>(obj));
	loadHistory();
}

void QuickChatController::setRootItem(QDeclarativeItem *rootItem)
{
	if (m_item.data() == rootItem)
		return;
	if (m_item) {
		removeItem(m_item.data());
		m_item.data()->deleteLater();
	}
	m_item = rootItem;
	addItem(m_item.data());
	emit rootItemChanged(m_item.data());
}

QString QuickChatController::parseEmoticons(const QString &text) const
{
	//TODO Write flexible textfield with animated emoticons, copy/paste and follow links support
	QString result;
	QList<EmoticonsTheme::Token> tokens = Emoticons::theme().tokenize(text);
	for (QList<EmoticonsTheme::Token>::iterator it = tokens.begin(); it != tokens.end(); it++) {
		switch(it->type) {
		case EmoticonsTheme::Image: {
			QImageReader reader(it->imgPath);
			QSize size = reader.size();
			if (!size.isValid()) {
				size = reader.read().size();
				if (!size.isValid())
					break;
			}
			QString imgHtml = QLatin1Literal("<img src=\"")
					% it->imgPath
					% QLatin1Literal("\" width=\"")
					% QString::number(size.width())
					% QLatin1Literal("\" height=\"")
					% QString::number(size.height())
					% QLatin1Literal("\" alt=\"%4\" title=\"%4\" />");
			result += imgHtml;
			break;
		}
		case EmoticonsTheme::Text:
			result += it->text;
		default:
			break;
		}
	}
	return result;
}

QObject *QuickChatController::unit() const
{
	return m_session ? m_session.data()->unit() : 0;
}

QString QuickChatController::chatState() const
{
	return chatStateToString(m_session ? m_session.data()->unit()->chatState() : ChatStateGone);
}

void QuickChatController::onChatStateChanged(qutim_sdk_0_3::ChatState state)
{
	emit chatStateChanged(chatStateToString(state));
}

void QuickChatController::appendText(const QString &text)
{
	debug() << Q_FUNC_INFO << text << m_session.data();
	ChatLayerImpl::insertText(m_session.data(), text % QLatin1Literal(" "));
}

} // namespace AdiumChat
} // namespace Core

