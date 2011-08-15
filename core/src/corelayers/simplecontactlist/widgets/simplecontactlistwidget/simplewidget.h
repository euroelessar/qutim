#ifndef SIMPLEWIDGET_H
#define SIMPLEWIDGET_H
#include <QMainWindow>
#include <abstractcontactlist.h>
#include <simplecontactlistview.h>
#include <qutim/servicemanager.h>
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
	Q_CLASSINFO("Service", "ContactListWidget")
	Q_CLASSINFO("Uses", "ContactDelegate")
	Q_CLASSINFO("Uses", "ContactModel")
	Q_CLASSINFO("SettingsDescription", "Default style")
public:
	SimpleWidget();
	~SimpleWidget();
	void loadGeometry();
	virtual void addButton(ActionGenerator *generator);
	virtual void removeButton(ActionGenerator *generator);
	virtual TreeView *contactView();
	AbstractContactModel *model() const;
protected:
	QAction *createGlobalStatusAction(Status::Type type);
	bool event(QEvent *event);
	bool eventFilter(QObject *, QEvent *);
private slots:
	void onServiceChanged(const QByteArray &name, QObject *now, QObject *old);
	void init();
	void onAccountCreated(qutim_sdk_0_3::Account *account);
	void onAccountStatusChanged(const qutim_sdk_0_3::Status &status);
	void onAccountDestroyed(QObject *obj);
	void onStatusChanged();
	void onSearchActivated();
	void onTextChanged(const QString &text);
	void showStatusDialog();
	void changeStatusTextAccepted();
#ifdef Q_WS_MAEMO_5
	void orientationChanged();
#endif
private:
	TreeView *m_view;
	ServicePointer<AbstractContactModel> m_model;
	ActionToolBar *m_mainToolBar;
	QPushButton *m_statusBtn;
	QLineEdit *m_searchBar;
	QHash<Account *, QAction *> m_actions;
	QAction *m_status_action;
	QList<QAction *> m_statusActions;
	QString m_pressedKeys;
};

} // namespace SimpleContactList
} // namespace Core

#endif // SIMPLEWIDGET_H
