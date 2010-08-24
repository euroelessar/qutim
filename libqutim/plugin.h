/****************************************************************************
 *  plugin.h
 *
 *  Copyright (c) 2010 by Nigmatullin Ruslan <euroelessar@gmail.com>
 *
 ***************************************************************************
 *                                                                         *
 *   This library is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
*****************************************************************************/

#ifndef PLUGIN_H
#define PLUGIN_H

#include "extensioninfo.h"
#include <QtCore/QtPlugin>
#include <QtCore/QObject>
#include <QtCore/QSharedDataPointer>

#define PLUGIN_VERSION(major, minor, secminor, patch) QUTIM_VERSION_CHECK(major, minor, secminor, patch)

namespace qutim_sdk_0_3
{
	class ModuleManager;
	class PluginInfoData;
	struct PluginPrivate;

	class LIBQUTIM_EXPORT PluginInfo
	{
	public:
		PluginInfo(const LocalizedString &name = LocalizedString(),
				   const LocalizedString &description = LocalizedString(),
				   quint32 version = 0, ExtensionIcon icon = ExtensionIcon());
		PluginInfo(const PluginInfo &other);
		~PluginInfo();
		PluginInfo &operator =(const PluginInfo &other);
		PluginInfo &addAuthor(const PersonInfo &author);
		PluginInfo &addAuthor(const LocalizedString &name, const LocalizedString &task,
							  const QString &email = QString(), const QString &web = QString());
		PluginInfo &setName(const LocalizedString &name);
		PluginInfo &setDescription(const LocalizedString &description);
		PluginInfo &setIcon(const ExtensionIcon &icon);
		PluginInfo &setVersion(quint32 version);
		QList<PersonInfo> authors() const;
		LocalizedString name() const;
		LocalizedString description() const;
		quint32 version() const;
		ExtensionIcon icon() const;
#ifndef Q_QDOC
		QString fileName() const;
	private:
		QSharedDataPointer<PluginInfoData> d;
	public:
		typedef PluginInfoData Data;
		Data *data();
#endif
	};
	
	struct LIBQUTIM_EXPORT CommandArgumentDescription
	{
		QString name;
		QString parameter;
		QString description;
	};
	
	typedef QList<CommandArgumentDescription> CommandArgumentDescriptions;
	
	class LIBQUTIM_EXPORT CommandArgumentsHandler
	{
	public:
		virtual ~CommandArgumentsHandler() {}
		virtual CommandArgumentDescriptions descriptions() = 0;
		virtual void addArgument(const QString &arg) = 0;
	};

#ifndef Q_QDOC
	typedef void ( /*QObject::*/ *ModuleInit)();
#endif

	class LIBQUTIM_EXPORT Plugin : public QObject
	{
		Q_OBJECT
		Q_DISABLE_COPY(Plugin)
		Q_DECLARE_PRIVATE(Plugin)
	public:
		Plugin();
		virtual ~Plugin();
		PluginInfo info() const;
		ExtensionInfoList avaiableExtensions() const;
		virtual void init() = 0;
		virtual bool load() = 0;
		virtual bool unload() = 0;
	protected:
		// Should be called at init
		void addAuthor(const LocalizedString &name, const LocalizedString &task,
					   const QString &email = QString(), const QString &web = QString());
		void setInfo(const LocalizedString &name, const LocalizedString &description,
					 quint32 version = 0, ExtensionIcon icon = ExtensionIcon());
		void addExtension(const LocalizedString &name, const LocalizedString &description,
						  const ObjectGenerator *generator, ExtensionIcon icon = ExtensionIcon());
		template<typename T>
		void addExtension(const LocalizedString &name, const LocalizedString &description,
						  ExtensionIcon icon = ExtensionIcon())
		{ addExtension(name, description, new GeneralGenerator<T>(), icon); }
		template<typename T, typename I0>
		void addExtension(const LocalizedString &name, const LocalizedString &description,
						  ExtensionIcon icon = ExtensionIcon())
		{ addExtension(name, description, new GeneralGenerator<T, I0>(), icon); }
		template<typename T, typename I0, typename I1>
		void addExtension(const LocalizedString &name, const LocalizedString &description,
						  ExtensionIcon icon = ExtensionIcon())
		{ addExtension(name, description, new GeneralGenerator<T, I0, I1>(), icon); }
#ifndef Q_QDOC
	private:
		QScopedPointer<PluginPrivate> p;
		friend class ModuleManager;
#endif
	};
	
	class LIBQUTIM_EXPORT PluginFactory
	{
	public:
		virtual ~PluginFactory() {}
		// Will be called after ::load() method
		virtual QList<Plugin*> loadPlugins() = 0;
	};

	LIBQUTIM_EXPORT QList<QPointer<Plugin> > pluginsList();
}

#ifdef QUTIM_STATIC_PLUGIN
# define QUTIM_EXPORT_PLUGIN_HELPER(PluginInstance, Class) \
	QT_PREPEND_NAMESPACE(QObject) \
		*PluginInstance() \
	Q_PLUGIN_INSTANCE(Class)
# define QUTIM_EXPORT_PLUGIN(Class) \
	QUTIM_EXPORT_PLUGIN_HELPER(QUTIM_PLUGIN_INSTANCE, Class)
# define QUTIM_EXPORT_PLUGIN2(Plugin,Class) \
	QUTIM_EXPORT_PLUGIN(Class)
#else
# define QUTIM_EXPORT_PLUGIN2(Plugin,Class) \
	Q_EXPORT_PLUGIN2(Plugin,Class) \
	static const char *qutim_plugin_verification_data = \
	"pattern=""QUTIM_PLUGIN_VERIFICATION_DATA""\n" \
	"libqutim="QUTIM_VERSION_STR"\0"; \
	Q_EXTERN_C Q_DECL_EXPORT \
	const char * Q_STANDARD_CALL qutim_plugin_query_verification_data() \
	{ return qutim_plugin_verification_data; } \
	LIBQUTIM_NO_EXPORT qptrdiff qutim_plugin_id() \
	{ return reinterpret_cast<qptrdiff>(&Class::staticMetaObject); }
	
#define QUTIM_EXPORT_PLUGIN(Plugin) \
	QUTIM_EXPORT_PLUGIN2(Plugin, Plugin)
#endif		
Q_DECLARE_INTERFACE(qutim_sdk_0_3::PluginFactory, "org.qutim.PluginFactory")
Q_DECLARE_INTERFACE(qutim_sdk_0_3::CommandArgumentsHandler, "org.qutim.CommandArgumentsHandler")

#endif // PLUGIN_H
