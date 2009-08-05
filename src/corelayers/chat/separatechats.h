/*
  SeparateChats

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

#ifndef SEPARATECHATS_H
#define SEPARATECHATS_H

#include <QObject>
#include "chatforms/chatwindow.h"
#include "chatforms/conferencewindow.h"
#include <qutim/plugininterface.h>
#include "generalwindow.h"
#include "logscity.h"

using namespace qutim_sdk_0_2;


class SeparateChats : public QObject
{
    Q_OBJECT
public:
    SeparateChats(const QString &profie_name);
    ~SeparateChats();
    void loadSettings();
    void createChat(const TreeModelItem &item,const QStringList &item_info,const QStringList &owner_info);
    void activateWindow(const TreeModelItem &item);
    void contactChangeHisStatus(const TreeModelItem &item, const QIcon &icon);
	void contactChangeHisClient(const TreeModelItem &item);
    bool checkForActivation(const TreeModelItem &item, bool just_check = false);
    void setItemTypingState(const TreeModelItem &item, TypingAttribute);
    void changeId(const TreeModelItem &item, const QString &new_id);
    void alertWindow(const TreeModelItem &item);
    void setConferenceTopic(const TreeModelItem &item, const QString &topic);
	QTextEdit *getEditField(const TreeModelItem &item);
private slots:
    void chatClosed(QObject *win);
    void windowActivatedByUser();
    void closeChat();
private:
    QString m_profile_name;
    QHash<QString,GeneralWindow*> m_chat_list;
    LogsCity &m_logs_city;
    bool m_webkit_mode;
    void restoreWindowSizeAndPosition(GeneralWindow *win);
    bool m_close_after_send;
    void setWindowOptions(GeneralWindow *win);
    bool m_send_on_enter;
    bool m_send_on_double_enter;
    bool m_send_typing_notifications;
    bool m_dont_blink;
};

#endif // SEPARATECHATS_H
