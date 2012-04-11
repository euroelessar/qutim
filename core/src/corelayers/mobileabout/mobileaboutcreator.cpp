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
		SystemIntegration::show(m_widget.data());
		m_widget.data()->raise();
		return;
	}
	m_widget = new MobileAboutDialog();
	SystemIntegration::show(m_widget.data());
}
}

