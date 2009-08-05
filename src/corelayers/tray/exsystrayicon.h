/*****************************************************************************
**                                                                          **
**    Copyright (C) 2008 Denisss (Denis Novikov). All rights reserved.      **
**                                                                          **
**    This file contains declarations of class  ExSysTray, a subclass       **
**    of QSystemTrayIcon                                                    **
**                                                                          **
**    This  file may be used  under the terms of the GNU General  Public    **
**    License  version 3.0 as published by the Free Software Foundation.    **
**    Alternatively  you may (at your option) use any  later  version of    **
**    the GNU General Public License if such license has  been  publicly    **
**    approved by Trolltech ASA (or its  successors, if any) and the KDE    **
**    Free Qt Foundation.                                                   **
**                                                                          **
**        This file is provided AS IS with NO  WARRANTY OF ANY KIND,        **
**          INCLUDING THE  WARRANTY OF DESIGN, MERCHANTABILITY AND          **
**                   FITNESS  FOR A PARTICULAR PURPOSE.                     **
**                                                                          **
*****************************************************************************/

#ifndef EXSYSTRAYICON_H
#define EXSYSTRAYICON_H

#include <QSystemTrayIcon>
#include <QApplication>
#include <QDesktopWidget>

class QTimerEvent;

class ExSysTrayIcon : public QSystemTrayIcon
{
Q_OBJECT
public:
	ExSysTrayIcon(const QIcon &icon, QObject *parent = 0);
	ExSysTrayIcon(QObject *parent = 0);
	void setToolTip (const QString &tip);
protected:
	void timerEvent(QTimerEvent *event);
	void init();
private:
	QString toolTipText;
	QDesktopWidget &desktop;
	bool tipShown;
};

#endif // EXSYSTRAYICON_H
