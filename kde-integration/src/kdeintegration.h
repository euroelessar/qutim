#ifndef KDEINTEGRATION_H
#define KDEINTEGRATION_H

#include <qutim/plugin.h>
#include <khelpmenu.h>

using namespace qutim_sdk_0_3;

namespace KdeIntegration
{
class KdePlugin : public Plugin
{
	Q_OBJECT
public:
	KdePlugin();
	virtual void init();
	virtual bool load();
	virtual bool unload();
	virtual bool eventFilter(QObject *, QEvent *);

	static KHelpMenu *helpMenu();
private:
	quint16 m_quetzal_id;
};
}

#endif // KDEINTEGRATION_H
