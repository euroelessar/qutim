/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Aleksey Sidorov <gorthauer87@yandex.ru>
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

#include "xsettingslayerimpl.h"
#include <qutim/icon.h>
#include <qutim/systemintegration.h>
#include "xsettingswindow.h"
#include <QApplication>

namespace Core
{
XSettingsLayerImpl::XSettingsLayerImpl()
{

}


XSettingsLayerImpl::~XSettingsLayerImpl()
{

}


void XSettingsLayerImpl::show(const SettingsItemList& settings, QObject* controller)
{
	XSettingsWindow *d = m_dialogs.value(controller).data();
    if (!d) {
		d = new XSettingsWindow(settings, controller, qApp->activeWindow());
		m_dialogs[controller] = d;
	}
	SystemIntegration::show(d);
}

void XSettingsLayerImpl::update(const SettingsItemList& settings, QObject* controller)
{
	XSettingsWindow *d = m_dialogs.value(controller).data();
	if (!d)
		return;
	d->update(settings);
}

void XSettingsLayerImpl::close(QObject *controller)
{
	XSettingsWindow *d = m_dialogs.value(controller).data();
	if (d) {
		d->deleteLater();
		m_dialogs.remove(d);
	}
}

}

