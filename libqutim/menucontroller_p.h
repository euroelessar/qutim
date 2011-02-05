/****************************************************************************
 *  menucontroller_p.h
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

#ifndef MENUCONTROLLER_P_H
#define MENUCONTROLLER_P_H

#include "menucontroller.h"

namespace qutim_sdk_0_3
{

typedef QMap<const QMetaObject *, ActionInfo> MenuActionMap;
typedef QMap<const ActionGenerator*,QMap<QObject*, QAction*> > ActionGeneratorMap;

ActionGeneratorMap *actionsCache();

struct ActionInfo
{
	ActionInfo(const ActionGenerator *g, const ActionGeneratorPrivate *g_p,
			   const QList<QByteArray> &m) : gen(g), gen_p(g_p), menu(m)
	{
		for (int i = 0; i < menu.size(); i++)
			hash << qHash(menu.at(i));
	}
	const ActionGenerator *gen;
	const ActionGeneratorPrivate *gen_p;
	QList<QByteArray> menu;
	QList<uint> hash;
};

struct ActionEntry
{
	inline ActionEntry(QMenu *m) : menu(m) {}
	inline ActionEntry(QAction *action) : menu(action->menu()) {}

	QPointer<QMenu> menu;
	QMap<uint, ActionEntry> entries;
};


class DynamicMenu;
class MenuControllerPrivate
{
public:
	Q_DECLARE_PUBLIC(MenuController)
	typedef MenuController::MenuFlag MenuFlag;
	MenuControllerPrivate(MenuController *c)
		: owner(0), flags(0xffff), q_ptr(c) {}
	QList<ActionInfo> actions;
	MenuController *owner;
	int flags;
	MenuController *q_ptr;
	mutable QPointer<DynamicMenu> menu;
	static MenuControllerPrivate *get(MenuController *gen) { return gen->d_func(); }
	static const MenuControllerPrivate *get(const MenuController *gen) { return gen->d_func(); }
};

class DynamicMenu : public QObject
{
	Q_OBJECT
public:
	DynamicMenu(const MenuControllerPrivate *d);
	virtual ~DynamicMenu();

	inline QMenu *menu() { return m_menu; }
	inline MenuController *controller() { return m_d->q_ptr; }
	void addActions(const QList<ActionInfo> &actions); //TODO need redesign
	void addAction(MenuController *owner, const ActionInfo &info);
	void removeAction(MenuController *owner, const ActionGenerator *gen);
	ActionEntry *findEntry(ActionEntry &entries, const ActionInfo &info);
	QAction *ensureAction(const ActionGenerator *gen);
public slots:
	void onActionAdded(const ActionInfo &info);
	void onMenuOwnerChanged(const MenuController *owner);
private slots:
	void onAboutToShow();
	void onAboutToHide();
private:
	QList<ActionInfo> allActions() const;
	const MenuControllerPrivate * const m_d;
	QMenu *m_menu;
	ActionEntry m_entry;
	mutable QMap<const ActionGenerator*, QObject*> m_owners;
	friend class MenuController;
};

class ActionContainerPrivate : public QSharedData
{
	Q_DISABLE_COPY(ActionContainerPrivate)
public:
	inline ActionContainerPrivate() : controller(0),filterType(ActionContainer::TypeMatch),filterData(QVariant()) {}
	inline ~ActionContainerPrivate() {}
	MenuController *controller;
	ActionContainer::ActionFilter filterType;
	QVariant filterData;
	QList<QAction*> actions;
	void ensureActions();
	inline bool checkTypeMask(const ActionInfo& info, int typeMask);
	inline void ensureAction(const ActionInfo& info);
};
//TODO create common handler for DynamicMenu, ActionContainer and ActionToolbar

typedef QList<QAction*> QActionList;

class ActionHandler : public QObject
{
	Q_OBJECT
public:
	ActionHandler();
	QAction *addAction(QAction *action);
	void onActionTriggered(QAction *action);
	QActionList actions() const {return m_actions;}
private slots:
	void onActionDestoyed(QObject *obj);
	void actionTriggered();
private:
	QActionList m_actions;
};

ActionHandler *handler();
}

#endif // MENUCONTROLLER_P_H
