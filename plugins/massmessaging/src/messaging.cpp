/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Aleksey Sidorov <gorthauer87@yandex.ru>
**
*****************************************************************************
**
** $QUTIM_BEGIN_LICENSE$
** This program is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
** See the GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program.  If not, see http://www.gnu.org/licenses/.
** $QUTIM_END_LICENSE$
**
****************************************************************************/

#include "messaging.h"
#include <qutim/actiongenerator.h>
#include <qutim/icon.h>
#include <qutim/debug.h>
#include <qutim/menucontroller.h>
#include <qutim/systemintegration.h>
#include  <qutim/servicemanager.h>
#include "messagingdialog.h"
#include <QApplication>

namespace MassMessaging
{

void MassMessaging::init()
{
	debug() << Q_FUNC_INFO;
	addAuthor(QLatin1String("sauron"));
	setInfo(QT_TRANSLATE_NOOP("Plugin", "MassMessaging"),
			QT_TRANSLATE_NOOP("Plugin", "Simple messaging to contact list"),
			PLUGIN_VERSION(0, 1, 0, 0));
	setCapabilities(Loadable);
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
		centerizeWidget(m_dialog.data());
	}
#ifdef Q_WS_MAEMO_5
	m_dialog.data()->setParent(QApplication::activeWindow());
	m_dialog.data()->setWindowFlags(m_dialog.data()->windowFlags() | Qt::Window);
	m_dialog.data()->setAttribute(Qt::WA_Maemo5StackedWindow);
#endif
	SystemIntegration::show(m_dialog.data());
}
}

QUTIM_EXPORT_PLUGIN(MassMessaging::MassMessaging)

