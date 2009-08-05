/*
  ConferenceWindow

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

#ifndef CONFERENCEWINDOW_H
#define CONFERENCEWINDOW_H

#include "ui_conferenceform.h"
#include "../generalwindow.h"
#include "../confcontactlist.h"
#include "../conferencetabcompletion.h"

class ConferenceWindow : public GeneralWindow
{
    Q_OBJECT
public:
    ConferenceWindow(const QString &profile_name, bool webkit_mode);
    ~ConferenceWindow();
    virtual void setItemData(const QStringList &data_list,bool owner_data);
    virtual void setOwnerItem(const TreeModelItem &item);

private slots:
    void on_chatInputEdit_textChanged();
    void on_configButton_clicked();
    void on_topicButton_clicked();
private:
    Ui::ConfForm *ui;
    virtual void setIcons();
    virtual void loadSettings();
    virtual void setNULLs();
    virtual void loadDefaultForm();
    virtual void loadCustomForm(const QString &form_path);
    bool eventFilter(QObject *obj, QEvent *ev);

    QLineEdit *m_topic_edit;
    QToolButton *m_topic_button;
    QToolButton *m_config_button;
    QListView *m_conference_list;
    QSplitter *m_splitter_chat;
    QSplitter *m_splitter_cl;
    ContactListItemDelegate *m_item_delegate;
    ConfContactListEventEater *m_cl_event_eater;
    ConfTabCompletion *m_tab_compl;
};

#endif // CONFERENCEWINDOW_H
