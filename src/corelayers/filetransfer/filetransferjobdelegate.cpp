/****************************************************************************
 *  filetransferjobdelegate.cpp
 *
 *  Copyright (c) 2011 by Prokhin Alexey <alexey.prokhin@yandex.ru>
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

#include "filetransferjobdelegate.h"
#include "filetransferjobmodel.h"
#include <QApplication>

namespace Core {

const int ProgressBarHeight = 20;

FileTransferJobDelegate::FileTransferJobDelegate(QObject* parent) :
	ItemDelegate(parent)
{
	setUserDefinedEditorSupport(true);
}

QSize FileTransferJobDelegate::sizeHint(const QStyleOptionViewItem& option,
										const QModelIndex& index) const
{
	QSize size = ItemDelegate::sizeHint(option, index);
	FileTransferJob *job =
			index.data(FileTransferJobModel::FileTransferJobRole).value<FileTransferJob*>();
	if (job)
		size.setHeight(size.height() + ProgressBarHeight);
	return size;
}

void FileTransferJobDelegate::paint(QPainter* painter,
									const QStyleOptionViewItem& option,
									const QModelIndex& index) const
{
	ItemDelegate::paint(painter, option, index);

	FileTransferJob *job =
			index.data(FileTransferJobModel::FileTransferJobRole).value<FileTransferJob*>();
	if (!job)
		return;

	quint64 progress = job->progress();
	quint64 size = job->totalSize();
	QStyleOptionProgressBar opt;
	opt.state = QStyle::State_Enabled;
	opt.direction = QApplication::layoutDirection();
	opt.rect = option.rect.adjusted(0, option.rect.height() - ProgressBarHeight - 5, 0, -5);
	opt.fontMetrics = QApplication::fontMetrics();
	opt.minimum = 0;
	opt.maximum = 100;
	opt.textAlignment = Qt::AlignCenter;
	opt.textVisible = true;
	opt.progress = size > 0 ? (progress * 100 / size) : 0;
	opt.text = QString("%1 / %2")
			   .arg(bytesToString(progress))
			   .arg(bytesToString(size));
	QApplication::style()->drawControl(QStyle::CE_ProgressBar, &opt, painter);
}

} // namespace Core
