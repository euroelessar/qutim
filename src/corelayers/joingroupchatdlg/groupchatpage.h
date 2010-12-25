/****************************************************************************
 *  groupchatpage.h
 *
 *  Copyright (c) 2010 by Sidorov Aleksey <sauron@citadelspb.com>
 *
 ***************************************************************************
 *                                                                         *
 *   This library is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
*****************************************************************************/

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
