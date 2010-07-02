/****************************************************************************
 *  actiongenerator.h
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

#ifndef ACTIONGENERATOR_H
#define ACTIONGENERATOR_H

#include "objectgenerator.h"
#include "localizedstring.h"
#include <QtGui/QIcon>
#include <QtGui/QAction>
#include <QtCore/QEvent>

namespace qutim_sdk_0_3
{
	class ActionGenerator;
	class ActionGeneratorPrivate;
	class MenuController;

	class LIBQUTIM_EXPORT ActionCreatedEvent : public QEvent
	{
	public:
		ActionCreatedEvent(QAction *action, ActionGenerator *gen);
		static QEvent::Type eventType();
		QAction *action() const { return m_action; }
		ActionGenerator *generator() const { return m_gen; }
	private:
		QAction *m_action;
		ActionGenerator *m_gen;
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
	// QWeakPointer at cache and QSharedPointer at ActionContainer
	//
	// Try to remove any use of legacy ActionGenerator's members everywhere at qutIM
	class LIBQUTIM_EXPORT ActionGenerator : public ObjectGenerator
	{
		Q_DECLARE_PRIVATE(ActionGenerator)
		Q_DISABLE_COPY(ActionGenerator)
	public:
		enum Type { StatusType = 0, GeneralType };
		
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
		MenuController *controller() const;
		void addCreationHandler(QObject *obj);
		void setCheckable(bool checkable);
		void setChecked(bool checked);
		void setToolTip(const LocalizedString &toolTip);
	protected:
		QAction *prepareAction(QAction *action) const;
		virtual QObject *generateHelper() const;
		virtual const QMetaObject *metaObject() const;
		virtual bool hasInterface(const char *id) const;
	private:
		friend class MenuController;
		friend class MenuControllerPrivate;
	};

	class LIBQUTIM_EXPORT MenuActionGenerator : public ActionGenerator
	{
		Q_DECLARE_PRIVATE(ActionGenerator)
		Q_DISABLE_COPY(MenuActionGenerator)
	public:
		MenuActionGenerator(const QIcon &icon, const LocalizedString &text, QMenu *menu);
		MenuActionGenerator(const QIcon &icon, const LocalizedString &text, MenuController *controller);
		virtual ~MenuActionGenerator();
	protected:
		virtual QObject *generateHelper() const;
	};
}

Q_DECLARE_METATYPE(qutim_sdk_0_3::ActionGenerator*)

#endif // ACTIONGENERATOR_H
