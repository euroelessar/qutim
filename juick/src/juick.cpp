/*

    Copyright (c) 2011 by Stanislav (proDOOMman) Kosolapov <prodoomman@gmail.com>

 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
*/

#include "juick.h"
#include <qutim/debug.h>
#include <qutim/messagesession.h>
#include <qutim/config.h>

namespace Juick
{
	void JuickPlugin::init()
	{
		debug() << Q_FUNC_INFO;
		setInfo(QT_TRANSLATE_NOOP("Plugin", "Juick"),
				QT_TRANSLATE_NOOP("Plugin", "Some integration with microblogging juick.com"),
				PLUGIN_VERSION(0, 1, 0, 0));
		addAuthor(QT_TRANSLATE_NOOP("Author","Stanislav (proDOOMman) Kosolapov"),
				  QT_TRANSLATE_NOOP("Task","Author"),
				  QLatin1String("prodoomman@gmail.com"));
	}
	
	bool JuickPlugin::load()
	{
		loadSettings();
		ChatLayer *layer = ChatLayer::instance();
		connect(layer,SIGNAL(sessionCreated(qutim_sdk_0_3::ChatSession*)),
				SLOT(sessionCreated(qutim_sdk_0_3::ChatSession*))
				);

		settings= new GeneralSettingsItem<JuickSettings>( Settings::Plugin, QIcon( ":/juick.png" ), QT_TRANSLATE_NOOP("Juick", "Juick"));
		Settings::registerItem( settings );
		connect(settings->widget(), SIGNAL(saved()), this, SLOT(loadSettings()));
		return true;
	}
	
	bool JuickPlugin::unload()
	{
		return true;
	}
	
	void JuickPlugin::sessionCreated(qutim_sdk_0_3::ChatSession* session)
	{
		QStringList juickJids;
		juickJids << "juick@juick.com" << "jubo@nologin.ru";
		if(!juickJids.contains(session->unit()->id(),Qt::CaseInsensitive))
			return;
		connect(session,
				SIGNAL(messageReceived(qutim_sdk_0_3::Message*)),
				SLOT(processMessage(qutim_sdk_0_3::Message*))
				);
		connect(session,
				SIGNAL(messageSent(qutim_sdk_0_3::Message*)),
				SLOT(processMessage(qutim_sdk_0_3::Message*))
				);
	}
	
	void JuickPlugin::processMessage(qutim_sdk_0_3::Message* message)
	{		
		QString html = message->property("html").toString();
		if (html.isEmpty())
		{
		    html = Qt::escape(message->text());
		}
		QString msg = html;
		
		static QRegExp jpost("(#\\d{3,}\\b|#\\d{3,}/\\d+\\b)");
		static QRegExp jusers("(@[a-zA-Z0-9-_@\\.]+)\\b");
		static QRegExp junderline("(\\s?)(_(\\S+)_)(\\s?)");
		static QRegExp jbold("(\\s?)(\\*(\\S+)\\*)(\\s?)");
		static QRegExp jitalic("(\\s+)(/(\\S+)/)(\\s+)");
		
		QString user_style = "color:"+m_user_color.name()+";";
		QString msg_style = "color:"+m_msg_color.name()+";";
		if (m_user_bold) user_style += "font-weight:bold;";
		if (m_user_italic) user_style += "font-style:italic;";
		if (m_user_underline) user_style += "text-decoration:underline;";
		if (m_msg_bold) msg_style += "font-weight:bold;";
		if (m_msg_italic) msg_style += "font-style:italic;";
		if (m_msg_underline) msg_style += "text-decoration:underline;";
		
		QString toReplace;

		int pos = 0;
		while ((pos = jpost.indexIn(html, pos)) != -1) {
		    toReplace = "<span onclick=\"client.appendText('"+jpost.cap(1)+"')\" style=\""+msg_style+"\">"+jpost.cap(1)+"</span>";
		    if(m_extra_cmds && !jpost.cap(1).contains("/"))
		    {
			if(!m_post_cmds.isEmpty())
			{
			    toReplace = ")"+toReplace;
			    foreach(QString left, m_post_cmds)
			    {
				toReplace = " <span onclick=\"client.appendText('"+left+" "+jpost.cap(1)+"')\" style=\""+msg_style+"\">"+left+"</span>" + toReplace;
			    }
			    toReplace.remove(0,1);
			    toReplace = "("+toReplace;
			}
			toReplace += "<span onclick=\"client.appendText('"+jpost.cap(1)+"+')\" style=\""+msg_style+"\">+</span>";
		    }
		    html.replace(pos, jpost.cap(1).length(), toReplace);
		    pos += toReplace.length();
		}
		pos = 0;
		while ((pos = jusers.indexIn(html, pos)) != -1) {
		    toReplace = "<span onclick=\"client.appendText('"+jusers.cap(1)+"')\" style=\""+user_style+"\">"+jusers.cap(1)+"</span>";
		    if(m_extra_cmds)
		    {
			if(!m_user_cmds.isEmpty())
			{
			    toReplace = ")"+toReplace;
			    foreach(QString left, m_user_cmds)
			    {
				toReplace = " <span onclick=\"client.appendText('"+left+" "+jusers.cap(1)+"')\" style=\""+user_style+"\">"+left+"</span>" + toReplace;
			    }
			    toReplace.remove(0,1);
			    toReplace = "("+toReplace;
			}
			toReplace += "<span onclick=\"client.appendText('"+jusers.cap(1)+"+')\" style=\""+user_style+"\">+</span>";
		    }
		    html.replace(pos, jusers.cap(1).length(), toReplace);
		    pos += toReplace.length();
		}
		// Underlines
		pos = 0;
		while ((pos = junderline.indexIn(msg, pos)) != -1) {
		    html.replace(junderline.cap(2), "<u>"+junderline.cap(3)+"</u>");
		    pos += junderline.matchedLength();
		}
		// Bolds
		pos = 0;
		while ((pos = jbold.indexIn(msg, pos)) != -1) {
		    html.replace(jbold.cap(2), "<b>"+jbold.cap(3)+"</b>");
		    pos += jbold.matchedLength();
		}
		// Italics
		pos = 0;
		while ((pos = jitalic.indexIn(msg, pos)) != -1) {
		    html.replace(jitalic.cap(2), "<i>"+jitalic.cap(3)+"</i>");
		    pos += jitalic.matchedLength();
		}

		message->setProperty("html",html);
	}

	void JuickPlugin::loadSettings()
	{
	    Config mainGroup = Config("juick").group("main");
	    m_msg_bold = mainGroup.value("post_bold",false);
	    m_msg_italic = mainGroup.value("post_italic",false);
	    m_msg_underline = mainGroup.value("post_under",false);
	    m_user_bold = mainGroup.value("user_bold",true);
	    m_user_italic = mainGroup.value("user_italic",false);
	    m_user_underline = mainGroup.value("user_under",false);
	    m_user_color = QColor(mainGroup.value("user_color",QColor(0, 0, 0).name()));
	    m_msg_color = QColor(mainGroup.value("post_color",QColor(0, 128, 0).name()));
	    m_extra_cmds = mainGroup.value("extra_cmds",true);
	    m_user_cmds = mainGroup.value("user_cmds","PM S U").split(" ",QString::SkipEmptyParts);
	    m_post_cmds = mainGroup.value("post_cmds","! S D U").split(" ",QString::SkipEmptyParts);
	}
}

QUTIM_EXPORT_PLUGIN(Juick::JuickPlugin);
