#include "vkontakteprotocol.h"
#include <qutim/account.h>
#include "vaccount.h"

struct VkontakteProtocolPrivate
{
	inline VkontakteProtocolPrivate() :
		accounts_hash(new QHash<QString, QPointer<VAccount> > ())
	{ }
	inline ~VkontakteProtocolPrivate() { delete accounts_hash; }
	union
	{
		QHash<QString, QPointer<VAccount> > *accounts_hash;
		QHash<QString, VAccount *> *accounts;
	};
};

VkontakteProtocol *VkontakteProtocol::self = 0;

VkontakteProtocol::VkontakteProtocol() :
	d_ptr(new VkontakteProtocolPrivate)
{
	Q_ASSERT(!self);
	self = this;
}

VkontakteProtocol::~VkontakteProtocol()
{
	self = 0;
}

Account* VkontakteProtocol::account(const QString& id) const
{
	Q_D(const VkontakteProtocol);
	return d->accounts_hash->value(id);
}
QList< Account* > VkontakteProtocol::accounts() const
{
	Q_D(const VkontakteProtocol);
	QList<Account *> accounts;
	QHash<QString, QPointer<VAccount> >::const_iterator it;
	for (it = d->accounts_hash->begin(); it != d->accounts_hash->end(); it++)
		accounts.append(it.value());
	return accounts;

}
void VkontakteProtocol::loadAccounts()
{
	Q_D(VkontakteProtocol);
	QStringList accounts = config("general").value("accounts", QStringList());
	foreach(const QString &uid, accounts) {
		VAccount *acc = new VAccount(uid);
		d->accounts_hash->insert(uid, acc);
		//acc->updateSettings();
		emit accountCreated(acc);
	}
}
