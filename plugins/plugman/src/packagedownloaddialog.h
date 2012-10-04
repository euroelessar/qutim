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

#ifndef PACKAGEDOWNLOADDIALOG_H
#define PACKAGEDOWNLOADDIALOG_H

#include <QDialog>
#include "packageengine.h"
#include "packagemodel.h"
#include "packageentrywidget.h"

namespace Ui {
    class PackageDownloadDialog;
}

class PackageDownloadDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PackageDownloadDialog(const QStringList &categories, const QString &path);
    ~PackageDownloadDialog();

protected slots:
    void onDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight);
	void onRowsInserted(const QModelIndex &parent, int first, int last);
	void onWidgetButtonClicked();
	void onTypeChecked(int type);
	void onScrollBarValueChanged(int value);
	
private:
    Ui::PackageDownloadDialog *ui;
	PackageModel *m_model;
	QVector<PackageEntryWidget*> m_widgets;
};

#endif // PACKAGEDOWNLOADDIALOG_H

