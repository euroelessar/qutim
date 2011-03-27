/****************************************************************************
 *  joingroupchat.h
 *
 *  Copyright (c) 2010 by Sidorov Aleksey <sauron@citadelspb.com>
 *  Copyright (c) 2010 by Alexey Prokhin <alexey.prokhin@yandex.ru>
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

#ifndef JOINGROUPCHAT_H
#define JOINGROUPCHAT_H

#include <QDialog>
#include <qutim/actiongenerator.h>
#include <qutim/dataforms.h>
#include <slidingstackedwidget.h>

class QHBoxLayout;
class QListWidgetItem;
class QModelIndex;

namespace Ui {
class JoinGroupChat;
}

namespace qutim_sdk_0_3
{
class Account;
}

namespace Core
{
using namespace qutim_sdk_0_3;

class BookmarksModel;
class JoinGroupChat : public QDialog
{
	Q_OBJECT
public:
	explicit JoinGroupChat(QWidget *parent = 0);
	~JoinGroupChat();
	virtual void showEvent(QShowEvent* );
protected:
	virtual void changeEvent(QEvent* );
private slots:
	void onToolBarActTriggered(QAction*);
	void onCurrentChanged(int);
	void onAccountBoxActivated(int index);
	void fillBookmarks(Account *account);
	void onItemActivated(const QModelIndex &index);
	void onBackActionTriggered();
	void onBookmarksChanged();
	void onFingerGesture(SlidingStackedWidget::SlideDirection);
private:
	void fillBookmarks(const QList<DataItem> &bookmarks, bool recent = false);
	Account *currentAccount();
	Account *account(int index);
	Ui::JoinGroupChat *ui;
	QAction *m_closeAction;
	QAction *m_backAction;
	BookmarksModel *m_bookmarksViewModel;
	BookmarksModel *m_bookmarksBoxModel;
};

}
#endif // JOINGROUPCHAT_H
