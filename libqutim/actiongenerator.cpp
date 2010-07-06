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
#include <QMetaMethod>
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

	void ActionGeneratorPrivate::ensureConnectionType()
	{
		if (QMetaObject::checkConnectArgs(member, SLOT(test(QAction*,QObject*))))
			connectionType = ActionConnectionFull;
		else if (QMetaObject::checkConnectArgs(member, SLOT(test(QObject*))))
			connectionType = ActionConnectionObjectOnly;
		else if (QMetaObject::checkConnectArgs(member, SLOT(test(QAction*))))
			connectionType = ActionConnectionActionOnly;
		else
			connectionType = ActionConnectionSimple;
	}

	ActionGenerator::ActionGenerator(const QIcon &icon, const LocalizedString &text,
									 const QObject *receiver, const char *member)
			  : ObjectGenerator(*new ActionGeneratorPrivate)
	{
		Q_D(ActionGenerator);
		d->icon = icon;
		d->text = text;
		d->receiver = const_cast<QObject *>(receiver);
		d->member = QMetaObject::normalizedSignature(member);
		char type = d->member[0];
		d->member[0] = '0' + QSIGNAL_CODE;
		d->ensureConnectionType();
		d->member[0] = type;
		if (d->connectionType == ActionConnectionLegacy) {
			d->legacyData = new LegacyActionData;
		} else {
			d->data = new ActionData;
		}
	}
	
	ActionGenerator::ActionGenerator(const QIcon &icon, const LocalizedString &text, const char *member)
		: ObjectGenerator(*new ActionGeneratorPrivate)
	{
		Q_D(ActionGenerator);
		d->icon = icon;
		d->text = text;
		d->member = QMetaObject::normalizedSignature(member);
		char type = d->member[0];
		d->member[0] = '0' + QSIGNAL_CODE;
		d->ensureConnectionType();
		d->member[0] = type;
		d->data = new ActionData;
	}

	ActionGenerator::ActionGenerator(ActionGeneratorPrivate &priv) : ObjectGenerator(priv)
	{
	}

	ActionGenerator::~ActionGenerator()
	{
		Q_D(ActionGenerator);
		if (d->connectionType == ActionConnectionLegacy)
			delete d->legacyData;
		else
			delete d->data;
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
		Q_D(ActionGenerator);
		if (d->connectionType == ActionConnectionLegacy)
			d->legacyData->controller = controller;
	}

	MenuController *ActionGenerator::controller() const
	{
		Q_D(const ActionGenerator);
		if (d->connectionType == ActionConnectionLegacy)
			return d->legacyData->controller;
		return 0;
	}

	void ActionGenerator::addCreationHandler(QObject *obj)
	{
		Q_ASSERT(obj);
		Q_D(ActionGenerator);
		if (d->connectionType == ActionConnectionLegacy)
			d->legacyData->handlers.append(obj);
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
		if (d->connectionType == ActionConnectionLegacy) {
			action->setCheckable(d->legacyData->checkable);
			action->setChecked(d->legacyData->checked);
		}
		action->setCheckable(d->data->checkable);
		action->setChecked(d->data->checked);
		action->setToolTip(d->toolTip);
		localizationHelper()->addAction(action, d);
/*		if (d->connectionType == ActionConnectionLegacy) {
			QObject *receiver = d->receiver;
			if (d->legacyData->controller) {
				action->setData(QVariant::fromValue(const_cast<MenuController *>(d->legacyData->controller)));
				if (!receiver)
					receiver = d->legacyData->controller;
			}
			if (!d->member.isEmpty() && receiver)
				QObject::connect(action, SIGNAL(triggered()), receiver, d->member);
		}*/
		if (d->connectionType == ActionConnectionSimple) {
			if (!d->member.isEmpty() && d->receiver)
				QObject::connect(action, SIGNAL(triggered()), d->receiver, d->member);
		} else {
			action->setData(QVariant::fromValue(const_cast<ActionGenerator *>(this)));
		}
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
		d->connectionType = ActionConnectionSimple;
		d->data = new ActionData;
		d->data->menu = menu;
	}

	MenuActionGenerator::MenuActionGenerator(const QIcon &icon, const LocalizedString &text, MenuController *controller) :
			ActionGenerator(*new ActionGeneratorPrivate)
	{
		Q_D(ActionGenerator);
		d->icon = icon;
		d->text = text;
		d->connectionType = ActionConnectionSimple;
		d->data = new ActionData;
		d->data->controller = controller;
	}

	MenuActionGenerator::~MenuActionGenerator()
	{
	}

	QObject *MenuActionGenerator::generateHelper() const
	{
		Q_D(const ActionGenerator);
		QAction *action = prepareAction(new QAction(NULL));
		if (d->data->menu) {
			action->setMenu(d->data->menu);
		} else if (d->data->controller) {
			QMenu *menu = d->data->controller->menu(false);
			QObject::connect(action, SIGNAL(destroyed()), menu, SLOT(deleteLater()));
			action->setMenu(menu);
		}
		return action;
	}
	
	void ActionGenerator::setCheckable(bool checkable)
	{
		d_func()->data->checkable = checkable;
	}

	void ActionGenerator::setChecked(bool checked)
	{
		d_func()->data->checked = checked;
	}

	void ActionGenerator::setToolTip(const LocalizedString& toolTip)
	{
		d_func()->toolTip = toolTip;
	}
	
	void ActionGenerator::showImpl(QAction*,QObject*)
	{

	}

	void ActionGenerator::hideImpl(QAction*, QObject*)
	{

	}

}
