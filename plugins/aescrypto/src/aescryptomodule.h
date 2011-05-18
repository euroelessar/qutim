#ifndef AESCRYPTOMODULE_H
#define AESCRYPTOMODULE_H

#include <qutim/plugin.h>

namespace AesCrypto
{
	using namespace qutim_sdk_0_3;

	class AesCryptoModule : public Plugin
	{
		Q_OBJECT
	public:
		virtual void init();
		virtual bool load();
		virtual bool unload();
	};

}
#endif // AESCRYPTOMODULE_H
