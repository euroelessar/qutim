/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2012 Ruslan Nigmatullin <euroelessar@yandex.ru>
** Copyright © 2001-2009 Justin Karneges
** Copyright © 2001-2009 Michail Pishchagin
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

#include "jpgpkeydialog.h"
#include "ui_jpgpkeydialog.h"
#include <QPushButton>
#include <QSortFilterProxyModel>
#include <QMessageBox>
#include <QTimer>
#include "jpgpsupport.h"

Q_DECLARE_METATYPE(QCA::KeyStoreEntry)

namespace Jabber {

enum {
	KeyId = Qt::UserRole + 5,
	KeyEntry
};

JPGPProxyModel::JPGPProxyModel(QObject *parent) : QSortFilterProxyModel(parent)
{
	setFilterCaseSensitivity(Qt::CaseInsensitive);
}

bool JPGPProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const
{
	for (int column = 0; column <= 1; ++column) {
		QModelIndex index = sourceModel()->index(sourceRow, column, sourceParent);
		if (index.data(Qt::DisplayRole).toString().contains(filterRegExp()))
			return true;
	}
	return false;
}


JPGPKeyDialog::JPGPKeyDialog(JPGPKeyDialog::Type type, const QString &pgpKeyId, QWidget *parent)
    : QDialog(parent), ui(new Ui::JPGPKeyDialog), m_type(type)
{
	setAttribute(Qt::WA_DeleteOnClose);
	ui->setupUi(this);
	m_addButton = ui->buttonBox->addButton(tr("Add key"), QDialogButtonBox::ActionRole);
	connect(m_addButton, SIGNAL(clicked()), SLOT(onAddKeyButtonClicked()));
	m_addButton->setEnabled(JPGPSupport::instance()->canAddKey());
	m_model = new QStandardItemModel(this);
	m_model->setHorizontalHeaderLabels(QStringList() << tr("Key ID") << tr("User ID"));
	m_proxyModel = new JPGPProxyModel(this);
	m_proxyModel->setSourceModel(m_model);
	connect(ui->filterEdit, SIGNAL(textChanged(QString)),
	        m_proxyModel, SLOT(setFilterWildcard(QString)));
	ui->treeView->setModel(m_proxyModel);
	ui->treeView->header()->setResizeMode(QHeaderView::ResizeToContents);
	JPGPSupport::KeyType keyType = type == SecretKeys
	                               ? JPGPSupport::SecretKey
	                               : JPGPSupport::PublicKey;
	connect(JPGPSupport::instance(), SIGNAL(keysUpdated()), SLOT(onTimerShot()));
	QStandardItem *selectedItem = 0;
	foreach (const QCA::KeyStoreEntry &keyEntry, JPGPSupport::instance()->pgpKeys(keyType)) {
		QString keyId = addKeyEntry(keyEntry);
		if (keyId == pgpKeyId || !selectedItem)
			selectedItem = m_model->item(m_model->rowCount() - 1, 0);
	}
	if (selectedItem) {
		QModelIndex realIndex = m_model->indexFromItem(selectedItem);
        QModelIndex fakeIndex = m_proxyModel->mapFromSource(realIndex);
        ui->treeView->setCurrentIndex(fakeIndex);
        ui->treeView->scrollTo(fakeIndex);
	} else {
		ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
	}
}

JPGPKeyDialog::~JPGPKeyDialog()
{
	delete ui;
}

QString JPGPKeyDialog::addKeyEntry(const QCA::KeyStoreEntry &keyEntry)
{
	QList<QStandardItem*> items;
	items << new QStandardItem(keyEntry.id().right(8));
	items << new QStandardItem(keyEntry.name());
	QCA::PGPKey key;
	if (keyEntry.type() == QCA::KeyStoreEntry::TypePGPPublicKey)
		key = keyEntry.pgpPublicKey();
	else
		key = keyEntry.pgpSecretKey();
	for (int i = 0; i < items.size(); ++i) {
		QStandardItem *item = items[i];
		item->setData(key.keyId(), KeyId);
		item->setData(qVariantFromValue(keyEntry), KeyEntry);
	}
	m_model->appendRow(items);
	
	QModelIndex realIndex = m_model->indexFromItem(items.first());
    QModelIndex fakeIndex = m_proxyModel->mapFromSource(realIndex);
    ui->treeView->setCurrentIndex(fakeIndex);
    ui->treeView->scrollTo(fakeIndex);
	
	ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(true);
	return key.keyId();
}

void JPGPKeyDialog::done(int result)
{
	if (ui->stackedWidget->currentIndex() == 0) {
		QDialog::done(result);
	} else {
		if (result == Accepted) {
			QCA::ConvertResult result = QCA::ConvertGood;
			QCA::PGPKey key = QCA::PGPKey::fromString(ui->plainTextEdit->toPlainText(), &result);

			if (result == QCA::ErrorPassphrase) {
				QMessageBox::warning(this, tr("PGP key importing error"), tr("Entered invalid passphrase"));
				qDebug() << "Try to add key with invalid passphrase";
			} else if (key.isNull()) {
				QMessageBox::warning(this, tr("PGP key importing error"), tr("Provided key is invalid"));
				qDebug() << "Try to add null key";
			} else {
				qDebug() << "Try to add" << key.keyId() << key.primaryUserId();
				QString keyId = JPGPSupport::instance()->addKey(key);
				if (!keyId.isEmpty()) {
					// qca-gnupg is slow
					m_keysToAdd << keyId;
				}
			}
		}
		ui->stackedWidget->setCurrentIndex(0);
		m_addButton->show();
	}
}

QCA::KeyStoreEntry JPGPKeyDialog::keyStoreEntry() const
{
	QVariant data = ui->treeView->currentIndex().data(KeyEntry);
	return data.value<QCA::KeyStoreEntry>();
}

void JPGPKeyDialog::onAddKeyButtonClicked()
{
	m_addButton->hide();
	ui->stackedWidget->setCurrentIndex(1);
}

void JPGPKeyDialog::onTimerShot()
{
	JPGPSupport::KeyType keyType = m_type == SecretKeys
	                               ? JPGPSupport::SecretKey
	                               : JPGPSupport::PublicKey;
	for (int i = 0; i < m_keysToAdd.size(); ++i) {
		QString keyId = m_keysToAdd.at(i);
		QCA::KeyStoreEntry keyEntry = JPGPSupport::instance()->findEntry(keyId, keyType);
		if (!keyEntry.isNull()) {
			m_keysToAdd.removeAt(i);
			--i;
			addKeyEntry(keyEntry);
		}
	}
}

} // namespace Jabber
