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

#include "smpdialog.h"
#include <QMessageBox>

SMPdialog::SMPdialog(TreeModelItem &item, OtrMessaging *otr, QWidget *parent) :
    QDialog(parent),
    m_item(item),
    m_otr(otr)
{
    setupUi(this);
    OtrSupport::Fingerprint fingerprint;
    bool found = false;

    foreach(fingerprint, m_otr->getFingerprints())
    {
        if (fingerprint.username == item.m_item_name
            && fingerprint.account == item.m_account_name )
        {
            found = true;
            break;
        }
    }

    if(!found)
    {
        QMessageBox::information(this,tr("Auth canceled"),tr("Could not find any fingerprints for this user."),QMessageBox::Ok,QMessageBox::Ok);
        this->close();
    }

    infoLabel->setTextFormat(Qt::RichText);
    infoLabel->setText(tr("Protocol %0.<br>Authorisation of user <b>%1</b> <br>with fingerprint <b>%2</b>").arg(item.m_protocol_name).arg(item.m_item_name).arg(fingerprint.fingerprintHuman));
    connect(this,SIGNAL(accepted()),this,SLOT(accept()));
    connect(this,SIGNAL(rejected()),this,SLOT(close()));
}

void SMPdialog::accept()
{
    switch(stackedWidget->currentIndex())
    {
    case 0:
        m_otr->requestAuth(m_item,false,lineEdit_2->text(),lineEdit->text());
        break;
    case 1:
        m_otr->requestAuth(m_item,false,lineEdit_3->text());
        break;
    case 2:
        if(radioButton->isChecked())
            m_otr->requestAuth(m_item,true);
        break;
    }
    this->close();
}
