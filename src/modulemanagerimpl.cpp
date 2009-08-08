#include "modulemanagerimpl.h"

namespace Core
{
	QList<ExtensionInfo> ModuleManagerImpl::extensions = QList<ExtensionInfo>();

	ModuleManagerImpl::ModuleManagerImpl()
	{
	}

	QList<ExtensionInfo> ModuleManagerImpl::coreExtensions() const
	{
		return extensions;
	}

	void ModuleManagerImpl::initExtensions()
	{
		ModuleManager::initExtensions();
	}
}
