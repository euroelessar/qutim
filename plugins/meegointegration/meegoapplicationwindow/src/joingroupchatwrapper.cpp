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

#include "joingroupchatwrapper.h"
#include <qutim/protocol.h>
#include <qutim/account.h>
#include <qutim/icon.h>
#include <qutim/event.h>
#include <qutim/dataforms.h>
#include <qutim/groupchatmanager.h>
#include <qutim/debug.h>
#include <qutim/icon.h>
#include <qutim/servicemanager.h>
#include <qdeclarative.h>

namespace MeegoIntegration
{
const static int m_max_recent_count = 4;

Q_GLOBAL_STATIC(QList<JoinGroupChatWrapper*>, m_managers)

JoinGroupChatWrapper::JoinGroupChatWrapper()
{
	m_bookmarksBoxModel = new BookmarksModel();
	m_bookmarksModel = new BookmarksModel();
	m_managers()->append(this);
	m_accounts = new QList<Account*>;

}

void JoinGroupChatWrapper::init()
{
	qmlRegisterType<JoinGroupChatWrapper>("org.qutim", 0, 3, "JoinGroupChatWrapper");
	qmlRegisterType<BookmarksModel>("org.qutim", 0, 3, "BookmarksModel");
}

JoinGroupChatWrapper::~JoinGroupChatWrapper()
{
	m_managers()->removeOne(this);
}


QStringList JoinGroupChatWrapper::accountIds()
{
	m_accountIds.clear();
	m_accounts->clear();
	foreach (Protocol *protocol, Protocol::all()) {
		foreach (Account *account, protocol->accounts())
		{
			m_accountIds.append(account->id());
			m_accounts->append(account);
		}

	}
	if (m_accounts->count() == 1)
	{
		setAccount(0);
	}
	return m_accountIds;
}

QString JoinGroupChatWrapper::currentAccountId()
{
	if (m_currentAccount)
		return m_currentAccount->id();
	else
		return QString();
}

Account * JoinGroupChatWrapper::currentAccount()
{
	return m_currentAccount;
}

void JoinGroupChatWrapper::setAccount(int index)
{
	Account *account = m_accounts->at(index);
	if (!account) {
		qDebug()<<"No Account";
		m_bookmarksModel->clear();
		return;
	}
	//fillBookmarks(account);
	m_currentAccount = account;
	emit currentAccountIdChanged();
}

void JoinGroupChatWrapper::fillBookmarks(const QList<DataItem> &bookmarks, bool recent)
{
	QString txt = recent ?
				QT_TRANSLATE_NOOP("JoinGroupChat", "Recent") :
				QT_TRANSLATE_NOOP("JoinGroupChat", "Bookmarks");
	m_bookmarksModel->addItem(BookmarkSeparator, txt);
	int count = 0;
	foreach (const DataItem &bookmark, bookmarks) {
		QString name = bookmark.title();
		QVariantMap fields;
		foreach (const DataItem &subitem, bookmark.subitems()) {
			 if (subitem.property("showInBookmarkInfo", true))
				fields.insert(subitem.title(), subitem.data());
		}
		BookmarkType type = recent ? BookmarkRecentItem : BookmarkItem;
		QVariant userData = QVariant::fromValue(bookmark);

		m_bookmarksModel->addItem(type, name, fields, userData);
		m_bookmarksBoxModel->addItem(type, name, fields, userData);
		++count;
		if (recent && (count >= m_max_recent_count))
			return;
	}
}

void JoinGroupChatWrapper::fillBookmarks(Account *account)
{

	GroupChatManager *manager = account->groupChatManager();
	if (!manager)
		return;

	m_bookmarksBoxModel->startUpdating();
	m_bookmarksModel->startUpdating();

	QVariant fields = qVariantFromValue(QT_TRANSLATE_NOOP("JoinGroupChat", "Join an existing or create a new groupchat"));
	m_bookmarksModel->addItem(BookmarkNew,
								  QT_TRANSLATE_NOOP("JoinGroupChat", "Join"),
								  fields);
	fields = qVariantFromValue(QT_TRANSLATE_NOOP("JoinGroupChat", "Create, edit, or delete saved bookmarks"));
	m_bookmarksModel->addItem(BookmarkEdit,
								  QT_TRANSLATE_NOOP("JoinGroupChat", "Manage bookmarks"),
								  fields);

	m_bookmarksBoxModel->addItem(BookmarkEmptyItem,QString());
	//Bookmarks
	fillBookmarks(manager->bookmarks());
	//Recent items
	fillBookmarks(manager->recent(), true);

	m_bookmarksBoxModel->endUpdating();
	m_bookmarksModel->endUpdating();

}

void JoinGroupChatWrapper::onItemActivated(const QModelIndex &index)
{
	Account *account = currentAccount();
	qDebug()<<"onItemActivatedAccount";
	if (!account || index.data(SeparatorRole).toBool())
		return;
	BookmarkType type = index.data(BookmarkTypeRole).value<BookmarkType>();
	qDebug()<<"onItemActivatedType";
	switch (type) {
	case BookmarkItem:
	case BookmarkRecentItem: {
		qDebug()<<"BookmarkItem";
		GroupChatManager *manager = account->groupChatManager();
		if (!manager)
			break;
		DataItem bookmark = index.data(BookmarkDataRole).value<DataItem>();
		manager->join(bookmark);
		qDebug()<<"Join";
		emit joined();
		break;
	}
	case BookmarkNew: {
		emit joinDialogShown();
		break;
	}
	case BookmarkEdit: {
		emit bookmarkEditDialogShown();
		break;
	}
	default:
		break;
	}
}

void JoinGroupChatWrapper::join(QVariant data)
{
	Account *account = currentAccount();
	GroupChatManager *manager = account->groupChatManager();
		if (!manager)
			return;
		DataItem bookmark = data.value<DataItem>();
		manager->join(bookmark);
		qDebug()<<"Join";
		emit joined();

}

void JoinGroupChatWrapper::onBookmarksChanged()
{
	fillBookmarks(currentAccount());
}

void JoinGroupChatWrapper::showDialog()
{
	for (int i = 0; i < m_managers()->count();i++)
	{
		emit m_managers()->at(i)->shown();
	}
}

}

