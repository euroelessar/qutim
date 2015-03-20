/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2014 Nicolay Izoderov <nico-izo@ya.ru>
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

#ifndef METAMENU_METAMENU_H
#define METAMENU_METAMENU_H

#include <qutim/plugin.h>
#include <qutim/servicemanager.h>
#include <qutim/menucontroller.h>
#include <qutim/chatsession.h>
#include <QMenuBar>

using namespace qutim_sdk_0_3;

class MetaMenuController : public MenuController
{
	Q_OBJECT
public:
	MetaMenuController(QObject *parent) : MenuController(parent)
	{
		if (MenuController *contactList = ServiceManager::getByName<MenuController *>("ContactList"))
			setMenuOwner(contactList);
	}
};

class MetamenuPlugin : public qutim_sdk_0_3::Plugin
{
	Q_OBJECT
	Q_PLUGIN_METADATA(IID "org.qutim.Plugin")
	Q_CLASSINFO("DebugName", "Metamenu")
	Q_CLASSINFO("Uses", "ChatLayer")
public:
	explicit MetamenuPlugin ();
	virtual void init();
	virtual bool load();
	virtual bool unload();
public slots:
	void shot();
	void onDestroyed();
private:
	QWidget* oneOfChatWindows();
	QMenuBar* m_menuBar = 0;
	MenuController* m_menu;
	bool m_added = false;
};

#endif /* end of include guard: METAMENU_METAMENU_H */

