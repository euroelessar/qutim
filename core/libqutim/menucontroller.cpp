/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
** Copyright © 2011 Aleksey Sidorov <gorthauer87@yandex.ru>
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

#include "menucontroller_p.h"
#include "actiongenerator_p.h"
#include <QMap>
#include <QDebug>
#include <QMetaMethod>
#include "debug.h"
#include "settingslayer.h"
#include "event.h"
#include <QCoreApplication>

namespace qutim_sdk_0_3
{
typedef QMap<QAction*, QObject*> ActionControllerMap;
Q_GLOBAL_STATIC(ActionMap, actionMap)
Q_GLOBAL_STATIC(ActionControllerMap, actionControllerMap)
Q_GLOBAL_STATIC(MenuActionMap, globalActions)
Q_GLOBAL_STATIC(QSet<QByteArray>, menuNameSet)

ActionValue::ActionValue(const ActionKey &k) : key(k)
{
	action = QWeakPointer<QAction>(key.second->generate<QAction>());
	Q_ASSERT(action);
	key.second->create(action.data(), key.first);
	actionControllerMap()->insert(action.data(), key.first);
}

ActionValue::~ActionValue()
{
	actionControllerMap()->remove(action.data());
	actionMap()->remove(key);
	delete action.data();
}

ActionValue::Ptr ActionValue::get(const ActionGenerator *gen, QObject *controller)
{
	ActionKey key(controller, gen);
	ActionMap::ConstIterator it = actionMap()->constFind(key);
	if (it != actionMap()->constEnd())
		return it.value().toStrongRef();
	ActionValue::Ptr value(new ActionValue(key));
	actionMap()->insert(key, value.toWeakRef());
	return value;
}

ActionValue::Ptr ActionValue::get(const ActionInfoV2 &info)
{
	return get(info.gen, info.controller);
}

ActionValue::WeakPtr ActionValue::find(const ActionGenerator *gen, QObject *controller)
{
	return actionMap()->value(ActionKey(controller, gen));
}

QList<ActionValue::WeakPtr> ActionValue::find(const ActionGenerator *gen)
{
	QList<ActionValue::WeakPtr> result;
	ActionMap::ConstIterator it = actionMap()->constBegin();
	ActionMap::ConstIterator endit = actionMap()->constEnd();
	for (; it != endit; ++it) {
		if (it.key().second == gen)
			result << it.value();
	}
	return result;
}

const QByteArray &menuNameBySet(const QByteArray &name)
{
	return *menuNameSet()->insert(name);
}

ActionHandlerHelper *handler()
{
	static ActionHandlerHelper handler;
	return &handler;
}

Q_GLOBAL_STATIC(QList<MenuController*>, activatedControllers)

MenuControllerPrivate::MenuControllerPrivate(MenuController *c):
	owner(0), flags(0xffff), q_ptr(c), actions(c)
{
}

MenuController::MenuController(QObject *parent) :
	QObject(parent), d_ptr(new MenuControllerPrivate(this))
{
}

MenuController::MenuController(MenuControllerPrivate &mup, QObject *parent) :
	QObject(parent), d_ptr(&mup)
{
}

MenuController::~MenuController()
{
	ActionCollectionPrivate::get(d_func()->actions)->handleDeath();
}

bool actionGeneratorLessThan(const ActionGenerator *a, const ActionGenerator *b)
{
	if (a->type() == b->type()) {
		if (a->priority() == b->priority())
			return a->text() < b->text();
		else
			return a->priority() > b->priority();
	} else {
		return a->type() < b->type();
	}
}

bool actionLessThan(const ActionInfo &a, const ActionInfo &b)
{
	int cmp = a.hash.size() - b.hash.size();
	if (cmp == 0) {
		for (int i = 0; i < a.hash.size() && !cmp; i++)
			cmp = a.hash.at(i) - b.hash.at(i); //qstrcmp(a.menu.at(i), b.menu.at(i));
		if (cmp != 0)
			return cmp < 0;
	} else {
		return cmp < 0;
	}

	// Items are inside one menu
	return actionGeneratorLessThan(a.gen, b.gen);
}

inline quint64 pack_helper(quint32 a, quint32 b)
{
	return (quint64(a) << 32) | quint64(b);
}

inline void unpack_helper(quint64 h, quint32 *a, quint32 *b)
{
	*b = h & Q_INT64_C(0xffffffff);
	*a = (h - *b) >> 32;
}

ActionEntry *DynamicMenu::findEntry(const ActionInfo &info)
{
	ActionEntry *current = &m_entry;
	for (int i = 0; i < info.hash.size(); i++) {
		QMap<uint, ActionEntry>::iterator it = current->entries.find(info.hash.at(i));
		if (it == current->entries.end()) {
			QString name = QCoreApplication::translate("Menu", info.menu.at(i));
			QMenu *menu = current->menu->addMenu(name);
			it = current->entries.insert(info.hash.at(i), ActionEntry(menu));
		}
		current = &it.value();
	}
	return current;
}

typedef QMenu * (*_menu_creator_hook)(const QString &title, QWidget *parent);
LIBQUTIM_EXPORT _menu_creator_hook menu_creator_hook = 0;

inline QMenu *create_menu(MenuController *controller)
{
	QString title = controller->property("title").toString();
	if (title.isEmpty())
		title = controller->property("id").toString();
	QMenu *menu = menu_creator_hook ? menu_creator_hook(title, 0) : new QMenu(title);
	//menu->setProperty("menuController", qVariantFromValue(controller));
	return menu;
}

DynamicMenu::DynamicMenu(MenuControllerPrivate *d)
    : m_controller(d->q_ptr), m_shown(false), m_entry(create_menu(d->q_ptr))
{
	setParent(m_entry.menu);
	connect(d->q_ptr, SIGNAL(destroyed()), m_entry.menu, SLOT(deleteLater()));
	m_entry.menu->installEventFilter(this);
	connect(m_entry.menu, SIGNAL(aboutToShow()), this, SLOT(onAboutToShow()));
	connect(m_entry.menu, SIGNAL(aboutToHide()), this, SLOT(onAboutToHide()));
	d->actions.ref();
	if (d->actions.size() == 0)
		return;
	
	// Stable actions, they use new api and are always at memory
	ActionCollectionPrivate *p = ActionCollectionPrivate::get(d->actions);
	int lastType = p->info(0).gen->type();
	QList<uint> lastMenu;
	ActionEntry *current = &m_entry;
	for (int i = 0; i < d->actions.size(); i++) {
		const ActionInfoV2 &act = p->info(i);
		if (lastMenu != act.hash) {
			lastType = act.gen->type();
			lastMenu = act.hash;
			current = findEntry(act);
		} else if (lastType != act.gen->type()) {
			lastType = act.gen->type();
			current->menu->addSeparator();
		}
		current->menu->addAction(d->actions.action(i));
		m_entries.append(current);
	}
	
	d->actions.addHandler(this);
}

DynamicMenu::~DynamicMenu()
{
	if (m_controller) {
		if (m_shown)
			d_func()->actions.showDeref();
		d_func()->actions.removeHandler(this);
		d_func()->actions.deref();
	}
}

void DynamicMenu::actionAdded(QAction *action, int index)
{
	ActionCollectionPrivate *p = ActionCollectionPrivate::get(d_func()->actions);
	const ActionInfoV2 &info = p->info(index);
	ActionEntry *entry = findEntry(info);
	Q_ASSERT(!entry->menu->actions().contains(action));
	m_entries.insert(index, entry);
	const ActionInfoV2 *prev = index > 0 ? &p->info(index - 1) : 0;
	if (prev && m_entries[index - 1] != entry)
		prev = 0;
	const ActionInfoV2 *next = (index + 1 < d_func()->actions.size()) ? &p->info(index + 1) : 0;
	if (next && m_entries[index + 1] != entry)
		next = 0;
	if (next) {
		QAction *nextAction = d_func()->actions.action(index + 1);
		if (next->gen->type() == info.gen->type()) {
			// do nothing special
		} else if (prev && prev->gen->type() == info.gen->type()) {
			QList<QAction*> actions = entry->menu->actions();
			int nextIndex = actions.indexOf(nextAction);
			nextAction = actions.at(nextIndex - 1);
			Q_ASSERT(nextAction->isSeparator());
		} else if (next->gen->type() != info.gen->type()) {
			nextAction = entry->menu->insertSeparator(nextAction);
		}
		entry->menu->insertAction(nextAction, action);
	} else {
		entry->menu->addAction(action);
		if (prev && prev->gen->type() != info.gen->type()) {
			entry->menu->insertSeparator(action);
		}
	}
}

void DynamicMenu::actionRemoved(int index)
{
	ActionEntry *entry = m_entries.takeAt(index);
	entry->menu->removeAction(d_func()->actions.action(index));
}

void DynamicMenu::actionsCleared()
{
	m_entry.menu->clear();
	m_entries.clear();
}

bool DynamicMenu::eventFilter(QObject *obj, QEvent *ev)
{
	if (obj == m_entry.menu && ev->type() == QEvent::Hide) {
		if (m_entry.menu->testAttribute(Qt::WA_DeleteOnClose))
			m_entry.menu->deleteLater();
		return false;
	}
	return QObject::eventFilter(obj, ev);
}

void DynamicMenu::onAboutToShow()
{
	if (!m_shown) {
		m_shown = true;
		d_func()->actions.showRef();
	}
}

void DynamicMenu::onAboutToHide()
{
	if (m_shown) {
		m_shown = false;
		d_func()->actions.showDeref();
	}
}

QMenu *MenuController::menu(bool deleteOnClose) const
{
	DynamicMenu *menu = new DynamicMenu(const_cast<MenuControllerPrivate*>(d_func()));
	menu->menu()->setAttribute(Qt::WA_DeleteOnClose, deleteOnClose);
	//if (!deleteOnClose)
	//	menu->onAboutToShow();

//	if (!d->menu)
//		d->menu = new DynamicMenu(const_cast<MenuControllerPrivate*>(d));
#ifdef Q_WS_MAEMO_5
	menu->menu()->setStyleSheet("QMenu { padding:0px;} QMenu::item { padding:4px; } QMenu::item:selected { background-color: #00a0f8; }");
	//maemo does not use QMenu in main menu. Only the action in menu.
	menu->onAboutToShow();
//	foreach (QAction *action, menu->menu()->actions()) {
//		ActionGenerator *gen = ActionGenerator::get(action);
//		if (!gen) {
//			if (!action->isSeparator())
//				qWarning() << "DynamicMenu::Invalid ActionGenerator:" << action->text();
//			continue;
//		}
//		QObject *controller = menu->menu->m_owners.value(gen);
//		ActionGeneratorPrivate::get(gen)->show(action,controller);
//	}
#endif
	return menu->menu();
}

void MenuController::showMenu(const QPoint &pos)
{
	menu(true)->popup(pos);
}

void MenuController::addAction(const ActionGenerator *gen, const QList<QByteArray> &menu)
{
	const ActionInfoV2 &info = d_func()->actions.addAction(gen, menu);
	foreach (MenuController *controller, *activatedControllers()) {
		if (controller == this)
			continue;
		MenuController *owner = controller;
		int flags = owner->d_func()->flags;
		while (owner != this && !!(owner = (flags & ShowOwnerActions) ? owner->d_func()->owner : 0))
			flags = owner->d_func()->flags;
		if (owner)
			controller->d_func()->actions.addAction(info);
	}
}

bool MenuController::removeAction(const ActionGenerator *gen)
{
	Q_D(MenuController);
	ActionCollectionPrivate *p = ActionCollectionPrivate::get(d->actions);
	for (int i = 0; i < p->localActions.size(); ++i) {
		if (p->localActions[i].gen == gen) {
			const ActionInfoV2 &info = p->localActions[i];
			foreach (MenuController *owner, *activatedControllers()) {
				int flags = owner->d_func()->flags;
				while (owner != this && !!(owner = (flags & ShowOwnerActions) ? owner->d_func()->owner : 0))
					flags = owner->d_func()->flags;
				if (owner)
					owner->d_func()->actions.removeAction(info);
			}
			return true;
		}
	}
	return false;
}

void MenuController::addAction(const ActionGenerator *gen, const QMetaObject *meta,
							   const QList<QByteArray> &menu)
{
	Q_ASSERT(gen && meta);
	const ActionInfo &info = *globalActions()->insert(meta, ActionInfo(gen, menu));
	foreach (MenuController *controller, *activatedControllers()) {
		MenuController *owner = controller;
		int flags = owner->d_ptr->flags;
		const QMetaObject *super = 0;
		do {
			if (!(flags & ShowSelfActions))
				continue;
			super = owner->metaObject();
			while (meta != super && !!(super = (flags & ShowSuperActions) ? super->superClass() : 0)) {}
		} while(!super && !!(owner = (flags & ShowOwnerActions) ? owner->d_func()->owner : 0));
		if (owner)
			controller->d_func()->actions.addAction(ActionInfoV2(info, owner));
	}
}

void MenuController::setMenuOwner(MenuController *controller)
{
	Q_D(MenuController);
	d->owner = controller;
	ActionCollectionPrivate::get(d->actions)->recalc();
}

void MenuController::setMenuFlags(const MenuFlags &flags)
{
	Q_D(MenuController);
	d->flags = flags;
}

MenuController::ActionList MenuController::dynamicActions() const
{
	return ActionList();
}

void MenuController::virtual_hook(int id, void *data)
{
	Q_UNUSED(id);
	Q_UNUSED(data);
}

ActionHandler::~ActionHandler()
{
}

ActionCollection::ActionCollection() :
	d_ptr(new ActionCollectionPrivate)
{
	Q_D(ActionCollection);
	d->controller = 0;
}

ActionCollection::ActionCollection(MenuController* controller) :
	d_ptr(new ActionCollectionPrivate)
{
	Q_D(ActionCollection);
	d->controller = controller;
}

ActionCollection& ActionCollection::operator=(const qutim_sdk_0_3::ActionCollection &other)
{
	d_ptr = other.d_ptr;
	return *this;
}

ActionCollection::~ActionCollection()
{
}

QAction* ActionCollection::action(int index) const
{
	Q_D(const ActionCollection);
	QAction *action = d->actions.at(index)->action.data();
	ActionGenerator *gen = ActionGenerator::get(action);
	if (gen)
		ActionGeneratorPrivate::get(gen)->show(action,d->controller);
	return action;
}

ActionCollection::ActionCollection(const qutim_sdk_0_3::ActionCollection& other) : d_ptr(other.d_ptr)
{
}

void ActionCollection::setController(MenuController *controller)
{
	d_func()->setController(controller);
}

const ActionInfoV2 &ActionCollection::addAction(const ActionGenerator *generator, const QList<QByteArray> &menu)
{
	return d_func()->addAction(generator, menu);
}

void ActionCollection::removeAction(const ActionInfoV2 &info)
{
	d_func()->removeAction(info);
}

void ActionCollection::addAction(const ActionInfoV2 &info)
{
	d_func()->addAction(info);
}

bool ActionCollection::isValid() const
{
	return d_func()->controller != NULL;
}

void ActionCollection::ref()
{
	Q_D(ActionCollection);
	if (1 == ++d->actionsRef) {
		Q_ASSERT(!activatedControllers()->contains(d->controller));
		activatedControllers()->append(d->controller);
		d->ensureActions();
	}
}

void ActionCollection::showRef()
{
	Q_D(ActionCollection);
	if (1 == ++d->showRef) {
		for (int i = 0; i < d->actions.size(); ++i) {
			const ActionInfoV2 &info = d->actionInfos.at(i);
			ActionGenerator *gen = const_cast<ActionGenerator*>(info.gen);
			ActionGeneratorPrivate::get(gen)->show(d->actions.at(i)->action.data(), info.controller);
		}
	}
}

void ActionCollection::deref()
{
	Q_D(ActionCollection);
	if (0 == --d->actionsRef) {
		Q_ASSERT(activatedControllers()->contains(d->controller));
		activatedControllers()->removeOne(d->controller);
		d->killActions();
	}
}

void ActionCollection::showDeref()
{
	Q_D(ActionCollection);
	if (0 == --d->showRef) {
		for (int i = 0; i < d->actions.size(); ++i) {
			const ActionInfoV2 &info = d->actionInfos.at(i);
			ActionGenerator *gen = const_cast<ActionGenerator*>(info.gen);
			ActionGeneratorPrivate *privateGen = gen ? ActionGeneratorPrivate::get(gen) : 0;
			if (privateGen)
				privateGen->hide(d->actions.at(i)->action.data(), info.controller);
		}
	}
}

void ActionCollection::addHandler(ActionHandler *handler)
{
	Q_D(ActionCollection);
	d->handlers.append(handler);
}

void ActionCollection::removeHandler(ActionHandler *handler)
{
	Q_D(ActionCollection);
	d->handlers.removeAll(handler);
}

int ActionCollection::count() const
{
	return d_func()->actions.count();
}

int ActionCollection::size() const
{
	return count();
}

QList< QByteArray > ActionCollection::menu(int index) const
{
	return d_func()->actionInfos.at(index).menu;
}

void ActionCollectionPrivate::setController(MenuController *newController)
{
	if (newController == controller)
		return;
	MenuController *oldController = controller;
	controller = newController;
	if (actionsRef == 0)
		return;
	QList<ActionInfoV2> tmp;
	qSwap(tmp, actionInfos);
	ensureActionInfos();
	int i = 0, j = 0;
	int l = 0;
	for (; i < tmp.size() && j < actionInfos.size(); i++) {
		const ActionInfoV2 &a = tmp.at(i);
		const ActionInfoV2 &b = actionInfos[j];
		if (a.gen == b.gen && a.hash == b.hash) {
			if (showRef > 0) {
				QAction *action = actions.at(l)->action.data();
				const ActionGeneratorPrivate *cp = ActionGeneratorPrivate::get(a.gen);
				ActionGeneratorPrivate *p = const_cast<ActionGeneratorPrivate*>(cp);
				p->hide(action, oldController);
				p->show(action, newController);
			}
			++l;
			++j;
		} else if (actionLessThan(a, b)) {
			for (int k = 0; k < handlers.size(); ++k)
				handlers.at(k)->actionRemoved(l);
			actions.removeAt(l);
		} else {
			do {
				insertAction(l, actionInfos.at(j));
				++l;
				++j;
			} while (j < actionInfos.size() && actionLessThan(actionInfos[j], a));
		}
	}
	for (; i < tmp.size(); ++i) {
		for (int k = 0; k < handlers.size(); ++k)
			handlers.at(k)->actionRemoved(l);
		actions.removeAt(l);
	}
	for (; j < actionInfos.size(); ++j, ++l) {
		insertAction(l, actionInfos.at(j));
	}
}

const ActionInfoV2 &ActionCollectionPrivate::info(int index)
{
	return actionInfos.at(index);
}

const ActionInfoV2 &ActionCollectionPrivate::addAction(const ActionGenerator *generator, const QList<QByteArray> &menu)
{
	localActions.append(ActionInfoV2(generator, menu, controller));
	const ActionInfoV2 &info = localActions.last();
	if (actionsRef > 0)
		addAction(info);
	return info;
}

void ActionCollectionPrivate::removeAction(const ActionInfoV2 &info)
{
	QList<ActionInfoV2>::ConstIterator it = qBinaryFind(actionInfos.constBegin(),
	                                                    actionInfos.constEnd(),
	                                                    info, actionLessThan);
	if (it != actionInfos.constEnd()) {
		int index = it - actionInfos.constBegin();
		for (int i = 0; i < handlers.size(); ++i)
			handlers[i]->actionRemoved(index);
		actionInfos.removeAt(index);
		actions.removeAt(index);
	}
}

void ActionCollectionPrivate::addAction(const ActionInfoV2 &info)
{
	int index = qLowerBound(actionInfos.begin(), actionInfos.end(),
	                        info, actionLessThan) - actionInfos.begin();
	insertAction(index, info);
}

void ActionCollectionPrivate::insertAction(int index, const ActionInfoV2 &info)
{
	ActionValue::Ptr action = ActionValue::get(info);
	if (showRef > 0) {
		const ActionGeneratorPrivate *cp = ActionGeneratorPrivate::get(info.gen);
		ActionGeneratorPrivate *p = const_cast<ActionGeneratorPrivate*>(cp);
		p->show(action->action.data(), controller);
	}
	actionInfos.insert(index, info);
	actions.insert(index, action);
	for (int i = 0; i < handlers.size(); ++i)
		handlers.at(i)->actionAdded(action->action.data(), index);
}

void ActionCollectionPrivate::ensureActionInfos()
{
	if (!actionInfos.isEmpty() || !controller)
		return;
	actionInfos = localActions;
	QSet<const QMetaObject *> metaObjects;
	MenuController *owner = controller;
	while (owner) {
		int flags = MenuControllerPrivate::get(owner)->flags;
		ActionCollection collection = MenuControllerPrivate::get(owner)->actions;
		ActionCollectionPrivate *p = ActionCollectionPrivate::get(collection);
		actionInfos.append(p->localActions);
		const QMetaObject *meta = owner->metaObject();
		while (meta) {
			if (metaObjects.contains(meta))
				break;
			foreach (const ActionInfo &info, globalActions()->values(meta))
				actionInfos << ActionInfoV2(info, owner);
			metaObjects.insert(meta);
			meta = (flags & MenuController::ShowSuperActions) ? meta->superClass() : 0;
		}
		owner = (flags & MenuController::ShowOwnerActions)
				? MenuControllerPrivate::get(owner)->owner : 0;
	}
	qSort(actionInfos.begin(), actionInfos.end(), actionLessThan);
}

void ActionCollectionPrivate::recalc()
{
	if (actionsRef == 0)
		return;
	killActions();
	ensureActions();
}

void ActionCollectionPrivate::ensureActions()
{
	if (!actionInfos.isEmpty())
		return;
	
	ensureActionInfos();
	QList<ActionInfoV2> infos;
	qSwap(infos, actionInfos);
	for (int i = 0; i < infos.size(); ++i)
		insertAction(i, infos[i]);
}

void ActionCollectionPrivate::killActions()
{
	if (actionsRef > 0) {
		for (int i = 0; i < handlers.size(); ++i)
			handlers[i]->actionsCleared();
	}
	actions.clear();
	dynamicActions.clear();
	actionInfos.clear();
}

void ActionCollectionPrivate::handleDeath()
{
	killActions();
	activatedControllers()->removeOne(controller);
	actionsRef = 0;
	controller = 0;
	localActions.clear();
}

class ActionContainerPrivate : public ActionHandler
{
public:
	ActionContainerPrivate() : filter(ActionContainer::Invalid), currentSize(-1), shown(false) {}
	void actionAdded(QAction *action, int index);
	void actionRemoved(int index);
	void actionsCleared();
	int size() const;
	int mappedIndex(int index) const;
	bool isNice(int index) const;
	
