/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Evgeniy Degtyarev <degtep@gmail.com>
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

#include "settingswrapper.h"
#include "quicksettingslayer.h"
#include <qdeclarative.h>
#include <qutim/debug.h>

namespace MeegoIntegration
{

using namespace qutim_sdk_0_3;

Q_GLOBAL_STATIC(QList<SettingsWrapper*>, m_managers)
QuickSettingsLayer* SettingsWrapper::m_currentDialog;

SettingsWrapper::SettingsWrapper()
{
	m_managers()->append(this);
}

SettingsWrapper::~SettingsWrapper()
{
	m_managers()->removeOne(this);
}

void SettingsWrapper::init()
{
	qmlRegisterType<SettingsWrapper>("org.qutim", 0, 3, "SettingsDialog");
}

void SettingsWrapper::show(QuickSettingsLayer * dialog)
{
	m_currentDialog = dialog;
	for (int i = 0; i < m_managers()->count();i++)
	{
		emit m_managers()->at(i)->shown();
	}
}

void SettingsWrapper::open()
{
	qDebug()<<"Open settings";
	Settings::showWidget();
}

}

