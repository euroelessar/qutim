#ifndef MESSAGEMODIFIER_H
#define MESSAGEMODIFIER_H

#include <QObject>

namespace AdiumChat
{
	class Session;
	class Message;
	class MessageModifier : public QObject
	{
		Q_OBJECT
	public:
		virtual QString getValue(const Session *session, const Message &message, const QString &name, const QString &value) = 0;
		virtual QStringList supportedNames() const = 0;
	};

}
#endif // MESSAGEMODIFIER_H
