/*
  GeneralWindow

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

#include "generalwindow.h"
#include "src/abstractlayer.h"
#include <QDateTime>
#include <QKeyEvent>
#include <QScrollBar>
#include "chatemoticonmenu.h"

bool EventEater::eventFilter(QObject *obj, QEvent *event)
{
	if ( m_accept_files && event->type() == QEvent::DragEnter )
	{
		QDragEnterEvent *drag_event = static_cast<QDragEnterEvent *>(event);
		if(drag_event->mimeData()->hasUrls())
		{
			drag_event->acceptProposedAction();
			return true;
		}
	}
	else if ( m_accept_files && event->type() == QEvent::Drop )
	{
		QDropEvent *drop_event = static_cast<QDropEvent *>(event);
		QStringList files;
		QList<QUrl> urls = drop_event->mimeData()->urls();
		foreach(const QUrl &url, urls)
		{
			QString file = url.toLocalFile();
			if(file.isEmpty())
				continue;
			files << file;
		}
		if(!files.isEmpty())
		{
			emit acceptUrls(files);
			return true;
		}
	}
	else if ( event->type() == QEvent::FocusIn )
	{
		if (obj->metaObject()->className() != QWebView::staticMetaObject.className())
		    emit focusedIn();
	}
	else if (event->type() == QEvent::KeyPress)
	{

	    QKeyEvent *key_event = static_cast<QKeyEvent *>(event);
	    //Closing current chat event
	    if ( (key_event->key() == Qt::Key_Escape ||
		 (key_event->key() == Qt::Key_W && key_event->modifiers() == Qt::ControlModifier))
		 && (obj->objectName() == "ChatForm" || obj->objectName() == "ConfForm"))
	    {
		emit closeCurrentChat();
		//Change tabs
	    } else if ((((key_event->key() == Qt::Key_Left || key_event->key() == Qt::Key_Right )
		&& key_event->modifiers() == Qt::AltModifier)
		||
			((key_event->key() == Qt::Key_Up || key_event->key() == Qt::Key_Down ) &&
		       key_event->modifiers() == Qt::ControlModifier)) &&
		       obj->metaObject()->className() != QTextEdit::staticMetaObject.className())
	    {


		emit changeTab(key_event->key() == Qt::Key_Left || key_event->key() == Qt::Key_Down?-1:1);
		}
		else if( key_event->modifiers() == Qt::ControlModifier && key_event->key() == Qt::Key_Tab )
		{
			emit changeTab(1);
			return true;
		}else if (obj->objectName() == "chatInputEdit" ) //send message
	    {

		if ( (key_event->key() == Qt::Key_Return || key_event->key() == Qt::Key_Enter)
				       && m_send_on_enter && key_event->modifiers() == Qt::ControlModifier)
		{
		    qobject_cast<QTextEdit*>(obj)->insertPlainText("\n");
		}
		if ( ( (key_event->key() == Qt::Key_Return && key_event->modifiers() == Qt::NoModifier)
			|| ( key_event->key() == Qt::Key_Enter && key_event->modifiers() == Qt::KeypadModifier) )
			&& m_send_on_enter )
		{
		    if( ! m_send_on_double_enter )
		    {
			emit sendMessage();
			return true;
		    } else {
			m_number_of_enters++;
			QTextCursor cursor = qobject_cast<QTextEdit*>(obj)->textCursor();
			if( m_number_of_enters > 1 )
			{
				cursor.setPosition(m_enter_position);
				cursor.deleteChar();
			    m_number_of_enters = 0;
			    emit sendMessage();
			    return true;
			}
			else
				m_enter_position = cursor.position();
		    }
		} else if (key_event->key() == Qt::Key_Return && !m_send_on_enter
				&& key_event->modifiers() == Qt::ControlModifier)
		{
		    emit sendMessage();
		    return true;
		} else m_number_of_enters = 0;
	    }else if (obj->metaObject()->className() == QWebView::staticMetaObject.className() )
	    {
		    if ( key_event->matches(QKeySequence::Copy) )
		    {
			    QWebView *temp_web = qobject_cast<QWebView*>(obj);
			    temp_web->triggerPageAction(QWebPage::Copy);
			    return true;
		    }
	    }


	}
	return QObject::eventFilter(obj, event);
}


GeneralWindow::GeneralWindow() : m_global_instance(TempGlobalInstance::instance()),
m_waiting_for_activation(false),
m_event_eater(new EventEater(this))
{
    connect(m_event_eater, SIGNAL(focusedIn()), this, SLOT(checkWindowFocus()));
    connect(m_event_eater, SIGNAL(sendMessage()), this, SLOT(on_sendButton_clicked()));

}

void GeneralWindow::setOwnerItem(const TreeModelItem &item)
{
	m_scroll_at_maximum = true;
	if(m_plain_text_edit) LogsCity::instance().setEditState(m_item,
		m_plain_text_edit->toHtml(),m_plain_text_edit->textCursor().position());

	m_item = item;

	if ( m_webkit_mode)
	{
		QWebPage *tmp_page = LogsCity::instance().giveMeMyHomeWebPage(item);
		if ( tmp_page)
		{
			m_web_view->setPage(tmp_page);
//			m_web_view->repaint();
		}
	} else
	{
		QTextDocument *tmp_doc = LogsCity::instance().giveMeMyHomeDocument(item);
		if ( tmp_doc)
			m_text_browser->setDocument(tmp_doc);
	}

	if ( m_event_eater ) m_event_eater->m_item = item;
	if(m_plain_text_edit)
	{
		TextEditState tmp_state = LogsCity::instance().getEditState(m_item);
		m_plain_text_edit->setHtml(tmp_state.m_text);
		QTextCursor tmp_cursor = m_plain_text_edit->textCursor();
		tmp_cursor.setPosition(tmp_state.m_cursor_position);
		m_plain_text_edit->setTextCursor(tmp_cursor);
	}
	focusTextEdit();
	browserTextChanged();
}


void GeneralWindow::on_sendButton_clicked()
{
    checkForScrollBarMaximum();
    if ( m_plain_text_edit )
    {
    QString send_message = m_plain_text_edit->toPlainText();
    if (!send_message.isEmpty())
    {
    m_global_instance.sendingMessageBeforeShowing(m_item,send_message);
	LogsCity::instance().addMessage(m_item, send_message,
							     QDateTime::currentDateTime(), false, false);
	 LayersCity::Notification()->userMessage( m_item, "", NotifyMessageSend );
	m_plain_text_edit->clear();
	focusTextEdit();

	if ( m_close_after_send && m_item.m_item_type !=32 )
	emit closeMe();
    }
    }




}

void GeneralWindow::loadSettings()
{
    QSettings settings(QSettings::defaultFormat(), QSettings::UserScope, "qutim/qutim."+m_profile_name, "profilesettings");
    settings.beginGroup("gui");
    m_chat_form_path = settings.value("chat","").toString();
    settings.endGroup();
}

void GeneralWindow::setIcons()
{
    if (m_send_button ) m_send_button->setIcon(m_global_instance.getIcon("message"));
    if (m_emoticons_button) m_emoticons_button->setIcon(m_global_instance.getIcon("emoticon"));
    if (m_translit_button ) m_translit_button->setIcon(m_global_instance.getIcon("translate"));
    if (m_on_enter_button) m_on_enter_button->setIcon(m_global_instance.getIcon("key_enter"));
    if (m_quote_button ) m_quote_button->setIcon(m_global_instance.getIcon("quote"));
    if (m_clear_button ) m_clear_button->setIcon(m_global_instance.getIcon("clear"));

    if (m_text_browser) connect(m_text_browser,SIGNAL(textChanged()), this, SLOT(browserTextChanged()));

    if ( m_emoticons_button )
    {
		m_emotic_menu = new QMenu(m_emoticons_button);
		m_emoticon_action = new QWidgetAction(m_emotic_menu);
		m_emoticon_widget = new ChatEmoticonMenu(m_emotic_menu);
		connect(m_emotic_menu, SIGNAL(aboutToShow()), m_emoticon_widget, SLOT(ensureGeometry()));
	    m_emoticon_action->setDefaultWidget(m_emoticon_widget);
	    m_emotic_menu->addAction(m_emoticon_action);
	    m_emoticons_button->setMenu(m_emotic_menu);
		m_emoticon_widget->setEmoticons(m_global_instance.getEmoticons());
	    connect(m_emoticon_widget, SIGNAL(insertSmile(const QString &)), this,
			    SLOT(insertEmoticon(const QString &)));
    }

	if(m_web_view)
		m_web_view->setAttribute(Qt::WA_OpaquePaintEvent, false);

	if( m_plain_text_edit )
	{
		AbstractLayer::Speller()->startSpellCheck( m_plain_text_edit );
		m_plain_text_edit->setContextMenuPolicy( Qt::CustomContextMenu );
		connect( m_plain_text_edit, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(createContextMenu(QPoint)) );
		m_plain_text_edit->setAcceptRichText( false );
	}
}

void GeneralWindow::setNULLs()
{
	m_translit_button = 0;
	m_quote_button = 0;
	m_clear_button = 0;
	m_emoticons_button = 0;
	m_on_enter_button = 0;
	m_send_button = 0;
	m_text_browser = 0;
	m_web_view = 0;
}

void GeneralWindow::windowActivatedByUser(QWidget *window)
{

    if ( window->isMinimized() )
    {

	window->setWindowState(window->windowState() & (~Qt::WindowMinimized | Qt::WindowActive));
    }

    window->show();
    window->activateWindow();
    window->raise();
    window->setFocus(Qt::ActiveWindowFocusReason);
    focusTextEdit();

}

void GeneralWindow::checkWindowFocus()
{
    if ( m_waiting_for_activation )
    {
	m_waiting_for_activation = false;

    }
    emit windowFocused();
    focusTextEdit();

}

void GeneralWindow::installEventEater()
{
	if (m_web_view)
	{
	    m_web_view->setFocusPolicy(Qt::ClickFocus);
	    m_web_view->installEventFilter(m_event_eater);
	    connect(m_web_view, SIGNAL(linkClicked(const QUrl &)),
		    this, SLOT(newsOnLinkClicked(const QUrl &)));
	}

	if ( m_plain_text_edit )
		m_plain_text_edit->installEventFilter(m_event_eater);
	if ( m_text_browser )
	    m_text_browser->setOpenExternalLinks(true);
	installEventFilter(m_event_eater);
}

void GeneralWindow::focusTextEdit()
{
    if ( m_plain_text_edit )
    {
    	m_plain_text_edit->setFocus();
    }
}

bool GeneralWindow::event(QEvent *event)
{

	if ( event->type() == QEvent::Show )
	{
	    if(this->parent()) qobject_cast<QWidget*>(this->parent())->activateWindow();
	    else
	    {
		activateWindow();
	    }
	    checkWindowFocus();
	    focusTextEdit();

	} else if (event->type() == QEvent::WindowActivate)
	{
	    checkWindowFocus();
	}
	return QWidget::event(event);
}

void GeneralWindow::setFocusPolicy()
{
    if (m_web_view)
	m_web_view->setFocusPolicy(Qt::ClickFocus);
    else if (m_text_browser)
	m_text_browser->setFocusPolicy(Qt::ClickFocus);
}

void GeneralWindow::on_translitButton_clicked()
{
    if (m_plain_text_edit )
    {
	    QString txt = m_plain_text_edit->toPlainText();
	    if ( m_plain_text_edit->textCursor().hasSelection() )
	    {
		    QString sel_text = m_plain_text_edit->textCursor().selectedText();
		    sel_text = invertMessage(sel_text);
		    txt.replace(m_plain_text_edit->textCursor().selectionStart(), sel_text.length(), sel_text);
	    }
	    else
	    {
		    txt = invertMessage(txt);
	    }
	    m_plain_text_edit->clear();
	    m_plain_text_edit->insertPlainText(txt);
    }
    focusTextEdit();
}

QString GeneralWindow::invertMessage(QString &text)
{
	QString tableR=tr("qwertyuiop[]asdfghjkl;'zxcvbnm,./QWERTYUIOP{}ASDFGHJKL:\"ZXCVBNM<>?");
	QString tableE="qwertyuiop[]asdfghjkl;'zxcvbnm,./QWERTYUIOP{}ASDFGHJKL:\"ZXCVBNM<>?";
	QString txt = text;
	for(int i = 0; i < txt.length(); i++)
	{
		if(txt.at(i) <= QChar('z'))
	    {
			int j = 0; bool b=true;
		while((j < tableE.length()) && b)
		{
			if(txt[i] == tableE[j])
		    {
		       b = false;
		       txt[i] = tableR[j];
		    }
		    j++;
		 }
	      }else{
		 int j = 0; bool b = true;
		 while((j < tableR.length()) && b)
		 {
		    if(txt[i] == tableR[j])
		    {
		       b = false;
		       txt[i] = tableE[j];
		    }
		    j++;
		 }
	      }
	   }
	   return txt;
}
void GeneralWindow::on_onEnterButton_clicked()
{
    m_event_eater->m_send_on_enter = m_on_enter_button->isChecked();
    focusTextEdit();
}

void GeneralWindow::on_quoteButton_clicked()
{
	QString selected_text;

	if (m_webkit_mode && m_web_view)
	{
		selected_text = m_web_view->selectedText();
	}
	else if ( m_text_browser )
	{
		selected_text = m_text_browser->textCursor().selectedText();
	}
	if ( m_plain_text_edit && !selected_text.isEmpty() )
	{
		if ( m_webkit_mode )
		{
			selected_text.prepend(">  ");
			selected_text.replace("\n", "\n>  ");
		}
		else
		{
			// Replace paragraph separators
			selected_text.replace(QChar::ParagraphSeparator, "\n>  ");
			// Qt internally uses U+FDD0 and U+FDD1 to mark the beginning and the end of frames.
			// They should be seen as non-printable characters, as trying to display them leads
			// to a crash caused by a Qt "noBlockInString" assertion.
			selected_text.replace(QChar(0xFDD0), " ");
			selected_text.replace(QChar(0xFDD1), " ");
			// Prepend text with ">" tag
			selected_text.prepend(">  ");
			selected_text.replace(QChar(0x2028), "\n>  ");
		}
		if(!selected_text.endsWith("\n"))
			selected_text += "\n";
		m_plain_text_edit->insertPlainText(selected_text);
		m_plain_text_edit->moveCursor(QTextCursor::End);
		m_plain_text_edit->ensureCursorVisible();
		m_plain_text_edit->setFocus();
	}
	focusTextEdit();
}

void GeneralWindow::on_clearChatButton_clicked()
{
	LogsCity::instance().clearMyHomeLog(m_item);
	m_scroll_at_maximum = true;
	focusTextEdit();
}

void GeneralWindow::browserTextChanged()
{
    if ( m_text_browser && m_scroll_at_maximum)
    {
	m_text_browser->ensureCursorVisible();
	m_text_browser->setLineWrapColumnOrWidth(m_text_browser->lineWrapColumnOrWidth());
	int scroll_maximum = m_text_browser->verticalScrollBar()->maximum();
	m_text_browser->verticalScrollBar()->setValue( scroll_maximum );
    }
}

void GeneralWindow::checkForScrollBarMaximum()
{

    if ( m_text_browser )
	m_scroll_at_maximum = (m_text_browser->verticalScrollBar()->maximum()
			       == m_text_browser->verticalScrollBar()->value());
}

bool GeneralWindow::compareItem(const TreeModelItem &item)
{
	return item.m_protocol_name == m_item.m_protocol_name
			&& item.m_account_name == m_item.m_account_name
			&& item.m_item_name == m_item.m_item_name;
}

void GeneralWindow::insertEmoticon(const QString &emoticon_text)
{
	if ( m_plain_text_edit )
	{
		m_plain_text_edit->insertHtml(" " + emoticon_text + " ");
		focusTextEdit();
	}
}

void GeneralWindow::newsOnLinkClicked(const QUrl &url)
{
	QDesktopServices::openUrl(url);
}

static QTextCursor speller_cursor;

void GeneralWindow::createContextMenu( const QPoint &pos )
{
	if( !m_plain_text_edit || !speller_cursor.isNull() )
		return;
	QMenu *menu = m_plain_text_edit->createStandardContextMenu( pos );
	QMenu *suggest_menu = 0;
	QList<QAction *> actions;
	QTextCursor cur = m_plain_text_edit->cursorForPosition( pos );
	bool d = cur.movePosition( QTextCursor::EndOfWord );
	int e = cur.position();
	bool b = cur.movePosition( QTextCursor::StartOfWord );
	int c = cur.position();
	QString text = m_plain_text_edit->toPlainText().mid( c, e - c );
	if( b && d && LayersCity::Speller()->isMisspelled( text ) )
	{
		QStringList suggestions = LayersCity::Speller()->suggest( text );
		suggest_menu = menu->addMenu( SystemsCity::IconManager()->getIcon( "speller" ), tr("Possible variants") );
		for( int i = 0; i < suggestions.size(); i++ )
		{
			QAction *action = suggest_menu->addAction( suggestions[i] );
			action->setData( suggestions[i] );
			actions << action;
		}
		if( actions.isEmpty() )
			suggest_menu->setDisabled( true );
	}
	QAction *action = menu->exec( m_plain_text_edit->mapToGlobal( pos ) );
	if( action && actions.contains( action ) )
	{
		for( int i = d; i <= e; i++ )
			cur.deleteChar();
		cur.insertText( action->data().toString() );
	}
	qDeleteAll( actions );
	delete suggest_menu;
	delete menu;
}
