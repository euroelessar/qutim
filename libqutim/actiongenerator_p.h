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
	public:
		ActionGeneratorPrivate() : type(0), priority(0), connectionType(ActionConnectionSimple) {}
		QIcon icon;
		LocalizedString text;
		LocalizedString toolTip;
		QPointer<QObject> receiver;
		QHash<int, QObjectList> subcribers;
		QByteArray member;
		int type;
		int priority;
		ActionConnectionType connectionType;
		ActionData *data;
		static ActionGeneratorPrivate *get(ActionGenerator *gen) { return gen->d_func(); }
		static const ActionGeneratorPrivate *get(const ActionGenerator *gen) { return gen->d_func(); }
		void ensureConnectionType();
	};
	
	class ActionGeneratorLocalizationHelper : public QObject
	{
		Q_OBJECT
	public:
		ActionGeneratorLocalizationHelper();
		virtual bool eventFilter(QObject *obj, QEvent *ev);
		void addAction(QAction *action, const ActionGeneratorPrivate *data);
	public slots:
		void onActionDeath(QObject *obj);
	private:
		QMap<QAction*, const ActionGeneratorPrivate*> m_actions;
	};
}

#endif // ACTIONGENERATOR_P_H
