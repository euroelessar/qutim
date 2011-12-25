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

#ifndef QUETZALCONTACT_H
#define QUETZALCONTACT_H

#include <qutim/contact.h>
#include <purple.h>
#include <qutim/configbase.h>

using namespace qutim_sdk_0_3;

class QuetzalAccount;

class QuetzalContact : public Contact
{
	Q_OBJECT
public:
	explicit QuetzalContact(PurpleBuddy *buddy);

	PurpleBuddy *purple() { return m_buddies.first(); }
	void save(ConfigGroup group);
	void update(PurpleBuddy *buddy);
	int addBuddy(PurpleBuddy *buddy);
	int removeBuddy(PurpleBuddy *buddy);
	virtual QString avatar() const;
	virtual QString id() const;
	virtual QString name() const;
	virtual QStringList tags() const;
	virtual Status status() const;
//	virtual QIcon statusIcon() const;
	virtual bool sendMessage(const Message &message);
	virtual void setName(const QString &name);
	virtual void setTags(const QStringList &tags);
	virtual bool isInList() const;
	virtual void setInList(bool inList);

protected:
	MenuController::ActionList dynamicActions() const;
signals:

public slots:
private:
	virtual bool event(QEvent *);
private:
	void ensureAvatarPath();
	Status m_status;
	QStringList m_tags;
	QString m_avatarPath;
	QString m_id;
	QString m_name;
	QList<PurpleBuddy*> m_buddies;
	friend class QuetzalAccount;
};

void quetzal_menu_add(QList<MenuController::Action> &actions, void *node,
					  GList *menu, const QList<QByteArray> &off, int type);

#endif // QUETZALCONTACT_H

