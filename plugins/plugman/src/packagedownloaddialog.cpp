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

#include "packagedownloaddialog.h"
#include "packagedelegate.h"
#include "ui_packagedownloaddialog.h"
#include <qutim/debug.h>
#include <QSignalMapper>
#include <QScrollBar>

using namespace qutim_sdk_0_3;

enum ItemsType
{
	NewestItems = Attica::Provider::Newest,
	MostRatingItems = Attica::Provider::Rating,
	MostDownloadsItems = Attica::Provider::Downloads,
	InstalledItems = -1
};

PackageDownloadDialog::PackageDownloadDialog(const QStringList &categories, const QString &path) :
    ui(new Ui::PackageDownloadDialog)
{
	ui->setupUi(this);
	m_model = new PackageModel(this);
	m_model->setCategories(categories);
	m_model->setPath(path);
	ui->listView->setModel(m_model);
	ui->listView->setItemDelegate(new PackageDelegate);
	ui->listView->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
	connect(ui->listView->verticalScrollBar(), SIGNAL(valueChanged(int)),
	        SLOT(onScrollBarValueChanged(int)));
	connect(m_model, SIGNAL(rowsInserted(QModelIndex,int,int)),
	        this, SLOT(onRowsInserted(QModelIndex,int,int)));
	connect(m_model, SIGNAL(dataChanged(QModelIndex,QModelIndex)),
	        this, SLOT(onDataChanged(QModelIndex,QModelIndex)));
	QSignalMapper *mapper = new QSignalMapper(this);
	connect(ui->newestButton, SIGNAL(clicked()), mapper, SLOT(map()));
	mapper->setMapping(ui->newestButton, NewestItems);
	connect(ui->ratingButton, SIGNAL(clicked()), mapper, SLOT(map()));
	mapper->setMapping(ui->ratingButton, MostRatingItems);
	connect(ui->mostDownloadsButton, SIGNAL(clicked()), mapper, SLOT(map()));
	mapper->setMapping(ui->mostDownloadsButton, MostDownloadsItems);
	connect(ui->installedButton, SIGNAL(clicked()), mapper, SLOT(map()));
	mapper->setMapping(ui->installedButton, InstalledItems);
	connect(mapper, SIGNAL(mapped(int)), SLOT(onTypeChecked(int)));
	setAttribute(Qt::WA_DeleteOnClose, true);
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
		connect(widget, SIGNAL(buttonClicked()), SLOT(onWidgetButtonClicked()));
		m_widgets[i] = widget;
		ui->listView->setIndexWidget(index, widget);
	}
}

void PackageDownloadDialog::onWidgetButtonClicked()
{
	PackageEntryWidget *widget = static_cast<PackageEntryWidget*>(sender());
	Q_ASSERT(qobject_cast<PackageEntryWidget*>(static_cast<QObject*>(widget)));
	if (widget->entry().status() == PackageEntry::Installed)
		m_model->engine()->remove(widget->entry());
	else
		m_model->engine()->install(widget->entry(), m_model->path());
}

void PackageDownloadDialog::onTypeChecked(int type)
{
	switch (type) {
	case MostRatingItems:
	case MostDownloadsItems:
	case NewestItems:
		m_model->setSortMode(static_cast<PackageModel::SortMode>(type));
		break;
	case InstalledItems:
		
		break;
	default:
		break;
	}
}

void PackageDownloadDialog::onScrollBarValueChanged(int value)
{
	if (value == ui->listView->verticalScrollBar()->maximum())
		m_model->requestNextPage();
}

