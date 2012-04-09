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

#include "ui_otrsettingswidget.h"
#include "otrsettingswidget.h"
#include "otrcrypt.h"
#include <qutim/config.h>
#include <QDebug>

using namespace qutim_sdk_0_3;

OtrSettingsWidget::OtrSettingsWidget()
    : ui(new Ui::OtrSettingsWidget)
{
	ui->setupUi(this);
	m_otr = OTRCrypt::instance()->connectionForPolicy(-1);
	ui->fingerprintsTable->setColumnWidth(0, 150);
	ui->fingerprintsTable->setColumnWidth(1, 150);
	ui->fingerprintsTable->setColumnWidth(2, 360);
	ui->fingerprintsTable->setColumnWidth(3, 80);
	ui->fingerprintsTable->setColumnWidth(4, 100);
	ui->forgetButton->setEnabled(false);
	ui->pushButton->setEnabled(false);
	connect(ui->fingerprintsTable, SIGNAL(currentCellChanged(int,int,int,int)),
	        this, SLOT(fingerprintChanged(int)));
	connect(ui->keysTable, SIGNAL(currentCellChanged(int,int,int,int)),
	        this, SLOT(keyChanged(int)));
	listenChildrenStates();
}

void OtrSettingsWidget::loadImpl()
{
	Config config;
	config.beginGroup("otr");
	ui->notify_checkbox->setChecked(config.value("notify", true));
	switch (config.value("policy", OtrSupport::PolicyAuto)) {
	case OtrSupport::PolicyOff:
		ui->polAuto->setChecked(false);
		ui->polEnable->setChecked(false);
		ui->polReq->setChecked(false);
		ui->polAuto->setEnabled(false);
		ui->polReq->setEnabled(false);
		break;
	case OtrSupport::PolicyRequire:
		ui->polAuto->setChecked(true);
		ui->polEnable->setChecked(true);
		ui->polReq->setChecked(true);
		ui->polAuto->setEnabled(true);
		ui->polReq->setEnabled(true);
		break;
	case OtrSupport::PolicyAuto:
		ui->polEnable->setChecked(true);
		ui->polReq->setChecked(false);
		ui->polAuto->setEnabled(true);
		ui->polReq->setEnabled(true);
		ui->polAuto->setChecked(true);
		break;
	case OtrSupport::PolicyEnabled:
		ui->polAuto->setChecked(false);
		ui->polEnable->setChecked(true);
		ui->polReq->setChecked(false);
		ui->polAuto->setEnabled(true);
		ui->polReq->setEnabled(false);
		break;
	}
	emit updateFingerprints();
	emit updateKeys();
}

void OtrSettingsWidget::saveImpl()
{
	Policy policy = PolicyOff;
	if(ui->polReq->isChecked() && ui->polReq->isEnabled())
		policy = PolicyRequire;
	else if(ui->polAuto->isChecked() && ui->polAuto->isEnabled())
		policy = OtrSupport::PolicyAuto;
	else if(ui->polEnable->isChecked() && ui->polEnable->isEnabled())
		policy = OtrSupport::PolicyEnabled;
	Config config;
	config.beginGroup("otr");
	config.setValue("policy", policy);
	config.setValue("notify", ui->notify_checkbox->isChecked());
	m_otr->setPolicy(policy);
}

void OtrSettingsWidget::cancelImpl()
{
	loadImpl();
}

void OtrSettingsWidget::fingerprintChanged(int row)
{
	ui->forgetButton->setEnabled(row >= 0);
}

void OtrSettingsWidget::keyChanged(int row)
{
	ui->pushButton->setEnabled(row >= 0);
}

