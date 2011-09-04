#ifndef JACCOUNT_P_H
#define JACCOUNT_P_H

#include "jaccount.h"
#include "muc/jbookmarkmanager.h"
#include <qutim/passworddialog.h>
//Jreen
#include <jreen/client.h>
#include <jreen/privacymanager.h>
//#include <jreen/pubsubmanager.h>
#include "../../sdk/jabber.h"
#include <QSignalMapper>
namespace Jreen
{
class Client;
}

namespace Jabber {

typedef QHash<QString, QHash<QString, QString> > Identities;

class JAccountPrivate
{
	Q_DECLARE_PUBLIC(JAccount)
public:
	inline JAccountPrivate(JAccount *q) : q_ptr(q),keepStatus(false) {}
	inline ~JAccountPrivate() {}
	//Jreen
	QScopedPointer<Jreen::Client> client;
	JSoftwareDetection *softwareDetection;
	JRoster *roster;
	Jreen::PrivacyManager *privacyManager;
	JMessageSessionManager *messageSessionManager;
	Jreen::PubSub::Manager *pubSubManager;
	Jreen::PrivateXml *privateXml;
	JMessageHandler *messageHandler;
	JAccount *q_ptr;
	QString nick;
	QString avatar;
	bool keepStatus;
	Status status;
	bool isOnline;
	QList<JabberExtension *> extensions;
	QPointer<JMUCManager> conferenceManager;
	QPointer<PasswordDialog> passwordDialog;
	QSignalMapper signalMapper;
	int loadedModules;
	int priority;
	
	void applyStatus(const Status &status);
	void setPresence(Jreen::Presence);
	void _q_set_nick(const QString &nick);
	void _q_disconnected(Jreen::Client::DisconnectReason);
	void _q_init_extensions(const QSet<QString> &features);
	void _q_on_module_loaded(int i);
	void _q_connected();
	void _q_on_password_finished(int result);

	//old code
	Identities identities;
};

}


#endif // JACCOUNT_P_H
