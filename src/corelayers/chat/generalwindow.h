/*
  GeneralWindow

    Copyright (c) 2009 by Rustam Chakin <qutim.develop@gmail.com>
						  Ruslan Nigmatullin <euroelessar@gmail.com>

 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
*/

#ifndef GENERALWINDOW_H
#define GENERALWINDOW_H

#include <QObject>
#include <QWidget>
#include <QTextBrowser>
#include <QFile>
#include <QWebView>
#include <QUiLoader>
#include <QPushButton>
#include <QToolButton>
#include <QWidgetAction>
#include <QFormLayout>
#include <QStackedLayout>
#include <QSplitter>
#include "tempglobalinstance.h"
#include "logscity.h"

class ChatEmoticonMenu;

class EventEater : public QObject
{
	Q_OBJECT

public:
   EventEater(QWidget *parent = 0) : QObject(parent), m_send_on_enter(false),
   m_number_of_enters(0), m_enter_position(0), m_accept_files(false) {}
    TreeModelItem m_item;
    bool m_send_on_enter;
    bool m_send_on_double_enter;
    quint8  m_number_of_enters;
	int m_enter_position;
	bool m_accept_files;
signals:
	void focusedIn();
	void closeCurrentChat();
	void changeTab(int);
	void sendMessage();
	void acceptUrls(const QStringList &files);
protected:
	bool eventFilter(QObject *obj, QEvent *event);

};

class GeneralWindow : public QWidget
{
    Q_OBJECT
public:
    GeneralWindow();
	virtual ~GeneralWindow() {}
    virtual void setOwnerItem(const TreeModelItem &item);
    TreeModelItem m_item;
    void windowActivatedByUser(QWidget *window);
	virtual void setItemData(const QStringList &data_list,bool owner_data){}
    bool m_waiting_for_activation;
	virtual void contactTyping(bool typing) {}
    EventEater *m_event_eater;
    virtual void setOptions(bool close_after_send,bool send_on_enter,
			    bool send_on_double_enter, bool send_typing)
    {
	m_close_after_send = close_after_send;
	m_event_eater->m_send_on_enter = send_on_enter;
	m_on_enter_button->setChecked(send_on_enter);
	m_event_eater->m_send_on_double_enter = send_on_double_enter;
	m_send_typing_notifications = send_typing;
    }
    void checkForScrollBarMaximum();
    void updateWebkit(bool update)
    {
	if ( m_web_view && m_webkit_mode ) m_web_view->setUpdatesEnabled(update);
    }
	bool compareItem(const TreeModelItem &item);
	inline QTextEdit *getEditField() { return m_plain_text_edit; }
private slots:
    virtual void on_sendButton_clicked();
    void checkWindowFocus();
    void on_translitButton_clicked();
    void on_onEnterButton_clicked();
    void on_quoteButton_clicked();
    void on_clearChatButton_clicked();
    void browserTextChanged();
    void insertEmoticon(const QString &emoticon_text);
    void newsOnLinkClicked(const QUrl &url);
	void createContextMenu( const QPoint &pos );
signals:
    void windowFocused();
    void closeMe();
protected:
    virtual bool event(QEvent *event);
    virtual void installEventEater();
    TempGlobalInstance &m_global_instance;
    virtual void setIcons();
    virtual void loadSettings();
    virtual void setNULLs();
    virtual void loadDefaultForm() = 0;
    virtual void loadCustomForm(const QString &form_path) = 0;
    virtual void focusTextEdit();
    virtual void setFocusPolicy();
    virtual QString invertMessage(QString &text);
	template<typename T>
	T *morphWidget(QWidget *widget);
    QTextBrowser *m_text_browser;
    QWebView *m_web_view;


    QString m_profile_name;
    QString m_chat_form_path;

    QPushButton *m_send_button;
    QToolButton *m_emoticons_button;
    QToolButton *m_translit_button;
    QToolButton *m_on_enter_button;
    QToolButton *m_quote_button;
    QToolButton *m_clear_button;

    QTextEdit *m_plain_text_edit;

    QString m_identification;
    bool m_webkit_mode;
    bool m_close_after_send;
    bool m_send_typing_notifications;
    bool m_scroll_at_maximum;
    QMenu *m_emotic_menu;
    QWidgetAction *m_emoticon_action;
    ChatEmoticonMenu *m_emoticon_widget;
};

template<typename T>
T *GeneralWindow::morphWidget(QWidget *widget)
{
	if(T *t = qobject_cast<T *>(widget))
		return t;
	T *t = new T(this);
	QRect geom = widget->geometry();
	if(QLayout *layout = widget->parentWidget()->layout())
	{
		int index = layout->indexOf(widget);
		if(QGridLayout *grid_layout = qobject_cast<QGridLayout *>(layout))
		{
			int row, column, row_span, column_span;
			grid_layout->getItemPosition(index, &row, &column, &row_span, &column_span);
			grid_layout->removeWidget(widget);
			grid_layout->addWidget(t, row, column, row_span, column_span);
		}
		else if(QBoxLayout *box_layout = qobject_cast<QBoxLayout *>(layout))
		{
			box_layout->insertWidget(index, t);
		}
		else if(QFormLayout *form_layout = qobject_cast<QFormLayout *>(layout))
		{
			int row;
			QFormLayout::ItemRole role;
			form_layout->getItemPosition(index, &row, &role);
			form_layout->removeWidget(widget);
			form_layout->setWidget(row, role, t);
		}
		else if(QStackedLayout *stacked_layout = qobject_cast<QStackedLayout *>(layout))
		{
			stacked_layout->insertWidget(index, widget);
		}
	}
	else if(QSplitter *splitter = qobject_cast<QSplitter *>(widget->parentWidget()))
	{
		int index = splitter->indexOf(widget);
		splitter->insertWidget(index, t);
	}
	delete widget;
	t->setGeometry(geom);
	return t;
}

#endif // GENERALWINDOW_H
