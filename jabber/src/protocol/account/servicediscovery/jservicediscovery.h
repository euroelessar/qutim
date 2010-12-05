#ifndef JSERVICEDISCOVERY_H
#define JSERVICEDISCOVERY_H

#include <QObject>
#include <jreen/disco.h>

namespace jreen
{
class IQ;
class JID;
class Error;
}

namespace Jabber
{

struct JServicePrivate;
class JAccount;
class JDiscoItem;
class JServiceReceiver;

class JServiceDiscovery : public QObject
{
	Q_OBJECT
public:
	JServiceDiscovery(JAccount *account);
	~JServiceDiscovery();
	//int getInfo(JServiceReceiver *receiver, const JDiscoItem &di);
	//int getItems(JServiceReceiver *receiver, const JDiscoItem &di);
	int getInfo(QObject *receiver, const JDiscoItem &di);
	int getItems(QObject *receiver, const JDiscoItem &di);
	void handleDiscoInfo(const jreen::JID &from, QSharedPointer<jreen::Disco::Info> info, int context);
	void handleDiscoItems(const jreen::JID &from, QSharedPointer<jreen::Disco::Items> items, int context);
	void handleDiscoError(const jreen::JID &from, QSharedPointer<jreen::Error> error, int context);
private slots:
	void handleIQ(const jreen::IQ &iq);
	void handleIQ(const jreen::IQ &iq,int context);
protected:
	void addDiscoIdentity(JDiscoItem &di,const jreen::Disco::Identity &identity);
	void setActions(JDiscoItem &di);
private:
	QScopedPointer<JServicePrivate> p;
};
}

#endif //JSERVICEDISCOVERY_H
