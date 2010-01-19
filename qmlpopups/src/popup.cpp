/****************************************************************************
 *  popup.cpp
 *
 *  Copyright (c) 2010 by Sidorov Aleksey <sauron@citadelspb.com>
 *
 ***************************************************************************
 *                                                                         *
 *   This library is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
*****************************************************************************/

#include "popup.h"
#include "popupwidget.h"
#include "manager.h"
#include <QPropertyAnimation>
#include <QApplication>
#include <QTimer>
#include <QFinalState>
#include <QState>
#include <QDebug>
#include <QStateMachine>

namespace QmlPopups
{

	Popup::Popup ( const QString& id)
			:	m_id ( id ), m_machine(0), m_notification_widget(0)
	{

	}
	
	void Popup::setId(const QString& id)
	{
		this->m_id = id;
	}

	Popup::~Popup()
	{
		Manager::self()->remove (m_id);
		Manager::self()->updateGeometry();
		m_notification_widget->deleteLater();
	}

	void Popup::setMessage ( const QString& title, const QString& body, QObject *sender )
	{
		m_title = title;
		m_body = body;
		m_sender = sender;
	}

	void Popup::appendMessage ( const QString& message )
	{
		m_body += "<br />" + message;
		updateMessage();
	}
	
	void Popup::updateMessage(const QString& message)
	{
		int message_count = m_notification_widget->property("messageCount").toInt() + 1;
		m_body = message + tr("<p> + %1 more notifications </p>").arg(message_count);
		m_notification_widget->setProperty("messageCount",message_count);
		updateMessage();
	}
	
	void Popup::updateMessage()
	{
		Manager *manager = Manager::self();
		
		m_notification_widget->setData(m_title,m_body,m_sender);

		if (manager->timeout > 0) {
			killTimer(m_timer_id);
			m_timer_id = startTimer(manager->timeout);
		}
	}

	QString Popup::getId() const
	{
		return m_id;
	}

	void Popup::send()
	{
		Manager *manager = Manager::self();
		
		if (m_machine || m_notification_widget) {
			qWarning("Notification already sended");
			return;
		}
		m_machine = new QStateMachine(this);

		m_notification_widget = new PopupWidget ();
		m_notification_widget->setData ( m_title,m_body,m_sender );
		QSize notify_size = m_notification_widget->sizeHint();

		m_show_geometry.setSize(notify_size);
		QRect geom = manager->insert(this);
		if ( geom.isEmpty() )
			deleteLater();

		m_show_state = new QState();
		m_hide_state = new QState();
//		m_update_state = new QState();
		QFinalState *final_state = new QFinalState();

		int x = manager->margin + geom.width();
		int y = manager->margin + geom.height();
		geom.moveTop(geom.y() - y);

		m_show_state->assignProperty(m_notification_widget,"geometry",geom);
		m_show_geometry = geom;
		if (manager->animation & Slide)
			geom.moveLeft(geom.left() + x);
		m_hide_state->assignProperty(m_notification_widget,"geometry",geom);
		m_notification_widget->setGeometry(geom);

		m_show_state->addTransition(m_notification_widget,SIGNAL(activated()),m_hide_state);
		m_show_state->addTransition(this,SIGNAL(updated()),m_show_state);
		m_hide_state->addTransition(m_hide_state,SIGNAL(propertiesAssigned()),final_state);
		connect(m_notification_widget,SIGNAL(PopupResized(QSize)),this,SLOT(onPopupResized(QSize)));

		if (manager->timeout > 0) {
			m_timer_id = startTimer(manager->timeout);
			connect(this,SIGNAL(timeoutReached()),m_notification_widget,SLOT(onTimeoutReached()));
		}

		m_machine->addState(m_show_state);
		m_machine->addState(m_hide_state);
		m_machine->addState(final_state);
		m_machine->setInitialState (m_show_state);

		if (manager->animation) {
			QPropertyAnimation *moving = new QPropertyAnimation ( m_notification_widget,"geometry" );
			m_machine->addDefaultAnimation (moving);
			moving->setDuration ( manager->animationDuration);
			moving->setEasingCurve (manager->easingCurve);			
			
			if (manager->animation & Opacity) {
				QPropertyAnimation *opacity = new QPropertyAnimation(m_notification_widget,"windowOpacity");
				m_machine->addDefaultAnimation(opacity);
				opacity->setDuration (manager->animationDuration);
				opacity->setEasingCurve (manager->easingCurve);
				m_notification_widget->setProperty("windowOpacity",0);
				m_show_state->assignProperty(m_notification_widget,"windowOpacity",1);
				m_hide_state->assignProperty(m_notification_widget,"windowOpacity",0);
			}
		}

		connect(m_machine,SIGNAL(finished()),SLOT(deleteLater()));
		m_machine->start();
		m_notification_widget->show();
	}

	void Popup::update(QRect geom)
	{
		if (Manager::self()->animation & Slide)
			geom.moveRight(geom.right() + m_notification_widget->width() + Manager::self()->margin);
		m_show_state->assignProperty(m_notification_widget,"geometry",geom);
		m_hide_state->assignProperty(m_notification_widget,"geometry",geom);
		updateGeometry(geom);
	}

	QRect Popup::geometry() const
	{
		return m_show_geometry;
	}

	void Popup::updateGeometry(const QRect &newGeometry)
	{
		m_show_geometry = newGeometry;		
		emit updated();
	}

	void Popup::timerEvent(QTimerEvent *e)
	{
		emit timeoutReached();
		killTimer(m_timer_id);
		QObject::timerEvent(e);
	}
	
	void Popup::onPopupResized(const QSize &size)
	{
		m_show_geometry.setSize(size);
		updateGeometry(m_show_geometry);
		Manager::self()->updateGeometry();
		m_show_state->assignProperty(m_notification_widget,"geometry",m_show_geometry);
	}

}

