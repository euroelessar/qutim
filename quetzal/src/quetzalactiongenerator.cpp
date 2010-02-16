#include "quetzalactiongenerator.h"
#include "quetzaleventloop.h"

struct QuetzalActionInfo
{
	PurpleCallback cb;
	void *user_data;
	PurpleBlistNode *node;
};

Q_DECLARE_METATYPE(QuetzalActionInfo)

QuetzalActionGenerator::QuetzalActionGenerator(PurpleMenuAction *action, PurpleBlistNode *node)
	: ActionGenerator(QIcon(), LocalizedString(action->label), QuetzalTimer::instance(), SLOT(onAction())),
	m_info(new QuetzalActionInfo)
{
	m_info->cb = action->callback;
	m_info->user_data = action->data;
	m_info->node = node;
}

// Yeah it's bad, I know
void QuetzalTimer::onAction()
{
	QAction *action = qobject_cast<QAction *>(sender());
	QuetzalActionInfo info = action->property("actionInfo").value<QuetzalActionInfo>();
	typedef void (*_callback)(gpointer, gpointer);
	_callback cb = (_callback) info.cb;
	cb(info.node, info.user_data);
}

QObject *QuetzalActionGenerator::generateHelper() const
{
	QAction *action = prepareAction(new QAction(NULL));
	action->setProperty("actionInfo", qVariantFromValue(*m_info));
	return action;
}
