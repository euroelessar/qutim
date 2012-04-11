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

#ifndef MENUCONTROLLER_P_H
#define MENUCONTROLLER_P_H

#include "menucontroller.h"

namespace qutim_sdk_0_3
{
class ActionCollectionPrivate;

typedef QMultiMap<const QMetaObject *, ActionInfo> MenuActionMap;
typedef QMap<const ActionGenerator*, QMap<QObject*, QAction*> > ActionGeneratorMap;

const QByteArray &menuNameBySet(const QByteArray &name);

struct ActionInfo
{
	ActionInfo(const ActionGenerator *g, const QList<QByteArray> &m) : gen(g), menu(m)
	{
		for (int i = 0; i < menu.size(); i++)
			hash << qHash(menu[i] = menuNameBySet(menu.at(i)));
	}
	const ActionGenerator *gen;
	QList<QByteArray> menu;
	QList<uint> hash;
};

struct ActionInfoV2 : public ActionInfo
{
	ActionInfoV2(const ActionGenerator *g, const QList<QByteArray> &m, MenuController *c)
	    : ActionInfo(g, m), controller(c) {}
	ActionInfoV2(const ActionInfo &o, MenuController *c) : ActionInfo(o), controller(c) {}
	MenuController *controller;
};

typedef QPair<QObject*, const ActionGenerator*> ActionKey;
class ActionValue : public QSharedData
{
	Q_DISABLE_COPY(ActionValue)
public:
	typedef QSharedPointer<ActionValue> Ptr;
	typedef QWeakPointer<ActionValue> WeakPtr;
	
	ActionValue(const ActionKey &key);
	~ActionValue();
	
	static ActionValue::Ptr get(const ActionGenerator *gen, QObject *controller);
	static ActionValue::Ptr get(const ActionInfoV2 &info);
	static ActionValue::WeakPtr find(const ActionGenerator *gen, QObject *controller);
	static QList<ActionValue::WeakPtr> find(const ActionGenerator *gen);
	
	ActionKey key;
	QWeakPointer<QAction> action;
};
typedef QMap<ActionKey, ActionValue::WeakPtr> ActionMap;

struct ActionEntry;
typedef QMap<uint, ActionEntry> ActionEntryMap;

struct ActionEntry
{
	inline ActionEntry(QMenu *m) : menu(m) {}
	inline ActionEntry(QAction *action) : menu(action->menu()) {}

	QMenu *menu;
	ActionEntryMap entries;
};

class ActionCollection
{
	Q_DECLARE_PRIVATE(ActionCollection)
public:
	// Constructor
	// Get all actions
	ActionCollection();
	ActionCollection(MenuController *controller);
	// Destructor, I think it shouldn't be virtual
	~ActionCollection();
	// Copy constructor and method
	ActionCollection(const ActionCollection &);
	ActionCollection &operator =(const ActionCollection &);
	
	void setController(MenuController *controller);
	const ActionInfoV2 &addAction(const ActionGenerator *generator, const QList<QByteArray> &menu);
	void addAction(const ActionInfoV2 &info);
	void removeAction(const ActionInfoV2 &info);
	bool isValid() const;
	// This should be call manually just before QAction will be really needed
	void ref();
	// This should be call manually just before QAction's actual data will be really needed
	void showRef();
	void deref();
	void showDeref();
	
	void addHandler(ActionHandler *handler);
	void removeHandler(ActionHandler *handler);

	// Access to actions, they should be sorted by qutim_sdk_0_3::actionLessThan
	int count() const;
	int size() const;
	// Can be accessed only after first ref's call
	QAction *action(int index) const;
	QList<QByteArray> menu(int index) const;

private:
	QExplicitlySharedDataPointer<ActionCollectionPrivate> d_ptr;
};

class DynamicMenu;
class MenuControllerPrivate
{
public:
	Q_DECLARE_PUBLIC(MenuController)
	typedef MenuController::MenuFlag MenuFlag;
	MenuControllerPrivate(MenuController *c);
	MenuController *owner;
	int flags;
	MenuController *q_ptr;
	ActionCollection actions;
	MenuController::ActionList dynamicActions() const { return q_func()->dynamicActions(); }
	static MenuControllerPrivate *get(MenuController *gen) { return gen->d_func(); }
	static const MenuControllerPrivate *get(const MenuController *gen) { return gen->d_func(); }
};

class DynamicMenu : public QObject, public ActionHandler
{
	Q_OBJECT
public:
	DynamicMenu(MenuControllerPrivate *p);
	virtual ~DynamicMenu();
	virtual void actionAdded(QAction *action, int index);
	virtual void actionRemoved(int index);
	virtual void actionsCleared();
	virtual bool eventFilter(QObject *, QEvent *);
	
	QMenu *menu() { return m_entry.menu; }
	ActionEntry *findEntry(const ActionInfo &info);
//	void addAction(const ActionInfoV2 &info);
//	void removeAction(MenuController *owner, const ActionGenerator *gen);

private slots:
	void onAboutToShow();
	void onAboutToHide();

private:
	QWeakPointer<MenuController> m_controller;
	MenuControllerPrivate * d_func() const
	{ return m_controller ? MenuControllerPrivate::get(m_controller.data()) : 0; }
	bool m_shown;
	ActionEntry m_entry;
	QList<ActionEntry*> m_entries;
//	QList<ActionGenerator *> dynamicActionsList;
//	mutable QMap<const ActionGenerator*, QObject*> m_owners;
	friend class MenuController;
};

class ActionCollectionPrivate : public QSharedData
{
	Q_DISABLE_COPY(ActionCollectionPrivate)
public:
	inline ActionCollectionPrivate()  : controller(0), actionsRef(0), showRef(0) {}
	~ActionCollectionPrivate();
	MenuController *controller;
	QList<ActionInfoV2> actionInfos;
	QList<ActionInfoV2> dynamicActions;
	QList<ActionInfoV2> localActions;
	QList<ActionValue::Ptr> actions;
	QList<ActionHandler*> handlers;
	qint16 actionsRef;
	qint16 showRef;
	
	static ActionCollectionPrivate *get(const ActionCollection &collection)
	{ return const_cast<ActionCollectionPrivate*>(collection.d_func()); }
	void setController(MenuController *controller);
	const ActionInfoV2 &info(int index);
	const ActionInfoV2 &addAction(const ActionGenerator *generator, const QList<QByteArray> &menu);
	void addAction(const ActionInfoV2 &info);
	void removeAction(const ActionInfoV2 &info);
	void insertAction(int index, const ActionInfoV2 &info);
	void ensureActionInfos();
	void recalc();
	void ensureActions();
	void killActions();
	void handleDeath();
//	inline bool checkTypeMask(const ActionInfo& info, int typeMask);
//	inline void ensureAction(const ActionInfo& info);
};
//TODO create common handler for DynamicMenu, ActionContainer and ActionToolbar

class ActionHandlerHelper : public QObject
{
	Q_OBJECT
public:
	ActionHandlerHelper();
	QAction *addAction(QAction *action);
	void onActionTriggered(QAction *action);
	const QSet<QAction*> &actions() const { return m_actions; }
private slots:
	void onActionDestoyed(QObject *obj);
	void actionTriggered();
private:
	QSet<QAction*> m_actions;
};

ActionHandlerHelper *handler();
}

#endif // MENUCONTROLLER_P_H

