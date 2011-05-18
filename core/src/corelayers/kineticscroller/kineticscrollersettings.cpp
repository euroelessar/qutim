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

#include "kineticscrollersettings.h"
#include "ui_kineticscrollersettings.h"

KineticScrollerSettings::KineticScrollerSettings(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::KineticScrollerSettings)
{
    ui->setupUi(this);
}

KineticScrollerSettings::~KineticScrollerSettings()
{
    delete ui;
}
