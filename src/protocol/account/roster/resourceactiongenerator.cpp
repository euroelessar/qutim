#include "resourceactiongenerator.h"
#include "jcontact.h"
#include <qutim/menucontroller.h>
#include <qutim/icon.h>
#include <QtGui/QMenu>

namespace Jabber
{
using namespace qutim_sdk_0_3;

ResourceActionGenerator::ResourceActionGenerator(const QIcon &icon, const LocalizedString &text,
												 const QObject *receiver, const char *member)
		: ActionGenerator(icon, text, receiver, member)
{
}

ResourceActionGenerator::~ResourceActionGenerator()
{
}

QObject *ResourceActionGenerator::generateHelper() const
{
	QAction *action = prepareAction(new QAction(NULL));
	if (JContact *contact = qobject_cast<JContact *>(action->data().value<MenuController *>())) {
		action->disconnect();
		QMenu *menu = new QMenu();
		QObject::connect(action, SIGNAL(destroyed()), menu, SLOT(deleteLater()));
		action->setMenu(menu);
		QStringList resources = contact->resources();
		foreach (const QString &resId, resources) {
			if (JContactResource *resource = contact->resource(resId)) {
				menu->addAction(Icon("user-status-online"), resId, receiver(), member());
			}
		}
		if (resources.isEmpty())
			action->setDisabled(true);
	} else {
		delete action;
		action = 0;
	}
	return action;
}

}
