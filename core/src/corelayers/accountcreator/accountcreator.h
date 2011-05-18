#ifndef ACCOUNTCREATOR_H
#define ACCOUNTCREATOR_H

#include <qutim/profilecreatorpage.h>
#include <qutim/plugin.h>

using namespace qutim_sdk_0_3;

namespace Core
{
class AccountCreator : public Plugin
{
	Q_OBJECT
public:
	virtual void init();
	virtual bool load();
	virtual bool unload();
	virtual ~AccountCreator();
private slots:
	void showWizard();
};

class AccountPageCreator : public ProfileCreatorPage
{
	Q_OBJECT
public:
	explicit AccountPageCreator(QObject *parent = 0);
	virtual double priority() const;
	virtual QList<QWizardPage *> pages(QWidget *parent);
};
}

#endif // ACCOUNTCREATOR_H
