#ifndef ACTIONGENERATOR_P_H
#define ACTIONGENERATOR_P_H

#include "actiongenerator.h"
#include "objectgenerator_p.h"

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
	void sendActionCreatedEvent(QAction *action, QObject *controller) const;
};

class ActionGeneratorLocalizationHelper : public QObject
{
	Q_OBJECT
public:
	ActionGeneratorLocalizationHelper();
	virtual bool eventFilter(QObject *obj, QEvent *ev);
	void addAction(QAction *action, const ActionGeneratorPrivate *data);
	void addAction(QObject *obj, QAction *action);
	ActionGenerator *getGenerator(QAction*) const;
public slots:
	void onActionDeath(QObject *obj);
private:
	QMap<QAction*, const ActionGeneratorPrivate*> m_actions;
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

#endif // ACTIONGENERATOR_P_H
