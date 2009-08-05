#ifndef PLUGIN_H
#define PLUGIN_H

#include "libqutim_global.h"
#include <QObject>
#include <QIcon>

#define PLUGIN_VERSION(major, minor, secminor, patch) ((major<<24)|(minor<<16)|(secminor<<8)|(patch))

namespace qutim_sdk_0_3
{
	struct AuthorInfo
	{
		// Full name, insert by QT_TRANSLATE_NOOP("Plugin", "Ivan Sidoroff")
		const char *name;
		// Devel task, insert by QT_TRANSLATE_NOOP("Plugin", "Plugin author")
		const char *task;
		// E-mail address
		QString email_address;
		// Web address, if exists
		QString web_address;
	};

	struct PluginInfo
	{
		// Authors
		QList<AuthorInfo> authors;
		// Name, insert by QT_TRANSLATE_NOOP("Plugin", "My cool plugin")
		const char *name;
		// Html description, insert by QT_TRANSLATE_NOOP("Plugin", "This is my first plugin")
		const char *description;
		// Can be accessed by Icon( name, Icon::Plugin );
		QIcon icon;
		// Format is like QT_VERSION, but contains 4 numbers, i.e. 0x00010405 for version 0.1.4.5
		quint32 version;
	};

	struct ExtensionInfo
	{
		// Extension icon, if null core would try to detect it by extension type
		QIcon icon;
		// Name, insert by QT_TRANSLATE_NOOP("Plugin", "Kickoff settings layer")
		const char *name;
		// Html description, insert by QT_TRANSLATE_NOOP("Plugin", "Make kickoff-like settings window")
		const char *description;
		// Meta object of Extension, i.e. &KickoffSettingsLayer::staticMetaObject();
		QMetaObject *meta;
	};

	class Plugin : public QObject
	{
		Q_OBJECT
	public:
		Plugin();
		inline const PluginInfo &info() const { return m_info; }
		const QList<ExtensionInfo> &avaiableExtensions() const { return m_extensions; }
		virtual void init();
		virtual bool load();
		virtual bool unload();
	protected:
		// Should be filled at init
		PluginInfo m_info;
		QList<ExtensionInfo> m_extensions;
	};
}

#endif // PLUGIN_H
