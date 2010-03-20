/****************************************************************************
 *  actiongenerator.cpp
 *
 *  Copyright (c) 2010 by Nigmatullin Ruslan <euroelessar@gmail.com>
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

#include "actiongenerator_p.h"
#include "menucontroller.h"
#include <QtGui/QIcon>
#include <QtGui/QAction>

namespace qutim_sdk_0_3
{
	ActionCreatedEvent::ActionCreatedEvent(QAction *action, ActionGenerator *gen) :
			QEvent(eventType()), m_action(action), m_gen(gen)
	{
	}

	QEvent::Type ActionCreatedEvent::eventType()
	{
		static QEvent::Type type = QEvent::Type(QEvent::registerEventType(QEvent::User + 103));
		return type;
	}

	ActionGenerator::ActionGenerator(const QIcon &icon, const LocalizedString &text,
									 const QObject *receiver, const char *member)
			  : ObjectGenerator(*new ActionGeneratorPrivate)
	{
		Q_D(ActionGenerator);
//		Q_ASSERT(receiver && member && *member);
		d->icon = icon;
		d->text = text;
		d->receiver = const_cast<QObject *>(receiver);
		d->member = member;
	}

	ActionGenerator::ActionGenerator(ActionGeneratorPrivate &priv) : ObjectGenerator(priv)
	{
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

	const QObject *ActionGenerator::receiver() const
	{
		return d_func()->receiver;
	}

	const char *ActionGenerator::member() const
	{
		return d_func()->member.constData();
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

	void ActionGenerator::setMenuController(MenuController *controller)
	{
		d_func()->controller = controller;
	}

	void ActionGenerator::addCreationHandler(QObject *obj)
	{
		Q_ASSERT(obj);
		d_func()->handlers.append(obj);
	}

	QAction *ActionGenerator::prepareAction(QAction *action) const
	{
		Q_D(const ActionGenerator);
//		if (d->receiver.isNull()) {
//			action->deleteLater();
//			return NULL;
//		}
		if (d->receiver)
			action->setParent(d->receiver);
		action->setIcon(d->icon);
		action->setText(d->text);
		if (d->controller)
			action->setData(QVariant::fromValue(const_cast<MenuController *>(d->controller)));
		if (d->receiver)
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

	MenuActionGenerator::MenuActionGenerator(const QIcon &icon, const LocalizedString &text, QMenu *menu) :
			ActionGenerator(*new ActionGeneratorPrivate)
	{
		Q_D(ActionGenerator);
		d->icon = icon;
		d->text = text;
		d->menu = menu;
	}

	MenuActionGenerator::MenuActionGenerator(const QIcon &icon, const LocalizedString &text, MenuController *controller) :
			ActionGenerator(*new ActionGeneratorPrivate)
	{
		Q_D(ActionGenerator);
		d->icon = icon;
		d->text = text;
		d->controller = controller;
	}

	MenuActionGenerator::~MenuActionGenerator()
	{
	}

	QObject *MenuActionGenerator::generateHelper() const
	{
		Q_D(const ActionGenerator);
		QAction *action = prepareAction(new QAction(NULL));
		if (d->menu) {
			action->setMenu(d->menu);
		} else if (d->controller) {
			QMenu *menu = d->controller->menu(false);
			QObject::connect(action, SIGNAL(destroyed()), menu, SLOT(deleteLater()));
			action->setMenu(menu);
		}
		return action;
	}
}
