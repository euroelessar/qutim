/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Alexander Kazarin <boiler@co.ru>
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

#include "imagepub.h"

bool imagepubPlugin::init(PluginSystemInterface *plugin_system)
{
	qRegisterMetaType<TreeModelItem>("TreeModelItem");

	PluginInterface::init(plugin_system);
	imagepub_plugin_icon = new QIcon(":/icons/imagepub-icon16.png");
	imagepub_plugin_system = plugin_system;

	serv_name = "imageshack.us";
	serv_url = "http://load.imageshack.us/";
	serv_postdata = "uploadtype=image";
	serv_fileinput = "fileupload";
	serv_regexp = "Direct\\s*<a\\s+style=\\'[^\\']+\\'\\s+href=\\'([^\\']+)\\'>link<\\/a>";
	serv_filefilter = tr("Image Files")+" (*.png *.jpg *.gif)";

	serv = "imageshack";
	
	return true;
}

void imagepubPlugin::release()
{
	
}

void imagepubPlugin::processEvent(Event &event)
{
	if (event.id == event_id) {
		event_item = *(TreeModelItem*)(event.args.at(0));
	}
}

QWidget *imagepubPlugin::settingsWidget()
{
	settingswidget = new imagepubSettings(imagepub_profile_name);
	return settingswidget;
}

void imagepubPlugin::setProfileName(const QString &profile_name)
{
	imagepub_profile_name = profile_name;

	QString actiontitle = tr("Send image via ImagePub plugin");
	QString actionicon(":/icons/imagepub-icon16.png");

	QSettings settings(QSettings::IniFormat, QSettings::UserScope, "qutim/qutim."+imagepub_profile_name, "plugin_imagepub");
	if (settings.value("main/actiontitle").isValid()) actiontitle = settings.value("main/actiontitle").toString();
	else settings.setValue("main/actiontitle", actiontitle);
	if (settings.value("main/actionicon").isValid()) actionicon = settings.value("main/actionicon").toString();

	QAction* menuaction = new QAction(QIcon(actionicon), actiontitle, this);
	connect(menuaction, SIGNAL(triggered()), this,  SLOT(actionStart()));

	m_plugin_system->registerContactMenuAction(menuaction);
	event_id = m_plugin_system->registerEventHandler("Core/ContactList/ContactContext", this);
}

QString imagepubPlugin::name()
{
	return "ImagePub";
}

QString imagepubPlugin::description()
{
	return "Send files via image exchange services";
}

QIcon *imagepubPlugin::icon()
{
	return imagepub_plugin_icon;
}

QString imagepubPlugin::type()
{
	return "other";
}


void imagepubPlugin::removeSettingsWidget()
{
	delete settingsWidget();
}

void imagepubPlugin::saveSettings()
{
	settingswidget->saveSettings();
}

