/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Ivan Vizir <define-true-false@yandex.com>
**
*****************************************************************************
**
** $QUTIM_BEGIN_LICENSE$
** This program is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
** See the GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program.  If not, see http://www.gnu.org/licenses/.
** $QUTIM_END_LICENSE$
**
****************************************************************************/

#include "winint.h"
#include "cmd-server.h"
#include "wsettings.h"
#include <QDir>
#include <QSysInfo>
#include <QFileInfo>
#include <QLibrary>
#include <QGraphicsDropShadowEffect>
#include <qutim/configbase.h>
#include <qutim/debug.h>
#include <qutim/extensionicon.h>
#include <qutim/icon.h>
#include <qutim/servicemanager.h>
#include <qutim/settingslayer.h>
#include <qutim/conference.h>
#include <qt_windows.h>

#include "../subplugins/win7taskbar/win7taskbar.h"

using namespace qutim_sdk_0_3;

WinIntegration *WinIntegration::pluginInstance = 0;
Win7Features *subpluginInstance = 0;

WinIntegration::WinIntegration()
	: subPlugins_(0)
{
	pluginInstance = this;
	subpluginInstance = new Win7Features();
	// being run from Explorer
	HWND console = FindWindowW(L"ConsoleWindowClass", QDir::toNativeSeparators(qApp->applicationFilePath()).toStdWString().data());
	if (!console) // being run from taskbar (pinned icon), assuming default link name (qutim); function is not case-sensitive
		console = FindWindowW(L"ConsoleWindowClass", L"qutim");
	if (console)
		ShowWindow(console, SW_HIDE);
	//CmdServer::instance()->registerHandler("winint2", this);
}

void WinIntegration::init()
{
	addAuthor(QLatin1String("viv"));
	setInfo(QT_TRANSLATE_NOOP("Plugin", "Windows Integration"),
			QT_TRANSLATE_NOOP("Plugin", "Adds count of unread messages as an icon to taskbar button of qutim, "
							  "along with some commands list and provides a bit more usable notification area "
							  "icon than default plugin."),
			PLUGIN_VERSION(2, 0, 0, 9999),
			ExtensionIcon());
	setCapabilities(Loadable);
	subpluginInstance->init();
}

bool WinIntegration::load()
{
	settingsItem = new GeneralSettingsItem<WSettingsWidget>(
					   Settings::Plugin,	QIcon(),
					   QT_TRANSLATE_NOOP("Plugin", "Windows Integration"));
	Settings::registerItem(settingsItem);
	connect(ChatLayer::instance(), SIGNAL(sessionCreated(qutim_sdk_0_3::ChatSession*)), SLOT(onSessionCreated(qutim_sdk_0_3::ChatSession*)));
	QSysInfo::WinVersion wv = QSysInfo::windowsVersion();
	switch (wv) { // falling through is not a mistake
	case QSysInfo::WV_WINDOWS7 : Win7SmallFeatures(true);
	case QSysInfo::WV_VISTA :    VistaSmallFeatures(true);
	default:                     XpSmallFeatures(true);
	}
	subpluginInstance->load();
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
	subpluginInstance->unload();
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
		qWarning() << "Zeros still appear in ChatForm's chatWidgets list.";
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
			qDebug() << "Zeros still appear in 'ChatLayer::instance()->sessions()'.";
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

	pWerAddExcludedApplication    = reinterpret_cast<WerExclFunctions_t>(wer_dll.resolve("WerAddExcludedApplication"));
	pWerRemoveExcludedApplication = reinterpret_cast<WerExclFunctions_t>(wer_dll.resolve("WerRemoveExcludedApplication"));
	pRegisterApplicationRestart   = reinterpret_cast<RegisterApplicationRestart_t>  (kernel32_dll.resolve("RegisterApplicationRestart"));
	pUnregisterApplicationRestart = reinterpret_cast<UnregisterApplicationRestart_t>(kernel32_dll.resolve("UnregisterApplicationRestart"));

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
	//qmlRegisterType<QGraphicsDropShadowEffect>("qutimCustomEffects", 1, 0, "DropShadow"); // TODO: remove, shouldn't be there, made for myself only
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
	subPlugins_ &= ~plugin;
}

bool WinIntegration::isPluginEnabled(SubPlugins plugin)
{
	return subPlugins_ & plugin;
}

void WinIntegration::onSettingsSaved()
{
	emit reloadSettigs();
}

void WinIntegration::updateAssocs()
{
	QWidget *window = dynamic_cast<QWidget*>(sender());
	if (window)
		window = window->window();
	ShellExecuteA((HWND)window->winId(), "runas", "wininthelper.exe", "assocreg", 0, 0);
}

QUTIM_EXPORT_PLUGIN(WinIntegration)

