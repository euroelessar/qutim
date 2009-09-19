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

#include "wIcon.h"

w7iicon::w7iicon( QString qutimIcon )
{
	icon = SystemsCity::PluginSystem()->getIcon( qutimIcon );
}

w7iicon::~w7iicon()
{
}

HICON w7iicon::getIcon( int xSize, int ySize )
{
	HICON result = 0;

	if ( !icon.isNull() )
	{
		QSize size = icon.actualSize( QSize( xSize, ySize ) );
		QPixmap pm = icon.pixmap( size );
		if ( pm.isNull() )
			return 0;

		QBitmap mask = pm.mask();
		if ( mask.isNull() )
		{
			mask = QBitmap( pm.size() );
			mask.fill( Qt::color1 );
		}

		HBITMAP im = createIconMask( mask );
		ICONINFO ii;
		ii.fIcon = true;
		ii.hbmMask = im;
		ii.hbmColor = pm.toWinHBITMAP( QPixmap::Alpha );
		ii.xHotspot = 0;
		ii.yHotspot = 0;
		result = CreateIconIndirect( &ii );

		DeleteObject( ii.hbmColor );
		DeleteObject( im );
	}

	return result;
}

HBITMAP w7iicon::createIconMask( const QBitmap &bitmap )
{
	QImage bm = bitmap.toImage().convertToFormat( QImage::Format_Mono );
	int w = bm.width();
	int h = bm.height();
	int bpl = ( ( w+15 )/16 )*2;
	uchar *bits = new uchar[ bpl*h ];
	bm.invertPixels();
	for ( int y=0; y<h; y++ )
		memcpy( bits+y*bpl, bm.scanLine( y ), bpl );

	HBITMAP hbm = CreateBitmap( w, h, 1, 1, bits );
	delete [] bits;

	return hbm;
}
