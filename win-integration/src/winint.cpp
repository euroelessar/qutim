#include "winint.h"
#include "wsettings.h"
#include "trayicon.h"
#include <QDir>
#include <qutim/configbase.h>
#include <qutim/debug.h>
#include <qutim/extensionicon.h>
#include <qutim/icon.h>
#include <qutim/servicemanager.h>
#include <qutim/settingslayer.h>
#include <qutim/conference.h>
#include <qt_windows.h>

using namespace qutim_sdk_0_3;

WinIntegration *WinIntegration::pluginInstance = 0;

WinIntegration::WinIntegration()
{
	pluginInstance = this;
	// being run from Explorer
	HWND console = FindWindowW(L"ConsoleWindowClass", QDir::toNativeSeparators(qApp->applicationFilePath()).toStdWString().data());
	if (!console) // being run from taskbar (pinned icon), assuming default link name (qutim); function is not case-sensitive
		console = FindWindowW(L"ConsoleWindowClass", L"qutim");
	if (console)
		ShowWindow(console, SW_HIDE);
}

void WinIntegration::init()
{
	addAuthor(QT_TRANSLATE_NOOP("Author", "Vizir Ivan"),
		QT_TRANSLATE_NOOP("Task", "Author"),
		QLatin1String("define-true-false@yandex.com"));
	setInfo(QT_TRANSLATE_NOOP("Plugin", "Windows Integration"),
		QT_TRANSLATE_NOOP("Plugin", "Adds count of unread messages as an icon to taskbar button of qutim, "
								"along with some commands list and provides a bit more usable notification area "
								"icon than default plugin."),
		PLUGIN_VERSION(2, 0, 0, 9999),
		ExtensionIcon());
	addExtension<NotSimpleTray>(QT_TRANSLATE_NOOP("Plugin", "Windows Notification Area Icon"),
									 QT_TRANSLATE_NOOP("Plugin", "Provides a bit more usable notification area "
															 "icon than default plugin."),
									 ExtensionIcon());
}

bool WinIntegration::load()
{
	settingsItem = new GeneralSettingsItem<WSettingsWidget>(
		Settings::Plugin,	QIcon(),
		QT_TRANSLATE_NOOP("Plugin", "Windows Integration"));
	Settings::registerItem(settingsItem);
	connect(ChatLayer::instance(), SIGNAL(sessionCreated(qutim_sdk_0_3::ChatSession*)), SLOT(onSessionCreated(qutim_sdk_0_3::ChatSession*)));
	return true;
}

bool WinIntegration::unload()
{
	Settings::removeItem(settingsItem);
	return true;
}

QWidget* WinIntegration::oneOfChatWindows()
{
	QObject* obj    = qobject_cast<QObject*>(ServiceManager::getByName("ChatForm"));
	QWidget *widget = 0;
	bool metZero    = false;
	QWidgetList list;
	QMetaObject::invokeMethod(obj, "chatWidgets", Qt::DirectConnection, Q_RETURN_ARG(QWidgetList, list));
	if(!list.size())
		return 0;
	foreach (QWidget *w, list)
		if (w) {
			widget = w->window();
			break;
		} else
			metZero = true;
	if (metZero) // TODO: this block should dissapear sometimes as well as variables used here
		warning() << "Zeros still appear in ChatForm's chatWidgets list.";
	return widget;
}

void WinIntegration::onSessionCreated(qutim_sdk_0_3::ChatSession *s)
{
	connect(s, SIGNAL(unreadChanged(qutim_sdk_0_3::MessageList)), SLOT(onUnreadChanged(qutim_sdk_0_3::MessageList)), Qt::UniqueConnection);
}

void WinIntegration::onUnreadChanged(qutim_sdk_0_3::MessageList)
{
	QList<ChatSession*> sessions = ChatLayer::instance()->sessions();
	quint32 unreadConfs = 0, unreadChats = 0;
	foreach (ChatSession* s, sessions) {
		if (!s) { // TODO: this block should dissapear sometimes
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
	emit unreadChanged(unreadChats, unreadConfs);
}

QUTIM_EXPORT_PLUGIN(WinIntegration)
