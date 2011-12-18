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

#ifndef CONTACTADAPTOR_H
#define CONTACTADAPTOR_H

#include <QDBusAbstractAdaptor>
#include <QDBusObjectPath>
#include <QDBusConnection>
#include <qutim/contact.h>

using namespace qutim_sdk_0_3;

class ContactAdaptor : public QDBusAbstractAdaptor
{
	Q_OBJECT
	Q_CLASSINFO("D-Bus Interface", "org.qutim.Contact")
	Q_PROPERTY(QStringList tags READ tags WRITE setTags NOTIFY tagsChanged)
	Q_PROPERTY(bool inList READ isInList WRITE setInList NOTIFY inListChanged)
public:
    explicit ContactAdaptor(Contact *contact);
	inline QStringList tags() const { return self()->tags(); }
	inline void setTags(const QStringList &ts) { self()->setTags(ts); }
	inline bool isInList() const { return self()->isInList(); }
	inline void setInList(bool is) { self()->setInList(is); }
public slots:
	inline void addToList() { setInList(true); }
	inline void removeFromList() { setInList(false); }
signals:
	void tagsChanged(const QStringList &current, const QStringList &previous);
	void inListChanged(bool inList);
private:
	inline Contact *self() const { return static_cast<Contact*>(parent()); }
};

#endif // CONTACTADAPTOR_H

