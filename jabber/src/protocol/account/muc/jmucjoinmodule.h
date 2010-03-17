#ifndef JMUCJOINMODULE_H
#define JMUCJOINMODULE_H

#include <QObject>
#include "sdk/jabber.h"
#include <qutim/account.h>
#include <qutim/icon.h>

namespace Jabber
{
	using namespace qutim_sdk_0_3;

	class JAccount;

	class JMUCJoinModule : public QObject, public JabberExtension
	{
		Q_OBJECT
		Q_INTERFACES(Jabber::JabberExtension)
		public:
			JMUCJoinModule();
			virtual void init(qutim_sdk_0_3::Account *account, const JabberParams &params);
		public slots:
			void showWindow();
		private:
			JAccount *m_account;
	};
}

#endif // JMUCJOINMODULE_H
