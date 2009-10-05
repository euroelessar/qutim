#ifndef LIBQUTIM_MESSAGE_H
#define LIBQUTIM_MESSAGE_H

#include "libqutim_global.h"
#include <QSharedData>
#include <QVariant>

namespace qutim_sdk_0_3
{
	class MessagePrivate;

	class Message
	{
	public:
		Message();
		Message(const QString &message);
		Message(const Message &other);
		virtual ~Message();
		const QString &message() const;
		void setMessage(const QString &message);
		const QDateTime &time() const;
		void setTime(const QDateTime &time);
		bool in() const;
		void setIn(bool input);
		QVariant property(const char *name) const;
		void setProperty(const char *name, const QVariant &value);
		QList<QByteArray> dynamicPropertyNames() const;
	private:
		QSharedDataPointer<MessagePrivate> p;
	};
}

#endif // LIBQUTIM_MESSAGE_H
