#ifndef QUETZALACCOUNTWIZARD_H
#define QUETZALACCOUNTWIZARD_H

#include <qutim/protocol.h>

using namespace qutim_sdk_0_3;

class QuetzalMetaObject;
class QuetzalProtocol;

class QuetzalAccountWizard : public AccountCreationWizard
{
	Q_OBJECT
public:
	QuetzalAccountWizard(const QuetzalMetaObject *proto);
	virtual QList<QWizardPage *> createPages(QWidget *parent);
private:
	QuetzalMetaObject *meta;
	QuetzalProtocol *m_proto;
};

#endif // QUETZALACCOUNTWIZARD_H
