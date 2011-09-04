/****************************************************************************
 *  chatunit.h
 *
 *  Copyright (c) 2010 by Nigmatullin Ruslan <euroelessar@gmail.com>
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

#ifndef CHATUNIT_H
#define CHATUNIT_H

#include "menucontroller.h"
#include <QMetaType>
#include <QEvent>

namespace qutim_sdk_0_3
{

class MetaContact;
class Account;
class Message;
class ChatUnit;
class ChatUnitPrivate;
class Contact;
typedef QList<ChatUnit *> ChatUnitList;

enum ChatState
{
	ChatStateActive = 0,    // User is actively participating in the chat session.
	ChatStateInActive,      // User has not been actively participating in the chat session.
	ChatStateGone,          // User has effectively ended their participation in the chat session.
	ChatStateComposing,     // User is composing a message.
	ChatStatePaused         // User had been composing but now has stopped.
};

/**
 * @brief ChatUnit is base class for all chat members
 */
class LIBQUTIM_EXPORT ChatUnit : public MenuController
{
	Q_DECLARE_PRIVATE(ChatUnit)
	Q_OBJECT
	Q_PROPERTY(QString id READ id CONSTANT)
	Q_PROPERTY(QString title READ title NOTIFY titleChanged)
	Q_PROPERTY(ChatState chatState READ chatState WRITE setChatState NOTIFY chatStateChanged)
	Q_PROPERTY(qutim_sdk_0_3::Account* account READ account CONSTANT)
	Q_PROPERTY(bool conference READ isConference CONSTANT)
public:
	/**
  * @brief default ChatUnit's contructor
  *
  * @param account Pointer to chatunit's account
  */
	ChatUnit(Account *account);
	/**
  * @brief ChatUnit's contructor
  *
  * @param d ChatUnitPrivate
  *
  * @param account Pointer to chatunit's account
  *
  * @internal For internal use only
  */
	ChatUnit(ChatUnitPrivate &d, Account *account);
	/**
  * @brief ChatUnit's destructor
  */
	virtual ~ChatUnit();
	/**
  * @brief Returns chatunit's identification, which is unique for account
  *
  * @return ChatUnit's identitification string
  */
	virtual QString id() const = 0;
	/**
  * @brief Returns chatunit's representable name
  *
  * @return ChatUnit's name
  */
	virtual QString title() const;
	/**
  * @brief Returns pointer to chatunits's @ref Account
  *
  * @return Pointer to account
  */
	Account *account();
	/**
  * @brief Returns pointer to chatunits's @ref Account
  *
  * @return Pointer to account
  */
	const Account *account() const;
	bool isConference() const;
	/**
  * @brief send message to chatunit
  *
  * @param message Message, which to be sent to the recipient
  */
	virtual bool sendMessage(const qutim_sdk_0_3::Message &message) = 0;
	
	Q_INVOKABLE bool send(const qutim_sdk_0_3::Message &message);
	/**
  * @brief Returns TODO
  *
  * @return ChatUnitList
  */
	Q_INVOKABLE virtual QList<qutim_sdk_0_3::ChatUnit*> lowerUnits();
	/**
  * @brief Returns TODO
  *
  * @return Pointer to upper chatunit
  */
	Q_INVOKABLE virtual qutim_sdk_0_3::ChatUnit *upperUnit();
	/**
  * @brief Returns TODO
  *
  * @return Pointer to upper buddy
  */
	ChatUnit *buddy();
	const ChatUnit *buddy() const;
	/**
  * @brief Returns TODO
  *
  * @return Pointer to upper metaContact or 0 if upper metaContact doesn't exist.
  */
	ChatUnit *metaContact();
	const ChatUnit *metaContact() const;
	virtual const ChatUnit *getHistoryUnit() const;
public slots:
	quint64 sendMessage(const QString &text);
	/**
  * @brief Sets a new chat state
  *
  * @param state New ChatState
  */
	void setChatState(qutim_sdk_0_3::ChatState state);
	qutim_sdk_0_3::ChatState chatState() const;
signals:
	/*!
	Notify that ChatUnit's \a title is changed, may be because of
	changes in name
  */
	void titleChanged(const QString &current, const QString &previous);
	/**
	Notifies that new lower unit is added.
   */
	void lowerUnitAdded(ChatUnit *unit);
	void chatStateChanged(qutim_sdk_0_3::ChatState current,qutim_sdk_0_3::ChatState previous);
};

/**
 * @brief The ChatStateEvent class provides events for change chat state
 */
class LIBQUTIM_EXPORT ChatStateEvent : public QEvent
{
public:
	/**
  * @brief default ChatStateEvent's contructor
  *
  * @param state ChatState
  */
	ChatStateEvent(ChatState state);
	/**
  * @brief Returns chatState
  *
  * @return ChatState shatState
  */
	inline ChatState chatState() const { return m_state; }
	/**
  * @brief event type
  *
  * @return QEvent::Type eventType
  */
	static QEvent::Type eventType();
protected:
	ChatState m_state;
};
}

Q_DECLARE_METATYPE(qutim_sdk_0_3::ChatUnit*)
Q_DECLARE_METATYPE(QList<qutim_sdk_0_3::ChatUnit*>)
Q_ENUMS(qutim_sdk_0_3::ChatState)

#endif // CHATUNIT_H
