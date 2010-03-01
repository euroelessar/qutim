#include "simplepasswordwidget.h"
#include "ui_simplepasswordwidget.h"
#include "libqutim/protocol.h"

namespace Core
{
SimplePasswordWidget::SimplePasswordWidget(Account *account) :
		ui(new Ui::SimplePasswordWidget)
{
    ui->setupUi(this);
	ui->label->setText(ui->label->text().arg(account->id(), account->protocol()->id()));
	connect(this, SIGNAL(accepted()), this, SLOT(onAccept()));
	m_account = account;
}

SimplePasswordWidget::~SimplePasswordWidget()
{
    delete ui;
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
	emit entered(ui->passwordLineEdit->text(), ui->rememberCheckBox->isChecked());
}
}
