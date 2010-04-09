#ifndef VKONTAKTEPLUGIN_H
#define VKONTAKTEPLUGIN_H
#include <qutim/plugin.h>
#include "vkontakte_global.h"

class VkontaktePlugin : public Plugin
{
	Q_OBJECT
public:
	virtual void init();
	virtual bool load();
	virtual bool unload();
};

#endif // VKONTAKTEPLUGIN_H
