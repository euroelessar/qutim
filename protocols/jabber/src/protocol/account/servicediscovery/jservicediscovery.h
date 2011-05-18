#ifndef JSERVICEDISCOVERY_H
#define JSERVICEDISCOVERY_H

#include <QObject>
#include <jreen/disco.h>

namespace Jreen
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
	void handleDiscoInfo(const Jreen::JID &from, QSharedPointer<Jreen::Disco::Info> info, int context);
	void handleDiscoItems(const Jreen::JID &from, QSharedPointer<Jreen::Disco::Items> items, int context);
	void handleDiscoError(const Jreen::JID &from, QSharedPointer<Jreen::Error> error, int context);
private slots:
	void handleIQ(const Jreen::IQ &iq);
	void handleIQ(const Jreen::IQ &iq,int context);
protected:
	void addDiscoIdentity(JDiscoItem &di,const Jreen::Disco::Identity &identity);
	void setActions(JDiscoItem &di);
private:
	QScopedPointer<JServicePrivate> p;
};
}

#endif //JSERVICEDISCOVERY_H
