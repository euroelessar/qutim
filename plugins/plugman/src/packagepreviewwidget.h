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

#ifndef PACKAGEPREVIEWWIDGET_H
#define PACKAGEPREVIEWWIDGET_H

#include <QWidget>
#include <QPixmap>

class PackagePreviewWidget : public QWidget
{
public:
    PackagePreviewWidget(QWidget *parent = 0);
	
	QSize minimumSizeHint() const;
	QSize sizeHint() const;
	void setPreview(const QPixmap &preview);
	void paintEvent(QPaintEvent *ev);
	
private:
	QPixmap m_preview;
};

#endif // PACKAGEPREVIEWWIDGET_H
