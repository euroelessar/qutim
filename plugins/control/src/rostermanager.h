/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2012 Ruslan Nigmatullin <euroelessar@yandex.ru>
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

#ifndef CONTROL_ROSTERMANAGER_H
#define CONTROL_ROSTERMANAGER_H

#include <qutim/rosterstorage.h>
#include <qutim/messagehandler.h>
#include "networkmanager.h"
#include "settingswidget.h"
#include <qutim/settingslayer.h>

namespace Control {

class RosterManager : public QObject, public Ureen::MessageHandler
{
	Q_OBJECT
public:
	explicit RosterManager();
	~RosterManager();
	
	virtual bool event(QEvent *);
	
	static RosterManager *instance();
	
	int accountId(const QString &protocol, const QString &id) const;
	void setAccountId(const QString &protocol, const QString &id, int pid);
	Ureen::MessageList messages(Ureen::ChatUnit *unit);
	NetworkManager *networkManager();
	
protected slots:
	void loadSettings(bool init = false);
	void onAccountCreated(Ureen::Account *account);
	void onAccountRemoved(Ureen::Account *account);
	void onContactCreated(Ureen::Contact *contact);
	void onContactUpdated();
	void onContactRemoved(Ureen::Contact *contact);
	void onContactInListChanged(bool inList);
	void onAutoReplyClicked(QAction *action, QObject *object);
	void onQuickAnswerClicked(QObject *object);
	void onGroupsClicked(QAction *action);
	
protected:
	void connectAccount(Ureen::Account *account);
	void connectContact(Ureen::Contact *contact);
	virtual Result doHandle(Ureen::Message &message, QString *reason);
	void onStarted();
	
private:
	struct AccountContext
	{
		AccountContext() : id(-1), created(false) {}
		int id;
		bool created;
		QMap<Ureen::Contact*, int> indexes;
		QList<int> freeIndexes;
	};
	static RosterManager *self;
	QScopedPointer<Ureen::ActionGenerator> m_autoReplyGenerator;
	QScopedPointer<Ureen::ActionGenerator> m_quickAnswerGenerator;
	QScopedPointer<Ureen::ActionGenerator> m_groupGenerator;
	Ureen::SettingsItem *m_settingsItem;
	NetworkManager *m_manager;
	QVariantList m_actions;
	QMap<Ureen::Account*, AccountContext> m_contexts;
};

} // namespace Control

#endif // CONTROL_ROSTERMANAGER_H
