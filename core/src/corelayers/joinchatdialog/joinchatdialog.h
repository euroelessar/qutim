/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2008 Denis Daschenko <daschenko@gmail.com>
** Copyright © 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
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

#ifndef JJOINCHAT_H
#define JJOINCHAT_H

#include <qutim/groupchatmanager.h>
#include <qutim/dataforms.h>
#include <qutim/status.h>
#include <QDialog>
#include <QPointer>

class QListWidgetItem;

namespace Ui
{
class JoinChat;
}

class JoinChatDialog : public QDialog
{
	Q_OBJECT

public:
	JoinChatDialog(QWidget *parent = 0);
	~JoinChatDialog();
public slots:
	void showConference(QListWidgetItem *current, QListWidgetItem *previous);
	void on_addConferenceButton_clicked();
	void on_removeConferenceButton_clicked();
	void on_accountBox_currentIndexChanged(int index);
	void setUri(const QString &uri);
protected:
	bool eventFilter(QObject *obj, QEvent *event);
	void rebuildItems(int index);
private slots:
	void onAccountCreated(qutim_sdk_0_3::Account *account);
	void onAccountStatusChanged(const qutim_sdk_0_3::Status &status);
	void onManagerChanged(qutim_sdk_0_3::GroupChatManager *manager);
	void onAccountDeath(QObject *object);
	void joinConference();
	void onDataChanged();
	void onSaveButtonClicked();
	void joinBookmark(QListWidgetItem *item);

private:
	void addAccount(qutim_sdk_0_3::Account *account);

private:
	qutim_sdk_0_3::GroupChatManager *groupChatManager();

	QString m_uri;
	Ui::JoinChat *m_ui;
	QPointer<qutim_sdk_0_3::AbstractDataForm> m_dataForm;
	qutim_sdk_0_3::Account *m_currentAcount;

};

#endif //JJOINCHAT_H

