/****************************************************************************
 *
 *  This file is part of qutIM
 *
 *  Copyright (c) 2011 by Nigmatullin Ruslan <euroelessar@gmail.com>
 *
 ***************************************************************************
 *                                                                         *
 *   This file is part of free software; you can redistribute it and/or    *
 *   modify it under the terms of the GNU General Public License as        *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************
 ****************************************************************************/

#include "mobileaboutcreator.h"
#include "mobileaboutdialog.h"
#include <qutim/servicemanager.h>
#include <qutim/icon.h>
#include <qutim/debug.h>
#include <qutim/menucontroller.h>
#include <qutim/systemintegration.h>

namespace Core
{
using namespace qutim_sdk_0_3;

MobileAboutCreator::MobileAboutCreator()
{
	if (MenuController *menu = ServiceManager::getByName<MenuController*>("ContactList")) {
		ActionGenerator *gen = new ActionGenerator(Icon(QLatin1String("qutim")),
		                                           QT_TRANSLATE_NOOP("Core", "About qutIM"),
		                                           this,
		                                           SLOT(showWidget()));
		gen->setPriority(1);
		gen->setType(ActionTypePreferences);
		menu->addAction(gen);
	}
}

MobileAboutCreator::~MobileAboutCreator()
{
}

void MobileAboutCreator::showWidget()
{
	if (m_widget) {
		SystemIntegration::show(m_widget);
		m_widget->raise();
		return;
	}
	m_widget = new MobileAboutDialog();
	SystemIntegration::show(m_widget);
}
}
