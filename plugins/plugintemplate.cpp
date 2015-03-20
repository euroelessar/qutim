#include "${extensionHeader}"
${includeQmlTypes}
#include <qutim/plugin.h>

class ${productName}Plugin : public qutim_sdk_0_3::Plugin
{
	Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qutim.Plugin")
public:
	virtual void init()
	{
		qutim_sdk_0_3::ExtensionIcon icon(QLatin1String("${pluginIcon}"));
		qutim_sdk_0_3::LocalizedString name("Plugin", "${pluginName}");
		qutim_sdk_0_3::LocalizedString description("Plugin", "${pluginDescription}");
		setInfo(name, description, QUTIM_VERSION, icon);
		addExtension<${extensionClass}>(name, description, icon);
		${registerQmlTypes}
	}
	virtual bool load() { return true; }
	virtual bool unload() { return true; }
};

QUTIM_EXPORT_PLUGIN(${productName}Plugin)

#include <${productName}genplugin.moc>
