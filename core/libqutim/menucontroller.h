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

#ifndef MENUUNIT_H
#define MENUUNIT_H

#include "actiongenerator.h"
#include <QMenu>
#include "debug.h"

namespace qutim_sdk_0_3
{

class SettingsItem;
typedef QList<SettingsItem *> SettingsItemList;

class MenuControllerPrivate;
struct ActionInfo;

/*!
  MenuController is base type for objects which are able
  to have menu.

  Example of how to register and use actions:
  \code
class MyObject : public QObject
{
Q_OBJECT
public:
MyObject(QObject *parent = 0);
void registerController(MenuController *);
public slots:
void onAction();
};

MyObject::MyObject(QObject *parent) : QObject(parent)
{
}

void MyObject::registerController(MenuController *controller)
{
ActionGenerator *gen = new ActionGenerator(..., this, SLOT(onAction());
controller->addAction(gen);
}

MyObject::onAction()
{
MenuController *controller = MenuController::getController(sender());
doSmth();
}
  \endcode
*/
class LIBQUTIM_EXPORT MenuController : public QObject
{
	Q_OBJECT
	Q_DISABLE_COPY(MenuController)
	Q_DECLARE_PRIVATE(MenuController)
public:
	enum MenuFlag
	{
		ShowSelfActions     = 0x01,
		ShowSuperActions    = 0x02,
		ShowOwnerActions    = 0x04
	};
	Q_DECLARE_FLAGS(MenuFlags, MenuFlag)

#ifndef Q_QDOC
	struct Action
	{
		ActionGenerator *generator;
		QList<QByteArray> menu;
	};
	typedef QList<Action> ActionList;
#endif
	/*!
	  Constructs MenuController with \a parent.
	*/
	MenuController(QObject *parent = 0);
#ifndef Q_QDOC
	MenuController(MenuControllerPrivate &p, QObject *parent = 0);
#endif
	/*!
	  Destructor
	*/
	virtual ~MenuController();
	/*!
	  Generate menu for this object and return pointer to it.
	  Menu will be deleted after closing if \a deleteOnClose is \b true.
	*/
	QMenu *menu(bool deleteOnClose = true) const;
	/*!
	  Add action \a gen to this object.
	  If \a menu is not empty action will be situated not in the root of
	  menu, but in the submenu hierarchy; \a menu contains untranslated
	  names of submenus in the tree.
	  \a deleteOnClose \deprecated
	*/
	void addAction(const ActionGenerator *gen, const QList<QByteArray> &menu = QList<QByteArray>());
	/*!
	  Add action \a gen to this object.
	  If \a menu is not empty action will be situated not in the root of
	  menu, but in the submenu hierarchy; \a menu contains list of
	  null-terminated strings, each of them is untranslated name of
	  submenu.
	*/
	template <int N>
	void addAction(const ActionGenerator *gen, const char (&menu)[N]);
	bool removeAction(const ActionGenerator *gen);
	/*!
	  Add action \a gen to every object with QMetaObject \a meta.
	  If \a menu is not empty action will be situated not in the root of
	  menu, but in the submenu hierarchy; \a menu contains untranslated
	  names of submenus in the tree.
	*/
	static void addAction(const ActionGenerator *gen, const QMetaObject *meta,
						  const QList<QByteArray> &menu = QList<QByteArray>());
	/*!
	  Add action \a gen to every object of type \a T.
	  If \a menu is not empty action will be situated not in the root of
	  menu, but in the submenu hierarchy; \a menu contains untranslated
	  names of submenus in the tree.
	*/
	template <typename T>
	static void addAction(const ActionGenerator *gen,
						  const QList<QByteArray> &menu = QList<QByteArray>());
	/*!
	  Add action \a gen to every object of type \a T.
	  If \a menu is not empty action will be situated not in the root of
	  menu, but in the submenu hierarchy; \a menu contains list of
	  null-terminated strings, each of them is untranslated name of
	  submenu.
	*/
	template <typename T, int N>
	static void addAction(const ActionGenerator *gen, const char (&menu)[N]);
	/*!
	  Add action \a gen to every object with QMetaObject \a meta.
	  If \a menu is not empty action will be situated not in the root of
	  menu, but in the submenu hierarchy; \a menu contains list of
	  null-terminated strings, each of them is untranslated name of
	  submenu.
	*/
	template <int N>
	static void addAction(const ActionGenerator *gen, const QMetaObject *meta, const char (&menu)[N]);
	/*!
	  Returns MenuController for action which has emitted signal
	  connected to this slot. \a obj must be result of sender().

	  Example of use inside slot method:
	  \code
void MyObject::onAction()
{
Account *account = MenuController::get<Account>(sender());
doStuff();
}
	  \endcode
	*/
	static QObject *get(QAction *);
	template <typename T>
	static T get(QAction *);
public slots:
	/*!
	  Show menu at position \a pos and delete it after closing.
	  It's equivalent for \code menu(true)->popup(pos) \endcode
	*/
	void showMenu(const QPoint &pos);
protected:
	/*!
	  Add to menu of this object also actions from another \a controller.
	*/
	void setMenuOwner(MenuController *controller);
	void setMenuFlags(const MenuFlags &flags);
#ifndef Q_QDOC
	virtual ActionList dynamicActions() const;
#endif
	virtual void virtual_hook(int id, void *data);
	QScopedPointer<MenuControllerPrivate> d_ptr;
};

// TODO: Implement class
// Warning: All legacy QAction's created for this container MUST be destroyed at it's death
// Notice: You should use QActionGroup as at DynamicMenu for actions triggering
// Think: May be wh should use this container inside DynamicMenu?
// Think: May be we should use filters also for menus?
class ActionContainerPrivate;

class LIBQUTIM_EXPORT ActionHandler
{
public:
	virtual ~ActionHandler();
	
