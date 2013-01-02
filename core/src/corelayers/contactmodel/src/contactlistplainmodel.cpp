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

#include "contactlistplainmodel.h"

using namespace qutim_sdk_0_3;

ContactListPlainModel::ContactListPlainModel(QObject *parent) :
    ContactListBaseModel(parent)
{
    Q_UNUSED(QT_TRANSLATE_NOOP("ContactList", "Show only contacts"));
}

void ContactListPlainModel::addContact(Contact *contact)
{
    ensureContact(contact, rootNode());
}

void ContactListPlainModel::removeContact(Contact *contact)
{
    eraseContact(contact, rootNode());
}
