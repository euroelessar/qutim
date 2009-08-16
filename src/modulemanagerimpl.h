#ifndef MODULEMANAGERIMPL_H
#define MODULEMANAGERIMPL_H

#include "libqutim/modulemanager.h"

using namespace qutim_sdk_0_3;

namespace Core
{
	template<typename T>
	class CoreModuleHelper;

	class ModuleManagerImpl : public ModuleManager
	{
		Q_OBJECT
	public:
		ModuleManagerImpl();
		virtual QList<ExtensionInfo> coreExtensions() const;
		virtual void initExtensions();
	private:
		inline static QList<ExtensionInfo> &extensions()
		{
			static QList<ExtensionInfo> exts;
			return exts;
		}
		template<typename T> friend class CoreModuleHelper;
	};

	template<typename T>
	class CoreModuleHelper
	{
	public:
		inline CoreModuleHelper(const char *name, const char *description)
		{
			ModuleManagerImpl::extensions() << ExtensionInfo(name, description, &T::staticMetaObject);
		}
	};
}

#endif // MODULEMANAGERIMPL_H
