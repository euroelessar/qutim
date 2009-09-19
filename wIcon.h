/*
    W7 integration plugin

    Copyright (c) 2009 by Belov Nikita <null@deltaz.ru>

 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
*/

#ifndef W7IICON_H
#define W7IICON_H

#include <QIcon>
#include <QBitmap>

#include "pluginLayer.h"

class w7iicon
{
public:
	w7iicon( QString qutimIcon );
	~w7iicon();

	HICON getIcon ( int xSize, int ySize );

private:
	QIcon icon;

	HBITMAP createIconMask( const QBitmap &bitmap );
};
#endif
