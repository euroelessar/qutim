#include "quetzalactiongenerator.h"
#include "quetzaleventloop.h"
#include <QtPlugin>

struct QuetzalActionInfo
{
	PurpleCallback cb;
	void *user_data;
	void *node;
};

Q_DECLARE_METATYPE(QuetzalActionInfo)

QuetzalActionGenerator::QuetzalActionGenerator(PurpleMenuAction *action, void *node)
	: ActionGenerator(QIcon(), LocalizedString(action->label), QuetzalEventLoop::instance(), SLOT(onAction())),
	m_info(new QuetzalActionInfo)
{
	m_info->cb = action->callback;
	m_info->user_data = action->data;
	m_info->node = node;
}

Q_EXTERN_C void purple_plugin_action_free(PurplePluginAction *action)
{
	g_free(action->label);
	g_free(action);
}

QuetzalActionGenerator::QuetzalActionGenerator(PurplePluginAction *action)
	: ActionGenerator(QIcon(), LocalizedString(action->label), QuetzalEventLoop::instance(), SLOT(onAction()))
{
	m_action = QSharedPointer<PurplePluginAction>(action, purple_plugin_action_free);
}

// Yeah it's bad, I know
void QuetzalEventLoop::onAction()
{
	QAction *action = qobject_cast<QAction *>(sender());
	QVariant var = action->property("actionInfo");
	if (var.canConvert<QuetzalActionInfo>()) {
		QuetzalActionInfo info = var.value<QuetzalActionInfo>();
		typedef void (*_callback)(gpointer, gpointer);
		_callback cb = (_callback) info.cb;
		cb(info.node, info.user_data);
	} else {
		PurplePluginAction *action = var.value<QSharedPointer<PurplePluginAction> >().data();
		action->callback(action);
	}
}

QObject *QuetzalActionGenerator::generateHelper() const
{
	QAction *action = prepareAction(new QAction(NULL));
	if (m_action)
		action->setProperty("actionInfo", qVariantFromValue(m_action));
	else
		action->setProperty("actionInfo", qVariantFromValue(*m_info));
	return action;
}
