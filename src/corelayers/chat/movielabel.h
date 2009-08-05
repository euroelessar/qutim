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


#ifndef MOVIELABEL_H_
#define MOVIELABEL_H_

#include <QtGui>

class movieLabel : public QLabel
{
	Q_OBJECT
public:
	movieLabel(QWidget *parent = 0);
	~movieLabel();
signals:
	void sendMovieTip(const QString &);
protected:
	void mousePressEvent ( QMouseEvent * event ); 
};

#endif /*MOVIELABEL_H_*/
