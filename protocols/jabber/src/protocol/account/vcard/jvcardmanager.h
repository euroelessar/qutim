#ifndef JVCARDMANAGER_H
#define JVCARDMANAGER_H

#include <QObject>
#include <QSharedPointer>
#include <jreen/vcard.h>

namespace Jreen
{
class JID;
class IQ;
}

namespace Jabber
{
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
	void storeVCard(const Jreen::VCard::Ptr &vcard);
	JAccount *account() const;
protected slots:
	void handleIQ(const Jreen::IQ &iq);
	void onIqReceived(const Jreen::IQ &,int);
private:
	QScopedPointer<JVCardManagerPrivate> d_ptr;
};

}

#endif // JVCARDMANAGER_H
