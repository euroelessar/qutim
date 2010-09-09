#ifndef MPLUGIN_H
#define MPLUGIN_H

#include <qutim/plugin.h>

namespace MacIntegration
{
	using namespace qutim_sdk_0_3;

	class MPlugin : public Plugin
	{
		Q_OBJECT
		Q_CLASSINFO("DebugName", "Mac")
		public:
			MPlugin();
			void init();
			bool load();
			bool unload();
	};
} // MacIntegration namespace

#endif // MPLUGIN_H
