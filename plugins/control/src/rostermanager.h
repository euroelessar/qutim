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

class RosterManager : public QObject, public qutim_sdk_0_3::MessageHandler
{
	Q_OBJECT
public:
	explicit RosterManager();
	~RosterManager();
	
	virtual bool event(QEvent *);
	
	static RosterManager *instance();
	
	int accountId(const QString &protocol, const QString &id) const;
	void setAccountId(const QString &protocol, const QString &id, int pid);
	qutim_sdk_0_3::MessageList messages(qutim_sdk_0_3::ChatUnit *unit);
	NetworkManager *networkManager();
	
protected slots:
	void loadSettings(bool init = false);
	void onAccountCreated(qutim_sdk_0_3::Account *account);
	void onAccountRemoved(qutim_sdk_0_3::Account *account);
	void onContactCreated(qutim_sdk_0_3::Contact *contact);
	void onContactUpdated();
	void onContactRemoved(qutim_sdk_0_3::Contact *contact);
	void onContactInListChanged(bool inList);
	void onAutoReplyClicked(QAction *action, QObject *object);
	void onQuickAnswerClicked(QObject *object);
	void onGroupsClicked(QAction *action);
	
protected:
	void connectAccount(qutim_sdk_0_3::Account *account);
	void connectContact(qutim_sdk_0_3::Contact *contact);
	virtual Result doHandle(qutim_sdk_0_3::Message &message, QString *reason);
	void onStarted();
	
private:
	struct AccountContext
	{
		AccountContext() : id(-1), created(false) {}
		int id;
		bool created;
		QMap<qutim_sdk_0_3::Contact*, int> indexes;
		QList<int> freeIndexes;
	};
	static RosterManager *self;
	QScopedPointer<qutim_sdk_0_3::ActionGenerator> m_autoReplyGenerator;
	QScopedPointer<qutim_sdk_0_3::ActionGenerator> m_quickAnswerGenerator;
	QScopedPointer<qutim_sdk_0_3::ActionGenerator> m_groupGenerator;
	qutim_sdk_0_3::SettingsItem *m_settingsItem;
	NetworkManager *m_manager;
	QVariantList m_actions;
	QMap<qutim_sdk_0_3::Account*, AccountContext> m_contexts;
};

} // namespace Control

#endif // CONTROL_ROSTERMANAGER_H
