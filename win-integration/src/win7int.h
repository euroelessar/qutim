#ifndef WIN7INT_H
#define WIN7INT_H

#include <QList>
#include <qutim/messagesession.h>
#include <qutim/plugin.h>

#define WI_CONFIG "win-integration"

class WPreviews;

namespace qutim_sdk_0_3
{
	class SettingsItem;
}

class Win7Int2 : public qutim_sdk_0_3::Plugin
{
	Q_OBJECT
	Q_CLASSINFO("DebugName", "Win7Int2")
	Q_CLASSINFO("Service",   "Dock")
	Q_CLASSINFO("Uses",      "ChatLayer")
	Q_CLASSINFO("Uses",      "ContactList")

	QPixmap generateOverlayIcon(quint32 unreadConfs, quint32 unreadChats);
	QWidget   *chatWindow();
	void       testTab();
	QWidget   *previousWindow;
	unsigned   previousTabId;
	static Win7Int2  *pluginInstance;
	qutim_sdk_0_3::SettingsItem* settingsItem;
	WPreviews *previews;

	bool cfg_displayCount;
	bool cfg_addConfs;
	bool cfg_overlayIconEnabled;
	bool cfg_thumbsEnabled;

public:
	Win7Int2();
	void init();
	bool load();
	bool unload();
	static Win7Int2 *instance() { return pluginInstance; }

public slots:
	void onChatwidgetDestroyed();
	void onSessionActivated(bool);
	void onSessionCreated  (qutim_sdk_0_3::ChatSession*);
	void onUnreadChanged   (qutim_sdk_0_3::MessageList);
	void onMessageSmthDid  (qutim_sdk_0_3::Message*);
	void reloadSettings();

private:
};

#define UNREAD_TITLE_X  10
#define UNREAD_TITLE_Y  5
#define CHATUNREAD_X    5
#define CHATUNREAD_Y    18
#define CONFUNREAD_X    5
#define CONFUNREAD_Y    31
#define AUTHORS_TITLE_X 10
#define AUTHORS_TITLE_Y 49
#define AUTHORS_LIST_X  10
#define AUTHORS_LIST_Y  62
#define ICON_SIZE       64

#endif // WIN7INT_H
