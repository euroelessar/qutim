/****************************************************************************
 *  account.h
 *
 *  Copyright (c) 2009 by Nigmatullin Ruslan <euroelessar@gmail.com>
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

#ifndef ACCOUNT_H
#define ACCOUNT_H

#include "configbase.h"
#include "menucontroller.h"
#include <QMetaType>

namespace qutim_sdk_0_3
{
	class ChatUnit;
	class Contact;
	class Protocol;
	class AccountPrivate;

	/**
	* @brief Account is base class for all account entites
	*/
	class LIBQUTIM_EXPORT Account : public MenuController
	{
		Q_DECLARE_PRIVATE(Account)
		Q_OBJECT
		Q_PROPERTY(QString id READ id)
		Q_PROPERTY(Status status READ status WRITE setStatus NOTIFY statusChanges)
		Q_PROPERTY(QString name READ name NOTIFY nameChanged)
	public:
		/**
		* @brief Account's contructor
		*
		* @param id Account's identification, i.e. JID for XMPP or UIN for ICQ
		*
		* @param protocol Pointer to account's protocol
		*/
		Account(const QString &id, Protocol *protocol);
		/**
		* @brief Account destructor
		*/
		virtual ~Account();
		/**
		* @brief Returnes account's identification, which is unique for protocol
		*
		* @return Account's identitification string
		*/
		QString id() const;
		/**
		* @brief Returnes account's representable name
		*
		* @return Account's name
		*/
		virtual QString name() const;
		/**
		* @brief Returnes @ref Config for current account
		*
		* @return Config entity
		*/
		Config config();
		/**
		* @brief Returnes @ref ConfigGroup for certain group
		*
		* @param group Group name for configs
		*
		* @return ConfigGroup entity
		*/
		ConfigGroup config(const QString &group);
		/**
		* @brief Returnes account's status
		*
		* @return Account's status
		*/
		Status status() const;
		/**
		* @brief Returnes pointer to account's @ref Protocol
		*
		* @return Pointer to protocol
		*/
		Protocol *protocol();
		/**
		* @brief Returnes pointer to account's @ref Protocol
		*
		* @return Pointer to protocol
		*/
		const Protocol *protocol() const;
		/**
		* @brief Asks account to change status on server. If status is not offline and
		* acount hasn't already connected to server it should try to do it, else if
		* status is offline and account is conntected to server it should disconnect
		*
		* @param status Status to be setted on server
		*/
		virtual void setStatus(Status status);
		/**
		* @brief Method looks for appropriate @ref ChatUnit for conversation with unit
		*
		* @param unit ChatUnit with which user wants to start conversation
		*
		* @return ChatUnit for @ref ChatSesion
		*/
		virtual ChatUnit *getUnitForSession(ChatUnit *unit);
		/**
		* @brief Method returnes @ref ChatUnit by it's identification string
		*
		* @param unitId Identification string for @ref ChatUnit
		* @param create If true create ChatUnit if it doesn't exist
		*
		* @note If unitId is invalid, i.e. invalid JID, unit won't be created and NULL will be returned
		*
		* @return ChatUnit with iq equal to unitId
		*/
		virtual ChatUnit *getUnit(const QString &unitId, bool create = false) = 0;
	signals:
		/**
		* @brief Signal is emitted when new contact was created
		*
		* @param contact Created contact
		*/
		void contactCreated(qutim_sdk_0_3::Contact *contact);
		/**
		* @brief Signal is emitted when account's name was changed
		*
		* @param name New account's name
		*/
		void nameChanged(const QString &name);
		/**
		* @brief Signal is emitted when account's status was changed
		*
		* @param status New account's status
		*/
        void statusChanged(qutim_sdk_0_3::Status status);
	};
}

Q_DECLARE_METATYPE(qutim_sdk_0_3::Account*)

#endif // ACCOUNT_H
