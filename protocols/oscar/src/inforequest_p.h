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

#ifndef OSCARINFOREQUEST_H
#define OSCARINFOREQUEST_H

#include "qutim/inforequest.h"
#include "metainfo/infometarequest.h"
#include "metainfo/updateaccountinfometarequest.h"
#include <QPointer>

namespace qutim_sdk_0_3 {

namespace oscar {

class IcqContact;

class IcqInfoRequestFactory : public InfoRequestFactory
{
	Q_OBJECT
public:
	IcqInfoRequestFactory(IcqAccount *account);
	virtual SupportLevel supportLevel(QObject *object);
protected:
	virtual InfoRequest *createrDataFormRequest(QObject *object);
	virtual bool startObserve(QObject *object);
	virtual bool stopObserve(QObject *object);
private slots:
	void onAccountStatusChanged(const qutim_sdk_0_3::Status &status,
								const qutim_sdk_0_3::Status &previous);
private:
	QSet<IcqContact*> m_contacts;
	IcqAccount *m_account;
};

class IcqInfoRequest : public InfoRequest
{
	Q_OBJECT
public:
	IcqInfoRequest(IcqAccount *account);
	IcqInfoRequest(IcqContact *contact);
	virtual ~IcqInfoRequest();
protected:
	virtual DataItem createDataItem() const;
	QVariant getValue(const QString &name) const;
	virtual void doRequest(const QSet<QString> &hints);
	virtual void doUpdate(const DataItem &dataItem);
	virtual void doCancel();
private slots:
	void onRequestDone(bool ok);
	void onInfoUpdated(bool ok);
private:
	void addItem(const MetaField &field, DataItem &group) const;
	void handleError(AbstractMetaRequest *request);
	QPointer<ShortInfoMetaRequest> m_request;
	QPointer<UpdateAccountInfoMetaRequest> m_updater;
	MetaInfoValuesHash m_values;
	bool m_accountInfo;
	DataItem m_item;
	union {
		IcqAccount *m_account;
		IcqContact *m_contact;
		QObject *m_object;
	};
};

} } // namespace qutim_sdk_0_3::oscar

#endif // OSCARINFOREQUEST_H

