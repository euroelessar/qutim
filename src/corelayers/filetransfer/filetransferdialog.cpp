/****************************************************************************
 *  filetransferdialog.cpp
 *
 *  Copyright (c) 2011 by Nigmatullin Ruslan <euroelessar@gmail.com>
 *                        Prokhin Alexey <alexey.prokhin@yandex.ru>
 *
 ***************************************************************************
 *                                                                         *
 *   This library is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
*****************************************************************************/

#include "filetransferdialog.h"
#include "filetransferjobdelegate.h"
#include "filetransferjobmodel.h"
#include "simplefiletransfer.h"
#include "ui_filetransferdialog.h"
#include <QFileDialog>
#include <QToolButton>
#include <qutim/actionbox.h>
#include <qutim/icon.h>

Q_DECLARE_METATYPE(Core::ActionWidget*);

namespace Core
{

ActionWidget::ActionWidget(FileTransferJob *job, QWidget *parent) :
	QWidget(parent)
{
	QGridLayout *l = new QGridLayout(this);
	l->setMargin(0);
	l->setSpacing(0);
	l->addItem(new QSpacerItem(0, 20, QSizePolicy::Minimum, QSizePolicy::Expanding), 1, 0);

	m_stopButton = new QToolButton(this);
	m_stopButton->setText(tr("Stop"));
	m_stopButton->setToolTip(tr("Stop the task"));
	m_stopButton->setIcon(Icon("media-playback-stop-filetransfer"));
	m_stopButton->setProperty("actionWidget", qVariantFromValue(this));
	connect(m_stopButton, SIGNAL(clicked()), parent, SLOT(onStopTransferJob()));
	l->addWidget(m_stopButton, 0, 1);

	QToolButton *removeButton = new QToolButton(this);
	removeButton->setText(tr("Remove"));
	removeButton->setToolTip(tr("Remove the task"));
	removeButton->setIcon(Icon("edit-delete-filetransfer"));
	removeButton->setProperty("actionWidget", qVariantFromValue(this));
	connect(removeButton, SIGNAL(clicked()), parent, SLOT(onRemoveTransferJob()));
	l->addWidget(removeButton, 0, 2);

	onStateChanged(job->state());
	connect(job, SIGNAL(stateChanged(qutim_sdk_0_3::FileTransferJob::State)),
			SLOT(onStateChanged(qutim_sdk_0_3::FileTransferJob::State)));
}

void ActionWidget::onStateChanged(qutim_sdk_0_3::FileTransferJob::State state)
{
	m_stopButton->setVisible(state == FileTransferJob::Initiation ||
							 state == FileTransferJob::Started);
}

FileTransferDialog::FileTransferDialog(FileTransferJobModel *model) :
    ui(new Ui::FileTransferDialog)
{
	ui->setupUi(this);
	setAttribute(Qt::WA_DeleteOnClose, true);
	setAttribute(Qt::WA_QuitOnClose, false);
	setModel(model);
	ui->jobsView->setItemDelegate(new FileTransferJobDelegate(this));

	ActionBox *actions = new ActionBox(this);
	{
		QAction *action = new QAction(QObject::tr("Remove finished tasks"), actions);
		QObject::connect(action, SIGNAL(triggered()), SLOT(onRemoveFinishedJobs()));
		actions->addAction(action);
	}
	{
		QAction *action = new QAction(QObject::tr("Close"), actions);
		QObject::connect(action, SIGNAL(triggered()), this, SLOT(deleteLater()));
		actions->addAction(action);
	}
	ui->verticalLayout->addWidget(actions);
}

FileTransferDialog::~FileTransferDialog()
{
	delete ui;
}

void FileTransferDialog::setModel(FileTransferJobModel *model)
{
	m_model = model;
	ui->jobsView->setModel(model);
	connect(model, SIGNAL(rowsInserted(QModelIndex,int,int)),
			SLOT(rowsInserted(QModelIndex,int,int)));
	connect(model, SIGNAL(rowsRemoved(QModelIndex,int,int)),
			SLOT(rowsRemoved(QModelIndex,int,int)));
	qDeleteAll(m_actionWidgets);
	m_actionWidgets.clear();
	for (int i = 0, n = model->rowCount(); i < n; ++i)
		createActionWidget(i);
}

int FileTransferDialog::currentJob()
{
	return ui->jobsView->currentIndex().row();
}

void FileTransferDialog::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void FileTransferDialog::createActionWidget(int row)
{
	ActionWidget *widget = new ActionWidget(m_model->getJob(row), this);
	QModelIndex index = ui->jobsView->model()->index(row, 0);
	ui->jobsView->setIndexWidget(index, widget);
	m_actionWidgets.insert(row, widget);
}

void FileTransferDialog::rowsInserted(const QModelIndex &parent, int start, int end)
{
	Q_UNUSED(parent);
	for (; start <= end; ++start)
		createActionWidget(start);
}

void FileTransferDialog::rowsRemoved(const QModelIndex &parent, int start, int end)
{
	Q_UNUSED(parent);
	for (; end >= start; --end)
		m_actionWidgets.takeAt(end)->deleteLater();
}

void FileTransferDialog::onStopTransferJob()
{
	ActionWidget *actionWidget = sender()->property("actionWidget").value<ActionWidget*>();
	if (!actionWidget)
		return;
	FileTransferJob *job = m_model->getJob(m_actionWidgets.indexOf(actionWidget));
	if (job)
		job->stop();
}

void FileTransferDialog::onRemoveTransferJob()
{
	ActionWidget *actionWidget = sender()->property("actionWidget").value<ActionWidget*>();
	if (!actionWidget)
		return;
	FileTransferJob *job = m_model->getJob(m_actionWidgets.indexOf(actionWidget));
	if (job)
		job->deleteLater();
}

void FileTransferDialog::onRemoveFinishedJobs()
{
	foreach (FileTransferJob *job, m_model->allJobs()) {
		FileTransferJob::State state = job->state();
		if (state == FileTransferJob::Finished || state == FileTransferJob::Error)
			job->deleteLater();
	}
}

}
