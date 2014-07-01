/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2008 Denis Daschenko <daschenko@gmail.com>
** Copyright © 2008 Rustam Chakin <qutim.develop@gmail.com>
** Copyright © 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
** Copyright © 2014 Nicolay Izoderov <nico-izo@ya.ru>
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

#include "joinchatdialog.h"
#include "ui_joinchatdialog.h"
#include <qutim/icon.h>
#include <qutim/account.h>
#include <qutim/protocol.h>
#include <QPushButton>
#include <QKeyEvent>

using namespace qutim_sdk_0_3;

JoinChatDialog::JoinChatDialog(QWidget *parent) :
    QDialog(parent), m_ui(new Ui::JoinChat)
{
	m_ui->setupUi(this);
	QPushButton *button = m_ui->buttonBox->button(QDialogButtonBox::Apply);
	button->setText(tr("Join"));
	connect(button, SIGNAL(clicked()), SLOT(joinConference()));
	button = m_ui->buttonBox->button(QDialogButtonBox::Close);
	connect(button, SIGNAL(clicked()), SLOT(close()));
	button = m_ui->buttonBox->button(QDialogButtonBox::Save);
	connect(button, SIGNAL(clicked()), SLOT(onSaveButtonClicked()));
	// TODO: Add search api
//	button = m_ui->buttonBox->addButton(tr("Search"));
//	button->setIcon(Icon("edit-find"));
//	connect(button, SIGNAL(clicked()), SLOT(on_searchButton_clicked()));
	
	setAttribute(Qt::WA_QuitOnClose, false);
	setAttribute(Qt::WA_DeleteOnClose, true);
	connect(m_ui->conferenceListWidget,
	        SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)),
	        SLOT(showConference(QListWidgetItem*,QListWidgetItem*)));
	connect(m_ui->conferenceListWidget, SIGNAL(itemDoubleClicked(QListWidgetItem*)),
	        SLOT(joinBookmark(QListWidgetItem*)));
	
	m_ui->addConferenceButton->setIcon(Icon("list-add"));
	m_ui->removeConferenceButton->setIcon(Icon("list-remove"));
	m_ui->conferenceListWidget->installEventFilter(this);
	
	foreach (Protocol *protocol, Protocol::all()) {
		connect(protocol, SIGNAL(accountCreated(qutim_sdk_0_3::Account*)),
		        SLOT(onAccountCreated(qutim_sdk_0_3::Account*)));
		foreach (Account *account, protocol->accounts())
			onAccountCreated(account);
	}
	
	Config config;
	config.beginGroup("joinChatDialog");
	restoreGeometry(config.value("geometry", QByteArray()));
	m_ui->splitter->restoreState(config.value("splitter", QByteArray()));
}

JoinChatDialog::~JoinChatDialog()
{
	Config config;
	config.beginGroup("joinChatDialog");
	config.setValue("save", saveGeometry());
	config.setValue("splitter", m_ui->splitter->saveState());
}

void JoinChatDialog::showConference(QListWidgetItem *current, QListWidgetItem *previous)
{
	Q_UNUSED(previous);
	delete m_dataForm.data();
	if (!current)
		return;
	m_dataForm = AbstractDataForm::get(current->data(Qt::UserRole).value<DataItem>());
	m_ui->dataFormLayout->insertWidget(0, m_dataForm.data());
	connect(m_dataForm.data(), SIGNAL(changed()), SLOT(onDataChanged()));
	m_ui->buttonBox->button(QDialogButtonBox::Save)->setEnabled(false);
}

void JoinChatDialog::joinBookmark(QListWidgetItem *item)
{
	if (groupChatManager()->join(item->data(Qt::UserRole).value<DataItem>()))
		close();
}

void JoinChatDialog::onAccountCreated(qutim_sdk_0_3::Account *account, bool first)
{
    if (first) {
		connect(account, SIGNAL(statusChanged(qutim_sdk_0_3::Status,qutim_sdk_0_3::Status)),
		        SLOT(onAccountStatusChanged(qutim_sdk_0_3::Status)));
		connect(account, SIGNAL(groupChatManagerChanged(qutim_sdk_0_3::GroupChatManager*)),
		        SLOT(onManagerChanged(qutim_sdk_0_3::GroupChatManager*)));
		connect(account, SIGNAL(destroyed(QObject*)), SLOT(onAccountDeath(QObject*)));
	}
	if (!account->groupChatManager())
		return;
	m_ui->accountBox->addItem(account->status().icon(), account->id(),
	                          qVariantFromValue(account));
	if (m_ui->accountBox->count() == 1)
		m_ui->accountBox->setCurrentIndex(0);
}

void JoinChatDialog::onAccountStatusChanged(const qutim_sdk_0_3::Status &status)
{
	Account *account = qobject_cast<Account*>(sender());
	int index = m_ui->accountBox->findData(qVariantFromValue(account));
	if (index >= 0)
		m_ui->accountBox->setItemIcon(index, status.icon());
}

