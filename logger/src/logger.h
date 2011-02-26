#ifndef LOGGER_H
#define LOGGER_H

#include <qutim/plugin.h>

namespace Logger
{

using namespace qutim_sdk_0_3;

class LoggerPlugin : public Plugin
{
	Q_OBJECT
	Q_CLASSINFO("DebugName", "Logger")
public:
	virtual void init();
	virtual bool load();
	virtual bool unload();
};

}
#endif // LOGGER_H
