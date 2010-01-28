/*
    Vaccount

    Copyright (c) 2009 by Rustam Chakin <qutim.develop@gmail.com>

 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
*/

#ifndef VACCOUNT_H
#define VACCOUNT_H
#include <QHBoxLayout>
#include <QMenu>
#include "vstatusobject.h"
#include "../protocol/vprotocolwrap.h"
#include "vcontactlist.h"

class Vaccount : public QObject
{
    Q_OBJECT
public:
    Vaccount(const QString &account, const QString &m_profile_name);
    ~Vaccount();
    void createAccountButton(QHBoxLayout *layout);
    QMenu *getAccountMenu();
    void editAccountSettings();
    bool m_edit_dialog_opened;
    QIcon getCurrentStatusIcon();
    void loadSettings();
    void removeCL();
    QString getToolTip(const QString &buddy_name);
    void sendMessage(const QString &buddy_id, const QString &message);
    QStringList getBuddyInfo(const QString &buddy_id);
    inline VcontactList *getContactList() const { return m_cl_object;}
private slots:
    void editAccountSettingsClosed(QObject*);

private:
    QString m_profile_name;
    QString m_account_name;
    VstatusObject *m_status_object;
    VprotocolWrap *m_protocol_wrapper;
    VcontactList *m_cl_object;
};

#endif // VACCOUNT_H