void JoinChatDialog::onManagerChanged(qutim_sdk_0_3::GroupChatManager *manager)
{
	Account *account = qobject_cast<Account*>(sender());
	int index = m_ui->accountBox->findData(qVariantFromValue(account));
	if (index < 0 && manager) {
		onAccountCreated(account, false);
	} else if (!manager) {
		m_ui->accountBox->removeItem(index);
	} else {
		m_ui->accountBox->removeItem(index);
		onAccountCreated(account, false);
	}
}

void JoinChatDialog::onAccountDeath(QObject *object)
{
	Account *account = static_cast<Account*>(object);
	int index = m_ui->accountBox->findData(qVariantFromValue(account));
	if (index >= 0)
		m_ui->accountBox->removeItem(index);
}

void JoinChatDialog::joinConference()
{
	if (m_dataForm) {
		groupChatManager()->join(m_dataForm.data()->item());
		close();
	}
}

void JoinChatDialog::on_accountBox_currentIndexChanged(int index)
{
	m_ui->conferenceListWidget->clear();
	if (index < 0) {
		m_ui->splitter->setEnabled(false);
		return;
	}
	m_ui->splitter->setEnabled(true);
	rebuildItems(index);
}

void JoinChatDialog::onDataChanged()
{
	m_ui->buttonBox->button(QDialogButtonBox::Save)->setEnabled(true);
}

void JoinChatDialog::onSaveButtonClicked()
{
	QListWidgetItem *item = m_ui->conferenceListWidget->currentItem();
	DataItem oldData = item->data(Qt::UserRole).value<DataItem>();
	GroupChatManager *manager = groupChatManager();
	DataItem data = m_dataForm.data()->item();
	if (manager->storeBookmark(data, oldData)) {
		int itemRow = m_ui->conferenceListWidget->currentRow();
		if (itemRow == 0) {
			itemRow = m_ui->conferenceListWidget->count();
			item = new QListWidgetItem(data.title(), m_ui->conferenceListWidget);
		}
		rebuildItems(itemRow);
	}
}

void JoinChatDialog::on_addConferenceButton_clicked()
{
	m_ui->conferenceListWidget->setCurrentRow(0);
}

void JoinChatDialog::on_removeConferenceButton_clicked()
{
	QListWidgetItem *item = m_ui->conferenceListWidget->currentItem();
	Q_ASSERT(item);
	DataItem data = item->data(Qt::UserRole).value<DataItem>();
	if (groupChatManager()->removeBookmark(data))
		delete item;
}

bool JoinChatDialog::eventFilter(QObject *obj, QEvent *event)
{
	if (event->type() == QEvent::KeyPress) {
		QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
		if (keyEvent->key() == Qt::Key_Return || keyEvent->key() == Qt::Key_Enter)
			joinConference();
		return QObject::eventFilter(obj, event);
	} else {
		return QObject::eventFilter(obj, event);
	}
}

void JoinChatDialog::rebuildItems(int index)
{
	GroupChatManager *manager = groupChatManager();
	QList<DataItem> bookmarks = manager->bookmarks();
	int count = m_ui->conferenceListWidget->count();
	if (count == 0) {
		QListWidgetItem *item = new QListWidgetItem(tr("New chat"), m_ui->conferenceListWidget);

		DataItem dataitem = manager->fields();
		if(!m_uri.isEmpty()) {
			QList<DataItem> items = dataitem.subitems();
			QList<DataItem>::const_iterator i;
			for(i = items.constBegin(); i != items.constEnd(); ++i)
				if((*i).name() == "conference") break;
			items[i-items.constBegin()] = DataItem("conference", QT_TRANSLATE_NOOP("Jabber", "Conference"), m_uri);
			dataitem.setSubitems(items);
		}
		item->setData(Qt::UserRole, qVariantFromValue(dataitem));
		++count;
	}
	for (int i = count - 1; i >= bookmarks.size() + 1; --i)
		delete m_ui->conferenceListWidget->item(i);
	for (int i = count; i < bookmarks.size() + 1; ++i)
		new QListWidgetItem(m_ui->conferenceListWidget);
	for (int i = 0; i < bookmarks.size(); ++i) {
		const DataItem &data = bookmarks.at(i);
		QListWidgetItem *item = m_ui->conferenceListWidget->item(i + 1);
		item->setText(data.title());
		item->setData(Qt::UserRole, qVariantFromValue(data));
	}
	index = qBound(0, index, m_ui->conferenceListWidget->count() - 1);
	if (m_ui->conferenceListWidget->currentRow() == index) {
		showConference(m_ui->conferenceListWidget->item(index), 0);
	} else {
		m_ui->conferenceListWidget->setCurrentRow(index);
	}
}

void JoinChatDialog::setUri(const QString &uri)
{
	m_uri = uri;
	on_accountBox_currentIndexChanged(m_ui->accountBox->currentIndex());
}

qutim_sdk_0_3::GroupChatManager *JoinChatDialog::groupChatManager()
{
	int index = m_ui->accountBox->currentIndex();
	Account *account = m_ui->accountBox->itemData(index).value<Account*>();
//	return qobject_cast<GroupChatManager*>(account);
	return account ? account->groupChatManager() : NULL;
}

