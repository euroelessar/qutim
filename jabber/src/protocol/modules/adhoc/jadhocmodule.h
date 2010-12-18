#ifndef JADHOCMODULE_H
#define JADHOCMODULE_H

#include <QObject>
#include "sdk/jabber.h"
#include <gloox/client.h>
#include <QHash>

//namespace Jabber
//{
//class JContactResource;
//class JAccount;

//class JAdHocModule : public QObject, public JabberExtension
//{
//	Q_OBJECT
//	Q_INTERFACES(Jabber::JabberExtension)
//public:
//	JAdHocModule();
//	void init(qutim_sdk_0_3::Account *account, const JabberParams &params);
//public slots:
//	void onActionClicked(JContactResource *resource = 0);
//private:
//	JAccount *m_account;
//	gloox::Client *m_client;
//	gloox::Adhoc *m_adhoc;
//	QHash<JAccount *, JAdHocModule *> m_modules;
//};
//}

#endif // JADHOCMODULE_H
