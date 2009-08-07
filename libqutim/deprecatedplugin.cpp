#include "deprecatedplugin_p.h"
#include <qutim/plugininterface.h>

namespace qutim_sdk_0_3
{
	Plugin *createDeprecatedPlugin(QObject *object)
	{
		qutim_sdk_0_2::PluginInterface *plugin = qobject_cast<qutim_sdk_0_2::PluginInterface *>(object);
		if(plugin)
			return new DeprecatedPlugin(object);
		return 0;
	}

	DeprecatedPlugin::DeprecatedPlugin(QObject *object) : m_inited(false)
	{
		setParent(object);
		m_object = object;
	}

	void DeprecatedPlugin::init()
	{
		qutim_sdk_0_2::PluginInterface *plugin = qobject_cast<qutim_sdk_0_2::PluginInterface *>(m_object);
		if(!plugin->init(0))
			return;
		m_name = plugin->name().toUtf8();
		m_description = plugin->description().toUtf8();
		m_info.setName(m_name.constData());
		m_info.setDescription(m_description.constData());
		if(plugin->icon())
			m_info.setIcon(*plugin->icon());
		m_inited = true;
	}

	bool DeprecatedPlugin::load()
	{
		return m_inited;
	}

	bool DeprecatedPlugin::unload()
	{
		return false;
	}
}
