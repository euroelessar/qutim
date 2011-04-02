#ifndef TESTPLUGIN_H
#define TESTPLUGIN_H

#include <qutim/plugin.h>

namespace qutim_sdk_0_3
{
	class SettingsItem;
}

class TestPlugin : public qutim_sdk_0_3::Plugin
{
	Q_OBJECT
	Q_CLASSINFO("DebugName", "TestPlugin")

	qutim_sdk_0_3::SettingsItem *settingsitem;

public:
	bool load();
	bool unload();
	void init();
};

#endif
