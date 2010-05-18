#ifndef CONTACTSEARCH_H
#define CONTACTSEARCH_H

#include <QVariant>
#include <QSharedData>
#include "localizedstring.h"
#include "dataforms.h"

namespace qutim_sdk_0_3
{
	class Protocol;
	class Account;
	class Contact;
	class ContactSearchFactoryPrivate;
	class GeneralContactSearchFactoryPrivate;

	class LIBQUTIM_EXPORT ContactSearchRequest : public QObject
	{
		Q_OBJECT
		Q_DISABLE_COPY(ContactSearchRequest)
	public:
		ContactSearchRequest();
		virtual ~ContactSearchRequest();
		virtual DataItem fields() const = 0;
		virtual QSet<QString> services() const;
		virtual void setService(const QString &service);
		virtual void start(const DataItem &fields) = 0;
		virtual void cancel() = 0;
		virtual int columnCount() const = 0;
		virtual QVariant headerData(int column, int role = Qt::DisplayRole) = 0;
		virtual int contactCount() const = 0;
		virtual QVariant data(int row, int column, int role = Qt::DisplayRole) = 0;
		virtual Contact *contact(int row) = 0;
	signals:
		void done(bool ok);
		void contactAboutToBeAdded(int row);
		void contactAdded(int row);
		void fieldsUpdated();
		void servicesUpdated();
	};

	class LIBQUTIM_EXPORT ContactSearchFactory : public QObject
	{
		Q_OBJECT
		Q_DISABLE_COPY(ContactSearchFactory)
		Q_DECLARE_PRIVATE(ContactSearchFactory)
	public:
		ContactSearchFactory();
		virtual ~ContactSearchFactory();
		virtual LocalizedStringList requestList() const = 0;
		virtual ContactSearchRequest *request(const QString &name) const = 0;
	signals:
		void requestListUpdated();
	protected:
		ContactSearchFactory(ContactSearchFactoryPrivate *d);
		QScopedPointer<ContactSearchFactoryPrivate> d_ptr;
	};

	class LIBQUTIM_EXPORT GeneralContactSearchFactory : public ContactSearchFactory
	{
		Q_OBJECT
		Q_DECLARE_PRIVATE(GeneralContactSearchFactory)
	public:
		GeneralContactSearchFactory(Protocol *protocol);
		virtual ~GeneralContactSearchFactory();
		virtual LocalizedStringList requestList() const;
		Account *account(const QString &requestName) const;
		Protocol *protocol() const;
	private slots:
		void accountAdded(qutim_sdk_0_3::Account *account);
		void accountRemoved();
	};
}

#endif // CONTACTSEARCH_H
