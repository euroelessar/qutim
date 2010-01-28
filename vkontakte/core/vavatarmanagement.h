/*
    VavatarManagement

    Copyright (c) 2009 by Alexander Kazarin <boiler@co.ru>

 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
*/

#ifndef VAVATARMANAGEMENT_H
#define VAVATARMANAGEMENT_H

#include <QtCore>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>

class VavatarManagement : public QObject
{
    Q_OBJECT
public:
    VavatarManagement(const QString &account_name,
                      const QString &profile_name, QObject *parent = 0);
    ~VavatarManagement();
    void requestForAvatar(const QString &buddy_id, const QString &avatar_url);

private slots:
    void replyFinished(QNetworkReply*);

signals:
    void avatarDownloaded(const QString &, const QByteArray &);

private:
    QString m_account_name;
    QString m_profile_name;
    QHash<QString,QNetworkAccessManager*> m_request_list;
};

#endif // VAVATARMANAGEMENT_H
