/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
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

#ifndef CRYPTOSERVICE_H
#define CRYPTOSERVICE_H

#include "libqutim_global.h"
#include <QPointer>
#include <QVariant>

namespace qutim_sdk_0_3
{
	class ModuleManager;
	class ExtensionInfo;

	class LIBQUTIM_EXPORT CryptoService : public QObject
	{
		Q_OBJECT
	public:
		static QVariant crypt(const QVariant &value);
		static QVariant decrypt(const QVariant &value);
		virtual QVariant cryptImpl(const QVariant &value) const = 0;
		virtual QVariant decryptImpl(const QVariant &value) const = 0;
		virtual void setPassword(const QString &password, const QVariant &data) = 0;
		virtual QVariant generateData(const QString &profile) const = 0;
	protected:
		QVariant variantFromData(const QByteArray &data) const;
		QByteArray dataFromVariant(const QVariant &val) const;
	public:
		CryptoService();
		virtual ~CryptoService();
	};
}

#endif // CRYPTOSERVICE_H

