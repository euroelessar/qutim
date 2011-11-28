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

