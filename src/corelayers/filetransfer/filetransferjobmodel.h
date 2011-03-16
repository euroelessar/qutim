/****************************************************************************
 *  filetransferjobmodel.h
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

#ifndef FILETRANSFERJOBMODEL_H
#define FILETRANSFERJOBMODEL_H

#include <QAbstractListModel>
#include <qutim/filetransfer.h>

using namespace qutim_sdk_0_3;

namespace Core {

class FileTransferJobModel : public QAbstractListModel
{
	Q_OBJECT
	Q_DISABLE_COPY(FileTransferJobModel)
public:
	FileTransferJobModel(QObject *parent = 0);
	virtual ~FileTransferJobModel();
	void handleJob(FileTransferJob *job, FileTransferJob *oldJob);
	bool containsJob(FileTransferJob *job) { return m_jobs.contains(job); }
protected:
	enum Columns
	{
		Direction = 0,
		Title = 1,
		FileName = 2,
		FileSize = 3,
		TotalSize = 4,
		Progress = 5,
		State = 6,
		LastColumn = State
	};
	virtual QVariant headerData(int section, Qt::Orientation orientation,
								int role = Qt::DisplayRole) const;
	virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
	virtual int columnCount(const QModelIndex &parent = QModelIndex()) const;
	virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
private:
	void removeJob(int row);
private slots:
	void removeJob(QObject *job);
	void updateJob();
	void handleError(qutim_sdk_0_3::FileTransferJob::ErrorType, qutim_sdk_0_3::FileTransferJob *newJob);
private:
	QList<FileTransferJob*> m_jobs;
};

} // namespace Core

#endif // FILETRANSFERJOBMODEL_H
