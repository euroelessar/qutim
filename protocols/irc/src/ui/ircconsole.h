/****************************************************************************
 *  ircconsole.h
 *
 *  Copyright (c) 2010 by Prokhin Alexey <alexey.prokhin@yandex.ru>
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
