/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright (C) 2011 Prokhin Alexey <alexey.prokhin@yandex.ru>
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

