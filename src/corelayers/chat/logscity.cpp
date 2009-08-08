/*
   LogWidgetHome LogsCity

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


#include "logscity.h"
#include "chatlayerclass.h"
#include <QTextCursor>
#include <QCryptographicHash>
#include <QFile>
#include <QWebView>
//#if (QT_VERSION >= QT_VERSION_CHECK(4, 5, 0))
//#include "include/qutim/settings.h"
//#include <QNetworkDiskCache>
//#include <QNetworkAccessManager>
//static QNetworkAccessManager *webkit_access_manager = 0;
//static QNetworkDiskCache *webkit_network_cache = 0;
//#endif

 const  char *nickname_colors[] = {
"black", "silver", "gray", "maroon", "red", "purple", "fuchsia", "green", "lime", "olive", "yellow", "navy", "blue",
"teal", "aqua"
};
	
LogJSHelper::LogJSHelper(LogWidgetHome *home) : m_home(home)
{
	static const QString client = "client";
	setObjectName( client );
}

void LogJSHelper::debugLog( const QVariant &text )
{
	qDebug( "WebKit: \"%s\"", qPrintable(text.toString()) );
}

bool LogJSHelper::zoomImage( const QVariant &text )
{
	return false;
}

void LogJSHelper::helperCleared()
{
	if( QWebFrame *frame = qobject_cast<QWebFrame *>(sender()) )
	{
		frame->addToJavaScriptWindowObject( objectName(), this );
	}
}

void LogJSHelper::appendNick( const QVariant &nick )
{
	if(QTextEdit *edit = static_cast<ChatLayerClass *>(LayersCity::Chat())->getEditField(m_home->m_item))
	{
		QTextCursor cursor = edit->textCursor();
		if(cursor.atStart())
			cursor.insertText(nick.toString() + ": ");
		else
			cursor.insertText(nick.toString() + " ");
		edit->setFocus();
	}
}

void LogJSHelper::contextMenu( const QVariant &nick )
{
	TreeModelItem item = m_home->m_item;
	QPoint point = QCursor::pos();
	TempGlobalInstance::instance().conferenceItemContextMenu(
			m_home->m_item.m_protocol_name,
			m_home->m_item.m_item_name,
			m_home->m_item.m_account_name,
			nick.toString(), point);
}

LogWidgetHome::LogWidgetHome(bool am_i_webkit, bool am_i_conference, const QString &webkit_style, const QString &webkit_variant)
{
	m_i_am_webkit_unit = am_i_webkit;
	m_i_am_conference = am_i_conference;
	m_text_document = 0;
	m_web_page = 0;
	m_web_helper = 0;
	m_contact_list = 0;
	m_style_output = 0;
	m_edit_state.m_cursor_position = 0;
	m_webkit_style_path = webkit_style;
	m_webkit_variant = webkit_variant;
	if ( !m_i_am_webkit_unit )
		m_text_document = new QTextDocument;
	else
	{
		m_web_page = new QWebPage;
//#if (QT_VERSION >= QT_VERSION_CHECK(4, 5, 0))
//		if( webkit_access_manager )
//			m_web_page->setNetworkAccessManager( webkit_access_manager );
//#endif
		m_web_helper = new LogJSHelper(this);
		m_web_page->mainFrame()->addToJavaScriptWindowObject( m_web_helper->objectName(), m_web_helper );
		QObject::connect( m_web_page->mainFrame(), SIGNAL(javaScriptWindowObjectCleared()), m_web_helper, SLOT(helperCleared()) );
		m_web_page->setLinkDelegationPolicy(QWebPage::DelegateAllLinks);
	}

	m_last_message_icon_position = 0;
	m_horizontal_separator_position = -1;
	m_separator_added = false;
	m_light_kill = false;
	m_history_loaded = false;
}

LogWidgetHome::~LogWidgetHome()
{
    delete m_text_document;
	if ( m_i_am_webkit_unit )
	{
		if( QWebView *view = qobject_cast<QWebView *>(m_web_page->view()) )
		{
			if( view->page() == m_web_page )
				view->setPage( 0 );
		}
		m_web_page->deleteLater();
		delete m_style_output;
	}
    delete m_contact_list;

}

static inline QString &validateCpp( QString &text )
{
	text.replace( "\"", "\\\"" ).replace( "\n", "\\n" ).replace( "\t", "\\t" );
	return text;
}

void LogWidgetHome::addMessage(const QString &message,const QDateTime &date, bool history, bool in, const QString &from,
			       bool win_active)
{
	if ( !m_i_am_webkit_unit )
	{
		if(message.startsWith("/me "))
		{
			QString tmp_msg = message;
			tmp_msg.replace("/me",from.isEmpty()?in?m_contact_nickname:m_onwer_nickname:from);
			addServiceMessage(tmp_msg);
			return;
		}

	QTextCursor tmp_cursor = QTextCursor(m_text_document);
	tmp_cursor.movePosition(QTextCursor::End);


	if ( !win_active && !history && !m_separator_added)
	{
		if (m_horizontal_separator_position != -1)
		{
		tmp_cursor.setPosition(m_horizontal_separator_position);
		tmp_cursor.deleteChar();
		tmp_cursor.deleteChar();
		tmp_cursor.deleteChar();
		tmp_cursor.movePosition(QTextCursor::End);
		}
		m_horizontal_separator_position = tmp_cursor.position();
		tmp_cursor.insertHtml("<hr/><br/><br/>");
		tmp_cursor.movePosition(QTextCursor::End);
		m_separator_added = true;

	} else if (win_active )
	{
		m_separator_added = false;
	}

	quint64 tmp_position = tmp_cursor.position();
	QString temp_text;
	if ( !m_i_am_conference )
	{
		tmp_cursor.insertImage(TempGlobalInstance::instance().getIconPath("message"));
		tmp_cursor.insertHtml("&nbsp;");
		temp_text.append(in?QString("<b><font color='red'>"):QString("<b><font color='blue'>"));
		if ( m_show_names )  temp_text.append(Qt::escape(in?m_contact_nickname:m_onwer_nickname));
		temp_text.append(QString(" ( %1 )</font></b><br>").arg(getTimeStamp(date)));

	} else
	{

		QString tmp_color = m_colorize_nicknames?m_color_names.value(from):QString("red");
		temp_text.append(
			from != m_onwer_nickname?QString("<b><font color='"
		+tmp_color+"'>"):QString("<b><font color='blue'>"));
		if ( m_show_names ) temp_text.append(Qt::escape(from));
		temp_text.append(QString(" ( %1 ): </font></b>").arg(getTimeStamp(date)));
	}

	temp_text.append(QString("%1<br>").arg(message));
	tmp_cursor.insertHtml(temp_text);
	tmp_cursor.movePosition(QTextCursor::End);
	if ( m_remove_after )
	{
		m_message_positions.append(tmp_cursor.position() - tmp_position);
		if ( m_message_positions.count() >=  (m_remove_count + 1))
		{
			int message_length = m_message_positions.at(0);
			QTextCursor cursor = QTextCursor(m_text_document);
			cursor.clearSelection();
			cursor.setPosition(0, QTextCursor::MoveAnchor);
			cursor.setPosition(message_length, QTextCursor::KeepAnchor);
			cursor.removeSelectedText();
			m_message_positions.remove(0);
			foreach( int icon_number, m_message_position_offset.keys() )
			{
				int old_position = m_message_position_offset.value(icon_number);
				m_message_position_offset.remove(icon_number);
				m_message_position_offset.insert(icon_number, old_position - message_length);
				m_horizontal_separator_position -= message_length;
			}
		}

	}

	}
	else
	{
		bool same_from = false;
		QString new_message;
		if ( m_i_am_conference )
		{
			if ( !win_active && !history && !m_separator_added)
			{

				QString js_message = "separator = document.getElementById(\"separator\");"
				"if(separator)"
				"	separator.parentNode.removeChild(separator);";
				m_web_page->mainFrame()->evaluateJavaScript(js_message);


				js_message = "appendMessage(\"<hr id=\\\"separator\\\"><div id=\\\"insert\\\"></div>\");";
						m_web_page->mainFrame()->evaluateJavaScript(js_message);
						m_previous_sender = "";
				m_separator_added = true;
			} else if (win_active )
			{
				m_separator_added = false;
			}


			in = (from == m_onwer_nickname);
			QString escaped_from = Qt::escape(from);
			QString from_color = "<span oncontextmenu='client.contextMenu(\"";
			from_color += escaped_from;
			from_color += "\");return false;' onclick='client.appendNick(\"";
			from_color += escaped_from;
			from_color += "\");'";
			if ( m_colorize_nicknames && from != m_onwer_nickname)
			{
				from_color += " style='color:";
				from_color += m_color_names.value(from);
				from_color += " !important;cursor:pointer;'";
			}
			from_color += ">";
			from_color += escaped_from;
			from_color += "</span>";
			if(message.startsWith("/me "))
			{
				QString tmp_msg = message.mid(3);
				new_message	 = m_style_output->makeAction(
						from_color,
						tmp_msg, in, date, getTimeStamp(date),
						"",
						true, from, m_protocol_name);
				m_previous_sender = "";
			}
			else
			{
				same_from = (m_previous_sender == from);
				new_message	 = m_style_output->makeMessage(
						from_color,
						message, in, date, getTimeStamp(date),
						"",
						true, from, m_protocol_name,
						same_from, history);
				m_previous_sender = from;
			}
		} else {
			if ( !history  && !m_history_loaded )
			{
				m_history_loaded = true;
				m_previous_sender="";
			}
			if(message.startsWith("/me "))
			{
				QString tmp_msg = message.mid(3);
				new_message = m_style_output->makeAction(
						Qt::escape(in?m_contact_nickname:m_onwer_nickname),
						tmp_msg, !in, date, getTimeStamp(date),
						in?m_contact_avatar:m_own_avatar,
						true, in?m_contact_id:m_owner_id, m_protocol_name);
				m_previous_sender = "";
			}
			else
			{
				same_from = (m_previous_sender == (in?"nme":"me"));
				new_message	 = m_style_output->makeMessage(
						Qt::escape(in?m_contact_nickname:m_onwer_nickname),
						message, !in, date, getTimeStamp(date),
						in?m_contact_avatar:m_own_avatar,
						true, in?m_contact_id:m_owner_id, m_protocol_name,
						same_from, history);
				m_previous_sender = (in?"nme":"me");
			}
		}

		QString js_result = m_web_page->mainFrame()->evaluateJavaScript(QString("getEditedHtml(\"%1\", \"%2\");")
																		.arg(validateCpp(new_message))
																		.arg(m_last_message_icon_position)).toString();

		QString js_message = QString("append%2Message(\"%1\");").arg(
				js_result.isEmpty() ? new_message :
				validateCpp(js_result.replace("\\","\\\\")), same_from?"Next":"");

		m_web_page->mainFrame()->evaluateJavaScript(js_message);
	}
}

QString LogWidgetHome::getTimeStamp(const QDateTime &datetime)
{
    	switch ( m_timestamp)
	{
	case 0:
		return datetime.toString("hh:mm:ss dd/MM/yyyy");
	case 1:
		return datetime.toString("hh:mm:ss");
	case 2:
		return datetime.toString("hh:mm dd/MMM/yyyy");
	default:
		return datetime.toString("hh:mm:ss");
	}
}

quint64 LogWidgetHome::getSendIconPos()
{
    if ( m_i_am_conference ) return 0;
    if ( m_i_am_webkit_unit )
    {
	return ++m_last_message_icon_position;
    }
    else
    {
	QTextCursor tmp_cursor = QTextCursor(m_text_document);
	tmp_cursor.movePosition(QTextCursor::End);
	if ( m_remove_after)
	{
		m_last_message_icon_position++;
		m_message_position_offset.insert(m_last_message_icon_position, tmp_cursor.position());
		return m_last_message_icon_position;
	}
	else
	{
		return tmp_cursor.position();
	}
	
    }
}

void LogWidgetHome::messageDelievered(int position)
{
	if (m_i_am_webkit_unit)
	{
	    m_web_page->mainFrame()->evaluateJavaScript(QString("messageDlvrd(\"%1\");").arg(position));
	}
	else
	{
	    QTextCursor tmp_cursor = QTextCursor(m_text_document);
	    if ( m_remove_after )
		tmp_cursor.setPosition(m_message_position_offset.value(position), QTextCursor::MoveAnchor);
	    else
		tmp_cursor.setPosition(position, QTextCursor::MoveAnchor);

	    tmp_cursor.deleteChar();
	    tmp_cursor.insertImage(TempGlobalInstance::instance().getIconPath("message_accept"));
	    tmp_cursor.movePosition(QTextCursor::End);
	}
}

void LogWidgetHome::addConferenceItem(const QString &name)
{

	if ( m_i_am_conference && m_contact_list)
	{
	    m_contact_list->addConferenceItem(name);
	    qsrand(QTime::currentTime().msec());
	    m_color_names.insert(name, nickname_colors[qrand()%15]);
	}
}

void LogWidgetHome::renameConferenceItem(const QString &old_name, const QString &name)
{

	if ( m_i_am_conference && m_contact_list)
	{
	    m_contact_list->renameConferenceItem(old_name, name);
	    m_color_names.insert(name, m_color_names.value(old_name));
	    m_color_names.remove(old_name);
	}
}


void LogWidgetHome::removeConferenceItem(const QString &name)
{

	if ( m_i_am_conference && m_contact_list)
	{
	    m_contact_list->removeConferenceItem(name);
	    m_color_names.remove(name);
	}
}

void LogWidgetHome::clearMyLog()
{
	if (m_i_am_webkit_unit)
	{
	    m_web_page->mainFrame()->setHtml(m_now_html);
	    m_previous_sender = "";
	}
	else
	{
		m_text_document->clear();
	}
}

void LogWidgetHome::addServiceMessage(const QString &message)
{
	if (m_i_am_webkit_unit)
	{
	    QString new_message = message;
		new_message = m_style_output->makeStatus(new_message, QDateTime::currentDateTime(),
												 QDateTime::currentDateTime().toString());
	    QString js_message = QString("appendMessage(\"%1\");").arg(
		new_message.replace("\"","\\\"").replace("\n","\\n"));
	    m_web_page->mainFrame()->evaluateJavaScript(js_message);
	    m_previous_sender = "";
	}
	else
	{
		QTextCursor tmp_cursor = QTextCursor(m_text_document);
		tmp_cursor.movePosition(QTextCursor::End);
		tmp_cursor.insertHtml(QString("<font size='-1' color='grey'>%1</font><br/><br/>")
					   .arg(QString(message).replace("\n","<br/>")));
		tmp_cursor.movePosition(QTextCursor::End);
	}
}

void LogWidgetHome::loadWebkitStyle( const QString &head )
{
   if ( m_i_am_webkit_unit && m_web_page )
    {
	m_style_output = new ChatWindowStyleOutput(m_webkit_style_path,
			m_webkit_variant);
	m_style_output->preparePage(m_web_page);
	m_now_html = m_style_output->makeSkeleton(m_contact_nickname,
			m_onwer_nickname,
			m_contact_nickname,
			m_own_avatar,
			m_contact_avatar,
			QDateTime::currentDateTime(),
			QDateTime::currentDateTime().toString());
	static const QRegExp regexp( "(\\<\\s*\\/\\s*head\\s*\\>)", Qt::CaseInsensitive );
	Q_ASSERT_X( regexp.isValid(), "LogWidgetHome::loadWebkitStyle(QString)", "RegExp is not valid" );
	m_now_html.replace( regexp, head );
	m_web_page->mainFrame()->setHtml(m_now_html);
    }
}


LogsCity::LogsCity()
{
}

LogsCity::~LogsCity()
{
	release();
}

void LogsCity::release()
{
//#if (QT_VERSION >= QT_VERSION_CHECK(4, 5, 0))
//	if( webkit_access_manager )
//		delete webkit_access_manager;
//	webkit_access_manager = 0;
//	webkit_network_cache = 0;
//#endif
	qDeleteAll(m_city_map);
	m_city_map.clear();
}


QTextDocument *LogsCity::giveMeMyHomeDocument(const QString &identification)
{
    if ( m_city_map.contains(identification) ) return m_city_map.value(identification)->m_text_document;
    else return 0;
}

QTextDocument *LogsCity::giveMeMyHomeDocument(const TreeModelItem &item)
{
	QString identification = QString("%1.%2.%3").arg(item.m_protocol_name)
	.arg(item.m_account_name).arg(item.m_item_name);
	return giveMeMyHomeDocument(identification);
}


QWebPage *LogsCity::giveMeMyHomeWebPage(const QString &identification)
{
    if ( m_city_map.contains(identification) ) return m_city_map.value(identification)->m_web_page;
    else return 0;
}

QWebPage *LogsCity::giveMeMyHomeWebPage(const TreeModelItem &item)
{
	QString identification = QString("%1.%2.%3").arg(item.m_protocol_name)
	.arg(item.m_account_name).arg(item.m_item_name);
	return giveMeMyHomeWebPage(identification);
}

void LogsCity::createHomeForMe(const TreeModelItem &item, bool webkit_mode, QString contact_nick, QString owner_nick,
			       const QString &own_avatar, const QString &contact_avatar)
{
    QString identification = QString("%1.%2.%3").arg(item.m_protocol_name)
	.arg(item.m_account_name).arg(item.m_item_name);

    LogWidgetHome *new_home = new LogWidgetHome(webkit_mode, item.m_item_type==32?true:false, m_webkit_style_path,
						m_webkit_variant);
	new_home->m_item = item;
    new_home->m_contact_nickname = contact_nick;
    new_home->m_onwer_nickname = owner_nick;
    new_home->m_own_avatar = own_avatar;
    new_home->m_contact_avatar = contact_avatar;
    new_home->m_protocol_name = item.m_protocol_name;
    new_home->m_owner_id = item.m_account_name;
    new_home->m_contact_id = item.m_item_name;
    new_home->m_remove_after = m_remove_after;
    new_home->m_remove_count = m_remove_count;
    new_home->m_show_names = m_show_names;
    new_home->m_timestamp = m_timestamp;
    new_home->m_colorize_nicknames = m_colorize_nicknames;
	new_home->loadWebkitStyle( m_html_for_head[item.m_item_type] + "\\1" );
    m_city_map.insert(identification, new_home);

}

void LogsCity::moveMyHome( const QString &old_id, const QString &new_id )
{
	m_city_map.insert( new_id, m_city_map.take( old_id ) );
}

bool LogsCity::doIHaveHome(const TreeModelItem &item)
{
    QString identification = QString("%1.%2.%3").arg(item.m_protocol_name)
	.arg(item.m_account_name).arg(item.m_item_name);
    return doIHaveHome(identification);
}

bool LogsCity::doIHaveHome(const QString &identification)
{
    return m_city_map.contains(identification);
}

void LogsCity::destroyMyHome(const TreeModelItem &item, bool light_bomb)
{
	QString identification = QString("%1.%2.%3").arg(item.m_protocol_name)
	.arg(item.m_account_name).arg(item.m_item_name);
	if ( m_city_map.contains(identification) )
	{
		m_city_map.value(identification)->m_light_kill = light_bomb;
		delete m_city_map.take(identification);
	}
}

void LogsCity::addConferenceItem(const TreeModelItem &item, const QString &name)
{
    QString identification = QString("%1.%2.%3").arg(item.m_protocol_name)
	.arg(item.m_account_name).arg(item.m_parent_name);
    if ( m_city_map.contains(identification) )
    {
	m_city_map.value(identification)->addConferenceItem(name);
    }
}

void LogsCity::setConferenceListView(const TreeModelItem &item, QListView *list_view)
{
    QString identification = QString("%1.%2.%3").arg(item.m_protocol_name)
	.arg(item.m_account_name).arg(item.m_item_name);
    if ( m_city_map.contains(identification) )
    {
	LogWidgetHome *tmp_home = m_city_map.value(identification);
	if ( !tmp_home->m_contact_list )
	{
	    tmp_home->m_contact_list = new ConfContactList(item.m_protocol_name,item.m_item_name,item.m_account_name,list_view);
	}
	tmp_home->m_contact_list->nowActive();

    }

}

void LogsCity::setConferenceItemStatus(const TreeModelItem &item, const QIcon &icon, const QString &status, int mass)
{
    QString identification = QString("%1.%2.%3").arg(item.m_protocol_name)
	.arg(item.m_account_name).arg(item.m_parent_name);
    if ( m_city_map.contains(identification) )
    {
	LogWidgetHome *tmp_home = m_city_map.value(identification);

	if ( tmp_home->m_i_am_conference && tmp_home->m_contact_list)
	{
	    tmp_home->m_contact_list->setConferenceItemStatus(item.m_item_name,icon,status, mass);
	}
    }
}

ConfContactList *LogsCity::getConferenceCL(const TreeModelItem &item)
{
    QString identification = QString("%1.%2.%3").arg(item.m_protocol_name)
	.arg(item.m_account_name).arg(item.m_item_name);
    if ( m_city_map.contains(identification) )
    {
		return m_city_map.value(identification)->m_contact_list;
    }
	return 0;
}

void LogsCity::renameConferenceItem(const TreeModelItem &item, const QString &new_name)
{
    QString identification = QString("%1.%2.%3").arg(item.m_protocol_name)
	.arg(item.m_account_name).arg(item.m_parent_name);
    if ( m_city_map.contains(identification) )
    {
	m_city_map.value(identification)->renameConferenceItem(item.m_item_name, new_name);
    }
}

void LogsCity::removeConferenceItem(const TreeModelItem &item)
{
    QString identification = QString("%1.%2.%3").arg(item.m_protocol_name)
	.arg(item.m_account_name).arg(item.m_parent_name);
    if ( m_city_map.contains(identification) )
    {
	m_city_map.value(identification)->removeConferenceItem(item.m_item_name);
    }
}

void LogsCity::setConferenceItemIcon(const TreeModelItem &item, const QIcon &icon, int position)
{
    QString identification = QString("%1.%2.%3").arg(item.m_protocol_name)
	.arg(item.m_account_name).arg(item.m_parent_name);
    if ( m_city_map.contains(identification) )
    {
	LogWidgetHome *tmp_home = m_city_map.value(identification);

	if ( tmp_home->m_i_am_conference && tmp_home->m_contact_list)
	{
	    tmp_home->m_contact_list->setConferenceItemIcon(item.m_item_name,icon,position);
	}
    }
}

void LogsCity::setConferenceItemRole(const TreeModelItem &item, const QIcon &icon, const QString &role, int mass)
{
    QString identification = QString("%1.%2.%3").arg(item.m_protocol_name)
	.arg(item.m_account_name).arg(item.m_parent_name);
    if ( m_city_map.contains(identification) )
    {
	LogWidgetHome *tmp_home = m_city_map.value(identification);

	if ( tmp_home->m_i_am_conference && tmp_home->m_contact_list)
	{
	    tmp_home->m_contact_list->setConferenceItemRole(item.m_item_name,icon,role,mass);
	}
    }
}

bool LogsCity::addMessage(const TreeModelItem &item, const QString &message,
				     const QDateTime &date, bool history, bool in, bool window_active)
{
    quint64 tmp_icon_pos = 0;
	QString identification;
	QString new_temp_message = (history && item.m_item_type != 32) ? message : Qt::escape(message).replace("\n", "<br/>");


    if ( !(item.m_item_type == 32 && !in) )
    {
		if ( item.m_item_type == 34 )
		identification = QString("%1.%2.%3").arg(item.m_protocol_name)
			.arg(item.m_account_name).arg(item.m_parent_name);
		else
		identification = QString("%1.%2.%3").arg(item.m_protocol_name)
			.arg(item.m_account_name).arg(item.m_item_name);

		if ( m_city_map.contains(identification) )
		{

			LogWidgetHome *tmp_home = m_city_map.value(identification);
			new_temp_message = findUrls( new_temp_message,tmp_home->m_i_am_webkit_unit );
			tmp_icon_pos = tmp_home->getSendIconPos();
			if ( in )
			TempGlobalInstance::instance().receivingMessageBeforeShowing(item,new_temp_message);
			else
			TempGlobalInstance::instance().sendingMessageBeforeShowing(item,new_temp_message);
			tmp_home->addMessage(new_temp_message,date,history,in,
						item.m_item_type == 34? item.m_item_name : "", window_active);
		}
	}

	if (item.m_item_type != 34 && item.m_item_type != 32 && !history )
	{
		HistoryItem h_item;
		h_item.m_in = in;
		h_item.m_from = item.m_item_name;
		h_item.m_message = Qt::escape(message).replace("\n", "<br/>");
		h_item.m_time = date;
		h_item.m_type = 1;
		h_item.m_user = item;
		TempGlobalInstance::instance().saveMessageToHistory(h_item);
    }

    if ( !in && !history)
    {
		TempGlobalInstance::instance().sendMessageTo(item,message,tmp_icon_pos);
    }
    if ( item.m_item_type == 34 && m_city_map.contains(identification) && !history)
		return message.contains(m_city_map.value(identification)->m_onwer_nickname);

    return false;

}

void LogsCity::changeId(const TreeModelItem &item, const QString &new_id)
{
    QString identification = QString("%1.%2.%3").arg(item.m_protocol_name)
	.arg(item.m_account_name).arg(item.m_item_name);
    if ( m_city_map.contains(identification) )
    {
	QString new_identification = QString("%1.%2.%3").arg(item.m_protocol_name)
	.arg(item.m_account_name).arg(new_id);
	LogWidgetHome *tmp_home = m_city_map.value(identification);
	m_city_map.remove(identification);
	m_city_map.insert(new_identification, tmp_home);
    }
}

void LogsCity::setEditState(const TreeModelItem &item, const QString &text, quint64 position)
{
    QString identification = QString("%1.%2.%3").arg(item.m_protocol_name)
	.arg(item.m_account_name).arg(item.m_item_name);
    TextEditState tmp_state;
    tmp_state.m_text = text;
    tmp_state.m_cursor_position = position;
    if ( m_city_map.contains(identification) ) m_city_map.value(identification)->m_edit_state = tmp_state;
}

TextEditState LogsCity::getEditState(const TreeModelItem &item)
{
    QString identification = QString("%1.%2.%3").arg(item.m_protocol_name)
	.arg(item.m_account_name).arg(item.m_item_name);

    if ( m_city_map.contains(identification) ) return m_city_map.value(identification)->m_edit_state;
    else return TextEditState();
}

void LogsCity::loadSettings(const QString &profile_name)
{
//#if (QT_VERSION >= QT_VERSION_CHECK(4, 5, 0))
//	if( !webkit_access_manager )
//		webkit_access_manager = new QNetworkAccessManager;
//	if( !webkit_network_cache )
//	{
//		webkit_network_cache = new QNetworkDiskCache( webkit_access_manager );
//		webkit_network_cache->setCacheDirectory( Settings::getProfileDir().absoluteFilePath( "webcache" ) );
//		webkit_access_manager->setCache( webkit_access_manager );
//	}
//#endif
    m_profile_name = profile_name;
    QSettings settings(QSettings::defaultFormat(), QSettings::UserScope, "qutim/qutim."+profile_name, "profilesettings");
    settings.beginGroup("chatwindow");
    m_remove_after = settings.value("remove", false).toBool();
    m_show_names = settings.value("names", true).toBool();
    m_remove_count = settings.value("removecount", 200).toUInt();
    m_timestamp = settings.value("timestamp", 1).toUInt();
    m_colorize_nicknames = settings.value("colorize", false).toBool();
    settings.endGroup();
    loadGuiSettings();
}

void LogsCity::messageDelievered(const TreeModelItem &item, int message_position)
{
    QString identification = QString("%1.%2.%3").arg(item.m_protocol_name)
	.arg(item.m_account_name).arg(item.m_item_name);
    if ( m_city_map.contains(identification) ) m_city_map.value(identification)->messageDelievered(message_position);
}

void LogsCity::changeOwnNickNameInConference(const TreeModelItem &item, const QString &new_nickname)
{
    QString identification = QString("%1.%2.%3").arg(item.m_protocol_name)
	.arg(item.m_account_name).arg(item.m_item_name);
    if ( m_city_map.contains(identification) )
	m_city_map.value(identification)->m_onwer_nickname = new_nickname;
}

void LogsCity::clearMyHomeLog(const TreeModelItem &item)
{
    QString identification = QString("%1.%2.%3").arg(item.m_protocol_name)
	.arg(item.m_account_name).arg(item.m_item_name);
    if ( m_city_map.contains(identification) )
	m_city_map.value(identification)->clearMyLog();
}

void LogsCity::addServiceMessage(const TreeModelItem &item, const QString &message)
{
     QString identification = QString("%1.%2.%3").arg(item.m_protocol_name)
	.arg(item.m_account_name).arg(item.m_item_name);
    if ( m_city_map.contains(identification) )
	m_city_map.value(identification)->addServiceMessage(message);
}

void LogsCity::notifyAboutFocusingConference(const TreeModelItem &item)
{
       QString identification = QString("%1.%2.%3").arg(item.m_protocol_name)
	.arg(item.m_account_name).arg(item.m_item_name);
       if ( m_city_map.contains(identification) )
	m_city_map.value(identification)->m_separator_added = false;
}

void LogsCity::loadGuiSettings()
{
    QSettings settings(QSettings::defaultFormat(), QSettings::UserScope, "qutim/qutim."+m_profile_name, "profilesettings");
    settings.beginGroup("gui");
    m_webkit_style_path = settings.value("wstyle","").toString();
    m_webkit_variant = settings.value("wvariant","").toString();
    settings.endGroup();
	if( m_city_map.isEmpty() )
		return;
	QString js;
	foreach( LogWidgetHome *home, m_city_map )
	{
		if( home->m_web_page
			&& home->m_webkit_style_path == m_webkit_style_path
			&& home->m_webkit_variant != m_webkit_variant )
		{
			home->m_webkit_variant = m_webkit_variant;
			home->m_style_output->setVariant( m_webkit_variant );
			if( js.isEmpty() )
			{
				js += "setStylesheet(\"mainStyle\",\"";
				js += home->m_style_output->getVariantCSS();
				js += "\");";
			}
			home->m_web_page->mainFrame()->evaluateJavaScript( js );
		}
	}
}

void LogsCity::appendHtmlToHead( const QString &html, const QList<quint8> &types )
{
	foreach( quint8 type, types )
		m_html_for_head[type].append( html );
}

QString LogsCity::findUrls(const QString &message,bool webkit)
{
	QString html = message;
	// TODO: Choose more correct one
	// "((https?://|ftp://|www\\.)[^\\s<>&]+)"
	// "((https?://|ftp://|www\\.)([\\w:/\\?#\\[\\]@!\\$&\\(\\)\\*\\+,;=\\._~-]|&amp;|%[0-9a-fA-F]{2})+)"
	// Reserved and unreserved characters are fine
//     unreserved    = ALPHA / DIGIT / "-" / "." / "_" / "~"
//     reserved      = gen-delims / sub-delims
//     gen-delims    = ":" / "/" / "?" / "#" / "[" / "]" / "@"
//     sub-delims    = "!" / "$" / "&" / "'" / "(" / ")"
//                         / "*" / "+" / "," / ";" / "="
//     :/?#[]@!$&()*+,;=._~-
//	   :/\\?#\\[\\]@!\\$&\\(\\)\\*\\+,;=\\._~-
	static QRegExp linkRegExp("([a-zA-Z0-9\\-\\_\\.]+@([a-zA-Z0-9\\-\\_]+\\.)+[a-zA-Z]+)|"
							  "((https?://|ftp://|www\\.)([\\w:/\\?#\\[\\]@!\\$&\\(\\)\\*\\+,;=\\._~-]|&amp;|%[0-9a-fA-F]{2})+)",
							  Qt::CaseInsensitive);
	Q_ASSERT(linkRegExp.isValid());
	int pos = 0;
    while(((pos = linkRegExp.indexIn(html, pos)) != -1))
    {
		QString link = linkRegExp.cap(0);
		QString tmplink = link;
		if (tmplink.toLower().startsWith("www."))
			tmplink.prepend("http://");
		else if(!tmplink.contains("//"))
			tmplink.prepend("mailto:");
		if(!webkit)
		{
			QString url;
			for(int i = 0; i < tmplink.length(); i++)
			{
				if(tmplink.at(i) == '&')
				{
					int length = tmplink.length() - i;
					if(length >= 4 && QStringRef(&tmplink, i, 4) == "&lt;")
					{
						url += "<";
						i += 3;
					}
					else if(length >= 4 && QStringRef(&tmplink, i, 4) == "&gt;")
					{
						url += ">";
						i += 3;
					}
					else if(length >= 5 && QStringRef(&tmplink, i, 5) == "&amp;")
					{
						url += "&";
						i += 4;
					}
					else
						url += tmplink.at(i);
				}
				else
					url += tmplink.at(i);
			}
			tmplink = url;
		}
		static const QString hrefTemplate( "<a href='%1' target='_blank'>%2</a>" );
		tmplink = hrefTemplate.arg(tmplink, link);
		html.replace(pos, link.length(), tmplink);
		pos += tmplink.count();
    }
#ifndef BUILD_QUTIM
	static QString qutim_url = "<a href=\"http://qutim.org/\">\\1</a>";
	// (?<![a-z./])(qutim)(?![a-z.])
	static QRegExp qutimRegExp("(?![a-z./])(qutim)(?![a-z.])");
//	Q_ASSERT(qutimRegExp.isValid());
	html.replace( qutimRegExp, qutim_url );
	// \b(кутим[а-я]*)\b
	static QRegExp qutimRuRegExp(QString::fromUtf8("\b(кутим[а-я]*)\b"));
	Q_ASSERT(qutimRuRegExp.isValid());
	html.replace( qutimRuRegExp, qutim_url );
#endif

	TempGlobalInstance::instance().checkForEmoticons(html);
/*	QRegExp img_exp("<img\\s+src\\=\"[^\"]+\"");
	    pos = 0;
    while(((pos = img_exp.indexIn(html, pos)) != -1))
	{
		QString img_addr = img_exp.cap(1);
		int imgAddrCount = img_addr.count();
		img_addr = "file://" + img_addr;
		if ((pos == 0) || ((pos != 0) && (html[pos - 1] != ':')))
		{
			html.replace(pos, imgAddrCount, img_addr);
		}
		pos += img_addr.count();
	}*/
	if ( webkit )
	{
		static QRegExp img_src_regexp( "(<img.*src=[\"\']{1})(?!(\\:|file\\://))" );
		img_src_regexp.setMinimal( true );
		Q_ASSERT_X(img_src_regexp.isValid(),"LogsCity::findUrls","Img RegExp is not valid");
#if defined(Q_OS_WIN32)
		static QString fixed_str( "\\1file:///" );
#else
		static QString fixed_str( "\\1file://" );
#endif
		html.replace(img_src_regexp,fixed_str);
		static QRegExp img_qrc_regexp( "(<img.*src=[\"\']{1})(\\:)" );
		img_qrc_regexp.setMinimal( true );
		Q_ASSERT_X(img_src_regexp.isValid(),"LogsCity::findUrls","Img qrc RegExp is not valid");
		static QString fixed_qrc( "\\1qrc://" );
		html.replace(img_qrc_regexp,fixed_qrc);
    }
	return html;
}

void LogsCity::addImage(const TreeModelItem &item, const QByteArray &image_raw,bool in)
{
	QCryptographicHash hash(QCryptographicHash::Md5);
	hash.addData(image_raw);
	QString path_to_new_picture = m_tmp_image_path + hash.result().toHex();
	if ( !QFile::exists(path_to_new_picture) )
	{
		QFile pic(path_to_new_picture);
		if ( pic.open(QIODevice::WriteOnly) )
		{
			pic.write(image_raw);
			pic.close();
		}
	}
	QString	identification = QString("%1.%2.%3").arg(item.m_protocol_name)
	    .arg(item.m_account_name).arg(item.m_item_name);
	if ( m_city_map.contains(identification) )
	{
	    LogWidgetHome *tmp_home = m_city_map.value(identification);
	    tmp_home->addMessage(QString("<img src='%1'>").arg(path_to_new_picture),QDateTime::currentDateTime(),
				 false,in,
					"", false);
	}
}
