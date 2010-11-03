#include "accountcreatorprotocols.h"
#include "accountcreatorwizard.h"
#include <qutim/extensioninfo.h>
#include <qutim/icon.h>
#include "qutim/metaobjectbuilder.h"
#include "ui_accountcreatorprotocols.h"
#include <QCommandLinkButton>
#include <QScrollBar>
#include <QDebug>
#ifndef Q_WS_WIN
#include <itemdelegate.h>
#endif

static const int buttonMinimumHeight = 50;

namespace Core
{
AccountCreatorProtocols::AccountCreatorProtocols(QWizard *parent) :
	QWizardPage(parent),
	ui(new Ui::AccountCreatorProtocols),
	m_wizard(parent)
{
	ui->setupUi(this);

#ifndef Q_WS_WIN
	ui->protocolList->setItemDelegate(new ItemDelegate(this));
#else
	m_ui->protocolList->setFrameStyle(QFrame::NoFrame);
#endif

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
		if (!icon.availableSizes().count())
			icon = Icon("applications-internet");

		QListWidgetItem *item = new QListWidgetItem(ui->protocolList);
		item->setData(Qt::UserRole + 1, reinterpret_cast<qptrdiff>(wizard));
		item->setData(Qt::UserRole + 2, qVariantFromValue(info));

#ifdef Q_WS_WIN
		QCommandLinkButton *b = new QCommandLinkButton(info.name(), info.description());
		connect(b, SIGNAL(clicked()), this, SLOT(protocolSelected()));
		b->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Preferred);
		b->setMinimumHeight(buttonMinimumHeight);

		b->setIcon(icon);
		m_ui->protocolList->setItemWidget(item, b);
		item->setSizeHint(b->size());
		item->setFlags(Qt::NoItemFlags);
#else
		item->setIcon(icon);
		item->setText(info.name());
		item->setData(DescriptionRole,info.description());
#endif
	}

	setTitle(tr("Select protocol"));
	connect(ui->protocolList,SIGNAL(itemActivated(QListWidgetItem*)),SLOT(protocolActivated(QListWidgetItem*)));
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

void AccountCreatorProtocols::protocolSelected()
{
#ifdef Q_WS_WIN
	m_ui->protocolList->setCurrentItem(m_items.value(qobject_cast<QCommandLinkButton *>(sender())));
	wizard()->next();
#endif
}

void AccountCreatorProtocols::protocolActivated(QListWidgetItem *)
{
	wizard()->next();
}

}
