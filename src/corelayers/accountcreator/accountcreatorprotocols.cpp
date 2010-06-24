#include "accountcreatorprotocols.h"
#include "accountcreatorwizard.h"
#include "libqutim/extensioninfo.h"
#include "libqutim/icon.h"
#include "ui_accountcreatorprotocols.h"
#include <QDebug>

namespace Core
{
	AccountCreatorProtocols::AccountCreatorProtocols(AccountCreatorWizard *parent) :
		QWizardPage(parent),
		m_ui(new Ui::AccountCreatorProtocols),
		m_wizard(parent)
	{
		m_ui->setupUi(this);
		
		connect(m_ui->protocolsBox, SIGNAL(currentIndexChanged(int)),
				this, SIGNAL(completeChanged()));
		qDebug() << Q_FUNC_INFO;
		m_lastId = Id;
		QSet<QByteArray> protocols;
		foreach (Protocol *protocol, allProtocols()) {
			protocols.insert(protocol->metaObject()->className());
		}

		foreach(const ObjectGenerator *gen, moduleGenerators<AccountCreationWizard>()) {
			const char *proto = metaInfo(gen->metaObject(), "DependsOn");
			if (!protocols.contains(proto))
				continue;
			AccountCreationWizard *wizard = gen->generate<AccountCreationWizard>();
			m_wizards.insert(wizard->info().name(), wizard);
		}

		foreach (AccountCreationWizard *wizard, m_wizards) {
			ExtensionInfo info = wizard->info();
			QIcon icon = info.icon();
			if (icon.isNull())
				icon = Icon(QLatin1String("im-") + info.name().original().toLower());
			m_ui->protocolsBox->addItem(info.icon(), info.name(), QVariant::fromValue(info));
			m_ui->protocolsBox->setItemData(m_ui->protocolsBox->count() - 1,
											reinterpret_cast<qptrdiff>(wizard),
											Qt::UserRole + 1);
		}
	}

	AccountCreatorProtocols::~AccountCreatorProtocols()
	{
		delete m_ui;
		qDeleteAll(m_wizards);
	}

	bool AccountCreatorProtocols::validatePage()
	{
		int index = m_ui->protocolsBox->currentIndex();
		qptrdiff wizardPtr = m_ui->protocolsBox->itemData(index, Qt::UserRole + 1).value<qptrdiff>();
		AccountCreationWizard *wizard = reinterpret_cast<AccountCreationWizard *>(wizardPtr);
		if (!wizard)
			return false;

		QMap<AccountCreationWizard *, int>::iterator it = ensureCurrentProtocol();
		return it.value() != -1;
	}

	bool AccountCreatorProtocols::isComplete() const
	{
		qDebug() << Q_FUNC_INFO << (m_ui->protocolsBox->currentIndex() != -1);
		return m_ui->protocolsBox->currentIndex() != -1;
	}

	int AccountCreatorProtocols::nextId() const
	{
		qDebug() << Q_FUNC_INFO << (m_ui->protocolsBox->count() == 0)
				<< (const_cast<AccountCreatorProtocols *>(this)->ensureCurrentProtocol() == m_wizardIds.end())
				<< (const_cast<AccountCreatorProtocols *>(this)->ensureCurrentProtocol() == m_wizardIds.end()
					? m_lastId + 1 : const_cast<AccountCreatorProtocols *>(this)->ensureCurrentProtocol().value());
		if (m_ui->protocolsBox->count() == 0)
			return -1;
		QMap<AccountCreationWizard *, int>::iterator it
				= const_cast<AccountCreatorProtocols *>(this)->ensureCurrentProtocol();
		return (it == m_wizardIds.end()  || it.value() == -1) ? m_lastId + 1 : it.value();
	}

	void AccountCreatorProtocols::on_protocolsBox_currentIndexChanged(int index)
	{
		ExtensionInfo info = m_ui->protocolsBox->itemData(index).value<ExtensionInfo>();
		m_ui->protocolDescription->setText(info.description());
	}

	QMap<AccountCreationWizard *, int>::iterator AccountCreatorProtocols::ensureCurrentProtocol()
	{
		int index = m_ui->protocolsBox->currentIndex();
		qptrdiff wizardPtr = m_ui->protocolsBox->itemData(index, Qt::UserRole + 1).value<qptrdiff>();
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
			m_ui->retranslateUi(this);
			break;
		default:
			break;
		}
	}
}
