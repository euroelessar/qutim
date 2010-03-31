#include "aescryptoservice.h"
#include "src/modulemanagerimpl.h"
#include <QCryptographicHash>

namespace Core
{
	static CoreModuleHelper<AesCryptoService> crypto_static(
			QT_TRANSLATE_NOOP("Plugin", "AES crypto"),
			QT_TRANSLATE_NOOP("Plugin", "Default qutIM crypto implementation. Based on algorithm aes256")
			);

	AesCryptoService::AesCryptoService()
	{
		// We use AES-256, so we need vector with length 32
		m_iv = QByteArray::fromHex("c898e1c1771eb0bc4dc846d5edba0005"
								   "a54d2bb6f0d24fbfbb3c58a977edc50f");
		m_cipher_enc = 0;
		m_cipher_dec = 0;
	}

	QVariant AesCryptoService::cryptImpl(const QVariant &valueVar) const
	{
		QByteArray value = dataFromVariant(valueVar);
		if(!m_cipher_enc)
			return value;
		QByteArray result;
		for(int i = 0x0; i < value.size(); i += 0xf)
		{
			m_cipher_enc->clear();
			QCA::SecureArray arg = value.mid(i, 0xf);
			m_cipher_enc->update(arg);
			if(!m_cipher_enc->ok())
				return result;
			result += m_cipher_enc->final().toByteArray();
		}
		return result;
	}

	QVariant AesCryptoService::decryptImpl(const QVariant &valueVar) const
	{
		if(!m_cipher_dec)
			return variantFromData(valueVar.toByteArray());
		QByteArray value = valueVar.toByteArray();
		QByteArray result;
		for(int i = 0x0; i < value.size(); i += 0x10)
		{
			m_cipher_dec->clear();
			QCA::SecureArray arg = value.mid(i, 0x10);
			m_cipher_dec->update(arg);
			if(!m_cipher_dec->ok())
				return result;
			result += m_cipher_dec->final().toByteArray();
		}
		return variantFromData(result);
	}

	void AesCryptoService::setPassword(const QString &password)
	{
		// Pass in utf-8
		QByteArray pass = password.toUtf8();
		// 64 bit sault
		pass += QByteArray::fromHex("5b225931d924bb30");
		m_key = QCA::Hash("sha256").hash(pass).toByteArray();
		m_cipher_enc = new QCA::Cipher(QString("aes256"),QCA::Cipher::CBC,
									   QCA::Cipher::DefaultPadding,
									   QCA::Encode, m_key, m_iv);
		m_cipher_dec = new QCA::Cipher(QString("aes256"),QCA::Cipher::CBC,
									   QCA::Cipher::DefaultPadding,
									   QCA::Decode, m_key, m_iv);
	}
}
