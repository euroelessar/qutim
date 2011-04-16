#ifndef ADDCONTACT_H
#define ADDCONTACT_H
#include <QScopedPointer>
#include <QDialog>
#include <qutim/account.h>
#include <qutim/status.h>

namespace Core {
using namespace qutim_sdk_0_3;
class AddContactPrivate;

class AddContact : public QDialog
{
	Q_OBJECT
	Q_DECLARE_PRIVATE(AddContact)
public:
	AddContact(Account *account = 0, QWidget *parent = 0);
	~AddContact();
	
	void setContactId(const QString &id);
	void setContactName(const QString &name);
	void setContactTags(const QStringList &tags);
protected:
	void setAccount(Account *account);
	void changeState(Account *account, const qutim_sdk_0_3::Status &status);
private slots:
	void on_okButton_clicked();
	void on_cancelButton_clicked();
	void onStartChatClicked();
	void onShowInfoClicked();
	void setAccount();
	void changeState(const qutim_sdk_0_3::Status &status);
	void currentChanged(int index);
private:
	QScopedPointer<AddContactPrivate> d_ptr;
};

class AddContactModule : public QObject
{
	Q_OBJECT
	Q_CLASSINFO("Service", "AddContact")
	Q_CLASSINFO("Uses", "IconLoader")
	Q_CLASSINFO("Uses", "ContactList")
public:
	AddContactModule();
	~AddContactModule();
public slots:
	void show(qutim_sdk_0_3::Account *account, const QString &id = QString(),
	          const QString &name = QString(), const QStringList &tags = QStringList());
private slots:
	void show();
private:
	QScopedPointer<ActionGenerator> m_addUserGen;
};
}
#endif // ADDCONTACT_H
