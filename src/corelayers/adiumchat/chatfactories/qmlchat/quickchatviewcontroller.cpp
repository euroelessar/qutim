/****************************************************************************
 *  quickchatviewcontroller.cpp
 *
 *  Copyright (c) 2011 by Sidorov Aleksey <sauron@citadelspb.com>
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

#include "quickchatviewcontroller.h"
#include <qutim/message.h>
#include <chatlayer/chatsessionimpl.h>
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
#include <qutim/notificationslayer.h>
#include <QImageReader>

namespace Core {
namespace AdiumChat {

using namespace qutim_sdk_0_3;

QString makeUrls (QString html) //TODO temporary
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

QVariant messageToVariant(const Message &mes)
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
	if (body.isEmpty()) {
		//handle service
		//FIXME remake with notifications api
		Notifications::Type type = static_cast<Notifications::Type>(mes.property("service", 0));
		body = Notifications::toString(type).arg(map.value("sender").toString());
	} else if (isMe)
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

QuickChatViewController::QuickChatViewController(QDeclarativeEngine *engine) :
	m_session(0),
	m_themeName(QLatin1String("default")),
	//	m_engine(engine) //TODO use one engine for all controllers
	m_engine(engine),
	m_storeServiceMessages(true)
{
	m_context = new QDeclarativeContext(m_engine, this);
	m_context->setContextProperty("controller", this);
	Config cfg = Config(QLatin1String("appearance")).group(QLatin1String("chat"));
	cfg.beginGroup(QLatin1String("history"));
	m_storeServiceMessages = cfg.value(QLatin1String("storeServiceMessages"), true);
	cfg.endGroup();

}


void QuickChatViewController::appendMessage(const qutim_sdk_0_3::Message& msg)
{
	bool isService = msg.property("service", false);
	if (msg.property("store", true) && (!isService || (isService && m_storeServiceMessages)))
		History::instance()->store(msg);
	emit messageAppended(messageToVariant(msg));
}

void QuickChatViewController::clearChat()
{
	emit clearChatField();
}

ChatSessionImpl* QuickChatViewController::getSession() const
{
	return m_session;
}

void QuickChatViewController::loadHistory()
{
	debug() << Q_FUNC_INFO;
	Config config = Config(QLatin1String("appearance")).group(QLatin1String("chat/history"));
	int max_num = config.value(QLatin1String("maxDisplayMessages"), 5);
	MessageList messages = History::instance()->read(m_session->getUnit(), max_num);
	foreach (Message mess, messages) {
		mess.setProperty("silent", true);
		mess.setProperty("store", false);
		mess.setProperty("history", true);
		if (!mess.chatUnit()) //TODO FIXME
			mess.setChatUnit(m_session->getUnit());
		appendMessage(mess);
	}
}

void QuickChatViewController::setChatSession(ChatSessionImpl* session)
{
	if (m_session == session)
		return;
	
	if(m_session) {
		m_session->disconnect(this);
		m_session->removeEventFilter(this);
	} else
		m_session = session;
	m_session->installEventFilter(this);
	loadSettings();
	emit sessionChanged(session);
}

QuickChatViewController::~QuickChatViewController()
{

}

QDeclarativeItem *QuickChatViewController::rootItem() const
{
	return m_item;
}

bool QuickChatViewController::eventFilter(QObject *obj, QEvent *ev)
{
	if (ev->type() == MessageReceiptEvent::eventType()) {
		MessageReceiptEvent *msgEvent = static_cast<MessageReceiptEvent *>(ev);
		emit messageDelivered(msgEvent->id());
		return true;
	}
	return QGraphicsScene::eventFilter(obj, ev);
}

void QuickChatViewController::loadSettings()
{
	ConfigGroup cfg = Config("appearance/quickChat").group("style");
	m_themeName = cfg.value<QString>("name","default");

	QString path = ThemeManager::path(QLatin1String("qmlchat"), m_themeName);
	QString main = path % QLatin1Literal("/main.qml");

	QDeclarativeComponent component (m_engine, main);
	QObject *obj = component.create(m_context);

	m_item = qobject_cast<QDeclarativeItem*>(obj);
	addItem(m_item);
	loadHistory();
	emit rootItemChanged();
}

void QuickChatViewController::setRootItem(QDeclarativeItem *rootItem)
{
	if (m_item == rootItem)
		return;
	if (m_item) {
		removeItem(m_item);
		m_item->deleteLater();
	}
	m_item = rootItem;
	addItem(m_item);
}

QString QuickChatViewController::parseEmoticons(const QString &text) const
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

} // namespace AdiumChat
} // namespace Core

