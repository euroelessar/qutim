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
#include "menucontroller_p.h"
#include <QMetaMethod>
#include <QtGui/QIcon>
#include <QtGui/QAction>
#include <QtCore/QCoreApplication>
#include "debug.h"
#include <QLatin1Literal>


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

void ActionGeneratorLocalizationHelper::addAction(QAction *action, const ActionGeneratorPrivate *data)
{
	m_actions.insert(action, data);
	connect(action, SIGNAL(destroyed(QObject*)), this, SLOT(onActionDeath(QObject*)));
}

void ActionGeneratorLocalizationHelper::addAction(QObject *obj, QAction *action)
{
	connect(obj, SIGNAL(destroyed()), action, SLOT(deleteLater()));
	if (const ActionGeneratorPrivate *data = m_actions.value(action))
		(*actionsCache())[data->q_ptr].insert(obj, action);
}

void ActionGeneratorLocalizationHelper::onActionDeath(QObject *obj)
{
	QAction *action = static_cast<QAction*>(obj);
	if (const ActionGeneratorPrivate *data = m_actions.take(action)) {
		QMap<QObject*, QAction*> &map = (*actionsCache())[data->q_ptr];
		map.remove(map.key(action));
	}
}

ActionCreatedEvent::ActionCreatedEvent(QAction *action, ActionGenerator *gen, QObject *con) :
	QEvent(eventType()),
	m_action(action),
	m_gen(gen),
	m_con(con)
{
}

//	MenuController *ActionCreatedEvent::controller() const
//	{
//		if (generator()->type() | ActionConnectionObjectOnly) {
//			QObject *obj = const_cast<QObject*>(actionsCache()->value(m_gen).key(m_action));
//			return qobject_cast<MenuController*>(obj);
//		}
//		return 0;
//	}

QEvent::Type ActionCreatedEvent::eventType()
{
	static QEvent::Type type = QEvent::Type(QEvent::registerEventType(QEvent::User + 102));
	return type;
}

ActionVisibilityChangedEvent::ActionVisibilityChangedEvent (QAction* action, QObject* controller, bool isVisible) :
	QEvent(eventType()),m_action(action),m_controller(controller),m_visible(isVisible)
{

}

