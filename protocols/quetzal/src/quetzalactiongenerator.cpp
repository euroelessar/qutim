/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright (C) 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
**
*****************************************************************************
**
** $QUTIM_BEGIN_LICENSE$
** This program is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
** See the GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program.  If not, see http://www.gnu.org/licenses/.
** $QUTIM_END_LICENSE$
**
****************************************************************************/
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
	: ActionGenerator(QIcon(), LocalizedString(action->label), QuetzalEventLoop::instance(), SLOT(onAction(QAction*))),
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
	: ActionGenerator(QIcon(), LocalizedString(action->label), QuetzalEventLoop::instance(), SLOT(onAction(QAction*)))
{
	m_action = QSharedPointer<PurplePluginAction>(action, purple_plugin_action_free);
}

// Yeah it's bad, I know
void QuetzalEventLoop::onAction(QAction *action)
{
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

