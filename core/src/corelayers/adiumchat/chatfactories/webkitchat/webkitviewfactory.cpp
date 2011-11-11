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
#include "webkitviewfactory.h"
#include "webkitchatviewwidget.h"
#include "chatstyleoutput.h"
#include "settings/chatappearance.h"
#include <qutim/settingslayer.h>
#include <qutim/servicemanager.h>
#include <qutim/icon.h>


namespace Core
{
namespace AdiumChat
{

WebkitViewFactory::WebkitViewFactory()
{
	m_appearanceSettings = new GeneralSettingsItem<ChatAppearance>(Settings::Appearance, Icon("view-choose"),
																   QT_TRANSLATE_NOOP("Settings","Chat"));

	Settings::registerItem(m_appearanceSettings);
}

WebkitViewFactory::~WebkitViewFactory()
{
	Settings::removeItem(m_appearanceSettings);
	delete m_appearanceSettings;
}

QWidget *WebkitViewFactory::createViewWidget()
{
	return new WebkitChatViewWidget();
}
QObject *WebkitViewFactory::createViewController()
{
	ChatStyleOutput *output = new ChatStyleOutput();
	m_appearanceSettings->connect(SIGNAL(saved()), output, SLOT(loadSettings()));
	return output;
}

}
}