QEvent::Type ActionVisibilityChangedEvent::eventType()
{
	static QEvent::Type type = QEvent::Type(QEvent::registerEventType(QEvent::User + 103));
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

ActionGeneratorPrivate::ActionGeneratorPrivate():
	q_ptr(0),type(0), priority(0),
	connectionType(ActionConnectionSimple)
{
	iconVisibleInMenu = qApp->testAttribute(Qt::AA_DontShowIconsInMenus);
}

ActionGenerator::ActionGenerator(const QIcon &icon, const LocalizedString &text,
								 const QObject *receiver, const char *member)
	: ObjectGenerator(*new ActionGeneratorPrivate)
{
	Q_D(ActionGenerator);
	d->q_ptr = this;
	d->icon = icon;
	d->text = text;
	d->receiver = const_cast<QObject *>(receiver);
	d->member = QMetaObject::normalizedSignature(member);
	char type = d->member[0];
	d->member[0] = '0' + QSIGNAL_CODE;
	if (!member)
		d->connectionType = ActionConnectionNone;
	else
		d->ensureConnectionType();
	d->member[0] = type;
	d->data = new ActionData;
}

ActionGenerator::ActionGenerator(const QIcon &icon, const LocalizedString &text, const char *member)
	: ObjectGenerator(*new ActionGeneratorPrivate)
{
	Q_D(ActionGenerator);
	d->q_ptr = this;
	d->icon = icon;
	d->text = text;
	d->member = QMetaObject::normalizedSignature(member);
	char type = d->member[0];
	d->member[0] = '0' + QSIGNAL_CODE;
	if (!member)
		d->connectionType = ActionConnectionNone;
	else
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
	if (actionsCache())
		actionsCache()->remove(this);
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
	Q_UNUSED(controller);
}

void ActionGenerator::addHandler(int type, QObject* obj)
{
	Q_D(ActionGenerator);
	d->subcribers.operator[](type).append(obj);
}

void ActionGenerator::removeHandler(int type, QObject* obj)
{
	Q_D(ActionGenerator);
	d->subcribers.operator[](type).removeAll(obj);
}

void ActionGeneratorPrivate::sendActionCreatedEvent(QAction *action, QObject *controller) const
{
	foreach (QObject *subcriber,subcribers.value(ActionCreatedHandler)) {
		ActionCreatedEvent event(action, q_ptr , controller);
		qApp->sendEvent(subcriber,&event);
	}
	QPair<QObject*, QByteArray> receiver;
	foreach (receiver, receivers) {
		const QMetaObject *meta = receiver.first->metaObject();
		const char *member = receiver.second.constData() + 1;
		int index = meta->indexOfMethod(member);
		if (index == -1) {
			qWarning("ActionGeneratorPrivate::onActionCreated: No such method %s::%s",
					 meta->className(), member);
		}
		QMetaMethod method = meta->method(index);
		method.invoke(receiver.first, Q_ARG(QAction*, action), Q_ARG(QObject*, controller));
	}
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
	if (!d->icon.isNull())
		action->setIcon(d->icon);
	if (!d->text.original().isEmpty())
		action->setText(d->text);
	action->setCheckable(d->data->checkable);
	action->setChecked(d->data->checked);
	action->setToolTip(d->toolTip);
	action->setShortcuts(d->shortCuts);
	action->setMenuRole(d->menuRole);
	//action->setIconVisibleInMenu(d->iconVisibleInMenu);
	localizationHelper()->addAction(action, d);
	//action->setData(QVariant::fromValue(const_cast<ActionGenerator *>(this)));

	if (!handler()->actions().contains(action))
		handler()->addAction(action);

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

QList<QByteArray> ActionGenerator::interfaces() const
{
	return QList<QByteArray>();
}

MenuActionGenerator::MenuActionGenerator(const QIcon &icon, const LocalizedString &text, QMenu *menu) :
	ActionGenerator(*new ActionGeneratorPrivate)
{
	Q_D(ActionGenerator);
	d->q_ptr = this;
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
	d->q_ptr = this;
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
	//		const ActionGenerator *gen = this;
	//		action->setData(QVariant::fromValue(const_cast<ActionGenerator *>(gen)));
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

void ActionGenerator::setShortcut(const QKeySequence &shortcut)
{
	Q_D(ActionGenerator);
	d->shortCuts.clear();
	d->shortCuts.append(shortcut);
}

void ActionGenerator::create(QAction *action, QObject *obj) const
{
	localizationHelper()->addAction(obj, action);
	createImpl(action, obj);
}

void ActionGenerator::createImpl(QAction *action, QObject *controller) const
{
	d_func()->sendActionCreatedEvent(action, controller);
}

void ActionGenerator::showImpl(QAction *,QObject *)
{
}

void ActionGenerator::hideImpl(QAction *,QObject *)
{
}

void ActionGeneratorPrivate::show(QAction *act,QObject *con)
{
	foreach (QObject *subcriber,subcribers.value(ActionVisibilityChangedHandler)) {
		ActionVisibilityChangedEvent ev(act,con);
		qApp->sendEvent(subcriber,&ev);
	}
	q_ptr->showImpl(act,con);
}

void ActionGeneratorPrivate::hide(QAction *act,QObject *con)
{
	foreach (QObject *subcriber,subcribers.value(ActionVisibilityChangedHandler)) {
		ActionVisibilityChangedEvent ev(act,con,false);
		qApp->sendEvent(subcriber,&ev);
	}
	q_ptr->hideImpl(act,con);
}

QList<QAction *> ActionGenerator::actions(QObject *object) const
{
	return actionsCache()->value(this).values(object);
}

QMap<QObject*, QAction*> ActionGenerator::actions() const
{
	return actionsCache()->value(this);
}

void ActionGenerator::subscribe(QObject *object, const char *method)
{
	if (!method || !object)
		return;
	QByteArray member = QMetaObject::normalizedSignature(method);
	QPair<QObject*, QByteArray> pair(object, member);
	d_func()->receivers.append(pair);
}

ActionGenerator *ActionGenerator::get(QAction *action)
{
	return localizationHelper()->getGenerator(action);
}

ActionGenerator * ActionGeneratorLocalizationHelper::getGenerator(QAction *action) const
{
	const ActionGeneratorPrivate *p = m_actions.value(action);
	return p ? p->q_ptr : 0;
}

void ActionGenerator::setMenuRole(QAction::MenuRole role)
{
	d_func()->menuRole = role;
}

QAction::MenuRole ActionGenerator::menuRole() const
{
	return d_func()->menuRole;
}


void ActionGenerator::setIconVisibleInMenu(bool visible)
{
	d_func()->iconVisibleInMenu = visible;
}

bool ActionGenerator::iconVisibleInMenu() const
{
	return d_func()->iconVisibleInMenu;
}



}

