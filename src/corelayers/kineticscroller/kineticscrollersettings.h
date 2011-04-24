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

#ifndef KINETICSCROLLERSETTINGS_H
#define KINETICSCROLLERSETTINGS_H

#include <QWidget>

namespace Ui {
    class KineticScrollerSettings;
}

class KineticScrollerSettings : public QWidget
{
    Q_OBJECT

public:
    explicit KineticScrollerSettings(QWidget *parent = 0);
    ~KineticScrollerSettings();

private:
    Ui::KineticScrollerSettings *ui;
};

#endif // KINETICSCROLLERSETTINGS_H
