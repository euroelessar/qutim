/****************************************************************************
 *  cryptoservice.cpp
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

#include "cryptoservice.h"

#ifdef INSIDE_MODULE_MANAGER
#undef INSIDE_MODULE_MANAGER

#include "modulemanager.h"
#include <QDataStream>
#include <QBuffer>
#include <QCoreApplication>

namespace qutim_sdk_0_3
{
	// is must be named as a lot of another variables
	static QPointer<CryptoService> self = NULL;

	CryptoService::CryptoService()
	{
		if (self) {
			qFatal("Double initialization of CryptoService, aborting");
			// ensure
			qApp->exit(1); //goodbye cruel world
		}
		self = this;
	}

	CryptoService::~CryptoService()
	{
		self = 0;
	}

	QVariant CryptoService::crypt(const QVariant &value)
	{
		return self.isNull() ? value : self->cryptImpl(value);
	}

	QVariant CryptoService::decrypt(const QVariant &value)
	{
		return self.isNull() ? value : self->decryptImpl(value);
	}

	QVariant CryptoService::variantFromData(const QByteArray &data) const
	{
		QVariant result;
		QByteArray tmp = data;
		{
			QBuffer buffer(&tmp);
			buffer.open(QIODevice::ReadOnly);
			QDataStream stream(&buffer);
			stream.setVersion(QDataStream::Qt_4_5);
			quint32 type;
			stream >> type;
			if(type == QVariant::String)
			{
				QByteArray tmp2;
				stream >> tmp2;
				result = QString::fromUtf8(tmp2, tmp2.size());
			}
			else
			{
				buffer.seek(0);
				result.load(stream);
			}
		}
		return result;
	}

	QByteArray CryptoService::dataFromVariant(const QVariant &value) const
	{
		QByteArray result;
		{
			QDataStream stream(&result, QIODevice::WriteOnly);
			stream.setVersion(QDataStream::Qt_4_5);
			if(value.type() == QVariant::String)
			{
				stream << quint32(QVariant::String);
				stream << value.toString().toUtf8();
			}
			else
				value.save(stream);
		}
		return result;
	}
}

#endif // INSIDE_MODULE_MANAGER
