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

#include "chooseclientpage.h"
#include "ui_chooseclientpage.h"
#include "clients/qip.h"
#include "clients/qutim.h"
#include "clients/kopete.h"
#include "clients/pidgin.h"
#include "clients/sim.h"
// #include "clients/jimm.h"
#include "clients/qippda.h"
#include "clients/qipinfium.h"
#include "clients/licq.h"
#include "clients/andrq.h"
#include "clients/psi.h"
#include "clients/gajim.h"
#include "clients/miranda.h"
#include "clients/jasmineim.h"
#include <qutim/objectgenerator.h>

#include <qutim/icon.h>

using namespace qutim_sdk_0_3;

namespace HistoryManager {

ChooseClientPage::ChooseClientPage(HistoryManagerWindow *parent) :
    QWizardPage(parent),
    m_ui(new Ui::ChooseClientPage)
{
    m_ui->setupUi(this);
	m_parent = parent;
	setTitle(tr("Client"));
	setSubTitle(tr("Choose client which history you want to import to qutIM."));
	GeneratorList gens = ObjectGenerator::module<HistoryImporter>();
	foreach (const ObjectGenerator *gen, gens)
		m_clients_list.append(gen->generate<HistoryImporter>());
//	ui.clientBox->addItem(Icon("jimm", IconInfo::Client), "Jimm", (qptrdiff)new jimm());
	m_clients_list
			<< m_parent->getQutIM()
			<< new kopete
			<< new qip
			<< new pidgin
			<< new qipinfium
			<< new andrq
			<< new pidgin
			<< new sim
			<< new qippda
			<< new licq
			<< new psi
			<< new gajim
			<< new miranda
			<< new jasmineim;

	QMap<QString, HistoryImporter *> clients;
	foreach(HistoryImporter *client, m_clients_list)
		clients.insert(client->name().toLower(), client);

	foreach(HistoryImporter *client, clients)
	{
		client->setDataBase(m_parent);
		QListWidgetItem *item = new QListWidgetItem(client->icon(), client->name(), m_ui->listWidget);
		item->setData(Qt::UserRole, (qptrdiff)client);
	}
	connect(m_ui->listWidget, SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)),
			this, SLOT(clientChanged(QListWidgetItem*,QListWidgetItem*)));
//	connect(m_ui->checkBox, SIGNAL(clicked()), this, SLOT(clientChanged()));
//	m_ui->checkBox->setDisabled(true);
}

ChooseClientPage::~ChooseClientPage()
{
    delete m_ui;
	qDeleteAll(m_clients_list);
	m_clients_list.clear();
}

void ChooseClientPage::initializePage()
{
	m_parent->setCurrentClient(0);
	m_ui->listWidget->setCurrentIndex(QModelIndex());

//	bool first = m_parent->getMessageNum() < 1;
//	m_ui->checkBox->setEnabled(!first);
//	m_ui->checkBox->setChecked(!first);
//	m_ui->listWidget->setEnabled(first);
	m_valid = false; // !first;
}

void ChooseClientPage::cleanupPage()
{
	m_valid = false;
}

bool ChooseClientPage::isComplete() const
{
	return m_valid;
}

int ChooseClientPage::nextId() const
{
	if(m_parent->getCurrentClient())
		return HistoryManagerWindow::ConfigClient;
	else
		return HistoryManagerWindow::ExportHistory;
}

void ChooseClientPage::clientChanged(QListWidgetItem *current, QListWidgetItem *)
{
	if(/*m_ui->checkBox->isChecked() || */!current)
	{
		m_valid = false; //m_ui->checkBox->isEnabled() && m_ui->checkBox->isChecked();
		m_parent->setCurrentClient(0);
	}
	else
	{
		m_parent->setCurrentClient((HistoryImporter *)current->data(Qt::UserRole).value<qptrdiff>());
		m_valid = true;
	}
	emit completeChanged();
}

void ChooseClientPage::changeEvent(QEvent *e)
{
    QWizardPage::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        m_ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

}

