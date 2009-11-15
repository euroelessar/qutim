#ifndef KDEINTEGRATION_H
#define KDEINTEGRATION_H

#include <qutim/plugin.h>

using namespace qutim_sdk_0_3;

namespace KdeIntegration
{
	class KdePlugin : public Plugin
	{
	public:
		KdePlugin();
		virtual void init();
		virtual bool load();
		virtual bool unload();
	};
}

#endif // KDEINTEGRATION_H
