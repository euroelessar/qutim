#include "nocryptoservice.h"

namespace Core
{
	NoCryptoService::NoCryptoService()
	{
	}

	NoCryptoService::~NoCryptoService()
	{
	}

	QVariant NoCryptoService::cryptImpl(const QVariant &value) const
	{
		return value;
	}

	QVariant NoCryptoService::decryptImpl(const QVariant &value) const
	{
		return value;
	}

	void NoCryptoService::setPassword(const QString &password, const QVariant &data)
	{
		Q_UNUSED(password);
		Q_UNUSED(data);
	}

	QVariant NoCryptoService::generateData(const QString &profile) const
	{
		Q_UNUSED(profile);
		return QVariant();
	}
}
