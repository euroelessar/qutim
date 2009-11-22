#include "accountcreatorprotocols.h"
#include "accountcreatorwizard.h"
#include "libqutim/extensioninfo.h"
#include "libqutim/icon.h"
#include "ui_accountcreatorprotocols.h"

namespace Core
{
	AccountCreatorProtocols::AccountCreatorProtocols(AccountCreatorWizard *parent) :
		QWizardPage(parent),
		m_ui(new Ui::AccountCreatorProtocols),
		m_wizard(parent)
	{
		m_ui->setupUi(this);
		ProtocolMap protocols = allProtocols();
		QStringList keys = protocols.keys();
		keys.sort();
		foreach (const QString &name, keys) {
			Protocol *proto = protocols.value(name);
			QVariant var = proto ? proto->property("extensioninfo") : QVariant();
			if (!var.isValid())
				continue;
			ExtensionInfo info = var.value<ExtensionInfo>();
			QIcon icon = info.icon();
			if (icon.isNull())
				icon = Icon(QLatin1String("im-") + name.toLower());
			m_ui->protocolsBox->addItem(info.icon(), info.name(), QVariant::fromValue(info));
			m_ui->protocolsBox->setItemData(m_ui->protocolsBox->count() - 1,
											reinterpret_cast<qptrdiff>(proto), Qt::UserRole + 1);
		}
	}

	AccountCreatorProtocols::~AccountCreatorProtocols()
	{
		delete m_ui;
	}

	bool AccountCreatorProtocols::validatePage()
	{
		int index = m_ui->protocolsBox->currentIndex();
		qptrdiff protoPtr = m_ui->protocolsBox->itemData(index, Qt::UserRole + 1).value<qptrdiff>();
		Protocol *proto = reinterpret_cast<Protocol *>(protoPtr);
		AccountCreationWizard *wizard = proto->accountCreationWizard();
		if (!wizard)
			return false;
		QList<QWizardPage *> pages = wizard->createPages(m_wizard);
		for (int i = 0; i < pages.size(); i++)
			m_wizard->setPage(Id + 1 + i, pages.at(i));
		return true;
	}

	bool AccountCreatorProtocols::isComplete() const
	{
		return true;
	}

	int AccountCreatorProtocols::nextId() const
	{
		return Id + 1;
	}

	void AccountCreatorProtocols::on_protocolsBox_currentIndexChanged(int index)
	{
		ExtensionInfo info = m_ui->protocolsBox->itemData(index).value<ExtensionInfo>();
		m_ui->protocolDescription->setText(info.description());
	}

	void AccountCreatorProtocols::changeEvent(QEvent *e)
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
