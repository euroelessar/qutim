/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Aleksey Sidorov <gorthauer87@yandex.ru>
** Copyright © 2014 Nicolay Izoderov <nico-izo@ya.ru>
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

#include "joingroupchat.h"
#include "ui_joingroupchat.h"
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
#include <qutim/itemdelegate.h>
#include "joinpage.h"
#include <qutim/servicemanager.h>
#include "uripage.h"

namespace Core
{
const static int m_max_recent_count = 4;

JoinGroupChat::JoinGroupChat(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::JoinGroupChat)
{
	ui->setupUi(this);
	setAttribute(Qt::WA_DeleteOnClose);

	connect(ui->stackedWidget, SIGNAL(currentChanged(int)),
			SLOT(onCurrentChanged(int)));
	connect(ui->accountBox, SIGNAL(currentIndexChanged(int)),
			SLOT(onAccountBoxActivated(int)));
	ui->bookmarksPage->setModel(m_bookmarksBoxModel = new BookmarksModel(this));

	ui->bookmarksView->setItemDelegate(new ItemDelegate(this));
	ui->bookmarksView->setModel(m_bookmarksViewModel = new BookmarksModel(this));
	connect(ui->bookmarksView, SIGNAL(activated(QModelIndex)),
			SLOT(onItemActivated(QModelIndex)));
	ui->accountBox->setModel(new AccountsModel(this));

	m_closeAction = new QAction(QT_TRANSLATE_NOOP("JoinGroupChat", "Close"),this);
	connect(m_closeAction,SIGNAL(triggered()), SLOT(close()));

	m_backAction = new QAction(QT_TRANSLATE_NOOP("JoinGroupChat", "Back"),this);
	ui->actionBox->addAction(m_backAction);
	connect(m_backAction, SIGNAL(triggered()), SLOT(onBackActionTriggered()));

	connect(ui->joinPage, SIGNAL(joined()), SLOT(close()));
	connect(ui->uriPage, SIGNAL(joined()), SLOT(close()));
	connect(ui->bookmarksPage, SIGNAL(bookmarksChanged()), SLOT(onBookmarksChanged()));
	connect(ui->stackedWidget, SIGNAL(fingerGesture(SlidingStackedWidget::SlideDirection)),
			this, SLOT(onFingerGesture(SlidingStackedWidget::SlideDirection)));

	if(QObject *scroller = ServiceManager::getByName("Scroller"))
		QMetaObject::invokeMethod(scroller, "enableScrolling", Q_ARG(QObject*, ui->bookmarksView->viewport()));
}

void JoinGroupChat::onFingerGesture(SlidingStackedWidget::SlideDirection direction)
{
	 if (direction == SlidingStackedWidget::LeftToRight)
		 onBackActionTriggered();
}

JoinGroupChat::~JoinGroupChat()
{
	delete ui;
}

void JoinGroupChat::changeEvent(QEvent *e)
{
	QDialog::changeEvent(e);
	switch (e->type()) {
	case QEvent::LanguageChange:
		ui->retranslateUi(this);
		break;
	default:
		break;
	}
}

void JoinGroupChat::showEvent(QShowEvent *ev)
{
	QDialog::showEvent(ev);
	onCurrentChanged(0);
}

void JoinGroupChat::onToolBarActTriggered(QAction *act)
{
	int index = act->data().toInt();
	ui->stackedWidget->setCurrentIndex(index);
}

void JoinGroupChat::onCurrentChanged(int index)
{
	ui->actionBox->clear();
	if(index)
		ui->actionBox->addAction(m_backAction);
	else
	{
#ifdef Q_OS_SYMBIAN
		ui->actionBox->addAction(m_closeAction);
#endif
	}

	ui->actionBox->addActions(ui->stackedWidget->currentWidget()->actions());
}

void JoinGroupChat::onAccountBoxActivated(int index)
{
	Account *account = this->account(index);
	if (!account) {
		m_bookmarksViewModel->clear();
		return;
	}
	fillBookmarks(account);
	ui->joinPage->setAccount(account);
	ui->uriPage->setAccount(account);
	ui->bookmarksPage->setAccount(account);
	ui->bookmarksPage->setModel(m_bookmarksBoxModel);
}

void JoinGroupChat::fillBookmarks(const QList<DataItem> &bookmarks, bool recent)
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

void JoinGroupChat::fillBookmarks(Account *account)
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
	if(!m_uri.isEmpty()) {
		fields = qVariantFromValue(QT_TRANSLATE_NOOP("JoinGroupChat",
													 QString("Join URI right now: %1").arg(m_uri).toUtf8()
													 ));
		m_bookmarksViewModel->addItem(BookmarkURI,
									  QT_TRANSLATE_NOOP("JoinGroupChat",
														QString("Join %1").arg(m_uri).toUtf8()
														),
									  fields);
	}

	m_bookmarksBoxModel->addItem(BookmarkEmptyItem,QString());
	//Bookmarks
	fillBookmarks(manager->bookmarks());
	//Recent items
	fillBookmarks(manager->recent(), true);

	m_bookmarksBoxModel->endUpdating();
	m_bookmarksViewModel->endUpdating();
}

void JoinGroupChat::onItemActivated(const QModelIndex &index)
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
		close();
		break;
	}
	case BookmarkNew: {
		ui->stackedWidget->slideInIdx(1);
		break;
	}
	case BookmarkEdit: {
		ui->stackedWidget->slideInIdx(2);
		break;
	}
	case BookmarkURI: {
		ui->stackedWidget->slideInIdx(3);
		break;
	}
	default:
		break;
	}
}

inline Account *JoinGroupChat::currentAccount()
{
	return account(ui->accountBox->currentIndex());
}

inline Account *JoinGroupChat::account(int index)
{
	QAbstractItemModel *model = ui->accountBox->model();
	return (model->data(model->index(index, 0), Qt::UserRole)).value<Account*>();
}

void JoinGroupChat::onBackActionTriggered()
{
	ui->stackedWidget->slideInIdx(0);
}

void JoinGroupChat::onBookmarksChanged()
{
	fillBookmarks(currentAccount());
}

void JoinGroupChat::setXmppURI(const QString &uri)
{
	m_uri = uri;
	ui->uriPage->setUri(uri);
	onBookmarksChanged();
}

}

