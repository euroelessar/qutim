#ifndef MODULEMANAGERIMPL_H
#define MODULEMANAGERIMPL_H

#include "libqutim/modulemanager.h"

using namespace qutim_sdk_0_3;

namespace Core
{
	template <typename T, typename I0 = void,
	typename I1 = void, typename I2 = void, typename I3 = void,
	typename I4 = void, typename I5 = void, typename I6 = void,
	typename I7 = void, typename I8 = void, typename I9 = void>
	class CoreModuleHelper;
	template <typename T, typename I0 = void,
	typename I1 = void, typename I2 = void, typename I3 = void,
	typename I4 = void, typename I5 = void, typename I6 = void,
	typename I7 = void, typename I8 = void, typename I9 = void>
	class CoreSingleModuleHelper;
	template <typename T, typename V>
	class CoreModuleHelper2;

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
		template <typename T, typename I0, typename I1, typename I2, typename I3,
		typename I4, typename I5, typename I6, typename I7, typename I8, typename I9>
		friend class CoreModuleHelper;
		template <typename T, typename I0, typename I1, typename I2, typename I3,
		typename I4, typename I5, typename I6, typename I7, typename I8, typename I9>
		friend class CoreSingleModuleHelper;
		template<typename T, typename V> friend class CoreModuleHelper2;
	};

	template <typename T, typename I0, typename I1, typename I2, typename I3,
	typename I4, typename I5, typename I6, typename I7, typename I8, typename I9>
	class CoreModuleHelper
	{
	public:
		inline CoreModuleHelper(const char *name, const char *description)
		{
			ModuleManagerImpl::extensions()
					<< ExtensionInfo(name, description,
									 new GeneralGenerator<T, I0, I1, I2, I3, I4, I5, I6, I7, I8, I9>());
		}
	};

	template <typename T, typename I0, typename I1, typename I2, typename I3,
	typename I4, typename I5, typename I6, typename I7, typename I8, typename I9>
	class CoreSingleModuleHelper
	{
	public:
		inline CoreSingleModuleHelper(const char *name, const char *description)
		{
			ModuleManagerImpl::extensions()
					<< ExtensionInfo(name, description,
									 new SingletonGenerator<T, I0, I1, I2, I3, I4, I5, I6, I7, I8, I9>());
		}
	};

	template <typename T, typename V>
	class CoreModuleHelper2
	{
	public:
		inline CoreModuleHelper2(const char *nameT, const char *descriptionT, const char *nameV, const char *descriptionV)
		{
			ModuleManagerImpl::extensions()
					<< ExtensionInfo(nameV, descriptionV, new GeneralGenerator<V>())
					<< ExtensionInfo(nameT, descriptionT, new GeneralGenerator<T>());
		}
	};
}

#endif // MODULEMANAGERIMPL_H
