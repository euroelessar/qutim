#ifndef JVCARDMANAGER_H
#define JVCARDMANAGER_H

#include <QObject>
#include <QSharedPointer>
#include <jreen/vcardmanager.h>
#include "sdk/jabber.h"

namespace Jabber
{
class JInfoRequest;

class JVCardManager : public QObject, public JabberExtension
{
	Q_OBJECT
	Q_INTERFACES(Jabber::JabberExtension)
	Q_CLASSINFO("DebugName", "Jabber::VCardManager")
public:
	JVCardManager();
	~JVCardManager();
	
	JInfoRequest *fetchVCard(const QString &contact, bool create);
	void storeVCard(const Jreen::VCard::Ptr &vcard);
	virtual void init(qutim_sdk_0_3::Account *account);
	bool eventFilter(QObject *obj, QEvent *event);

protected slots:
	void onConnected();
	void onVCardReceived(const Jreen::VCard::Ptr &vcard, const Jreen::JID &jid);
	
private:
	friend class JInfoRequest;
	bool m_autoLoad;
	qutim_sdk_0_3::Account *m_account;
	Jreen::Client *m_client;
	Jreen::VCardManager *m_manager;
};

}

#endif // JVCARDMANAGER_H
