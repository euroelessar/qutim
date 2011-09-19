/****************************************************************************
**
** qutIM instant messenger
**
** Copyright (C) 2011 Evgeniy Degtyarev <degtep@gmail.com>
**
*****************************************************************************
**
** $QUTIM_BEGIN_LICENSE$
** This program is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 2 of the License, or
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

#include "maemo6idle.h"


namespace MeegoIntegration
{

Maemo6Idle::Maemo6Idle()
{
	m_qmActivity = new QmActivity();
	connect(m_qmActivity,SIGNAL(activityChanged (MeeGo::QmActivity::Activity)),this,SLOT(activityChanged (MeeGo::QmActivity::Activity)));
	m_activity = m_qmActivity->get();

	idle_timer = new QBasicTimer();
	idle_timer->start(60000,this);
}

void Maemo6Idle::timerEvent(QTimerEvent* ev)
{
	Q_UNUSED(ev);

	if (m_activity == MeeGo::QmActivity::Active)
	idleSeconds += 60;

	emit secondsIdle(idleSeconds);
}

void Maemo6Idle::activityChanged (MeeGo::QmActivity::Activity activity)
{
	m_activity = activity;
	if (m_activity == MeeGo::QmActivity::Active)
	{
		idleSeconds = 0;
		idle_timer->stop();
	}
	else
	{
		idle_timer->start(60000,this);
	}
}
}
