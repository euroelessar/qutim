#ifndef ACCOUNTCREATORPROTOCOLS_H
#define ACCOUNTCREATORPROTOCOLS_H

#include <QtGui/QWizardPage>
#include <qutim/protocol.h>

using namespace qutim_sdk_0_3;

namespace Ui {
class AccountCreatorProtocols;
}

class QListWidgetItem;
class QCommandLinkButton;

namespace Core
{
class AccountCreatorWizard;

class AccountCreatorProtocols : public QWizardPage
{
	Q_OBJECT
public:
	enum { Id = 1 };
	AccountCreatorProtocols(QWizard *parent = 0);
	~AccountCreatorProtocols();
	virtual bool validatePage();
	virtual bool isComplete() const;
	virtual int nextId() const;
public slots:
	void protocolSelected();
	void protocolActivated(QListWidgetItem*);
protected:
	QMap<AccountCreationWizard *, int>::iterator ensureCurrentProtocol();
	void changeEvent(QEvent *e);
private:
	Ui::AccountCreatorProtocols *ui;
	QWizard *m_wizard;
	QMultiMap<QString, AccountCreationWizard *> m_wizards;
	QMap<AccountCreationWizard *, int> m_wizardIds;
#ifdef Q_WS_WIN
	QHash<QCommandLinkButton *, QListWidgetItem *> m_items;
#endif
	int m_lastId;
};
}

#endif // ACCOUNTCREATORPROTOCOLS_H
