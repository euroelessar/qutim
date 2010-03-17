#include "jmucjoinmodule.h"
#include "../jaccount.h"
#include "jmucmanager.h"

namespace Jabber
{
	JMUCJoinModule::JMUCJoinModule()
	{
		m_account = 0;
	}

	void JMUCJoinModule::init(Account *account, const JabberParams &)
	{
		m_account = qobject_cast<JAccount *>(account);
		account->addAction(new ActionGenerator(Icon(""),
				QT_TRANSLATE_NOOP("Jabber", "Join groupchat"),
				this, SLOT(showWindow())), "Additional");
	}

	void JMUCJoinModule::showWindow()
	{
		m_account->conferenceManager()->openJoinWindow();
	}
}
