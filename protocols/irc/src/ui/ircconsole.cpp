/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright (C) 2011 Prokhin Alexey <alexey.prokhin@yandex.ru>
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

#include "ircconsole.h"
#include "ui_console.h"
#include "../ircaccount.h"

namespace qutim_sdk_0_3 {

namespace irc {

IrcConsoleFrom::IrcConsoleFrom(IrcAccount *account, const QString &log, QWidget *parent) :
    QWidget(parent),
	ui(new Ui::IrcConsoleForm),
	m_account(account)
{
    ui->setupUi(this);
	ui->consoleWidget->setHtml(log);
	connect(ui->cmdEdit, SIGNAL(returnPressed()), SLOT(sendCommand()));
	connect(account, SIGNAL(destroyed()), SLOT(deleteLater()));
}

IrcConsoleFrom::~IrcConsoleFrom()
{
    delete ui;
}

void IrcConsoleFrom::appendMessage(const QString &msg)
{
	ui->consoleWidget->append(msg);
}

void IrcConsoleFrom::sendCommand()
{
	QString cmd = ui->cmdEdit->text();
	m_account->log(cmd);
	m_account->send(cmd.startsWith('/') ? cmd.mid(1) : cmd, true, IrcCommandAlias::Console);
	ui->cmdEdit->clear();
}

void IrcConsoleFrom::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

} } // namespace qutim_sdk_0_3::irc

