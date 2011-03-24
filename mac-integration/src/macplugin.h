#ifndef MACPLUGIN_H
#define MACPLUGIN_H

#include <qutim/plugin.h>

namespace MacIntegration
{
	using namespace qutim_sdk_0_3;

	class MacPlugin : public Plugin
	{
		Q_OBJECT
		Q_CLASSINFO("DebugName", "Mac")
		public:
			MacPlugin();
			void init();
			bool load();
			bool unload();
	};
} // MacIntegration namespace

#endif // MACPLUGIN_H
