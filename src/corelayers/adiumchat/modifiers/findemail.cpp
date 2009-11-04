#include "findemail.h"
#include <modulemanagerimpl.h>
#include <corelayers/adiumchat/chatlayerimpl.h> //FIXME

namespace AdiumChat
{
	static Core::CoreModuleHelper<ChatLayerImpl> chat_layer_static(
		QT_TRANSLATE_NOOP("Plugin", "Webkit chat layer"),
		QT_TRANSLATE_NOOP("Plugin", "Default qutIM chat realization, based on Adium chat styles")
	);
	
	QString FindEmail::getValue(const qutim_sdk_0_3::ChatSession* session, const qutim_sdk_0_3::Message& message, const QString& name, const QString& value)
	{
		return QString();
	}


	QStringList FindEmail::supportedNames() const
	{
		return QStringList();
	}

}