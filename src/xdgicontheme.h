/*
    Copyright (C) 2009 Maia Kozheva <sikon@ubuntu.com>

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

#ifndef XDGICONTHEME_H
#define XDGICONTHEME_H

#include <QtCore/QDir>
#include <QtCore/QMap>
#include <QtCore/QList>
#include <QtCore/QString>
#include <QtCore/QVector>
#include "xdgexport.h"
#include "xdgicon.h"

class XdgIconThemePrivate;

class XDG_API XdgIconTheme
{
    Q_DECLARE_PRIVATE_D(p, XdgIconTheme)
    Q_DISABLE_COPY(XdgIconTheme)
public:
    XdgIconTheme(const QVector<QDir> &basedirs, const QString &id, const QString &indexFileName);
    virtual ~XdgIconTheme();

    QString id() const;
    QString name() const;
    QStringList parentNames() const;

    void addParent(const XdgIconTheme *parent);
    QString getIconPath(const QString &name, uint size = 22) const;

    inline QIcon getIcon(const QString &name) const
    { return XdgIcon(name, this); }
    inline QPixmap getPixmap(const QString &name, int size) const
    { return getIcon(name).pixmap(size); }
    inline QPixmap getPixmap(const QString &name, QSize size) const
    { return getIcon(name).pixmap(size); }

protected:
    XdgIconThemePrivate *p;
public:
    typedef XdgIconThemePrivate * Data;
    const Data &data() const { return p; }
};

#endif // XDGICONTHEME_H
