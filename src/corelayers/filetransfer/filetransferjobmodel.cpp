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
#include "filetransferjobdelegate.h"
#include <qutim/localizedstring.h>
#include <qutim/icon.h>
#include <qutim/chatunit.h>

namespace Core {

static LocalizedString headers[]= {
	QT_TR_NOOP("Direction"),
	QT_TR_NOOP("Title"),
	QT_TR_NOOP("File name"),
	QT_TR_NOOP("File size"),
	QT_TR_NOOP("Total size"),
	QT_TR_NOOP("Contact"),
	QT_TR_NOOP("Progress"),
	QT_TR_NOOP("State")
};

FileTransferJobModel::FileTransferJobModel(QObject *parent) :
	QAbstractListModel(parent), m_rowBeingRemoved(-1)
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

		int row = m_jobs.size();
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

void FileTransferJobModel::removeJob(QObject *job)
{
	int row = m_jobs.indexOf(static_cast<FileTransferJob*>(job));
	Q_ASSERT(row >= 0);
	// Tell data() to skip this row.
	m_rowBeingRemoved = row;
	beginRemoveRows(QModelIndex(), row, row);
	m_jobs.takeAt(row)->deleteLater();
	endRemoveRows();
	m_rowBeingRemoved = -1;
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
	if (section >= 0 && section < LastColumn)
		return qVariantFromValue(headers[section]);
	return QVariant();
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
	if (index.row() == m_rowBeingRemoved)
		return QVariant();

	FileTransferJob *job = m_jobs.value(index.row());
	if (!job)
		return QVariant();

	if (role == Qt::DecorationRole) {
		if (index.column() == Direction)
			return Icon(job->direction() == FileTransferJob::Outgoing ?
						"go-up-filetransfer" :
						"go-down-filetransfer");
	}

	if (role == FileTransferJobRole)
		return qVariantFromValue(job);

	if (role == DescriptionRole) {
		QVariantMap map;
		QString name = job->fileName();
		if (!name.isEmpty())
			map.insert(headers[FileName], name);
		map.insert(headers[State], getState(job));
		map.insert(job->direction() == FileTransferJob::Incoming ?
				   tr("From") :
				   tr("To"),
				   job->chatUnit()->title());
		return map;
	}

	if (role != Qt::DisplayRole)
		return QVariant();

	switch (index.column())
	{
	case Title:
		return job->title();
	case FileName:
		return job->fileName();
	case FileSize:
		return bytesToString(job->fileSize());
	case TotalSize:
		return bytesToString(job->totalSize());
	case Progress:
		return job->progress() * 100 / job->totalSize();
	case State:
		return getState(job);
	case Contact:
		return job->chatUnit()->title();
	default:
		return QVariant();
	}
	return QVariant();
}

QString FileTransferJobModel::getState(FileTransferJob *job) const
{
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
			return QString();
		}
	default:
		return QString();
	}
}

QString bytesToString(quint64 bytes)
{
	double kb = (double)bytes / 1024;
	if (kb >= 1) {
		double mb = kb / 1024;
		if (mb >= 1) {
			double gb = mb / 1024;
			if (gb >= 1)
				return QObject::tr("%1 GB").arg(gb, 0, 'f', 2, ' ');
			else
				return QObject::tr("%1 MB").arg(mb, 0, 'f', 2, ' ');
		} else {
			return QObject::tr("%1 KB").arg(kb, 0, 'f', 2, ' ');
		}
	} else {
		return QObject::tr("%1 B").arg(bytes);
	}
}

} // namespace Core
