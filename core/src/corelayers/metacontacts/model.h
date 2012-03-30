/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Aleksey Sidorov <gorthauer87@yandex.ru>
** Copyright © 2012 Sergei Lopatin <magist3r@gmail.com>
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

#ifndef MODEL_H
#define MODEL_H

#include <QStandardItemModel>
#include <QWeakPointer>

namespace qutim_sdk_0_3 {
class MetaContact;
class Contact;
}

namespace Core {
namespace MetaContacts {

class MetaContactImpl;
class Model : public QStandardItemModel
{
    Q_OBJECT
public:
    explicit Model(QObject *parent = 0);
	void setMetaContact(MetaContactImpl*);
	MetaContactImpl *metaContact() const;
    QList<qutim_sdk_0_3::Contact*> getContacts() const;
public slots:
	void searchContacts(const QString &name);
	void activated(const QModelIndex &index);
signals:
	void addContactTriggered(qutim_sdk_0_3::Contact*);
	void removeContactTriggered(qutim_sdk_0_3::Contact*);
private:
	void addContact(qutim_sdk_0_3::Contact *,QStandardItem *root);
	QWeakPointer<MetaContactImpl> m_metaContact;
    QStandardItem *m_metaRoot;
	QStandardItem *m_searchRoot;
};

} // namespace MetaContacts
} // namespace Core

#endif // MODEL_H

