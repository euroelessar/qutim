#include "account_selection.h"
#include "ui_account_selection.h"

#include <qutim/account.h>

using namespace qutim_sdk_0_3;

AccSelectionDlg::AccSelectionDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AccSelectionDlg)
{
    ui->setupUi(this);
}

AccSelectionDlg::~AccSelectionDlg()
{
    delete ui;
}

void AccSelectionDlg::setQuestion(const QString &q)
{
	ui->question->setText(q);
}

void AccSelectionDlg::setDescription(const QString &d)
{
	ui->description->setText(d);
}

Account* AccSelectionDlg::selectedAcc()
{
	return ui->accounts->itemData(ui->accounts->currentIndex()).value<Account*>();
}

void AccSelectionDlg::setAccsList(QList<qutim_sdk_0_3::Account *> accounts)
{
	foreach (Account *acc, accounts) {
		ui->accounts->addItem(QString("%1 <%2>").arg(acc->name(), acc->id()), QVariant::fromValue(acc));
	}
}
