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

#include "iconbackend_p.h"

IconBackend::IconBackend(const QString &name) : m_name(name)
{
	m_theme_icon = QIcon::fromTheme(name);
}

void IconBackend::paint(QPainter *painter, const QRect &rect, QIcon::Mode mode, QIcon::State state)
{
	QIconEngine *engine = currentIconEngine();
	engine->paint(painter, rect, mode, state);
}

QSize IconBackend::actualSize(const QSize &size, QIcon::Mode mode, QIcon::State state)
{
	QIconEngine *engine = currentIconEngine();
	return engine->actualSize(size, mode, state);
}

QPixmap IconBackend::pixmap(const QSize &size, QIcon::Mode mode, QIcon::State state)
{
	QIconEngine *engine = currentIconEngine();
	return engine->pixmap(size, mode, state);
}

void IconBackend::addPixmap(const QPixmap &, QIcon::Mode, QIcon::State)
{
}

void IconBackend::addFile(const QString &, const QSize &, QIcon::Mode, QIcon::State)
{
}

QString IconBackend::key() const
{
	return QLatin1String("qutIM");
}

QIconEngine *IconBackend::clone() const
{
	IconBackend *icon = new IconBackend;
	icon->m_name = m_name;
	icon->m_theme_icon = m_theme_icon;
	return icon;
}

bool IconBackend::read(QDataStream &in)
{
	in >> m_name;
	return true;
}

bool IconBackend::write(QDataStream &out) const
{
	out << m_name;
	return true;
}

void IconBackend::virtual_hook(int id, void *data)
{
	QIconEngine *engine = currentIconEngine();
	return engine->virtual_hook(id, data);
}

