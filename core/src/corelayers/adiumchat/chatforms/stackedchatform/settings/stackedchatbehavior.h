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

#ifndef STACKEDCHATBEHAVIOR_H
#define STACKEDCHATBEHAVIOR_H

#include <QWidget>
#include <qutim/settingswidget.h>
#include <chatlayer/chatedit.h>
#include "../stackedchatwidget.h"

namespace Ui {
class StackedChatBehavior;
}
class QButtonGroup;
namespace Core
{
namespace AdiumChat
{

using namespace qutim_sdk_0_3;
class StackedChatBehavior : public SettingsWidget
{
	Q_OBJECT

public:
	explicit StackedChatBehavior();
	virtual ~StackedChatBehavior();
	virtual void cancelImpl();
	virtual void loadImpl();
	virtual void saveImpl();
protected:
	void changeEvent(QEvent *e);
private slots:
	void onButtonClicked(int id);
private:
	void setFlags(AdiumChat::ChatFlags,bool set = true);
	Ui::StackedChatBehavior *ui;
	AdiumChat::SendMessageKey m_send_message_key;
	QButtonGroup *m_group;
	AdiumChat::ChatFlags m_flags;
};

}
}
#endif // STACKEDCHATBEHAVIOR_H

