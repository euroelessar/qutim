/****************************************************************************
 *  quetzaljoinchatdialog.h
 *
 *  Copyright (c) 2009 by Nigmatullin Ruslan <euroelessar@gmail.com>
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

#ifndef QUETZALJOINCHATDIALOG_H
#define QUETZALJOINCHATDIALOG_H

#include <QDialog>
#include <purple.h>
#include <qutim/groupchatmanager.h>
#include <QMetaTypeId>

namespace Ui {
    class QuetzalJoinChatDialog;
}

struct QuetzalChatGuard
{
	typedef QSharedPointer<QuetzalChatGuard> Ptr;
	PurpleChat *chat;
};

class QuetzalAccount;

class QuetzalJoinChatManager : public qutim_sdk_0_3::GroupChatManager
{
public:
	QuetzalJoinChatManager(QuetzalAccount *account);
	~QuetzalJoinChatManager();
	
	virtual qutim_sdk_0_3::DataItem fields() const;
	virtual bool join(const qutim_sdk_0_3::DataItem &fields);
	virtual bool storeBookmark(const qutim_sdk_0_3::DataItem &fields, const qutim_sdk_0_3::DataItem &oldFields);
	virtual bool removeBookmark(const qutim_sdk_0_3::DataItem &fields);
	virtual QList<qutim_sdk_0_3::DataItem> bookmarks() const;
	virtual QList<qutim_sdk_0_3::DataItem> recent() const;

protected:
	QList<qutim_sdk_0_3::DataItem> convertChats(bool recent) const;
	qutim_sdk_0_3::DataItem fields(const char *alias, GHashTable *comps) const;
//	void changeEvent(QEvent *e);

//protected slots:
//	void onJoinButtonClicked();
//	void on_bookmarkBox_currentIndexChanged(int index);

private:
	void init(const char *data);
	Ui::QuetzalJoinChatDialog *ui;
	QPushButton *m_searchButton;
	QPushButton *m_joinButton;
	PurpleConnection *m_gc;
};

Q_DECLARE_METATYPE(QuetzalChatGuard::Ptr)

#endif // QUETZALJOINCHATDIALOG_H
