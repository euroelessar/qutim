/****************************************************************************
**
** qutIM instant messenger
**
** Copyright (C) 2011 Ruslan Nigmatullin <euroelessar@ya.ru>
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

#ifndef CONTACTLIST_H
#define CONTACTLIST_H

#include <QDeclarativeComponent>
#include <qutim/menucontroller.h>
#include <qutim/protocol.h>
#include <qutim/contact.h>
#include <qutim/account.h>
#include "statuswrapper.h"

QML_DECLARE_TYPE_HASMETATYPE(qutim_sdk_0_3::Account)
QML_DECLARE_TYPE_HASMETATYPE(qutim_sdk_0_3::ChatUnit)
QML_DECLARE_TYPE_HASMETATYPE(qutim_sdk_0_3::Contact)

namespace MeegoIntegration
{
class ContactList : public qutim_sdk_0_3::MenuController
{
    Q_OBJECT
	Q_CLASSINFO("Service", "ContactList")
	Q_CLASSINFO("Uses", "IconLoader")
	Q_PROPERTY(MeegoIntegration::StatusWrapper::Type status READ status WRITE setStatus NOTIFY statusChanged)
	Q_PROPERTY(QDeclarativeListProperty<qutim_sdk_0_3::Account> accounts READ accounts)
public:
    explicit ContactList();
	
	static void init();
	StatusWrapper::Type status() const;
	void setStatus(StatusWrapper::Type type);
	QDeclarativeListProperty<qutim_sdk_0_3::Account> accounts();
	
	static QUrl statusUrl(const QVariant &type, const QString &subtype);
	static QString statusIcon(const QVariant &type, const QString &subtype);

public slots:
	static QString statusName(const QVariant &type);
	static QString statusIcon(const QVariant &type);
	static QUrl statusUrl(const QVariant &type);
	
private slots:
	void onAccountAdded(qutim_sdk_0_3::Account *account);
	void onAccountRemoved(qutim_sdk_0_3::Account *account);
	
signals:
	void statusChanged(MeegoIntegration::StatusWrapper::Type);
	
//	Q_INVOKABLE void addButton(ActionGenerator *generator);
private:
	qutim_sdk_0_3::Status m_globalStatus;
	qutim_sdk_0_3::AccountList m_accounts;
};
}

#endif // CONTACTLIST_H
