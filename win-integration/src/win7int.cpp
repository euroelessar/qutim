#include "win7int.h"
#include "thumbnails.h"
#include "wsettings.h"
#include <QDir>
#include <QImage>
#include <QImage>
#include <QMap>
#include <QPainter>
#include <QTimer>
#include <QSysInfo>
#include <qutim/conference.h>
#include <qutim/configbase.h>
#include <qutim/debug.h>
#include <qutim/extensionicon.h>
#include <qutim/icon.h>
#include <qutim/servicemanager.h>
#include <qutim/settingslayer.h>
#include <qt_windows.h>
#include <WinThings/OverlayIcon.h>

using namespace qutim_sdk_0_3;

Win7Int2 *Win7Int2::pluginInstance;

Win7Int2::Win7Int2()
{
	pluginInstance = this;
	previousWindow = 0;
	previousTabId  = 0;
	previews       = 0;
#ifndef QT_NO_DEBUG
	// being run from Explorer
	HWND console = FindWindowW(L"ConsoleWindowClass", QDir::toNativeSeparators(qApp->applicationFilePath()).toStdWString().data());
	if (!console) // being run from taskbar (pinned icon), assuming default link name (qutim); function is not case-sensitive
		console = FindWindowW(L"ConsoleWindowClass", L"qutim");
	if (console)
		ShowWindow(console, SW_HIDE);
#endif
}

void Win7Int2::init()
{
	addAuthor(QT_TRANSLATE_NOOP("Author", "Vizir Ivan"),
		QT_TRANSLATE_NOOP("Task", "Author"),
		QLatin1String("define-true-false@yandex.com"));
	setInfo(QT_TRANSLATE_NOOP("Plugin", "Windows 7 Integration"),
		QT_TRANSLATE_NOOP("Plugin", "Adds count of unread messages as an icon to taskbar button of qutim, along with some commands list."),
		PLUGIN_VERSION(2, 0, 0, 9999),
		ExtensionIcon());
}

bool Win7Int2::load()
{
	if (QSysInfo::windowsVersion() != QSysInfo::WV_WINDOWS7)
		return false; // even if plugin suddenly loads on WinVista, it won't work
	connect(ChatLayer::instance(), SIGNAL(sessionCreated(qutim_sdk_0_3::ChatSession*)), SLOT(onSessionCreated(qutim_sdk_0_3::ChatSession*)));
	if ((cfg_thumbsEnabled = Config(WI_CONFIG).value("tt_enabled", true))) // so cannot be changed in runtime
		previews = new WPreviews;
	QWidget *cl = ServiceManager::getByName("ContactList")->property("widget").value<QWidget*>();
	TaskbarPreviews::setWindowAttributes(cl, TA_Peek_ExcludeFrom|TA_Flip3D_ExcludeBelow);
	settingsItem = new GeneralSettingsItem<WSettingsWidget>(
		Settings::Plugin,	QIcon(),
		QT_TRANSLATE_NOOP("Plugin", "Windows 7 Integration"));
	Settings::registerItem(settingsItem);
	reloadSettings();
	return true;
}

bool Win7Int2::unload()
{
	QWidget *cl = ServiceManager::getByName("ContactList")->property("widget").value<QWidget*>();
	TaskbarPreviews::setWindowAttributes(cl, TA_NoAttributes);
	if (previews)
		delete previews;
	Settings::removeItem(settingsItem);
	return true;
}

void Win7Int2::onSessionCreated(qutim_sdk_0_3::ChatSession *s)
{
	if (previews)
		connect(s, SIGNAL(unreadChanged(qutim_sdk_0_3::MessageList)), this->previews, SLOT(onUnreadChanged(qutim_sdk_0_3::MessageList)), Qt::UniqueConnection);
	connect(s, SIGNAL(unreadChanged(qutim_sdk_0_3::MessageList)), SLOT(onUnreadChanged(qutim_sdk_0_3::MessageList)), Qt::UniqueConnection);
	connect(s, SIGNAL(messageReceived(qutim_sdk_0_3::Message*)),  SLOT(onMessageSmthDid(qutim_sdk_0_3::Message*)));
	connect(s, SIGNAL(messageSent(qutim_sdk_0_3::Message*)),      SLOT(onMessageSmthDid(qutim_sdk_0_3::Message*)));
	connect(s, SIGNAL(activated(bool)), SLOT(onSessionActivated(bool)));
}

void Win7Int2::onSessionActivated(bool activated)
{
	if (activated)
		testTab();
	QWidget *w = chatWindow();
	if (w && cfg_thumbsEnabled) {
		TaskbarPreviews::tabSetTitle(previousTabId, w->windowTitle());
		TaskbarPreviews::tabPreviewsRefresh(previousTabId);
	}
}

