#ifndef ADDCONTACT_H
#define ADDCONTACT_H
#include <QScopedPointer>
#include <QDialog>
#include <libqutim/account.h>

namespace Core {
	using namespace qutim_sdk_0_3;
	struct AddContactPrivate;

	class AddContact : public QDialog
	{
		Q_OBJECT
		Q_DECLARE_PRIVATE(AddContact)
	public:
				AddContact(Account *account = 0, QWidget *parent = 0);
		~AddContact();
	protected:
		void setAccount(Account *account);
		void changeState(Account *account, const qutim_sdk_0_3::Status &status);
	private slots:
		void on_okButton_clicked();
		void on_cancelButton_clicked();
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
	protected:
		bool event(QEvent *ev);
	private slots:
		void show();
	};
}
#endif // ADDCONTACT_H
