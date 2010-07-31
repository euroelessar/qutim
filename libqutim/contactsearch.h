#ifndef CONTACTSEARCH_H
#define CONTACTSEARCH_H

#include "abstractsearchrequest.h"

namespace qutim_sdk_0_3
{
	class Protocol;
	class Account;
	class Contact;
	class Status;
	class ContactSearchFactoryPrivate;
	class GeneralContactSearchFactoryPrivate;

	class LIBQUTIM_EXPORT ContactSearchRequest : public AbstractSearchRequest
	{
		Q_OBJECT
		Q_DISABLE_COPY(ContactSearchRequest)
	public:
		ContactSearchRequest();
		virtual ~ContactSearchRequest();
		virtual Contact *contact(int row) = 0;
		virtual int actionCount() const;
		virtual QVariant actionData(int index, int role = Qt::DisplayRole);
		virtual void actionActivated(int actionIndex, int row);
	};

	class LIBQUTIM_EXPORT ContactSearchFactory : public AbstractSearchFactory
	{
		Q_OBJECT
		Q_DECLARE_PRIVATE(ContactSearchFactory)
	public:
		ContactSearchFactory();
		virtual ~ContactSearchFactory();
	protected:
		ContactSearchFactory(ContactSearchFactoryPrivate *d);
	};

	class LIBQUTIM_EXPORT GeneralContactSearchFactory : public ContactSearchFactory
	{
		Q_OBJECT
		Q_DECLARE_PRIVATE(GeneralContactSearchFactory)
	public:
		GeneralContactSearchFactory(Protocol *protocol);
		virtual ~GeneralContactSearchFactory();
		virtual QStringList requestList() const;
		virtual QVariant data(const QString &request, int role = Qt::DisplayRole);
		Account *account(const QString &name) const;
		Protocol *protocol() const;
	private slots:
		void accountAdded(qutim_sdk_0_3::Account *account);
		void accountRemoved();
		void accountStatusChanged(const qutim_sdk_0_3::Status &status);
	};
}

#endif // CONTACTSEARCH_H
