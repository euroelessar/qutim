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

#include "vibrobackend.h"
#include <QtFeedback/QFeedbackActuator>
#include <cstdio>

#define VIBRO_STRONG_EFFECT_PATH "file:///usr/share/sounds/vibra/tct_message.ivt"
#define VIBRO_EFFECT_PATH "file:///usr/share/sounds/vibra/tct_chat.ivt"

namespace Core
{

static QtMsgHandler previousHandler;

void msgHandler(QtMsgType type, const char *msg)
{
	if (previousHandler)
		previousHandler(type, msg);
	if (msg[0] == 'M' && QChar(QLatin1Char(msg[1])).isUpper())
		return;
	fprintf(stderr, "%s", msg);
}

VibroBackend::VibroBackend()
	: NotificationBackend("Vibration"),
	  m_strongEffect(new QFeedbackFileEffect(this)),
	  m_effect(new QFeedbackFileEffect(this)),
	  m_windowActive(true)
{
	setDescription(QT_TR_NOOP("Vibrate"));
	m_strongEffect->setSource(QUrl(QLatin1String(VIBRO_STRONG_EFFECT_PATH)));
	m_strongEffect->load();
	m_effect->setSource(QUrl(QLatin1String(VIBRO_EFFECT_PATH)));
	m_effect->load();
	previousHandler = qInstallMsgHandler(msgHandler);
}

VibroBackend::~VibroBackend()
{
}

void VibroBackend::handleNotification(qutim_sdk_0_3::Notification *)
{
	if (m_windowActive)
		m_effect->start();
	else
		m_strongEffect->start();
}

void VibroBackend::setWindowActive(bool active)
{
	m_windowActive = active;
}

bool VibroBackend::windowActive() const
{
	return m_windowActive;
}

}
