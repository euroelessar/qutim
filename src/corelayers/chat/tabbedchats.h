/*
  TabbedChats

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

#ifndef TABBEDCHATS_H
#define TABBEDCHATS_H

#include <QWidget>
#include <qutim/plugininterface.h>
#include <QTabBar>
#include <QVBoxLayout>
#include "chatforms/chatwindow.h"
#include "chatforms/conferencewindow.h"
#include "logscity.h"

struct TabbedWindow {
  QTabBar *m_tab_bar;
  QWidget *m_main_window;
  QVBoxLayout *m_main_layout;
};

struct UserSpace {
    TreeModelItem m_item;
    QString m_identification;
    QStringList m_item_info;
    QStringList m_owner_info;
    quint8 m_state;
};

enum TabbedWindowType {
  MergedWindow = 0, PrivatesWindow, ConferencesWindow
};

using namespace qutim_sdk_0_2;


class TabbedChats : public QObject
{
    Q_OBJECT
public:
    TabbedChats(const QString &profile_name);
    ~TabbedChats();
    void loadSettings();
    void createChat(const TreeModelItem &item,const QStringList &item_info,const QStringList &owner_info,bool new_message);
    void activateWindow(const TreeModelItem &item,bool new_message);
    void contactChangeHisStatus(const TreeModelItem &item, const QIcon &icon);
	void contactChangeHisClient(const TreeModelItem &item);
    bool checkForActivation(const TreeModelItem &item, bool just_check = false);
    void setItemTypingState(const TreeModelItem &item, TypingAttribute);
    void changeId(const TreeModelItem &item, const QString &new_id);
    void alertWindow(const TreeModelItem &item);
    void setConferenceTopic(const TreeModelItem &item, const QString &topic);
	QTextEdit *getEditField(const TreeModelItem &item);
    void updateWebkit(bool update);
private slots:
    void destroyMergedWindow(QObject *);
    void destroyPrivatesWindow(QObject *);
    void destroyConferencesWindow(QObject *);
    void tabChanged(int);
    void tabClosed(int);
    void closeAllTabs();
    void windowActivatedByUser();
    void closeCurrentChat();
    void changeTab(int offset);
protected:
    bool eventFilter(QObject *obj, QEvent *event);

signals:
    void restorePreviousTabs();
private:
    QString m_profile_name;
    bool m_merge_conf_and_chats;
    TabbedWindow m_merged_window;
    TabbedWindow m_private_chats_window;
    TabbedWindow m_all_conference_window;
    ChatWindow *m_chat_window;
    ConferenceWindow *m_conference_window;
    void createTabbedWindow(TabbedWindow *window);
    LogsCity &m_logs_city;
    bool m_webkit_mode;
    bool m_closable_tabs;
    bool m_movable_tabs;
    bool m_remember_privates;
    void deleteTabData(QTabBar *tab_bar,int index);
    QTabBar *getBarForItemType(int type);
    QWidget *getMainWindowForItemType(int type);
    GeneralWindow *getGeneralWindowForItemType(int type);
    QColor getColorForState(quint8 state);
    void connectGeneralWindow(GeneralWindow *general_window);
    void saveSizeAndPosition(QWidget *win, TabbedWindowType type);
    void restoreSizeAndPosition(QWidget *win, TabbedWindowType type);
    bool m_close_after_send;
    void setWindowOptions(GeneralWindow *win);
    bool m_send_on_enter;
    bool m_send_on_double_enter;
    bool m_send_typing_notifications;
    bool m_dont_blink;
};

#endif // TABBEDCHATS_H
