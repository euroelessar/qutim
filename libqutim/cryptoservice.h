/****************************************************************************
 *  cryptoservice.h
 *
 *  Copyright (c) 2010 by Nigmatullin Ruslan <euroelessar@gmail.com>
 *
 ***************************************************************************
 *                                                                         *
 *   This library is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
*****************************************************************************/

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
	protected:
		virtual QVariant cryptImpl(const QVariant &value) const = 0;
		virtual QVariant decryptImpl(const QVariant &value) const = 0;
		virtual void setPassword(const QString &password) = 0;
		QVariant variantFromData(const QByteArray &data) const;
		QByteArray dataFromVariant(const QVariant &val) const;
		CryptoService();
		virtual ~CryptoService();
	};
}

#endif // CRYPTOSERVICE_H
