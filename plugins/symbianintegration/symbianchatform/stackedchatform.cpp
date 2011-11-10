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
#include "stackedchatform.h"
#include "stackedchatwidget.h"
#include <qutim/settingslayer.h>
#include "settings/stackedchatbehavior.h"
#include <qutim/icon.h>
#include <qutim/shortcut.h>
#include <qutim/systemintegration.h>

namespace Core
{
namespace AdiumChat
{
namespace Symbian
{
	
using namespace qutim_sdk_0_3;

StackedChatForm::StackedChatForm() :
	AbstractChatForm(),
	m_settingsItem(0)
{
	m_settingsItem = new GeneralSettingsItem<StackedChatBehavior>(Settings::General, Icon("view-choose"),
															  QT_TRANSLATE_NOOP("Settings","Chat"));
	m_settingsItem->connect(SIGNAL(saved()),this,SLOT(onSettingsChanged()));
	Settings::registerItem(m_settingsItem);

	SystemIntegration::show(createWidget(QString())); //nice hack
}

StackedChatForm::~StackedChatForm()
{
	Settings::removeItem(m_settingsItem);
	delete m_settingsItem;
}

AbstractChatWidget *StackedChatForm::createWidget(const QString &key)
{
	Q_UNUSED(key)
	if(!m_chatWidget)
		m_chatWidget = new StackedChatWidget();
	return m_chatWidget;
}

} // namespace Symbian
} // namespace AdiumChat
} // namespace Core

