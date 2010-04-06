/*
   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License version 2 as published by the Free Software Foundation.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include "messagingdialog.h"

#include <QDebug>
#include <QFileDialog>
#include <QMenu>
#include "ui_messagingdialog.h"
#include "manager.h"

namespace MassMessaging
{
	MessagingDialog::MessagingDialog() : ui(new Ui::Dialog),m_manager(new Manager)
	{
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
	}
	void MessagingDialog::onLoadButtonClicked()
	{

	}
	void MessagingDialog::onSaveButtonClicked()
	{

	}
	void MessagingDialog::onSendButtonClicked()
	{

	}
	void MessagingDialog::onStopButtonClicked()
	{

	}
	void MessagingDialog::updateProgressBar(const uint& completed, const uint& total, const QString& message)
	{

	}
	MessagingDialog::~MessagingDialog()
	{
		delete ui;
	}


}
