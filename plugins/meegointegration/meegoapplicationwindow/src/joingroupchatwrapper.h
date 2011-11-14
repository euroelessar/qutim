/****************************************************************************
**
** qutIM instant messenger
**
** Copyright (C) 2011 Evgeniy Degtyarev <degtep@gmail.com>
** Copyright (c) 2010 by Sidorov Aleksey <sauron@citadelspb.com>
** Copyright (c) 2010 by Alexey Prokhin <alexey.prokhin@yandex.ru>
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
	Q_PROPERTY(QString currentAccountId READ currentAccountId NOTIFY currentAccountIdChanged)

public:
	JoinGroupChatWrapper();
	~JoinGroupChatWrapper();
	Q_INVOKABLE QStringList accountIds();
	static void init();
	static void showDialog();
	QString currentAccountId();
	Account * currentAccount();

signals:
	void currentAccountIdChanged();
	void joinDialogShown();
	void bookmarkEditDialogShown();
	void joined();
	void shown();

public slots:
	Q_INVOKABLE void setAccount(int index);
	void fillBookmarks(Account *account);
	Q_INVOKABLE void onItemActivated(const QModelIndex &index);
	Q_INVOKABLE void join(QVariant data);
	void onBookmarksChanged();

private:
	void fillBookmarks(const QList<DataItem> &bookmarks, bool recent = false);
	Account *account(int index);
	QAction *m_closeAction;
	QAction *m_backAction;
	BookmarksModel *m_bookmarksModel;
	BookmarksModel *m_bookmarksBoxModel;
	Account * m_currentAccount;
	QList<Account*> *m_accounts;
	QStringList m_accountIds;

};

}
#endif // JOINGROUPCHATWRAPPER_H
