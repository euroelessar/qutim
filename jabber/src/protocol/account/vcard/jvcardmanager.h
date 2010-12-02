#ifndef JVCARDMANAGER_H
#define JVCARDMANAGER_H

#include <QObject>
#include <QSharedPointer>
#include <gloox/vcardhandler.h>
#include <jreen/vcard.h>

namespace jreen
{
class JID;
class IQ;
}

namespace gloox
{
class VCardManager;
class Client;
}

namespace Jabber
{

using namespace gloox;

class JAccount;
class JInfoRequest;
class JVCardManagerPrivate;

class JVCardManager : public QObject
{
	Q_OBJECT
	Q_DECLARE_PRIVATE(JVCardManager)
public:
	JVCardManager(JAccount *account);
	~JVCardManager();
	void fetchVCard(const QString &contact, JInfoRequest *request = 0);
	void storeVCard(jreen::VCard *vcard);
	//void handleVCardResult(VCardContext context, const JID &jid, StanzaError se);
	//VCardManager *manager();
public slots:
	void handleVCard(const jreen::JID &jid,QSharedPointer<jreen::VCard> fetchedVCard);
	void onIqReceived(const jreen::IQ &,int);
private:
	QScopedPointer<JVCardManagerPrivate> d_ptr;
};

}

#endif // JVCARDMANAGER_H
