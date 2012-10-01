/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Alexey Prokhin <alexey.prokhin@yandex.ru>
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

#include "filetransfersettings.h"
#include <qutim/filetransfer.h>
#include <qutim/config.h>
#include <qutim/localizedstring.h>
#include <qutim/settingslayer.h>
#include <qutim/icon.h>
#include <QListWidget>
#include <QVBoxLayout>
#include <qutim/itemdelegate.h>

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
	setModified(false);
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
		setModified(true);
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

