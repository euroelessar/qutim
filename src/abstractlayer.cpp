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

#include "abstractlayer.h"
#include "profilelogindialog.h"
#include "addaccountwizard.h"
#include "qutimsettings.h"
#include "qutim.h"
#include "iconmanager.h"
//#include "abstracthistorylayer.h"
#include "globalsettings/abstractglobalsettings.h"

AbstractLayer::Data *AbstractLayer::d = new AbstractLayer::Data();

AbstractLayer::AbstractLayer()
{
}

AbstractLayer::~AbstractLayer()
{
	
}

AbstractLayer &AbstractLayer::instance()
{
}

void AbstractLayer::setPointers(qutIM *parent)
{

}

bool AbstractLayer::showLoginDialog()
{

}

void AbstractLayer::loadCurrentProfile()
{

}

void AbstractLayer::createNewAccount()
{

}

void AbstractLayer::openSettingsDialog()
{

}

void AbstractLayer::initializePointers(QTreeView *contact_list_view, QHBoxLayout *account_button_layout,
		QMenu *tray_menu, QAction *action_before)
{
}

void AbstractLayer::clearMenuFromStatuses()
{

}

void AbstractLayer::addStatusesToMenu()
{

}

void AbstractLayer::reloadGeneralSettings()
{

}

void AbstractLayer::addAccountMenusToTrayMenu(bool add)
{

}

void AbstractLayer::updateTrayIcon()
{

}

void AbstractLayer::updateStausMenusInTrayMenu()
{

}

void AbstractLayer::setAutoAway()
{

}

void AbstractLayer::setStatusAfterAutoAway()
{

}

void AbstractLayer::animateTrayNewMessage()
{

}

void AbstractLayer::stopTrayNewMessageAnimation()
{

}

qutIM *AbstractLayer::getParent()
{

}

void AbstractLayer::showBalloon(const QString &title, const QString &message, int time)
{

}

void AbstractLayer::reloadStyleLanguage()
{

}

void AbstractLayer::addActionToMainMenu(QAction *action)
{

}
