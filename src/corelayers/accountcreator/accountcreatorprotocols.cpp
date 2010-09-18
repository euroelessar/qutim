#include "accountcreatorprotocols.h"
#include "accountcreatorwizard.h"
#include <qutim/extensioninfo.h>
#include <qutim/icon.h>
#include "qutim/metaobjectbuilder.h"
#include "ui_accountcreatorprotocols.h"
#include <QCommandLinkButton>
#include <QScrollBar>
#include <QDebug>

static const int buttonMinimumHeight = 50;

namespace Core
{
	AccountCreatorProtocols::AccountCreatorProtocols(QWizard *parent) :
		QWizardPage(parent),
		m_ui(new Ui::AccountCreatorProtocols),
		m_wizard(parent)
	{
		m_ui->setupUi(this);
		
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

		m_ui->protocolList->setFrameStyle(QFrame::NoFrame);
		foreach (AccountCreationWizard *wizard, m_wizards) {
			ExtensionInfo info = wizard->info();
			QIcon icon = info.icon();
			if (icon.isNull())
				icon = Icon(QLatin1String("im-") + info.name());

			QListWidgetItem *item = new QListWidgetItem(m_ui->protocolList);
			item->setData(Qt::UserRole + 1, reinterpret_cast<qptrdiff>(wizard));
			item->setData(Qt::UserRole + 2, qVariantFromValue(info));
			item->setFlags(Qt::NoItemFlags);

			QCommandLinkButton *b = new QCommandLinkButton(info.name(), info.description());
			connect(b, SIGNAL(clicked()), this, SLOT(protocolSelected()));
			b->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Preferred);
			b->setMinimumHeight(buttonMinimumHeight);
			if (icon.availableSizes().count())
				b->setIcon(icon);
			m_ui->protocolList->setItemWidget(item, b);
			item->setSizeHint(b->size());
			m_items.insert(b, item);
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
		return true;
	}

	bool AccountCreatorProtocols::isComplete() const
	{
		return false;
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
				m_ui->retranslateUi(this);
				break;
			default:
				break;
		}
	}

	void AccountCreatorProtocols::protocolSelected()
	{
		m_ui->protocolList->setCurrentItem(m_items.value(qobject_cast<QCommandLinkButton *>(sender())));
		wizard()->next();
	}
}
