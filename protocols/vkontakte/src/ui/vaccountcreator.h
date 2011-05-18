#ifndef VACCOUNTCREATOR_H
#define VACCOUNTCREATOR_H
#include <QWizardPage>
#include "vkontakte_global.h"
#include <qutim/protocol.h>

class VkontakteProtocol;
class VAccountWizardPage;
class VAccountCreator : public AccountCreationWizard
{
	Q_OBJECT
	Q_CLASSINFO("DependsOn", "VkontakteProtocol")
public:
	VAccountCreator();
	virtual ~VAccountCreator();
	virtual QList< QWizardPage* > createPages(QWidget* parent);
	void finished();
private:
	VAccountWizardPage *page;
	VkontakteProtocol *protocol;
};

#endif // VACCOUNTCREATOR_H
