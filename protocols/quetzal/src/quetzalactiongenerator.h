#ifndef QUETZALACTIONGENERATOR_H
#define QUETZALACTIONGENERATOR_H

#include <qutim/actiongenerator.h>
#include <QMetaType>
#include <purple.h>

using namespace qutim_sdk_0_3;
struct QuetzalActionInfo;

class QuetzalActionGenerator : public ActionGenerator
{
public:
	QuetzalActionGenerator(PurpleMenuAction *action, void *node);
	QuetzalActionGenerator(PurplePluginAction *action);
protected:
	QObject *generateHelper() const;
private:
	QScopedPointer<QuetzalActionInfo> m_info;
	QSharedPointer<PurplePluginAction> m_action;
};

Q_DECLARE_METATYPE(QSharedPointer<PurplePluginAction>)

#endif // QUETZALACTIONGENERATOR_H
