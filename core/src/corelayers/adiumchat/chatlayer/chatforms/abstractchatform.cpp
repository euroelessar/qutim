/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
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

#include "abstractchatform.h"
#include "abstractchatwidget.h"
#include "chatviewfactory.h"
#include <qutim/conference.h>
#include <qutim/configbase.h>
#include <qutim/chatsession.h>
#include <qutim/debug.h>
#include <chatlayer/chatsessionimpl.h>
#include <QPlainTextEdit>
#include <qutim/systemintegration.h>
#include <qutim/servicemanager.h>

namespace Core
{
namespace AdiumChat
{

AbstractChatForm::AbstractChatForm()
{
	connect(ServiceManager::instance(), SIGNAL(serviceChanged(QObject*,QObject*)),
	        SLOT(onServiceChanged(QObject*,QObject*)));
	connect(ChatLayer::instance(), SIGNAL(sessionCreated(qutim_sdk_0_3::ChatSession*)),
			SLOT(onSessionCreated(qutim_sdk_0_3::ChatSession*)));
}

void AbstractChatForm::onChatWidgetDestroyed(QObject *object)
{
	AbstractChatWidget *widget = reinterpret_cast<AbstractChatWidget*>(object);
	QString key = m_chatWidgets.key(widget);
	m_chatWidgets.remove(key);
}


QString AbstractChatForm::getWidgetId(ChatSessionImpl *sess) const
{
	ConfigGroup group = Config("behavior/chat").group("widget");
	int windows = group.value<int>("windows", 0);

	//TODO add configuration

	if (!windows) {
		return QLatin1String("session");
	} else  {
		if (qobject_cast<const Conference*>(sess->getUnit())) {
			return QLatin1String("conference");
		} else {
			return QLatin1String("chat");
		}
	}
}

void AbstractChatForm::onSessionActivated(bool active)
{
	//init or update chat widget(s)
	ChatSessionImpl *session = qobject_cast<ChatSessionImpl*>(sender());
	if (!session)
		return;
	QString key = getWidgetId(session);
	AbstractChatWidget *w = widget(key);
	if (active) {
		if (!w->contains(session))
			w->addSession(session);
		if (w->isHidden())
			SystemIntegration::show(w);
		w->activate(session);
	}
}

void AbstractChatForm::onSettingsChanged()
{
	debug() << Q_FUNC_INFO;
	foreach (AbstractChatWidget *widget, m_chatWidgets) {
		if (widget)
			widget->loadSettings();
	}
}

void AbstractChatForm::onServiceChanged(QObject *newObject, QObject *oldObject)
{
	if (this != newObject) {
		if (ChatViewFactory *factory = qobject_cast<ChatViewFactory*>(newObject)) {
			foreach (AbstractChatWidget *chat, m_chatWidgets)
				chat->setView(factory->createViewWidget());
		}
		return;
	}
	AbstractChatForm *form = qobject_cast<AbstractChatForm*>(oldObject);
	if (!form)
		return;

	QHashIterator<QString, AbstractChatWidget*> it(form->m_chatWidgets);
	QSet<ChatSessionImpl*> sessions;
	foreach (ChatSession *session, ChatLayer::instance()->sessions()) {
		if (ChatSessionImpl *impl = qobject_cast<ChatSessionImpl*>(session))
			sessions << impl;
	}

	while (it.hasNext()) {
		it.next();
		AbstractChatWidget *oldWidget = form->widget(it.key());
		AbstractChatWidget *newWidget = widget(it.key());
		QMutableSetIterator<ChatSessionImpl*> jt(sessions);
		while (jt.hasNext()) {
			ChatSessionImpl *session = jt.next();
			if (oldWidget->contains(session)) {
				newWidget->addSession(session);
				jt.remove();
			}
		}
		SystemIntegration::show(newWidget);
	}
}

AbstractChatWidget *AbstractChatForm::findWidget(ChatSession *sess) const
{
	QHash<QString, AbstractChatWidget*>::const_iterator it;
	ChatSessionImpl *session = qobject_cast<ChatSessionImpl*>(sess);
	for (it=m_chatWidgets.constBegin();it!=m_chatWidgets.constEnd();it++) {
		if ((*it) && it.value()->contains(session))
			return it.value();
	}
	return 0;
}

QObject *AbstractChatForm::textEdit(ChatSession *session)
{
	AbstractChatWidget *widget = findWidget(session);
	if (widget && widget->currentSession() == session)
		return widget->getInputField();
	return 0;
}

QWidgetList AbstractChatForm::chatWidgets()
{
	QWidgetList list;
	foreach (QWidget *widget, m_chatWidgets)
		list << widget;
	return list;
}

QWidget* AbstractChatForm::chatWidget(ChatSession* session) const
{
	return findWidget(session);
}

AbstractChatWidget *AbstractChatForm::widget(const QString &key)
{
	AbstractChatWidget *widget = m_chatWidgets.value(key,0);
	if (!widget) {
		widget = createWidget(key);
		widget->addActions(m_actions);
		m_chatWidgets.insert(key,widget);
		connect(widget,SIGNAL(destroyed(QObject*)),SLOT(onChatWidgetDestroyed(QObject*)));
#ifdef Q_WS_MAEMO_5
		Config config = Config().group(QLatin1String("Maemo5"));
		switch (config.value(QLatin1String("orientation"),0))
		{
		case 0:
			widget->setAttribute(Qt::WA_Maemo5AutoOrientation, true);
			break;
		case 1:
			widget->setAttribute(Qt::WA_Maemo5PortraitOrientation, true);
			break;
		case 2:
			widget->setAttribute(Qt::WA_Maemo5LandscapeOrientation, true);
			break;
		}
#endif
	}
	debug() << widget << key;
	return widget;
}

void AbstractChatForm::onSessionCreated(ChatSession *session)
{
	ChatSessionImpl *s = static_cast<ChatSessionImpl*>(session);
	QString key = getWidgetId(s);
	AbstractChatWidget *w = widget(key);
	if (!w->contains(s))
		w->addSession(s);
	connect(s, SIGNAL(activated(bool)), SLOT(onSessionActivated(bool)));
}

AbstractChatForm::~AbstractChatForm()
{
	foreach (AbstractChatWidget *widget,m_chatWidgets) {
		widget->disconnect(this);
		delete widget;
	}
}

void AbstractChatForm::addAction(qutim_sdk_0_3::ActionGenerator *gen)
{
	m_actions.append(gen);
}

void AbstractChatForm::removeAction(qutim_sdk_0_3::ActionGenerator *gen)
{
	m_actions.removeAll(gen);
}

}
}

