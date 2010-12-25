/****************************************************************************
 *  bookmarkspage.h
 *
 *  Copyright (c) 2010 by Sidorov Aleksey <sauron@citadelspb.com>
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

#ifndef BOOKMARKSPAGE_H
#define BOOKMARKSPAGE_H

#include <QScrollArea>
#include <QPointer>
#include "groupchatpage.h"
#include <qutim/dataforms.h>

class QComboBox;
namespace Core {

class BookmarksModel;
class BookmarksPage : public GroupChatPage
{
    Q_OBJECT
public:
	explicit BookmarksPage(QWidget *parent = 0);
	void setModel(BookmarksModel *model);
signals:
	void bookmarksChanged();
public slots:
	void updateDataForm(qutim_sdk_0_3::DataItem fields);
protected:
	void showEvent(QShowEvent *);
private slots:
	void onCurrentIndexChanged(int index);
	void onSave();
	void onRemove();
private:
	bool isRecent(int index);
	qutim_sdk_0_3::DataItem fields(int index);
	QComboBox *m_bookmarksBox;
	QAction *m_removeAction;
	QPointer<qutim_sdk_0_3::AbstractDataForm> m_dataForm;
};

} // namespace Core

#endif // BOOKMARKSPAGE_H
