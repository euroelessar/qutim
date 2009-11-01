/****************************************************************************
 *  modulemanager.h
 *
 *  Copyright (c) 2009 by Nigmatullin Ruslan <euroelessar@gmail.com>
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

#ifndef MODULEMANAGER_H
#define MODULEMANAGER_H

#include "libqutim_global.h"
#include "plugin.h"
#include <QStringList>
#include <QMultiMap>
#include "event.h"

namespace qutim_sdk_0_3
{
	class ModuleManagerPrivate;
	typedef QList<ExtensionInfo> ExtensionList;

	class LIBQUTIM_EXPORT ModuleManager : public QObject
	{
		Q_OBJECT
	public:
		template<typename T, typename M>
		static void registerModule(ModuleInit method)
		{
//			ExtensionList list;
//			T::M(list);
		}
	protected:
		// Constructor
		ModuleManager(QObject *parent = 0);
		// Destructor
		virtual ~ModuleManager();

		// Fields
		QMultiMap<Plugin *, ExtensionInfo> getExtensions(const QMetaObject *service_meta) const;
		QMultiMap<Plugin *, ExtensionInfo> getExtensions(const char *interface_id) const;

		// Methods
		void loadPlugins(const QStringList &additional_paths = QStringList());
		QObject *initExtension(const QMetaObject *service_meta);

		// Virtual Methods
		virtual QList<ExtensionInfo> coreExtensions() const = 0;
		virtual void initExtensions();

		// Inline Methods
		template<typename T>
		inline QMultiMap<Plugin *, ExtensionInfo> getExtensions()
		{
			return getExtensions(&T::staticMetaObject);
		}

		template<typename T>
		inline T *initExtension()
		{
			return static_cast<T *>(initExtension(&T::staticMetaObject));
		}
	private:
		// Friend functions
		friend bool isCoreInited();
		friend GeneratorList moduleGenerators(const QMetaObject *);
		friend GeneratorList moduleGenerators(const char *);
		friend ProtocolMap allProtocols();

		// Static Fields
		static ModuleManager *self;
		ModuleManagerPrivate *p;
	};

	template<typename T, typename M>
	class ModuleRegisterHelper
	{
	public:
		inline ModuleRegisterHelper(M method)
		{
			ModuleManager::registerModule<T>(static_cast<ModuleInit>(method));
		}
	};
}

#endif // MODULEMANAGER_H
