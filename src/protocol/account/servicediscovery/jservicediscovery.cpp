#include "jservicediscovery.h"
#include "jdiscoitem.h"
#include "jservicereceiver.h"
#include "../jaccount.h"
#include <qutim/debug.h>

namespace Jabber
{
	struct JServicePrivate
	{
		JAccount *account;
		QMap<int, JDiscoItem *> items;
		QMap<int, JServiceReceiver *> receivers;
		int count;
	};

	JServiceDiscovery::JServiceDiscovery(JAccount *account)
			: p(new JServicePrivate)
	{
		p->account = account;
		p->count = 0;
		debug() << Q_FUNC_INFO;
	}

	JServiceDiscovery::~JServiceDiscovery()
	{
	}

	int JServiceDiscovery::getInfo(JServiceReceiver *receiver, JDiscoItem *di)
	{
		int id = p->count++;
		p->receivers.insert(id, receiver);
		debug() << p->items;
		debug() << id << di;
		p->items.insert(id, di);
		p->account->connection()->client()->disco()->getDiscoInfo(di->jid().toStdString(),
				di->node().toStdString(), dynamic_cast<DiscoHandler *>(this), id);
		return id;
	}

	int JServiceDiscovery::getItems(JServiceReceiver *receiver, JDiscoItem *di)
	{
		int id = p->count++;
		p->receivers.insert(id, receiver);
		p->items.insert(id, di);
		p->account->connection()->client()->disco()->getDiscoItems(di->jid().toStdString(),
				di->node().toStdString(), dynamic_cast<DiscoHandler *>(this), id);
		return id;
	}

	void JServiceDiscovery::handleDiscoInfo(const JID &from, const Disco::Info &info, int context)
	{
		JDiscoItem *di = p->items.take(context);
		di->setJID(QString::fromStdString(from.full()));
		di->setNode(QString::fromStdString(info.node()));
		foreach (std::string feature, info.features())
			di->addFeature(QString::fromStdString(feature));
		foreach (Disco::Identity *identity, info.identities())
			addDiscoIdentity(di, identity);
		setActions(di);
		p->receivers.take(context)->setInfo(context);
	}

	void JServiceDiscovery::handleDiscoItems(const JID &from, const Disco::Items &items, int context)
	{
		QList<JDiscoItem *> discoItems;
		foreach (Disco::Item *item, items.items()) {
			JDiscoItem *di = new JDiscoItem();
			di->setExpand(false);
			di->setName(QString::fromStdString(item->name()).replace("\n", " | "));
			di->setJID(QString::fromStdString(item->jid().full()));
			di->setNode(QString::fromStdString(item->node()));
			discoItems << di;
		}
		p->items.remove(context);
		p->receivers.take(context)->setItems(context, discoItems);
	}

