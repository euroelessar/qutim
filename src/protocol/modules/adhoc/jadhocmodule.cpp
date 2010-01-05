#include "jadhocmodule.h"
#include "jadhocwidget.h"
#include "../../account/roster/jcontact.h"
#include "../../account/jaccount.h"
#include "../../account/roster/resourceactiongenerator.h"
#include <qutim/protocol.h>
#include <qutim/account.h>
#include <qutim/icon.h>
#include <gloox/adhoc.h>
#include <QAction>

using namespace gloox;
using namespace qutim_sdk_0_3;

namespace Jabber
{
	JAdHocModule::JAdHocModule() : m_account(0), m_client(0), m_adhoc(0)
	{
    }

	void JAdHocModule::init(qutim_sdk_0_3::Account *account, const JabberParams &params)
	{
		static JAdHocModule *self = 0;
		m_account = static_cast<JAccount *>(account);
		m_client = params.item<Client>();
		m_adhoc = params.item<Adhoc>();
		if (!self) {
			self = new JAdHocModule;
			self->setParent(account->protocol());
			JResourceActionGenerator *gen =
					new JResourceActionGenerator(Icon("utilities-terminal"),
												 QT_TRANSLATE_NOOP("Jabber", "Execute command"),
												 self,
												 SLOT(onActionClicked()));
			gen->setFeature(gloox::XMLNS_ADHOC_COMMANDS);
			MenuController::addAction<JContact>(gen);
		}
		self->m_modules.insert(m_account, this);
    }

	void JAdHocModule::onActionClicked(JContactResource *resource)
	{
		if (!m_client) {
			QAction *action = qobject_cast<QAction *>(sender());
			Q_ASSERT(action);
			qDebug() << action->data().value<MenuController *>();
			Q_ASSERT(resource = qobject_cast<JContactResource *>(action->data().value<MenuController *>()));
			m_modules.value(static_cast<JAccount *>(resource->account()))->onActionClicked(resource);
		} else {
			JAdHocWidget *adhoc = new JAdHocWidget(resource->id().toStdString(), m_adhoc);
			adhoc->show();
		}
	}
}
