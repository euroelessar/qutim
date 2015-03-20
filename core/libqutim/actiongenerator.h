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

#ifndef ACTIONGENERATOR_H
#define ACTIONGENERATOR_H

#include "objectgenerator.h"
#include "localizedstring.h"
#include <QIcon>
#include <QEvent>
#include <QAction>

namespace qutim_sdk_0_3
{
class ActionGenerator;
class ActionGeneratorPrivate;
class MenuController;
class ActionToolBar;
class ActionCollectionPrivate;
class ActionValue;

//TBD list of all action roles
// Keep in sink with QuickActionContainer::ActionType
enum ActionType
{
	ActionTypeContactList = 0x01, //Actions in contactlist
	ActionTypeChatButton = 0x02, //Chat buttons
	ActionTypeAdditional = 0x04, //Additional modifier
	ActionTypeAccount = 0x8,
	ActionTypePreferences = 0x10, //QAction::PreferencesRole
    ActionTypeContactInfo = 0x20
};

Q_DECLARE_FLAGS(ActionsType,ActionType)

enum ActionHandlerType
{
	ActionCreatedHandler,
	ActionVisibilityChangedHandler
};

Q_DECLARE_FLAGS(ActionHandlerTypes,ActionHandlerType)

class LIBQUTIM_EXPORT ActionCreatedEvent : public QEvent
{
public:
	ActionCreatedEvent(QAction *action, ActionGenerator *gen, QObject *controller);
	static QEvent::Type eventType();
	QAction *action() const { return m_action; }
	ActionGenerator *generator() const { return m_gen; }
	QObject *controller() const {return m_con;}
private:
	QAction *m_action;
	ActionGenerator *m_gen;
	QObject *m_con;
};

class LIBQUTIM_EXPORT ActionVisibilityChangedEvent : public QEvent
{
public:
	ActionVisibilityChangedEvent(QAction *action,QObject *controller, bool isVisible = true);
	static QEvent::Type eventType();
	QAction *action() const { return m_action; }
	QObject *controller() const { return m_controller; }
	bool isVisible() const { return m_visible; }
private:
	QAction *m_action;
	QObject *m_controller;
	bool m_visible;

};

// TODO: Resolve problem with action groups, checkable actions and so one
// Possible solutions:
// * add flag for creating 'unique' actions with different instances for different objects
// * add new quint64 property, named 'groupId', which should be unique for every action
//   and equal for action groups (use algorithm equal to message ids), if core founds
//   actions with similiar group ids it addes them to QActionGroup
//
// Also It should be usefull to create some class-listener for unique actions, for such
// events as ActionCreated, ActionDestroyed and with ability for changing them during
// their lifetime (making disabled/checked and so on)
// It should be usefull to create method for getting QAction for MenuController passed
// as argument. BTW, unique actions should be created only once for each object, use i.e.
// QPointer at cache and QSharedPointer at ActionContainer
//
// Try to remove any use of legacy ActionGenerator's members everywhere at qutIM
class LIBQUTIM_EXPORT ActionGenerator : public ObjectGenerator
{
	Q_DECLARE_PRIVATE(ActionGenerator)
	Q_DISABLE_COPY(ActionGenerator)
	Q_GADGET
public:
	enum Type { StatusType = 0, GeneralType };
	/*
   * \code
void MyObject::onAction(QObject *obj)
{
 Account *account = qobject_cast<Account*>(obj);
 Q_ASSERT(account);
 doStuff();
}
	\endcode
   */
	ActionGenerator(const QIcon &icon, const LocalizedString &text, const QObject *receiver, const char *member);
	// Convience constructor for menus
	ActionGenerator(const QIcon &icon, const LocalizedString &text, const char *member);
#ifndef Q_QDOC
	ActionGenerator(ActionGeneratorPrivate &priv);
#endif
	virtual ~ActionGenerator();
	QIcon icon() const;
	const LocalizedString &text() const;
	const QObject *receiver() const;
	const char *member() const;
	ActionGenerator *addProperty(const QByteArray &name, const QVariant &value);
	int type() const;
	ActionGenerator *setType(int type);
	int priority() const;
	ActionGenerator *setPriority(int priority);
	void setMenuController(MenuController *controller);
	void subscribe(QObject *object, const char *method);
	//void unsubscribe(QObject *object); //TODO implement me
	void addHandler(int type, QObject *obj);
	void removeHandler(int type, QObject *obj);
	void setCheckable(bool checkable);
	void setChecked(bool checked);
	void setToolTip(const LocalizedString &toolTip);
	void setShortcut(const QKeySequence &shortcut);
	void setShortcut(const QString &id);
	QString shortcut() const;
	void setMenuRole(QAction::MenuRole role);
	QAction::MenuRole menuRole() const;
	void setIconVisibleInMenu(bool visible);
	bool iconVisibleInMenu() const;
	QList<QAction*> actions(QObject *object) const;
	QMap<QObject*, QAction*> actions() const;
	static ActionGenerator *get(QAction *);
protected:
	QAction *prepareAction(QAction *action) const;
	virtual QObject *generateHelper() const;
	virtual const QMetaObject *metaObject() const;
	virtual QList<QByteArray> interfaces() const;
	void create(QAction *action,QObject *obj) const;
	virtual void createImpl(QAction *action,QObject *obj) const;
	virtual void showImpl(QAction *action,QObject *obj); //obj usally is controller, default implementation do nothing
	virtual void hideImpl(QAction *action,QObject *obj);
private:
	friend class MenuController;
	friend class ActionToolBar;
	friend class MenuControllerPrivate;
	friend class ActionValue;
	friend class ActionContainerPrivate;
};

class LIBQUTIM_EXPORT MenuActionGenerator : public ActionGenerator
{
	Q_DECLARE_PRIVATE(ActionGenerator)
	Q_DISABLE_COPY(MenuActionGenerator)
public:
	MenuActionGenerator(const QIcon &icon, const LocalizedString &text, MenuController *controller);
	virtual ~MenuActionGenerator();
protected:
	virtual QObject *generateHelper() const;
};
}

Q_DECLARE_METATYPE(qutim_sdk_0_3::ActionGenerator*)

#endif // ACTIONGENERATOR_H

