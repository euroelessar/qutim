/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
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
#include "jresourceactiongenerator.h"
#include "jcontactresource.h"
#include "jcontact.h"
#include <qutim/menucontroller.h>
#include <qutim/icon.h>
#include <QMenu>
#include <QDebug>

namespace Jabber
{
	using namespace qutim_sdk_0_3;

	class JResourceActionGeneratorPrivate
	{
	public:
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

	inline bool resource_less_than(JContactResource *r1, JContactResource *r2)
	{ return r1->name() < r2->name(); }

	QObject *JResourceActionGenerator::generateHelper() const
	{
		Q_D(const JResourceActionGenerator);
		QAction *action = prepareAction(new QAction(NULL));
		if (JContact *contact = qobject_cast<JContact *>(action->data().value<MenuController *>())) {
			action->disconnect();
			QMenu *menu = new QMenu();
			QObject::connect(action, SIGNAL(destroyed()), menu, SLOT(deleteLater()));
			action->setMenu(menu);
			QList<JContactResource *> resources = contact->resources();
			qSort(resources.begin(), resources.end(), resource_less_than);
			bool isEmpty = true;
			foreach (JContactResource *resource, resources) {
				if (d->feature.isEmpty() || resource->checkFeature(d->feature)) {
					isEmpty = false;
					QAction *action = menu->addAction(Icon("user-online-jabber"), resource->name(), receiver(), member());
					action->setData(qVariantFromValue<MenuController *>(resource));
				}
			}
			if (isEmpty)
				action->setDisabled(true);
		} else {
			action->setVisible(false);
		}
		return action;
	}
}

