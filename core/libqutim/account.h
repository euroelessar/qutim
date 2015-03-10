/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
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

#ifndef ACCOUNT_H
#define ACCOUNT_H

#include "configbase.h"
#include "menucontroller.h"
#include "status.h"
#include <QMetaType>

namespace qutim_sdk_0_3
{
class ChatUnit;
class Contact;
class Protocol;
class Conference;
class AccountPrivate;
class GroupChatManager;
class ContactsFactory;
class InfoRequestFactory;

class Account;
typedef QList<Account*> AccountList;

/*!
  Account is base class for all account entites.
*/
class LIBQUTIM_EXPORT Account : public MenuController
{
	Q_DECLARE_PRIVATE(Account)
	Q_OBJECT
	Q_PROPERTY(QString id READ id)
	Q_PROPERTY(qutim_sdk_0_3::Protocol* protocol READ protocol CONSTANT)
	Q_PROPERTY(qutim_sdk_0_3::Status status READ status NOTIFY statusChanged)
	Q_PROPERTY(qutim_sdk_0_3::Status userStatus READ userStatus WRITE setUserStatus NOTIFY userStatusChanged)
	Q_PROPERTY(QString name READ name NOTIFY nameChanged)
	Q_PROPERTY(QVariantMap parameters READ parameters WRITE updateParameters NOTIFY parametersChanged)
	Q_PROPERTY(State state READ state NOTIFY stateChanged)
	Q_ENUMS(State)
public:
	enum AccountHookEnum {
		// all values below are reserved for MenuController
		ReadParametersHook = 0x100,
		UpdateParametersHook
	};

	enum State {
		Disconnected,
		Connecting,
		Connected,
		Disconnecting
	};
	
	struct UpdateParametersArgument
	{
		QVariantMap parameters;
		QStringList reconnectionRequired;
	};

	/*!
	  Account's contructor with identification \a id and \a protocol.
	  Identification is unique in current \a protocol, i.e. JID for
	  XMPP or UIN for ICQ.
	*/
	Account(const QString &id, Protocol *protocol);
	Account(AccountPrivate &p, Protocol *protocol);
	/*!
	  Account destructor
	*/
	virtual ~Account();
	/*!
	  Returns account's identification, which is unique for protocol
	*/
	QString id() const;
	/*!
	  Returns account's representable name
	*/
	virtual QString name() const;
	/*!
	  Returns \ref Config for current account. It's equal to
	  \code Config(protocol()->id() + '.' + id() + "/account") \endcode
	*/
	Config config();
	/*!
	  Returns \ref ConfigGroup for certain group with \a name. It's equal to
	  \code config().group(name) \endcode
	*/
	ConfigGroup config(const QString &name);
	/*!
	  Returns account's status
	*/
	Status status() const;
	/*!
	  Returns pointer to account's \ref Protocol
	*/
	Protocol *protocol();
	/*!
	  Returns pointer to account's \ref Protocol
	*/
	const Protocol *protocol() const;

	State state() const;

	/*!
	 * Connects to remote server.
	 *
	 * If account is currently at Connecting or Connected state nothing happens.
	 *
	 * \sa doConnectToServer
	 */
	void connectToServer();

	/*!
	 * Disconnectes from remote server.
	 *
	 * If current state is Disconnecting or Disconnected nothing happens.
	 *
	 * \sa doDisconnectFromServer
	 */
	void disconnectFromServer();

	/*!
	  Asks account to change \a userStatus on server. If \a userStatus is not offline and
	  acount hasn't already connected to server it should try to do it, else if
	  \a userStatus is offline and account is connected to server it should disconnect.
	*/
	void setUserStatus(const Status &userStatus);

	/*!
	 * Returns status set by user.
	 */
	Status userStatus() const;

	/*!
	  Method looks for appropriate \ref ChatUnit for conversation with \a unit.
	  Returns ChatUnit for \ref ChatSession.
	*/
	virtual ChatUnit *getUnitForSession(ChatUnit *unit);
	/*!
	  Method Returns \ref ChatUnit by it's identification string \a unitId.
	  If \a create is \b true create new ChatUnit if it doesn't exist.
	  Returnes ChatUnit with iq equal to unitId.

	  If unitId is invalid, i.e. invalid JID, unit won't be created and NULL will be returned.
	*/
	virtual ChatUnit *getUnit(const QString &unitId, bool create = false) = 0;

