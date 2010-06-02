/*
    UrlPreviewPlugin

		Copyright (c) 2008-2009 by Alexander Kazarin <boiler@co.ru>
		(c) 2010 by Aleksey Sidorov <sauron@citadelspb.com> 

 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
*/

#include "urlpreview.h"
#include <qutim/debug.h>
#include <qutim/messagesession.h>
#include <QNetworkRequest>
#include <QNetworkAccessManager>
#include <qnetworkreply.h>

namespace UrlPreview
{
	void UrlPreviewPlugin::init()
	{
		debug() << Q_FUNC_INFO;
		addAuthor(QT_TRANSLATE_NOOP("Author","Alexander Kazarin"),
				  QT_TRANSLATE_NOOP("Task","Author"),
				  QLatin1String("boiler@co.ru"));
		addAuthor(QT_TRANSLATE_NOOP("Author","Aleksey Sidorov"),
				  QT_TRANSLATE_NOOP("Task","Developer"),
				  QLatin1String("sauron@citadelspb.com"),
				  QLatin1String("sauron.me"));
	}
	
	bool UrlPreviewPlugin::load()
	{
		ChatLayer *layer = ChatLayer::instance();
		m_netman =  new QNetworkAccessManager(this);

		connect(layer,SIGNAL(sessionCreated(qutim_sdk_0_3::ChatSession*)),
				SLOT(sessionCreated(qutim_sdk_0_3::ChatSession*))
				);
		connect(m_netman,SIGNAL(authenticationRequired(QNetworkReply*,QAuthenticator*)),
				SLOT(authenticationRequired(QNetworkReply*,QAuthenticator*))
				);
		connect(m_netman,SIGNAL(finished(QNetworkReply*)),
				SLOT(netmanFinished(QNetworkReply*))
				);
		connect(m_netman,SIGNAL(sslErrors(QNetworkReply*,QList<QSslError>)),
				SLOT(netmanSslErrors(QNetworkReply*,QList<QSslError>))
				);

		//TODO
		m_flags = static_cast<PreviewFlags>(PreviewImages | PreviewYoutube);
		m_max_image_size =  QSize(800,600);
		m_maxfilesize = 100000;
		m_template = "<br><b>"+tr("URL Preview")+"</b>: <i>%TYPE%, %SIZE% "+tr("bytes")+"</i><br>";
		m_image_template = "<img src=\"%URL%\" style=\"display: none;\" onload=\"if (this.width>%MAXW%) this.style.width='%MAXW%px'; if (this.height>%MAXH%) { this.style.width=''; this.style.height='%MAXH%px'; } this.style.display='';\"><br>";
		m_youtube_template = "<img src=\"http://img.youtube.com/vi/%YTID%/1.jpg\"> <img src=\"http://img.youtube.com/vi/%YTID%/2.jpg\"> <img src=\"http://img.youtube.com/vi/%YTID%/3.jpg\"><br>";

		return false;
	}
	
	bool UrlPreviewPlugin::unload()
	{
		m_netman->deleteLater();
		return false;
	}
	
	void UrlPreviewPlugin::sessionCreated(qutim_sdk_0_3::ChatSession* session)
	{
		connect(session,
				SIGNAL(messageReceived(qutim_sdk_0_3::Message*)),
				SLOT(processMessage(qutim_sdk_0_3::Message*))
				);
		connect(session,
				SIGNAL(messageSent(qutim_sdk_0_3::Message*)),
				SLOT(processMessage(qutim_sdk_0_3::Message*))
				);
	}
	
	void UrlPreviewPlugin::processMessage(qutim_sdk_0_3::Message* message)
	{		
		//TODO may be need refinement
		QString html = message->property("html").toString();
		if (html.isEmpty())
			html = message->text();
		
		static QRegExp linkRegExp("([a-zA-Z0-9\\-\\_\\.]+@([a-zA-Z0-9\\-\\_]+\\.)+[a-zA-Z]+)|"
								  "(([a-zA-Z]+://|www\\.)([\\w:/\\?#\\[\\]@!\\$&\\(\\)\\*\\+,;=\\._~-]|&amp;|%[0-9a-fA-F]{2})+)",
								  Qt::CaseInsensitive);
		Q_ASSERT(linkRegExp.isValid());
		int pos = 0;
		while(((pos = linkRegExp.indexIn(html, pos)) != -1))
		{
			QString link = linkRegExp.cap(0);
			QString tmplink = link;
			if (tmplink.toLower().startsWith("www."))
				tmplink.prepend("http://");

			QRegExp urlrx("youtube\\.com/watch\\?v\\=([^\\&]*)");
			if (urlrx.indexIn(tmplink)>-1 && (m_flags)) {
				tmplink = "http://www.youtube.com/v/"+urlrx.cap(1);
			}
			
			QString uid = QString::number(message->id());
			
			debug() << "url" << tmplink;
			
			html.replace(pos, link.length(), tmplink +"<span id='urlpreview"+uid+"'></span>");
			pos += tmplink.count();
			
			QNetworkRequest request;
			request.setUrl(QUrl(tmplink));
			request.setRawHeader("Ranges", "bytes=0-0");			
			QNetworkReply *reply = m_netman->head(request);
			reply->setProperty("uid",uid);
			reply->setProperty("unit",qVariantFromValue<ChatUnit *>(const_cast<ChatUnit *>(message->chatUnit())));
			
		}
		message->setProperty("html",html);
	}

