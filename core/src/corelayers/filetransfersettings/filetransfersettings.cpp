/****************************************************************************
 *  filetransfersettings.cpp
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

#include "filetransfersettings.h"
#include <qutim/filetransfer.h>
#include <qutim/config.h>
#include <qutim/localizedstring.h>
#include <qutim/settingslayer.h>
#include <qutim/icon.h>
#include <QListWidget>
#include <QVBoxLayout>
#include "3rdparty/itemdelegate/itemdelegate.h"

namespace Core {

const int FactoryClassNameRole = Qt::UserRole + 123;

FileTransferSettingsWidget::FileTransferSettingsWidget() :
	m_changed(false)
{
	QVBoxLayout *layout = new QVBoxLayout(this);
	m_factoriesWidget = new QListWidget(this);
	m_factoriesWidget->setItemDelegate(new ItemDelegate(this));
	m_factoriesWidget->setDragEnabled(true);
	m_factoriesWidget->viewport()->setAcceptDrops(true);
	m_factoriesWidget->setDefaultDropAction(Qt::MoveAction);
	m_factoriesWidget->setDropIndicatorShown(true);
	m_factoriesWidget->setDragDropMode(QAbstractItemView::InternalMove);
	layout->addWidget(m_factoriesWidget);
	connect(m_factoriesWidget->model(),
			SIGNAL(rowsMoved(QModelIndex,int,int,QModelIndex,int)),
			SLOT(onChanged()));
}

void FileTransferSettingsWidget::clearState()
{
	m_changed = false;
	emit modifiedChanged(false);
}

void FileTransferSettingsWidget::loadImpl()
{
	m_factoriesWidget->clear();
	clearState();
	foreach (FileTransferFactory *factory, FileTransferManager::factories()) {
		QListWidgetItem *item = new QListWidgetItem(m_factoriesWidget);
		item->setText(factory->name());
		item->setIcon(factory->icon());
		item->setData(DescriptionRole, qVariantFromValue(factory->description()));
		item->setData(FactoryClassNameRole, factory->metaObject()->className());
	}
}

void FileTransferSettingsWidget::saveImpl()
{
	QStringList factories;
	for (int i = 0, c = m_factoriesWidget->count(); i < c; ++i) {
		QListWidgetItem *item = m_factoriesWidget->item(i);
		QString name = item->data(FactoryClassNameRole).toString();
		Q_ASSERT(!name.isEmpty());
		factories << name;
	}

	FileTransferManager::updateFactories(factories);
	clearState();
}

void FileTransferSettingsWidget::cancelImpl()
{
	loadImpl();
}

void FileTransferSettingsWidget::onChanged()
{
	if (!m_changed) {
		m_changed = true;
		emit modifiedChanged(true);
	}
}

FileTransferSettings::FileTransferSettings()
{
	GeneralSettingsItem<FileTransferSettingsWidget> *item =
			new GeneralSettingsItem<FileTransferSettingsWidget>(
					Settings::General,
					Icon("document-save-filetransfer-settings"),
					QT_TRANSLATE_NOOP("Settings","File transfer")
					);
	Settings::registerItem(item);
}

} // namespace Core
