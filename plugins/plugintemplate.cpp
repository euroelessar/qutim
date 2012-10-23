#include "${extensionHeader}"
#include <qutim/plugin.h>

class ${productName}Plugin : public Ureen::Plugin
{
	Q_OBJECT
public:
	virtual void init()
	{
		Ureen::ExtensionIcon icon(QLatin1String("${pluginIcon}"));
		Ureen::LocalizedString name("Plugin", "${pluginName}");
		Ureen::LocalizedString description("Plugin", "${pluginDescription}");
		setInfo(name, description, QUTIM_VERSION, icon);
		addExtension<${extensionClass}>(name, description, icon);
	}
	virtual bool load() { return true; }
	virtual bool unload() { return true; }
};

QUTIM_EXPORT_PLUGIN(${productName}Plugin)

#include <${productName}genplugin.moc>
