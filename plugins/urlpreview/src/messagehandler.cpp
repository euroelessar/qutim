/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Alexander Kazarin <boiler@co.ru>
** Copyright © 2011 Aleksey Sidorov <gorthauer87@yandex.ru>
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
#include <qutim/utils.h>
#include <qutim/json.h>

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QTextDocument>
#include <QStringBuilder>

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
	m_template = "<br><b>" % tr("URL Preview") % "</b>: <i>%TYPE%, %SIZE% " % tr("bytes") % "</i><br>";
	m_imageTemplate = "<img src=\"%URL%\" style=\"display: none;\" "
								 "onload=\"if (this.width>%MAXW%) this.style.width='%MAXW%px'; "
								 "if (this.height>%MAXH%) { this.style.width=''; this.style.height='%MAXH%px'; } "
								 "this.style.display='';\"><br>";
	m_youtubeTemplate =	"<img src=\"http://img.youtube.com/vi/%YTID%/1.jpg\">"
								   "<img src=\"http://img.youtube.com/vi/%YTID%/2.jpg\">"
								   "<img src=\"http://img.youtube.com/vi/%YTID%/3.jpg\"><br>";

	m_html5AudioTemplate = "<audio controls=\"controls\" preload=\"none\"><source src=\"%AUDIOURL%\" type=\"%FILETYPE%\"/>" % tr("Something went wrong.") % "</audio>";

	m_html5VideoTemplate = "<video controls=\"controls\" preload=\"none\"><source src=\"%VIDEOURL%\" type=\"%VIDEOTYPE%\" />" % tr("Something went wrong.") % "</video>";
	m_yandexRichContentTemplate = "<div class=\"yandex-rca\">"
								  "<img align=\"left\" src=\"%IMAGE%\" width=\"30%\"/>"
								  "<b>%TITLE%</b><br/>"
								  "%CONTENT%<br/>"
								  "</div>";
	m_enableYoutubePreview = cfg.value("youtubePreview", true);
	m_enableImagesPreview = cfg.value("imagesPreview", true);
	m_enableHTML5Audio = cfg.value("HTML5Audio", true);
	m_enableHTML5Video = cfg.value("HTML5Video", true);
	m_enableYandexRichContent = cfg.value("yandexRichContent", true);
	cfg.endGroup();
}

UrlHandler::Result UrlHandler::doHandle(Message &message, QString *)
{
	ChatSession *session = ChatLayer::get(message.chatUnit(), false);
    if (!session || !session->property("supportJavaScript").toBool())
		return UrlHandler::Accept;

	const QString originalHtml = message.html();
	QString html;
	foreach (const UrlParser::UrlToken &token,
			 UrlParser::tokenize(originalHtml, UrlParser::Html)) {
		if (token.url.isEmpty()) {
			html += token.text.toString();
		} else {
			static int uid = 1;
			QString link = token.url;
			checkLink(token.text, link, message.chatUnit(), uid++);
			html += link;
		}
	}
	message.setHtml(html);
	return UrlHandler::Accept;
}

void UrlHandler::checkLink(const QStringRef &originalLink, QString &link, ChatUnit *from, qint64 id)
{
	const char *entitiesIn[] = { "&quot;", "&gt;", "&lt;", "&amp;" };
	const char *entitiesOut[] = { "\"", ">", "<", "&" };
	const int entitiesCount = sizeof(entitiesIn) / sizeof(entitiesIn[0]);
	
	for (int i = 0; i < entitiesCount; ++i) {
		link.replace(QLatin1String(entitiesIn[i]),
		             QLatin1String(entitiesOut[i]),
		             Qt::CaseInsensitive);
	}

	const QUrl url = QUrl::fromUserInput(link);

	if (m_flags & PreviewYoutube) {
		const QString youtubeId = (url.host() == QLatin1String("youtube.com")
								   || url.host() == QLatin1String("www.youtube.com"))
								  ? (url.path().startsWith(QLatin1String("/v/"))
									 ? url.path().mid(3)
									 : (url.path().startsWith(QLatin1String("/embed/"))
										? url.path().mid(7)
										: url.queryItemValue(QLatin1String("v"))))
								  : (url.host() == QLatin1String("youtu.be")
									 ? url.path().mid(1)
									 : QString());

		if (!youtubeId.isEmpty()) {
			QString html = m_template;
			html.replace("%TYPE%", tr("YouTube video"));
			html += m_youtubeTemplate;
			html.replace("%YTID%", youtubeId);
			html.replace("%SIZE%", tr("Unknown"));
			html.prepend(originalLink.toString() + QLatin1String(" "));
			link = html;
			return;
		}
	}

	const QString uid = QString::number(id);

	QNetworkRequest request;
	request.setUrl(QUrl(link));
	request.setRawHeader("Ranges", "bytes=0-0");
	QNetworkReply *reply = m_netman->head(request);
	reply->setProperty("uid", uid);
	reply->setProperty("unit", qVariantFromValue<ChatUnit *>(from));

	link = QString::fromLatin1("%1 <span id='urlpreview%2'></span> ")
		   .arg(originalLink.toString(), uid);

	debug() << "url" << link;
}

