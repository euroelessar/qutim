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
#include "accountsmodel.h"
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
	Q_PROPERTY(MeegoIntegration::BookmarksModel* bookmarksBox READ bookmarksBox NOTIFY bookmarksBoxChanged)
	Q_PROPERTY(MeegoIntegration::BookmarksModel* bookmarks READ bookmarks NOTIFY bookmarksChanged)
	Q_PROPERTY(MeegoIntegration::AccountsModel* accounts READ accounts NOTIFY accountsChanged)
public:
	JoinGroupChatWrapper();
	~JoinGroupChatWrapper();
	BookmarksModel *bookmarksBox();
	BookmarksModel *bookmarks();
	AccountsModel *accounts();
	static void init();
	static void showDialog();

signals:
	void bookmarksBoxChanged();
	void bookmarksChanged();
	void accountsChanged();
	void joinDialogShown();
	void bookmarkEditDialogShown();
	void joined();
	void shown();

private slots:
	void onAccountBoxActivated(int index);
	void fillBookmarks(Account *account);
	void onItemActivated(const QModelIndex &index);
	void onBookmarksChanged();

private:
	void fillBookmarks(const QList<DataItem> &bookmarks, bool recent = false);
	Account *currentAccount();
	Account *account(int index);
	QAction *m_closeAction;
	QAction *m_backAction;
	BookmarksModel *m_bookmarksModel;
	BookmarksModel *m_bookmarksBoxModel;
	AccountsModel *m_accountsModel;

};

}
#endif // JOINGROUPCHATWRAPPER_H
