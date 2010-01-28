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

#ifndef VSTATUSOBJECT_H
#define VSTATUSOBJECT_H

#include <QtCore>
#include <QMenu>
#include <QAction>
#include <QToolButton>
#include <QHBoxLayout>
#include "../protocol/vprotocolwrap.h"

class VstatusObject : public QObject
{
    Q_OBJECT
public:
    VstatusObject(const QString &account_name,const QString &profile_name,
		  VprotocolWrap *protocol_wrapper,
		  QObject *parent = 0);
    ~VstatusObject();
    void addButtonToLayout(QHBoxLayout *layout);
    QMenu* getAccountMenu() {return m_status_menu;}
    QIcon getCurrentIcon();
    void loadSettings();
private slots:
    void connectToServer();
    void disconnectFromServer();
    void wrapperDisconnected();
    void wrapperConnected();
signals:
    void iMOffline();
private:
    QMenu *m_status_menu;
    QAction *m_online_action;
    QAction *m_offline_action;
    QToolButton *m_account_button;
    VprotocolWrap *m_protocol_wrapper;
    bool m_current_connected;
    QIcon m_online_icon;
    QIcon m_offline_icon;
    QIcon m_connecting_icon;
    QString m_profile_name;
    QString m_account_name;

};

#endif // VSTATUSOBJECT_H
