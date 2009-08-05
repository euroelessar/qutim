#ifndef MODULEMANAGER_H
#define MODULEMANAGER_H

#include "libqutim_global.h"
#include <QObject>
#include <QStringList>

namespace qutim_sdk_0_3
{
	class ModuleManager : public QObject
	{
		Q_OBJECT
	protected:
		ModuleManager(QObject *parent = 0);
		void loadPlugins(const QStringList &additional_paths = QStringList());
	private:
		static ModuleManager *self;
	};
}

#endif // MODULEMANAGER_H
