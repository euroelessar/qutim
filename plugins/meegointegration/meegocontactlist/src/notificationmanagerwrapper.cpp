/****************************************************************************
**
** qutIM instant messenger
**
** Copyright (C) 2011 Ruslan Nigmatullin <euroelessar@ya.ru>
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

#include "notificationmanagerwrapper.h"

namespace MeegoIntegration
{
using namespace qutim_sdk_0_3;

NotificationManagerWrapper::NotificationManagerWrapper()
{
}

void NotificationManagerWrapper::init()
{
	qmlRegisterType<NotificationManagerWrapper>("org.qutim", 0, 3, "NotificationManager");
	qmlRegisterType<NotificationBackendWrapper>("org.qutim", 0, 3, "NotificationBackend");
}

void NotificationManagerWrapper::setBackendState(const QString &type, bool enabled)
{
	NotificationManager::setBackendState(type.toUtf8(), enabled);
}

void NotificationManagerWrapper::enableBackend(const QString &type)
{
	setBackendState(type, true);
}

void NotificationManagerWrapper::disableBackend(const QString &type)
{
	setBackendState(type, true);
}

bool NotificationManagerWrapper::isBackendEnabled(const QString &type)
{
	return isBackendEnabled(type);
}

void NotificationManagerWrapper::onBackendStateChanged(const QByteArray &type, bool enabled)
{
	emit backendStateChanged(QString::fromUtf8(type), enabled);
}

NotificationBackendWrapper::NotificationBackendWrapper()
{
}

NotificationBackendWrapper::~NotificationBackendWrapper()
{
}

QString NotificationBackendWrapper::type() const
{
	return QString::fromUtf8(m_type);
}

void NotificationBackendWrapper::setType(const QString &type)
{
	if (type == m_type)
		return;
	bool enabled = isEnabled();
	m_type = type.toUtf8();
	emit typeChanged(type);
	if (enabled != isEnabled())
		emit enabledChanged(!enabled);
}

bool NotificationBackendWrapper::isEnabled() const
{
	return NotificationManager::isBackendEnabled(m_type);
}

void NotificationBackendWrapper::setEnabled(bool enabled)
{
	NotificationManager::setBackendState(m_type, enabled);
}

void NotificationBackendWrapper::onBackendStateChanged(const QByteArray &type, bool enabled)
{
	if (m_type == type)
		emit enabledChanged(enabled);
}
}
