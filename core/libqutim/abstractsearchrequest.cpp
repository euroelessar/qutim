#include "abstractsearchrequest_p.h"
#include <QSet>

namespace qutim_sdk_0_3
{
	AbstractSearchRequest::AbstractSearchRequest()
	{
	}

	AbstractSearchRequest::~AbstractSearchRequest()
	{
	}

	QSet<QString> AbstractSearchRequest::services() const
	{
		return QSet<QString>();
	}

	void AbstractSearchRequest::setService(const QString &service)
	{
		Q_UNUSED(service);
	}

	AbstractSearchFactory::AbstractSearchFactory(AbstractSearchFactoryPrivate *d) :
		d_ptr(d)
	{
	}

	AbstractSearchFactory::AbstractSearchFactory()
	{
	}

	AbstractSearchFactory::~AbstractSearchFactory()
	{
	}

	QVariant AbstractSearchFactory::data(const QString &request, int role)
	{
		if (role == Qt::DisplayRole)
			return request;
		return QVariant();
	}

}