void OtrSettingsWidget::updateKeys()
{
	ui->keysTable->setRowCount(0);
	QHash<QString, QHash<QString ,QString> > privateKeys = m_otr->getPrivateKeys();
	foreach(QString acc,privateKeys.keys())
	{
		foreach(QString prot, privateKeys.value(acc).keys())
		{
			QString fp = privateKeys.value(acc).value(prot);
			ui->keysTable->insertRow(ui->keysTable->rowCount());
			QList<QTableWidgetItem*> row;
			row.append(new QTableWidgetItem(acc));
			row.append(new QTableWidgetItem(prot));
			row.append(new QTableWidgetItem(fp));
			ui->keysTable->setItem(ui->keysTable->rowCount()-1,0,row.at(0));
			ui->keysTable->setItem(ui->keysTable->rowCount()-1,1,row.at(1));
			ui->keysTable->setItem(ui->keysTable->rowCount()-1,2,row.at(2));
		}
	}
}

void OtrSettingsWidget::updateFingerprints()
{
	ui->fingerprintsTable->setRowCount(0);
	m_fingerprints = m_otr->getFingerprints();
	QListIterator<OtrSupport::Fingerprint> fingerprintIt(m_fingerprints);
	while(fingerprintIt.hasNext())
	{
		OtrSupport::Fingerprint fp = fingerprintIt.next();
		ui->fingerprintsTable->insertRow(ui->fingerprintsTable->rowCount());
		ui->fingerprintsTable->setItem(ui->fingerprintsTable->rowCount()-1,0,new QTableWidgetItem(fp.account));
		ui->fingerprintsTable->setItem(ui->fingerprintsTable->rowCount()-1,1,new QTableWidgetItem(fp.username));
		ui->fingerprintsTable->setItem(ui->fingerprintsTable->rowCount()-1,2,new QTableWidgetItem(fp.fingerprintHuman));
		ui->fingerprintsTable->setItem(ui->fingerprintsTable->rowCount()-1,3,new QTableWidgetItem(fp.trust));
		ui->fingerprintsTable->setItem(ui->fingerprintsTable->rowCount()-1,4,new QTableWidgetItem(fp.messageState));
		ui->fingerprintsTable->item(ui->fingerprintsTable->rowCount()-1,0)->setData(Qt::UserRole,fp.fingerprintHuman);
	}
}

void OtrSettingsWidget::on_forgetButton_released()
{
	if(ui->fingerprintsTable->currentRow()<0)
		return;
	QString msg(tr("Are you sure you want to delete the fingerprint:\naccount: %1\nbuddy: %2\nfingerprint: %3")
	            .arg(m_fingerprints[ui->fingerprintsTable->currentRow()].account)
	            .arg(m_fingerprints[ui->fingerprintsTable->currentRow()].username)
	            .arg(m_fingerprints[ui->fingerprintsTable->currentRow()].fingerprintHuman));
	
	QMessageBox mb(QMessageBox::Question, tr("qutim-otr"), msg,
	               QMessageBox::Yes | QMessageBox::No, this,
	               Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);
	
	if (mb.exec() == QMessageBox::Yes)
	{
		m_otr->deleteFingerprint(m_fingerprints[ui->fingerprintsTable->currentRow()]);
		updateFingerprints();
	}
}

void OtrSettingsWidget::on_pushButton_released()
{
	if(ui->keysTable->currentRow()<0)
		return;
	QString msg(tr("Are you sure you want to delete the private key:\naccount: %1\nprotocol: %2\n fingerptint: %3")
	            .arg(ui->keysTable->item(ui->keysTable->currentRow(),0)->text())
	            .arg(ui->keysTable->item(ui->keysTable->currentRow(),1)->text())
	            .arg(ui->keysTable->item(ui->keysTable->currentRow(),2)->text()));
	
	QMessageBox mb(QMessageBox::Question, tr("qutim-otr"), msg,
	               QMessageBox::Yes | QMessageBox::No, this,
	               Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);
	
	if (mb.exec() == QMessageBox::Yes)
	{
		m_otr->deleteKey(ui->keysTable->item(ui->keysTable->currentRow(),0)->text(),ui->keysTable->item(ui->keysTable->currentRow(),1)->text());
		emit updateKeys();
	}
}
