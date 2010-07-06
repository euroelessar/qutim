#include "authorization_p.h"
#include "icqcontact.h"
#include "icqaccount.h"
#include "connection.h"
#include <qutim/authorizationdialog.h>
#include <qutim/notificationslayer.h>

namespace qutim_sdk_0_3 {

namespace oscar {

Authorization *Authorization::self = 0;

AuthorizeActionGenerator::AuthorizeActionGenerator() :
	ActionGenerator(QIcon(), QT_TRANSLATE_NOOP("ContactList", "Ask authorization"),
					Authorization::instance(), SLOT(onSendRequestClicked(QAction*, QObject*)))
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

Authorization::Authorization()
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
		IcqContact *contact = conn->account()->getContact(uin);
		if (contact) {
			AuthorizationDialog *dialog = AuthorizationDialog::request(contact, reason);
			connect(dialog, SIGNAL(finished(bool)), SLOT(sendAuthResponse(bool)));
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
			Notifications::sendNotification(Notifications::System, contact, reason); // TODO:
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
	IcqContact *contact = feedbag->account()->getContact(item.name(), true);
	contact->setProperty("authorizedBy", !item.containsField(SsiBuddyReqAuth));
	return false;
}

void Authorization::sendAuthResponse(bool auth)
{
	AuthorizationDialog *dialog = qobject_cast<AuthorizationDialog *>(sender());
	Q_ASSERT(dialog);
	IcqContact *contact = qobject_cast<IcqContact*>(dialog->contact());
	SNAC snac(ListsFamily, ListsCliAuthResponse);
	snac.append<qint8>(contact->id()); // uin.
	snac.append<qint8>(auth ? 0x01 : 0x00); // auth flag.
	snac.append<qint16>(dialog->text());
	contact->account()->connection()->send(snac);
}

void Authorization::onSendRequestClicked(QAction *action, QObject *object)
{
	Q_ASSERT(qobject_cast<IcqContact*>(object) != 0);
	IcqContact *contact = reinterpret_cast<IcqContact*>(object);
	AuthorizationDialog *dialog = AuthorizationDialog::request(contact, QT_TRANSLATE_NOOP("ContactList", "Please, authorize me"), false);
	connect(dialog, SIGNAL(finished(bool)), SLOT(sendAuthRequest()));
}

void Authorization::sendAuthRequest()
{
	AuthorizationDialog *dialog = qobject_cast<AuthorizationDialog *>(sender());
	Q_ASSERT(dialog);
	IcqContact *contact = qobject_cast<IcqContact*>(dialog->contact());
	Q_ASSERT(contact);
	SNAC snac(ListsFamily, ListsRequestAuth);
	snac.append<qint8>(contact->id()); // uin.
	snac.append<qint16>(dialog->text());
	snac.append<quint16>(0);
	contact->account()->connection()->send(snac);
}

} } // namespace qutim_sdk_0_3::oscar
