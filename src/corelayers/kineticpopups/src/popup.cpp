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
#include "backend.h"
#include "manager.h"
#include <QPropertyAnimation>
#include <QApplication>
#include <QTimer>
#include <QFinalState>
#include <QState>
#include <qutim/debug.h>
#include <QStateMachine>
#include <qutim/objectgenerator.h>

namespace Core
{
namespace KineticPopups
{
Popup::Popup ( const QString& id )
	:	m_popup(0),
	m_id ( id ),
	m_machine(0),
	m_moving_animation(0),
	m_opacity_animation(0)
{

}

void Popup::setId(const QString& id)
{
	this->m_id = id;
}


Popup::~Popup()
{
	Manager::self()->remove (m_id);
	m_popup->deleteLater();
	m_machine->stop();
}

void Popup::setMessage ( const QString& title, const QString& body, QObject *sender )
{
	m_title = title;
	m_body = body;
	m_sender = sender;
}

void Popup::setData ( const QVariant &data)
{
	m_data = data;
}

void Popup::appendMessage ( const QString& message )
{
	if (!message.isEmpty())
		m_body += "<br />" + message;
	updateMessage();
}

void Popup::updateMessage(const QString& message)
{
	int message_count = m_popup->property("messageCount").toInt() + 1;
	m_body = message + tr("<p> + %1 more notifications </p>").arg(message_count);
	m_popup->setProperty("messageCount",message_count);
	updateMessage();
}

void Popup::updateMessage()
{
	m_popup->setData(m_title,m_body,m_sender,m_data);
}


void Popup::onPopupWidgetSizeChanged ( const QSize& size )
{
	QRect geom = m_show_geometry;
	geom.setSize(size);
	update(geom);
	Manager::self()->updateGeometry();
}


QString Popup::getId() const
{
	return m_id;
}

void Popup::send()
{
	Manager *manager = Manager::self();
	if (m_machine || m_popup) {
		warning() << "Notification already sended";
		return;
	}
	m_machine = new QStateMachine(this);

	//FIXME
	const ObjectGenerator *gen = ObjectGenerator::module<AbstractPopupWidget>().value(0);
	Q_ASSERT(gen);
	m_popup = gen->generate<AbstractPopupWidget>();
	Q_ASSERT(m_popup);
	m_popup->setData(m_title,m_body,m_sender,m_data);
	QSize notify_size = m_popup->sizeHint();

	m_show_geometry.setSize(notify_size);
	QRect geom = manager->insert(this);
	if(geom.isEmpty())
		deleteLater();

	m_show_state = new QState(m_machine);
	m_hide_state = new QFinalState(m_machine);

	int x = manager->margin + geom.width();
	int y = manager->margin + geom.height();
	geom.moveTop(geom.y() - y);

	m_show_geometry = geom;
	if(manager->animation & Slide)
		geom.moveLeft(geom.left() + x);
	m_popup->setGeometry(geom);

	m_machine->setInitialState(m_show_state);

	if (manager->animation) {
		m_moving_animation = new QPropertyAnimation ( m_popup,"geometry",this );
		m_moving_animation->setDuration ( manager->animationDuration);
		m_moving_animation->setEasingCurve (manager->easingCurve);
	}

	if (manager->animation & Opacity) {
		m_opacity_animation = new QPropertyAnimation(m_popup,"windowOpacity",this);
		m_opacity_animation->setDuration (manager->animationDuration);
		m_opacity_animation->setEasingCurve (manager->easingCurve);
		m_opacity_animation->setStartValue(0);
		m_opacity_animation->setEndValue(1);
		m_opacity_animation->start();
	}

	connect(m_machine,SIGNAL(finished()),SLOT(deleteLater()));
	connect(m_popup,SIGNAL(sizeChanged(QSize)),SLOT(onPopupWidgetSizeChanged(QSize)));
	connect(m_popup,SIGNAL(activated()),SLOT(onPopupActivated()));
	m_machine->start();
	m_popup->show();
}

void Popup::update(QRect geom)
{
	if (Manager::self()->animation & Slide)
		geom.moveRight(geom.right() + m_popup->width() + Manager::self()->margin);
	if (m_moving_animation) {
		m_moving_animation->stop();
		m_moving_animation->setStartValue(m_show_geometry);
		m_moving_animation->setEndValue(geom);
		m_moving_animation->start();
	}
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

void Popup::onPopupActivated()
{
	disconnect(m_popup,SIGNAL(activated()),this,SLOT(onPopupActivated()));
	if (m_opacity_animation) {
		m_show_state->addTransition(m_opacity_animation,SIGNAL(finished()),m_hide_state);
		m_opacity_animation->stop();
		m_opacity_animation->setStartValue(1);
		m_opacity_animation->setEndValue(0);
		m_opacity_animation->start();
	}
	else
		m_show_state->addTransition(m_hide_state);
}


}
}