	virtual void handleActionAdded(int index, QAction *action) = 0;
	virtual void handleActionRemoved(int index) = 0;
	virtual void handleActionsCleared() = 0;
};

class LIBQUTIM_EXPORT ActionContainer
{
	Q_DISABLE_COPY(ActionContainer)
	Q_DECLARE_PRIVATE(ActionContainer)
public:
	enum Filter { Invalid = -1, TypeMatch, TypeMismatch };
	// Constructor
	// Get all actions
	ActionContainer();
	ActionContainer(Filter filter, const QVariant &data);
	ActionContainer(MenuController *controller);
	// Get only actions, which satisfy filter, i.e. filter=TypeMatch, data=1
	// for getting all actions with type == 1
	ActionContainer(MenuController *controller, Filter filter, const QVariant &data);
	// Destructor, I think it shouldn't be virtual
	~ActionContainer();
	
	void setController(MenuController *controller);
	void show();
	void hide();
	
	void addHandler(ActionHandler *handler);
	void removeHandler(ActionHandler *handler);

	// Access to actions, they should be sorted by qutim_sdk_0_3::actionLessThan
	int count() const;
	int size() const;
	// Can be accessed only after first ref's call
	QAction *action(int index) const;
	QList<QByteArray> menu(int index) const;
	const ActionGenerator *generator(int index) const;

private:
	QScopedPointer<ActionContainerPrivate> d_ptr;
};

template <int N>
Q_INLINE_TEMPLATE void MenuController::addAction(const ActionGenerator *gen, const char (&menu)[N])
{
	addAction(gen, QByteArray::fromRawData(menu, N - 1).split('\0'));
}

template <typename T>
Q_INLINE_TEMPLATE void MenuController::addAction(const ActionGenerator *gen, const QList<QByteArray> &menu)
{
	addAction(gen, &T::staticMetaObject, menu);
}

template <typename T, int N>
Q_INLINE_TEMPLATE void MenuController::addAction(const ActionGenerator *gen, const char (&menu)[N])
{
	addAction(gen, &T::staticMetaObject, QByteArray::fromRawData(menu, N - 1).split('\0'));
}

template <int N>
Q_INLINE_TEMPLATE void MenuController::addAction(const ActionGenerator *gen,
												 const QMetaObject *meta,
												 const char (&menu)[N])
{
	addAction(gen, meta, QByteArray::fromRawData(menu, N - 1).split('\0'));
}

}

Q_DECLARE_METATYPE(qutim_sdk_0_3::MenuController*)
Q_DECLARE_OPERATORS_FOR_FLAGS(qutim_sdk_0_3::MenuController::MenuFlags)

namespace qutim_sdk_0_3
{

template<typename T>
Q_INLINE_TEMPLATE T MenuController::get(QAction *action)
{
	return qobject_cast<T>(MenuController::get(action));
}

}

#endif // MENUUNIT_H

