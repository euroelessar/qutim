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

#include "conferencewindow.h"
#include "../chatemoticonmenu.h"

ConferenceWindow::ConferenceWindow(const QString &profile_name,bool webkit_mode)
{
	m_profile_name = profile_name;
	m_webkit_mode = webkit_mode;
	setNULLs();
	loadSettings();

	QString window_file_path;
	if ( m_webkit_mode )
	{
		window_file_path = m_chat_form_path + "/webconfwindow.ui";
	}
	else
	{
		window_file_path = m_chat_form_path + "/textconfwindow.ui";
	}

	if (m_chat_form_path.isEmpty() || !QFile::exists(window_file_path) )
		loadDefaultForm();
	else
		loadCustomForm(window_file_path);

	setAttribute(Qt::WA_QuitOnClose, false);
	setAttribute(Qt::WA_DeleteOnClose, true);
	setIcons();
	if ( m_conference_list )
	{
		m_item_delegate = new ContactListItemDelegate();
		m_item_delegate->setTreeView(m_conference_list);
		m_conference_list->setItemDelegate(m_item_delegate);
		m_conference_list->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
		m_conference_list->setSelectionMode(QAbstractItemView::SingleSelection);
		m_conference_list->setSelectionBehavior(QAbstractItemView::SelectItems);
		m_cl_event_eater = new ConfContactListEventEater();
		m_conference_list->findChild<QObject *>("qt_scrollarea_viewport")->installEventFilter(m_cl_event_eater);
		m_conference_list->installEventFilter(m_cl_event_eater);

	}

	if ( m_plain_text_edit)
	{
		m_tab_compl = new ConfTabCompletion(this);
		m_tab_compl->setTextEdit(m_plain_text_edit);
		m_plain_text_edit->installEventFilter(this);
	}
	GeneralWindow::installEventEater();
	GeneralWindow::setFocusPolicy();
	focusTextEdit();
	if ( m_splitter_chat && m_splitter_cl )
	{
		QSettings settings(QSettings::defaultFormat(), QSettings::UserScope, "qutim/qutim."+m_profile_name, "profilesettings");
		settings.beginGroup("conference");
		m_splitter_chat->restoreState(settings.value("splitter1", QByteArray()).toByteArray());
		m_splitter_cl->restoreState(settings.value("splitter2", QByteArray()).toByteArray());
		settings.endGroup();
	}
	if ( m_send_button ) m_send_button->setEnabled(false);
}

ConferenceWindow::~ConferenceWindow()
{
    if ( m_splitter_chat && m_splitter_cl )
    {
	QSettings settings(QSettings::defaultFormat(), QSettings::UserScope, "qutim/qutim."+m_profile_name, "profilesettings");
	settings.beginGroup("conference");
	settings.setValue("splitter1", m_splitter_chat->saveState());
	settings.setValue("splitter2", m_splitter_cl->saveState());
	settings.endGroup();
    }


    delete m_tab_compl;
    delete m_cl_event_eater;
    delete m_item_delegate;
    delete m_topic_edit;
    delete m_topic_button;
    delete m_config_button;
//    if ( m_emoticon_widget )
//	    delete m_emoticon_widget;
//    if ( m_emotic_menu )
//	    delete m_emotic_menu;
    delete m_emoticons_button;
    delete m_translit_button;
    delete m_on_enter_button;
    delete m_quote_button;
    delete m_clear_button;
    delete m_send_button;
    delete m_text_browser;
    //delete m_web_view;
    delete m_conference_list;
    delete m_splitter_chat;
    delete m_splitter_cl;
}

void ConferenceWindow::setIcons()
{
	if( m_topic_button )
		m_topic_button->setIcon(Icon("conftopic"));
	if( m_config_button )
		m_config_button->setIcon(Icon("confsettings"));
    GeneralWindow::setIcons();
}

void ConferenceWindow::loadSettings()
{
    GeneralWindow::loadSettings();
}

void ConferenceWindow::setNULLs()
{
    GeneralWindow::setNULLs();
    m_topic_edit = 0;
    m_topic_button = 0;
    m_config_button = 0;
    m_conference_list = 0;
    m_splitter_chat = 0;
    m_splitter_cl = 0;
}

