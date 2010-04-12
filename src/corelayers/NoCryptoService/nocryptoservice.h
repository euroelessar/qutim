#ifndef NOCRYPTOSERVICE_H
#define NOCRYPTOSERVICE_H

#include "libqutim/cryptoservice.h"

using namespace qutim_sdk_0_3;

namespace Core
{
	class NoCryptoService : public CryptoService
	{
		Q_OBJECT
	public:
		NoCryptoService();
		~NoCryptoService();
		virtual QVariant cryptImpl(const QVariant &value) const;
		virtual QVariant decryptImpl(const QVariant &value) const;
		virtual void setPassword(const QString &password, const QVariant &data);
		virtual QVariant generateData(const QString &profile) const;
	};
}

#endif // NOCRYPTOSERVICE_H
