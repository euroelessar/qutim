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

#include "meegointegration.h"

namespace MeegoIntegration {
using namespace qutim_sdk_0_3;

MeeGoIntegration::MeeGoIntegration()
{
}

void MeeGoIntegration::init()
{
}

bool MeeGoIntegration::isAvailable() const
{
	return true;
}

int MeeGoIntegration::priority()
{
	return DesktopEnvironment;
}

QVariant MeeGoIntegration::doGetValue(SystemIntegration::Attribute attr, const QVariant &data) const
{
	Q_UNUSED(attr);
	Q_UNUSED(data);
	return QVariant();
}

QVariant MeeGoIntegration::doProcess(SystemIntegration::Operation act, const QVariant &data) const
{
	switch (act) {
	case ShowWidget: {
		QWidget *widget = data.value<QWidget*>();
		m_window->showWidget(widget);
		return QVariant();
	}
	default:
		break;
	}
	return QVariant();
}

bool MeeGoIntegration::canHandle(SystemIntegration::Attribute attribute) const
{
	Q_UNUSED(attribute);
	return false;
}

bool MeeGoIntegration::canHandle(SystemIntegration::Operation operation) const
{
	switch (operation) {
	case ShowWidget:
		return true;
	default:
		return false;
	}
}

}
