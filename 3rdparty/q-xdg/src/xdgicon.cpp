/*
    Copyright (C) 2009 Nigmatullin Ruslan <euroelessar@ya.ru>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "xdgicon.h"
#include "xdgiconengine_p.h"
#include "xdgicontheme_p.h"

XdgIcon::XdgIcon(const QString &name, const XdgIconTheme *theme)
        : QIcon(new XdgIconEngine(theme->data()->findIcon(name)))
{
}

XdgIcon::XdgIcon(const QIcon &other) : QIcon(other)
{
}

XdgIcon::XdgIcon()
{
}

XdgIcon::~XdgIcon()
{
}

XdgIcon &XdgIcon::operator =(const XdgIcon &other)
{
    if (this != &other) {
        QIcon::operator =(other);
    }
    return *this;
}
