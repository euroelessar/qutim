/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2013 Tretyakov Roman <roman@trett.ru>
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

#include "handler.h"
#include <qutim/chatsession.h>
#include <QTextDocument>
#include <QTextDocumentFragment>
#include <QHttpMultiPart>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <qutim/config.h>
#include <QTimer>

using namespace qutim_sdk_0_3;

Handler::Handler(QWidget *parent):QDialog(parent),
	ui(new Ui::Handler),m_link("")
{
	ui->setupUi(this);
	setWindowFlags (windowFlags() & ~Qt::WindowContextHelpButtonHint);
	ui->locationBox->addItem("paste.ubuntu.com","http://paste.ubuntu.com");
	ui->locationBox->addItem("hastebin.com","http://hastebin.com/documents");
	ui->languageBox->addItem("Plain Text","text");
	ui->languageBox->addItem("C++","cpp");
	ui->languageBox->addItem("Bash","bash");
	ui->languageBox->addItem("Perl","perl");
	ui->languageBox->addItem("PHP","php");
	ui->languageBox->addItem("C#","csharp");
	ui->languageBox->addItem("HTML","html");
	ui->languageBox->addItem("JavaScript","js");
	ui->languageBox->addItem("Java","java");
	ui->languageBox->addItem("Makefile","make");
	ui->languageBox->addItem("XML","xml");
	ui->languageBox->addItem("CSS","css");
	m_manager = new QNetworkAccessManager(this);
	readSettings();
	QObject::connect(m_manager,SIGNAL(finished(QNetworkReply*)),this,SLOT(finishedSlot(QNetworkReply*)));
}

Handler::Result Handler::doHandle(Message &message, QString *reason)
{
	Q_UNUSED(reason)
	if (!message.isIncoming() && !message.property("service", false) &&
			!message.property("history", false) && message.text().count('\n') >= m_lineCount-1) {
		m_message = message.text();
		if (m_autoSubmit) QTimer::singleShot(m_delay,this,SLOT(accept()));
		switch (exec()) {
		case QDialog::Accepted:
			sendMessage(message);
			break;
		case QDialog::Rejected:
			return Accept;
		}
	}
	return Accept;
}

void Handler::accept()
{
	QString host = (ui->locationBox->itemData(ui->locationBox->currentIndex(),Qt::UserRole)).toString();
	int switchHost = ui->locationBox->currentIndex();
	QByteArray syntax = (ui->languageBox->itemData(ui->languageBox->currentIndex(),Qt::UserRole)).toByteArray();
	QByteArray content = m_message.toAscii();
	QHttpMultiPart *multi = new QHttpMultiPart(QHttpMultiPart::FormDataType);
	QByteArray hastebinBody;
	switch (switchHost) {
	case 0:
		append_part(multi,"poster","qutim");
		append_part(multi,"syntax",syntax);
		append_part(multi,"content",content);
		break;
	case 1:
		hastebinBody.append(content);
		break;
	}
	QUrl url(host);
	QNetworkRequest request;
	request.setUrl(url);
	if (host == "http://paste.ubuntu.com") {
		QNetworkReply *r = m_manager->post(request, multi);
		multi->setParent(r);
	} else {
		request.setHeader(QNetworkRequest::ContentTypeHeader,"application/x-www-form-urlencoded");
		m_manager->post(request, hastebinBody);
	}
}

void Handler::finishedSlot(QNetworkReply *reply)
{
	reply->deleteLater();
	if (reply->error() == QNetworkReply::NoError) {
		if(ui->locationBox->currentIndex() == 0){
			QVariant answer = reply->header(QNetworkRequest::LocationHeader);
			m_link = answer.toString();
		} else {
			QByteArray bytes = reply->readAll();
			QString string(bytes);
			string.replace(QRegExp("\\{\"key\":\"([a-z0-9]+)\"\\}"),"\\1");
			//  qDebug()<<string;
			m_link = "http://hastebin.com/" + string;
		}
	} else {
		m_link = reply->errorString();
	}
	done(QDialog::Accepted);
}

void Handler::append_part(QHttpMultiPart *multi, const QByteArray &name, const QByteArray &value)
{
	QHttpPart part;
	part.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"" + name + "\""));
	part.setBody(value);
	multi->append(part);
}

void Handler::readSettings()
{
	Config cfg;
	cfg.beginGroup("AutoPaster");
	m_autoSubmit = cfg.value(QLatin1String("AutoSubmit")).toBool();
	m_defaultLocation = cfg.value(QLatin1String("DefaultLocation")).toInt();
	m_lineCount = cfg.value(QLatin1String("LineCount")).toInt();
	m_delay = cfg.value(QLatin1String("Delay")).toInt();
	cfg.endGroup();
	ui->locationBox->setCurrentIndex(m_defaultLocation);
}

Handler::~Handler()
{
	delete ui;
}

void Handler::sendMessage(qutim_sdk_0_3::Message &message)
{
	QRegExp rx("^http://.+");
	if (rx.exactMatch(m_link) == true) {
		message.setText(m_link);
	} else {
		message.setProperty("service", true);
		message.setText(tr("Failed to send message to paste service, service reported error: %1").arg(m_link));
	}
}

