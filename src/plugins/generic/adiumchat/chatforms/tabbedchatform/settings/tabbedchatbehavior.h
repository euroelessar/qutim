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
#ifndef TABBEDCHATBEHAVIOR_H
#define TABBEDCHATBEHAVIOR_H

#include <QWidget>
#include <qutim/settingswidget.h>
#include <qutim/adiumchat/chatedit.h>
#include "../tabbedchatwidget.h"

namespace Ui {
class TabbedChatBehavior;
}
class QButtonGroup;
namespace Core
{
namespace AdiumChat
{

using namespace qutim_sdk_0_3;
class TabbedChatBehavior : public SettingsWidget
{
	Q_OBJECT

public:
	explicit TabbedChatBehavior();
	virtual ~TabbedChatBehavior();
	virtual void cancelImpl();
	virtual void loadImpl();
	virtual void saveImpl();
protected:
	void changeEvent(QEvent *e);
private slots:
	void onButtonClicked(int id);
private:
	void setFlags(AdiumChat::ChatFlags,bool set = true);
	Ui::TabbedChatBehavior *ui;
	AdiumChat::SendMessageKey m_send_message_key;
	bool m_autoresize;
	QButtonGroup *m_group;
	AdiumChat::ChatFlags m_flags;
};

}
}
#endif // TABBEDCHATBEHAVIOR_H

