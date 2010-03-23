/****************************************************************************
 *  quetzalcontact.h
 *
 *  Copyright (c) 2009 by Nigmatullin Ruslan <euroelessar@gmail.com>
 *
 ***************************************************************************
 *                                                                         *
 *   This library is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
*****************************************************************************/

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

	PurpleBuddy *buddy() { return m_buddy; }
	void save(ConfigGroup group);
	void update();
	virtual QString avatar() const;
	virtual QString id() const;
	virtual QString name() const;
	virtual QSet<QString> tags() const;
	virtual Status status() const;
//	virtual QIcon statusIcon() const;
	virtual void sendMessage(const Message &message);
	virtual void setName(const QString &name);
	virtual void setTags(const QSet<QString> &tags);
	virtual bool isInList() const;
	virtual void setInList(bool inList);

protected:
	QList<MenuController::Action> dynamicActions() const;
signals:

public slots:
private:
	virtual bool event(QEvent *);
private:
	Status m_status;
	QSet<QString> m_tags;
	QString m_id;
	QString m_name;
	PurpleBuddy *m_buddy;
	friend class QuetzalAccount;
};

void quetzal_menu_add(QList<MenuController::Action> &actions, void *node,
					  GList *menu, const QList<QByteArray> &off, int type);

#endif // QUETZALCONTACT_H
