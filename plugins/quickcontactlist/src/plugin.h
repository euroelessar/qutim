#ifndef QUICKCONTACTLIST_PLUGIN_H
#define QUICKCONTACTLIST_PLUGIN_H

#include <qutim/plugin.h>

namespace QuickContactList {

class Plugin : public qutim_sdk_0_3::Plugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qutim.Plugin")
public:
    explicit Plugin();

    void init();
    bool load();
    bool unload();
};

} // namespace QuickContactList

#endif // QUICKCONTACTLIST_PLUGIN_H
