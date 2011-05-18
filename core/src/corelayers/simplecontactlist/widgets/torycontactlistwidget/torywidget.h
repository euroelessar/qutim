#ifndef TORYWIDGET_H
#define TORYWIDGET_H
#include <QMainWindow>
#include <abstractcontactlist.h>
#include <simplecontactlistview.h>
#include <qutim/status.h>

namespace qutim_sdk_0_3
{
class Account;
class Contact;
class ChatSession;
}

class QLineEdit;


namespace Core {
namespace SimpleContactList {
struct ToryWidgetPrivate;

class ToryWidget : public QMainWindow, public AbstractContactListWidget
{
	Q_OBJECT
	Q_INTERFACES(Core::SimpleContactList::AbstractContactListWidget)
	Q_CLASSINFO("Uses", "ContactDelegate")
	Q_CLASSINFO("Uses", "ContactModel")
	Q_CLASSINFO("Service", "ContactListWidget")
	Q_DECLARE_PRIVATE(ToryWidget)
public:
	ToryWidget();
	~ToryWidget();
	virtual void addButton(ActionGenerator *generator);
	virtual void removeButton(ActionGenerator *generator);
	virtual TreeView *contactView();
private slots:
	void showStatusDialog();
	void changeStatusTextAccepted();
	void onAccountCreated(qutim_sdk_0_3::Account *account);
	void onAccountStatusChanged(const qutim_sdk_0_3::Status &status);
	void onAccountDestroyed(QObject *obj);
	void onSessionCreated(qutim_sdk_0_3::ChatSession*);
	void onSessionDestroyed();
	void onActivatedSession(bool state);
	void onSearchButtonToggled(bool toggled);
	void onStatusChanged();
	void initMenu();
	void onServiceChanged(const QByteArray &name, QObject *newObject, QObject *oldObject);
protected:
	void loadGeometry();
	QAction *createGlobalStatus(Status::Type type);
	bool eventFilter(QObject *obj, QEvent *event);
private:
	QScopedPointer<ToryWidgetPrivate> d_ptr;
};

} // namespace SimpleContactList
} // namespace Core

#endif // TORYWIDGET_H
