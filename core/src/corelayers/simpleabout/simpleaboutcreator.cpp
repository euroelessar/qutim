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

#include "simpleaboutcreator.h"
#include <qutim/servicemanager.h>
#include <qutim/icon.h>
#include <qutim/debug.h>
#include <qutim/menucontroller.h>
#include <qutim/thememanager.h>
#include <qutim/declarativeview.h>

namespace Core
{
using namespace qutim_sdk_0_3;

SimpleAboutCreator::SimpleAboutCreator() : m_dialog(QStringLiteral("simpleabout"))
{
	if (MenuController *menu = ServiceManager::getByName<MenuController*>("ContactList")) {
		ActionGenerator *gen = new ActionGenerator(Icon(QLatin1String("qutim")),
													QT_TRANSLATE_NOOP("Core", "About qutIM"),
													this,
													SLOT(showWidget()));
		gen->setPriority(1);
		gen->setMenuRole(QAction::AboutRole);
		gen->setType(ActionTypePreferences);
		menu->addAction(gen);
	}
}

SimpleAboutCreator::~SimpleAboutCreator()
{
}

void SimpleAboutCreator::showWidget()
{
	m_dialog.show();
}
}

