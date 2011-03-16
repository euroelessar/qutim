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

Q_DECLARE_METATYPE(qutim_sdk_0_3::FileTransferJob*);

namespace Core {

class FileTransferJobModel : public QAbstractListModel
{
	Q_OBJECT
	Q_DISABLE_COPY(FileTransferJobModel)
public:
	enum Roles
	{
		FileTransferJobRole = Qt::UserRole
	};

	FileTransferJobModel(QObject *parent = 0);
	virtual ~FileTransferJobModel();
	void handleJob(FileTransferJob *job, FileTransferJob *oldJob);
	bool containsJob(FileTransferJob *job) { return m_jobs.contains(job); }
protected:
	enum Columns
	{
		Title = 0,
		Direction = 0,
		FileName = 1,
		FileSize = 2,
		TotalSize = 3,
		Progress = 4,
		State = 5,
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
	QString getState(FileTransferJob *job) const;
private:
	QList<FileTransferJob*> m_jobs;
};

QString bytesToString(quint64 bytes);

} // namespace Core

#endif // FILETRANSFERJOBMODEL_H
