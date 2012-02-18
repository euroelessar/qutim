/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Aleksey Sidorov <gorthauer87@yandex.ru>
**
*****************************************************************************
**
** $QUTIM_BEGIN_LICENSE$
** This program is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
** See the GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program.  If not, see http://www.gnu.org/licenses/.
** $QUTIM_END_LICENSE$
**
****************************************************************************/

#ifndef SEVENWIDGET_H
#define SEVENWIDGET_H
#include <QMainWindow>
#include <qutim/simplecontactlist/abstractcontactlist.h>
#include <qutim/simplecontactlist/simplecontactlistview.h>
#include <qutim/status.h>

namespace qutim_sdk_0_3
{
class Account;
class Contact;
}

class ToolFrameWindow;
class QPushButton;
class QLineEdit;
class QAction;
namespace Core {
namespace SimpleContactList {

class SevenWidget : public QWidget, public AbstractContactListWidget
{
	Q_OBJECT
	Q_INTERFACES(Core::SimpleContactList::AbstractContactListWidget)
	Q_CLASSINFO("Service", "ContactListWidget")
	Q_CLASSINFO("Uses", "ContactDelegate")
	Q_CLASSINFO("Uses", "ContactModel")
public:
	SevenWidget();
	~SevenWidget();
	void loadGeometry();
	virtual void addButton(ActionGenerator *generator);
	virtual void removeButton(ActionGenerator *generator);
	AbstractContactModel *model() const;
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
	void save();
private:
	TreeView *m_view;
	AbstractContactModel *m_model;
	QPushButton *m_statusBtn;
	QLineEdit *m_searchBar;
	QHash<Account *, QAction *> m_actions;
	QAction *m_status_action;
	QList<QAction *> m_statusActions;
	ToolFrameWindow *m_toolFrameWindow;
	MenuController *m_controller;
};

} // namespace SimpleContactList
} // namespace Core

#endif // SEVENWIDGET_H

