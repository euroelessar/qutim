/****************************************************************************
 *  chatstyleoutput.cpp
 *
 *  Copyright (c) 2010 by Sidorov Aleksey <sauron@citadelspb.com>
 *                        Ruslan Nigmatullin <euroelessar@gmail.com>
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

#include <QTextDocument>
#include <QStringList>
#include "chatstyle.h"
#include "chatstyleoutput.h"
#include <chatlayer/chatsessionimpl.h>
#include <QDateTime>
#include <QWebFrame>
#include <QWebPage>
#include <QWebElement>
#include <QFileInfo>
#include <QStringBuilder>
#include <QDesktopServices>
#include <chatlayer/messagemodifier.h>
#include <chatlayer/javascriptclient.h>
#include <qutim/configbase.h>
#include <qutim/account.h>
#include <qutim/protocol.h>
#include <qutim/emoticons.h>
#include <qutim/objectgenerator.h>
#include <qutim/debug.h>
#include <qutim/conference.h>
#include <qutim/thememanager.h>
#include <qutim/servicemanager.h>
#include <qutim/history.h>

namespace Core
{
namespace AdiumChat
{
struct MessageModifierTrack
{
	inline MessageModifierTrack(const QRegExp &r, const QString &n, MessageModifier *m)
		: regexp(r), name(n), modifier(m) {}
	QRegExp regexp;
	QString name;
	MessageModifier *modifier;
};

void ChatStyleOutput::setChatSession(ChatSessionImpl *session)
{
	if(m_session) {
		m_session->disconnect(this);
		m_session->removeEventFilter(this);
	}
	m_session = session;
	setParent(session);
	loadSettings();
	setChatUnit(session->unit());

	connect(m_session,SIGNAL(activated(bool)),SLOT(onSessionActivated(bool)));
	connect(m_session,SIGNAL(chatUnitChanged(qutim_sdk_0_3::ChatUnit*)),
			this,SLOT(setChatUnit(qutim_sdk_0_3::ChatUnit*)));
	JavaScriptClient *client = new JavaScriptClient(session);
	mainFrame()->addToJavaScriptWindowObject(client->objectName(), client);
	connect(mainFrame(), SIGNAL(javaScriptWindowObjectCleared()),
			client, SLOT(helperCleared()));
	session->installEventFilter(this);
}

ChatSessionImpl *ChatStyleOutput::getSession() const
{
	return m_session;
}

QVariant ChatStyleOutput::evaluateJavaScript(const QString &scriptSource)
{
	return currentFrame()->evaluateJavaScript(scriptSource);
}

void ChatStyleOutput::onSessionActivated(bool active)
{
	if (!active)
		separator = false;
}

void ChatStyleOutput::onLinkClicked(const QUrl &url)
{
	QDesktopServices::openUrl(url);
}

void ChatStyleOutput::setChatUnit(ChatUnit *unit)
{
	if(!m_session)
		return;
	preparePage(m_session);
	bool isConference = !!qobject_cast<Conference*>(unit);
	QWebFrame *frame = mainFrame();
	QWebElement chatElem = frame->findFirstElement("#Chat");
	if (!chatElem.isNull()) {
		if (isConference != chatElem.hasClass("groupchat")) {
			if (isConference)
				chatElem.addClass("groupchat");
			else
				chatElem.removeClass("groupchat");
		}
	}
}

bool ChatStyleOutput::eventFilter(QObject *, QEvent *ev)
{
	if (ev->type() == MessageReceiptEvent::eventType()) {
		MessageReceiptEvent *msgEvent = static_cast<MessageReceiptEvent *>(ev);
		QWebFrame *frame = mainFrame();
		QWebElement elem = frame->findFirstElement(QLatin1Literal("#message")
												   % QString::number(msgEvent->id()));
		if (!elem.isNull()) {
			if (msgEvent->success()) {
				elem.removeClass(QLatin1String("notDelivered"));
				elem.addClass(QLatin1String("delivered"));
			} else {
				elem.addClass(QLatin1String("failedToDevliver"));
			}
		}
		return true;
	}

	return ChatStyleOutput::event(ev);
}

void ChatStyleOutput::processMessage(QString &html, const ChatSession *session, const Message &message)
{
	// TODO: add cleanup
	static QList<MessageModifier *> modifiers;
	static QList<MessageModifierTrack> list;
	static bool is_inited = false;
	if(!is_inited)
	{
		is_inited = true;
		GeneratorList generators = ObjectGenerator::module<MessageModifier>();
		foreach(const ObjectGenerator *gen, generators)
		{
			MessageModifier *modifier = gen->generate<MessageModifier>();
			modifiers << modifier;
			foreach(const QString &name, modifier->supportedNames())
			{
				if(name.isEmpty() || name == QLatin1String("message"))
					continue;
				QString escaped = QRegExp::escape(name);
				QRegExp regexp(QLatin1Literal("%") % escaped % QLatin1Literal("\\{(.*)\\}%")
							   % QLatin1Literal("|%") % escaped % QLatin1Literal("()%"));
				list << MessageModifierTrack(regexp, name, modifier);
			}
		}
	}
	QList<MessageModifierTrack>::iterator it = list.begin();
	for(; it != list.end(); it++)
	{
		int pos=0;
		while((pos = it->regexp.indexIn(html, pos)) != -1)
		{
			QString modified = it->modifier->getValue(session, message, it->name, it->regexp.cap(1));
			html.replace(pos, it->regexp.cap(0).length(), modified);
		}
	}

	QString text = message.property("html").toString();
	if(text.isEmpty())
		text = Qt::escape(message.text());
	makeUrls(text, message);
	makeUrls(text);
	text = Emoticons::theme().parseEmoticons(text);
	text = text.replace("\n","<br />");
	makeSenderColors(html, message);
	makeBackground(html);
	makeUserIcons(message,html);
	html.replace(QLatin1String("%message%"), text);
}

ChatStyleOutput::ChatStyleOutput (QObject *parent) :
	QWebPage(parent),
	m_session(0),
	separator(true)
{
	Config cfg = Config("appearance").group("chat");
	groupUntil = cfg.value<int>("groupUntil", 900);
	store_service_messages = cfg.group("history").value<bool>("storeServiceMessages", true);
	skipOneMerge = true;
	setLinkDelegationPolicy(QWebPage::DelegateAllLinks);
	connect(this,SIGNAL(linkClicked(QUrl)),SLOT(onLinkClicked(QUrl)));
}

void ChatStyleOutput::loadHistory()
{
	ConfigGroup adium_chat = Config("appearance").group("chat/history");
	int max_num = adium_chat.value<int>("maxDisplayMessages",5);
	MessageList messages = History::instance()->read(m_session->getUnit(), max_num);
	foreach (Message mess, messages) {
		mess.setProperty("silent",true);
		mess.setProperty("store",false);
		mess.setProperty("history",true);
		if (!mess.chatUnit()) //TODO FIXME
			mess.setChatUnit(m_session->getUnit());
		appendMessage(mess);
	}
	previous_sender.clear();
	skipOneMerge = true;
}

void ChatStyleOutput::loadSettings()
{
	ConfigGroup adium_chat = Config("appearance/adiumChat").group("style");
	QString theme = adium_chat.value<QString>("name","default");
	QString path = ThemeManager::path("webkitstyle", theme);
	QString variant = adium_chat.value<QString>("variant", QString());
	Config achat(QStringList()
				 << "appearance/adiumChat"
				 << ThemeManager::path("webkitstyle",theme).append("/Contents/Resources/custom.json"));
	ConfigGroup variables = achat;
	int count = variables.beginArray(theme);
	QString css;
	for (int num = 0; num < count; num++) {
		ConfigGroup parameter = variables.arrayElement(num);
		css.append(QString("%1 { %2: %3; } ")
				   .arg(parameter.value("selector", QString()))
				   .arg(parameter.value("parameter", QString()))
				   .arg(parameter.value("value", QString())));
	}
	setCustomCSS(css);
	loadTheme(path,variant);
	m_current_datetime_format = adium_chat.value<QString>("datetimeFormat","hh:mm:ss dd/MM/yyyy");
}

void ChatStyleOutput::loadTheme(const QString& path, const QString& variant)
{
	m_current_style_path = path;
	ChatStyleGenerator generator (m_current_style_path,variant);
	m_current_style = generator.getChatStyle();
	m_current_variant = variant.isEmpty() ? m_current_style.defaultVariant.first : variant;
	preparePage(m_session);
}

void ChatStyleOutput::reloadStyle()
{
	QString js;
	js += "setStylesheet(\"mainStyle\",\"";
	js += getVariantCSS();
	js += "\");";
	currentFrame()->evaluateJavaScript(js);
	js = QString("setCustomStylesheet(\"%1\")").arg(m_current_css);
	currentFrame()->evaluateJavaScript(js);
}

void ChatStyleOutput::setCustomCSS(const QString &css)
{
	m_current_css = css;
	reloadStyle();
}

void ChatStyleOutput::clear()
{
	preparePage(m_session);
}

ChatStyleOutput::~ChatStyleOutput()
{
}

StyleVariants ChatStyleOutput::getVariants() const
{
	return m_current_style.variants;
}

QString ChatStyleOutput::getMainCSS()
{
	return  m_current_style.mainCSS;
}

QString ChatStyleOutput::getVariantCSS()
{
	return m_current_style.variants.value(m_current_variant, m_current_style.defaultVariant.second);
}

QString ChatStyleOutput::getVariant() const
{
	return m_current_style.variants.contains(m_current_variant) ? m_current_variant : m_current_style.defaultVariant.first;
}

void ChatStyleOutput::preparePage (const ChatSessionImpl *session)
{
	QPalette palette = this->palette();
	if(m_current_style.backgroundIsTransparent)
	{
		palette.setBrush(QPalette::Base, Qt::transparent);
		if(view())
			view()->setAttribute(Qt::WA_OpaquePaintEvent, false);
	}
	else
	{
		palette.setBrush(QPalette::Base, m_current_style.backgroundColor);
	}
	setPalette(palette);
	//TODO
	QString html = makeSkeleton(session,QDateTime::currentDateTime());
	mainFrame()->setHtml(html);
	reloadStyle();
	loadHistory();
}

QString ChatStyleOutput::makeSkeleton (const ChatSessionImpl *session, const QDateTime&)
{
	//TODO
	QString headerHTML = m_current_style.headerHtml;
	QString footerHTML = m_current_style.footerHtml;

	QString generalSkeleton = m_current_style.templateHtml;
	generalSkeleton.replace(generalSkeleton.indexOf("%@"),2,m_current_style.baseHref);
	generalSkeleton.replace(generalSkeleton.lastIndexOf("%@"),2,m_current_style.footerHtml);
	generalSkeleton.replace(generalSkeleton.lastIndexOf("%@"),2,m_current_style.headerHtml);
	generalSkeleton.replace(generalSkeleton.lastIndexOf("%@"),2,getVariantCSS());
	if(generalSkeleton.contains("%@"))
		generalSkeleton.replace(generalSkeleton.indexOf("%@"),2,"@import url( \"main.css\" );");
	//	generalSkeleton.replace("%rep2%", "main.css");
	//	generalSkeleton.replace("%rep3%", "Variants/" + variantUsedName + ".css");
	//	generalSkeleton.replace("%rep4%", headerHTML);

	generalSkeleton = generalSkeleton.replace("%chatName%", session->getId());
	generalSkeleton = generalSkeleton.replace("%sourceName%", Qt::escape(session->getAccount()->name() ));
	generalSkeleton = generalSkeleton.replace("%destinationName%", Qt::escape(session->getUnit()->title()));
	const Buddy *c = qobject_cast< const Buddy *>(session->getUnit());
	QString _ownerIconPath = session->getAccount()->property("avatar").toString();
	QString _partnerIconPath = c ? c->avatar() : QString();

	if(_ownerIconPath == "")
		generalSkeleton = generalSkeleton.replace("%outgoingIconPath%", "outgoing_icon.png");
	else
		generalSkeleton = generalSkeleton.replace("%outgoingIconPath%", _ownerIconPath);
	if(_partnerIconPath == "")
		generalSkeleton = generalSkeleton.replace("%incomingIconPath%", "incoming_icon.png");
	else
		generalSkeleton = generalSkeleton.replace("%incomingIconPath%", _partnerIconPath);

	return generalSkeleton;
}

void ChatStyleOutput::appendMessage(const qutim_sdk_0_3::Message &msg)
{
	bool same_from = false;
	QString item;
	Message message = msg;
	qint64 id = message.id();
	bool service = message.property("service").isValid();
	if(message.text().startsWith("/me ")) {
		// FIXME we shouldn't copy data there
		QString text = message.property("html").toString();
		if (text.startsWith("/me ")) {
			message.setProperty("html",text.mid(3));
		}
		text = message.text();
		message.setText(text.mid(3));
		item = makeAction(m_session,message);
		message.setText(text);
		previous_sender.clear();
		skipOneMerge = true;
	}
	else if (message.property("action").toBool()) {
		item = makeAction(m_session,message);
		previous_sender.clear();
		skipOneMerge = true;
	}
	else if (service) {
		item =  makeStatus(m_session,message);
		previous_sender.clear();
		skipOneMerge = true;
	}
	else {
		QString currentSender;
		if (message.isIncoming()) {
			currentSender = message.property("senderName",message.chatUnit()->title());
		}
		else {
			currentSender = message.property("senderName",message.chatUnit()->account()->name());
		}

		same_from = (!skipOneMerge) && (previous_sender == currentSender);
		debug() << previous_sender << currentSender << skipOneMerge;
		if (lastDate.isNull())
			lastDate = message.time();
		if (lastDate.secsTo(message.time()) > groupUntil)
			same_from = false;
		lastDate = message.time();
		item =  makeMessage(m_session, message, same_from, id);
		previous_sender = currentSender;
		skipOneMerge = false;
	}

	if (!m_session->isActive()) {
		if (!separator && !message.property("service", false) && qobject_cast<const Conference *>(message.chatUnit())) {
			QString jsInactive = QString("separator = document.getElementById(\"separator\");")
					% QString("if (separator) separator.parentNode.removeChild(separator);")
					% QString("appendMessage(\"<hr id='separator'><div id='insert'></div>\");");
			mainFrame()->evaluateJavaScript(jsInactive);
			previous_sender.clear();
			separator = true;
		}
	}

	QString jsTask = QString("append%2Message(\"%1\");").arg(
				validateCpp(item), same_from?"Next":"");
	mainFrame()->evaluateJavaScript(jsTask);

	if (message.property("store", true) && (!service || (service && store_service_messages)))
		History::instance()->store(message);
}

void ChatStyleOutput::setVariant ( const QString& _variantName )
{
	m_current_variant = _variantName;
	reloadStyle();
}

QString ChatStyleOutput::makeMessage(const ChatSessionImpl *session, const Message &mes,
									 bool sameSender, qint64 id)
{
	// prepare values, so they could be inserted to html code
	QString html;
	if (!mes.chatUnit())
	{
		//				qDebug() << "Chat unit is not defined";
		return QString();
	}

	if(mes.property("history").toBool())
	{
		if ( !mes.isIncoming() )
			html = sameSender ? m_current_style.nextOutgoingHistoryHtml : m_current_style.outgoingHistoryHtml;
		else
			html = sameSender ? m_current_style.nextIncomingHistoryHtml : m_current_style.incomingHistoryHtml;
	}
	else
	{
		if ( !mes.isIncoming() )
			html = sameSender ? m_current_style.nextOutgoingHtml : m_current_style.outgoingHtml;
		else
			html = sameSender ? m_current_style.nextIncomingHtml : m_current_style.incomingHtml;
	}
	// Replace %sender% to name

	QString sender_name = makeName(mes);
	QString sender_id = makeId(mes);

	html = html.replace("%messageId%", "message"+QString::number(id));
	html = html.replace("%sender%", Qt::escape(sender_name));
	// Replace %senderScreenName% to name
	html = html.replace("%senderScreenName%", Qt::escape(sender_id));
	makeTime(html,mes.time());
	// Replace %service% to protocol name
	// TODO: have to get protocol global value somehow
	html = html.replace("%service%", Qt::escape(mes.chatUnit()->account()->protocol()->id()));
	// Replace %protocolIcon% to sender statusIcon path
	// TODO: find icon to add here
	html = html.replace("%senderStatusIcon%", "");
	html = html.replace("%messageDirection%", mes.text().isRightToLeft() ? "rtl" : "ltr" );
	processMessage(html, session, mes);
	return html;
}

QString ChatStyleOutput::makeAction (const ChatSessionImpl *session, const Message &mes)
{
	QString html = (!mes.isIncoming()) ? m_current_style.outgoingActionHtml : m_current_style.incomingActionHtml;

	QString sender_name = makeName(mes);
	QString sender_id = makeId(mes);

	// Replace %sender% to name
	html = html.replace("%sender%", Qt::escape(sender_name));
	// Replace %senderScreenName% to name
	html = html.replace("%senderScreenName%", Qt::escape(sender_id));
	makeTime(html, mes.time());
	// Replace %service% to protocol name
	// TODO: have to get protocol global value somehow
	html = html.replace("%service%", Qt::escape(mes.chatUnit()->account()->protocol()->id()));
	// Replace %protocolIcon% to sender statusIcon path
	// TODO: find icon to add here
	html = html.replace("%senderStatusIcon%", "");

	// Replace %messageDirection% with "rtl"(Right-To-Left) or "ltr"(Left-to-right)
	html = html.replace("%messageDirection%", mes.text().isRightToLeft() ? "rtl" : "ltr" );

	processMessage(html,session,mes);
	return html;
}

QString ChatStyleOutput::makeStatus (const ChatSessionImpl *session, const Message &mes)
{
	Message tmp_msg = mes;
	QString html = m_current_style.statusHtml;
	makeTime(html, mes.time());
	QString title = tmp_msg.property("title").toString();
	if (!title.isEmpty()) {
		QString txt = tmp_msg.text();
		tmp_msg.setText(txt.isEmpty() ? title : QString("%1: %2").arg(title,txt));
	}

	processMessage(html,session,tmp_msg);
	return html;
}

void AdiumChat::ChatStyleOutput::makeUserIcons(const Message &mes, QString &source)
{
	// Replace userIconPath
	QString avatarPath;
	if (mes.isIncoming()) {
		const Buddy *c = qobject_cast< const Buddy *>(mes.chatUnit());
		//				debug() << c << mes.chatUnit();
		avatarPath = c ? c->avatar() : (m_current_style.baseHref + "Outgoing/buddy_icon.png");
	}
	else {
		//TODO
		avatarPath = mes.chatUnit()->account()->property("avatar").toString();
	}

	if(avatarPath.isEmpty())
	{
		if(!mes.isIncoming())
			avatarPath = (m_current_style.baseHref + "Outgoing/buddy_icon.png");
		else
			avatarPath = (m_current_style.baseHref + "Incoming/buddy_icon.png");

	}
	source.replace("%userIconPath%", avatarPath);
}

void ChatStyleOutput::makeBackground(QString &html)
{
	// search for background colors and change them, so CSS would stay clean
	QString bgColor = "inherit";
	static QRegExp textBackgroundRegExp("%textbackgroundcolor\\{([^}]*)\\}%");
	int textPos=0;
	while((textPos=textBackgroundRegExp.indexIn(html, textPos)) != -1)
	{
		html = html.replace(textPos, textBackgroundRegExp.cap(0).length(), bgColor);
	}
}

void ChatStyleOutput::makeTime(QString &input, const QDateTime& datetime, const QString &regexp)
{
	QString time = datetime.toString(m_current_datetime_format);
	// Replace %time% to time
	input.replace("%time%", Qt::escape(time));
	// Replace %time{X}%
	static QRegExp timeRegExp(regexp);
	int pos=0;
	while((pos=timeRegExp.indexIn(input, pos)) != -1)
		input.replace(pos, timeRegExp.cap(0).length(), Qt::escape(convertTimeDate(timeRegExp.cap(1), datetime)));
	// Replace %message%'s, replacing last to avoid errors if messages contains tags
}

void ChatStyleOutput::makeSenderColors(QString &html, const Message &message)
{
	const QStringList &senderColors = m_current_style.senderColors;
	int id = qHash(makeName(message)) % senderColors.size();
	html.replace(QLatin1String("%senderColor%"), senderColors.at(id));
}

void ChatStyleOutput::makeUrls(QString &html,const Message& message)
{
	QVariant data = message.property("urls");
	QList<QUrl> urls;
	if (data.type() == QVariant::Url)
		urls.append(data.toUrl());
	else if (data.type() == QVariant::List) {
		QVariantList list = data.toList();
		for (int i = 0; i < list.size(); ++i) {
			if (list.at(i).type() == QVariant::Url)
				urls.append(list.at(i).toUrl());
		}
	}
	QList<QUrl>::const_iterator it;
	for (it=urls.constBegin();it!=urls.constEnd();it++) {
		html = html.append("<br /><a href=\"%1\">%2</a>").arg(it->toEncoded(),it->toString());
	}
}

void ChatStyleOutput::makeUrls(QString &html)
{
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
}

QString ChatStyleOutput::makeName(const Message &mes)
{
	QString sender_name = mes.property("senderName",QString());
	if (sender_name.isEmpty()) {
		if (!mes.isIncoming()) {
			const Conference *conf = qobject_cast<const Conference*>(mes.chatUnit());
			if (conf && conf->me()) {
				sender_name = conf->me()->title();
			}
			else {
				sender_name = mes.chatUnit()->account()->name();
			}
		}
		else {
			sender_name = mes.chatUnit()->title();
		}
	}
	return sender_name;
}

QString ChatStyleOutput::makeId(const Message &mes)
{
	QString sender_id = mes.property("senderId",QString());
	if (sender_id.isEmpty()) {
		if (!mes.isIncoming()) {
			const Conference *conf = qobject_cast<const Conference*>(mes.chatUnit());
			if (conf && conf->me()) {
				sender_id = conf->me()->id();
			}
			else {
				sender_id = mes.chatUnit()->account()->id();
			}
		} else {
			sender_id = mes.chatUnit()->id();
		}
	}
	return sender_id;
}

}
}

