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
#include "authorization_p.h"
#include "icqcontact_p.h"
#include "icqaccount.h"
#include "connection.h"
#include <qutim/authorizationdialog.h>
#include <qutim/notification.h>
#include <QApplication>
#include <qutim/servicemanager.h>

namespace qutim_sdk_0_3 {

namespace oscar {

Authorization *Authorization::self = 0;
const int actionType = 34644;

inline static LocalizedString authActionText(bool authorized)
{
	return authorized ?
			QT_TRANSLATE_NOOP("ContactList", "Reask authorization") :
			QT_TRANSLATE_NOOP("ContactList", "Ask authorization");
}

AuthorizeActionGenerator::AuthorizeActionGenerator() :
	ActionGenerator(QIcon(), LocalizedString(),
					Authorization::instance(),
					SLOT(onSendRequestClicked(QObject*)))
{
	setType(actionType);
}

void AuthorizeActionGenerator::createImpl(QAction *action, QObject *obj) const
{
	action->setText(authActionText(obj->property("authorizedBy").toBool()));
}

Authorization::Authorization() :
	FeedbagItemHandler(30)
{
	Q_ASSERT(!self);
	self = this;
	m_infos << SNACInfo(ListsFamily, ListsAuthRequest)
			<< SNACInfo(ListsFamily, ListsSrvAuthResponse);
	m_types << SsiBuddy;

	m_authActionGen = new AuthorizeActionGenerator;
	MenuController::addAction<IcqContact>(m_authActionGen);

	ActionGenerator *gen = new ActionGenerator(QIcon(), tr("Grant authorization"),
											   this, SLOT(onGrantAuthClicked(QObject*)));
	gen->setType(actionType);
	MenuController::addAction<IcqContact>(gen);
}

void Authorization::handleSNAC(AbstractConnection *conn, const SNAC &sn)
{
	switch ((sn.family() << 16) | sn.subtype()) {
	case ListsFamily << 16 | ListsAuthRequest: {
		QString uin = sn.read<QString, quint8>();
		QString reason = sn.read<QString, qint16>();
		IcqContact *contact = conn->account()->getContact(uin, true);
		if (contact) {
			QEvent *event = new qutim_sdk_0_3::Authorization::Reply(qutim_sdk_0_3::Authorization::Reply::New,
																	contact,
																	reason);
			qApp->postEvent(qutim_sdk_0_3::Authorization::service(),event);
		}
		debug() << QString("Authorization request from \"%1\" with reason \"%2").arg(uin).arg(reason);
		break;
	}
	case ListsFamily << 16 | ListsSrvAuthResponse: {
		QString uin = sn.read<QString, qint8>();
		bool isAccepted = sn.read<qint8>();
		QString reason = sn.read<QString, qint16>();
		IcqContact *contact = conn->account()->getContact(uin);
		if (contact) {
			if (isAccepted)
				onAuthChanged(contact, true);
			if(reason.isEmpty())
				reason = isAccepted ? QT_TRANSLATE_NOOP("ContactList", "Authorization request accepted")
									: QT_TRANSLATE_NOOP("ContactList", "Authorization request declined");
			QEvent *event = new qutim_sdk_0_3::Authorization::Reply(isAccepted ? qutim_sdk_0_3::Authorization::Reply::Accepted
																			   : qutim_sdk_0_3::Authorization::Reply::Rejected,
																	contact,
																	reason);
			qApp->postEvent(qutim_sdk_0_3::Authorization::service(),event);
		}
		debug() << "Auth response" << uin << isAccepted << reason;
		break;
	}
	}
}

bool Authorization::handleFeedbagItem(Feedbag *feedbag, const FeedbagItem &item, Feedbag::ModifyType type, FeedbagError error)
{
	Q_ASSERT(item.type() == SsiBuddy);
	if (type == Feedbag::Remove)
		return false;
	if (error == FeedbagError::RequiresAuthorization) {
		Q_ASSERT(!item.isInList());
		FeedbagItem itemCopy = item;
		itemCopy.setId(item.feedbag()->uniqueItemId(SsiBuddy));
		itemCopy.setField(SsiBuddyReqAuth);
		itemCopy.add();
	} else if (error != FeedbagError::NoError) {
		return false;
	}
	IcqContact *contact = feedbag->account()->getContact(item.name());
	Q_ASSERT(contact);
	onAuthChanged(contact, !item.containsField(SsiBuddyReqAuth));
	return false;
}

void Authorization::onSendRequestClicked(QObject *object)
{
	Q_ASSERT(qobject_cast<IcqContact*>(object) != 0);
	IcqContact *contact = reinterpret_cast<IcqContact*>(object);
	QEvent *event = new qutim_sdk_0_3::Authorization::Request(contact,
															  QT_TRANSLATE_NOOP("ContactList", "Please, authorize me"));
	qApp->postEvent(qutim_sdk_0_3::Authorization::service(),event);
}

void Authorization::onGrantAuthClicked(QObject *object)
{
	Q_ASSERT(qobject_cast<IcqContact*>(object) != 0);
	IcqContact *contact = reinterpret_cast<IcqContact*>(object);
	SNAC snac(ListsFamily, ListsGrantAuth);
	snac.append<quint8>(contact->id());
	snac.append<quint16>(0); // reason length
	snac.append<quint16>(0); // unknown
	contact->account()->connection()->send(snac);
}

void Authorization::onAuthChanged(IcqContact *contact, bool auth)
{
	contact->setProperty("authorizedBy", auth);
	foreach (QAction *action, m_authActionGen->actions(contact))
		action->setText(authActionText(auth));
}

} } // namespace qutim_sdk_0_3::oscar