void imagepubPlugin::actionStart()
{
	if (!event_item.m_item_name.isEmpty()) {
//qDebug()<<"Event item name"<<event_item.m_item_name;
		QSettings settings(QSettings::IniFormat, QSettings::UserScope, "qutim/qutim."+imagepub_profile_name, "plugin_imagepub");
		if (settings.value("main/service").isValid()) {
			serv = settings.value("main/service").toString();
			if (QUrl(settings.value("serv-"+serv+"/url").toString()).isValid()) {
				serv_name = settings.value("serv-"+serv+"/name").toString();
				serv_url = settings.value("serv-"+serv+"/url").toString();
				serv_postdata = settings.value("serv-"+serv+"/postdata").toString();
				serv_fileinput = settings.value("serv-"+serv+"/fileinput").toString();
				serv_regexp = settings.value("serv-"+serv+"/regexp").toString();
				serv_filefilter = settings.value("serv-"+serv+"/filefilter").toString();
			}
		}

		uploadwidget = new uploadDialog();
		connect(uploadwidget, SIGNAL(canceled()), this, SLOT(removeUploadWidget()));
		uploadwidget->show();

		netman = new QNetworkAccessManager();
		connect ( netman, SIGNAL ( finished( QNetworkReply* ) ), this, SLOT ( netrpFinished( QNetworkReply* ) ) );
		QNetworkRequest netreq;
		netreq.setUrl(QUrl(serv_url));

		uploadwidget->setStatus(tr("Choose file for upload to")+"\n"+serv_name);
		QString filepath = QFileDialog::getOpenFileName(uploadwidget, tr("Choose file"), settings.value("main/lastdir").toString(), serv_filefilter);
		QFile file(filepath);
		fileinfo.setFile(file);
		QString boundary = "AaB03x";
		QString filename = fileinfo.fileName();
		if (filepath.isEmpty()) {
			uploadwidget->setStatus(tr("Canceled"));
			removeUploadWidget();
		}
		else if (fileinfo.size()==0) {
			uploadwidget->setStatus(tr("File size is null"));
		}
		else if (file.open(QIODevice::ReadOnly)) {
			settings.setValue("main/lastdir", fileinfo.dir().path());
			QByteArray mpData;

			if (serv_postdata.length()>0) {
				foreach (QString poststr, serv_postdata.split("&")) {
					QStringList postpair = poststr.split("=");
					mpData.append("--" + boundary + "\r\n");
					mpData.append("Content-Disposition: form-data; name=\"" + postpair[0] + "\"\r\n");
					mpData.append("\r\n" + postpair[1] + "\r\n");
				}
			}

			mpData.append("--" + boundary + "\r\n");
			mpData.append("Content-Disposition: form-data; name=\"" + serv_fileinput + "\"; filename=\"" + filename.toUtf8() + "\"\r\n");
			mpData.append("Content-Transfer-Encoding: binary\r\n");
			mpData.append("\r\n");
			mpData.append(file.readAll());
			mpData.append("\r\n--" + boundary + "--\r\n");
			file.close();

			netreq.setRawHeader("User-Agent", "qutIM ImagePub plugin");
			netreq.setRawHeader("Content-Type", "multipart/form-data, boundary=" + boundary.toLatin1());
			netreq.setRawHeader("Cache-Control", "no-cache");
			netreq.setRawHeader("Accept", "*/*");
			netreq.setRawHeader("Content-Length", QString::number(mpData.length()).toLatin1());

			uploadwidget->setFilename(filename);
			uploadwidget->timeStart();

			QNetworkReply* netrp = netman->post(netreq, mpData);
			connect(netrp, SIGNAL(uploadProgress(qint64, qint64)), uploadwidget, SLOT(progress(qint64, qint64)));
		}
	}
}

void imagepubPlugin::actionError(QString error) {
	uploadwidget->setStatus(error);
}

void imagepubPlugin::netrpFinished( QNetworkReply* reply )
{
        uploadwidget->setStatus(tr("Sending file URL..."));
	if (reply->rawHeader("Location").length()>0) {
		QUrl netrequrl(reply->rawHeader("Location"));
		if (netrequrl.host().isEmpty()) netrequrl = QUrl("http://"+reply->url().encodedHost()+reply->rawHeader("Location"));
		QNetworkRequest netreq(netrequrl);
		netman->get(netreq);
	}
	else {
		page = reply->readAll();
		QRegExp rx(serv_regexp);
		if (rx.indexIn(page)>-1) {
			QString imageurl = rx.cap(1);
			QString msgtemplate;
			QSettings settings(QSettings::IniFormat, QSettings::UserScope, "qutim/qutim."+imagepub_profile_name, "plugin_imagepub");
			if (settings.value("main/msgtemplate").isValid()) msgtemplate = settings.value("main/msgtemplate").toString();
                        else msgtemplate = tr("File sent: %N (%S bytes)\n%U");
			QString sendmsg = msgtemplate;
			sendmsg.replace("%N", fileinfo.fileName());
			sendmsg.replace("%U", imageurl);
			sendmsg.replace("%S", QString::number(fileinfo.size()));
                        uploadwidget->setStatus(tr("File URL sent"));
			m_plugin_system->sendCustomMessage(event_item, sendmsg);
		}
		else uploadwidget->setStatus(tr("Can't parse URL"));
		removeUploadWidget();
	}
}

void imagepubPlugin::removeUploadWidget()
{
    netman->disconnect();
    netman=0; delete netman;
    uploadwidget->close();
}


Q_EXPORT_PLUGIN2(imagepubPlugin,imagepubPlugin);

