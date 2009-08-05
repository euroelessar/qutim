/*
    AbstractLayer

    Copyright (c) 2008 by Rustam Chakin <qutim.develop@gmail.com>

 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
*/

#ifndef ABSTRACTLAYER_H_
#define ABSTRACTLAYER_H_

#include <QtGui>
#include <QSettings>
#include <QMenu>
#include "pluginsystem.h"
#include "include/qutim/layerscity.h"
class qutIM;
class IconManager;
class QHBoxLayout;

using namespace qutim_sdk_0_2;

class AbstractLayer : public LayersCity
{
public:
	bool showLoginDialog();
	void loadCurrentProfile();
	void openSettingsDialog();
	void setPointers(qutIM *);
	void createNewAccount();
	static AbstractLayer &instance();
	void initializePointers(QTreeView *, QHBoxLayout *, QMenu *, QAction *);
	void clearMenuFromStatuses();
	void addStatusesToMenu();
	void reloadGeneralSettings();
	void addAccountMenusToTrayMenu(bool add);
	void updateTrayIcon(); //huhuhuhu ;DD
	inline void setCurrentAccountIconName(const QString &account_name)
	{ d->current_account_icon_name = account_name; }
	void updateStausMenusInTrayMenu();
	void setAutoAway();
	void setStatusAfterAutoAway();
	inline QString getCurrentProfile() const {return d->current_profile; }
	void animateTrayNewMessage();
	void stopTrayNewMessageAnimation();
	qutIM *getParent();
	void showBalloon(const QString &title, const QString &message, int time);
	void reloadStyleLanguage();
	void addActionToMainMenu(QAction *action);
private:
	AbstractLayer();
	~AbstractLayer();
	struct Data
	{
		inline Data() : show_account_menus(false), is_new_profile(false) {}
		QString current_profile;
		qutIM *parent;
		::PluginSystem *plugin_system;
		QMenu *tray_menu;
		QList<QMenu*> account_status_menu_list;
		QAction *action_tray_menu_before;
		bool show_account_menus;
		bool is_new_profile;
		QString current_account_icon_name;
	};
	static Data *d;
};

#endif /*ABSTRACTLAYER_H_*/
