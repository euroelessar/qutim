#ifndef LOGGER_H
#define LOGGER_H

#include <qutim/plugin.h>
#include <qutim/settingslayer.h>

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
protected slots:
	void reloadSettings();
private:
	SettingsItem *m_settingsItem;
};

}
#endif // LOGGER_H
