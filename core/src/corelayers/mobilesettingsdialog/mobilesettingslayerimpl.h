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

#ifndef MOBILESETTINGSLAYERIMPL_H
#define MOBILESETTINGSLAYERIMPL_H

#include <qutim/settingslayer.h>

namespace Core
{

class MobileSettingsWindow;
using namespace qutim_sdk_0_3;

class  MobileSettingsLayerImpl : public SettingsLayer
{
	Q_OBJECT
public:
	 MobileSettingsLayerImpl();
	virtual ~ MobileSettingsLayerImpl();
	virtual void close(QObject* controller = 0);
	virtual void show(const qutim_sdk_0_3::SettingsItemList& settings, QObject* controller = 0);
	virtual void update(const qutim_sdk_0_3::SettingsItemList& settings, QObject* controller = 0);
private:
	QHash<const QObject *,QPointer< MobileSettingsWindow> > m_dialogs;
};

}
#endif // MOBILESETTINGSLAYERIMPL_H