	Q_INVOKABLE inline qutim_sdk_0_3::ChatUnit *unit(const QString &unitId, bool create = false);

	QVariantMap parameters() const;
	Q_INVOKABLE QStringList updateParameters(const QVariantMap &parameters);

	/*!
	 * Returns interface by it's meta class
	 */
	QObject *interface(const QMetaObject *meta);

	/*!
	 * Returns interface for type \a T
	 *
	 * It search the correct interface by the class' info "Interface":
	 * \code
	 * Q_CLASSINFO("Interface", "GroupChatManager")
	 * \endcode
	 *
	 * \sa setInterface
	 */
	template <typename T> inline T *interface()
	{
		return qobject_cast<T *>(interface(&T::staticMetaObject));
	}

	/*!
	  Returns the group chat manager of the account.

	  \see resetGroupChatManager()
	*/
	GroupChatManager *groupChatManager();
	ContactsFactory *contactsFactory();
	InfoRequestFactory *infoRequestFactory();

protected:
	/*!
	 * Set \a interface by it's type.
	 *
	 * Ownership is passed to Account's class, \a interface will be destroyed
	 * once there will be passed another object by the same type, or at
	 * account's destruction.
	 */
	inline void setInterface(QObject *interface)
	{
		setInterface(interface->metaObject(), interface);
	}

	/*!
	 * \internal
	 */
	void setInterface(const QMetaObject *meta, QObject *interface);

	template <typename T> inline void setInterface(T *interface = nullptr)
	{
		setInterface(&T::staticMetaObject, interface);
	}

	/**
	  Sets the group chat \a manager to be used by this account.

	  \see groupChatManager()
	*/
	void resetGroupChatManager(GroupChatManager *manager = nullptr);
	void setContactsFactory(ContactsFactory *factory);
	void setInfoRequestFactory(InfoRequestFactory *factory);

	/*!
	 * Implementation of connectToServer method
	 */
	virtual void doConnectToServer() = 0;

	/*!
	 * Implementation of disconnectToServer method
	 */
	virtual void doDisconnectFromServer() = 0;

	/*!
	 * Update current account's status to \a status
	 */
	virtual void doStatusChange(const Status &status) = 0;

	void setEffectiveStatus(const Status &status);
	void setState(State state, Status::ChangeReason reason = Status::ByUnknown);
signals:
	/*!
	  Signal is emitted when new \a contact was created.
	*/
	void contactCreated(qutim_sdk_0_3::Contact *contact);
	/*!
	  Signal is emitted when new \a conference was created.
	*/
	void conferenceCreated(qutim_sdk_0_3::Conference *conference);
	/*!
	  Signal is emitted when account's \a name was changed.
	*/
	void nameChanged(const QString &current, const QString &previous);
	/*!
	  Signal is emitted when account's \a status was changed.
	*/
	void statusChanged(const qutim_sdk_0_3::Status &current, const qutim_sdk_0_3::Status &previous);
	void userStatusChanged(const qutim_sdk_0_3::Status &current, const qutim_sdk_0_3::Status &previous);

	void parametersChanged(const QVariantMap &parameters);

	/*!
	 * Emits when \a interface for \a name was changed.
	 *
	 * \a Interface may be zero object if it was just destroyed.
	 */
	void interfaceChanged(const QByteArray &name, QObject *interface);

	/*!
	 * Account moved to new \a state.
	 */
	void stateChanged(qutim_sdk_0_3::Account::State state);

	/*!
	 * Account is connected to remote server
	 */
	void connected();

	/*!
	 * Account is disconnected from remote server by \a reason
	 */
	void disconnected(qutim_sdk_0_3::Status::ChangeReason reason);
};

ChatUnit *Account::unit(const QString &unitId, bool create)
{
	return getUnit(unitId, create);
}
}

Q_DECLARE_METATYPE(qutim_sdk_0_3::Account*)
Q_DECLARE_METATYPE(QList<qutim_sdk_0_3::Account*>)

#endif // ACCOUNT_H

