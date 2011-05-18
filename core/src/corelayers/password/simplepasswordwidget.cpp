#include "simplepassworddialog.h"
#include "ui_simplepasswordwidget.h"
#include <qutim/protocol.h>

namespace Core
{
SimplePasswordWidget::SimplePasswordWidget(Account *account, SimplePasswordDialog *parent) :
		ui(new Ui::SimplePasswordWidget)
{
	ui->setupUi(this);
	ui->label->setText(ui->label->text().arg(account->id(), account->protocol()->id()));
	connect(this, SIGNAL(accepted()), this, SLOT(onAccept()));
	connect(this, SIGNAL(rejected()), this, SLOT(onReject()));
	m_account = account;
	m_parent = parent;
}

SimplePasswordWidget::~SimplePasswordWidget()
{
    delete ui;
}

void SimplePasswordWidget::setValidator(QValidator *validator)
{
	ui->passwordLineEdit->setValidator(validator);
}

void SimplePasswordWidget::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
		ui->label->setText(ui->label->text().arg(m_account->id(), m_account->protocol()->id()));
        break;
    default:
        break;
    }
}

void SimplePasswordWidget::onAccept()
{
	m_parent->apply(ui->passwordLineEdit->text(), ui->rememberCheckBox->isChecked());
}

void SimplePasswordWidget::onReject()
{
	m_parent->reject();
}
}
