/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Aleksey Sidorov <gorthauer87@yandex.ru>
**
*****************************************************************************
**
** $QUTIM_BEGIN_LICENSE$
** This program is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
** See the GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program.  If not, see http://www.gnu.org/licenses/.
** $QUTIM_END_LICENSE$
**
****************************************************************************/


#include "messagingdialog.h"

#include <QDebug>
#include <QFileDialog>
#include <QMenu>
#include "ui_messagingdialog.h"
#include "manager.h"
#include <QTime>

MessagingDialog::MessagingDialog() : ui(new Ui::Dialog)
{
	m_manager = new Manager(this);
	ui->setupUi(this);
	setAttribute(Qt::WA_DeleteOnClose);

	QList<int> sizes;
	sizes.append(150);
	sizes.append(250);
	ui->splitter->setSizes(sizes);

	ui->progressBar->hide();
	ui->progressHint->hide();

	ui->treeView->setModel(m_manager->model());
	m_manager->reload();

	connect(ui->sendButton,SIGNAL(clicked(bool)),SLOT(onSendButtonClicked()));
	connect(m_manager,SIGNAL(finished(bool)),SLOT(onManagerFinished(bool)));
	connect(m_manager,SIGNAL(update(uint,uint,QString)),SLOT(updateProgressBar(uint,uint,QString)));
}

void MessagingDialog::onSendButtonClicked()
{
	if (!m_manager->currentState()) {
		ui->progressBar->show();
		ui->sendButton->setText(tr("Stop"));
		//ui->progressHint->show();
		int interval = ui->intervalEdit->text().toInt()*1000;
		m_manager->start(ui->messageEdit->toPlainText(),interval);
	}
	else
		m_manager->stop();
}

void MessagingDialog::updateProgressBar(const uint& completed, const uint& total, const QString& message)
{
	ui->progressBar->setMaximum(total);
	ui->progressBar->setValue(completed);
	ui->progressBar->setFormat(tr("Sending message to %1: %v/%m").arg(message));
	ui->progressBar->setToolTip(tr("Sending message to %1").arg(message));
	//progressHint->setText(tr("Sending messages: (%2/%3)").arg(completed).arg(total));
	int secs = (total-completed)*ui->intervalEdit->text().toInt();
	QTime time;
	time = time.addSecs(secs);
	setWindowTitle(tr("Sending message to %1 (%2/%3), time remains: %4").arg(message).arg(completed).arg(total).arg(time.toString()));
}

MessagingDialog::~MessagingDialog()
{
	delete ui;
}

void MessagingDialog::onManagerFinished(bool ok)
{
	ui->progressBar->hide();
	//ui->progressHint->hide();
	setWindowTitle(ok ? tr("Finished") : tr("Error"));
	ui->sendButton->setText(tr("Start"));
}
