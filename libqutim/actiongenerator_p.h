#ifndef ACTIONGENERATOR_P_H
#define ACTIONGENERATOR_P_H

#include "actiongenerator.h"
#include "objectgenerator_p.h"

namespace qutim_sdk_0_3
{
	class ActionGeneratorPrivate : public ObjectGeneratorPrivate
	{
	public:
		ActionGeneratorPrivate() : type(0), priority(0), controller(NULL), menu(NULL) {}
		QIcon icon;
		LocalizedString text;
		QPointer<QObject> receiver;
		QByteArray member;
		int type;
		int priority;
		MenuController *controller;
		QMenu *menu;
	};
}

#endif // ACTIONGENERATOR_P_H
