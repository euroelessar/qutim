/****************************************************************************
 *  MobileSettingsLayerImpl.cpp
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

#include "mobilesettingslayerimpl.h"
#include <qutim/icon.h>
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
	MobileSettingsWindow *d = m_dialogs.value(controller);
	if (!d) {
		d = new MobileSettingsWindow(settings,controller);
		m_dialogs[controller] = d;
	}
	d->setParent(QApplication::activeWindow());
	d->setAttribute(Qt::WA_Maemo5StackedWindow);
	d->setWindowFlags(d->windowFlags() | Qt::Window);
	d->showMaximized();
}

void MobileSettingsLayerImpl::update (const SettingsItemList& settings, QObject* controller )
{
	MobileSettingsWindow *d = m_dialogs.value(controller);
	if (!d)
		return;
	d->update(settings);
}

void MobileSettingsLayerImpl::close(QObject *controller)
{
	MobileSettingsWindow *d = m_dialogs.value(controller);
	if (d) {
		d->deleteLater();
		m_dialogs.remove(d);
	}
}

}
