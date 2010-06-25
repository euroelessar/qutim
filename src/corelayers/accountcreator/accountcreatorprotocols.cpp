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
		
		connect(m_ui->protocolList, SIGNAL(currentRowChanged(int)),
				SIGNAL(completeChanged()));
		connect(m_ui->protocolList,SIGNAL(currentRowChanged(int)),
				SLOT(onCurrentRowChanged(int)));
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

			QListWidgetItem *item = new QListWidgetItem(icon,
														info.name(),
														m_ui->protocolList);
			item->setData(Qt::UserRole + 1,reinterpret_cast<qptrdiff>(wizard));
			item->setData(Qt::UserRole + 2,qVariantFromValue(info));
			item->setToolTip(info.description());
		}
		setTitle(tr("Select protocol"));
	}

	AccountCreatorProtocols::~AccountCreatorProtocols()
	{
		delete m_ui;
		qDeleteAll(m_wizards);
	}

	bool AccountCreatorProtocols::validatePage()
	{
		QListWidgetItem *item = m_ui->protocolList->currentItem();
		qptrdiff wizardPtr = item->data(Qt::UserRole + 1).value<qptrdiff>();
		AccountCreationWizard *wizard = reinterpret_cast<AccountCreationWizard *>(wizardPtr);
		if (!wizard)
			return false;

		QMap<AccountCreationWizard *, int>::iterator it = ensureCurrentProtocol();
		return it.value() != -1;
	}

	bool AccountCreatorProtocols::isComplete() const
	{
		return m_ui->protocolList->currentIndex().row() != -1;
	}

	int AccountCreatorProtocols::nextId() const
	{
		if (m_ui->protocolList->count() == 0)
			return -1;
		QMap<AccountCreationWizard *, int>::iterator it
				= const_cast<AccountCreatorProtocols *>(this)->ensureCurrentProtocol();
		return (it == m_wizardIds.end()  || it.value() == -1) ? m_lastId + 1 : it.value();
	}

	QMap<AccountCreationWizard *, int>::iterator AccountCreatorProtocols::ensureCurrentProtocol()
	{
		QListWidgetItem *item = m_ui->protocolList->currentItem();

		if (!item)
			return m_wizardIds.end();

		qptrdiff wizardPtr = item->data(Qt::UserRole + 1).value<qptrdiff>();	AccountCreationWizard *wizard = reinterpret_cast<AccountCreationWizard *>(wizardPtr);
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

	void AccountCreatorProtocols::onCurrentRowChanged(int row)
	{
		QListWidgetItem *item = m_ui->protocolList->item(row);
		ExtensionInfo info = item->data(Qt::UserRole+2).value<ExtensionInfo>();
		setSubTitle(info.description());
	}

}
