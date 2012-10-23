#ifndef DOCKPLUGIN_H
#define DOCKPLUGIN_H
#include <qutim/plugin.h>
#include <QPointer>
#include "docktile.h"

class DockPlugin : public Ureen::Plugin
{
public:
	virtual void init();
	virtual bool load();
	virtual bool unload();
private:
	QPointer<DockTile> m_dockTile;
};

#endif // DOCKPLUGIN_H
