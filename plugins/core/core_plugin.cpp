/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2013 Ruslan Nigmatullin <euroelessar@yandex.ru>
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

#include "core_plugin.h"
#include "quickaccountmanager.h"
#include "contactlistmodel.h"
#include <TelepathyQt/Types>

#include <qqml.h>

void CorePlugin::registerTypes(const char *uri)
{
    Tp::registerTypes();

    // @uri org.qutim.core
    qmlRegisterType<QuickAccountManager>(uri, 0, 4, "AccountManager");
    qmlRegisterUncreatableType<Account>(uri, 0, 4, "Account", "Creatable only by ureen core");
    qmlRegisterType<ContactListModel>(uri, 0, 4, "ContactListModel");
}