void UrlHandler::netmanFinished(QNetworkReply *reply)
{
	reply->deleteLater();

	bool rcaIsValid = true;

	if (reply->property("yandexRCA").toBool()) {
		QVariantMap data = Json::parse(reply->readAll()).toMap();

		if (!data.contains("title") && !data.contains("content")) {
			rcaIsValid = false;
		} else {
			QString html = m_yandexRichContentTemplate;
			html.replace("%URL%", data.value("finalurl").toString());
			html.replace("%IMAGE%", data.value("img").toList().value(0).toString());
			html.replace("%TITLE%", data.value("title").toString().replace("\n", "<br/>"));
			html.replace("%CONTENT%", data.value("content").toString().replace("\n", "<br/>"));

			updateData(reply->property("unit").value<ChatUnit *>(),
					   reply->property("uid").toString(),
					   html);
			return;
		}
	}

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
	
	debug() << url << reply->rawHeaderList() << type;
	if (type.isNull())
		return;

	QString uid = reply->property("uid").toString();

	QString pstr;
	bool showPreviewHead = true;
	QRegExp typerx("^text/html");
	if (type.contains(typerx)) {
		showPreviewHead = false;
	}

	if (m_enableHTML5Audio &&
			(type == QLatin1String("audio/ogg")
			 || type == QLatin1String("audio/mpeg")
			 || type == QLatin1String("application/ogg")
			 || type == QLatin1String("audio/x-wav"))) {
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

	if (m_enableHTML5Video &&
			(type == QLatin1String("video/webm")
			 || type == QLatin1String("video/ogg")
			 || type == QLatin1String("video/mp4"))) {
		pstr = m_template;
		showPreviewHead = false;
		pstr.replace("%TYPE%", tr("HTML5 Video"));
		pstr += m_html5VideoTemplate;
		pstr.replace("%VIDEOTYPE%", type);
		pstr.replace("%VIDEOURL%", url);
		pstr.replace("%SIZE%", QString::number(size));
	}

	if (rcaIsValid && m_enableYandexRichContent &&
			(type == QLatin1String("text/html")
			 || type == QLatin1String("text/xhtml")
			 || type == QLatin1String("application/xhtml")
			 || type == QLatin1String("application/xhtml+xml"))) {
		QUrl rcaUrl(QLatin1String("http://rca.yandex.com/"));
		rcaUrl.addEncodedQueryItem("key", "svV1bfH1");
		rcaUrl.addEncodedQueryItem("url", url.toUtf8().toPercentEncoding("", "+"));
		QNetworkRequest request(rcaUrl);
		QNetworkReply *rcaReply = m_netman->get(request);
		rcaReply->setProperty("yandexRCA", true);
		rcaReply->setProperty("uid", reply->property("uid"));
		rcaReply->setProperty("unit", reply->property("unit"));
		return;
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

	updateData(reply->property("unit").value<ChatUnit *>(),
			   uid,
			   pstr);
}

void UrlHandler::updateData(ChatUnit *unit, const QString &uid, const QString &html)
{
	QString js = QLatin1Literal("urlpreview")
				 % uid
				 % QLatin1Literal(".innerHTML = \"")
				 % QString(html).replace("\"", "\\\"")
				 % QLatin1Literal("\";");
	ChatSession *session = ChatLayer::get(unit);

	debug() << unit << uid << js;

	QMetaObject::invokeMethod(session, "evaluateJavaScript", Q_ARG(QString, js));
}

void UrlHandler::authenticationRequired(QNetworkReply *, QAuthenticator *)
{

}

void UrlHandler::netmanSslErrors(QNetworkReply *, const QList<QSslError> &)
{

}

} // namespace UrlPreview