void ConferenceWindow::loadDefaultForm()
{
    ui = new Ui::ConfForm;
    ui->setupUi(this);

    m_topic_edit = ui->topicLineEdit;
    m_topic_button = ui->topicButton;
    m_config_button = ui->configButton;
    m_emoticons_button = ui->emoticonButton;
    m_translit_button = ui->translitButton;
    m_on_enter_button = ui->onEnterButton;
    m_quote_button = ui->quoteButton;
    m_clear_button = ui->clearChatButton;
    m_send_button = ui->sendButton;
    m_web_view = 0;
    m_conference_list = ui->conferenceList;
    m_splitter_chat = ui->splitter;
    m_splitter_cl = ui->splitter_2;
    m_plain_text_edit = ui->chatInputEdit;

    if ( m_webkit_mode )
    {
	QFrame *frame = new QFrame(this);
	QGridLayout *layout = new QGridLayout(frame);
	m_web_view = new QWebView(this);
	layout->addWidget(m_web_view);
	frame->setLayout(layout);
	layout->setMargin(0);
	frame->setFrameShape(QFrame::StyledPanel);
	frame->setFrameShadow(QFrame::Sunken);


	ui->splitter->insertWidget(0, frame);
    }
    else
    {
	m_text_browser = new QTextBrowser(this);
	ui->splitter->insertWidget(0, m_text_browser);
    }
}

void ConferenceWindow::loadCustomForm(const QString &form_path)
{
    QUiLoader loader;
    QFile file(form_path);
    if ( file.open(QFile::ReadOnly) )
    {
	QWidget *chat_widget = loader.load(&file, this);
	file.close();

	m_topic_edit = qFindChild<QLineEdit*>(this, "topicLineEdit");
	m_topic_button = qFindChild<QToolButton*>(this, "topicButton");
	m_config_button = qFindChild<QToolButton*>(this, "configButton");
	m_emoticons_button = qFindChild<QToolButton*>(this, "emoticonButton");
	m_translit_button = qFindChild<QToolButton*>(this, "translitButton");
	m_on_enter_button = qFindChild<QToolButton*>(this, "onEnterButton");
	m_quote_button = qFindChild<QToolButton*>(this, "quoteButton");
	m_clear_button = qFindChild<QToolButton*>(this, "clearChatButton");
	m_send_button = qFindChild<QPushButton*>(this, "sendButton");
	m_web_view = 0;
	m_conference_list = qFindChild<QListView*>(this, "conferenceList");
	m_splitter_chat = qFindChild<QSplitter*>(this, "splitter");
	m_splitter_cl = qFindChild<QSplitter*>(this, "splitter_2");
	m_plain_text_edit = qFindChild<QTextEdit*>(this, "chatInputEdit");

	if ( m_web_view )
	{
		QWidget *widget = qFindChild<QWidget *>(this, "webView");
		m_web_view = morphWidget<QWebView>(widget);
	}
	else
	    m_text_browser = qFindChild<QTextBrowser*>(this, "chatViewBrowser");

	QMetaObject::connectSlotsByName(this);
	QVBoxLayout *layout = new QVBoxLayout;
	layout->setMargin(0);
	layout->addWidget(chat_widget);
	setLayout(layout);
    }
}

void ConferenceWindow::setItemData(const QStringList &data_list,bool owner_data)
{
    if ( owner_data )
    if (data_list.count() > 0 )
    {
	QString topic = data_list.at(0);
	if (m_topic_edit) m_topic_edit->setText(topic.replace("\n"," "));
    } else if ( m_topic_edit )m_topic_edit->clear();

}


void ConferenceWindow::setOwnerItem(const TreeModelItem &item)
{
    GeneralWindow::setOwnerItem(item);
    if ( m_conference_list)
	LogsCity::instance().setConferenceListView(item,m_conference_list);
    if ( m_cl_event_eater )
    {
	m_cl_event_eater->m_contact_list = LogsCity::instance().getConferenceCL(item);
	m_tab_compl->setContactList(m_cl_event_eater->m_contact_list);
    }
}

bool ConferenceWindow::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == m_plain_text_edit && event->type() == QEvent::KeyPress)
    {
	    QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
	    if ( keyEvent->key() == Qt::Key_Tab )
	    {
		    m_tab_compl->tryComplete();
		    return true;
	    }

	    m_tab_compl->reset();
	    return false;
    }
    return QObject::eventFilter( obj, event );
}

void ConferenceWindow::on_chatInputEdit_textChanged()
{
    if ( m_send_button )
	    m_send_button->setEnabled(!m_plain_text_edit->toPlainText().isEmpty());
}

void ConferenceWindow::on_configButton_clicked()
{

    m_global_instance.showConferenceMenu(m_item,mapToGlobal(m_config_button->pos()));
}

void ConferenceWindow::on_topicButton_clicked()
{
    m_global_instance.showTopicConfig(m_item);
}
