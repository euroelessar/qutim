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

#ifndef WIN7INT_H
#define WIN7INT_H

#include <qutim/plugin.h>
#include <qutim/chatsession.h>
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

