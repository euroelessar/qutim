#ifndef ACCOUNTCREATORWIZARD_H
#define ACCOUNTCREATORWIZARD_H

#include <QWizard>
#include "libqutim/protocol.h"
#include "libqutim/account.h"

class AccountCreatorWizard : public QWizard
{
	Q_OBJECT
public:
    AccountCreatorWizard();
};

#endif // ACCOUNTCREATORWIZARD_H
