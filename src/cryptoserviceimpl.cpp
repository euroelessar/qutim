#include "cryptoserviceimpl.h"
#include <QCryptographicHash>

namespace Core
{
	CryptoServiceImpl::CryptoServiceImpl()
	{
		// We use AES-256, so we need vector with length 32
		m_iv = QByteArray::fromHex("c898e1c1771eb0bc4dc846d5edba0005"
								   "a54d2bb6f0d24fbfbb3c58a977edc50f"
								   "d8a9d9bddf0d2986de2c205fce21603e"
								   "1dbf510c810b9f5ef6bc324e01afb37c"
								   "0fc699e8579bd0f97cf3f5e45a307d76"
								   "4787c28376aeea9f207a53a34610b51f"
								   "f7ce86159cca2bdbde4511126bb02547"
								   "58537524340402b748977ad6f6c6b850");
		m_cipher_enc = 0;
		m_cipher_dec = 0;
	}

	QByteArray CryptoServiceImpl::cryptImpl(const QByteArray &value) const
	{
		if(!m_cipher_enc)
			return value;
		m_cipher_enc->clear();
		QCA::SecureArray arg = value;
		m_cipher_enc->update(arg);
		if(!m_cipher_enc->ok())
			return value;
		return m_cipher_enc->final().toByteArray();
	}

	QByteArray CryptoServiceImpl::decryptImpl(const QByteArray &value) const
	{
		if(!m_cipher_dec)
			return value;
		m_cipher_dec->clear();
		QCA::SecureArray arg = value;
		m_cipher_dec->update(arg);
		if(!m_cipher_dec->ok())
			return value;
		return m_cipher_dec->final().toByteArray();
	}

	void CryptoServiceImpl::setPassword(const QString &password)
	{
		QByteArray key = QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Sha1);
		// Hash results 160 bits, we need 96 more
		key += QByteArray::fromHex("cbc4b80d5c9da3494b19d13d29a1367a"
								   "ca2e86b4c8df4d2de0303d092107ef70"
								   "5fc06adbefbeaccaeeeba2787f3bac4b");
		m_key = key;
		m_cipher_enc = new QCA::Cipher(QString("aes256"),QCA::Cipher::CBC,
									   QCA::Cipher::DefaultPadding,
									   QCA::Encode, m_key, m_iv);
		m_cipher_dec = new QCA::Cipher(QString("aes256"),QCA::Cipher::CBC,
									   QCA::Cipher::DefaultPadding,
									   QCA::Decode, m_key, m_iv);
	}
}
