#ifndef WIN7INT_H
#define WIN7INT_H

#include <qutim/plugin.h>
#include <qutim/messagesession.h>
#include "links-openner.h"

const char *const WI_ConfigName = "win-integration";

#ifdef winint2_EXPORTS
#	define WININT_EXPORTS Q_DECL_EXPORT
#else
#	define WININT_EXPORTS Q_DECL_IMPORT
#endif

enum SubPluginsList {
	WI_Win7Taskbar = 0x0001
};

Q_DECLARE_FLAGS(SubPlugins, SubPluginsList)
Q_DECLARE_OPERATORS_FOR_FLAGS(SubPlugins)

namespace qutim_sdk_0_3
{
	class SettingsItem;
}

class WININT_EXPORTS WinIntegration : public qutim_sdk_0_3::Plugin
{
	Q_OBJECT
	Q_CLASSINFO("DebugName", "WinIntegration")
	Q_CLASSINFO("Uses",      "ChatLayer")
	Q_CLASSINFO("Uses",      "ContactList")

public:
	WinIntegration();
	void init();
	bool load();
	bool unload();
	static WinIntegration *instance() { return pluginInstance; }
	static QWidget *oneOfChatWindows();
	void enabledPlugin  (SubPlugins plugin);
	void disabledPlugin (SubPlugins plugin);
	bool isPluginEnabled(SubPlugins plugin);

signals:
	void unreadChanged(unsigned chats, unsigned confs);
	void reloadSettigs();

public slots:
	void onSessionCreated(qutim_sdk_0_3::ChatSession*);
	void onUnreadChanged(qutim_sdk_0_3::MessageList);
	void onSettingsSaved();
	void updateAssocs();

private:
	SubPlugins subPlugins_;
	void Win7SmallFeatures(bool);
	void VistaSmallFeatures(bool);
	void XpSmallFeatures(bool); // win2k && winXP
	static WinIntegration *pluginInstance;
	qutim_sdk_0_3::SettingsItem* settingsItem;
	LinksOpenner links;
};

#endif // WIN7INT_H
