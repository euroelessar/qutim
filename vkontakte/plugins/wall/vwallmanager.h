#ifndef VWALLMANAGER_H
#define VWALLMANAGER_H
#include <qutim/plugin.h>
#include <QHash>

class VWallSession;

namespace Vkontakte
{

using namespace qutim_sdk_0_3;
	
class VWallManager : public Plugin
{
    Q_OBJECT
public:
    virtual void init();
    virtual bool load();
    virtual bool unload();
private slots:
	void onWallDestroyed(QObject *wall);
	void onViewWallTriggered(QObject *obj);
private:
	QHash<QObject*, QHash<QString, VWallSession*> > m_sessions;
};

}

#endif // VWALLMANAGER_H
