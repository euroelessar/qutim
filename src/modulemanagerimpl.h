#ifndef MODULEMANAGERIMPL_H
#define MODULEMANAGERIMPL_H

#include "libqutim/modulemanager.h"

using namespace qutim_sdk_0_3;

namespace Core
{
	class ModuleManagerImpl : public ModuleManager
	{
		Q_OBJECT
	public:
		ModuleManagerImpl();
		virtual ExtensionInfoList coreExtensions() const;
	private slots:
		virtual void initExtensions();
	private:
		inline static ExtensionInfoList &extensions()
		{
			static ExtensionInfoList exts;
			return exts;
		}
	};
}

#endif // MODULEMANAGERIMPL_H
