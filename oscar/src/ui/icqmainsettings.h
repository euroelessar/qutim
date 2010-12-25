/****************************************************************************
 *  icqmainsettings.h
 *
 *  Copyright (c) 2010 by Prokhin Alexey <alexey.prokhin@yandex.ru>
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

#ifndef ICQMAINSETTINGS_H
#define ICQMAINSETTINGS_H

#include <icq_global.h>
#include <qutim/settingswidget.h>
#include <qutim/config.h>
#include <qutim/dataforms.h>

class QVBoxLayout;

namespace qutim_sdk_0_3 {

namespace oscar {

class IcqMainSettings: public SettingsWidget
{
	Q_OBJECT
public:
	IcqMainSettings();
	virtual ~IcqMainSettings();
	virtual void loadImpl();
	virtual void cancelImpl();
	virtual void saveImpl();
private slots:
	void extSettingsChanged();
private:
	QScopedPointer<AbstractDataForm> m_extSettings;
	QVBoxLayout *m_layout;
};

} } // namespace qutim_sdk_0_3::oscar

#endif // ICQMAINSETTINGS_H
