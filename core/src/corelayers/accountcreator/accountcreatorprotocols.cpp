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
#include "accountcreatorprotocols.h"
#include "accountcreatorwizard.h"
#include <qutim/extensioninfo.h>
#include <qutim/icon.h>
#include "qutim/metaobjectbuilder.h"
#include "ui_accountcreatorprotocols.h"
#include <QCommandLinkButton>
#include <QScrollBar>
#include <qutim/debug.h>
#include <qutim/itemdelegate.h>
#include <qutim/servicemanager.h>

namespace Core
{
AccountCreatorProtocols::AccountCreatorProtocols(QWizard *parent) :
	QWizardPage(parent),
	ui(new Ui::AccountCreatorProtocols),
	m_wizard(parent)
{
	ui->setupUi(this);

	ItemDelegate *delegate = new ItemDelegate(this);
	ui->protocolList->setItemDelegate(delegate);
#ifdef Q_OS_WIN
	delegate->setCommandLinkStyle(true);
#endif
	ui->protocolList->setFrameShape(QFrame::NoFrame);

	m_lastId = Id;
	QSet<QByteArray> protocols;
	foreach (Protocol *protocol, Protocol::all()) {
		protocols.insert(protocol->metaObject()->className());
	}

	foreach(const ObjectGenerator *gen, ObjectGenerator::module<AccountCreationWizard>()) {
		const char *proto = MetaObjectBuilder::info(gen->metaObject(), "DependsOn");
		if (!protocols.contains(proto))
			continue;
		AccountCreationWizard *wizard = gen->generate<AccountCreationWizard>();
		m_wizards.insert(wizard->info().name(), wizard);
	}

	foreach (AccountCreationWizard *wizard, m_wizards) {
		ExtensionInfo info = wizard->info();
		QIcon icon = info.icon();
		if (icon.isNull())
			icon = Icon(QLatin1String("im-") + info.name());
		if (!icon.actualSize(QSize(1,1)).isValid())
			icon = Icon("applications-internet");

		QListWidgetItem *item = new QListWidgetItem(ui->protocolList);
		item->setData(Qt::UserRole + 1, reinterpret_cast<qptrdiff>(wizard));
		item->setData(Qt::UserRole + 2, qVariantFromValue(info));

		item->setIcon(icon);
		item->setText(info.name());
		item->setData(DescriptionRole,info.description().toString());
	}

	setTitle(tr("Select protocol"));
	connect(ui->protocolList, SIGNAL(itemClicked(QListWidgetItem*)),
			this, SLOT(protocolActivated(QListWidgetItem*)));

	if(QObject *scroller = ServiceManager::getByName("Scroller"))
		QMetaObject::invokeMethod(scroller, "enableScrolling", Q_ARG(QObject*, ui->protocolList->viewport()));
}

AccountCreatorProtocols::~AccountCreatorProtocols()
{
	delete ui;
	qDeleteAll(m_wizards);
}

bool AccountCreatorProtocols::validatePage()
{
	return true;
}

bool AccountCreatorProtocols::isComplete() const
{
	return false;
}

int AccountCreatorProtocols::nextId() const
{
	if (ui->protocolList->count() == 0)
		return -1;
	QMap<AccountCreationWizard *, int>::iterator it
			= const_cast<AccountCreatorProtocols *>(this)->ensureCurrentProtocol();
	return (it == m_wizardIds.end()  || it.value() == -1) ? m_lastId + 1 : it.value();
}

QMap<AccountCreationWizard *, int>::iterator AccountCreatorProtocols::ensureCurrentProtocol()
{
	QListWidgetItem *item = ui->protocolList->currentItem();

	if (!item)
		return m_wizardIds.end();

	qptrdiff wizardPtr = item->data(Qt::UserRole + 1).value<qptrdiff>();
	AccountCreationWizard *wizard = reinterpret_cast<AccountCreationWizard *>(wizardPtr);
	if (!wizard)
		return m_wizardIds.end();

	QMap<AccountCreationWizard *, int>::iterator it = m_wizardIds.find(wizard);
	if (it == m_wizardIds.end()) {
		QList<QWizardPage *> pages = wizard->createPages(m_wizard);
		if (pages.isEmpty()) {
			it = m_wizardIds.insert(wizard, -1);
		} else {
			it = m_wizardIds.insert(wizard, m_lastId + 1);
			for (int i = 0; i < pages.size(); i++)
				m_wizard->setPage(++m_lastId, pages.at(i));
		}
	}
	return it;
}

void AccountCreatorProtocols::changeEvent(QEvent *e)
{
	QWizardPage::changeEvent(e);
	switch (e->type()) {
	case QEvent::LanguageChange:
		ui->retranslateUi(this);
		break;
	default:
		break;
	}
}

void AccountCreatorProtocols::protocolActivated(QListWidgetItem *)
{
	wizard()->next();
}

}

