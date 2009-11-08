#ifndef TIMEMODIFIER_H
#define TIMEMODIFIER_H

#include "../messagemodifier.h"

using namespace qutim_sdk_0_3;

namespace AdiumChat
{
	class TimeModifier : public QObject, public MessageModifier
	{
		Q_OBJECT
		Q_INTERFACES(AdiumChat::MessageModifier)
	public:
		TimeModifier();
		virtual ~TimeModifier();
		virtual QString getValue(const ChatSession *session, const Message &message, const QString &name, const QString &value);
		virtual QStringList supportedNames() const;
	private:
		QString m_datetimeFormat;
	};
}

#endif // TIMEMODIFIER_H
