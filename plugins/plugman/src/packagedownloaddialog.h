/****************************************************************************
 *
 *  This file is part of qutIM
 *
 *  Copyright (c) 2011 by Nigmatullin Ruslan <euroelessar@gmail.com>
 *
 ***************************************************************************
 *                                                                         *
 *   This file is part of free software; you can redistribute it and/or    *
 *   modify it under the terms of the GNU General Public License as        *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************
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
	PackageEngine *m_engine;
	PackageModel *m_model;
	QVector<PackageEntryWidget*> m_widgets;
};

#endif // PACKAGEDOWNLOADDIALOG_H
