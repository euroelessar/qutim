#include "authorization_p.h"
#include "icqcontact.h"
#include "icqaccount.h"
#include "connection.h"
#include <qutim/authorizationdialog.h>
#include <qutim/notificationslayer.h>
#include <QApplication>
#include <qutim/servicemanager.h>

namespace qutim_sdk_0_3 {

namespace oscar {

Authorization *Authorization::self = 0;

AuthorizeActionGenerator::AuthorizeActionGenerator() :
	ActionGenerator(QIcon(), QT_TRANSLATE_NOOP("ContactList", "Ask authorization"),
		Authorization::instance(), SLOT(onSendRequestClicked(QObject*)))
{
	setPriority(50);
	setType(34644);
}

void AuthorizeActionGenerator::showImpl(QAction *action, QObject *object)
{
	Q_ASSERT(qobject_cast<IcqContact*>(object) != 0);
	IcqContact *contact = reinterpret_cast<IcqContact*>(object);
	Status::Type status = contact->account()->status().type();
	action->setVisible(status != Status::Offline && status != Status::Connecting &&
					   !contact->property("authorizedBy").toBool());
}

Authorization::Authorization() :
	FeedbagItemHandler(30)
{
	Q_ASSERT(!self);
	self = this;
	m_infos << SNACInfo(ListsFamily, ListsAuthRequest)
			<< SNACInfo(ListsFamily, ListsSrvAuthResponse);
	m_types << SsiBuddy;
	MenuController::addAction<IcqContact>(new AuthorizeActionGenerator);
}

void Authorization::handleSNAC(AbstractConnection *conn, const SNAC &sn)
{
	switch ((sn.family() << 16) | sn.subtype()) {
	case ListsFamily << 16 | ListsAuthRequest: {
		sn.skipData(8); // cookie
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
		sn.skipData(8); // cookie
		QString uin = sn.read<QString, qint8>();
		bool isAccepted = sn.read<qint8>();
		QString reason = sn.read<QString, qint16>();
		IcqContact *contact = conn->account()->getContact(uin);
		if (contact) {
			if (isAccepted)
				contact->setProperty("authorizedBy", true);
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
	static const int SsiBuddyReqAuth = 0x0066;
	if (type == Feedbag::Remove)
		return false;
	if (error == FeedbagError::RequiresAuthorization) {
		Q_ASSERT(!item.isInList());
		FeedbagItem i = item;
		i.setField(SsiBuddyReqAuth);
		i.update();
	} else if (error != FeedbagError::NoError) {
		return false;
	}
	IcqContact *contact = feedbag->account()->getContact(item.name());
	Q_ASSERT(contact);
	contact->setProperty("authorizedBy", !item.containsField(SsiBuddyReqAuth));
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

} } // namespace qutim_sdk_0_3::oscar
