#include "cryptoservice.h"
#include <QDataStream>

namespace qutim_sdk_0_3
{
	QPointer<CryptoService> CryptoService::self = NULL;

	CryptoService::CryptoService()
	{
	}

	CryptoService::~CryptoService()
	{
	}

	QByteArray CryptoService::crypt(const QVariant &value)
	{
		QByteArray result;
		QDataStream stream(&result, QIODevice::WriteOnly);
		value.save(stream);
		return self.isNull() ? result : self->cryptImpl(result);
	}

	QVariant CryptoService::decrypt(const QByteArray &value)
	{
		QVariant result;
		QDataStream stream(self.isNull() ? value : self->decryptImpl(value));
		result.load(stream);
		return result;
	}
}
