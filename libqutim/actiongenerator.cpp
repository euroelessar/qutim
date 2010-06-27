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
#include <QtCore/QCoreApplication>

namespace qutim_sdk_0_3
{
	Q_GLOBAL_STATIC(ActionGeneratorLocalizationHelper, localizationHelper)
	
	ActionGeneratorLocalizationHelper::ActionGeneratorLocalizationHelper()
	{
		qApp->installEventFilter(this);
	}

	bool ActionGeneratorLocalizationHelper::eventFilter(QObject *, QEvent *ev)
	{
		if (ev->type() == QEvent::LanguageChange) {
			QMap<QAction*, const ActionGeneratorPrivate*>::iterator it = m_actions.begin();
			QMap<QAction*, const ActionGeneratorPrivate*>::iterator endit = m_actions.end();
			for (; it != endit; it++) {
				QAction *action = it.key();
				const ActionGeneratorPrivate *data = it.value();
				action->setText(data->text);
				action->setToolTip(data->toolTip);
			}
		}
		return false;
	}

	void ActionGeneratorLocalizationHelper::addAction(QAction *action,
													  const ActionGeneratorPrivate *data)
	{
		m_actions.insert(action, data);
		connect(action, SIGNAL(destroyed(QObject*)), this, SLOT(onActionDeath(QObject*)));
	}

	void ActionGeneratorLocalizationHelper::onActionDeath(QObject *obj)
	{
		m_actions.remove(static_cast<QAction*>(obj));
	}
	
	ActionCreatedEvent::ActionCreatedEvent(QAction *action, ActionGenerator *gen) :
			QEvent(eventType()), m_action(action), m_gen(gen)
	{
	}

	QEvent::Type ActionCreatedEvent::eventType()
	{
		static QEvent::Type type = QEvent::Type(QEvent::registerEventType(QEvent::User + 102));
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

	MenuController *ActionGenerator::controller() const
	{
		return d_func()->controller;
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
		action->setCheckable(d->checkable);
		action->setChecked(d->checked);
		action->setToolTip(d->toolTip);
		localizationHelper()->addAction(action, d);
		QObject *receiver = d->receiver;
		if (d->controller) {
			action->setData(QVariant::fromValue(const_cast<MenuController *>(d->controller)));
			if (!receiver)
				receiver = d->controller;
		}
		if (!d->member.isEmpty() && receiver)
			QObject::connect(action, SIGNAL(triggered()), receiver, d->member);
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
	
	void ActionGenerator::setCheckable(bool checkable)
	{
		d_func()->checkable = checkable;
	}

	void ActionGenerator::setChecked(bool checked)
	{
		d_func()->checked = checked;
	}

	void ActionGenerator::setToolTip(const LocalizedString& toolTip)
	{
		d_func()->toolTip = toolTip;
	}

}
