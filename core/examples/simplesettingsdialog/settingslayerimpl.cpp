/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright (C) 2011 Ruslan Nigmatullin euroelessar@yandex.ru
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
#include "settingslayerimpl.h"
#include "settingsdialog.h"
#include "modulemanagerimpl.h"
#include <libqutim/icon.h>

static Core::CoreModuleHelper<SettingsLayerImpl> settings_layer_static( //при помощи данной функции мы сообщаем менеджеру модулей о том, что перед ним лежит модуль
		QT_TRANSLATE_NOOP("Plugin", "Simple Settings dialog"), //описание, макрос QT_TRANSLATE_NOOP позволяет делать его переводимым на разные языки
		QT_TRANSLATE_NOOP("Plugin", "SDK03 example")
		);

void SettingsLayerImpl::show (const SettingsItemList& settings )
{
	if (m_dialog.isNull())
		m_dialog =  new SettingsDialog(settings); //создаем диалог, которому передаем наш список настроек
	m_dialog->show();
}


void SettingsLayerImpl::close()
{
	m_dialog->deleteLater();
}


SettingsLayerImpl::SettingsLayerImpl()
{
  
}


SettingsLayerImpl::~SettingsLayerImpl()
{
  
}

