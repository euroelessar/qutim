#include "contactsearch.h"
#include "dynamicpropertydata_p.h"
#include <QSet>
#include "protocol.h"
#include "account.h"

namespace qutim_sdk_0_3
{
	class ContactSearchFactoryPrivate
	{
	};

	class GeneralContactSearchFactoryPrivate : public ContactSearchFactoryPrivate
	{
	public:
		QHash<QString, Account*> accounts;
		Protocol *protocol;
	};

	ContactSearchRequest::ContactSearchRequest()
	{
	}

	ContactSearchRequest::~ContactSearchRequest()
	{
	}

	QSet<QString> ContactSearchRequest::services() const
	{
		return QSet<QString>();
	}

	void ContactSearchRequest::setService(const QString &service)
	{
		Q_UNUSED(service);
	}

	ContactSearchFactory::ContactSearchFactory(ContactSearchFactoryPrivate *d) :
		d_ptr(d)
	{
	}

	ContactSearchFactory::ContactSearchFactory()
	{
	}

	ContactSearchFactory::~ContactSearchFactory()
	{
	}
	
	GeneralContactSearchFactory::GeneralContactSearchFactory(Protocol *protocol) :
		ContactSearchFactory(new GeneralContactSearchFactoryPrivate)
	{
		Q_D(GeneralContactSearchFactory);
		d->protocol = protocol;
		foreach (Account *account, protocol->accounts()) {
			d->accounts.insert(account->id(), account);
			connect(account, SIGNAL(destroyed()), SLOT(accountRemoved()));
		}
		connect(protocol, SIGNAL(accountCreated(qutim_sdk_0_3::Account*)), SLOT(accountAdded(qutim_sdk_0_3::Account*)));
	}
	
	GeneralContactSearchFactory::~GeneralContactSearchFactory()
	{
	}
	
	LocalizedStringList GeneralContactSearchFactory::requestList() const
	{
		Q_D(const GeneralContactSearchFactory);
		LocalizedStringList requests;
		QHashIterator<QString, Account*> itr(d->accounts);
		while (itr.hasNext()) {
			itr.next();
			requests << itr.key();
		}
		return requests;
	}
	
	Account *GeneralContactSearchFactory::account(const QString &requestName) const
	{
		return d_func()->accounts.value(requestName);
	}

	Protocol *GeneralContactSearchFactory::protocol() const
	{
		return d_func()->protocol;
	}
	
	void GeneralContactSearchFactory::accountAdded(qutim_sdk_0_3::Account *account)
	{
		Q_D(GeneralContactSearchFactory);
		d->accounts.insert(account->name(), account);
		connect(account, SIGNAL(destroyed()), SLOT(onAccountRemoved()));
		emit requestListUpdated();
	}
	
	void GeneralContactSearchFactory::accountRemoved()
	{
		Q_D(GeneralContactSearchFactory);
		Account *account = reinterpret_cast<Account*>(sender());
		QHash<QString, Account*>::iterator itr = d->accounts.begin();
		QHash<QString, Account*>::iterator endItr = d->accounts.end();
		while (itr != endItr) {
			if (*itr == account) {
				d->accounts.erase(itr);
				emit requestListUpdated();
				break;
			}
			++itr;
		}
	}
}
