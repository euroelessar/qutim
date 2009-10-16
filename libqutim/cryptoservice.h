/****************************************************************************
 *  cryptoservice.h
 *
 *  Copyright (c) 2009 by Nigmatullin Ruslan <euroelessar@gmail.com>
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
		static QByteArray crypt(const QVariant &value);
		static QVariant decrypt(const QByteArray &value);
	protected:
		virtual QByteArray cryptImpl(const QByteArray &value) const = 0;
		virtual QByteArray decryptImpl(const QByteArray &value) const = 0;
		virtual void setPassword(const QString &password) = 0;
		CryptoService();
		virtual ~CryptoService();
	private:
//		static void init(const QList<ExtensionInfo> &exts);
		static QPointer<CryptoService> self;
		friend class ModuleManager;
	};
}

#endif // CRYPTOSERVICE_H
