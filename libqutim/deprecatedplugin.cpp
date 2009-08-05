#include "deprecatedplugin_p.h"
#include <qutim/plugininterface.h>

namespace qutim_sdk_0_3
{
	Plugin *createDeprecatedPlugin(QObject *object)
	{
		qutim_sdk_0_2::PluginInterface *plugin = qobject_cast<qutim_sdk_0_2::PluginInterface *>(object);
		if(plugin)
		{
	//			PluginInfo info = { plugin->name(), plugin->description(), plugin->type(), QIcon() };
			return new DeprecatedPlugin(object);
		}
		return 0;
	}

	DeprecatedPlugin::DeprecatedPlugin(QObject *object)
	{
		qutim_sdk_0_2::PluginInterface *plugin = qobject_cast<qutim_sdk_0_2::PluginInterface *>(object);
		m_name = plugin->name().toUtf8();
		m_info.name = m_name.constData();
		m_description = plugin->description().toUtf8();
		m_info.description = m_description.constData();
		m_info.icon = plugin->icon() ? *plugin->icon() : QIcon();
		m_info.version = PLUGIN_VERSION(0, 0, 0, 0);
	}
}
