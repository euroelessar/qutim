/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2012 Ruslan Nigmatullin <euroelessar@yandex.ru>
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

#include "adiumwebviewfactory.h"
#include "adiumwebviewcontroller.h"
#include "adiumwebviewwidget.h"
#include "adiumwebviewappearance.h"
#include <qutim/settingslayer.h>
#include <qutim/icon.h>

namespace Adium {

using namespace qutim_sdk_0_3;

WebViewFactory::WebViewFactory()
{
	m_appearanceSettings = new GeneralSettingsItem<WebViewAppearance>(
	            Settings::Appearance, Icon("view-choose"), QT_TRANSLATE_NOOP("Settings","Chat"));
	m_appearanceSettings->connect(SIGNAL(saved()), this, SIGNAL(settingsSaved()));
	Settings::registerItem(m_appearanceSettings);
}

WebViewFactory::~WebViewFactory()
{
	Settings::removeItem(m_appearanceSettings);
	delete m_appearanceSettings;
}

QObject *WebViewFactory::createViewController()
{
	WebViewController *controller = new WebViewController();
	connect(this, SIGNAL(settingsSaved()), controller, SLOT(onSettingsSaved()));
	return controller;
}

QWidget *WebViewFactory::createViewWidget()
{
	return new WebViewWidget();
}
	
} // namespace Adium

