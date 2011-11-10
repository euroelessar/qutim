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

#include "OverlayIcon.h"
#include "../../apilayer/src/ApiOverlayIcon.h"
#include <QPixmap>
#include <QIcon>
#include <QWidget>

OverlayIcon::OverlayIcon(QWidget *w, QObject *o)
	: QObject(o)
{
	changeWindow(w);
}

void OverlayIcon::changeWindow(QWidget *w)
{
	m_window = w;
}

void OverlayIcon::set(const QIcon &icon)
{
	set(icon.pixmap(16, 16));
}

void OverlayIcon::set(const QPixmap &icon)
{
	if (!m_window)
		return;
	HICON hIcon = icon.scaled(16, 16).toWinHICON();
	SetOverlayIcon(m_window->winId(), hIcon, 0);
	DestroyIcon(hIcon);
}

void OverlayIcon::set(const QString &str)
{
	set(QPixmap(str));
}

void OverlayIcon::set(QWidget *w, const QIcon &i)
{
	OverlayIcon oi(w);
	oi.set(i);
}

void OverlayIcon::set(QWidget *w, const QPixmap &p)
{
	OverlayIcon oi(w);
	oi.set(p);
}

void OverlayIcon::set(QWidget *w, const QString &s)
{
	OverlayIcon oi(w);
	oi.set(s);
}

void OverlayIcon::clear()
{
	if (m_window)
		ClearOverlayIcon(m_window->winId());
}

void OverlayIcon::clear(QWidget *w)
{
	if (w)
		ClearOverlayIcon(w->winId());
}

