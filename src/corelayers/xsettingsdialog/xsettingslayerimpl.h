/****************************************************************************
 *  xsettingslayerimpl.h
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

#ifndef XSETTINGSLAYERIMPL_H
#define XSETTINGSLAYERIMPL_H

#include "libqutim/settingslayer.h"

class XSettingsDialog;
using namespace qutim_sdk_0_3;

class XSettingsLayerImpl : public SettingsLayer
{
	Q_OBJECT
public:
	XSettingsLayerImpl();
	virtual ~XSettingsLayerImpl();
	virtual void close(QObject* controller = 0);
	virtual void show(const qutim_sdk_0_3::SettingsItemList& settings, QObject* controller = 0);
	virtual void update(const qutim_sdk_0_3::SettingsItemList& settings, QObject* controller = 0);
private:
	QHash<const QObject *,QPointer<XSettingsDialog> > m_dialogs;
};

#endif // XSETTINGSLAYERIMPL_H
