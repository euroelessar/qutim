/****************************************************************************
 *  joingroupchat.cpp
 *
 *  Copyright (c) 2010 by Aleksey Sidorov <sauron@citadelspb.com>
 *
 ***************************************************************************
 *                                                                         *
 *   This library is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
*****************************************************************************/

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
#include <QPushButton>
#include <qutim/debug.h>
#include <QCommandLinkButton>
#include <QLatin1Literal>
#include <qutim/icon.h>
#include <itemdelegate.h>
#include "joinpage.h"

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
	m_closeAction->setSoftKeyRole(QAction::NegativeSoftKey);
	ui->actionBox->addAction(m_closeAction);
	connect(m_closeAction,SIGNAL(triggered()),SLOT(close()));

	m_backAction = new QAction(QT_TRANSLATE_NOOP("JoinGroupChat", "Back"),this);
	m_backAction->setSoftKeyRole(QAction::NegativeSoftKey);
	ui->actionBox->addAction(m_backAction);
	connect(m_backAction,SIGNAL(triggered()),SLOT(onBackActionTriggered()));

	connect(ui->joinPage,SIGNAL(joined()),SLOT(close()));
	connect(ui->bookmarksPage,SIGNAL(bookmarksChanged()),SLOT(onBookmarksChanged()));
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

void JoinGroupChat::onCurrentChanged( int index)
{
	ui->actionBox->clear();
	if(!index)
		ui->actionBox->addAction(m_closeAction);
	else
		ui->actionBox->addAction(m_backAction);
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
	ui->bookmarksPage->setAccount(account);
	ui->bookmarksPage->setModel(m_bookmarksBoxModel);
}

void JoinGroupChat::fillBookmarks(const QVariantList &items, bool recent)
{
	QString txt = recent ?
				QT_TRANSLATE_NOOP("JoinGroupChat", "Recent") :
				QT_TRANSLATE_NOOP("JoinGroupChat", "Bookmarks");
	m_bookmarksViewModel->addItem(BookmarkSeparator, txt);
	int count = 0;
	foreach (const QVariant &itemVar, items) {
		QVariantMap item = itemVar.toMap();
		QString name = item.value("name").toString();
		QVariantMap fields = item.value("fields").toMap();
		BookmarkType type = recent ? BookmarkRecentItem : BookmarkItem;
		m_bookmarksViewModel->addItem(type, name, fields);
		m_bookmarksBoxModel->addItem(type, name, fields);
		++count;
		if (recent && (count >= m_max_recent_count))
			return;
	}
}

void JoinGroupChat::fillBookmarks(Account *account)
{
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

	Event event ("groupchat-bookmark-list");
	qApp->sendEvent(account,&event);

	m_bookmarksBoxModel->addItem(BookmarkEmptyItem,QString());
	//Bookmarks
	QVariantList bookmarks = event.at<QVariantList>(0);
	fillBookmarks(bookmarks);
	//Recent items
	m_bookmarksBoxModel->addItem(BookmarkSeparator, QT_TRANSLATE_NOOP("JoinGroupChat", "Recent"));
	bookmarks = event.at<QVariantList>(1);
	fillBookmarks(bookmarks, true);

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
		//fill data
		Event event("groupchat-fields");
		event.args[1] = index.data().toString();
		event.args[2] = true;
		qApp->sendEvent(account, &event);
		DataItem item = event.at<DataItem>(0);
		//join
		event = Event("groupchat-join",qVariantFromValue(item));
		qApp->sendEvent(account, &event);
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

}
