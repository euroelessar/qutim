/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright (C) 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
**
*****************************************************************************
**
** $QUTIM_BEGIN_LICENSE$
** This program is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
** See the GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program.  If not, see http://www.gnu.org/licenses/.
** $QUTIM_END_LICENSE$
**
****************************************************************************/
#include "aescryptoservice.h"
#include <QCryptographicHash>
#include <QtCrypto>

namespace AesCrypto
{

	AesCryptoService::AesCryptoService()
	{
		static QCA::Initializer qca_init;
//		qDebug() << QCA::Hash::supportedTypes();
//		qDebug() << QCA::Cipher::supportedTypes();
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
//		qDebug() << m_cipher_enc->blockSize() << m_cipher_enc->keyLength().minimum() << m_cipher_enc->keyLength().maximum();
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

	void AesCryptoService::setPassword(const QString &password, const QVariant &data)
	{
		Q_UNUSED(data);
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

	QVariant AesCryptoService::generateData(const QString &profile) const
	{
		Q_UNUSED(profile);
		return QVariant();
	}
}

