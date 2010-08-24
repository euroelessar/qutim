/****************************************************************************
 *  joingroupchat.h
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

#ifndef JOINGROUPCHAT_H
#define JOINGROUPCHAT_H

#include <QDialog>
#include <qutim/actiongenerator.h>

class QHBoxLayout;
class QListWidgetItem;
class QModelIndex;

namespace Ui {
    class JoinGroupChat;
}

namespace qutim_sdk_0_3
{
	class DataItem;
	class Account;
}

namespace Core
{
	using namespace qutim_sdk_0_3;

	enum ButtonType
	{
		ButtonTypeNew,
		ButtonTypeEditBookmarks,
		ButtonTypeBookmark,
		ButtonTypeSeparator
	};

	Q_DECLARE_FLAGS(ButtonTypes,ButtonType);
	
	class JoinGroupChat : public QDialog
	{
		Q_OBJECT		
	public:
		explicit JoinGroupChat(QWidget *parent = 0);
		~JoinGroupChat();
		virtual void showEvent(QShowEvent* );
		virtual void closeEvent(QCloseEvent *);
	protected:
		virtual void changeEvent(QEvent* );
	private slots:
		void onToolBarActTriggered(QAction*);
		void onCurrentChanged(int);
		void onAccountBoxActivated(int index);
		void onBookmarksBoxActivated(int index);
		void fillAccounts();
		void fillBookmarks(Account *account);
		void onItemActivated(const QModelIndex &index);
		void onNegativeActionTriggered();
		void onPositiveActionTriggered();
		void updateBookmark(bool remove = false);
	private:
		void fillBookmarks(const QVariantList &items, bool recent = false);
		void updateDataForm(const DataItem &item, int pos = 0);
		Account *currentAccount();
		QListWidgetItem *createItem(const QString &name,const QVariant &description = QVariant());
		Ui::JoinGroupChat *ui;
		QPointer<QWidget> m_dataform_widget;
		QAction *m_positive_action;
		QAction *m_negative_action;
	};

}
#endif // JOINGROUPCHAT_H
