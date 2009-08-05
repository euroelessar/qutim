/*
  ChatWindow

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

#ifndef CHATWINDOW_H
#define CHATWINDOW_H

#include "ui_chatform.h"
#include "../generalwindow.h"
#include <QContextMenuEvent>

class ChatWindow : public GeneralWindow
{
    Q_OBJECT
	enum TypingState
	{
		InitTyping,
		NotStartedTyping,
		IsTyping,
		FinishedTyping
	};
public:
    ChatWindow(const QString &profile_name,bool webkit_mode);
    ~ChatWindow();
    virtual void setItemData(const QStringList &data_list,bool owner_data);
    virtual void contactTyping(bool typing);
    virtual void setOwnerItem(const TreeModelItem &item);
    virtual void setOptions(bool close_after_send,bool send_on_enter,
			    bool send_on_double_enter, bool send_typing)
    {
	GeneralWindow::setOptions(close_after_send,send_on_enter,send_on_double_enter,send_typing);
	if ( m_send_typing_button) m_send_typing_button->setChecked(send_typing);

    }
protected:
	void contextMenuEvent(QContextMenuEvent * event);
private slots:
    void on_chatInputEdit_textChanged();
    void typingNow();
    void on_historyButton_clicked();
    void on_sendPictureButton_clicked();
    void on_sendFileButton_clicked();
    void on_infoButton_clicked();
    void on_typingButton_clicked();
	void acceptFiles(const QStringList &files);
private:
    Ui::ChatForm *ui;

    virtual void setIcons();
    virtual void loadSettings();
    virtual void setNULLs();
    virtual void loadDefaultForm();
    virtual void loadCustomForm(const QString &form_path);
    QLabel *m_contact_avatar_label;
    QLabel *m_name_label;
    QLabel *m_id_label;
    QLabel *m_cliend_id_label;
    QLabel *m_owner_avatar_label;
    QLabel *m_additional_label;
    QLabel *m_typing_label;
    QLabel *m_client_label;
    QToolButton *m_info_button;
    QToolButton *m_history_button;
    QToolButton *m_send_picture_button;
    QToolButton *m_send_file_button;
    QToolButton *m_send_typing_button;
    QSplitter *m_main_splitter;
    QString m_client_id;
	TypingState m_typing_state;
	QString m_typing_label_html;
    QWebPage *m_web_page;
};

#endif // CHATWINDOW_H
