/****************************************************************************
 *  popup.cpp
 *
 *  Copyright (c) 2009 by Sidorov Aleksey <sauron@citadelspb.com>
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
#include <libqutim/chatunit.h>
#include <libqutim/messagesession.h>

namespace KineticPopups
{

	Popup::Popup ( const QString& id, uint timeOut )
			:	id ( id ), timeout ( timeOut )
	{

	}

	Popup::Popup(QObject* parent): QObject(parent)
	{

	}

	void Popup::setSender(QObject *sender)
	{
		m_sender = sender;
	}

	QObject* Popup::getSender() const
	{
		return m_sender;
	}

	void Popup::setId(const QString& id)
	{
		this->id = id;
	}

	void Popup::setTimeOut(uint timeOut)
	{
		this->timeout = timeOut;
	}

	Popup::~Popup()
	{
		Manager::self()->remove (id);
		Manager::self()->updateGeometry();
		notification_widget->deleteLater();
	}

	void Popup::setMessage ( const QString& title, const QString& body, const QString &imagePath )
	{
		this->title = title;
		this->body = body;
		this->image_path = imagePath;
	}

	void Popup::appendMessage ( const QString& message )
	{
		body += "<br />" + message;
		updateMessage();
	}
	
	void Popup::updateMessage(const QString& message)
	{
		int message_count = notification_widget->property("messageCount").toInt() + 1;
		body = message + tr("<p> + %1 more notifications </p>").arg(message_count);
		notification_widget->setProperty("messageCount",message_count);
		updateMessage();
	}
	
	void Popup::updateMessage()
	{
		QSize newSize = notification_widget->setData(title,body,image_path);
		show_geometry.setSize(newSize);
		updateGeometry(show_geometry);
		Manager::self()->updateGeometry();
		show_state->assignProperty(notification_widget,"geometry",show_geometry);
		if (timeout > 0) {
			killTimer(timer_id);
			timer_id = startTimer(timeout);
		}
	}

	QString Popup::getId() const
	{
		return id;
	}

	void Popup::send()
	{
		Manager *manager = Manager::self();
		machine = new QStateMachine(this);

		notification_widget = new PopupWidget (manager->popupSettings);
		notification_widget->setData(title,body,image_path);
		QSize notify_size = notification_widget->setData ( title,body,image_path );
		connect (notification_widget,SIGNAL(action1Activated()),SLOT(action1Activated()));
		connect (notification_widget,SIGNAL(action2Activated()),SLOT(action2Activated()));

		show_geometry.setSize(notify_size);
		QRect geom = manager->insert(this);
		if ( geom.isEmpty() )
			deleteLater();

		show_state = new QState();
		hide_state = new QState();
		QFinalState *final_state = new QFinalState();

		int x = manager->popupSettings.margin + geom.width();
		int y = manager->popupSettings.margin + geom.height();
		geom.moveTop(geom.y() - y);

		show_state->assignProperty(notification_widget,"geometry",geom);
		show_geometry = geom;
		if (manager->animation & Slide)
			geom.moveLeft(geom.left() + x);
		hide_state->assignProperty(notification_widget,"geometry",geom);
		notification_widget->setGeometry(geom);

		show_state->addTransition(notification_widget,SIGNAL(action1Activated()),hide_state);
		show_state->addTransition(notification_widget,SIGNAL(action2Activated()),hide_state);
		show_state->addTransition(this,SIGNAL(updated()),show_state); //Black magic

		if (timeout > 0) {
			timer_id = startTimer(timeout);
			connect(this,SIGNAL(timeoutReached()),notification_widget,SIGNAL(action2Activated()));
		}

		machine->addState(show_state);
		machine->addState(hide_state);
		machine->addState(final_state);
		machine->setInitialState (show_state);

		if (manager->animation) {
			QPropertyAnimation *moving = new QPropertyAnimation ( notification_widget,"geometry" );
			machine->addDefaultAnimation (moving);
			moving->setDuration ( manager->animationDuration);
			moving->setEasingCurve (manager->easingCurve);
			connect(moving,SIGNAL(finished()),hide_state,SIGNAL(exited()));//HACK
			
			if (manager->animation & Opacity) {
				QPropertyAnimation *opacity = new QPropertyAnimation(notification_widget,"windowOpacity");
				machine->addDefaultAnimation(opacity);
				opacity->setDuration (manager->animationDuration);
				opacity->setEasingCurve (manager->easingCurve);
				notification_widget->setProperty("windowOpacity",0);
				show_state->assignProperty(notification_widget,"windowOpacity",1);
				hide_state->assignProperty(notification_widget,"windowOpacity",0);
			}
			hide_state->addTransition(hide_state,SIGNAL(exited()),final_state);
		}
		else {
			hide_state->addTransition(hide_state,SIGNAL(entered()),final_state);
		}

		connect(machine,SIGNAL(finished()),SLOT(deleteLater()));
		machine->start();
		notification_widget->show();
	}

	void Popup::update(QRect geom)
	{
		show_state->assignProperty(notification_widget,"geometry",geom);
		updateGeometry(geom);
		if (Manager::self()->animation & Slide)
			geom.moveRight(geom.right() + notification_widget->width() + Manager::self()->popupSettings.margin);
		hide_state->assignProperty(notification_widget,"geometry",geom);
	}

	QRect Popup::geometry() const
	{
		return show_geometry;
	}

	void Popup::updateGeometry(const QRect &newGeometry)
	{
		show_geometry = newGeometry;
		emit updated();
	}

	void Popup::timerEvent ( QTimerEvent* ev)
	{
		emit timeoutReached();
		QObject::timerEvent(ev);
	}

	void Popup::action1Activated()
	{
		ChatUnit *unit = qobject_cast<ChatUnit *>(m_sender);
		ChatSession *sess;
		if (unit && (sess = ChatLayer::get(unit)))
		{
			sess->setActive(true);
		}
	}

	void Popup::action2Activated()
	{
		ChatUnit *unit = qobject_cast<ChatUnit *>(m_sender);
		ChatSession *sess;
		if (unit && (sess = ChatLayer::get(unit,false)))
		{
			//TODO
			if (!sess->isActive())
				sess->deleteLater();
		}
	}

}
