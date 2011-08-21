#include "accountwizardmain.h"
#include "ui_accountwizardmain.h"
#include "../../base/mrimprotocol.h"

AccountWizardMain::AccountWizardMain(QWidget *parent) :
    QWizardPage(parent),
    ui(new Ui::AccountWizardMain)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose, true);

    registerField("email*", ui->emailEdit);
    registerField("pass*", ui->passEdit);
}

AccountWizardMain::~AccountWizardMain()
{
    delete ui;
}

void AccountWizardMain::changeEvent(QEvent *e)
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

bool AccountWizardMain::validatePage()
{
    QWizardPage::validatePage();

    bool ret = false;

    if (isFinalPage())
    {
	QString email = field("email").value<QString>().toLower().trimmed() + ui->domainComboBox->currentText();
        MrimProtocol::AccountCreationError err = MrimProtocol::instance()->createAccount(email,field("pass").value<QString>());

        if (err == MrimProtocol::None)
        {
            ret = true;
        }
        else
        {
        //TODO: show message box with error
        }
    }
    return ret;
}
