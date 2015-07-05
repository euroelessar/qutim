/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Nikita Belov <null@deltaz.org>
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

#ifndef WCONTACT_H
#define WCONTACT_H

#include <QApplication>

#include <qutim/contact.h>
#include <qutim/icon.h>
#include <qutim/message.h>
#include <qutim/chatsession.h>
#include <qutim/tooltip.h>

#include "wmanager.h"

using namespace qutim_sdk_0_3;

class WAccount;

class WContact : public Contact
{
	Q_OBJECT

public:
	WContact(const QString &code, const QString &name, Account *account);
	~WContact();

	WAccount *account();
	bool sendMessage(const Message &message);

	void setName(const QString &name);
	void setNameInternal(const QString &name);
	void setTags(const QStringList &tags);
	QString id() const;
	QString name() const;
	QString title() const;
	QStringList tags() const;
	Status status() const;
	void setStatusInternal(QString iconId, const QString &text);
	bool isInList() const;
	void setInList(bool inList);
	QString avatar() const;
	void updateStatus();
	bool isWaitingCurrent() const;

protected:
	virtual bool event(QEvent *ev);

private slots:
	void getWeather();
	void getForecast();

private:
	Status m_status;
	QString m_city;
	QString m_name;
	QStringList m_tags;
};

#endif // WCONTACT_H

