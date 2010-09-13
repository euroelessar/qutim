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
#include "itemdelegate.h"

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
		connect(ui->bookmarksBox, SIGNAL(currentIndexChanged(int)),
				SLOT(onBookmarksBoxActivated(int)));
		connect(ui->bookmarksView, SIGNAL(activated(QModelIndex)),
				SLOT(onItemActivated(QModelIndex)));

		ui->bookmarksView->setItemDelegate(new ItemDelegate(this));
		ui->bookmarksView->setModel(m_bookmarksViewModel = new BookmarksModel(this));
		ui->bookmarksBox->setItemDelegate(ui->bookmarksView->itemDelegate());
		ui->bookmarksBox->setModel(m_bookmarksBoxModel = new BookmarksModel(this));
		ui->accountBox->setModel(new AccountsModel(this));

		m_negative_action = new QAction(QT_TRANSLATE_NOOP("JoinGroupChat", "Close"),this);
		m_negative_action->setSoftKeyRole(QAction::NegativeSoftKey);
		ui->actionBox->addAction(m_negative_action);
		connect(m_negative_action,SIGNAL(triggered()),SLOT(onNegativeActionTriggered()));

		m_positive_action = new QAction(this);
		m_positive_action->setSoftKeyRole(QAction::PositiveSoftKey);
		connect(m_positive_action,SIGNAL(triggered()),SLOT(onPositiveActionTriggered()));
		ui->actionBox->addAction(m_positive_action);
		m_positive_action->setVisible(false);
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
		Account *account = currentAccount();
		if (!account)
			return;
		//1 - join page
		//2 - bookmarks page
		//TODO rewrite on enums
		if (!index) {
			m_positive_action->setVisible(false);
			m_negative_action->setText(QT_TRANSLATE_NOOP("JoinGroupChat", "Close"));
#ifndef QUTIM_MOBILE_UI
			m_negative_action->setVisible(false);
#endif
		}
		else {
			m_negative_action->setVisible(true);
			m_positive_action->setVisible(true);
			m_negative_action->setText(QT_TRANSLATE_NOOP("JoinGroupChat", "Back"));
			if (index == 1) {
				Event event("groupchat-fields");
				qApp->sendEvent(account,&event);
				DataItem item = event.at<DataItem>(0);
				updateDataForm(item);
				m_positive_action->setText(QT_TRANSLATE_NOOP("JoinGroupChat", "Join"));
			} else if (index == 2) {
				onBookmarksBoxActivated(ui->bookmarksBox->currentIndex());
			}
		}
	}

	void JoinGroupChat::onAccountBoxActivated(int index)
	{
		Account *account = this->account(index);
		if (!account) {
			m_bookmarksViewModel->clear();
			return;
		}
		fillBookmarks(account);
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

		m_bookmarksBoxModel->addItem(BookmarkEmptyItem, QString());
		//Bookmarks
		QVariantList bookmarks = event.at<QVariantList>(0);
		fillBookmarks(bookmarks);
		//Recent items
		m_bookmarksBoxModel->addItem(BookmarkSeparator, QT_TRANSLATE_NOOP("JoinGroupChat", "Recent"));
		bookmarks = event.at<QVariantList>(1);
		fillBookmarks(bookmarks, true);

		m_bookmarksBoxModel->endUpdating();
		m_bookmarksViewModel->endUpdating();
		ui->bookmarksBox->setCurrentIndex(0);
	}

	void JoinGroupChat::updateDataForm(const DataItem &items, int pos)
	{
		if (m_dataform_widget)
			m_dataform_widget->deleteLater();

		m_dataform_widget = AbstractDataForm::get(items);
		if (m_dataform_widget) {
			m_dataform_widget->setParent(this);
			m_dataform_widget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
			QVBoxLayout *layout = static_cast<QVBoxLayout*>(ui->stackedWidget->currentWidget()->layout());
			layout->insertWidget(pos,m_dataform_widget.data());
		}
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
			ui->stackedWidget->setCurrentIndex(1);
			break;
		}
		case BookmarkEdit: {
			ui->stackedWidget->setCurrentIndex(2);
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

	void JoinGroupChat::onNegativeActionTriggered()
	{
		if (!ui->stackedWidget->currentIndex())
			close();
		else
			ui->stackedWidget->setCurrentIndex(0);
	}

	void JoinGroupChat::closeEvent(QCloseEvent *)
	{
		if (!ui->bookmarksBox->currentText().isEmpty())
			updateBookmark(false);
	}

	void JoinGroupChat::onPositiveActionTriggered()
	{
		Account *account = currentAccount();
		if (!account)
			return;
		if (ui->stackedWidget->currentIndex() == 1) {
			//join
			DataItem item = qobject_cast<AbstractDataForm*>(m_dataform_widget)->item();
			Event event ("groupchat-join",qVariantFromValue(item));
			if (ui->saveBookmark->isChecked()) {
				event.args[1] = ui->saveBookmark->isChecked();
				event.args[2] = ui->bookmarkName->text();
			}
			qApp->sendEvent(account,&event);
			close();
		} else if (ui->stackedWidget->currentIndex() == 2) {
			int index = ui->bookmarksBox->currentIndex();
			updateBookmark(!isRecent(index));
			fillBookmarks(account);
		}
	}

	void JoinGroupChat::updateBookmark(bool remove)
	{
		Account *account = currentAccount();
		if (!account)
			return;
		if (!remove) {
			DataItem item = qobject_cast<AbstractDataForm*>(m_dataform_widget)->item();
			Event event ("groupchat-bookmark-save", qVariantFromValue(item));
			event.args[1] = ui->bookmarksBox->currentText(); //old name
			qApp->sendEvent(account, &event);
			onCurrentChanged(2);
		} else {
			Event event ("groupchat-bookmark-remove", ui->bookmarksBox->currentText());
			qApp->sendEvent(account, &event);
			onCurrentChanged(2);
		}
	}

	void JoinGroupChat::onBookmarksBoxActivated(int index)
	{
		Account *account = currentAccount();
		if (!account || ui->stackedWidget->currentIndex() != 2)
			return;

		m_positive_action->setText(!isRecent(index) ?
								   QT_TRANSLATE_NOOP("JoinGroupChat", "Remove") :
								   QT_TRANSLATE_NOOP("JoinGroupChat", "Add"));
		Event event("groupchat-fields");
		event.args[1] = ui->bookmarksBox->itemText(index);
		event.args[2] = true;
		qApp->sendEvent(account, &event);
		DataItem item = event.at<DataItem>(0);
		updateDataForm(item, 2);
	}

	bool JoinGroupChat::isRecent(int index)
	{
		BookmarkType type = m_bookmarksBoxModel->data(index, BookmarkTypeRole).value<BookmarkType>();
		return type == BookmarkRecentItem;
	}

}
