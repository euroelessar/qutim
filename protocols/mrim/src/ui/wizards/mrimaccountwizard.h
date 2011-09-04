#ifndef MRIMACCOUNTWIZARD_H
#define MRIMACCOUNTWIZARD_H

#include "../../base/mrimprotocol.h"

using namespace qutim_sdk_0_3;

struct MrimAccountWizardPrivate;

class MrimAccountWizard : public AccountCreationWizard
{
Q_OBJECT
Q_CLASSINFO("DependsOn", "MrimProtocol")
public:
	MrimAccountWizard();
    QList<QWizardPage*> createPages(QWidget *parent);
    ~MrimAccountWizard();

private:
    QScopedPointer<MrimAccountWizardPrivate> p;
};

#endif // MRIMACCOUNTWIZARD_H
