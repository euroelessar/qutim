#ifndef ACTIONGENERATOR_P_H
#define ACTIONGENERATOR_P_H

#include "actiongenerator.h"
#include "objectgenerator_p.h"

namespace qutim_sdk_0_3
{
	class ActionGeneratorPrivate : public ObjectGeneratorPrivate
	{
	public:
		ActionGeneratorPrivate() : type(0), priority(0), controller(NULL), menu(NULL),checkable(false),checked(false) {}
		QIcon icon;
		LocalizedString text;
		QPointer<QObject> receiver;
		QByteArray member;
		int type;
		int priority;
		MenuController *controller;
		QMenu *menu;
		mutable QList<QPointer<QObject> > handlers;
		bool checkable;
		bool checked;
		LocalizedString toolTip;
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
