#include "ircconsole.h"
#include "ui_console.h"
#include "ircaccount.h"

namespace qutim_sdk_0_3 {

namespace irc {

IrcConsoleFrom::IrcConsoleFrom(IrcAccount *account, const QString &log, QWidget *parent) :
    QWidget(parent),
	ui(new Ui::IrcConsoleForm),
	m_account(account)
{
    ui->setupUi(this);
	ui->consoleWidget->setHtml(log);
	connect(ui->cmdEdit, SIGNAL(returnPressed()), SLOT(sendCommand()));
	connect(account, SIGNAL(destroyed()), SLOT(deleteLater()));
}

IrcConsoleFrom::~IrcConsoleFrom()
{
    delete ui;
}

void IrcConsoleFrom::appendMessage(const QString &msg)
{
	ui->consoleWidget->append(msg);
}

void IrcConsoleFrom::sendCommand()
{
	m_account->log(ui->cmdEdit->text());
	m_account->send(ui->cmdEdit->text(), IrcCommandAlias::Console);
	ui->cmdEdit->clear();
}

void IrcConsoleFrom::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

} } // namespace qutim_sdk_0_3::irc
