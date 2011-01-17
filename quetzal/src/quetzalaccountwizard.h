#ifndef QUETZALACCOUNTWIZARD_H
#define QUETZALACCOUNTWIZARD_H

#include <qutim/protocol.h>
#include <qutim/dataforms.h>
#include <QWizardPage>
#include <QLabel>
#include <purple.h>

class QuetzalMetaObject;
class QuetzalProtocol;
class QuetzalAccountSettings;

class QuetzalAccountWizardPage : public QWizardPage
{
	Q_OBJECT
public:
	QuetzalAccountWizardPage(QuetzalProtocol *proto, QWidget *parent = 0);
    void initializePage();
    void cleanupPage();
	bool validatePage();
	bool isComplete() const;
	void handleRegisterResult(PurpleAccount *account, bool result);
protected slots:
	void onRegisterButtonClicked();
	void onDataChanged(const QString &name, const QVariant &data);
private:
	PurpleAccount *createAccount();
	bool m_isUsernameFilled;
	QuetzalProtocol *m_proto;
	PurpleAccount *m_account;
	qutim_sdk_0_3::AbstractDataForm *m_generalWidget;
	QuetzalAccountSettings *m_settingsWidget;
	QAbstractButton *m_registerButton;
	bool m_isRegistering;
};

class QuetzalAccountWizard : public qutim_sdk_0_3::AccountCreationWizard
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
