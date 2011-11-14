/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright (C) 2011 Sidorov Aleksey <sauron@citadelspb.com>
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

#ifndef GROUPCHATPAGE_H
#define GROUPCHATPAGE_H

#include <QScrollArea>

namespace qutim_sdk_0_3
{
class Account;
};

class QVBoxLayout;
namespace Core {

class GroupChatPage : public QScrollArea
{
    Q_OBJECT
public:
    explicit GroupChatPage(QWidget *parent = 0);
	virtual void setAccount(qutim_sdk_0_3::Account *account) {m_account = account;}
	qutim_sdk_0_3::Account *account() const {return m_account;}
protected:
	QVBoxLayout *m_layout;
private:
	qutim_sdk_0_3::Account *m_account;

};

} // namespace Core

#endif // GROUPCHATPAGE_H