void Win7Int2::testTab()
{
	if (!cfg_thumbsEnabled)
		return;
	QWidget *w = chatWindow();
	if (w != previousWindow && cfg_thumbsEnabled) {
		TaskbarPreviews::tabRemove(previousTabId);
		previousTabId = 0;
	}
	if (cfg_thumbsEnabled) {
		if (!previousTabId && w) {
			previousTabId  = TaskbarPreviews::tabAddVirtual(previews, w, w->windowTitle());
			previousWindow = w;
			connect(w, SIGNAL(destroyed()), SLOT(onChatwidgetDestroyed()));
		} else
			if (w)
				TaskbarPreviews::tabSetTitle(previousTabId, w->windowTitle());
	}
}

void Win7Int2::onUnreadChanged(qutim_sdk_0_3::MessageList)
{
	QList<ChatSession*> sessions = ChatLayer::instance()->sessions();
	quint32 unreadConfs = 0, unreadChats = 0;
	foreach (ChatSession* s, sessions) {
		if (!s) { // that's strange, but happens // TODO: this block should dissapear sometimes
			debug() << "Zeros still appear in 'ChatLayer::instance()->sessions()'.";
			continue;
		}
		ChatUnit *unit      = s->getUnit();
		unsigned unreadSize = s->unread().size();
		if (qobject_cast<Conference*>(unit))
			unreadConfs += unreadSize;
		else
			unreadChats += unreadSize;
	}
	if (cfg_thumbsEnabled)
		previews->updateNumbers(unreadConfs, unreadChats);
	QWidget *window = chatWindow();
	if (window) {
		if (cfg_overlayIconEnabled) {
			if (unreadChats || unreadConfs)
				OverlayIcon::set(window, generateOverlayIcon(unreadConfs, unreadChats));
			else
				OverlayIcon::clear(window);
		}
		if (cfg_thumbsEnabled) {
			if (previousWindow != window)
				testTab();
			else
				TaskbarPreviews::tabPreviewsRefresh(previousTabId);
		}
	} else {
		if (cfg_thumbsEnabled) {
			TaskbarPreviews::tabRemove(previousTabId);
			previousTabId = 0;
		}
	}
	previousWindow = window;
}

QWidget *Win7Int2::chatWindow()
{
	QObject* obj    = qobject_cast<QObject*>(ServiceManager::getByName("ChatForm"));
	QWidget *widget = 0;
	bool metZero    = false;
	QWidgetList list;
	QMetaObject::invokeMethod(obj, "chatWidgets", Qt::DirectConnection, Q_RETURN_ARG(QWidgetList, list));
	if(!list.size())
		return 0; // empty list
	foreach (QWidget *w, list)
		if (w) {
			widget = w->window(); // some exists
			break;
		} else
			metZero = true;
	if (metZero) // TODO: this block should dissapear sometimes as well as variables used here
		debug() << "Zeros still appear in ChatForm's chatWidgets list.";
	return widget;
}

QPixmap Win7Int2::generateOverlayIcon(quint32 unreadConfs, quint32 unreadChats)
{
	quint32 count = unreadChats + (cfg_addConfs ? unreadConfs : 0);
	QPixmap icon;
	if(unreadConfs && !unreadChats)
		icon = Icon("mail-message")   .pixmap(16, 16);
	else
		icon = Icon("mail-unread-new").pixmap(16, 16);
	QPainter painter(&icon);
	if(cfg_displayCount && count){
		QFont font;
		font.setWeight(QFont::DemiBold);
		font.setFamily("Segoe UI");
		font.setPointSize(7);
		painter.setFont(font);
		painter.setPen(Qt::darkBlue);
		if (cfg_displayCount)
			painter.drawText(QRect(0, 1, 15, 15), Qt::AlignCenter, QString::number(count));
	}
	return icon;
}

void Win7Int2::onChatwidgetDestroyed()
{
	if (previousTabId && cfg_thumbsEnabled)
		TaskbarPreviews::tabRemove(previousTabId);
	previousTabId = 0;
	testTab();
}

void Win7Int2::onMessageSmthDid(qutim_sdk_0_3::Message *)
{
	if (cfg_thumbsEnabled)
		TaskbarPreviews::tabPreviewsRefresh(this->previousTabId);
}

void Win7Int2::reloadSettings()
{
	Config cfg(WI_CONFIG);
	cfg_displayCount = cfg.value("oi_showNewMsgCount",    true);
	cfg_addConfs     = cfg.value("oi_addNewConfMsgCount", false);
	cfg_overlayIconEnabled = cfg.value("oi_enabled",      true);
	if (cfg_thumbsEnabled)
		previews->reloadSettings();
	if (!cfg_overlayIconEnabled) {
		QWidget *w = chatWindow();
		if (w)
			OverlayIcon::clear(w);
	}
}

QUTIM_EXPORT_PLUGIN(Win7Int2)
