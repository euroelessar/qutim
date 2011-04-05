/****************************************************************************
 *
 *  This file is part of qutIM
 *
 *  Copyright (c) 2011 by Nigmatullin Ruslan <euroelessar@gmail.com>
 *
 ***************************************************************************
 *                                                                         *
 *   This file is part of free software; you can redistribute it and/or    *
 *   modify it under the terms of the GNU General Public License as        *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************
 ****************************************************************************/

#ifndef PACKAGEDELEGATE_H
#define PACKAGEDELEGATE_H

#include <QAbstractItemDelegate>

class PackageDelegate : public QAbstractItemDelegate
{
    Q_OBJECT
public:
    explicit PackageDelegate(QObject *parent = 0);
	
    virtual void paint(QPainter *painter,  const QStyleOptionViewItem &option,
                       const QModelIndex &index) const;

    virtual QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;
};

#endif // PACKAGEDELEGATE_H
