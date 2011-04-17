#include "accountcreatorlist.h"
#include <qutim/icon.h>
#include <qutim/protocol.h>
#include <qutim/account.h>
#include <qutim/settingslayer.h>
#include <qutim/servicemanager.h>
#include "ui_accountcreatorlist.h"
#include <QListWidgetItem>
#include <QContextMenuEvent>
#include "itemdelegate.h"
#include <QMessageBox>
#include <qutim/debug.h>
#include <QHBoxLayout>
#include <QToolButton>

namespace Core
{
AccountCreatorList::AccountCreatorList() :
	SettingsWidget(),
	ui(new Ui::AccountCreatorList)
{
	ui->setupUi(this);

	ui->listWidget->installEventFilter(this);
	ItemDelegate *delegate = new ItemDelegate(this);
	delegate->setUserDefinedEditorSupport();
	ui->listWidget->setItemDelegate(delegate);

#ifdef Q_WS_S60
	int width = style()->pixelMetric(QStyle::QStyle::PM_ListViewIconSize);
#elif defined(Q_WS_WIN32)
	int width = 22;
#else
	int width = style()->pixelMetric(QStyle::QStyle::PM_ToolBarIconSize);
#endif

	QSize size = QSize(width, width);
	ui->listWidget->setIconSize(size);

	connect(ui->listWidget,
			SIGNAL(itemActivated(QListWidgetItem*)),
			SLOT(listViewClicked(QListWidgetItem*))
			);
	QListWidgetItem *addItem = new QListWidgetItem(ui->listWidget);
	addItem->setText(QT_TRANSLATE_NOOP("Account","Add new account"));
	addItem->setToolTip(QT_TRANSLATE_NOOP("Account","Just add or create new account"));
	addItem->setData(DescriptionRole,addItem->toolTip());
	addItem->setIcon(Icon("list-add-user"));

	addItem = new QListWidgetItem(ui->listWidget);
	addItem->setText(QT_TRANSLATE_NOOP("Account","Accounts"));
	addItem->setData(SeparatorRole,true);

	foreach(Protocol *protocol, Protocol::all()) {
		connect(protocol,SIGNAL(accountCreated(qutim_sdk_0_3::Account*)),SLOT(addAccount(qutim_sdk_0_3::Account*)));
		connect(protocol,SIGNAL(accountRemoved(qutim_sdk_0_3::Account*)),SLOT(removeAccount(qutim_sdk_0_3::Account*)));
		foreach(Account *account, protocol->accounts())	{
			addAccount(account);
		}
	}

}

AccountCreatorList::~AccountCreatorList()
{
	delete ui;
}

void AccountCreatorList::loadImpl()
{
}

void AccountCreatorList::saveImpl()
{
}

void AccountCreatorList::cancelImpl()
{
}

void AccountCreatorList::onWizardDestroyed()
{
	QWidget *parent = window();
	if (parent)
		window()->setEnabled(true);
}

void AccountCreatorList::changeEvent(QEvent *e)
{
	QWidget::changeEvent(e);
	switch (e->type())
	{
	case QEvent::LanguageChange:
		ui->retranslateUi(this);
		break;
	default:
		break;
	}
}

void AccountCreatorList::addAccount(qutim_sdk_0_3::Account *account)
{
	Icon protoIcon(QLatin1String("im-") + account->protocol()->id()); //FIXME wtf?
	debug() << protoIcon.availableSizes() << QLatin1String("im-") + account->protocol()->id();
	if (!protoIcon.actualSize(QSize(1,1)).isValid())
		protoIcon = Icon("applications-internet");

	QListWidgetItem *accountItem = new QListWidgetItem(ui->listWidget);
	accountItem->setText(account->name());
	accountItem->setToolTip(account->name());
	accountItem->setIcon(protoIcon);
	accountItem->setData(Qt::UserRole,qVariantFromValue<Account *>(account));

	QWidget *buttons = new QWidget(this);

	QHBoxLayout *l = new QHBoxLayout(buttons);
	l->setMargin(0);
	l->setSpacing(0);

	QToolButton *btn = new QToolButton(buttons);
	btn->setText(tr("Properties"));
	btn->setToolTip(tr("Account settings"));
	btn->setIcon(Icon("document-properties"));
	btn->setProperty("account",qVariantFromValue(account));
	connect(btn,SIGNAL(clicked()),SLOT(onAccountPropertiesTriggered()));
	l->addWidget(btn);

	btn = new QToolButton(buttons);
	btn->setText(tr("Remove account"));
	btn->setIcon(Icon("list-remove-user"));
	btn->setToolTip(tr("Remove account"));
	btn->setProperty("account",qVariantFromValue(account));
	connect(btn,SIGNAL(clicked()),SLOT(onAccountRemoveTriggered()));
	l->addWidget(btn);

	ui->listWidget->setItemWidget(accountItem,buttons);

	QVariantMap fields;
	QString id = account->protocol()->data(Protocol::ProtocolIdName).toString();
	fields.insert(id,account->id());
	accountItem->setData(DescriptionRole,fields);
}

void AccountCreatorList::removeAccount(qutim_sdk_0_3::Account *removed)
{
	for (int index = 0; index != ui->listWidget->count();index++) {
		QListWidgetItem *item = ui->listWidget->item(index);
		Account *account = item->data(Qt::UserRole).value<Account*>();
		if (account == removed) {
			debug() << "removed account";
			delete item;
			return;
		}
	}
}

void AccountCreatorList::listViewClicked(QListWidgetItem *item)
{
	if (item->data(SeparatorRole).toBool() || item->data(Qt::UserRole).value<Account *>())
		return;

	if (!m_wizard.isNull())
		return;
	QWidget *parent = window();
	if (parent)
		window()->setEnabled(false);
	AccountCreatorWizard *wizard = new AccountCreatorWizard();
	connect(wizard,SIGNAL(destroyed()),SLOT(onWizardDestroyed()));
#if defined(QUTIM_MOBILE_UI)
	wizard->showMaximized();
#else
	centerizeWidget(wizard);
	wizard->show();
	wizard->raise();
#endif
}

void AccountCreatorList::onAccountRemoveTriggered()
{
	Account *account = sender()->property("account").value<Account*>();
	if (!account)
		return;

	int ret = QMessageBox::question(this, tr("Remove account"),
									tr("Are you sure want to remove %1").arg(account->name()),
									QMessageBox::Yes | QMessageBox::No,
									QMessageBox::No);
	if (ret == QMessageBox::Yes)
		account->protocol()->removeAccount(account);
}

void AccountCreatorList::onAccountPropertiesTriggered()
{
	Account *account = sender()->property("account").value<Account*>();
	if (!account)
		return;
	SettingsLayer *layer = ServiceManager::getByName<SettingsLayer*>("SettingsLayer");
	layer->show(account);
}
}
