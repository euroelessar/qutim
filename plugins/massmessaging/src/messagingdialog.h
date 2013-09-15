/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Aleksey Sidorov <gorthauer87@yandex.ru>
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

#ifndef MESSAGINGDIALOG_H
#define MESSAGINGDIALOG_H
#include <QDialog>

namespace Ui
{
class Dialog;
}

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

#endif // MESSAGINGDIALOG_H

