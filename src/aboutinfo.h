/*
    aboutInfo

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

#ifndef ABOUTINFO_H
#define ABOUTINFO_H

#include <QtGui/QWidget>
#include "ui_aboutinfo.h"

class aboutInfo : public QWidget
{
    Q_OBJECT

public:
    aboutInfo(QWidget *parent = 0);
    ~aboutInfo();

private:
    Ui::aboutInfoClass ui;
};

#endif // ABOUTINFO_H
