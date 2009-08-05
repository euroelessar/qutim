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

#include "chatwindow.h"
#include "iconmanager.h"
#include <QTimer>
#include <QFileDialog>
#include <QMessageBox>
#include "../chatemoticonmenu.h"

ChatWindow::ChatWindow(const QString &profile_name, bool webkit_mode)
{
    m_profile_name = profile_name;
    m_webkit_mode = webkit_mode;
    setNULLs();
    loadSettings();

    QString window_file_path;
	if ( m_webkit_mode )
	{
		window_file_path = m_chat_form_path + "/webchatwindow.ui";
	}
	else
	{
		window_file_path = m_chat_form_path + "/textchatwindow.ui";
	}



    if (m_chat_form_path.isEmpty() || !QFile::exists(window_file_path) )
		loadDefaultForm();
    else
		loadCustomForm(window_file_path);


    setAttribute(Qt::WA_QuitOnClose, false);
    setAttribute(Qt::WA_DeleteOnClose, true);
    setIcons();
    GeneralWindow::installEventEater();
    GeneralWindow::setFocusPolicy();
    focusTextEdit();
    if ( m_main_splitter )
    {
		QSettings settings(QSettings::defaultFormat(), QSettings::UserScope, "qutim/qutim."+m_profile_name, "profilesettings");
		m_main_splitter->restoreState(settings.value("chatwindow/splitter", QByteArray()).toByteArray());
	}
	m_typing_state = InitTyping;
    if ( m_send_button ) m_send_button->setEnabled(false);
    //if ( m_webkit_mode && m_web_page ) m_web_page = m_web_view->page();
	m_typing_label_html = "<img src='" + IconManager::instance().getIconPath("typing", IconInfo::System) + "'>";
	setAcceptDrops(true);
	m_event_eater->m_accept_files = true;
	connect(m_event_eater, SIGNAL(acceptUrls(QStringList)), this, SLOT(acceptFiles(QStringList)));

}

ChatWindow::~ChatWindow()
{
	if(m_send_typing_notifications && m_typing_state > NotStartedTyping)
		m_global_instance.sendTypingNotification(m_item, 0);

    if ( m_main_splitter )
    {
		QSettings settings(QSettings::defaultFormat(), QSettings::UserScope, "qutim/qutim."+m_profile_name, "profilesettings");
		settings.setValue("chatwindow/splitter", m_main_splitter->saveState());
    }

    delete m_send_button;
    delete m_plain_text_edit;
    delete m_text_browser;
   // m_web_page = 0;
    //m_web_view->setPage(m_web_page);
    delete m_web_view;

    delete m_contact_avatar_label;
    delete m_name_label;
    delete m_id_label;
    delete m_cliend_id_label;
    delete m_owner_avatar_label;
    delete m_additional_label;
    delete m_info_button;
	delete m_history_button;
//    if ( m_emotic_menu )
//	    delete m_emotic_menu;
//	if ( m_emoticon_widget )
//		delete m_emoticon_widget;
    delete m_emoticons_button;

    delete m_send_file_button;
    delete m_send_picture_button;
    delete m_main_splitter;
    delete ui;
}

void ChatWindow::setNULLs()
{
	GeneralWindow::setNULLs();


	m_contact_avatar_label = 0;
	m_name_label = 0;
	m_id_label = 0;
	m_cliend_id_label = 0;
	m_owner_avatar_label = 0;
	m_additional_label = 0;
	m_info_button = 0;

	m_history_button = 0;
	m_send_picture_button = 0;
	m_send_file_button = 0;

	m_send_typing_button = 0;
	m_typing_label = 0;
	m_client_label = 0;
	m_main_splitter = 0;
	ui = 0;
}

