/****************************************************************************
 *  chatstyleoutput.h
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

#ifndef CHATSTYLEOUTPUT_H_
#define CHATSTYLEOUTPUT_H_

#include <QObject>
#include "chatstyle.h"
#include "chatsessionimpl.h"
#include <libqutim/contact.h>
class QWebPage;
class QDateTime;

namespace qutim_sdk_0_3
{
	class ChatSession;
}

namespace AdiumChat
{
	class ChatStyleOutput
	{
	public:
		/*
		constructor, _styleName - name of the style to use, it have to be same to directory name
		_variantName - name of the CSS file to use, don't write .css
		*/
		ChatStyleOutput();
		~ChatStyleOutput();

		/*
		passes valriants QHash (list of all variants) from lower class
		*/
		StyleVariants getVariants() const;
		QString getVariant() const;

		void setVariant(const QString &_variantName);
		void setCustomCSS(const QString &css);
		QString makeMessage(const ChatSessionImpl *session, const Message &mes, bool _sameSender);
		/*
		changes keywords to action atributes in html like "Bob is writing on the desk"
		_name - sender's nickname
		_message - message text
		_direction - "true" if it was send, "false" if it was recived
		_time - time at witch message was send
		_avatarPath - path to avatar, if "" uses style provided picture
		_aligment - "true" if left-to-right, "false" if right-to-left
		_senderID - sender's ID
		_servise - protocol used to send a message
		*/
		QString makeAction(const ChatSessionImpl *session, const Message &mes);

		/*
		It is used for displaying system and user messages like "user gone offline",
		"Marina is now away", "You are being ignored" etc.
		_message - message by it self to be shown
		_time - timestamp
		*/
		QString makeStatus(const ChatSessionImpl *session, const Message &mes);

		inline QString getMainCSS();
		inline QString getVariantCSS();
		void preparePage(QWebPage *page,const ChatSessionImpl *session);
		void reloadStyle(QWebPage* page);
		void loadTheme(const QString &name, const QString &variant = QString());
	private:
		/*
		creats a html skeleton. Future messages will be added to it
		skeleton consist of styles, header and footer
		it has a mark as well. before this mark new messages should be added
		*/
		QString makeSkeleton(const ChatSessionImpl *session, const QDateTime &datetime);
		void makeUserIcons(const Message &mes, QString &source);
		inline void makeTime (QString &input, const QDateTime& datetime,const QString &regexp = "%time\\{([^}]*)\\}%");
		void makeUrls(QString& html, const Message& message);//create url list under message body
		void makeUrls(QString &html); //TODO separate to adiumchat SDK
		QString makeName(const Message &mes);
		QString makeId(const Message &mes);
		void makeBackground(QString &html);
		void processMessage(QString &html, const ChatSession *session, const Message &message);
		void loadSettings();
		/*
		style used for output generation
		*/
		ChatStyle m_current_style;
		QScopedPointer<ChatStyleGenerator> m_chat_style_generator;
		/*
		remembers current variant name
		*/
		QString m_current_variant;
		QString m_current_style_path;
		QString m_current_datetime_format;
		QString m_current_css;
	};
}

#endif /*CHATSTYLEOUTPUT_H_*/