	void JServiceDiscovery::handleDiscoError(const JID &from, const Error *error, int context)
	{
		JDiscoItem *di = p->items.take(context);
		di->setJID(QString::fromStdString(from.full()));
		di->setError(QString::fromStdString(error->text()));
		if (di->error().isEmpty()) {
			QString errorText;
			switch (error->error()) {
			case StanzaErrorBadRequest:
				errorText = tr("The sender has sent XML that is malformed or that cannot be processed.");
				break;
			case StanzaErrorConflict:
				errorText = tr("Access cannot be granted because an existing resource or session exists with the same name or address.");
				break;
			case StanzaErrorFeatureNotImplemented:
				errorText = tr("The feature requested is not implemented by the recipient or server and therefore cannot be processed.");
				break;
			case StanzaErrorForbidden:
				errorText = tr("The requesting entity does not possess the required permissions to perform the action.");
				break;
			case StanzaErrorGone:
				errorText = tr("The recipient or server can no longer be contacted at this address.");
				break;
			case StanzaErrorInternalServerError:
				errorText = tr("The server could not process the stanza because of a misconfiguration or an otherwise-undefined internal server error.");
				break;
			case StanzaErrorItemNotFound:
				errorText = tr("The addressed JID or item requested cannot be found.");
				break;
			case StanzaErrorJidMalformed:
				errorText = tr("The sending entity has provided or communicated an XMPP address or aspect thereof that does not adhere to the syntax defined in Addressing Scheme.");
				break;
			case StanzaErrorNotAcceptable:
				errorText = tr("The recipient or server understands the request but is refusing to process it because it does not meet criteria defined by the recipient or server.");
				break;
			case StanzaErrorNotAllowed:
				errorText = tr("The recipient or server does not allow any entity to perform the action.");
				break;
			case StanzaErrorNotAuthorized:
					errorText = tr("The sender must provide proper credentials before being allowed to perform the action, or has provided impreoper credentials.");
				break;
			case StanzaErrorNotModified:
				errorText = tr("The item requested has not changed since it was last requested.");
				break;
			case StanzaErrorPaymentRequired:
				errorText = tr("The requesting entity is not authorized to access the requested service because payment is required.");
				break;
			case StanzaErrorRecipientUnavailable:
				errorText = tr("The intended recipient is temporarily unavailable.");
				break;
			case StanzaErrorRedirect:
				errorText = tr("The recipient or server is redirecting requests for this information to another entity, usually temporarily.");
				break;
			case StanzaErrorRegistrationRequired:
				errorText = tr("The requesting entity is not authorized to access the requested service because registration is required.");
				break;
			case StanzaErrorRemoteServerNotFound:
				errorText = tr("A remote server or service specified as part or all of the JID of the intended recipient does not exist.");
				break;
			case StanzaErrorRemoteServerTimeout:
				errorText = tr("A remote server or service specified as part or all of the JID of the intended recipient could not be contacted within a reasonable amount of time.");
				break;
			case StanzaErrorResourceConstraint:
				errorText = tr("The server or recipient lacks the system resources necessary to service the request.");
				break;
			case StanzaErrorServiceUnavailable:
				errorText = tr("The server or recipient does not currently provide the requested service.");
				break;
			case StanzaErrorSubscribtionRequired:
				errorText = tr("The requesting entity is not authorized to access the requested service because a subscription is required.");
				break;
			case StanzaErrorUndefinedCondition:
				errorText = tr("The unknown error condition.");
				break;
			case StanzaErrorUnexpectedRequest:
				errorText = tr("The recipient or server understood the request but was not expecting it at this time.");
				break;
			case StanzaErrorUnknownSender:
				errorText = tr("The stanza 'from' address specified by a connected client is not valid for the stream.");
			}
			di->setError(errorText);
		}
		p->receivers.take(context)->setError(context, di);
	}

	bool JServiceDiscovery::handleDiscoSet(const IQ &iq)
	{
		return false;
	}

	void JServiceDiscovery::addDiscoIdentity(JDiscoItem *di, Disco::Identity *identity)
	{
		if (di->name().isEmpty())
			di->setName(QString::fromStdString(identity->name()).replace("\n", " | "));
		JDiscoItem::JDiscoIdentity discoIdentity;
		discoIdentity.name = QString::fromStdString(identity->name()).replace("\n", " | ");
		discoIdentity.category = QString::fromStdString(identity->category());
		discoIdentity.type = QString::fromStdString(identity->type());
		di->addIdentity(discoIdentity);
	}

	void JServiceDiscovery::setActions(JDiscoItem *di)
	{
		bool isIRC = false;
		foreach (JDiscoItem::JDiscoIdentity identity, di->identities())
			if (identity.category == "conference" && identity.type == "irc")
				isIRC = true;
		di->addAction(JDiscoItem::JDiscoAdd);
		if (di->hasFeature("http://jabber.org/protocol/muc")
				&& (!QString::fromStdString(JID(di->jid().toStdString()).username()).isEmpty()
				|| isIRC))
			di->addAction(JDiscoItem::JDiscoJoin);
		if (di->hasFeature("http://jabber.org/protocol/bytestreams"))
			di->addAction(JDiscoItem::JDiscoProxy);
		if(di->hasFeature("http://jabber.org/protocol/muc#register")
				|| di->hasFeature("jabber:iq:register"))
			di->addAction(JDiscoItem::JDiscoRegister);
		if (di->hasFeature("jabber:iq:search"))
			di->addAction(JDiscoItem::JDiscoSearch);
		if (di->hasFeature("vcard-temp"))
			di->addAction(JDiscoItem::JDiscoVCard);
		if (di->hasFeature("http://jabber.org/protocol/disco#items")
				|| di->hasFeature("http://jabber.org/protocol/muc") && !isIRC)
			di->setExpand(true);
		if (di->hasIdentity("automation")) {
			bool expand = false;
			foreach (JDiscoItem::JDiscoIdentity identity, di->identities())
				if (identity.type == "command-list")
					expand = true;
			if (expand)
				di->setExpand(true);
			else
				di->addAction(JDiscoItem::JDiscoExecute);
		} else if (di->hasFeature("http://jabber.org/protocol/commands")) {
			di->setExpand(true);
		}
	}
}