void ChatWindow::loadDefaultForm()
{
    ui = new Ui::ChatForm;
    ui->setupUi(this);

    m_send_button = ui->sendButton;
    m_plain_text_edit = ui->chatInputEdit;
    m_typing_label = ui->typingLabel;
    m_client_label = ui->clientLabel;
    m_info_button = ui->infoButton;
    m_history_button = ui->historyButton;
    m_emoticons_button = ui->emoticonButton;
    m_send_picture_button = ui->sendPictureButton;
    m_send_file_button = ui->sendFileButton;
    m_on_enter_button = ui->onEnterButton;
    m_send_typing_button = ui->typingButton;
    m_translit_button = ui->translitButton;
    m_quote_button = ui->quoteButton;
    m_clear_button = ui->clearChatButton;
    m_main_splitter= ui->splitter;
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

void ChatWindow::loadCustomForm(const QString &form_path)
{
    QUiLoader loader;
    QFile file(form_path);
    if ( file.open(QFile::ReadOnly) )
    {
	QWidget *chat_widget = loader.load(&file, this);
	file.close();
	m_send_button = qFindChild<QPushButton*>(this, "sendButton");
	m_plain_text_edit = qFindChild<QTextEdit*>(this, "chatInputEdit");
	m_contact_avatar_label = qFindChild<QLabel*>(this, "contactAvatarLabel");
	m_name_label = qFindChild<QLabel*>(this, "nickNameLabel");
	m_id_label = qFindChild<QLabel*>(this, "idLabel");
	m_typing_label = qFindChild<QLabel*>(this, "typingLabel");
	m_cliend_id_label = qFindChild<QLabel*>(this, "typingClientLabel");
	m_owner_avatar_label = qFindChild<QLabel*>(this, "ownAvatarLabel");
	m_additional_label = qFindChild<QLabel*>(this, "additionalLabel");
	m_client_label = qFindChild<QLabel*>(this, "clientLabel");
	m_info_button = qFindChild<QToolButton*>(this, "infoButton");
	m_history_button = qFindChild<QToolButton*>(this, "historyButton");
	m_emoticons_button = qFindChild<QToolButton*>(this, "emoticonButton");
	m_send_picture_button = qFindChild<QToolButton*>(this, "sendPictureButton");
	m_send_file_button = qFindChild<QToolButton*>(this, "sendFileButton");
	m_on_enter_button = qFindChild<QToolButton*>(this, "onEnterButton");
	m_send_typing_button = qFindChild<QToolButton*>(this, "typingButton");
	m_translit_button = qFindChild<QToolButton*>(this, "translitButton");
	m_quote_button = qFindChild<QToolButton*>(this, "quoteButton");
	m_clear_button = qFindChild<QToolButton*>(this, "clearChatButton");
	m_main_splitter= qFindChild<QSplitter*>(this, "splitter");

	if ( m_webkit_mode )
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

void ChatWindow::setIcons()
{
    	if (m_info_button) m_info_button->setIcon(m_global_instance.getIcon("contactinfo") );

    	if ( m_history_button) m_history_button->setIcon(m_global_instance.getIcon("history") );

    	if (m_send_picture_button) m_send_picture_button->setIcon(m_global_instance.getIcon("image") );

    	if ( m_send_file_button) m_send_file_button->setIcon(m_global_instance.getIcon("save_all") );

    	if ( m_on_enter_button) m_on_enter_button->setIcon(m_global_instance.getIcon("key_enter") );

    	if (m_send_typing_button ) m_send_typing_button->setIcon(m_global_instance.getIcon("typing") );

	GeneralWindow::setIcons();
}

void ChatWindow::loadSettings()
{
    GeneralWindow::loadSettings();
}

void ChatWindow::setItemData(const QStringList &data_list,bool owner_data)
{

    if ( !owner_data )
    {
	if ( data_list.count() > 0)
	{
		if ( m_name_label )
			m_name_label->setText(data_list.at(0));
	} else if ( m_name_label )
	    m_name_label->clear();
	if ( data_list.count() > 1 )
	{
		if ( m_contact_avatar_label)
			m_contact_avatar_label->setPixmap(data_list.at(1).isEmpty()?QPixmap():QPixmap(data_list.at(1)));
	} else
	    if( m_contact_avatar_label )
		m_contact_avatar_label->setPixmap(QPixmap());
	if ( data_list.count() > 2 )
	{
		if ( m_cliend_id_label )
			m_cliend_id_label->setText(data_list.at(2));
		if ( m_client_label )
			m_client_label->setToolTip(data_list.at(2));
		m_client_id = data_list.at(2);
	} else {
	    m_client_id.clear();
	    if ( m_cliend_id_label )
		    m_cliend_id_label->clear();
	    if ( m_client_label )
		    m_client_label->clear();
	}
	if ( data_list.count() > 3 )
	{
		if ( m_additional_label )
		{
		    QString add_data = data_list.at(3);
		    m_additional_label->setText(add_data.remove('\n'));
		}
	} else if ( m_additional_label )
	    m_additional_label->clear();
	if ( m_id_label )
		m_id_label->setText(m_item.m_item_name);

	if ( m_client_label )
	{
		m_client_label->setPixmap(m_global_instance.getContactIcon(m_item,12).pixmap(16));
	}
    } else {
	if ( data_list.count() > 1 )
	{
		if ( m_owner_avatar_label)
			m_owner_avatar_label->setPixmap(data_list.at(1).isEmpty()?QPixmap():QPixmap(data_list.at(1)));
	} else if (m_owner_avatar_label)
	    m_owner_avatar_label->setPixmap(QPixmap());
    }
}

void ChatWindow::contactTyping(bool typing)
{
	if ( m_cliend_id_label )
		m_cliend_id_label->setText(typing?tr("<font color='green'>Typing...</font>"):
			m_client_id);
	if ( m_typing_label )
		m_typing_label->setText(typing?m_typing_label_html:QString());
}

void ChatWindow::on_chatInputEdit_textChanged()
{
	if ( m_send_typing_notifications)
	{
		if( m_plain_text_edit->toPlainText().isEmpty() && m_typing_state > NotStartedTyping )
		{
			m_typing_state = NotStartedTyping;
			m_global_instance.sendTypingNotification(m_item, 0);
		}
		else if( m_typing_state == NotStartedTyping )
		{
			m_typing_state = FinishedTyping;
			m_global_instance.sendTypingNotification(m_item, 2);
			QTimer::singleShot(5000, this, SLOT(typingNow()));
		}
		else if( m_typing_state == InitTyping )
			m_typing_state = NotStartedTyping;
		else
			m_typing_state = IsTyping;
	}

		if ( m_send_button )
			m_send_button->setEnabled(!m_plain_text_edit->toPlainText().isEmpty());
}

void ChatWindow::typingNow()
{
	if ( m_send_typing_notifications )
	{
		if( m_typing_state == IsTyping )
		{
			m_typing_state = FinishedTyping;
			m_global_instance.sendTypingNotification(m_item, 1);
			QTimer::singleShot(5000, this, SLOT(typingNow()));
		}
		else if( m_typing_state == FinishedTyping )
		{
			m_typing_state = NotStartedTyping;
			m_global_instance.sendTypingNotification(m_item, 0);
		}
	}
}

void ChatWindow::setOwnerItem(const TreeModelItem &item)
{
	if (m_send_typing_notifications && m_typing_state > NotStartedTyping)
		m_global_instance.sendTypingNotification(m_item, 0);
	m_typing_state = InitTyping;
    GeneralWindow::setOwnerItem(item);

}

void ChatWindow::on_historyButton_clicked()
{
    m_global_instance.openHistoryFor(m_item);
}

void ChatWindow::on_sendPictureButton_clicked()
{
	QString fileName =   QFileDialog::getOpenFileName(this, tr("Open File"),
			 "",
							 tr("Images (*.gif *.png *.bmp *.jpg *.jpeg)"));

	if ( !fileName.isEmpty())
	{
		QFile iconFile(fileName);
		if ( iconFile.size() > (7 * 1024 + 600))
		{
			QMessageBox::warning(this, tr("Open error"),
					   tr("Image size is too big"));
		}
		else
		{
			if (iconFile.open(QIODevice::ReadOnly))
			{
			    QByteArray image_array = iconFile.readAll();
				m_global_instance.sendImageTo(m_item, image_array);
				LogsCity::instance().addImage(m_item, image_array, false);
				iconFile.close();
			}
		}
	}
	focusTextEdit();
}

void ChatWindow::on_sendFileButton_clicked()
{
	QStringList file_names = QFileDialog::getOpenFileNames(0,QObject::tr("Open File"),"",QObject::tr("All files (*)"));
	acceptFiles(file_names);
}

void ChatWindow::on_infoButton_clicked()
{
    m_global_instance.showContactInformation(m_item);
    focusTextEdit();
}

void ChatWindow::on_typingButton_clicked()
{
    m_send_typing_notifications = m_send_typing_button->isChecked();
    focusTextEdit();
}

void ChatWindow::acceptFiles(const QStringList &files)
{
	focusTextEdit();
	if(!files.isEmpty())
		m_global_instance.sendFileTo(m_item, files);
}

void ChatWindow::contextMenuEvent(QContextMenuEvent * event)
{
    m_global_instance.showContextMenu(m_item, event->globalPos());
}
