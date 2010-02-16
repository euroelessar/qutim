#ifndef QUETZALACTIONGENERATOR_H
#define QUETZALACTIONGENERATOR_H

#include <qutim/actiongenerator.h>
#include <purple.h>

using namespace qutim_sdk_0_3;
struct QuetzalActionInfo;

class QuetzalActionGenerator : public ActionGenerator
{
public:
	QuetzalActionGenerator(PurpleMenuAction *action, PurpleBlistNode *node);
protected:
	QObject *generateHelper() const;
private:
	QScopedPointer<QuetzalActionInfo> m_info;
};

#endif // QUETZALACTIONGENERATOR_H