	void UrlPreviewPlugin::authenticationRequired(QNetworkReply* , QAuthenticator* )
	{

	}
	void UrlPreviewPlugin::netmanFinished(QNetworkReply* reply)
	{
		QString url = reply->url().toString();
        QByteArray typeheader;
        QString type;
        QByteArray sizeheader;
        qint64 size=-1;
        QRegExp hrx; hrx.setCaseSensitivity(Qt::CaseInsensitive);
        foreach (QString header, reply->rawHeaderList()) {
            if (type.isEmpty()) {
                hrx.setPattern("^content-type$");
                if (hrx.indexIn(header)==0) typeheader = header.toAscii();
            }
            if (sizeheader.isEmpty()) {
                hrx.setPattern("^content-range$");
                if (hrx.indexIn(header)==0) sizeheader = header.toAscii();
            }
            if (sizeheader.isEmpty()) {
                hrx.setPattern("^content-length$");
                if (hrx.indexIn(header)==0) sizeheader = header.toAscii();
            }
        }
        if (!typeheader.isEmpty()) {
            hrx.setPattern("^([^\\;]+)");
            if (hrx.indexIn(reply->rawHeader(typeheader))>=0)
				type = hrx.cap(1);
        }
        if (!sizeheader.isEmpty()) {
            hrx.setPattern("(\\d+)");
            if (hrx.indexIn(reply->rawHeader(sizeheader))>=0)
                size = hrx.cap(1).toInt();
        }

		if (type.isNull())
			return;
        
        QString uid = reply->property("uid").toString();
		
		QString pstr;
		bool show_preview_head=true;
		QRegExp typerx("^text/html");
		if (typerx.indexIn(type)!=0 && (m_flags & DisableTextHtml)) {
			show_preview_head=false;
		}

		QRegExp urlrx("^http://www\\.youtube\\.com/v/([\\w\\-]+)");
		if (urlrx.indexIn(url)==0 && (m_flags & PreviewYoutube)) {
			pstr = m_template;
			if (type == "application/x-shockwave-flash") {
				show_preview_head=false;
				pstr.replace("%TYPE%", tr("YouTube video"));
				pstr += m_youtube_template;
				pstr.replace("%YTID%", urlrx.cap(1));
				pstr.replace("%SIZE%", tr("Unknown"));
			}
			else {
				pstr.replace("%SIZE%", QString::number(size));
			}
		}

		if (show_preview_head) {
			QString sizestr = (size>0) ? QString::number(size) : tr("Unknown");
			pstr = m_template;
			pstr.replace("%TYPE%", type);
			pstr.replace("%SIZE%", sizestr);
		}

		typerx.setPattern("^image/");
		if (typerx.indexIn(type)==0 && size>0 && size<m_maxfilesize) {
			QString amsg = m_image_template;
			amsg.replace("%URL%", url);
			amsg.replace("%UID%", uid);
			amsg.replace("%MAXW%", QString::number(m_max_image_size.width()));
			amsg.replace("%MAXH%", QString::number(m_max_image_size.height()));
			pstr += amsg;
		}

		QString js = "urlpreview"+uid+".innerHTML = \""+pstr.replace("\"", "\\\"")+"\";";
		ChatUnit *unit = reply->property("unit").value<ChatUnit *>();
		ChatSession *session = ChatLayer::get(unit);
		session->metaObject()->invokeMethod(session,
											"evaluateJavaScript",
											Q_ARG(QString,js)
											);
	}
	void UrlPreviewPlugin::netmanSslErrors(QNetworkReply* , const QList< QSslError >& )
	{

	}


}

QUTIM_EXPORT_PLUGIN(UrlPreview::UrlPreviewPlugin);
