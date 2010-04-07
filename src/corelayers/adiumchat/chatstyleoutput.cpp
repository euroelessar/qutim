/****************************************************************************
 *  chatstyleoutput.cpp
 *
 *  Copyright (c) 2010 by Sidorov Aleksey <sauron@citadelspb.com>
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

#include <QtDebug>
#include <QTextDocument>
#include <QStringList>
#include "chatstyle.h"
#include "chatstyleoutput.h"
#include <libqutim/libqutim_global.h>
#include "chatsessionimpl.h"
#include <QDateTime>
#include <libqutim/configbase.h>
#include <libqutim/account.h>
#include <libqutim/protocol.h>
#include <QWebFrame>
#include <QWebPage>
#include <QFileInfo>
#include <QStringBuilder>
#include "messagemodifier.h"
#include "libqutim/objectgenerator.h"
#include "chatsessionimpl.h"
#include <libqutim/emoticons.h>
#include <libqutim/debug.h>
#include "libqutim/conference.h"

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

	void ChatStyleOutput::processMessage(QString &html, const ChatSession *session, const Message &message)
	{
		// TODO: add cleanup
		static QList<MessageModifier *> modifiers;
		static QList<MessageModifierTrack> list;
		static bool is_inited = false;
		if(!is_inited)
		{
			is_inited = true;
			GeneratorList generators = moduleGenerators<MessageModifier>();
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
		text = Emoticons::theme().parseEmoticons(text);
		text = text.replace("\n","<br />");
		makeBackground(html);
		makeUrls(text, message);
		makeUserIcons(message,html);
		html.replace(QLatin1String("%message%"), text);
	}

	ChatStyleOutput::ChatStyleOutput ()
	{
		loadSettings();
	}

	void ChatStyleOutput::loadSettings()
	{
		ConfigGroup adium_chat = Config("appearance/adiumChat").group("style");
		QString theme = adium_chat.value<QString>("name","default");
		QString path = getThemePath("webkitstyle", theme);
		QString variant = adium_chat.value<QString>("variant", QString());
		Config achat(QStringList() << "appearance/adiumChat"
				<< getThemePath("webkitstyle",theme).append("/Contents/Resources/custom.json"));
		const ConfigGroup variables = achat.group(theme);
		int count = variables.arraySize();
		QString css;
		for (int num = 0; num < count; num++) {
			ConfigGroup parameter = variables.at(num);
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
	}

	void ChatStyleOutput::reloadStyle(QWebPage* page)
	{
		QString js;
		js += "setStylesheet(\"mainStyle\",\"";
		js += getVariantCSS();
		js += "\");";
		page->currentFrame()->evaluateJavaScript(js);
		js = QString("setCustomStylesheet(\"%1\")").arg(m_current_css);
		page->currentFrame()->evaluateJavaScript(js);
	}

	void ChatStyleOutput::setCustomCSS(const QString &css)
	{
		m_current_css = css;
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

	void ChatStyleOutput::preparePage ( QWebPage* page,const ChatSessionImpl *session)
	{
		QPalette palette = page->palette();
		if(m_current_style.backgroundIsTransparent)
		{
			palette.setBrush(QPalette::Base, Qt::transparent);
			if(page->view())
				page->view()->setAttribute(Qt::WA_OpaquePaintEvent, false);
		}
		else
		{
			palette.setBrush(QPalette::Base, m_current_style.backgroundColor);
		}
		page->setPalette(palette);
		//TODO
		QString html = makeSkeleton(session,QDateTime::currentDateTime());
		page->mainFrame()->setHtml(html);
		reloadStyle(page);
	}

	QString ChatStyleOutput::makeSkeleton (const ChatSessionImpl *session, const QDateTime& _dateTime)
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

	void ChatStyleOutput::setVariant ( const QString& _variantName )
	{
		m_current_variant = _variantName;
	}

	QString ChatStyleOutput::makeMessage(const ChatSessionImpl *session, const Message &mes, bool sameSender)
	{
		// prepare values, so they could be inserted to html code
		QString html;
		if (!mes.chatUnit())
		{
			qDebug() << "Chat unit is not defined";
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
		//FIXME
		QString sender_name;
		QString sender_id;

		if (!mes.isIncoming()) {
			const Conference *conf = qobject_cast<const Conference*>(mes.chatUnit());
			if (conf && conf->me()) {
				sender_name = conf->me()->title();
				sender_id = conf->me()->id();
			}
			else {
				sender_name = mes.chatUnit()->account()->name();
				sender_id = mes.chatUnit()->account()->id();
			}
		}
		else {
			sender_name = mes.chatUnit()->title();
			sender_id = mes.chatUnit()->id();
		}

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

		//FIXME
		QString sender_name;
		QString sender_id;
		if (!mes.isIncoming()) {
			const Conference *conf = qobject_cast<const Conference*>(mes.chatUnit());
			if (conf && conf->me()) {
				sender_name = conf->me()->title();
				sender_id = conf->me()->id();
			}
			else {
				sender_name = mes.chatUnit()->account()->name();
				sender_id = mes.chatUnit()->account()->id();
			}
		}
		else {
			sender_name = mes.chatUnit()->title();
			sender_id = mes.chatUnit()->id();
		}

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
			debug() << c << mes.chatUnit();
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

	void ChatStyleOutput::makeUrls(QString &html,const Message& message)
	{
		QVariant data = message.property("urls");
		QList<QUrl> urls;
		if (data.type() == QVariant::Url)
			urls.append(data.toUrl());
		else if (data.type() == QVariant::List) {
			QList<QVariant> list = data.toList();
				for (int i = 0; i < list.size(); ++i) {
				if (list.at(i).type() == QVariant::Url)
				urls.append(list.at(i).toUrl());
			}
		}
		QList<QUrl>::const_iterator it;
		for (it=urls.begin();it!=urls.end();it++)
		{
			html.append("<br /><a href=\"%1\">%2</a>").arg(it->toEncoded(),it->toString());
		}
	}
}


