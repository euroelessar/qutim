#include "jservicediscovery.h"
#include "jdiscoitem.h"
#include "jservicereceiver.h"
#include "../jaccount.h"
#include <jreen/client.h>
#include <jreen/disco.h>
#include <jreen/iq.h>
#include <jreen/error.h>
namespace Jabber
{

struct JServicePrivate
{
	JAccount *account;
	QMap<int, JDiscoItem> items;
	QMap<int, QObject *> receivers;
	int context;
};

JServiceDiscovery::JServiceDiscovery(JAccount *account)
	: p(new JServicePrivate)
{
	p->account = account;
	p->context = 1;
	connect(account->client(),SIGNAL(newIQ(jreen::IQ)),SLOT(handleIQ(jreen::IQ)));
}

JServiceDiscovery::~JServiceDiscovery()
{
}

void JServiceDiscovery::handleIQ(const jreen::IQ &iq)
{
	//	QSharedPointer<jreen::Disco::Info> info = iq.findExtension<jreen::Disco::Info>();
	//	if(info)
	//		handleDiscoInfo(iq.from(),info);
}

void JServiceDiscovery::handleIQ(const jreen::IQ &iq,int context)
{
	if(QSharedPointer<jreen::Disco::Info> info = iq.findExtension<jreen::Disco::Info>())
		handleDiscoInfo(iq.from(),info,context);
	else if(QSharedPointer<jreen::Disco::Items> items = iq.findExtension<jreen::Disco::Items>())
		handleDiscoItems(iq.from(),items,context);
}

//int JServiceDiscovery::getInfo(JServiceReceiver *receiver, const JDiscoItem &di)
int JServiceDiscovery::getInfo(QObject *receiver, const JDiscoItem &di)
{
	int id = p->context++;
	p->receivers.insert(id, receiver);
	p->items.insert(id, di);
	jreen::IQ info(jreen::IQ::Get,di.jid());
	info.addExtension(new jreen::Disco::Info(di.node()));
	p->account->client()->send(info,this,SLOT(handleIQ(jreen::IQ,int)),id);
	return id;
}

//int JServiceDiscovery::getItems(JServiceReceiver *receiver, const JDiscoItem &di)
int JServiceDiscovery::getItems(QObject *receiver, const JDiscoItem &di)
{
	int id = p->context++;
	p->receivers.insert(id, receiver);
	p->items.insert(id, di);
	jreen::IQ discoItems(jreen::IQ::Get,di.jid());
	discoItems.addExtension(new jreen::Disco::Items(di.node()));
	p->account->client()->send(discoItems,this,SLOT(handleIQ(jreen::IQ,int)),id);
	return id;
}

void JServiceDiscovery::handleDiscoInfo(const jreen::JID &from,
										QSharedPointer<jreen::Disco::Info> info,
										int context)
{
	JDiscoItem di = p->items.take(context);
	di.setJID(from.full());
	di.setNode(info->node());
	foreach (QString feature, info->features())
		di.addFeature(feature);
	foreach (const jreen::Disco::Identity &identity, info->identities())
		addDiscoIdentity(di, identity);
	setActions(di);
	if (JServiceReceiver *receiver = qobject_cast<JServiceReceiver *>(p->receivers.take(context)))
		receiver->setInfo(context);
}

void JServiceDiscovery::handleDiscoItems(const jreen::JID &from,
										 QSharedPointer<jreen::Disco::Items> items,
										 int context)
{
	QList<JDiscoItem> discoItems;
	foreach (jreen::Disco::Item item, items->items()) {
		JDiscoItem di;
		di.setExpandable(false);
		di.setName(item.name.replace("\n", " | "));
		di.setJID(item.jid.full());
		di.setNode(item.node);
		discoItems << di;
	}
	p->items.remove(context);
	//if (JServiceReceiver *receiver = p->receivers.take(context))
	if (JServiceReceiver *receiver = qobject_cast<JServiceReceiver *>(p->receivers.take(context)))
		receiver->setItems(context, discoItems);
}

void JServiceDiscovery::handleDiscoError(const jreen::JID &from,
										 QSharedPointer<jreen::Error> error, int context)
{
	JDiscoItem di = p->items.take(context);
	di.setJID(from.full());
	if (di.error().isEmpty()) {
		QString errorText;
		// TODO: Move this translation to Jabber utils
		switch (error->condition()) {
		case jreen::Error::BadRequest:
			errorText = tr("The sender has sent XML that is malformed or that cannot be processed.");
			break;
		case jreen::Error::Conflict:
			errorText = tr("Access cannot be granted because an existing resource or session exists with the same name or address.");
			break;
		case jreen::Error::FeatureNotImplemented:
			errorText = tr("The feature requested is not implemented by the recipient or server and therefore cannot be processed.");
			break;
		case jreen::Error::Forbidden:
			errorText = tr("The requesting entity does not possess the required permissions to perform the action.");
			break;
		case jreen::Error::Gone:
			errorText = tr("The recipient or server can no longer be contacted at this address.");
			break;
		case jreen::Error::InternalServerError:
			errorText = tr("The server could not process the stanza because of a misconfiguration or an otherwise-undefined internal server error.");
			break;
		case jreen::Error::ItemNotFound:
			errorText = tr("The addressed JID or item requested cannot be found.");
			break;
		case jreen::Error::JidMalformed:
			errorText = tr("The sending entity has provided or communicated an XMPP address or aspect thereof that does not adhere to the syntax defined in Addressing Scheme.");
			break;
		case jreen::Error::NotAcceptable:
			errorText = tr("The recipient or server understands the request but is refusing to process it because it does not meet criteria defined by the recipient or server.");
			break;
		case jreen::Error::NotAllowed:
			errorText = tr("The recipient or server does not allow any entity to perform the action.");
			break;
		case jreen::Error::NotAuthorized:
			errorText = tr("The sender must provide proper credentials before being allowed to perform the action, or has provided impreoper credentials.");
			break;
		case jreen::Error::NotModified:
			errorText = tr("The item requested has not changed since it was last requested.");
			break;
		case jreen::Error::PaymentRequired:
			errorText = tr("The requesting entity is not authorized to access the requested service because payment is required.");
			break;
		case jreen::Error::RecipientUnavailable:
			errorText = tr("The intended recipient is temporarily unavailable.");
			break;
		case jreen::Error::Redirect:
			errorText = tr("The recipient or server is redirecting requests for this information to another entity, usually temporarily.");
			break;
		case jreen::Error::RegistrationRequired:
			errorText = tr("The requesting entity is not authorized to access the requested service because registration is required.");
			break;
		case jreen::Error::RemoteServerNotFound:
			errorText = tr("A remote server or service specified as part or all of the JID of the intended recipient does not exist.");
			break;
		case jreen::Error::RemoteServerTimeout:
			errorText = tr("A remote server or service specified as part or all of the JID of the intended recipient could not be contacted within a reasonable amount of time.");
			break;
		case jreen::Error::ResourceConstraint:
			errorText = tr("The server or recipient lacks the system resources necessary to service the request.");
			break;
		case jreen::Error::ServiceUnavailable:
			errorText = tr("The server or recipient does not currently provide the requested service.");
			break;
		case jreen::Error::SubscribtionRequired:
			errorText = tr("The requesting entity is not authorized to access the requested service because a subscription is required.");
			break;
		case jreen::Error::UndefinedCondition:
			errorText = tr("The unknown error condition.");
			break;
		case jreen::Error::UnexpectedRequest:
			errorText = tr("The recipient or server understood the request but was not expecting it at this time.");
			break;
		case jreen::Error::UnknownSender:
			errorText = tr("The stanza 'from' address specified by a connected client is not valid for the stream.");
			break;
		case jreen::Error::Undefined:
			errorText = tr("No stanza error occured. You're just sleeping.");
			break;
		}
		QString additionalText = QString();
		if (!additionalText.isEmpty()) {
			errorText += '\n';
			errorText += additionalText;
		}
		di.setError(errorText);
	}
	if (JServiceReceiver *receiver = qobject_cast<JServiceReceiver *>(p->receivers.take(context)))
		receiver->setError(context);
}

void JServiceDiscovery::addDiscoIdentity(JDiscoItem &di,const jreen::Disco::Identity &identity)
{
	if (di.name().isEmpty()) {
		QString name = identity.name;
		di.setName(name.replace("\n", " | "));
	}
	di.addIdentity(identity);
}

void JServiceDiscovery::setActions(JDiscoItem &di)
{
//	bool isIRC = false;
//	foreach (jreen::Disco::Identity identity, di.identities())
//		if (identity.category == "conference" && identity.type == "irc")
//			isIRC = true;
//	di.addAction(JDiscoItem::ActionAdd);
//	if (di.hasFeature(QString::fromStdString(gloox::XMLNS_MUC))
//		&& (!jreen::JID(di.jid()).node().isEmpty() || isIRC))
//		di.addAction(JDiscoItem::ActionJoin);
//	if (di.hasFeature(QString::fromStdString(gloox::XMLNS_BYTESTREAMS)))
//		di.addAction(JDiscoItem::ActionProxy);
//	if(di.hasFeature("http://jabber.org/protocol/muc#register")
//			|| di.hasFeature(QString::fromStdString(gloox::XMLNS_REGISTER)))
//		di.addAction(JDiscoItem::ActionRegister);
//	if (di.hasFeature(QString::fromStdString(gloox::XMLNS_SEARCH)))
//		di.addAction(JDiscoItem::ActionSearch);
//	if (di.hasFeature(QString::fromStdString(gloox::XMLNS_VCARD_TEMP)))
//		di.addAction(JDiscoItem::ActionVCard);
//	if (di.hasFeature(QString::fromStdString(gloox::XMLNS_DISCO_ITEMS))
//			|| (di.hasFeature(QString::fromStdString(gloox::XMLNS_MUC)) && !isIRC)
//			|| (di.features().isEmpty() && di.identities().isEmpty()))
//		di.setExpandable(true);
//	if (di.hasIdentity("automation")) {
//		bool expand = false;
//		foreach (jreen::Disco::Identity identity, di.identities())
//			if (identity.type == "command-list")
//				expand = true;
//		if (expand)
//			di.setExpandable(true);
//		//			else
//		di.addAction(JDiscoItem::ActionExecute);
//	} else if (di.hasFeature(QString::fromStdString(gloox::XMLNS_ADHOC_COMMANDS))) {
//		di.setExpandable(true);
//		di.addAction(JDiscoItem::ActionExecute);
//	}
}

}
