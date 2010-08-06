/****************************************************************************
 *  xsettingslayerimpl.cpp
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

#include "xsettingslayerimpl.h"
#include "xsettingsdialog.h"
#include "src/modulemanagerimpl.h"
#include <libqutim/icon.h>

static Core::CoreModuleHelper<XSettingsLayerImpl> xsettings_layer_static(
		QT_TRANSLATE_NOOP("Plugin", "X Settings dialog"),
		QT_TRANSLATE_NOOP("Plugin", "Default qutIM settings dialog implementation with OS X style top bar")
		);

XSettingsLayerImpl::XSettingsLayerImpl()
{

}


XSettingsLayerImpl::~XSettingsLayerImpl()
{

}


void XSettingsLayerImpl::show (const SettingsItemList& settings, const QObject* controller )
{
	XSettingsDialog *d = m_dialogs.value(controller);
	if (!d) {
		d = new XSettingsDialog(settings);
		m_dialogs[controller] = d;
	}
	d->show();
}

void XSettingsLayerImpl::update (const SettingsItemList& settings, const QObject* controller )
{
	XSettingsDialog *d = m_dialogs.value(controller);
	if (!d)
		return;
	d->update(settings);
}

void XSettingsLayerImpl::close(const QObject *controller)
{
	XSettingsDialog *d = m_dialogs.value(controller);
	if (d) {
		d->deleteLater();
		m_dialogs.remove(d);
	}
}
