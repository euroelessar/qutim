#ifndef ABSTRACTSEARCHREQUEST_H
#define ABSTRACTSEARCHREQUEST_H

#include <QVariant>
#include <QSharedData>
#include "localizedstring.h"
#include "dataforms.h"

namespace qutim_sdk_0_3
{
	class AbstractSearchFactoryPrivate;

	class LIBQUTIM_EXPORT AbstractSearchRequest : public QObject
	{
		Q_OBJECT
		Q_DISABLE_COPY(AbstractSearchRequest)
	public:
		AbstractSearchRequest();
		virtual ~AbstractSearchRequest();
		virtual DataItem fields() const = 0;
		virtual QSet<QString> services() const;
		virtual void setService(const QString &service);
		virtual void start(const DataItem &fields) = 0;
		virtual void cancel() = 0;
		virtual int actionCount() const = 0;
		virtual QVariant actionData(int index, int role = Qt::DisplayRole) = 0;
		virtual void actionActivated(int actionIndex, int row) = 0;
		virtual int columnCount() const = 0;
		virtual QVariant headerData(int column, int role = Qt::DisplayRole) = 0;
		virtual int rowCount() const = 0;
		virtual QVariant data(int row, int column, int role = Qt::DisplayRole) = 0;
	signals:
		void done(bool ok);
		void rowAboutToBeAdded(int row);
		void rowAdded(int row);
		void fieldsUpdated();
		void servicesUpdated();
		void actionsUpdated();
	};

	class LIBQUTIM_EXPORT AbstractSearchFactory : public QObject
	{
		Q_OBJECT
		Q_DISABLE_COPY(AbstractSearchFactory )
		Q_DECLARE_PRIVATE(AbstractSearchFactory )
	public:
		AbstractSearchFactory();
		virtual ~AbstractSearchFactory();
		virtual QStringList requestList() const = 0;
		virtual QVariant data(const QString &request, int role = Qt::DisplayRole);
		virtual AbstractSearchRequest *request(const QString &name) const = 0;
	signals:
		void requestAdded(const QString &request);
		void requestRemoved(const QString &request);
		void requestUpdated(const QString &request);
	protected:
		AbstractSearchFactory(AbstractSearchFactoryPrivate *d);
		QScopedPointer<AbstractSearchFactoryPrivate> d_ptr;
	};

} // namespace qutim_sdk_0_3

#endif // ABSTRACTSEARCHREQUEST_H
