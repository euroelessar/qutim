#ifndef CONTACT_H
#define CONTACT_H

#include "libqutim_global.h"
#include <QSet>

namespace qutim_sdk_0_3
{
	class Account;
	class MetaContact;
	class Message;

	class Contact : public QObject
	{
		Q_OBJECT
	public:
		enum Type { Meta, Simple, Sub };
		Contact(Account *account);
		Type type() const { return m_type; }
		const QSet<QString> &tags() const { return m_tags; }
		virtual QString id() const = 0;
		virtual QString name() const;
		Account *account() { return m_account; }
		const Account *account() const { return m_account; }
		MetaContact *metaContact() { return m_metacontact; }
		const MetaContact *metaContact() const { return m_metacontact; }
		virtual void sendMessage(const Message &message) = 0;
	protected:
		Type m_type;
		QSet<QString> m_tags;
		Account *m_account;
		MetaContact *m_metacontact;
	};
}

#endif // CONTACT_H
