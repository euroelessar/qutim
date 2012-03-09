/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
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

#include "filetransferdialog.h"
#include "filetransferjobdelegate.h"
#include "filetransferjobmodel.h"
#include "simplefiletransfer.h"
#include "ui_filetransferdialog.h"
#include <QFileDialog>
#include <QToolButton>
#include <QMenu>
#include <QUrl>
#include <QDesktopServices>
#include <qutim/actionbox.h>
#include <qutim/icon.h>

Q_DECLARE_METATYPE(Core::ActionWidget*)

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
	connect(m_stopButton, SIGNAL(clicked()), job, SLOT(stop()));
	l->addWidget(m_stopButton, 0, 1);

	QToolButton *removeButton = new QToolButton(this);
	removeButton->setText(tr("Remove"));
	removeButton->setToolTip(tr("Remove the task"));
	removeButton->setIcon(Icon("edit-delete-filetransfer"));
	connect(removeButton, SIGNAL(clicked()), job, SLOT(deleteLater()));
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
	setContextMenuPolicy(Qt::CustomContextMenu);
	connect(this, SIGNAL(customContextMenuRequested(QPoint)),SLOT(customContextMenuRequested(QPoint)));
	connect(ui->jobsView, SIGNAL(doubleClicked(QModelIndex)), SLOT(onOpenFileAction(QModelIndex)));
	setModel(model);
	ui->jobsView->setItemDelegate(new FileTransferJobDelegate(this));

	ActionBox *actions = new ActionBox(this);
	{
		QAction *action = new QAction(QObject::tr("Remove finished tasks"), actions);
#ifdef QUTIM_MOBILE_UI
		action->setSoftKeyRole(QAction::PositiveSoftKey);
#endif
		QObject::connect(action, SIGNAL(triggered()), SLOT(onRemoveFinishedJobs()));
		actions->addAction(action);
	}
	{
		QAction *action = new QAction(QObject::tr("Close"), actions);
#ifdef QUTIM_MOBILE_UI
		action->setSoftKeyRole(QAction::NegativeSoftKey);
#endif
		QObject::connect(action, SIGNAL(triggered()), this, SLOT(deleteLater()));
		actions->addAction(action);
	}
	ui->verticalLayout->addWidget(actions);

	m_removeAction = new QAction(tr("Remove"), this);
	connect(m_removeAction, SIGNAL(triggered()), SLOT(onRemoveJob()));
	m_stopAction = new QAction(tr("Stop"), this);
	connect(m_stopAction, SIGNAL(triggered()), SLOT(onStopJob()));
	m_openDirAction = new QAction(tr("Open containing folder"), this);
	connect(m_openDirAction, SIGNAL(triggered()), SLOT(onOpenDirAction()));
	m_openFileAction = new QAction(tr("Open"), this);
	connect(m_openFileAction, SIGNAL(triggered()), SLOT(onOpenFileAction()));
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

FileTransferJob *FileTransferDialog::getSelectedJob()
{
	int row = ui->jobsView->currentIndex().row();
	return m_model->getJob(row);
}

void FileTransferDialog::openPath(const QString &path)
{
	QDesktopServices::openUrl(QLatin1String("file:///") + path);
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

void FileTransferDialog::onRemoveFinishedJobs()
{
	foreach (FileTransferJob *job, m_model->allJobs()) {
		FileTransferJob::State state = job->state();
		if (state == FileTransferJob::Finished || state == FileTransferJob::Error)
			job->deleteLater();
	}
}

void FileTransferDialog::customContextMenuRequested(const QPoint &pos)
{
	FileTransferJob *job = getSelectedJob();
	if (!job)
		return;

	FileTransferJob::State state = job->state();
	QMenu *menu = new QMenu(this);
	setAttribute(Qt::WA_DeleteOnClose, true);
	if (job->direction() == FileTransferJob::Incoming &&
		state == FileTransferJob::Finished)
	{
		menu->addAction(m_openFileAction);
		if (job->filesCount() == 1)
			menu->addAction(m_openDirAction);
	}
	if (state == FileTransferJob::Initiation ||
		state == FileTransferJob::Started)
	{
		menu->addAction(m_stopAction);
	}
	menu->addAction(m_removeAction);
	menu->move(ui->jobsView->mapToGlobal(pos));
	menu->show();
}

void FileTransferDialog::onRemoveJob()
{
	FileTransferJob *job = getSelectedJob();
	if (job)
		job->deleteLater();
}

void FileTransferDialog::onStopJob()
{
	FileTransferJob *job = getSelectedJob();
	if (job)
		job->stop();
}

void FileTransferDialog::onOpenFileAction(const QModelIndex &index)
{
	FileTransferJob *job = m_model->getJob(index.row());
	if (!job)
		return;
	if (job->direction() == FileTransferJob::Incoming &&
		job->state() == FileTransferJob::Finished)
	{
		QString path = job->property("localPath").toString();
		openPath(path);
	}
}

void FileTransferDialog::onOpenFileAction()
{
	FileTransferJob *job = getSelectedJob();
	if (!job)
		return;
	QString path = job->property("localPath").toString();
	openPath(path);
}

void FileTransferDialog::onOpenDirAction()
{
	FileTransferJob *job = getSelectedJob();
	if (!job)
		return;
	QString path = job->property("localPath").toString();
	openPath(QFileInfo(path).absolutePath());
}

}

