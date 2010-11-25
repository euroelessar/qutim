#ifndef JPLUGIN_H
#define JPLUGIN_H

#include <qutim/plugin.h>

namespace Jabber
{
using namespace qutim_sdk_0_3;

class JPlugin : public Plugin
{
	Q_OBJECT
	Q_CLASSINFO("DebugName", "Jabber")
public:
	JPlugin();
	void init();
	bool load();
	bool unload();
private slots:
};
} // Jabber namespace

#endif // JPLUGIN_H
