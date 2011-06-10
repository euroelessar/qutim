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
	ActionData() : controller(0),menu(0),checkable(0),checked(0) {}
	MenuController *controller;
	QMenu *menu;
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
	QWeakPointer<QObject> m_object;
	QWeakPointer<QAction> m_action;
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

//	class SubscribersHandler : public QObject
//	{
//		Q_OBJECT
//	public:
//		SubscribersHandler(ActionGeneratorPrivate *gen_p);
//		ActionGeneratorPrivate *p;
//	public slots:
//		void onSubscriberDeath(QObject *subscriber);
//	};
}

Q_DECLARE_METATYPE(QSharedPointer<QShortcut>)

#endif // ACTIONGENERATOR_P_H
