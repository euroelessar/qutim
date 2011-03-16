/****************************************************************************
 *  filetransferjobmodel.cpp
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

#include "filetransferjobmodel.h"

namespace Core {

FileTransferJobModel::FileTransferJobModel(QObject *parent) :
	QAbstractListModel(parent)
{
}

FileTransferJobModel::~FileTransferJobModel()
{
}

void FileTransferJobModel::handleJob(FileTransferJob *job, FileTransferJob *oldJob)
{
	bool oldJobFound = false;
	if (oldJob) {
		int row = m_jobs.indexOf(oldJob);
		if (row != -1) {
			oldJobFound = true;
			disconnect(oldJob, 0, this, 0);
			m_jobs[row] = job;
		}
	}

	if (!oldJobFound) {
		if (m_jobs.contains(job))
			return;

		int row = m_jobs.size()-1;
		beginInsertRows(QModelIndex(), row, row);
		m_jobs.push_back(job);
		endInsertRows();
	}

	connect(job,
			SIGNAL(titleChanged(QString)),
			SLOT(updateJob()));
	connect(job,
			SIGNAL(fileNameChanged(QString)),
			SLOT(updateJob()));
	connect(job,
			SIGNAL(fileSizeChanged(qint64)),
			SLOT(updateJob()));
	connect(job,
			SIGNAL(totalSizeChanged(qint64)),
			SLOT(updateJob()));
	connect(job,
			SIGNAL(progressChanged(qint64)),
			SLOT(updateJob()));
	connect(job,
			SIGNAL(stateChanged(qutim_sdk_0_3::FileTransferJob::State)),
			SLOT(updateJob()));
	connect(job,
			SIGNAL(error(qutim_sdk_0_3::FileTransferJob::ErrorType,
						 qutim_sdk_0_3::FileTransferJob*)),
			SLOT(handleError(qutim_sdk_0_3::FileTransferJob::ErrorType,
							 qutim_sdk_0_3::FileTransferJob*)));
	connect(job,
			SIGNAL(destroyed(QObject*)),
			SLOT(removeJob(QObject*)));
}

void FileTransferJobModel::removeJob(int row)
{
	beginRemoveRows(QModelIndex(), row, row);
	m_jobs.removeAt(row);
	endRemoveRows();
}

void FileTransferJobModel::removeJob(QObject *job)
{
	int row = m_jobs.indexOf(static_cast<FileTransferJob*>(job));
	Q_ASSERT(row >= 0);
	removeJob(row);
}

void FileTransferJobModel::updateJob()
{
	int row = m_jobs.indexOf(static_cast<FileTransferJob*>(sender()));
	Q_ASSERT(row >= 0);
	QModelIndex index = this->index(row);
	emit dataChanged(index, index.sibling(0, LastColumn));
}

void FileTransferJobModel::handleError(qutim_sdk_0_3::FileTransferJob::ErrorType, qutim_sdk_0_3::FileTransferJob *newJob)
{
	if (newJob)
		return;
	int row = m_jobs.indexOf(static_cast<FileTransferJob*>(sender()));
	Q_ASSERT(row >= 0);
	QModelIndex index = this->index(row, State);
	emit dataChanged(index, index);
}

QVariant FileTransferJobModel::headerData(int section, Qt::Orientation orientation,
										  int role) const
{
	if (role != Qt::DisplayRole || orientation != Qt::Horizontal)
		return QVariant();
	switch (section)
	{
	case Direction:
		return tr("Direction");
	case Title:
		return tr("Title");
	case FileName:
		return tr("File name");
	case FileSize:
		return tr("File size");
	case TotalSize:
		return tr("Total size");
	case Progress:
		return tr("Progress");
	case State:
		return tr("State");
	default:
		return QVariant();
	}
}

int FileTransferJobModel::rowCount(const QModelIndex &parent) const
{
	Q_UNUSED(parent);
	return m_jobs.size();
}

int FileTransferJobModel::columnCount(const QModelIndex &parent) const
{
	Q_UNUSED(parent);
	return LastColumn+1;
}

QVariant FileTransferJobModel::data(const QModelIndex &index, int role) const
{
	if (role != Qt::DisplayRole)
		return QVariant();
	FileTransferJob *job = m_jobs.value(index.row());
	if (!job)
		return QVariant();
	switch (index.column())
	{
	case Direction:
		return job->direction() == FileTransferJob::Incoming ?
				tr("Incoming") :
				tr("Outgoing");
	case Title:
		return job->title();
	case FileName:
		return job->fileName();
	case FileSize:
		return job->fileSize();
	case TotalSize:
		return job->totalSize();
	case Progress:
		return job->progress() * 100 / job->totalSize();
	case State: {
		switch (job->state())
		{
		case FileTransferJob::Initiation:
			return tr("Initiation");
		case FileTransferJob::Started:
			return tr("Started");
		case FileTransferJob::Finished:
			return tr("Finished");
		case FileTransferJob::Error:
			switch (job->error())
			{
			case FileTransferJob::NetworkError:
				return tr("Network error");
			case FileTransferJob::Canceled:
				return tr("Canceled");
			case FileTransferJob::NotSupported:
				return tr("Not supported");
			default:
				return QVariant();
			}
		}
	}
	default:
		return QVariant();
	}
	return QVariant();
}

} // namespace Core
