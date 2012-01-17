/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Sidorov Aleksey <sauron@citadelspb.com>
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

#ifndef VCONTACT_H
#define VCONTACT_H
#include "vkontakte_global.h"
#include <qutim/contact.h>

class VAccount;
class VContactPrivate;
class LIBVKONTAKTE_EXPORT VContact : public Contact
{
	Q_OBJECT
	Q_DECLARE_PRIVATE(VContact)
public:
	VContact(const QString& id, VAccount* account);
	virtual QString id() const;
	virtual bool isInList() const;
	virtual bool sendMessage(const Message& message);
	virtual void setTags(const QStringList& tags);
	virtual void setInList(bool inList);
	void setContactTags(const QStringList& tags);
	void setContactInList(bool inList);
	void setOnline(bool set);
	void setActivity(const QString &activity);
	QString activity() const;
	virtual Status status() const;
	virtual ~VContact();
	virtual QStringList tags() const;
	virtual QString name() const;
	void setContactName(const QString& name);
	virtual void setName(const QString& name);
	void setAvatar(const QString &avatar);
	virtual QString avatar() const;
	VAccount *account() const;
private:
	virtual bool event(QEvent *ev);
	QScopedPointer<VContactPrivate> d_ptr;
};

Q_DECLARE_METATYPE(VContact*)

#endif // VCONTACT_H

