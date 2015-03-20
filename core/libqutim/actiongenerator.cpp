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

#include "actiongenerator_p.h"
#include "menucontroller.h"
#include "menucontroller_p.h"
#include <QMetaMethod>
#include <QIcon>
#include <QAction>
#include <QtCore/QCoreApplication>
#include "debug.h"
#include <QLatin1Literal>


namespace qutim_sdk_0_3
{
Q_GLOBAL_STATIC(ActionGeneratorHelper, localizationHelper)

void action_sequence_update_handler(const QString &id, const QKeySequence &key)
{
	localizationHelper()->updateSequence(id, key);
}

ActionGeneratorHelper::ActionGeneratorHelper()
{
	qApp->installEventFilter(this);
	ShortcutSelf::addUpdateHandler(action_sequence_update_handler);
}

ActionGeneratorHelper::~ActionGeneratorHelper()
{
	ShortcutSelf::removeUpdateHandler(action_sequence_update_handler);
}

bool ActionGeneratorHelper::eventFilter(QObject *, QEvent *ev)
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

void ActionGeneratorHelper::addAction(QAction *action, const ActionGeneratorPrivate *data)
{
	m_actions.insert(action, data);
	if (!data->shortCut.isEmpty()) {
		KeySequence sequence = Shortcut::getSequence(data->shortCut);
		action->setShortcut(sequence.key);
		action->setShortcutContext(sequence.context);
		m_shortcuts.insert(data->shortCut, action);
	}
	connect(action, SIGNAL(destroyed(QObject*)), this, SLOT(onActionDeath(QObject*)));
}

void ActionGeneratorHelper::updateSequence(const QString &id, const QKeySequence &key)
{
	QMultiHash<QString, QAction *>::const_iterator it = m_shortcuts.constFind(id);
	if (it == m_shortcuts.constEnd())
		return;
	do {
		it.value()->setShortcut(key);
		++it;
	} while (it != m_shortcuts.constEnd() && it.key() == id);
}

void ActionGeneratorHelper::handleDeath(ActionGeneratorPrivate *data)
{
	ActionValue::handleDeath(data->q_ptr);
	QList<QAction*> actions = m_actions.keys(data);
	for (int i = 0; i < actions.size(); ++i)
		actions.at(i)->deleteLater();
}

void ActionGeneratorHelper::onActionDeath(QObject *obj)
{
	QAction *action = static_cast<QAction*>(obj);
	m_actions.remove(action);
	QMultiHash<QString, QAction *>::iterator it = m_shortcuts.begin();
	for (; it != m_shortcuts.end(); ++it) {
		if (it.value() == action) {
			m_shortcuts.erase(it);
			break;
		}
	}
}

ActionCleanupHandler::ActionCleanupHandler(QObject *parent)
{
	setParent(parent);
}

ActionCleanupHandler *ActionCleanupHandler::get(QObject *object)
{
    const QObjectList &children = object->children();
	for (int i = 0; i < children.size(); ++i) {
		if (children[i]->metaObject() == &ActionCleanupHandler::staticMetaObject)
			return static_cast<ActionCleanupHandler*>(children[i]);
	}
	return new ActionCleanupHandler(object);
}

ActionCreatedEvent::ActionCreatedEvent(QAction *action, ActionGenerator *gen, QObject *con)
    : QEvent(eventType()), m_action(action), m_gen(gen), m_con(con)
{
}

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
	connectionType(ActionConnectionSimple),
	menuRole(QAction::NoRole)
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
    localizationHelper()->handleDeath(d);
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
    return d_func()->receiver.data();
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
    d_func()->data->controller = controller;
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
			qWarning() << "ActionGeneratorPrivate::onActionCreated: No such method" << meta->className() << "::" << member;
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
        action->setParent(d->receiver.data());
	if (!d->icon.isNull())
		action->setIcon(d->icon);
	if (!d->text.original().isEmpty())
		action->setText(d->text);
	action->setCheckable(d->data->checkable);
	action->setChecked(d->data->checked);
	action->setToolTip(d->toolTip);
	if (d->shortCut.isEmpty())
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
	if (d->data->controller) {
		QMenu *menu = d->data->controller->menu(false);
		action->setMenu(menu);
        action->setProperty("conrtoller", QVariant::fromValue(d->data->controller));
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
	d->shortCut.clear();
	d->shortCuts.clear();
	d->shortCuts.append(shortcut);
}

QString ActionGenerator::shortcut() const
{
	return d_func()->shortCut;
}

void ActionGenerator::setShortcut(const QString &id)
{
	Q_D(ActionGenerator);
	d->shortCuts.clear();
	d->shortCut = id;
}

void ActionGenerator::create(QAction *action, QObject *obj) const
{
//	localizationHelper()->addAction(obj, action);
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
	foreach (QObject *subcriber, subcribers.value(ActionVisibilityChangedHandler)) {
		ActionVisibilityChangedEvent ev(act,con,false);
		qApp->sendEvent(subcriber,&ev);
	}
	q_ptr->hideImpl(act,con);
}

QList<QAction *> ActionGenerator::actions(QObject *object) const
{
	QList<QAction*> list;
	if (ActionValue::WeakPtr value = ActionValue::find(this, object))
		list.append(value.data()->action.data());
	return list;
}

QMap<QObject*, QAction*> ActionGenerator::actions() const
{
	QMap<QObject*, QAction*> map;
	foreach (const ActionValue::WeakPtr &value, ActionValue::find(this))
		map.insert(value.data()->key.first, value.data()->action.data());
	return map;
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

ActionGenerator * ActionGeneratorHelper::getGenerator(QAction *action) const
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