	ActionContainer::Filter filter;
	QVariant data;
	ActionCollection collection;
	int currentSize : 31;
	int shown : 1;
	QList<ActionHandler*> handlers;
};

void ActionContainerPrivate::actionAdded(QAction *action, int index)
{
	if (!isNice(index))
		return;
	index = mappedIndex(index);
	for (int i = 0; i < handlers.size(); ++i)
		handlers.at(i)->actionAdded(action, index);
}

void ActionContainerPrivate::actionRemoved(int index)
{
	if (!isNice(index))
		return;
	index = mappedIndex(index);
	for (int i = 0; i < handlers.size(); ++i)
		handlers.at(i)->actionRemoved(index);
}

void ActionContainerPrivate::actionsCleared()
{
	for (int i = 0; i < handlers.size(); ++i)
		handlers.at(i)->actionsCleared();
}

int ActionContainerPrivate::size() const
{
	return currentSize == -1 ? collection.size() : currentSize;
}

int ActionContainerPrivate::mappedIndex(int index) const
{
	if (filter == ActionContainer::Invalid)
		return index;
	if (!isNice(index))
		return -1;
	int nice = 0;
	for (int i = 0; i < index; ++i)
		nice += isNice(i);
	return nice;
}

bool ActionContainerPrivate::isNice(int index) const
{
	if (filter == ActionContainer::Invalid)
		return true;

	ActionCollectionPrivate *p = ActionCollectionPrivate::get(collection);
	switch (filter) {
	case ActionContainer::TypeMatch:
		return (p->info(index).gen->type() & data.toInt());
	case ActionContainer::TypeMismatch:
		return !(p->info(index).gen->type() & data.toInt());
	default:
		return true;
	}
}

ActionContainer::ActionContainer() : d_ptr(new ActionContainerPrivate)
{
}

ActionContainer::ActionContainer(ActionContainer::Filter filter, const QVariant &data)
 : d_ptr(new ActionContainerPrivate)
{
	Q_D(ActionContainer);
	d->filter = filter;
	d->data = data;
}

ActionContainer::ActionContainer(MenuController *controller)
 : d_ptr(new ActionContainerPrivate)
{
	Q_D(ActionContainer);
	d->collection = MenuControllerPrivate::get(controller)->actions;
	d->collection.ref();
	d->collection.addHandler(d);
	d->currentSize = 0;
	for (int i = 0; i < d->collection.size(); ++i)
		d->currentSize += d->isNice(i);
}

ActionContainer::ActionContainer(MenuController *controller, ActionContainer::Filter filter, const QVariant &data)
 : d_ptr(new ActionContainerPrivate)
{
	Q_D(ActionContainer);
	d->filter = filter;
	d->data = data;
	d->collection = MenuControllerPrivate::get(controller)->actions;
	d->collection.ref();
	d->collection.addHandler(d);
	d->currentSize = 0;
	for (int i = 0; i < d->collection.size(); ++i)
		d->currentSize += d->isNice(i);
		
}

ActionContainer::~ActionContainer()
{
	Q_D(ActionContainer);
	if (d->collection.isValid()) {
		d->collection.removeHandler(d);
		if (d->shown)
			d->collection.showDeref();
		d->collection.deref();
	}
}

void ActionContainer::setController(MenuController *controller)
{
	Q_D(ActionContainer);
	if (d->collection.isValid()) {
		d->collection.deref();
		if (d->shown)
			d->collection.showDeref();
	}
	ActionCollection collection;
	qSwap(collection, d->collection);
	d->actionsCleared();
	if (controller) {
		collection = MenuControllerPrivate::get(controller)->actions;
		collection.ref();
		if (d->shown)
			collection.showRef();
	}
	qSwap(collection, d->collection);
	d->currentSize = 0;
	for (int i = 0; i < d->collection.size(); ++i) {
		if (d->isNice(i)) {
			d->currentSize++;
			for (int j = 0; j < d->handlers.size(); ++j)
				d->handlers[j]->actionAdded(d->collection.action(i), d->currentSize - 1);
		}
	}
}

void ActionContainer::show()
{
	Q_D(ActionContainer);
	if (!d->shown) {
		d->shown = true;
		d->collection.showRef();
	}
}

void ActionContainer::hide()
{
	Q_D(ActionContainer);
	if (d->shown) {
		d->shown = false;
		d->collection.showDeref();
	}
}

void ActionContainer::addHandler(ActionHandler *handler)
{
	d_func()->handlers.append(handler);
}

void ActionContainer::removeHandler(ActionHandler *handler)
{
	d_func()->handlers.removeAll(handler);
}

int ActionContainer::count() const
{
	return d_func()->size();
}

int ActionContainer::size() const
{
	return d_func()->size();
}

QAction *ActionContainer::action(int index) const
{
	Q_D(const ActionContainer);
	if (index >= d->size())
		return 0;
	return d->collection.action(d->mappedIndex(index));
}

QList<QByteArray> ActionContainer::menu(int index) const
{
	Q_D(const ActionContainer);
	if (index >= d->size())
		return QList<QByteArray>();
	return d->collection.menu(d->mappedIndex(index));
}

const ActionGenerator *ActionContainer::generator(int index) const
{
	Q_D(const ActionContainer);
	if (index >= d->size())
		return 0;
	return ActionCollectionPrivate::get(d->collection)->info(index).gen;
}

ActionHandlerHelper::ActionHandlerHelper() : QObject(0)
{

}

void ActionHandlerHelper::onActionTriggered(QAction *action)
{
	const ActionGenerator *gen = ActionGenerator::get(action);
	if (!gen) {
//		qWarning("ActionHandler::onActionTriggered: Invalid ActionGenerator");
		return;
	}
	const ActionGeneratorPrivate *d = ActionGeneratorPrivate::get(gen);
	QObject *controller = actionControllerMap()->value(action);
//	QObject *controller = const_cast<QObject*>(actionsCache()->value(gen).key(action)); //fixme
	QObject *obj = d->receiver ? d->receiver.data() : controller;
	if (!obj)
		return;
	const QMetaObject *meta = obj->metaObject();
	int index = meta->indexOfMethod(d->member.constData() + 1);
	if (index == -1) {
		qWarning("ActionHandler::onActionTriggered: No such method %s::%s",
				 meta->className(), d->member.constData() + 1);
		return;
	}
	QMetaMethod method = meta->method(index);
	//qDebug("DynamicMenu::onActionTriggered: Trying %s::%s",
	//	   meta->className(), d->member.constData() + 1);
	debug() << "DynamicMenu::onActionTriggered: Trying " << meta->className() << "::" << d->member.constData() + 1;
	debug() << gen->text() << gen << action << controller;
	switch (d->connectionType) {
	case ActionConnectionObjectOnly:
		method.invoke(obj, Q_ARG(QObject*, controller));
		break;
	case ActionConnectionActionOnly:
		method.invoke(obj, Q_ARG(QAction*, action));
		break;
	case ActionConnectionFull:
		method.invoke(obj, Q_ARG(QAction*, action), Q_ARG(QObject*, controller));
		break;
	case ActionConnectionSimple:
		method.invoke(obj);
		break;
	default:
		break;
	}
}

void ActionHandlerHelper::onActionDestoyed(QObject *obj)
{
	m_actions.remove(static_cast<QAction*>(obj));
}

QAction *ActionHandlerHelper::addAction(QAction* action)
{
	connect(action,SIGNAL(destroyed(QObject*)),SLOT(onActionDestoyed(QObject*)));
	connect(action,SIGNAL(triggered()),SLOT(actionTriggered()));
	m_actions.insert(action);
	return action;
}

void ActionHandlerHelper::actionTriggered()
{
	QAction *action = qobject_cast<QAction*>(sender());
	Q_ASSERT(action);
	onActionTriggered(action);
}

QObject *MenuController::get(QAction *action)
{
	return actionControllerMap()->value(action);
}

}

