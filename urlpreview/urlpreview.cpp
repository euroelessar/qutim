/*
    urlpreviewPlugin

		Copyright (c) 2008-2009 by Alexander Kazarin <boiler@co.ru>

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

bool urlpreviewPlugin::init(PluginSystemInterface *plugin_system)
{
	qRegisterMetaType<TreeModelItem>("TreeModelItem");

	PluginInterface::init(plugin_system);
	m_plugin_icon = new QIcon(":/icons/urlpreview-plugin.png");
	m_plugin_system = plugin_system;

	netman = new QNetworkAccessManager();
	connect(netman, SIGNAL(finished(QNetworkReply*)), this, SLOT(netmanFinished(QNetworkReply*)));
        connect(netman, SIGNAL(authenticationRequired(QNetworkReply*,QAuthenticator*)), this, SLOT(authenticationRequired(QNetworkReply*,QAuthenticator*)));
        connect(netman, SIGNAL(sslErrors(QNetworkReply*,QList<QSslError>)), this, SLOT(netmanSslErrors(QNetworkReply*,QList<QSslError>)));

	m_event_msgin = m_plugin_system->registerEventHandler("Core/ChatWindow/ReceiveLevel3", this);
	m_event_msgout = m_plugin_system->registerEventHandler("Core/ChatWindow/SendLevel1", this);
	
	return true;
}

QIcon *urlpreviewPlugin::icon()
{
	return m_plugin_icon;
}

void urlpreviewPlugin::saveSettings()
{
	settingswidget->saveSettings();
        loadSettings();
}

void urlpreviewPlugin::loadSettings()
{
	QSettings settings(QSettings::IniFormat, QSettings::UserScope, "qutim/qutim."+m_profile_name, "plugin_urlpreview");

        QString template_default = "<br><b>"+tr("URL Preview")+"</b>: <i>%TYPE%, %SIZE% "+tr("bytes")+"</i><br>";
	if (settings.value("template").isNull()) settings.setValue("template", template_default);

        QString template_image_default = "<img src=\"%URL%\" style=\"display: none;\" onload=\"if (this.width>%MAXW%) this.style.width='%MAXW%px'; if (this.height>%MAXH%) { this.style.width=''; this.style.height='%MAXH%px'; } this.style.display='';\"><br>";
	if (settings.value("image/template").isNull()) settings.setValue("image/template", template_image_default);

        QString template_youtube_default = "<img src=\"http://img.youtube.com/vi/%YTID%/1.jpg\"> <img src=\"http://img.youtube.com/vi/%YTID%/2.jpg\"> <img src=\"http://img.youtube.com/vi/%YTID%/3.jpg\"><br>";
	if (settings.value("youtube/template").isNull()) settings.setValue("youtube/template", template_youtube_default);

	cfg_enable_in = settings.value("enable_in", true).toBool();
	cfg_enable_out = settings.value("enable_out", false).toBool();
	cfg_disable_text_html = settings.value("disable_text_html", true).toBool();
        cfg_ignore_sslwarn = settings.value("ignore_sslwarn", false).toBool();
        cfg_str["template"] = settings.value("template", template_default).toString();
	cfg_image_maxfilesize = settings.value("image/maxfilesize", 100000).toUInt();
        cfg_str["image_maxwidth"] = settings.value("image/maxwidth", "400").toString();
        cfg_str["image_maxheight"] = settings.value("image/maxheight", "600").toString();
        cfg_str["image_template"] = settings.value("image/template", template_image_default).toString();
	cfg_enable_youtube = settings.value("youtube/enable", true).toBool();
        cfg_str["youtube_template"] = settings.value("youtube/template", template_image_default).toString();
}

QWidget *urlpreviewPlugin::settingsWidget()
{
	settingswidget = new urlpreviewSettings(m_profile_name);
	connect(settingswidget, SIGNAL(settingsChanged()), this, SLOT(loadSettings()));
	return settingswidget;
}

void urlpreviewPlugin::removeSettingsWidget()
{
	delete settingsWidget();
}

void urlpreviewPlugin::setProfileName(const QString &profile_name)
{
	m_profile_name = profile_name;
	loadSettings();
}


void urlpreviewPlugin::processEvent(Event &event)
{
	if ((event.id==m_event_msgin && cfg_enable_in) ||
			(event.id==m_event_msgout && cfg_enable_out)) {
		TreeModelItem item = *(TreeModelItem*)(event.args.at(0));
		QString *msg = (QString*)(event.args.at(1));

		QRegExp urlRegExp("(\\<a\\s+href='([^'\\s]+)[^<]+</a>)");
		int pos = 0;
		QString smsg = *msg;
		QString rmsg = *msg;
		while ((pos = urlRegExp.indexIn(smsg, pos)) != -1) {
			QString url = urlRegExp.cap(2);
			QRegExp urlrx("youtube\\.com/watch\\?v\\=([^\\&]*)");
			if (urlrx.indexIn(url)>-1 && cfg_enable_youtube) {
			  url = "http://www.youtube.com/v/"+urlrx.cap(1);
			}

			QString uid;
			while (uid.isEmpty() || uidurls.contains(uid)) {
				uid = QString::number(rand() % 0x8000);
			}
			uidurls[uid] = url;
			uiditems[uid] = item;

			rmsg.replace(urlRegExp.cap(1), urlRegExp.cap(1)+"<span id='urlpreview"+uid+"'></span>");
			pos += urlRegExp.matchedLength();

			QNetworkRequest netreq;
			netreq.setUrl(QUrl(url));
			netreq.setRawHeader("Ranges", "bytes=0-0");
			netman->head(netreq);

//                        qDebug()<<"URLPreview request"<<url<<uid; // <<rmsg;
//                        qDebug()<<"URLPreview event"<<item.m_account_name<<item.m_item_history<<item.m_item_type<<item.m_parent_name<<item.m_protocol_name;
		}
		*msg = rmsg;
	}
}

void urlpreviewPlugin::netmanFinished( QNetworkReply* reply )
{
        QNetworkRequest netreq = reply->request();
        QString url = netreq.url().toString();

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
//        qDebug()<<"URLPreview reply"<<type<<size<<url;

        if (!type.isEmpty()) {
		foreach (QString uid, uidurls.keys(url)) {
			if (uidurls[uid]==url) {
				QString pstr;
				bool show_preview_head=true;
				QRegExp typerx("^text/html");
				if (typerx.indexIn(type)==0 && cfg_disable_text_html) {
					show_preview_head=false;
				}

                                QRegExp urlrx("^http://www\\.youtube\\.com/v/([\\w\\-]+)");
				if (urlrx.indexIn(url)==0 && cfg_enable_youtube) {
					pstr = cfg_str["template"];
					if (type == "application/x-shockwave-flash") {
						show_preview_head=false;
						pstr.replace("%TYPE%", tr("YouTube video"));
						pstr += cfg_str["youtube_template"];
						pstr.replace("%YTID%", urlrx.cap(1));
                                                pstr.replace("%SIZE%", tr("Unknown"));
					}
                                        else {
                                                pstr.replace("%SIZE%", QString::number(size));
                                        }
				}

				if (show_preview_head) {
                                        QString sizestr = (size>0) ? QString::number(size) : tr("Unknown");
					pstr = cfg_str["template"];
					pstr.replace("%TYPE%", type);
                                        pstr.replace("%SIZE%", sizestr);
				}

				typerx.setPattern("^image/");
                                if (typerx.indexIn(type)==0 && size>0 && size<cfg_image_maxfilesize) {
					QString amsg = cfg_str["image_template"];
					amsg.replace("%URL%", url);
					amsg.replace("%UID%", uid);
					amsg.replace("%MAXW%", cfg_str["image_maxwidth"]);
					amsg.replace("%MAXH%", cfg_str["image_maxheight"]);
					pstr += amsg;
				}

                                printStrToUID (uid, pstr);
                                uidurls[uid].clear();
                                uiditems.remove(uid);
			}

		}
	}

}

void urlpreviewPlugin::authenticationRequired ( QNetworkReply * reply, QAuthenticator * /*authenticator*/ ) {
        QNetworkRequest netreq = reply->request();
        QString url = netreq.url().toString();
//        qDebug()<<"URLPreview authenticationRequired"<<url;

        QString pstr = cfg_str["template"];
        pstr.replace("%TYPE%", tr("Authentication Required"));
        pstr.replace("%SIZE%", tr("Unknown"));

        foreach (QString uid, uidurls.keys(url)) {
            if (uidurls[uid]==url) {
                printStrToUID (uid, pstr);
            }
            printStrToUID (uid, pstr);
            uidurls[uid].clear();
            uiditems.remove(uid);
        }

}

void urlpreviewPlugin::netmanSslErrors(QNetworkReply * reply, const QList<QSslError> &) {
    if (cfg_ignore_sslwarn) reply->ignoreSslErrors();
}

void urlpreviewPlugin::printStrToUID (QString uid, QString pstr) {
                                QString js = "urlpreview"+uid+".innerHTML = \""+pstr.replace("\"", "\\\"")+"\";";
                                TreeModelItem item = uiditems[uid];
                                quint16 event_evaljs_id = SystemsCity::PluginSystem()->registerEventHandler("Core/ChatWindow/EvaluateJavaScript");

//				qDebug()<<"URLPreview reply"<<item.m_account_name<<item.m_item_history<<item.m_item_type<<item.m_parent_name<<item.m_protocol_name;

                                if (item.m_item_type==34) {
                                        item.m_item_type = 32;
                                        item.m_item_name = item.m_parent_name;
                                        item.m_parent_name = item.m_account_name;
                                }

                                Event(event_evaljs_id, 2, &item, &js).send();
}

Q_EXPORT_PLUGIN2(urlpreviewPlugin,urlpreviewPlugin);
