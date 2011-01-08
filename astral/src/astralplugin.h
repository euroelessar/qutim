#ifndef ASTRALPLUGIN_H
#define ASTRALPLUGIN_H

#include <qutim/plugin.h>
#include "astralprotocol.h"

using namespace qutim_sdk_0_3;

class AstralPlugin : public Plugin
{
	Q_OBJECT
public:
	AstralPlugin();
	virtual void init();
	virtual bool load();
	virtual bool unload();
protected:
	QList<ConnectionManagerPtr> listConnectionManagers();
};

#endif // ASTRALPLUGIN_H
