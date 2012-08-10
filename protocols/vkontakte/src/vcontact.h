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

#ifndef VCONTACT_H
#define VCONTACT_H
#include <qutim/contact.h>
#include <qutim/chatsession.h>
#include <QPointer>
#include <vreen/contact.h>

namespace Vreen {
class ChatSession;
class Message;
}

class VAccount;
class VContactPrivate;
class VContact : public qutim_sdk_0_3::Contact
{
	Q_OBJECT
public:
	VContact(Vreen::Buddy *contact, VAccount* account);

	virtual QString id() const;
	virtual bool isInList() const;
	virtual bool sendMessage(const qutim_sdk_0_3::Message& message);
	virtual void setTags(const QStringList& tags);
	virtual void setInList(bool inList);
	virtual qutim_sdk_0_3::Status status() const;
	virtual ~VContact();
	virtual QStringList tags() const;
	virtual QString name() const;
	virtual void setName(const QString& name);
	virtual QString avatar() const;
	QString activity() const;

	void handleMessage(const Vreen::Message &message);
    Vreen::Client *client() const;
    Vreen::Buddy *buddy() const;
protected:
	void setStatus(const qutim_sdk_0_3::Status &status);
	Vreen::ChatSession *chatSession();
	virtual bool event(QEvent *ev);
	void downloadAvatar(const QString &url);
public slots:
	void setTyping(bool set = false);
private slots:
	void onActivityChanged(const QString &activity);
	void onStatusChanged(Vreen::Contact::Status);
	void onTagsChanged(const QStringList &tags);
	void onNameChanged(const QString &name);
	void onMessageSent(const QVariant &response);
	void onUnreadChanged(qutim_sdk_0_3::MessageList unread);
	void onSessionCreated(qutim_sdk_0_3::ChatSession *session);
	void onPhotoSourceChanged(const QString &source, Vreen::Contact::PhotoSize);
	void onAvatarDownloaded(const QString &path);
private:
	Vreen::Buddy *m_buddy;
	QPointer<QTimer> m_typingTimer;
	QPointer<Vreen::ChatSession> m_chatSession;
	qutim_sdk_0_3::Status m_status;
	QString m_name;
	QStringList m_tags;
	QString m_avatar;
	qutim_sdk_0_3::MessageList m_unreadMessages;
	typedef QList<QPair<int, int> > SentMessagesList;
	SentMessagesList m_sentMessages;
};

Q_DECLARE_METATYPE(VContact*)

#endif // VCONTACT_H

