#ifndef CORE_CONTACTCOMPARATORS_H
#define CORE_CONTACTCOMPARATORS_H

#include <qutim/plugin.h>

namespace Core {

class ContactComparators : public qutim_sdk_0_3::Plugin
{
	Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qutim.Plugin")
	Q_CLASSINFO("DebugName", "ContactComparators")
public:
	virtual void init();
	virtual bool load();
	virtual bool unload();
};

} // namespace Core

#endif // CORE_CONTACTCOMPARATORS_H
