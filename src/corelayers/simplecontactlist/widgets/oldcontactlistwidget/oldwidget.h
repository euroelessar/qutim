#ifndef OLDWIDGET_H
#define OLDWIDGET_H
#include <QMainWindow>
#include <abstractcontactlist.h>
#include <simplecontactlistview.h>
#include <qutim/status.h>

namespace qutim_sdk_0_3
{
class Account;
class Contact;
}

class QPushButton;
class QLineEdit;
class QAction;
namespace Core {
namespace SimpleContactList {

class OldWidget : public QMainWindow, public AbstractContactListWidget
{
	Q_OBJECT
	Q_INTERFACES(Core::SimpleContactList::AbstractContactListWidget)
	Q_CLASSINFO("Service", "ContactListWidget")
	Q_CLASSINFO("Uses", "ContactDelegate")
	Q_CLASSINFO("Uses", "ContactModel")
public:
	OldWidget();
	~OldWidget();
	void loadGeometry();
	virtual void addButton(ActionGenerator *generator);
	virtual void removeButton(ActionGenerator *generator);
protected:
	bool event(QEvent *event);
private slots:
	void init();
	void onAccountCreated(qutim_sdk_0_3::Account *account);
	void onAccountStatusChanged(const qutim_sdk_0_3::Status &status);
	void showStatusDialog();
	void changeStatusTextAccepted();
	void onAccountDestroyed(QObject *obj);
private:
	TreeView *m_view;
	AbstractContactModel *m_model;
	ActionToolBar *m_mainToolBar;
	ActionToolBar *m_statusToolBar;
	QLineEdit *m_searchBar;
	QHash<Account*, QAction*> m_actions;
};

} // namespace SimpleContactList
} // namespace Core

#endif // OLDWIDGET_H
