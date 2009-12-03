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
#include "xdgenvironmentmap.h"

#ifdef QT_GUI_LIB
#include <QtGui/QPixmap>
#endif

class XdgIconThemePrivate;

class XdgIconTheme
{
    Q_DECLARE_PRIVATE_D(p, XdgIconTheme)
    Q_DISABLE_COPY(XdgIconTheme)
public:
    enum DefaultTheme
    {
        DefaultThemeAny = 0,
        DefaultThemeGNOME = 1,
        DefaultThemeKDE = 2,
    };

    XdgIconTheme(const QVector<QDir> &basedirs, const QString &id, const QString &indexFileName);
    virtual ~XdgIconTheme();

    QString id() const;
    QString name() const;
    QStringList parentNames() const;

    void addParent(const XdgIconTheme *parent);
    QString getIconPath(const QString& name, uint size = 22) const;

#ifdef QT_GUI_LIB
    QPixmap getPixmap(const QString& name, int size) const
    {
        QPixmap pm(getIconPath(name));

        if(pm.size() != QSize(size, size))
            pm = pm.scaled(size, size, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

        return pm;
    }
#endif
protected:
    XdgIconThemePrivate *p;
};

#endif // XDGICONTHEME_H
