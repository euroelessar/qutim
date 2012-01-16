/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Alexander Kazarin <boiler@co.ru>
** Copyright © 2011 Sidorov Aleksey <sauron@citadelspb.com>
**
*****************************************************************************
**
** $QUTIM_BEGIN_LICENSE$
** This program is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
** See the GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program.  If not, see http://www.gnu.org/licenses/.
** $QUTIM_END_LICENSE$
**
****************************************************************************/


#include "messagehandler.h"
#include <qutim/debug.h>
#include <qutim/config.h>
#include <qutim/chatsession.h>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QTextDocument>

namespace UrlPreview {

using namespace qutim_sdk_0_3;

UrlHandler::UrlHandler() :
	m_netman(new QNetworkAccessManager(this))
{
	connect(m_netman, SIGNAL(authenticationRequired(QNetworkReply*,QAuthenticator*)),
			SLOT(authenticationRequired(QNetworkReply*,QAuthenticator*))
			);
	connect(m_netman, SIGNAL(finished(QNetworkReply*)),
			SLOT(netmanFinished(QNetworkReply*))
			);
	connect(m_netman, SIGNAL(sslErrors(QNetworkReply*,QList<QSslError>)),
			SLOT(netmanSslErrors(QNetworkReply*,QList<QSslError>))
			);
	loadSettings();
}

void UrlHandler::loadSettings()
{
	Config cfg;
	cfg.beginGroup("urlPreview");
	m_flags = cfg.value(QLatin1String("flags"), PreviewImages | PreviewYoutube);
	m_maxImageSize.setWidth(cfg.value(QLatin1String("maxWidth"), 800));
	m_maxImageSize.setHeight(cfg.value(QLatin1String("maxHeight"), 600));
	m_maxFileSize = cfg.value(QLatin1String("maxFileSize"), 100000);
	m_template = "<br><b>" + tr("URL Preview") + "</b>: <i>%TYPE%, %SIZE% " + tr("bytes") + "</i><br>";
	m_imageTemplate = "<img src=\"%URL%\" style=\"display: none;\" "
								 "onload=\"if (this.width>%MAXW%) this.style.width='%MAXW%px'; "
								 "if (this.height>%MAXH%) { this.style.width=''; this.style.height='%MAXH%px'; } "
								 "this.style.display='';\"><br>";
	m_youtubeTemplate =	"<img src=\"http://img.youtube.com/vi/%YTID%/1.jpg\">"
								   "<img src=\"http://img.youtube.com/vi/%YTID%/2.jpg\">"
								   "<img src=\"http://img.youtube.com/vi/%YTID%/3.jpg\"><br>";

	m_html5AudioTemplate = "<audio controls=\"controls\"><source src=\"%AUDIOURL%\" type=\"%FILETYPE%\"/> Something went wrong.</audio>";

	m_html5VideoTemplate = "<video controls=\"controls\"><source src=\"%VIDEOURL%\" type=\"%VIDEOTYPE%\" />Something went wrong.</video>";
	m_enableYoutubePreview = cfg.value("youtubePreview", true);
	m_enableImagesPreview = cfg.value("imagesPreview", true);
	m_enableHTML5Audio = cfg.value("HTML5Audio", true);
	m_enableHTML5Video = cfg.value("HTML5Video", true);
	cfg.endGroup();
}

UrlHandler::Result UrlHandler::doHandle(Message &message, QString *)
{
	ChatSession *session = ChatLayer::get(message.chatUnit(), false);
    if (!session || !session->property("supportJavaScript").toBool())
		return UrlHandler::Accept;

	debug() << Q_FUNC_INFO;
	QString html = message.html();
	if (html.isEmpty()) {
		html = Qt::escape(message.text());
		QString newHtml;
		const QRegExp &linkRegExp = getLinkRegExp();
		int pos = 0;
		int lastPos = 0;
		while (((pos = linkRegExp.indexIn(html, pos)) != -1)) {
			html.midRef(lastPos, pos - lastPos).appendTo(&newHtml);
			static int uid = 1;
			QString link = linkRegExp.cap(0);
			const QString oldLink = link;
			checkLink(link, const_cast<ChatUnit*>(message.chatUnit()), uid);
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
		//	checkLink(link, const_cast<ChatUnit*>(message.chatUnit()), message.id());
		//	cursor.removeSelectedText();
		//	cursor.insertHtml(link);
		//}
		//qDebug() << html << doc.toHtml();
		//html = doc.toHtml();
	}
	message.setHtml(html);
	return UrlHandler::Accept;
}

const QRegExp &UrlHandler::getLinkRegExp()
{
#if 0
	static QRegExp linkRegExp("([a-zA-Z0-9\\-\\_\\.]+@([a-zA-Z0-9\\-\\_]+\\.)+[a-zA-Z]+)|"
							  "(([a-zA-Z]+://|www\\.)([\\w:/\\?#\\[\\]@!\\$&\\(\\)\\*\\+,;=\\._~-]|&amp;|%[0-9a-fA-F]{2})+)",
							  Qt::CaseInsensitive);
#else
	static QRegExp linkRegExp("([a-z]+(\\+[a-z]+)?://|www\\.)"
							  "[\\w-]+(\\.[\\w-]+)*\\.\\w+"
							  "(:\\d+)?"
							  "(/[\\w\\+\\.\\[\\]!%\\$/\\(\\),:;@\\'&=~-]*"
							  "(\\?[\\w\\+\\.\\[\\]!%\\$/\\(\\),:;@\\'&=~-]*)?"
							  "(#[\\w\\+\\.\\[\\]!%\\$/\\\\\\(\\)\\|,:;@\\'&=~-]*)?)?",
							  Qt::CaseInsensitive, QRegExp::RegExp2);
#endif
	Q_ASSERT(linkRegExp.isValid());
	return linkRegExp;
}

void UrlHandler::checkLink(QString &link, qutim_sdk_0_3::ChatUnit *from, qint64 id)
{
	//TODO may be need refinement
	if (link.toLower().startsWith("www."))
		link.prepend("http://");
	link = QUrl::fromEncoded(link.toUtf8()).toString();
	link.replace(" ", QLatin1String("%20"));

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

	//link = QUrl::fromEncoded(link.toUtf8()).toString();
	link += " <span id='urlpreview"+uid+"'></span> ";

	debug() << "url" << link;
}

void UrlHandler::netmanFinished(QNetworkReply *reply)
{
	reply->deleteLater();
	QString url = reply->url().toString();
	QByteArray typeheader;
	QString decodedUrl = QUrl::fromEncoded(url.toUtf8()).toString();
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
	bool showPreviewHead = true;
	QRegExp typerx("^text/html");
	if (type.contains(typerx)) {
		showPreviewHead = false;
	}

	static QRegExp urlrx("^http://www\\.youtube\\.com/v/([\\w\\-]+)");
	if (urlrx.indexIn(url)==0 && m_enableYoutubePreview) {
		pstr = m_template;
		if (type == QLatin1String("application/x-shockwave-flash")) {
			showPreviewHead = false;
			pstr.replace("%TYPE%", tr("YouTube video"));
			pstr += m_youtubeTemplate;
			pstr.replace("%YTID%", urlrx.cap(1));
			pstr.replace("%SIZE%", tr("Unknown"));
		} else {
			pstr.replace("%SIZE%", QString::number(size));
		}
	}


	if (( ( (type == QLatin1String("audio/ogg") || type == QLatin1String("audio/mpeg")) || type == QLatin1String("application/ogg")) || type == QLatin1String("audio/x-wav") ) && m_enableHTML5Audio) {
				pstr = m_template;
				showPreviewHead = false;
				pstr.replace("%TYPE%", tr("HTML5 Audio"));
				pstr += m_html5AudioTemplate;
				if (type == QLatin1String("application/ogg")) {
					pstr.replace("%FILETYPE%", "audio/ogg");
				} else {
					pstr.replace("%FILETYPE%", type);
				}
				pstr.replace("%AUDIOURL%", url);
				pstr.replace("%SIZE%", QString::number(size));
			}

	if (((type == QLatin1String("video/webm") || type == QLatin1String("video/ogg")) || type == QLatin1String("video/mp4")) && m_enableHTML5Video) {
				pstr = m_template;
				showPreviewHead = false;
				pstr.replace("%TYPE%", tr("HTML5 Video"));
				pstr += m_html5VideoTemplate;
				pstr.replace("%VIDEOTYPE%", type);
				pstr.replace("%VIDEOURL%", url);
				pstr.replace("%SIZE%", QString::number(size));
			}


	if (showPreviewHead) {
		QString sizestr = size ? QString::number(size) : tr("Unknown");
		pstr = m_template;
		pstr.replace("%TYPE%", type);
		pstr.replace("%SIZE%", sizestr);
	}

	typerx.setPattern("^image/");
	if (type.contains(typerx) && 0 < size && size < m_maxFileSize && m_enableImagesPreview) {
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

	QMetaObject::invokeMethod(session, "evaluateJavaScript", Q_ARG(QString, js));
}

void UrlHandler::authenticationRequired(QNetworkReply *, QAuthenticator *)
{

}

void UrlHandler::netmanSslErrors(QNetworkReply *, const QList<QSslError> &)
{

}

} // namespace UrlPreview

