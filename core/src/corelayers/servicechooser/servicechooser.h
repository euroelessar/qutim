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

#ifndef SERVICECHOOSER_H
#define SERVICECHOOSER_H

#include <QObject>
#include <qutim/startupmodule.h>

namespace qutim_sdk_0_3
{
class ExtensionInfo;
}

namespace Core
{
using namespace qutim_sdk_0_3;

class ServiceChooser : public QObject, public qutim_sdk_0_3::StartupModule
{
	Q_OBJECT
	Q_INTERFACES(qutim_sdk_0_3::StartupModule)
public:
	ServiceChooser(QObject* parent = 0);
	static const char *className(const ExtensionInfo &info);
	static QString html(const ExtensionInfo &info);
};

}
#endif // SERVICECHOOSER_H

