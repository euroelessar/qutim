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

#ifndef CONTACT_H
#define CONTACT_H

#include "buddy.h"
#include <QSet>

namespace qutim_sdk_0_3
{
class Account;
class MetaContact;
class Message;
class ContactPrivate;

typedef QPair<LocalizedString, QVariant> InfoField;
typedef QList<InfoField> InfoFieldList;

/**
 * @brief Contact is base class for all contactlist members
 */
class LIBQUTIM_EXPORT Contact : public Buddy
{
	Q_OBJECT
	Q_DECLARE_PRIVATE(Contact)
	Q_PROPERTY(QStringList tags READ tags WRITE setTags NOTIFY tagsChanged)
	Q_PROPERTY(bool inList READ isInList WRITE setInList NOTIFY inListChanged)
public:
	/**
  * @brief default Contact's contructor
  *
  * @param account Pointer to chatunit's account
  */
	Contact(Account *account);
	Contact(ContactPrivate &d, Account *account);
	/**
  * @brief Contact's destructor
  */
	virtual ~Contact();
	/**
  * @brief Returns set of tags that mark the contact
  *
  * @return QSet<QString> tags
  */
	virtual QStringList tags() const;
	/**
  * @brief set a set of tags
  *
  * @param tags
  */
	virtual void setTags(const QStringList &tags) = 0;
	/**
  * @brief Returns membership contact to contactlist
  *
  * @return isInList
  */
	virtual bool isInList() const = 0;
	/**
  * @brief set a membership contact to contactlist
  *
  * @param inList
  */
	virtual void setInList(bool inList) = 0;
	/**
  * @brief add contact to contactlist, see also setInList(bool inList)
  */
	void addToList() { setInList(true); }
	/**
  * @brief remove contact from contactlist, see also setInList(bool inList)
  */
	void removeFromList() { setInList(false); }
	virtual ChatUnit *upperUnit();
protected:
	bool event(QEvent *);
signals:
	void tagsChanged(const QStringList &current, const QStringList &previous);
	void inListChanged(bool inList);
private:
	friend class MetaContact;
};

class LIBQUTIM_EXPORT ContactComparator : public QObject
{
	Q_OBJECT
	Q_CLASSINFO("Service", "ContactComparator")
public:
	virtual int compare(Contact *a, Contact *b) = 0;
	void startListen(qutim_sdk_0_3::Contact *contact);
	void stopListen(qutim_sdk_0_3::Contact *contact);
protected:
	virtual void doStartListen(qutim_sdk_0_3::Contact *contact) = 0;
	virtual void doStopListen(qutim_sdk_0_3::Contact *contact) = 0;
signals:
	void contactChanged(qutim_sdk_0_3::Contact*);
};

}

Q_DECLARE_METATYPE(qutim_sdk_0_3::Contact*)
Q_DECLARE_METATYPE(QList<qutim_sdk_0_3::Contact*>)

#endif // CONTACT_H

