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

#ifndef XDGICONENGINE_P_H
#define XDGICONENGINE_P_H

#include <QtGui/QIconEngineV2>
#include "xdgicontheme_p.h"

class XdgIconEngine : public QIconEngineV2
{
public:
    XdgIconEngine(XdgIconData *data);
    virtual ~XdgIconEngine();

    virtual void paint(QPainter *painter, const QRect &rect, QIcon::Mode mode, QIcon::State state);
    virtual QSize actualSize(const QSize &size, QIcon::Mode mode, QIcon::State state);
    virtual QPixmap pixmap(const QSize &size, QIcon::Mode mode, QIcon::State state);

    virtual void addPixmap(const QPixmap &pixmap, QIcon::Mode mode, QIcon::State state);
    virtual void addFile(const QString &fileName, const QSize &size, QIcon::Mode mode, QIcon::State state);

    virtual QString key() const;
    virtual QIconEngineV2 *clone() const;
    virtual bool read(QDataStream &in);
    virtual bool write(QDataStream &out) const;
    virtual void virtual_hook(int id, void *data);
protected:
    XdgIconData *m_data;
};

#endif // XDGICONENGINE_P_H
