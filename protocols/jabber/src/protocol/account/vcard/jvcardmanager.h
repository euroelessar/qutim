/****************************************************************************
**
** qutIM instant messenger
**
** Copyright (c) 2011 Ruslan Nigmatullin <euroelessar@gmail.com>
**                    Alexey Prokhin <alexey.prokhin@yandex.ru>
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
** along with this program. If not, see http://www.gnu.org/licenses/.
** $QUTIM_END_LICENSE$
**
****************************************************************************/

#ifndef JVCARDMANAGER_H
#define JVCARDMANAGER_H

#include <QObject>
#include <QSharedPointer>
#include <jreen/vcard.h>
#include <qutim/inforequest.h>
#include <qutim/status.h>

namespace Jreen
{
class JID;
class IQ;
}

namespace Jabber
{
using namespace qutim_sdk_0_3;

class JAccount;
class JInfoRequest;
class JVCardManagerPrivate;

class JVCardManager : public QObject, public InfoRequestFactory
{
	Q_OBJECT
	Q_DECLARE_PRIVATE(JVCardManager)
	Q_CLASSINFO("DebugName", "Jabber::VCardManager")
public:
	JVCardManager(JAccount *account);
	~JVCardManager();
	void fetchVCard(const QString &contact, JInfoRequest *request = 0);
	bool containsRequest(const QString &contact);
	void storeVCard(const Jreen::VCard::Ptr &vcard);
	JAccount *account() const;
protected: // InformationRequestFactory stuff
	virtual SupportLevel supportLevel(QObject *object);
	virtual InfoRequest *createrDataFormRequest(QObject *object);
	virtual bool startObserve(QObject *object);
	virtual bool stopObserve(QObject *object);
protected slots:
	void handleIQ(const Jreen::IQ &iq);
	void onAccountStatusChanged(const qutim_sdk_0_3::Status &status,
								const qutim_sdk_0_3::Status &previous);
	void onIqReceived(const Jreen::IQ &,int);
private:
	QScopedPointer<JVCardManagerPrivate> d_ptr;
};

}

#endif // JVCARDMANAGER_H
