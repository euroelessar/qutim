#include "icq_global.h"
#include "icqprotocol.h"
#include "icqaccount.h"
#include <QStringList>
#include <QPointer>
#include <QDebug>

IcqProtocol *IcqProtocol::self = 0;

struct IcqProtocolPrivate
{
	inline IcqProtocolPrivate() : accounts_hash(new QHash<QString, QPointer<IcqAccount> >()) {}
	inline ~IcqProtocolPrivate() { delete accounts_hash; }
	union {
		QHash<QString, QPointer<IcqAccount> > *accounts_hash;
		QHash<QString, IcqAccount *> *accounts;
	};
};

qutim_sdk_0_3::Status icqStatusToQutim(quint16 status)
{
	if(status & IcqFlagInvisible)
		return Invisible;
	else if(status & IcqFlagEvil)
		return Evil;
	else if(status & IcqFlagDepress)
		return Depression;
	else if(status & IcqFlagHome)
		return AtHome;
	else if(status & IcqFlagWork)
		return AtWork;
	else if(status & IcqFlagLunch)
		return OutToLunch;
	else if(status & IcqFlagDND)
		return DND;
	else if(status & IcqFlagOccupied)
		return Occupied;
	else if(status & IcqFlagNA)
		return NA;
	else if(status & IcqFlagAway)
		return Away;
	else if(status & IcqFlagFFC)
		return FreeChat;
	return Online;
}

IcqProtocol::IcqProtocol() : p(new IcqProtocolPrivate)
{
	Q_ASSERT(!self);
	self = this;
}

IcqProtocol::~IcqProtocol()
{
	self = 0;
}

void IcqProtocol::loadAccounts()
{
	QStringList accounts = config("general").value("accounts", QStringList());
	foreach(const QString &uin, accounts)
		p->accounts_hash->insert(uin, new IcqAccount(uin));
}
