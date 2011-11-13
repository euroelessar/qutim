/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright (C) 2011 Ivan Vizir <define-true-false@yandex.com>
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

#ifndef OVERLAYICON_H
#define OVERLAYICON_H

#include "global.h"
#include <QObject>

class QIcon;
class QWidget;
class QPixmap;

class WTQTEXPORT OverlayIcon : public QObject
{
	QWidget *m_window;

public:
	OverlayIcon(QWidget *, QObject *parent = 0);
	static void set(QWidget *, const QIcon &);
	static void set(QWidget *, const QPixmap &);
	static void set(QWidget *, const QString &path);
	static void clear(QWidget *);
	void set(const QIcon &);
	void set(const QPixmap &);
	void set(const QString &path);
	void clear();
	void changeWindow(QWidget*);
};

#endif // OVERLAYICON_H

