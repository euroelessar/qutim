/****************************************************************************
 *  chatstyleoutput.h
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

#ifndef CHATSTYLEOUTPUT_H_
#define CHATSTYLEOUTPUT_H_

#include <QObject>
#include "chatstyle.h"
#include <chatlayer/chatsessionimpl.h>
#include <qutim/contact.h>
#include <QWebPage>
#include <QDateTime>
#include <chatlayer/chatviewfactory.h>

class QDateTime;

namespace qutim_sdk_0_3
{
class ChatSession;
LIBQUTIM_EXPORT QString convertTimeDate(const QString &mac_format, const QDateTime &datetime);
LIBQUTIM_EXPORT QString &validateCpp(QString &text);
}

namespace Core
{
namespace AdiumChat
{

class ChatStyleOutput : public QWebPage, public ChatViewController
{
	Q_OBJECT
	Q_INTERFACES(Core::AdiumChat::ChatViewController)
public:
	/*
  constructor, _styleName - name of the style to use, it have to be same to directory name
  _variantName - name of the CSS file to use, don't write .css
  */
	ChatStyleOutput(QObject *parent = 0);
	virtual ~ChatStyleOutput();
	virtual void appendMessage(const qutim_sdk_0_3::Message &msg);
	virtual void setChatSession(ChatSessionImpl *session);
	virtual ChatSessionImpl *getSession() const;
	/*
  passes valriants QHash (list of all variants) from lower class
  */
	StyleVariants getVariants() const;
	QString getVariant() const;

	void setVariant(const QString &_variantName);
	void setCustomCSS(const QString &css);
	QString makeMessage(const ChatSessionImpl *session, const Message &mes, bool _sameSender, qint64 id);
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
	void preparePage(const ChatSessionImpl *session);
	void reloadStyle();
	void loadTheme(const QString &name, const QString &variant = QString());
	bool virtual eventFilter(QObject *obj, QEvent *ev);
	virtual void clear();
public slots:
	QVariant evaluateJavaScript(const QString &scriptSource);
	void loadSettings();
private slots:
	void onSessionActivated(bool active);
	void onLinkClicked(const QUrl &url);
	void setChatUnit(ChatUnit *unit);
private:
	/*
  creats a html skeleton. Future messages will be added to it
  skeleton consist of styles, header and footer
  it has a mark as well. before this mark new messages should be added
  */
	QString makeSkeleton(const ChatSessionImpl *session, const QDateTime &datetime);
	void makeUserIcons(const Message &mes, QString &source);
	inline void makeTime (QString &input, const QDateTime& datetime,const QString &regexp = "%time\\{([^}]*)\\}%");
	void makeSenderColors(QString &html, const Message &message);
	void makeUrls(QString& html, const Message& message);//create url list under message body
	void makeUrls(QString &html); //TODO separate to adiumchat SDK
	QString makeName(const Message &mes);
	QString makeId(const Message &mes);
	void makeBackground(QString &html);
	void processMessage(QString &html, const ChatSession *session, const Message &message);
	void loadHistory();
	ChatSessionImpl *m_session;
	/*
  style used for output generation
  */
	ChatStyleStruct m_current_style;
	QScopedPointer<ChatStyleGenerator> m_chat_style_generator;
	/*
  remembers current variant name
  */
	QString m_current_variant;
	QString m_current_style_path;
	QString m_current_datetime_format;
	QString m_current_css;
	bool skipOneMerge;
	QString previous_sender; // null if outcoming
	QDateTime lastDate;
	int groupUntil;
	bool separator;
	bool store_service_messages;
};

}
}
#endif /*CHATSTYLEOUTPUT_H_*/
