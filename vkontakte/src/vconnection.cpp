#include "vconnection.h"
#include "vaccount.h"

struct VConnectionPrivate
{
	VAccount *account;
	QString remixPasswd;
	QString sid;
	VConnectionState state;
};

VConnection::VConnection(VAccount* account, QObject* parent): QNetworkAccessManager(parent), d_ptr(new VConnectionPrivate)
{
	Q_D(VConnection);
	d->account = account;
	d->state = Disconnected;
}

void VConnection::connectToHost(const QString& passwd)
{

}

void VConnection::disconnectFromHost()
{
	emit connectionStateChanged(Disconnected);
}
