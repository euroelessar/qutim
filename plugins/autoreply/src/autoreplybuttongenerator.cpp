#include "autoreplybuttongenerator.h"
#include "autoreplyplugin.h"

using namespace qutim_sdk_0_3;

AutoReplyButtonGenerator::AutoReplyButtonGenerator(AutoReplyPlugin *plugin)
	: ActionGenerator(QIcon(), LocalizedString("AutoReply"), 0, 0), m_plugin(plugin)
{
	setType(ActionTypeChatButton);
	setCheckable(true);
}

QObject *AutoReplyButtonGenerator::generateHelper() const
{
	QAction *action = prepareAction(new QAction(NULL));
	action->setChecked(m_plugin->isActive());
	QObject::connect(action, SIGNAL(toggled(bool)), m_plugin, SLOT(onActionToggled(bool)));
	QObject::connect(m_plugin, SIGNAL(activeChanged(bool)), action, SLOT(setChecked(bool)));
	return action;
}
