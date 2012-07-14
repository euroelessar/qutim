/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Alexey Prokhin <alexey.prokhin@yandex.ru>
** Copyright © 2011 Evgeniy Degtyarev <degtep@gmail.com>
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

#ifndef JOINGROUPCHATWRAPPER_H
#define JOINGROUPCHATWRAPPER_H

#include <QDialog>
#include <QDeclarativeListProperty>
#include <qutim/actiongenerator.h>
#include <qutim/dataforms.h>
#include "quickjoingroupchat.h"
#include "bookmarksmodel.h"

class QModelIndex;

namespace qutim_sdk_0_3
{
class Account;
}

namespace MeegoIntegration
{
using namespace qutim_sdk_0_3;

enum ItemRole
{
	DescriptionRole = Qt::UserRole + 33,
	SeparatorRole,
	TitleRole, //NOTE: in title and separator description and decoration roles will be ignored
	ActionRole
};

class JoinGroupChatWrapper : public QObject
{
	Q_OBJECT
	Q_PROPERTY(qutim_sdk_0_3::Account *currentAccount READ currentAccount WRITE setCurrentAccount NOTIFY currentAccountChanged)
	Q_PROPERTY(QDeclarativeListProperty<qutim_sdk_0_3::Account> accounts READ accounts NOTIFY accountsChanged)
	Q_PROPERTY(QVariantList bookmarks READ bookmarks NOTIFY bookmarksChanged)
	Q_PROPERTY(QVariantList recent READ recent NOTIFY recentChanged)
public:
	JoinGroupChatWrapper();
	~JoinGroupChatWrapper();
	Q_INVOKABLE QStringList accountIds();
	static void init();
	static void showDialog();
	QString currentAccountId();
	Account *currentAccount();
	void setCurrentAccount(Account *currentAccount);
	QVariantList bookmarks() const;
	QVariantList recent() const;
	
	QDeclarativeListProperty<qutim_sdk_0_3::Account> accounts();

signals:
	void accountsChanged(const QDeclarativeListProperty<qutim_sdk_0_3::Account> &accounts);
	void currentAccountChanged(qutim_sdk_0_3::Account *currentAccount);
	void bookmarksChanged(const QVariantList &bookmarks);
	void recentChanged(const QVariantList &recent);
	void currentAccountIdChanged();
	void joinDialogShown();
	void bookmarkEditDialogShown();
	void joined();
	void shown();

public slots:
	Q_INVOKABLE void setAccount(int index);
	void fillBookmarks(Account *account);
	Q_INVOKABLE void onItemActivated(const QModelIndex &index);
	void onBookmarksChanged();
	
	QVariant fields();
	bool join(const qutim_sdk_0_3::DataItem &item);
	bool remove(const qutim_sdk_0_3::DataItem &item);
	bool save(const qutim_sdk_0_3::DataItem &item, const qutim_sdk_0_3::DataItem &oldItem);
	
protected slots:
	void onAccountCreated(qutim_sdk_0_3::Account *account, bool first = true);
	void onManagerChanged(qutim_sdk_0_3::GroupChatManager *manager);
	void onAccountDeath(QObject *object);

private:
	void rebuildBookmarks();
	void fillBookmarks(const QList<DataItem> &bookmarks, QVariantList &list);
	
	void fillBookmarks(const QList<DataItem> &bookmarks, bool recent = false);
	Account *account(int index);
	QAction *m_closeAction;
	QAction *m_backAction;
	BookmarksModel *m_bookmarksModel;
	BookmarksModel *m_bookmarksBoxModel;
	QStringList m_accountIds;
	
	Account *m_currentAccount;
	QList<qutim_sdk_0_3::Account*> m_accounts;
	QList<DataItem> m_bookmarks;
	QList<DataItem> m_recent;
	QVariantList m_variantBookmarks;
	QVariantList m_variantRecent;

};

}
#endif // JOINGROUPCHATWRAPPER_H

