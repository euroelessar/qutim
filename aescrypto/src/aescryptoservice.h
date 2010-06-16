#ifndef CRYPTOSERVICEIMPL_H
#define CRYPTOSERVICEIMPL_H

#include <qutim/cryptoservice.h>
#include <QtCrypto>

using namespace qutim_sdk_0_3;

namespace AesCrypto
{
	class AesCryptoService : public CryptoService
	{
		Q_OBJECT
	public:
		AesCryptoService();
	protected:
		virtual QVariant cryptImpl(const QVariant &value) const;
		virtual QVariant decryptImpl(const QVariant &value) const;
		virtual void setPassword(const QString &password, const QVariant &data);
		virtual QVariant generateData(const QString &profile) const;
	private:
		QCA::SymmetricKey m_key;
		QCA::InitializationVector m_iv;
		QCA::Cipher *m_cipher_enc;
		QCA::Cipher *m_cipher_dec;
	};
}

#endif // CRYPTOSERVICEIMPL_H
