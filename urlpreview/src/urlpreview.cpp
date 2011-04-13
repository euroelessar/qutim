/*
    UrlPreviewPlugin

  Copyright (c) 2008-2009 by Alexander Kazarin <boiler@co.ru>
  (c) 2010 by Sidorov Aleksey <sauron@citadelspb.com>

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
#include <qutim/config.h>
#include <qutim/messagesession.h>
#include <QNetworkRequest>
#include <QNetworkAccessManager>
#include <qnetworkreply.h>
#include <QTextDocument>
#include <QTextCursor>

namespace UrlPreview
{
void UrlPreviewPlugin::init()
{
	debug() << Q_FUNC_INFO;
	setInfo(QT_TRANSLATE_NOOP("Plugin", "UrlPreview"),
			QT_TRANSLATE_NOOP("Plugin", "Preview images directly in the chat window"),
			PLUGIN_VERSION(0, 1, 0, 0));
	addAuthor(QT_TRANSLATE_NOOP("Author","Alexander Kazarin"),
			  QT_TRANSLATE_NOOP("Task","Author"),
			  QLatin1String("boiler@co.ru"));
	addAuthor(QT_TRANSLATE_NOOP("Author","Sidorov Aleksey"),
			  QT_TRANSLATE_NOOP("Task","Developer"),
			  QLatin1String("sauron@citadelspb.com"),
			  QLatin1String("sauron.me"));
	m_netman = 0;
}

bool UrlPreviewPlugin::load()
{
	if (m_netman)
		return false;
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

	loadSettings();
	return true;
}

void UrlPreviewPlugin::loadSettings()
{
	Config cfg;
	cfg.beginGroup(QLatin1String("urlpreview"));
	m_flags = cfg.value(QLatin1String("falgs"), PreviewImages | PreviewYoutube);
	m_maxImageSize.setWidth(cfg.value(QLatin1String("maxWidth"), 800));
	m_maxImageSize.setHeight(cfg.value(QLatin1String("maxHeight"), 600));
	m_maxFileSize = cfg.value(QLatin1String("maxFileSize"), 100000);
	m_template = cfg.value(QLatin1String("template"), "<br><b>" + tr("URL Preview")
	                       + "</b>: <i>%TYPE%, %SIZE% " + tr("bytes") + "</i><br>");
	m_imageTemplate = cfg.value(QLatin1String("imageTemplate"), "<img src=\"%URL%\" style=\"display: none;\" "
	                             "onload=\"if (this.width>%MAXW%) this.style.width='%MAXW%px'; "
	                             "if (this.height>%MAXH%) { this.style.width=''; this.style.height='%MAXH%px'; } "
	                             "this.style.display='';\"><br>");
	m_youtubeTemplate = cfg.value(QLatin1String("youtubeTemplate"),
	                               "<img src=\"http://img.youtube.com/vi/%YTID%/1.jpg\">"
	                               "<img src=\"http://img.youtube.com/vi/%YTID%/2.jpg\">"
	                               "<img src=\"http://img.youtube.com/vi/%YTID%/3.jpg\"><br>");
}

bool UrlPreviewPlugin::unload()
{
	if (m_netman) {
		m_netman->deleteLater();
		m_netman = 0;
		return true;
	}
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
	if (!m_netman)
		return;
	QString html = message->property("html").toString();
	if (html.isEmpty()) {
		html = Qt::escape(message->text());
		QString newHtml;
		const QRegExp &linkRegExp = getLinkRegExp();
		int pos = 0;
		int lastPos = 0;
		while (((pos = linkRegExp.indexIn(html, pos)) != -1)) {
			html.midRef(lastPos, pos - lastPos).appendTo(&newHtml);
			static int uid = 1;
			QString link = linkRegExp.cap(0);
			const QString oldLink = link;
			checkLink(link, const_cast<ChatUnit*>(message->chatUnit()), uid);
			newHtml += link;
			uid++;
			pos = lastPos = pos + oldLink.size();
		}
		html.midRef(lastPos, html.size() - lastPos).appendTo(&newHtml);
		html = newHtml;
	} else {
		//QTextDocument doc(html);
		//const QRegExp &linkRegExp = getLinkRegExp();
		//QTextCursor cursor(&doc);
		//while (true) {
		//	cursor = doc.find(linkRegExp, cursor);
		//	if (cursor.isNull())
		//		break;
		//	QString link = cursor.selectedText();
		//	checkLink(link, const_cast<ChatUnit*>(message->chatUnit()), message->id());
		//	cursor.removeSelectedText();
		//	cursor.insertHtml(link);
		//}
		//qDebug() << html << doc.toHtml();
		//html = doc.toHtml();
	}
	message->setProperty("html",html);
}

const QRegExp &UrlPreviewPlugin::getLinkRegExp()
{
#if 0
	static QRegExp linkRegExp("([a-zA-Z0-9\\-\\_\\.]+@([a-zA-Z0-9\\-\\_]+\\.)+[a-zA-Z]+)|"
							  "(([a-zA-Z]+://|www\\.)([\\w:/\\?#\\[\\]@!\\$&\\(\\)\\*\\+,;=\\._~-]|&amp;|%[0-9a-fA-F]{2})+)",
							  Qt::CaseInsensitive);
#else
	static QRegExp linkRegExp("([a-z]+(\\+[a-z]+)?://|www\\.)"
	                          "[\\w-]+(\\.[\\w-]+)*\\.\\w+"
	                          "(:\\d+)?"
	                          "(/[\\w\\+\\.\\[\\]!%\\$/\\(\\),&=~-]*"
	                          "(\\?[\\w\\+\\.\\[\\]!%\\$/\\(\\),&=~-]*)?"
	                          "(#[\\w\\+\\.\\[\\]!%\\$/\\(\\),&=~-]*)?)?",
	                          Qt::CaseInsensitive, QRegExp::RegExp2);
#endif
	Q_ASSERT(linkRegExp.isValid());
	return linkRegExp;
}

void UrlPreviewPlugin::checkLink(QString &link, ChatUnit *from, qint64 id)
{
	//TODO may be need refinement
	if (link.toLower().startsWith("www."))
		link.prepend("http://");

	static QRegExp urlrx("youtube\\.com/watch\\?v\\=([^\\&]*)");
	Q_ASSERT(urlrx.isValid());
	if (urlrx.indexIn(link)>-1 && (m_flags)) {
		link = QLatin1String("http://www.youtube.com/v/") + urlrx.cap(1);
	}

	QString uid = QString::number(id);

	QNetworkRequest request;
	request.setUrl(QUrl(link));
	request.setRawHeader("Ranges", "bytes=0-0");
	QNetworkReply *reply = m_netman->head(request);
	reply->setProperty("uid", uid);
	reply->setProperty("unit", qVariantFromValue<ChatUnit *>(from));

	link += " <span id='urlpreview"+uid+"'></span> ";

	debug() << "url" << link;
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
	quint64 size=0;
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
	if (type.contains(typerx)) {
		show_preview_head=false;
	}

	QRegExp urlrx("^http://www\\.youtube\\.com/v/([\\w\\-]+)");
	if (urlrx.indexIn(url)==0 && (m_flags & PreviewYoutube)) {
		pstr = m_template;
		if (type == "application/x-shockwave-flash") {
			show_preview_head=false;
			pstr.replace("%TYPE%", tr("YouTube video"));
			pstr += m_youtubeTemplate;
			pstr.replace("%YTID%", urlrx.cap(1));
			pstr.replace("%SIZE%", tr("Unknown"));
		}
		else {
			pstr.replace("%SIZE%", QString::number(size));
		}
	}

	if (show_preview_head) {
		QString sizestr = size ? QString::number(size) : tr("Unknown");
		pstr = m_template;
		pstr.replace("%TYPE%", type);
		pstr.replace("%SIZE%", sizestr);
	}

	typerx.setPattern("^image/");
	if (type.contains(typerx) && size<m_maxFileSize) {
		QString amsg = m_imageTemplate;
		amsg.replace("%URL%", url);
		amsg.replace("%UID%", uid);
		amsg.replace("%MAXW%", QString::number(m_maxImageSize.width()));
		amsg.replace("%MAXH%", QString::number(m_maxImageSize.height()));
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
