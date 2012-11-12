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

#ifndef VINFOREQUEST_H
#define VINFOREQUEST_H

#include <qutim/inforequest.h>
#include <qutim/status.h>
#include <QSet>

namespace Vreen {
class Client;
} //namespace Vreen

class VConnection;
class VAccount;
class VContact;

class VInfoFactory : public QObject, public qutim_sdk_0_3::InfoRequestFactory
{
	Q_OBJECT
	Q_INTERFACES(qutim_sdk_0_3::InfoRequestFactory)
public:
	VInfoFactory(VAccount *account);
	virtual SupportLevel supportLevel(QObject *object);
protected:
	virtual qutim_sdk_0_3::InfoRequest *createrDataFormRequest(QObject *object);
	virtual bool startObserve(QObject *object);
	virtual bool stopObserve(QObject *object);
private slots:
	void onAccountStatusChanged(const qutim_sdk_0_3::Status &status,
								const qutim_sdk_0_3::Status &previous);
private:
	VAccount *m_account;
	QSet<VContact *> m_contacts;
};

class VInfoRequest : public qutim_sdk_0_3::InfoRequest
{
	Q_OBJECT
public:
	enum DataType
	{
		NickName,
		FirstName,
		LastName,
		Sex,
		BDate,
		City,
		Country,
		Photo,
		HomePhone,
		MobilePhone,
		University,
		Faculty,
		Graduation
	};

    VInfoRequest(QObject *parent);
	virtual qutim_sdk_0_3::DataItem createDataItem() const;
	virtual void doRequest(const QSet<QString> &hints);
	virtual void doUpdate(const qutim_sdk_0_3::DataItem &dataItem);
	virtual void doCancel();
signals:
	void canceled();
private slots:
	void onRequestFinished();
	void onAddressEnsured();
private:
	void ensureAddress(DataType type);
	void addItem(DataType type, qutim_sdk_0_3::DataItem &group, const QVariant &data) const;
	inline void addItem(DataType type, qutim_sdk_0_3::DataItem &group, const char *name) const
	{ addItem(type, group, m_data.value(QLatin1String(name))); }
	
	QString m_id;
	Vreen::Client *m_client;
	int m_unknownCount;
	QVariantMap m_data;
};

#endif // VINFOREQUEST_H

