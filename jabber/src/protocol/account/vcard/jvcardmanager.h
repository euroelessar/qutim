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
	Q_CLASSINFO("DebugName", "Jabber::VCardManager")
public:
	JVCardManager(JAccount *account);
	~JVCardManager();
	void fetchVCard(const QString &contact, JInfoRequest *request = 0);
	bool containsRequest(const QString &contact);
	void storeVCard(jreen::VCard *vcard);
	JAccount *account() const;
protected slots:
	void handleIQ(const jreen::IQ &iq);
	void onIqReceived(const jreen::IQ &,int);
private:
	QScopedPointer<JVCardManagerPrivate> d_ptr;
};

}

#endif // JVCARDMANAGER_H
