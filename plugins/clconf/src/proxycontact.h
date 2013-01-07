/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Alexey Prokhin <alexey.prokhin@yandex.ru>
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

#ifndef PROXYCONTACT_H
#define PROXYCONTACT_H

#include <qutim/conference.h>
#include <qutim/contact.h>

using namespace qutim_sdk_0_3;

class ProxyContact : public Contact
{
	Q_OBJECT
public:
	ProxyContact(Conference *conf);
	QStringList tags() const;
	void setTags(const QStringList &tags);
	bool isInList() const;
	void setInList(bool inList);
	virtual QString id() const;
	virtual QString title() const;
	virtual QString name() const;
	virtual Status status() const;
	virtual bool sendMessage(const Message &message);
	Conference *conference() const { return m_conf.data(); }
private slots:
	void onAccountStatusChanged(const qutim_sdk_0_3::Status &current,
								const qutim_sdk_0_3::Status &previous);
	void updateStatus();
protected:
	bool event(QEvent *ev);
	bool eventFilter(QObject *obj, QEvent *ev);
private:
	Status m_status;
	QPointer<Conference> m_conf;
};

#endif // PROXYCONTACT_H

