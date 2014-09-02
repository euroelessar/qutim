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

#ifndef JVCARDMANAGER_H
#define JVCARDMANAGER_H

#include <QObject>
#include <QSharedPointer>
#include <jreen/vcardmanager.h>
#include <qutim/inforequest.h>
#include <qutim/status.h>
#include <qutim/chatunit.h>
#include "../../../sdk/jabber.h"

namespace Jabber
{
using namespace qutim_sdk_0_3;

class JInfoRequest;

class JVCardManager : public InfoRequestFactory, public JabberExtension
{
	Q_OBJECT
	Q_INTERFACES(Jabber::JabberExtension)
	Q_CLASSINFO("DebugName", "Jabber::VCardManager")
public:
	JVCardManager();
	~JVCardManager();
	
	virtual void init(qutim_sdk_0_3::Account *account);
	static QString ensurePhoto(const Jreen::VCard::Photo &photo, QString *path = NULL);

protected: // InformationRequestFactory stuff
	virtual SupportLevel supportLevel(QObject *object);
	virtual InfoRequest *createrDataFormRequest(QObject *object);
	virtual bool startObserve(QObject *object);
	virtual bool stopObserve(QObject *object);

protected slots:
	void onConnected();
	void onVCardReceived(const Jreen::VCard::Ptr &vcard, const Jreen::JID &jid);
	void onVCardUpdateDetected(const Jreen::JID &jid, const Jreen::VCardUpdate::Ptr &update);
	void onAccountStatusChanged(const qutim_sdk_0_3::Status &status,
								const qutim_sdk_0_3::Status &previous);
	
private:
	friend class JInfoRequest;
	bool m_autoLoad;
	qutim_sdk_0_3::Account *m_account;
	Jreen::Client *m_client;
	Jreen::VCardManager *m_manager;
	QSet<qutim_sdk_0_3::ChatUnit*> m_observedUnits;
};

}

#endif // JVCARDMANAGER_H

