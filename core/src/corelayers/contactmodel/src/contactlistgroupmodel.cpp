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

#include "contactlistgroupmodel.h"

using namespace qutim_sdk_0_3;

ContactListGroupModel::ContactListGroupModel(QObject *parent) :
    ContactListBaseModel(parent)
{
    Q_UNUSED(QT_TRANSLATE_NOOP("ContactList", "Show tags and contacts"));
}

void ContactListGroupModel::updateContactTags(Contact *contact, const QStringList &pseudoCurrent, const QStringList &pseudoPrevious)
{
    const QStringList current = fixTags(pseudoCurrent);
    const QStringList previous = fixTags(pseudoPrevious);
    foreach (const QString &tag, previous) {
        if (!current.contains(tag)) {
            TagNode *tagNode = ensureTag(tag, rootNode());
            eraseContact(contact, tagNode);
        }
    }
    foreach (const QString &tag, current) {
        if (!previous.contains(tag)) {
            TagNode *tagNode = ensureTag(tag, rootNode());
            ensureContact(contact, tagNode);
        }
    }
}

void ContactListGroupModel::addContact(Contact *contact)
{
    foreach (const QString &tag, fixTags(contact->tags())) {
        TagNode *tagNode = ensureTag(tag, rootNode());
        ContactNode *contactNode = ensureContact(contact, tagNode);
        Q_UNUSED(contactNode);
    }
}

void ContactListGroupModel::removeContact(Contact *contact)
{
    const QStringList tags = contact->tags();
    if (tags.isEmpty()) {
        eraseContact(contact, rootNode());
    } else {
        foreach (const QString &tag, tags) {
            TagNode *tagNode = ensureTag(tag, rootNode());
            eraseContact(contact, tagNode);
        }
    }
}
