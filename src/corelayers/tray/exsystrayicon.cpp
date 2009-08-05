/*****************************************************************************
**                                                                          **
**    Copyright (C) 2008 Denisss (Denis Novikov). All rights reserved.      **
**                                                                          **
**    This file contains  implementation of class ExSysTray, a subclass     **
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

#include <QTimerEvent>
#include <QToolTip>
#include <QCursor>

#include "exsystrayicon.h"

#ifdef _MSC_VER
#pragma warning (disable:4100)		// Unref local parameter
#endif

ExSysTrayIcon::ExSysTrayIcon(const QIcon &icon, QObject *parent)
:QSystemTrayIcon(icon, parent), desktop(*QApplication::desktop())
{
	init();
}

ExSysTrayIcon::ExSysTrayIcon(QObject *parent)
:QSystemTrayIcon(parent), desktop(*QApplication::desktop())
{
	init();
}

void ExSysTrayIcon::init()
{
	tipShown = false;
	startTimer(400);
}

void ExSysTrayIcon::timerEvent(QTimerEvent *event)
{
	if (!geometry().contains(QCursor::pos()))
	{
		if (tipShown) QToolTip::hideText();
		tipShown = false;
		return;
	}

	if (tipShown || toolTipText.isEmpty()) return;

	QToolTip::showText(QCursor::pos(), toolTipText);
	tipShown = true;
}

void ExSysTrayIcon::setToolTip(const QString &tip)
{
	toolTipText = tip;
	return;
}
