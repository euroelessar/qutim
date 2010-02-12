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

namespace Ui {
    class QuetzalJoinChatDialog;
}

class QuetzalJoinChatDialog : public QDialog
{
	Q_OBJECT
public:
	QuetzalJoinChatDialog(PurpleConnection *gc, QWidget *parent = 0);
	QuetzalJoinChatDialog(PurpleConnection *gc, const char *data, QWidget *parent = 0);
	~QuetzalJoinChatDialog();

protected:
	void changeEvent(QEvent *e);

protected slots:
	void onJoinButtonClicked();
	void on_bookmarkBox_currentIndexChanged(int index);

private:
	void init(const char *data);
	Ui::QuetzalJoinChatDialog *ui;
	QPushButton *m_searchButton;
	QPushButton *m_joinButton;
	PurpleConnection *m_gc;
};

#endif // QUETZALJOINCHATDIALOG_H
