/*
    VprotocolWrap

    Copyright (c) 2009 by Rustam Chakin <qutim.develop@gmail.com>
				  2009 by Alexander Kazarin <boiler@co.ru>

 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
*/

#ifndef VPROTOCOLWRAP_H
#define VPROTOCOLWRAP_H


#include <QtCore>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QScriptEngine>

enum CurrentConnectionState {Online,Connecting,Offline, ObsoleteSID};

struct FriendBuddy {
    QString m_id;
    QString m_avatar_url;
    QString m_name;
    bool m_online;
};

struct Activity {
    QString m_id;
    QString m_activity;
};

struct Message {
    QString m_sender_id;
    QString m_message;
    QDateTime m_time;
    QString m_tmp_name;
    QString m_tmp_avatar_url;
};

struct Photo {
	QString m_id;
	QString m_photo_id;
	QString m_preview_url;
	QString m_photo_url;
};

class VprotocolWrap : public QObject
{
    Q_OBJECT
public:
    VprotocolWrap(const QString &account_name, const QString &profile_name);
    ~VprotocolWrap();

    //Authorisation
    void sendAuthorisation();
    void sendLogout();

    //Data: friend list

    void sendMessage(const QString &buddy_id, const QString &message);
    void loadSettings();

private slots:
    void getReply(QNetworkReply*);
    void sendProlongation();
    void sendRequestForFriendList();
    void checkForNewMessages();
	void sendRequestForNews();

signals:
    void iMDisconnected();
    void iMConnected();
    void friendListArrived(QList<FriendBuddy> &friends);
	void faveListArrived(QList<FriendBuddy> &friends);
    void activitiesListArrived(QList<Activity>&);
    void getNewMessages(QList<Message> &messages);

private:
    QNetworkAccessManager *m_network_handler;
		QNetworkRequest m_network_request;
    QString m_account_name;
    QString m_profile_name;
    CurrentConnectionState m_current_state;
    QByteArray m_mine_sid;
    QByteArray m_mine_id;
    QByteArray m_remixpassword;
	QHash<QString, QString> m_friend_names;
    quint32 m_refresh_friend_list;
    void processProfileData(QString data);
	void processFaves(QString data);
    void askForFriendsActivity();
    void processActiveData(QString data);
    quint32 m_check_for_messages_every;
    void setNewMessagesCount(QString data);
    quint32 m_new_messages_count;
    void getNewMessages(quint32 count);
    void processMessagesArray(QString data);
    void markMessagesAsReaded(QString list);
	quint32 m_check_for_news_every;
	void processNews(QString data);
	bool m_news_photo_enable;
	bool m_news_photo_preview;
	bool m_news_photo_fullsize;

    int checkForErrorReply(QString data);
    void stopTimers();
    QTimer *prolongation;
    QTimer *friendsUpdate;
    QTimer *messagesUpdate;
    QTimer *newsUpdate;

};

#endif // VPROTOCOLWRAP_H
