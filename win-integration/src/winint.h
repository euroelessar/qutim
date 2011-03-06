#ifndef WIN7INT_H
#define WIN7INT_H

#include <qutim/plugin.h>
#include <qutim/messagesession.h>

#define WI_CONFIG "win-integration"

#ifdef winint2_EXPORTS
#	define WININT_EXPORTS Q_DECL_EXPORT
#else
#	define WININT_EXPORTS Q_DECL_IMPORT
#endif

class WThumbnailsProvider;

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

	static WinIntegration  *pluginInstance;
	qutim_sdk_0_3::SettingsItem* settingsItem;

public:
	WinIntegration();
	void init();
	bool load();
	bool unload();
	static WinIntegration *instance() { return pluginInstance; }
	static QWidget *oneOfChatWindows();

signals:
	void unreadChanged(unsigned chats, unsigned confs);

public slots:
	void onSessionCreated(qutim_sdk_0_3::ChatSession*);
	void onUnreadChanged(qutim_sdk_0_3::MessageList);

private:
};

#endif // WIN7INT_H
