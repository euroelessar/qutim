#include "accountcreatorlist.h"
#include "libqutim/icon.h"
#include "libqutim/protocol.h"
#include "libqutim/account.h"
#include "ui_accountcreatorlist.h"

namespace Core
{
	AccountCreatorList::AccountCreatorList() :
		SettingsWidget(),
		m_ui(new Ui::AccountCreatorList)
	{
		m_ui->setupUi(this);
		m_ui->addButton->setIcon(Icon("list-add-user"));
		m_ui->upButton->setIcon(Icon("go-up"));
		m_ui->downButton->setIcon(Icon("go-down"));
		Icon removeIcon("list-remove-user");
		int num = 0;
		foreach(Protocol *protocol, allProtocols())
		{
			Icon protoIcon(QLatin1String("im-") + protocol->id());
			foreach(Account *account, protocol->accounts())
			{
				QTableWidgetItem *accountItem = new QTableWidgetItem(protoIcon, account->id());
				QTableWidgetItem *removeItem = new QTableWidgetItem;
				removeItem->setIcon(removeIcon);
				removeItem->setToolTip(tr("Remove account"));
				m_ui->tableWidget->insertRow(num);
				m_ui->tableWidget->setItem(num, 0, accountItem);
				m_ui->tableWidget->setItem(num, 1, removeItem);
				num++;
			}
		}
	}

	AccountCreatorList::~AccountCreatorList()
	{
		delete m_ui;
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

	void AccountCreatorList::on_addButton_clicked()
	{
		if (!m_wizard.isNull())
			return;
		QWidget *parent = window();
		if (parent)
			window()->setEnabled(false);
		AccountCreatorWizard *wizard = new AccountCreatorWizard();
		connect(wizard, SIGNAL(destroyed()), this, SLOT(on_wizard_destroyed()));
		wizard->show();
	}

	void AccountCreatorList::on_wizard_destroyed()
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
			m_ui->retranslateUi(this);
			break;
		default:
			break;
		}
	}
}
