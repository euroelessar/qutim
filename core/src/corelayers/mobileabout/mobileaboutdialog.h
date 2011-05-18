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

#ifndef SIMPLEABOUTDIALOG_H
#define SIMPLEABOUTDIALOG_H

#include <QDialog>

namespace Ui {
    class MobileAboutDialog;
}

namespace Core
{
class MobileAboutDialog : public QDialog
{
    Q_OBJECT

public:
    explicit MobileAboutDialog(QWidget *parent = 0);
    ~MobileAboutDialog();

private:
    Ui::MobileAboutDialog *ui;
};
}

#endif // SIMPLEABOUTDIALOG_H
