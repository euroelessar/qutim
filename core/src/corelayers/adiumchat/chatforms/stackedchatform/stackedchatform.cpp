/****************************************************************************
**
** qutIM instant messenger
**
** Copyright (c) 2011 by Sidorov Aleksey <sauron@citadelspb.com>
** Copyright (C) 2011 Evgeniy Degtyarev <degtep@gmail.com>
**
*****************************************************************************
**
** $QUTIM_BEGIN_LICENSE$
** This program is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 2 of the License, or
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

namespace Core
{
namespace AdiumChat
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
	Shortcut::registerSequence("chatListSession",
							   QT_TRANSLATE_NOOP("ChatLayer", "Open session list"),
							   "ChatWidget",
							   QKeySequence("Alt+S")
							   );
}

StackedChatForm::~StackedChatForm()
{
	Settings::removeItem(m_settingsItem);
	delete m_settingsItem;
}

AbstractChatWidget *StackedChatForm::createWidget(const QString &key)
{
	return new StackedChatWidget(key);
}

}
}
