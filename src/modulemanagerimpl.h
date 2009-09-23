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
	private slots:
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
			ModuleManagerImpl::extensions() << ExtensionInfo(name, description, new GeneralGenerator<T>());
		}
	};
}

#endif // MODULEMANAGERIMPL_H
