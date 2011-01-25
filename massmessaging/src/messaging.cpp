#include "messaging.h"
#include <qutim/actiongenerator.h>
#include <qutim/icon.h>
#include <qutim/debug.h>
#include <qutim/menucontroller.h>
#include  <qutim/servicemanager.h>
#include "messagingdialog.h"
#include <QApplication>

namespace MassMessaging
{

	void MassMessaging::init()
	{
		debug() << Q_FUNC_INFO;
		addAuthor(QT_TRANSLATE_NOOP("Author","Aleksey Sidorov"),
				  QT_TRANSLATE_NOOP("Task","Author"),
				  QLatin1String("sauron@citadelspb.com"),
				  QLatin1String("sauron.me"));
		setInfo(QT_TRANSLATE_NOOP("Plugin", "MassMessaging"),
				QT_TRANSLATE_NOOP("Plugin", "Simple messaging to contact list"),
				PLUGIN_VERSION(0, 1, 0, 0));
	}

	bool MassMessaging::load()
	{
		ActionGenerator *gen = new ActionGenerator(Icon("mail-send"),
										QT_TRANSLATE_NOOP("MassMessaging", "&Mass Messaging"),
										this,
										SLOT(onActionTriggered())
										);
		gen->setType(ActionTypeAdditional);
		MenuController *contactList = ServiceManager::getByName<MenuController*>("ContactList");
		if (contactList)
			contactList->addAction(gen);

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
#ifdef Q_WS_MAEMO_5
		m_dialog->setParent(QApplication::activeWindow());
		m_dialog->setWindowFlags(m_dialog->windowFlags() | Qt::Window);
		m_dialog->setAttribute(Qt::WA_Maemo5StackedWindow);
#endif
		m_dialog->show();

	}
}

QUTIM_EXPORT_PLUGIN(MassMessaging::MassMessaging)
