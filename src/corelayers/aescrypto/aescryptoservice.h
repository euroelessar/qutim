#ifndef CRYPTOSERVICEIMPL_H
#define CRYPTOSERVICEIMPL_H

#include "libqutim/cryptoservice.h"
#include <QtCrypto>

using namespace qutim_sdk_0_3;

namespace Core
{
	class AesCryptoService : public CryptoService
	{
		Q_OBJECT
	public:
		AesCryptoService();
	protected:
		virtual QVariant cryptImpl(const QVariant &value) const;
		virtual QVariant decryptImpl(const QVariant &value) const;
		virtual void setPassword(const QString &password);
	private:
		QCA::SymmetricKey m_key;
		QCA::InitializationVector m_iv;
		QCA::Cipher *m_cipher_enc;
		QCA::Cipher *m_cipher_dec;
	};
}
using namespace Core;

#endif // CRYPTOSERVICEIMPL_H
