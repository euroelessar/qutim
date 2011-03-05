#ifndef ACCOUNTCREATORWIZARD_H
#define ACCOUNTCREATORWIZARD_H

#include <QWizard>
#include "accountcreatorprotocols.h"

using namespace qutim_sdk_0_3;

namespace Core
{
class AccountCreatorWizard : public QWizard
{
	Q_OBJECT
public:
	AccountCreatorWizard();
};
}

#endif // ACCOUNTCREATORWIZARD_H
