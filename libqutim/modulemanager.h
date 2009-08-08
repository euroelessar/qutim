#ifndef MODULEMANAGER_H
#define MODULEMANAGER_H

#include "libqutim_global.h"
#include "plugin.h"
#include <QStringList>
#include <QMultiMap>

namespace qutim_sdk_0_3
{
	class ModuleManagerPrivate;

	class LIBQUTIM_EXPORT ModuleManager : public QObject
	{
		Q_OBJECT
	protected:
		ModuleManager(QObject *parent = 0);
		void loadPlugins(const QStringList &additional_paths = QStringList());
		QMultiMap<Plugin *, ExtensionInfo> getExtensions(const QMetaObject *service_meta) const;
		template<typename T>
		inline QMultiMap<Plugin *, ExtensionInfo> getExtensions()
		{ return getExtensions(&T::staticMetaObject); }
		virtual QList<ExtensionInfo> coreExtensions() const = 0;
		virtual void initExtensions();
		QObject *initExtension(const QMetaObject *service_meta);
		template<typename T>
		inline T *initExtension()
		{ return static_cast<T *>(initExtension(&T::staticMetaObject)); }
	private:
		static ModuleManager *self;
		ModuleManagerPrivate *p;
	};
}

#endif // MODULEMANAGER_H
