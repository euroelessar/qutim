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

#include "simpleaboutcreator.h"
#include "simpleaboutdialog.h"
#include <qutim/servicemanager.h>
#include <qutim/icon.h>
#include <qutim/debug.h>
#include <qutim/menucontroller.h>

namespace Core
{
using namespace qutim_sdk_0_3;

SimpleAboutCreator::SimpleAboutCreator()
{
	if (MenuController *menu = ServiceManager::getByName<MenuController*>("ContactList")) {
		ActionGenerator *gen = new ActionGenerator(Icon(QLatin1String("qutim")),
		                                           QT_TRANSLATE_NOOP("Core", "About qutIM"),
		                                           this,
		                                           SLOT(showWidget()));
		gen->setType(ActionTypePreferences);
		menu->addAction(gen);
	}
}

SimpleAboutCreator::~SimpleAboutCreator()
{
}

void SimpleAboutCreator::showWidget()
{
	if (m_widget) {
		m_widget->show();
		m_widget->raise();
		return;
	}
	m_widget = new SimpleAboutDialog();
	m_widget->show();
}
}
