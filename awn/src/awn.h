#ifndef AWN
#define AWN

#include <qutim/plugin.h>

class AWNPlugin : public qutim_sdk_0_3::Plugin
{
	Q_OBJECT
public:
	explicit AWNPlugin ();
	virtual void init();
	virtual bool load();
	virtual bool unload();
};

#endif //AWN

