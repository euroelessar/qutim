#include "cryptoservice.h"
#include <QDataStream>
#include <QBuffer>

namespace qutim_sdk_0_3
{
	QPointer<CryptoService> CryptoService::self = NULL;

	CryptoService::CryptoService()
	{
	}

	CryptoService::~CryptoService()
	{
	}

	QByteArray CryptoService::crypt(const QVariant &value)
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
		return self.isNull() ? result : self->cryptImpl(result);
	}

	QVariant CryptoService::decrypt(const QByteArray &value)
	{
		QVariant result;
		QByteArray tmp = self.isNull() ? value : self->decryptImpl(value);
		{
			QBuffer buffer(&tmp);
			buffer.open(QIODevice::ReadOnly);
			QDataStream stream(&buffer);
			stream.setVersion(QDataStream::Qt_4_5);
			quint32 type;
			stream >> type;
			if(type == QVariant::String)
			{
				stream >> tmp;
				result = QString::fromUtf8(tmp, tmp.size());
			}
			else
			{
				buffer.seek(0);
				result.load(stream);
			}
		}
		return result;
	}
}
