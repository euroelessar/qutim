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
#ifndef MODULEMANAGER_P_H
#define MODULEMANAGER_P_H

#include "modulemanager.h"
#include "protocol.h"
#include "../3rdparty/qtsolutions/qtlocalpeer.h"
#include <QSet>

namespace qutim_sdk_0_3
{
enum ModuleFlag
{
};
Q_DECLARE_FLAGS(ModuleFlags, ModuleFlag)

struct ExtensionNode
{
	QList<ExtensionNode*> children;
	ExtensionInfoList infos;
};

struct PluginData
{
	QWeakPointer<Plugin> plugin;
	PluginInfo info;
};

typedef QHash<QByteArray, ExtensionNode*> ExtensionNodeHash;

/**
  * ModuleManagerPrivate class used to hide inner structure of ModuleManager to provide binary compatibility between different versions.
  */
class ModuleManagerPrivate
{
public:
	inline ModuleManagerPrivate() :
			is_inited(false),
            isServicesInited(false)
	{}
    inline ~ModuleManagerPrivate() {}
	void initLocalPeer(const QString &message, bool *shouldExit);

    QList<QWeakPointer<Plugin> > plugins;
	QScopedPointer<QtLocalPeer> localPeer;
	bool is_inited;
	bool isServicesInited;
    ProtocolHash protocols;
    QHash<QString, QHash<QString, ModuleFlags> > choosed_modules;
	QHash<QByteArray, QObject *> services;
	QObjectList serviceOrder;
	QHash<QByteArray, ExtensionInfo> extensionsHash;
	QHash<QString, Plugin*> extsPlugins;
	ExtensionInfoList extensions;
	QSet<QByteArray> interface_modules;
	QSet<const QMetaObject *> meta_modules;
	QList<const ExtensionInfo> modules;
	ExtensionNodeHash nodes;
};

class LazyGenerator : public ObjectGenerator
{
public:
	LazyGenerator(PluginData *plugin, int id);

	virtual const QMetaObject *metaObject() const;
	virtual QList<QByteArray> interfaces() const;
protected:
	virtual QObject *generateHelper() const;
};

bool isCoreInited();
GeneratorList moduleGenerators(const QMetaObject *module, const char *iid);
ProtocolHash allProtocols();

}

#endif // MODULEMANAGER_P_H

