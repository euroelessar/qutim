/****************************************************************************
 *  messagingdialog.h
 *
 *  Copyright (c) 2010-2011 by Sidorov Aleksey <sauron@citadelspb.com>
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

#ifndef MESSAGINGDIALOG_H
#define MESSAGINGDIALOG_H
#include <QDialog>

namespace Ui
{
class Dialog;
}

namespace MassMessaging
{
class Manager;
class MessagingDialog : public QDialog
{
	Q_OBJECT
public:
	MessagingDialog();
	~MessagingDialog();
public slots:
	void updateProgressBar(const uint &completed, const uint &total, const QString &message);
private slots:
	void onSendButtonClicked();
	void onManagerFinished(bool ok);
private:
	Ui::Dialog *ui;
	Manager *m_manager;
};
}
#endif // MESSAGINGDIALOG_H
