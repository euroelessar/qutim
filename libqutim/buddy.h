#ifndef BUDDY_H
#define BUDDY_H

#include "chatunit.h"
#include "status.h"
#include <QIcon>
#include <QMetaType>

namespace qutim_sdk_0_3
{
	class Account;
	class Message;
	class BuddyPrivate;
	typedef QPair<LocalizedString, QVariant> InfoField;
	typedef QList<InfoField> InfoFieldList;

	class LIBQUTIM_EXPORT Buddy : public ChatUnit
	{
		Q_OBJECT
		Q_DECLARE_PRIVATE(Buddy)
		Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
		Q_PROPERTY(QString avatar READ avatar NOTIFY avatarChanged)
		Q_PROPERTY(qutim_sdk_0_3::Status status READ status NOTIFY statusChanged)
	public:
		Buddy(Account *account);
		Buddy(BuddyPrivate &d, Account *account);
		virtual ~Buddy();
		/*!
		  Path to buddy image
		*/
		virtual QString avatar() const;
		/**
		* @brief Returns contact's representable name
		*
		* @return Contact's name
		*/
		virtual QString title() const;
		virtual QString name() const;
		virtual Status status() const;
		/**
		* @brief send message to contact
		*
		* @param message Message, which to be sent to the recipient
		*/
		virtual void sendMessage(const Message &message) = 0;
		virtual void setName(const QString &name);
		/*!
		  Returnes list of protocol-specific info pairs for tooltip, i.e.:
		  \code
InfoFieldList list;
list.append(Field(QT_TRANSLATE_NOOP("Jabber", "Mood"),     tr("Angry")));
list.append(Field(QT_TRANSLATE_NOOP("Jabber", "Activity"), tr("Playing")));
list.append(Field(QT_TRANSLATE_NOOP("Protocol", "Idle"),   m_idle - QDateTime::currentDateTime().toTime_t())));
return list;
		  \endcode
		 */
		virtual InfoFieldList info(bool extra = false);
	signals:
		void avatarChanged(const QString &path);
		void statusChanged(const qutim_sdk_0_3::Status &status);
		void nameChanged(const QString &name);
	};
}

Q_DECLARE_METATYPE(qutim_sdk_0_3::Buddy*)

#endif // BUDDY_H
