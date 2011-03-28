#include "winint.h"
#include "wsettings.h"
#include <QDir>
#include <QSysInfo>
#include <QFileInfo>
#include <QLibrary>
#include <QtDeclarative/qdeclarative.h>
#include <QGraphicsDropShadowEffect>
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
	: subPlugins_(0)
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
}

bool WinIntegration::load()
{
	settingsItem = new GeneralSettingsItem<WSettingsWidget>(
		Settings::Plugin,	QIcon(),
		QT_TRANSLATE_NOOP("Plugin", "Windows Integration"));
	Settings::registerItem(settingsItem);
	connect(ChatLayer::instance(), SIGNAL(sessionCreated(qutim_sdk_0_3::ChatSession*)), SLOT(onSessionCreated(qutim_sdk_0_3::ChatSession*)));
	QSysInfo::WinVersion wv = QSysInfo::windowsVersion();
	switch (wv) {
	case QSysInfo::WV_WINDOWS7 : Win7SmallFeatures(true);
	case QSysInfo::WV_VISTA :    VistaSmallFeatures(true);
	default:                     XpSmallFeatures(true);
	}
	return true;
}

bool WinIntegration::unload()
{
	Settings::removeItem(settingsItem);
	QSysInfo::WinVersion wv = QSysInfo::windowsVersion();
	switch (wv) {
	case QSysInfo::WV_WINDOWS7 : Win7SmallFeatures(false);
	case QSysInfo::WV_VISTA :    VistaSmallFeatures(false);
	default:                     XpSmallFeatures(false);
	}
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

void WinIntegration::Win7SmallFeatures(bool)
{

}

void WinIntegration::VistaSmallFeatures(bool enable)
{
	typedef HRESULT (WINAPI * WerExclFunctions_t)(PCWSTR, BOOL);
	typedef HRESULT (WINAPI * RegisterApplicationRestart_t)(PCWSTR, DWORD);
	typedef HRESULT (WINAPI * UnregisterApplicationRestart_t)();

	WerExclFunctions_t pWerAddExcludedApplication, pWerRemoveExcludedApplication;
	RegisterApplicationRestart_t   pRegisterApplicationRestart;
	UnregisterApplicationRestart_t pUnregisterApplicationRestart;
	QLibrary wer_dll("wer.dll"), kernel32_dll("kernel32.dll");

	pWerAddExcludedApplication    = static_cast<WerExclFunctions_t>(wer_dll.resolve("WerAddExcludedApplication"));
	pWerRemoveExcludedApplication = static_cast<WerExclFunctions_t>(wer_dll.resolve("WerRemoveExcludedApplication"));
	pRegisterApplicationRestart   = static_cast<RegisterApplicationRestart_t>  (kernel32_dll.resolve("RegisterApplicationRestart"));
	pUnregisterApplicationRestart = static_cast<UnregisterApplicationRestart_t>(kernel32_dll.resolve("UnregisterApplicationRestart"));

	static void * restart = ServiceManager::getByName("CrashHandler"); // TODO: is there such services?
	if (enable) {
		pWerAddExcludedApplication(QFileInfo(qApp->applicationFilePath()).fileName().toStdWString().c_str(), 0);
		if (!restart)
			pRegisterApplicationRestart(L"", 0);
	} else {
		pWerRemoveExcludedApplication(QFileInfo(qApp->applicationFilePath()).fileName().toStdWString().c_str(), 0);
		if (!restart)
			pUnregisterApplicationRestart();
	}
	qmlRegisterType<QGraphicsDropShadowEffect>("qutimCustomEffects", 1, 0, "DropShadow"); // TODO: remove, shouldn't be there
}

void WinIntegration::XpSmallFeatures(bool)
{

}

void WinIntegration::enabledPlugin(SubPlugins plugin)
{
	subPlugins_ |= plugin;
}

void WinIntegration::disabledPlugin(SubPlugins plugin)
{
	subPlugins_ =~ plugin;
}

bool WinIntegration::isPluginEnabled(SubPlugins plugin)
{
	return subPlugins_ & plugin;
}

void WinIntegration::onSettingsSaved()
{
	emit reloadSettigs();
}

QUTIM_EXPORT_PLUGIN(WinIntegration)
