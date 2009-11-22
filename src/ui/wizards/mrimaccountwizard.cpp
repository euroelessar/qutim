#include "mrimaccountwizard.h"
#include "accountwizardmain.h"

struct MrimAccountWizardPrivate
{

};

MrimAccountWizard::MrimAccountWizard(Protocol *protocol)
        : AccountCreationWizard(protocol), p(new MrimAccountWizardPrivate)
{    
}

QList<QWizardPage*> MrimAccountWizard::createPages(QWidget *parent)
{
    QList<QWizardPage*> pages;
    pages.append(new AccountWizardMain(parent));
    return pages;
}

MrimAccountWizard::~MrimAccountWizard()
{
}
