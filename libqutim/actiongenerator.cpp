/****************************************************************************
 *  actiongenerator.cpp
 *
 *  Copyright (c) 2009 by Nigmatullin Ruslan <euroelessar@gmail.com>
 *
 ***************************************************************************
 *                                                                         *
 *   This library is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
*****************************************************************************/

#include "actiongenerator.h"
#include "objectgenerator_p.h"
#include <QtGui/QIcon>
#include <QtGui/QAction>

namespace qutim_sdk_0_3
{
	struct ActionGeneratorPrivate : public ObjectGeneratorPrivate
	{
		QIcon icon;
		LocalizedString text;
		QPointer<QObject> receiver;
		const char *member;
		int type;
		int priority;
	};

	ActionGenerator::ActionGenerator(const QIcon &icon, const LocalizedString &text, const QObject *receiver, const char *member)
			: ObjectGenerator(*new ActionGeneratorPrivate)
	{
		Q_D(ActionGenerator);
		Q_ASSERT(receiver && member && *member);
		d->icon = icon;
		d->text = text;
		d->receiver = const_cast<QObject *>(receiver);
		d->member = member;
		d->type = 0;
		d->priority = 0;
	}

	ActionGenerator::~ActionGenerator()
	{
	}

	QIcon ActionGenerator::icon() const
	{
		return d_func()->icon;
	}

	const LocalizedString &ActionGenerator::text() const
	{
		return d_func()->text;
	}

	ActionGenerator *ActionGenerator::addProperty(const QByteArray &name, const QVariant &value)
	{
		return static_cast<ActionGenerator *>(ObjectGenerator::addProperty(name, value));
	}

	int ActionGenerator::type() const
	{
		return d_func()->type;
	}

	ActionGenerator *ActionGenerator::setType(int type)
	{
		d_func()->type = type;
		return this;
	}

	int ActionGenerator::priority() const
	{
		return d_func()->priority;
	}

	ActionGenerator *ActionGenerator::setPriority(int priority)
	{
		d_func()->priority = priority;
		return this;
	}

	QAction *ActionGenerator::prepareAction(QAction *action) const
	{
		Q_D(const ActionGenerator);
		if (d->receiver.isNull()) {
			action->deleteLater();
			return NULL;
		}
		action->setParent(d->receiver);
		action->setIcon(d->icon);
		action->setText(d->text);
		QObject::connect(action, SIGNAL(triggered()), d->receiver, d->member);
		return action;
	}

	QObject *ActionGenerator::generateHelper() const
	{
		return prepareAction(new QAction(NULL));
	}

	const QMetaObject *ActionGenerator::metaObject() const
	{
		return &QAction::staticMetaObject;
	}

	bool ActionGenerator::hasInterface(const char *id) const
	{
		Q_UNUSED(id);
		return false;
	}
}
