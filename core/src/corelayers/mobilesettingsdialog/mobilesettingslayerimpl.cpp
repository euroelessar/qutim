/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Evgeniy Degtyarev <degtep@gmail.com>
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

#include "mobilesettingslayerimpl.h"
#include <qutim/icon.h>
#include <qutim/systemintegration.h>
#include "mobilesettingswindow.h"
#include <QApplication>

namespace Core
{
MobileSettingsLayerImpl::MobileSettingsLayerImpl()
{

}


MobileSettingsLayerImpl::~MobileSettingsLayerImpl()
{

}


void MobileSettingsLayerImpl::show (const SettingsItemList& settings, QObject* controller )
{
	MobileSettingsWindow *d = m_dialogs.value(controller).data();
	if (!d) {
		d = new MobileSettingsWindow(settings,controller);
		m_dialogs[controller] = d;
	}
	d->setParent(QApplication::activeWindow());
#ifdef Q_WS_MAEMO_5
	d->setAttribute(Qt::WA_Maemo5StackedWindow);
#endif
	d->setWindowFlags(d->windowFlags() | Qt::Window);
	SystemIntegration::show(d);
}

void MobileSettingsLayerImpl::update (const SettingsItemList& settings, QObject* controller )
{
	MobileSettingsWindow *d = m_dialogs.value(controller).data();
	if (!d)
		return;
	d->update(settings);
}

void MobileSettingsLayerImpl::close(QObject *controller)
{
	MobileSettingsWindow *d = m_dialogs.value(controller).data();
	if (d) {
		d->deleteLater();
		m_dialogs.remove(d);
	}
}

}

