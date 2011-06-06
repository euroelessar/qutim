/****************************************************************************
**
** qutIM instant messenger
**
** Copyright (C) 2011 Ruslan Nigmatullin <euroelessar@ya.ru>
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

#include "scriptsettingswidget.h"

namespace qutim_sdk_0_3
{
ScriptSettingsWidget::ScriptSettingsWidget()
{
}

QScriptValue ScriptSettingsWidget::loadFunction() const
{
	return m_load;
}

void ScriptSettingsWidget::setLoadFunction(const QScriptValue &func)
{
	m_load = func;
}

QScriptValue ScriptSettingsWidget::saveFunction() const
{
	return m_save;
}

void ScriptSettingsWidget::setSaveFunction(const QScriptValue &func)
{
	m_save = func;
}

void ScriptSettingsWidget::loadImpl()
{
	m_load.call(m_this);
}

void ScriptSettingsWidget::saveImpl()
{
	m_save.call(m_this);
}

void ScriptSettingsWidget::cancelImpl()
{
	loadImpl();
}

void ScriptSettingsWidget::listenChildrenStates()
{
	SettingsWidget::listenChildrenStates();
}
}
