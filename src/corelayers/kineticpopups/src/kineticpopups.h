#ifndef KINETICPOPUPS_H
#define KINETICPOPUPS_H
#include <qutim/plugin.h>

using namespace qutim_sdk_0_3;

namespace Core
{
namespace KineticPopups
{

class KineticPopupsPlugin : public Plugin
{
	Q_OBJECT
	Q_CLASSINFO("DebugName", "KineticPopups")
public:
	virtual void init();
	virtual bool load();
	virtual bool unload();
};

}
}

#endif // KINETICPOPUPS_H
