#include "timemodifier.h"
#include "src/modulemanagerimpl.h"
#include <QDateTime>

namespace AdiumChat
{
	static Core::CoreInterfaceModuleHelper<TimeModifier, MessageModifier> chat_layer_static(
		QT_TRANSLATE_NOOP("Plugin", "Time modifier"),
		QT_TRANSLATE_NOOP("Plugin", "Modifier for %time% param, based on Apple's documentation")
	);

	TimeModifier::TimeModifier()
	{
		ConfigGroup adium_chat = Config("appearance").group("adiumChat/style");
		m_datetimeFormat = adium_chat.value<QString>("datetimeFormat","hh:mm:ss dd/MM/yyyy");
	}

	TimeModifier::~TimeModifier()
	{
	}

	QString TimeModifier::getValue(const ChatSession *session, const Message &message, const QString &name, const QString &value)
	{
		if(name == QLatin1String("time"))
		{
			QDateTime time = message.time();
			return convertTimeDate(value.isEmpty() ? m_datetimeFormat : value, time.isValid() ? time : QDateTime::currentDateTime());
		}
		return value;
	}

	QStringList TimeModifier::supportedNames() const
	{
		return QStringList() << QLatin1String("time");
	}
}
