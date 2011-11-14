/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright (C) 2011 Prokhin Alexey <alexey.prokhin@yandex.ru>
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

#ifndef ICQMAINSETTINGS_H
#define ICQMAINSETTINGS_H

#include "../icq_global.h"
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

