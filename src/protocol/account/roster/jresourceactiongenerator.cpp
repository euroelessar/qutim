#include "jresourceactiongenerator.h"
#include "jcontact.h"
#include <qutim/menucontroller.h>
#include <qutim/icon.h>
#include <QtGui/QMenu>

namespace Jabber
{
	using namespace qutim_sdk_0_3;

	struct JResourceActionGeneratorPrivate
	{
		QString feature;
	};

	JResourceActionGenerator::JResourceActionGenerator(const QIcon &icon, const LocalizedString &text,
													   const QObject *receiver, const char *member)
	: ActionGenerator(icon, text, receiver, member), d_ptr(new JResourceActionGeneratorPrivate)
	{
	}

	JResourceActionGenerator::~JResourceActionGenerator()
	{
	}

	void JResourceActionGenerator::setFeature(const QString &feature)
	{
		d_func()->feature = feature;
	}

	void JResourceActionGenerator::setFeature(const std::string &feature)
	{
		d_func()->feature = QString::fromStdString(feature);
	}

	void JResourceActionGenerator::setFeature(const QLatin1String &feature)
	{
		d_func()->feature = feature;
	}

	QObject *JResourceActionGenerator::generateHelper() const
	{
		Q_D(const JResourceActionGenerator);
		QAction *action = prepareAction(new QAction(NULL));
		if (JContact *contact = qobject_cast<JContact *>(action->data().value<MenuController *>())) {
			action->disconnect();
			QMenu *menu = new QMenu();
			QObject::connect(action, SIGNAL(destroyed()), menu, SLOT(deleteLater()));
			action->setMenu(menu);
			QStringList resourceIds = contact->resources();
			resourceIds.sort();
			QList<JContactResource *> resources;
			foreach (const QString &resId, resourceIds) {
				if (JContactResource *resource = contact->resource(resId)) {
					if (d->feature.isEmpty() || resource->checkFeature(d->feature)) {
						resources << resource;
						QAction *action = menu->addAction(Icon("user-online-jabber"), resId, receiver(), member());
						action->setData(qVariantFromValue<MenuController *>(resource));
					}
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
