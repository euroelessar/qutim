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

#include "joingroupchatwrapper.h"
#include "accountsmodel.h"
#include "bookmarksmodel.h"
#include <QEvent>
#include <qutim/protocol.h>
#include <qutim/account.h>
#include <qutim/icon.h>
#include <qutim/event.h>
#include <qutim/dataforms.h>
#include <qutim/groupchatmanager.h>
#include <QPushButton>
#include <qutim/debug.h>
#include <QCommandLinkButton>
#include <QLatin1Literal>
#include <qutim/icon.h>
#include <itemdelegate.h>
//#include "joinpage.h"
#include <qutim/servicemanager.h>

namespace MeegoIntegration
{
const static int m_max_recent_count = 4;

JoinGroupChatWrapper::JoinGroupChatWrapper(QWidget *parent) :
	QDialog(parent)
{
	m_bookmarksBoxModel = new BookmarksModel(this);
	m_bookmarksModel = new BookmarksModel(this);
	m_accountsModel = new AccountsModel(this);


	//connect(ui->accountBox, SIGNAL(currentIndexChanged(int)),
	//		SLOT(onAccountBoxActivated(int)));
	//connect(ui->bookmarksView, SIGNAL(activated(QModelIndex)),
	//		SLOT(onItemActivated(QModelIndex)));

	//connect(ui->joinPage,SIGNAL(joined()),SLOT(close()));
	//connect(ui->bookmarksPage,SIGNAL(bookmarksChanged()),SLOT(onBookmarksChanged()));

}

void JoinGroupChatWrapper::init()
{
	qmlRegisterType<JoinGroupChatWrapper>("org.qutim", 0, 3, "JoinGroupChatWrapper");
	qmlRegisterType<AccountsModel>("org.qutim", 0, 3, "AccountsModel");
	qmlRegisterType<BookmarksModel>("org.qutim", 0, 3, "BookmarksModel");
}

JoinGroupChatWrapper::~JoinGroupChatWrapper()
{
	delete ui;
}

JoinGroupChatWrapper::bookmarksBox()
{
	return m_bookmarksBoxModel;
}

JoinGroupChatWrapper::bookmarks()
{
	return m_bookmarksModel;
}

JoinGroupChatWrapper::accounts()
{
	return m_accountsModel;
}

void JoinGroupChatWrapper::onAccountBoxActivated(int index)
{
	Account *account = this->account(index);
	if (!account) {
		m_bookmarksViewModel->clear();
		return;
	}
	fillBookmarks(account);
	//ui->joinPage->setAccount(account);
	//ui->bookmarksPage->setAccount(account);
	//ui->bookmarksPage->setModel(m_bookmarksBoxModel);
}

void JoinGroupChatWrapper::fillBookmarks(const QList<DataItem> &bookmarks, bool recent)
{
	QString txt = recent ?
				QT_TRANSLATE_NOOP("JoinGroupChat", "Recent") :
				QT_TRANSLATE_NOOP("JoinGroupChat", "Bookmarks");
	m_bookmarksViewModel->addItem(BookmarkSeparator, txt);
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

		m_bookmarksViewModel->addItem(type, name, fields, userData);
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
	m_bookmarksViewModel->startUpdating();

	QVariant fields = qVariantFromValue(QT_TRANSLATE_NOOP("JoinGroupChat", "Join an existing or create a new groupchat"));
	m_bookmarksViewModel->addItem(BookmarkNew,
								  QT_TRANSLATE_NOOP("JoinGroupChat", "Join"),
								  fields);
	fields = qVariantFromValue(QT_TRANSLATE_NOOP("JoinGroupChat", "Create, edit, or delete saved bookmarks"));
	m_bookmarksViewModel->addItem(BookmarkEdit,
								  QT_TRANSLATE_NOOP("JoinGroupChat", "Manage bookmarks"),
								  fields);

	m_bookmarksBoxModel->addItem(BookmarkEmptyItem,QString());
	//Bookmarks
	fillBookmarks(manager->bookmarks());
	//Recent items
	fillBookmarks(manager->recent(), true);

	m_bookmarksBoxModel->endUpdating();
	m_bookmarksViewModel->endUpdating();
}

void JoinGroupChatWrapper::onItemActivated(const QModelIndex &index)
{
	Account *account = currentAccount();
	if (!account || index.data(SeparatorRole).toBool())
		return;
	BookmarkType type = index.data(BookmarkTypeRole).value<BookmarkType>();
	switch (type) {
	case BookmarkItem:
	case BookmarkRecentItem: {
		GroupChatManager *manager = account->groupChatManager();
		if (!manager)
			break;
		DataItem bookmark = index.data(Qt::UserRole).value<DataItem>();
		manager->join(bookmark);
		//close();
		break;
	}
	case BookmarkNew: {
		//ui->stackedWidget->slideInIdx(1);
		break;
	}
	case BookmarkEdit: {
		//ui->stackedWidget->slideInIdx(2);
		break;
	}
	default:
		break;
	}
}

inline Account *JoinGroupChatWrapper::currentAccount()
{
	return account(ui->accountBox->currentIndex());
}

inline Account *JoinGroupChatWrapper::account(int index)
{
	QAbstractItemModel *model = ui->accountBox->model();
	return (model->data(model->index(index, 0), Qt::UserRole)).value<Account*>();
}
void JoinGroupChatWrapper::onBookmarksChanged()
{
	fillBookmarks(currentAccount());
}

}
