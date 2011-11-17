/****************************************************************************
**
** qutIM instant messenger
**
** Copyright (C) 2011 Evgeniy Degtyarev <degtep@gmail.com>
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

#ifndef SETTINGSWRAPPER_H
#define SETTINGSWRAPPER_H

#include <QtCore/QObject>
#include <QtCore/QList>
#include "quicksettingslayer.h"

namespace MeegoIntegration
{
using namespace qutim_sdk_0_3;
class SettingsWrapper : public QObject {
	Q_OBJECT

public:
	SettingsWrapper();
	~SettingsWrapper();
	Q_INVOKABLE void open();

	static void init();
	static void show(QuickSettingsLayer * dialog);

signals:
	void shown();

private:

	static QuickSettingsLayer *m_currentDialog;

};
}

#endif /* SETTINGSWRAPPER_H */
