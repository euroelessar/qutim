#include "messaging.h"
#include <qutim/actiongenerator.h>
#include <qutim/icon.h>
#include <qutim/contactlist.h>
#include "messagingdialog.h"

namespace MassMessaging
{

	void MassMessaging::init()
	{
	}
	bool MassMessaging::load()
	{
		ActionGenerator *gen = new ActionGenerator(Icon("mail-send"),
										QT_TRANSLATE_NOOP("MassMessaging", "&Mass Messaging"),
										this,
										SLOT(onActionTriggered())
										);
		MenuController::addAction<ContactList>(gen);
		return true;
	}
	bool MassMessaging::unload()
	{
		return false;
	}

	void MassMessaging::onActionTriggered()
	{
		if (!m_dialog) {
			m_dialog = new MessagingDialog();
			centerizeWidget(m_dialog);
		}
		m_dialog->show();

	}
}

QUTIM_EXPORT_PLUGIN(MassMessaging::MassMessaging)
