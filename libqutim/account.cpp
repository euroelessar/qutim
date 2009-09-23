#include <QPointer>
#include "account.h"
#include "contact.h"
#include "protocol.h"

namespace qutim_sdk_0_3
{
	struct AccountPrivate
	{
		QPointer<Protocol> protocol;
		QString id;
		Status status;
	};

	Account::Account(const QString &id, Protocol *protocol) : QObject(protocol), p(new AccountPrivate)
	{
		p->protocol = protocol;
		p->id = id;
		p->status = Offline;
	}

	Account::~Account()
	{
	}

	QString Account::id()
	{
		return p->id;
	}

	Config Account::config()
	{
		return Config(p->protocol->id() + QLatin1Char('.') + p->id + QLatin1String("/account"));
	}

	ConfigGroup Account::config(const QString &group)
	{
		return config().group(group);
	}
		
	Status Account::status()
	{
		return p->status;
	}
		
	void Account::setStatus(Status status)
	{
		p->status = status;
	}
}
