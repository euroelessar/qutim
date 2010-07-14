#include "messaging.h"
#include <qutim/actiongenerator.h>
#include <qutim/icon.h>
#include <qutim/menucontroller.h>
#include "messagingdialog.h"

namespace MassMessaging
{

	void MassMessaging::init()
	{
	}

	bool MassMessaging::load()
	{
		addAuthor(QT_TRANSLATE_NOOP("Author","Aleksey Sidorov"),
				  QT_TRANSLATE_NOOP("Task","Author"),
				  QLatin1String("sauron@citadelspb.com"),
				  QLatin1String("sauron.me"));
		setInfo(QT_TRANSLATE_NOOP("Plugin", "MassMessaging"),
				QT_TRANSLATE_NOOP("Plugin", "Simple messaging to contact list"),
				PLUGIN_VERSION(0, 1, 0, 0));
		ActionGenerator *gen = new ActionGenerator(Icon("mail-send"),
										QT_TRANSLATE_NOOP("MassMessaging", "&Mass Messaging"),
										this,
										SLOT(onActionTriggered())
										);
		qobject_cast<MenuController*>(getService("ContactList"))->addAction(gen);

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
