#ifndef MODULEMANAGER_P_H
#define MODULEMANAGER_P_H

#include "modulemanager.h"
#include "protocol.h"
#include <QSet>

namespace qutim_sdk_0_3
{
	enum ModuleFlag
	{
	};
	Q_DECLARE_FLAGS(ModuleFlags, ModuleFlag)

	/**
	 * ModuleManagerPrivate class used to hide inner structure of ModuleManager to provide binary compatibility between different versions.
	 */
	class ModuleManagerPrivate
	{
	public:
		inline ModuleManagerPrivate() : is_inited(false), protocols_hash(new QHash<QString, QPointer<Protocol> >()) {}
		inline ~ModuleManagerPrivate() { delete protocols_hash; }
		QList<QPointer<Plugin> > plugins;
		bool is_inited;
		union { // This union is intended to be used as reinterpret_cast =)
			QHash<QString, QPointer<Protocol> > *protocols_hash;
			QHash<QString, Protocol *> *protocols;
		};
		QHash<QString, QHash<QString, ModuleFlags> > choosed_modules;
		QHash<QByteArray, QObject *> services;
		QObjectList serviceOrder;
		QHash<QByteArray, ExtensionInfo> extensionsHash;
		QHash<QString, Plugin*> extsPlugins;
		ExtensionInfoList extensions;
		QSet<QByteArray> interface_modules;
		QSet<const QMetaObject *> meta_modules;
		QList<const ExtensionInfo> modules;
	};

	bool isCoreInited();
	QHash<QByteArray, QObject *> &services();
	GeneratorList moduleGenerators(const QMetaObject *module, const char *iid);
	ProtocolMap allProtocols();
}

#endif // MODULEMANAGER_P_H
