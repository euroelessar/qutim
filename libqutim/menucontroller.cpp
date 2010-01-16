/****************************************************************************
 *  menucontroller.cpp
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

#include "menucontroller_p.h"
#include <QMap>
#include <QDebug>

namespace qutim_sdk_0_3
{

	typedef QMap<const QMetaObject *, ActionInfo> MenuActionMap;

	Q_GLOBAL_STATIC(MenuActionMap, globalActions)

	MenuController::MenuController(QObject *parent) : QObject(parent), d_ptr(new MenuControllerPrivate)
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
		} else
			return cmp < 0;

		// Items are inside one menu
		if (a.gen->type() == b.gen->type()) {
			if (a.gen->priority() == b.gen->priority())
				return a.gen->text() < b.gen->text();
			else
				return a.gen->priority() > b.gen->priority();
		} else
			return a.gen->type() < b.gen->type();
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

	struct ActionEntry
	{
		inline ActionEntry(QMenu *m) : menu(m) {}
		inline ActionEntry(QAction *action) : menu(action->menu()) {}

		QMenu *menu;
		QMap<uint, ActionEntry> entries;
	};

	ActionEntry *findEntry(ActionEntry &entries, const ActionInfo &info)
	{
		ActionEntry *current = &entries;
		for (int i = 0; i < info.menu.size(); i++) {
			QMap<uint, ActionEntry>::iterator it = current->entries.find(info.hash.at(i));
			if (it == current->entries.end()) {
				QMenu *menu = current->menu->addMenu(QString::fromUtf8(info.menu.at(i), info.menu.at(i).size()));
				it = current->entries.insert(info.hash.at(i), ActionEntry(menu));
			}
			current = &it.value();
		}
		return current;
	}

	QList<ActionInfo> MenuControllerPrivate::allActions(const MenuController *owner) const
	{
		QList<ActionInfo> actions;
		QSet<const QMetaObject *> metaObjects;
		while (owner) {
			actions.append(owner->d_ptr->actions);
			const QMetaObject *meta = owner->metaObject();
			while (meta) {
				if (metaObjects.contains(meta))
					break;
				actions.append(globalActions()->values(meta));
				metaObjects.insert(meta);
				meta = meta->superClass();
			}
			owner = owner->d_ptr->owner;
		}
		return actions;
	}

	QMenu *MenuController::menu(bool deleteOnClose) const
	{
		QMenu *menu = new QMenu();
		menu->setAttribute(Qt::WA_DeleteOnClose, deleteOnClose);
		QList<ActionInfo> actions = d_func()->allActions(this);
		if (actions.isEmpty())
			return menu;
		qSort(actions.begin(), actions.end(), actionLessThan);
		int lastType = actions[0].gen->type();
		QList<uint> lastMenu;
		ActionEntry entry(menu);
		ActionEntry *currentEntry = &entry;
		for (int i = 0; i < actions.size(); i++) {
			const ActionInfo &act = actions[i];
			if (!isEqualMenu(lastMenu, act.hash)) {
				lastType = act.gen->type();
				lastMenu = act.hash;
				currentEntry = findEntry(entry, act);
			} else if (lastType != act.gen->type()) {
				lastType = act.gen->type();
				menu->addSeparator();
			}
			const_cast<ActionGenerator *>(act.gen)->setMenuController(const_cast<MenuController *>(this));
			if (QAction *action = act.gen->generate<QAction>()) {
				action->setParent(currentEntry->menu);
				currentEntry->menu->addAction(action);
			}
		}
		return menu;
	}

	void MenuController::showMenu(const QPoint &pos)
	{
		menu(true)->popup(pos);
	}

	void MenuController::addAction(const ActionGenerator *gen, const QList<QByteArray> &menu)
	{
		Q_ASSERT(gen);
		d_func()->actions.append(ActionInfo(gen, menu));
	}

	void MenuController::addAction(const ActionGenerator *gen, const QMetaObject *meta, const QList<QByteArray> &menu)
	{
		Q_ASSERT(gen && meta);
		globalActions()->insertMulti(meta, ActionInfo(gen, menu));
	}

	void MenuController::setMenuOwner(MenuController *controller)
	{
		Q_D(MenuController);
		d->owner = controller;
	}
}
