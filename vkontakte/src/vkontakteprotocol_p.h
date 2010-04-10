#ifndef VKONTAKTEPROTOCOL_P_H
#define VKONTAKTEPROTOCOL_P_H
#include <QHash>
#include <QPointer>

class VAccount;
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

#endif // VKONTAKTEPROTOCOL_P_H
