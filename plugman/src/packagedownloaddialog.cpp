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

#include "packagedownloaddialog.h"
#include "packagedelegate.h"
#include "ui_packagedownloaddialog.h"
#include <qutim/debug.h>

using namespace qutim_sdk_0_3;

PackageDownloadDialog::PackageDownloadDialog(PackageEngine *engine) :
    ui(new Ui::PackageDownloadDialog)
{
    ui->setupUi(this);
	m_model = new PackageModel(engine);
	ui->listView->setModel(m_model);
	ui->listView->setItemDelegate(new PackageDelegate);
	connect(m_model, SIGNAL(rowsInserted(QModelIndex,int,int)),
	        this, SLOT(onRowsInserted(QModelIndex,int,int)));
	connect(m_model, SIGNAL(dataChanged(QModelIndex,QModelIndex)),
	        this, SLOT(onDataChanged(QModelIndex,QModelIndex)));
}

PackageDownloadDialog::~PackageDownloadDialog()
{
    delete ui;
}

void PackageDownloadDialog::onDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight)
{
	for (int i = topLeft.row(); i <= bottomRight.row(); ++i) {
		if (PackageEntryWidget *widget = m_widgets.value(i))
			widget->updateData();
	}
}

void PackageDownloadDialog::onRowsInserted(const QModelIndex &parent, int first, int last)
{
	m_widgets.resize(m_model->rowCount());
	for (int i = first; i <= last; ++i) {
		QModelIndex index = m_model->index(i, 0, parent);
		PackageEntry entry = index.data(Qt::UserRole).value<PackageEntry>();
		PackageEntryWidget *widget = new PackageEntryWidget(entry);
		m_widgets[i] = widget;
		ui->listView->setIndexWidget(index, widget);
	}
}
