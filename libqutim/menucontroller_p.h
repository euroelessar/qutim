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
	typedef QMap<const ActionGenerator*,QMap<const QObject*, QAction*> > ActionGeneratorMap;

	Q_GLOBAL_STATIC(MenuActionMap, globalActions)
	Q_GLOBAL_STATIC(ActionGeneratorMap,actionsCache);

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
		MenuControllerPrivate(MenuController *c) : owner(0), q_ptr(c) {}
		QList<ActionInfo> actions;
		MenuController *owner;
		MenuController *q_ptr;
		mutable QPointer<DynamicMenu> menu;
		static MenuControllerPrivate *get(MenuController *gen) { return gen->d_func(); }
		static const MenuControllerPrivate *get(const MenuController *gen) { return gen->d_func(); }
	};

	class DynamicMenu : public QMenu
	{
		Q_OBJECT
	public:
		DynamicMenu(const MenuControllerPrivate *d);
		virtual ~DynamicMenu();
		void addActions(const QList<ActionInfo> &actions); //TODO need redesign
		ActionEntry *findEntry(ActionEntry &entries, const ActionInfo &info);
	public slots:
		void onActionAdded(const ActionInfo &info);
		void onMenuOwnerChanged(const MenuController *owner);
	private slots:
		void onAboutToShow();
		void onAboutToHide();
	private:
		QList<ActionInfo> allActions() const;
		const MenuControllerPrivate * const m_d;
		ActionEntry m_entry;
		mutable QMap<const ActionGenerator*, QObject*> m_owners;
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

	Q_GLOBAL_STATIC(ActionHandler,handler);

	//hack for stupid gcc!

	static inline void gccHack() {
		Q_UNUSED(globalActions())
		Q_UNUSED(actionsCache())
		Q_UNUSED(handler())
	}

}

#endif // MENUCONTROLLER_P_H
