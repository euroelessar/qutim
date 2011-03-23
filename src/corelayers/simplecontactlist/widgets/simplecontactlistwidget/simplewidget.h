#ifndef SIMPLEWIDGET_H
#define SIMPLEWIDGET_H
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

class SimpleWidget : public QMainWindow, public AbstractContactListWidget
{
	Q_OBJECT
	Q_INTERFACES(Core::SimpleContactList::AbstractContactListWidget)
	Q_CLASSINFO("Uses", "ContactDelegate")
	Q_CLASSINFO("Uses", "ContactModel")
	Q_CLASSINFO("Service", "ContactListWidget")
public:
	SimpleWidget();
	~SimpleWidget();
	void loadGeometry();
	virtual void addButton(ActionGenerator *generator);
	virtual void removeButton(ActionGenerator *generator);
protected:
	QAction *createGlobalStatusAction(Status::Type type);
	bool event(QEvent *event);
private slots:
	void init();
	void onAccountCreated(qutim_sdk_0_3::Account *account);
	void onAccountStatusChanged(const qutim_sdk_0_3::Status &status);
	void onAccountDestroyed(QObject *obj);
	void onStatusChanged();
	void onSearchButtonToggled(bool toggled);
	void showStatusDialog();
	void changeStatusTextAccepted();
	void orientationChanged();
private:
	TreeView *m_view;
	AbstractContactModel *m_model;
	ActionToolBar *m_mainToolBar;
	QPushButton *m_statusBtn;
	QPushButton *m_searchBtn;
	QLineEdit *m_searchBar;
	QHash<Account *, QAction *> m_actions;
	QAction *m_status_action;
	QList<QAction *> m_statusActions;
};

} // namespace SimpleContactList
} // namespace Core

#endif // SIMPLEWIDGET_H
