/*
    movieLabel

    Copyright (c) 2008 by Rustam Chakin <qutim.develop@gmail.com>

 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
*/


#include "movielabel.h"



movieLabel::movieLabel(QWidget *parent) : QLabel(parent)
{
	
}

movieLabel::~movieLabel()
{
	
}

void movieLabel::mousePressEvent( QMouseEvent * event )
{
	emit sendMovieTip(toolTip());
	QLabel::mousePressEvent(event);
}
