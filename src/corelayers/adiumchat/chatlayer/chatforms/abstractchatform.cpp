/****************************************************************************
 *
 *  This file is part of qutIM
 *
 *  Copyright (c) 2010 by Nigmatullin Ruslan <euroelessar@gmail.com>
 *
 ***************************************************************************
 *                                                                         *
 *   This file is part of free software; you can redistribute it and/or    *
 *   modify it under the terms of the GNU General Public License as        *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************
 ****************************************************************************/

#include "abstractchatform.h"
#include "abstractchatwidget.h"
#include <qutim/conference.h>
#include <qutim/configbase.h>
#include <qutim/messagesession.h>
#include <qutim/debug.h>
#include <chatlayer/chatsessionimpl.h>
#include <QPlainTextEdit>
#include <qutim/systemintegration.h>

namespace Core
{
namespace AdiumChat
{

AbstractChatForm::AbstractChatForm()
{
	connect(ChatLayer::instance(), SIGNAL(sessionCreated(qutim_sdk_0_3::ChatSession*)),
			SLOT(onSessionCreated(qutim_sdk_0_3::ChatSession*)));
}

void AbstractChatForm::onChatWidgetDestroyed(QObject *object)
{
	AbstractChatWidget *widget = reinterpret_cast<AbstractChatWidget*>(object);
	QString key = m_chatwidgets.key(widget);
	m_chatwidgets.remove(key);
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
	qDebug("%s", Q_FUNC_INFO);
	foreach (AbstractChatWidget *widget, m_chatwidgets) {
		if (widget)
			widget->loadSettings();
	}
}


AbstractChatWidget *AbstractChatForm::findWidget(ChatSession *sess) const
{
	QHash<QString, AbstractChatWidget*>::const_iterator it;
	ChatSessionImpl *session = qobject_cast<ChatSessionImpl*>(sess);
	for (it=m_chatwidgets.constBegin();it!=m_chatwidgets.constEnd();it++) {
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
	foreach (QWidget *widget, m_chatwidgets)
		list << widget;
	return list;
}

QWidget* AbstractChatForm::chatWidget(ChatSession* session) const
{
	return findWidget(session);
}

AbstractChatWidget *AbstractChatForm::widget(const QString &key)
{
	AbstractChatWidget *widget = m_chatwidgets.value(key,0);
	if (!widget) {
		widget = createWidget(key);
		widget->addActions(m_actions);
		m_chatwidgets.insert(key,widget);
		connect(widget,SIGNAL(destroyed(QObject*)),SLOT(onChatWidgetDestroyed(QObject*)));
#ifdef Q_WS_MAEMO_5
		widget->setAttribute(Qt::WA_Maemo5AutoOrientation, true);
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
	foreach (AbstractChatWidget *widget,m_chatwidgets) {
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
