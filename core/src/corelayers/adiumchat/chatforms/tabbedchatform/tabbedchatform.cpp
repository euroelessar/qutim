/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
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
#include "tabbedchatform.h"
#include "tabbedchatwidget.h"
#include <qutim/settingslayer.h>
#include "settings/tabbedchatbehavior.h"
#include <qutim/icon.h>
#include <qutim/shortcut.h>

namespace Core
{
namespace AdiumChat
{

using namespace qutim_sdk_0_3;

TabbedChatForm::TabbedChatForm() :
	AbstractChatForm(),
	m_settingsItem(0)
{
	m_settingsItem = new GeneralSettingsItem<TabbedChatBehavior>(Settings::General, Icon("view-choose"),
															  QT_TRANSLATE_NOOP("Settings","Chat"));
	m_settingsItem->connect(SIGNAL(saved()),this,SLOT(onSettingsChanged()));
	Settings::registerItem(m_settingsItem);
	Shortcut::registerSequence("chatListSession",
							   QT_TRANSLATE_NOOP("ChatLayer", "Open session list"),
							   "ChatWidget",
							   QKeySequence("Alt+S")
							   );
}

TabbedChatForm::~TabbedChatForm()
{
	Settings::removeItem(m_settingsItem);
	delete m_settingsItem;
}

AbstractChatWidget *TabbedChatForm::createWidget(const QString &key)
{
	return new TabbedChatWidget(key);
}

}
}

