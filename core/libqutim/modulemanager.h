/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
**
*****************************************************************************
**
** $QUTIM_BEGIN_LICENSE$
** This program is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
** See the GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program.  If not, see http://www.gnu.org/licenses/.
** $QUTIM_END_LICENSE$
**
****************************************************************************/

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

/**
  * ModuleManager class used to manage plug-ins and extensions/
  * ModuleManagerImpl inherit this class and used to hide core layers and protect those.
  */
class LIBQUTIM_EXPORT ModuleManager : public QObject
{
	Q_OBJECT
protected:
	// Constructor
	ModuleManager(QObject *parent = 0);
	// Destructor
	virtual ~ModuleManager();
	
	ExtensionInfoList extensions(const char *interfaceId) const;
	ExtensionInfoList extensions(const QMetaObject *meta) const;

	// Methods
	void loadPlugins(const QStringList &additional_paths = QStringList());
	QObject *initExtension(const QMetaObject *service_meta);

	// Virtual Methods
	virtual ExtensionInfoList coreExtensions() const = 0;
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
protected slots:
	void onQuit();
    void _q_protocolDestroyed(QObject *obj); //TODO remove me
	void _q_messageReceived(const QString &);
protected:
	virtual void virtual_hook(int id, void *data);
private:
	friend LIBQUTIM_EXPORT GeneratorList moduleGenerators(const QMetaObject *);
    friend LIBQUTIM_EXPORT GeneratorList moduleGenerators(const char *);
};

//	LIBQUTIM_EXPORT void registerModule(const char *name, const char *description, const char *face, const QMetaObject *meta, int min = 0, int max = -1);
//	inline void registerModule(const char *name, const char *description, const char *face, int min = 0, int max = -1)
//	{ registerModule(name, description, face, NULL, min, max); }
//	inline void registerModule(const char *name, const char *description, const QMetaObject *meta, int min = 0, int max = -1)
//	{ registerModule(name, description, NULL, meta, min, max); }
//
//	template<typename T, int Min, int Max>
//	class ModuleHelper
//	{
//	public:
//		inline ModuleHelper(const char *name, const char *description)
//		{
//			registerModule(name, description, qobject_interface_iid<T *>(), meta_helper<T>(reinterpret_cast<T *>(0)), Min, Max);
//		}
//	private:
//		template <typename F>
//		inline const QMetaObject *meta_helper(const QObject *obj)
//		{ return &F::staticMetaObject; }
//		template <typename F>
//		inline const QMetaObject *meta_helper(const void *obj)
//		{ return NULL; }
//	};
//
//	template <typename T>
//	class SingleModuleHelper : public ModuleHelper<T, 1, 1>
//	{
//	public:
//		inline SingleModuleHelper(const char *name, const char *description) : ModuleHelper<T, 1, 1>(name, description) {}
//	};
//
//	template <typename T>
//	class MultiModuleHelper : public ModuleHelper<T, 0, -1>
//	{
//	public:
//		inline MultiModuleHelper(const char *name, const char *description) : ModuleHelper<T, 0, -1>(name, description) {}
//	};
}

#endif // MODULEMANAGER_H

