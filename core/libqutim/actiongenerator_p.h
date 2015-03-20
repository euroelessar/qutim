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
#ifndef ACTIONGENERATOR_P_H
#define ACTIONGENERATOR_P_H

#include "actiongenerator.h"
#include "objectgenerator_p.h"
#include "shortcut_p.h"
#include <QObjectCleanupHandler>

namespace qutim_sdk_0_3
{
struct LegacyActionData
{
	LegacyActionData() : controller(NULL), menu(NULL), checkable(false), checked(false) {}
	MenuController *controller;
	QMenu *menu;
    mutable QList<QPointer<QObject> > handlers;
	bool checkable;
	bool checked;
};

struct ActionData
{
	ActionData() : checkable(0), checked(0) {}
	QPointer<MenuController> controller;
	bool checkable;
	bool checked;
};

enum ActionConnectionType
{
	ActionConnectionNone		= 0x01,		// member is null
	ActionConnectionSimple		= 0x02,	// methodName()
	ActionConnectionObjectOnly	= 0x04,	// methodName(QObject*)
	ActionConnectionActionOnly	= 0x08,	// methodName(QAction*)
	ActionConnectionFull		= 0x0C	// methodName(QAction*,QObject*)
};

class ActionGeneratorPrivate : public ObjectGeneratorPrivate
{
	Q_DECLARE_PUBLIC(ActionGenerator)
public:
	ActionGeneratorPrivate();
	ActionGenerator *q_ptr;
	QIcon icon;
	LocalizedString text;
	LocalizedString toolTip;
    QPointer<QObject> receiver;
	QMap<int, QList<QObject* > > subcribers;
	QList<QPair<QObject*, QByteArray> > receivers;
	QByteArray member;
	int type;
	int priority;
	ActionConnectionType connectionType;
	ActionData *data;
	QAction::MenuRole menuRole;
	bool iconVisibleInMenu;
	static ActionGeneratorPrivate *get(ActionGenerator *gen) { return gen->d_func(); }
	static const ActionGeneratorPrivate *get(const ActionGenerator *gen) { return gen->d_func(); }
	void ensureConnectionType();
	void show(QAction *act,QObject *con);
	void hide(QAction *act,QObject *con);
	QList<QKeySequence> shortCuts;
	QString shortCut;
	void sendActionCreatedEvent(QAction *action, QObject *controller) const;
};

class ActionGeneratorHelper : public QObject
{
	Q_OBJECT
public:
	ActionGeneratorHelper();
	~ActionGeneratorHelper();
	virtual bool eventFilter(QObject *obj, QEvent *ev);
	void addAction(QAction *action, const ActionGeneratorPrivate *data);
	void updateSequence(const QString &id, const QKeySequence &key);
	void handleDeath(ActionGeneratorPrivate *data);
	ActionGenerator *getGenerator(QAction*) const;
public slots:
	void onActionDeath(QObject *obj);
private:
	QMultiHash<QString, QAction *> m_shortcuts;
	QMap<QAction*, const ActionGeneratorPrivate*> m_actions;
};

class ActionCleanupHandler : public QObjectCleanupHandler
{
	Q_OBJECT
public:
	static ActionCleanupHandler *get(QObject *object);

protected:
    ActionCleanupHandler(QObject *parent);
};

class ActionPointerData : public QSharedData
{
	Q_DISABLE_COPY(ActionPointerData)
public:
	ActionPointerData() : m_generator(0) {}
	ActionPointerData(QObject *o, const ActionGenerator *gen) : m_object(o), m_generator(gen) {}
	~ActionPointerData() { if (m_action) delete m_action.data(); }

	QAction *action();
private:
	QPointer<QObject> m_object;
	QPointer<QAction> m_action;
	const ActionGenerator *m_generator;
};

class ActionPointer
{
public:
	ActionPointer();
	ActionPointer(const ActionPointer &o);
	ActionPointer &operator =(const ActionPointer &o);
	~ActionPointer();
private:
	QExplicitlySharedDataPointer<ActionPointerData> d;
};

}

Q_DECLARE_METATYPE(QSharedPointer<QShortcut>)

#endif // ACTIONGENERATOR_P_H

