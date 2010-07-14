/****************************************************************************
 *  menucontroller.cpp
 *
 *  Copyright (c) 2010 by Nigmatullin Ruslan <euroelessar@gmail.com>
 *  Copyright (c) 2010 by Sidorov Aleksey <sauron@citadelspb.com>
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

#include "menucontroller_p.h"
#include "actiongenerator_p.h"
#include <QMap>
#include <QDebug>
#include <QMetaMethod>
#include "debug.h"

namespace qutim_sdk_0_3
{
	MenuController::MenuController(QObject *parent) : QObject(parent), d_ptr(new MenuControllerPrivate(this))
	{
	}

	MenuController::MenuController(MenuControllerPrivate &mup, QObject *parent) : QObject(parent), d_ptr(&mup)
	{
	}

	MenuController::~MenuController()
	{
	}

	bool actionLessThan(const ActionInfo &a, const ActionInfo &b)
	{
		int cmp = a.menu.size() - b.menu.size();
		if (cmp == 0) {
			for (int i = 0; i < a.menu.size() && !cmp; i++)
				cmp = a.hash.at(i) - b.hash.at(i); //qstrcmp(a.menu.at(i), b.menu.at(i));
			if (cmp != 0)
				return cmp < 0;
		} else {
			return cmp < 0;
		}

		// Items are inside one menu
		if (a.gen->type() == b.gen->type()) {
			if (a.gen->priority() == b.gen->priority())
				return a.gen->text() < b.gen->text();
			else
				return a.gen->priority() > b.gen->priority();
		} else {
			return a.gen->type() < b.gen->type();
		}
	}

	inline bool isEqualMenu(const QList<uint> &a, const QList<uint> &b)
	{
		if (a.size() != b.size())
			return false;
		for (int i = 0; i < a.size(); i++) {
			if (a[i] != b[i])
				return false;
		}
		return true;
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

	ActionEntry *DynamicMenu::findEntry(ActionEntry &entries, const ActionInfo &info)
	{
		ActionEntry *current = &entries;
		for (int i = 0; i < info.menu.size(); i++) {
			QMap<uint, ActionEntry>::iterator it = current->entries.find(info.hash.at(i));
			if (it == current->entries.end()) {
				QMenu *menu = current->menu->addMenu(QString::fromUtf8(info.menu.at(i), info.menu.at(i).size()));
				QAction *action = menu->menuAction();
				action->setData(pack_helper(info.hash.at(i), info.hash.size()));
				it = current->entries.insert(info.hash.at(i), ActionEntry(menu));
			}
			current = &it.value();
		}
		return current;
	}

	// Move method somewhere outside the DynamicMenu
	QList<ActionInfo> DynamicMenu::allActions() const
	{
		const MenuController *owner = m_d->q_func();
		QList<ActionInfo> actions;
		QSet<const QMetaObject *> metaObjects;
		while (owner) {
			foreach (const ActionInfo &info, MenuControllerPrivate::get(owner)->actions) {
				actions << info;
			}
			const QMetaObject *meta = owner->metaObject();
			while (meta) {
				if (metaObjects.contains(meta))
					break;
				foreach (const ActionInfo &info, globalActions()->values(meta)) {
					actions << info;
					m_owners.insert(info.gen, const_cast<MenuController*>(owner));
				}
				metaObjects.insert(meta);
				meta = meta->superClass();
			}
			owner = MenuControllerPrivate::get(owner)->owner;
		}
		qSort(actions.begin(), actions.end(), actionLessThan);
		return actions;
	}

	DynamicMenu::DynamicMenu(const MenuControllerPrivate *d) :
			m_d(d),m_entry(this)
	{
		connect(this, SIGNAL(aboutToShow()), this, SLOT(onAboutToShow()));
		connect(this, SIGNAL(aboutToHide()), this, SLOT(onAboutToHide()));
		QList<ActionInfo> actions = allActions();
		if (actions.isEmpty()) {
			return;
		}
		addActions(actions);
	}

	void DynamicMenu::addActions(const QList<ActionInfo> &actions)
	{
		// Stable actions, they use new api and are always at memory
		int lastType = actions[0].gen->type();
		QList<uint> lastMenu;
		ActionEntry *currentEntry = &m_entry;
		for (int i = 0; i < actions.size(); i++) {
			const ActionInfo &act = actions[i];
		
			if (!isEqualMenu(lastMenu, act.hash)) {
				lastType = act.gen->type();
				lastMenu = act.hash;
				currentEntry = findEntry(m_entry, act);
			} else if (lastType != act.gen->type()) {
				lastType = act.gen->type();
				currentEntry->menu->addSeparator();
			}

			QObject *controller = m_owners.value(act.gen);
			QAction *action = actionsCache()->value(act.gen).value(controller);
			if (!action) {
				action = act.gen->generate<QAction>();
				actionsCache()->operator[](act.gen).insert(controller,action);
			}
			if (action) {
				if (!handler()->actions().contains(action))
					handler()->addAction(action);
				currentEntry->menu->addAction(action);
			}
		}
	}

	void DynamicMenu::onActionAdded(const ActionInfo &)
	{
		//TODO epic fail  
		addActions(allActions());
	}
	
	void DynamicMenu::onMenuOwnerChanged(const MenuController *)
	{
		//epic fail v2
		//addActions(allActions(false));
	}	
	
	DynamicMenu::~DynamicMenu()
	{
// 		foreach (QAction *action, actions()) {			
// 			if (!actionsCache()->key(action))
// 				action->deleteLater();
// 		}
	}

	void DynamicMenu::onAboutToShow()	
	{
		foreach (QAction *action,this->actions()) {
			ActionGenerator *gen = action->data().value<ActionGenerator*>();
			if (!gen) {
				qWarning() << "DynamicMenu::Invalid ActionGenerator:" << action->text();
				continue;
			}
			QObject *controller = m_owners.value(gen);
			gen->showImpl(action,controller);
		}
	}

	void DynamicMenu::onAboutToHide()
	{
		foreach (QAction *action,this->actions()) {
			ActionGenerator *gen = action->data().value<ActionGenerator*>();
			if (!gen) {
				continue;
			}
			QObject *controller = m_owners.value(gen);
			gen->hideImpl(action,controller);
		}
	}
	
	QMenu *MenuController::menu(bool deleteOnClose) const
	{
		Q_UNUSED(deleteOnClose);
		if (!d_func()->menu) {
			d_func()->menu = new DynamicMenu(d_func());
		}
		return d_func()->menu;
	}

	void MenuController::showMenu(const QPoint &pos)
	{
		menu(true)->popup(pos);
	}

	void MenuController::addAction(const ActionGenerator *gen, const QList<QByteArray> &menu)
	{
		Q_D(MenuController);
		Q_ASSERT(gen);
		ActionInfo info = ActionInfo(gen, gen->d_func(), menu);
		d_func()->actions.append(info);
		if (d->menu)
			d->menu->onActionAdded(info);
	}

	bool MenuController::removeAction(const ActionGenerator *gen)
	{
		Q_D(MenuController);
		for (int i = 0; i < d->actions.size(); i++) {
			if (d->actions.at(i).gen == gen) {
				d->actions.removeAt(i);
				return true;
			}
		}
		return false;
	}

	void MenuController::addAction(const ActionGenerator *gen, const QMetaObject *meta, const QList<QByteArray> &menu)
	{
		Q_ASSERT(gen && meta);
		globalActions()->insertMulti(meta, ActionInfo(gen, gen->d_func(), menu));
	}

	void MenuController::setMenuOwner(MenuController *controller)
	{
		Q_D(MenuController);
		d->owner = controller;
		if (d->menu)
			d->menu->onMenuOwnerChanged(controller);
	}

	void MenuController::virtual_hook(int id, void *data)
	{
		Q_UNUSED(id);
		Q_UNUSED(data);
	}
		
	ActionContainer::ActionContainer(MenuController* controller) : 
		d_ptr(new ActionContainerPrivate)
	{
		Q_D(ActionContainer);
		d->controller = controller;
		d->filterType = TypeMatch;
		d->ensureActions();
	}

	ActionContainer::ActionContainer(MenuController* controller, ActionContainer::ActionFilter filter, const QVariant& data) :
		d_ptr(new ActionContainerPrivate)
	{
		Q_D(ActionContainer);
		d->controller = controller;
		d->filterType = filter;
		d->filterData = data;
		d->ensureActions();
	}
	QAction* ActionContainer::action(int index) const
	{
		return d_func()->actions.at(index);
	}

	ActionContainer::ActionContainer(const qutim_sdk_0_3::ActionContainer& other) : d_ptr(other.d_ptr)
	{

	}

	int ActionContainer::count() const
	{
		return d_func()->actions.count();
	}

	QList< QByteArray > ActionContainer::menu(int index) const
	{
		//TODO implement logic
		Q_UNUSED(index);
		QList<QByteArray> itemlist;
		return itemlist;
	}

	ActionContainer& ActionContainer::operator=(const qutim_sdk_0_3::ActionContainer &other)
	{
		d_ptr = other.d_ptr;
		return *this;
	}

	ActionContainer::~ActionContainer()
	{
		//TODO may be need a delete created actions
	}
	
	void ActionContainerPrivate::ensureActions()
	{
		actions.clear();

		QList<ActionInfo> actions;
		QSet<const QMetaObject *> metaObjects;
		const MenuController *owner = controller;
		while (owner) {
			foreach (const ActionInfo &info, MenuControllerPrivate::get(owner)->actions) {
				actions << info;
			}
			const QMetaObject *meta = owner->metaObject();
			while (meta) {
				if (metaObjects.contains(meta))
					break;
				foreach (const ActionInfo &info, globalActions()->values(meta)) {
					actions << info;
				}
				metaObjects.insert(meta);
				meta = meta->superClass();
			}
			owner = MenuControllerPrivate::get(owner)->owner;
		}

		qSort(actions.begin(), actions.end(), actionLessThan);

		foreach (ActionInfo info,actions) {
			if (filterData.canConvert(QVariant::Int)) {
				int typeMask = filterData.toInt();
				if (checkTypeMask(info,typeMask)) {
					ensureAction(info);
				}
			}
			else if (filterData.isNull()) {
				ensureAction(info);
			}
		}
	}
	
	void ActionContainerPrivate::ensureAction(const ActionInfo &info)
	{
		QAction *action = actionsCache()->value(info.gen).value(controller);
		if (!action) {
			action = info.gen->generate<QAction>();
			if (!action)
				return;
			actionsCache()->operator[](info.gen).insert(controller,action);
			handler()->addAction(action);
		}
		else if (!handler()->actions().contains(action))
			handler()->addAction(action);
		//small hack
		const_cast<ActionGenerator*>(info.gen)->showImpl(action,controller);
		actions.append(action);
	}
	
	bool ActionContainerPrivate::checkTypeMask(const ActionInfo &info,int typeMask)
	{
		switch (filterType) {
			case ActionContainer::TypeMatch: {
				if (info.gen_p->type & typeMask) {
					return true;
				}
				break;
			}
			case ActionContainer::TypeMismatch: {
				if (!(info.gen_p->type & typeMask))
					return true;
				break;
			}
			default:
				break;
		}
		return false;
	}
	
	ActionHandler::ActionHandler() : QActionGroup(0)
	{
		connect(this,SIGNAL(triggered(QAction*)),SLOT(onActionTriggered(QAction*)));
	}
	
	void ActionHandler::onActionTriggered(QAction *action)
	{
		const ActionGenerator *gen = action->data().value<ActionGenerator*>();
		if (!gen) {
			qWarning("DynamicMenu::onActionTriggered: Invalid ActionGenerator");
			return;
		}
		const ActionGeneratorPrivate *d = ActionGeneratorPrivate::get(gen);
		QObject *controller = const_cast<QObject*>(actionsCache()->value(gen).key(action)); //fixme
		QObject *obj = d->receiver ? d->receiver.data() : controller;
		const QMetaObject *meta = obj->metaObject();
		int index = meta->indexOfMethod(d->member.constData() + 1);
		if (index == -1) {
			qWarning("DynamicMenu::onActionTriggered: No such method %s::%s",
					 meta->className(), d->member.constData() + 1);
			return;
		}
		QMetaMethod method = meta->method(index);
		qDebug("DynamicMenu::onActionTriggered: Trying %s::%s",
			   meta->className(), d->member.constData() + 1);
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
	
	void ActionHandler::onActionDestoyed(QObject *obj)
	{
		QAction *action = reinterpret_cast<QAction*>(obj);
		ActionGeneratorMap::iterator it;
		for (it = actionsCache()->begin();it!=actionsCache()->end();it++) {
			if (const QObject *key = it->key(action))
				it->remove(key);
		}
	}
	
	QAction *ActionHandler::addAction(QAction *action)
	{
		connect(action,SIGNAL(destroyed(QObject*)),SLOT(onActionDestoyed(QObject*)));
		return QActionGroup::addAction(action);
	}

}
