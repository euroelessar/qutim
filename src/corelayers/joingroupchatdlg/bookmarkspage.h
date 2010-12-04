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
	void updateDataForm(const QString &name = QString());
protected:
	void showEvent(QShowEvent *);
private slots:
	void onCurrentIndexChanged(int index);
	void onSave();
	void onRemove();
private:
	bool isRecent(int index);
	QComboBox *m_bookmarksBox;
	QAction *m_removeAction;
	QPointer<QWidget> m_dataForm;
};

} // namespace Core

#endif // BOOKMARKSPAGE_H
