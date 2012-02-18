/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
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

