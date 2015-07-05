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

#ifndef XSETTINGSLAYERIMPL_H
#define XSETTINGSLAYERIMPL_H

#include <qutim/settingslayer.h>

namespace Core
{

class XSettingsWindow;
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
	QHash<const QObject *,QPointer<XSettingsWindow> > m_dialogs;
};

}
#endif // XSETTINGSLAYERIMPL_H

