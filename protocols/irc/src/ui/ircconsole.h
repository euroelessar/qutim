/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Prokhin Alexey <alexey.prokhin@yandex.ru>
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

#ifndef IRCCONSOLE_H
#define IRCCONSOLE_H

#include <QWidget>
#include <QHash>

namespace Ui {
	class IrcConsoleForm;
}

namespace qutim_sdk_0_3 {

namespace irc {

class IrcAccount;

class IrcConsoleFrom : public QWidget {
    Q_OBJECT
public:
	IrcConsoleFrom(IrcAccount *account, const QString &log, QWidget *parent = 0);
	~IrcConsoleFrom();
	void appendMessage(const QString &msg);
private slots:
	void sendCommand();
protected:
    void changeEvent(QEvent *e);
private:
	Ui::IrcConsoleForm *ui;
	QHash<QString, QString> m_colors;
	IrcAccount *m_account;
};

} } // namespace qutim_sdk_0_3::irc

#endif // IRCCONSOLE_H

