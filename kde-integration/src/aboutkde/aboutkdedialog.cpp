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

#include "aboutkdedialog.h"
#include "../kdeintegration.h"
#include <kstandardaction.h>
#include <kaction.h>
#include <khelpmenu.h>
#include <qutim/actiongenerator.h>
#include <qutim/servicemanager.h>
#include <qutim/menucontroller.h>
#include <qutim/icon.h>

using namespace qutim_sdk_0_3;

class AboutKdeActionGenerator : public ActionGenerator
{
public:
	AboutKdeActionGenerator() 
	    : ActionGenerator(QIcon(), LocalizedString(), 0, 0)
	{
	}

	virtual QObject *generateHelper() const
	{
		return KStandardAction::aboutKDE(KdeIntegration::KdePlugin::helpMenu(),
		                                 SLOT(aboutKDE()), 0);
	}
};

AboutKdeDialog::AboutKdeDialog()
{
	deleteLater();
	ActionGenerator *gen;
	MenuController *menu = ServiceManager::getByName<MenuController*>("ContactList");
	if (!menu)
		return;
	gen = new AboutKdeActionGenerator();
	gen->setPriority(0);
	gen->setType(ActionTypePreferences);
	menu->addAction(gen);
}
