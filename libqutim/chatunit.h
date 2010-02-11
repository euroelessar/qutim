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
	class Account;
	class Message;
	class ChatUnit;
	class ChatUnitPrivate;
	class Contact;
	typedef QList<ChatUnit *> ChatUnitList;

	/**
	* @brief ChatUnit is base class for all chat members
	*/
	class LIBQUTIM_EXPORT ChatUnit : public MenuController
	{
		Q_DECLARE_PRIVATE(ChatUnit)
		Q_OBJECT
		Q_PROPERTY(QString id READ id)
		Q_PROPERTY(QString title READ title NOTIFY titleChanged)
		Q_PROPERTY(Account* account READ account)
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
		/**
		* @brief send message to chatunit
		*
		* @param message Message, which to be sent to the recipient
		*/
		virtual void sendMessage(const Message &message) = 0;
		/**
		* @brief Returns TODO
		*
		* @return ChatUnitList
		*/
		virtual ChatUnitList lowerUnits();
		/**
		* @brief Returns TODO
		*
		* @return Pointer to upper chatunit
		*/
		virtual ChatUnit *upperUnit();
	public slots:
		/**
		* @brief Sets a new chat state
		*
		* @param state New ChatState
		*/
		void setChatState(ChatState state);
	signals:
		/*!
		  Notify that ChatUnit's \a title is changed, may be because of
		  changes in name
		*/
		void titleChanged(const QString &title);
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

#endif // CHATUNIT_H
