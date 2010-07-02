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
		ActionData() {}
	};
	
	enum ActionConnectionType
	{
		ActionConnectionLegacy,		// methodName()
		ActionConnectionObjectOnly,	// methodName(QObject*)
		ActionConnectionActionOnly,	// methodName(QAction*)
		ActionConnectionFull,		// methodName(QAction*,QObject*)
		ActionConnectionSimple		// methodName()
	};

	class ActionGeneratorPrivate : public ObjectGeneratorPrivate
	{
	public:
		ActionGeneratorPrivate() : type(0), priority(0), connectionType(ActionConnectionLegacy) {}
		QIcon icon;
		LocalizedString text;
		LocalizedString toolTip;
		QPointer<QObject> receiver;
		QByteArray member;
		int type;
		int priority;
		ActionConnectionType connectionType;
		union
		{
			LegacyActionData *legacyData;
			ActionData *data;
		};
		
		static ActionGeneratorPrivate *get(ActionGenerator *gen) { return gen->d_func(); }
		static const ActionGeneratorPrivate *get(const ActionGenerator *gen) { return gen->d_func(); }
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
