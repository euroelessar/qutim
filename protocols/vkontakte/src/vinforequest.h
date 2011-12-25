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

using namespace qutim_sdk_0_3;

class VConnection;
class VAccount;
class VContact;

class VInfoFactory : public QObject, public InfoRequestFactory
{
	Q_OBJECT
public:
	VInfoFactory(VAccount *account);
	virtual SupportLevel supportLevel(QObject *object);
protected:
	virtual InfoRequest *createrDataFormRequest(QObject *object);
	virtual bool startObserve(QObject *object);
	virtual bool stopObserve(QObject *object);
private:
	void onAccountStatusChanged(const qutim_sdk_0_3::Status &status,
								const qutim_sdk_0_3::Status &previous);
	VAccount *m_account;
	QSet<VContact *> m_contacts;
};

class VInfoRequest : public InfoRequest
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
	virtual DataItem createDataItem() const;
	virtual void doRequest(const QSet<QString> &hints);
	virtual void doUpdate(const DataItem &dataItem);
	virtual void doCancel();
signals:
	void canceled();
private slots:
	void onRequestFinished();
	void onAddressEnsured();
private:
	void ensureAddress(DataType type);
	void addItem(DataType type, DataItem &group, const QVariant &data) const;
	inline void addItem(DataType type, DataItem &group, const char *name) const
	{ addItem(type, group, m_data.value(QLatin1String(name))); }
	
	QString m_id;
	VConnection *m_connection;
	int m_unknownCount;
	QVariantMap m_data;
};

#endif // VINFOREQUEST_H

