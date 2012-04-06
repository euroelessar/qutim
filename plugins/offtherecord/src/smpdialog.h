/*

    Copyright (c) 2010 by Stanislav (proDOOMman) Kosolapov <prodoomman@shell.tor.hu>

 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
*/

#ifndef SMPDIALOG_H
#define SMPDIALOG_H

#include "ui_smpdialog.h"
#include <qutim/protocol.h>
#include "otrmessaging.h"

using namespace OtrSupport;

class SMPdialog : public QDialog, private Ui::SMPdialog {
    Q_OBJECT
public:
    SMPdialog(TreeModelItem &item, OtrMessaging *otr, QWidget *parent = 0);
public slots:
    void accept();
private:
    TreeModelItem &m_item;
    OtrMessaging* m_otr;
};

#endif // SMPDIALOG_H
