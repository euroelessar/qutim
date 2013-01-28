/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Alexander Kazarin <boiler@co.ru>
** Copyright © 2011 Aleksey Sidorov <gorthauer87@yandex.ru>
** Copyright © 2012 Nicolay Izoderov <nico-izo@ya.ru>
**
*****************************************************************************
**
** $QUTIM_BEGIN_LICENSE$
** This program is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation,   either version 3 of the License,   or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
** See the GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program.  If not,   see http://www.gnu.org/licenses/.
** $QUTIM_END_LICENSE$
**
****************************************************************************/

#include "shoter.h"
#include <QFileDialog>
#include <QPixmap>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QHttpMultiPart>
#include <qutim/config.h>
#if defined Q_OS_WIN
#include <windows.h>
#elif defined Q_OS_LINUX
#include <X11/Xlib.h>
#endif
Shoter::Shoter(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::Screenshoter)
{
	ui->setupUi(this);
	QObject::connect(ui->btnCancel,  SIGNAL(clicked()),  this,  SLOT(onButtonCancelClicked()));
	QObject::connect(ui->btnShot,  SIGNAL(clicked()),  this,  SLOT(onShotButtonClicked()));
	QObject::connect(ui->btnSave,  SIGNAL(clicked()),  this,  SLOT(onPushSaveClicked()));
	QObject::connect(ui->btnSend,  SIGNAL(clicked()),  this, SLOT(onButtonSendClicked()));
	ui->statusBar->addWidget(&label);
	label.setText(" Click \"Send\" to get the link!");
	ui->statusBar->addWidget(&progressBar);
	progressBar.hide();
	ui->comboBox->addItem("pix.academ.org", 1);
	ui->comboBox->addItem("ompldr.org", 2);
	ui->comboBox_2->addItem("AllDesktop", 1);
	ui->comboBox_2->addItem("ActiveWindow", 2);
	ui->btnCancel->setShortcut(Qt::CTRL + Qt::Key_Q);
	ui->btnCancel->setToolTip("Ctrl+Q");
	ui->btnSend->setShortcut(Qt::CTRL + Qt::Key_P);
	ui->btnSend->setToolTip("Ctrl+P");
	ui->btnSave->setShortcut(Qt::CTRL + Qt::Key_S);
	ui->btnSave->setToolTip("Ctrl+S");
	ui->btnShot->setShortcut(Qt::CTRL + Qt::Key_R);
	ui->btnShot->setToolTip("Ctrl+R");
	readSettings();
}

Shoter::~Shoter()
{
	writeSettings();
	delete ui;
}

void Shoter::onButtonSendClicked()
{
	/*Create file */
	QDate date = QDate::currentDate();
	QString name_file = date.toString();
	QString savedFile = name_file+".png";
	QTemporaryFile file;
	file.setFileName(savedFile);
	if (!m_screenshot.save(&file)) {
		qDebug("file is not saved");
		exit(1);
	}
	// qDebug()<<file.fileName();
	QFile *openFile = new QFile(savedFile);
	openFile->open(QIODevice::ReadOnly);
	QHttpMultiPart *multi = new QHttpMultiPart(QHttpMultiPart::FormDataType);
	/* Upload to pix.academ*/
	if (ui->comboBox->currentIndex()== 0) {
		QHttpPart action;
		action.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("name=\"action\""));
		action.setBody("upload_image");
		QHttpPart image;
		image.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("image/png"));
		image.setHeader(QNetworkRequest::ContentDispositionHeader,
						QVariant("form-data; name=\"image\"; filename=\""+savedFile+"\""));
		image.setBodyDevice(openFile);
		openFile->setParent(multi);
		multi->append(action);
		multi->append(image);
		upload("http://pix.academ.org", multi);
		/* Upload to ompldr.org*/
	} else if (ui->comboBox->currentIndex() == 1) {
		QHttpPart file1;
		file1.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("image/png"));
		file1.setHeader(QNetworkRequest::ContentDispositionHeader,
						QVariant("form-data; name=\"file1\"; filename=\""+savedFile+"\""));
		file1.setBodyDevice(openFile);
		openFile->setParent(multi);
		multi->append(file1);
		upload("http://ompldr.org/upload", multi);
	}
}
void Shoter::onButtonCancelClicked()
{
	writeSettings();
	close();
}
void Shoter::finishedSlot(QNetworkReply *reply)
{
	if (reply->error() == QNetworkReply::NoError) {
		QByteArray bytes = reply->readAll();
		QString string(bytes);
		QStringList list;
		//		qDebug()<<string;
		QRegExp rx("http://pix.academ.org/img[^\"]+|http://ompldr.org/[^<]+");
		int pos = 0;
		while ((pos = rx.indexIn(string, pos)) != -1){
			list << rx.cap(0);
			pos += rx.matchedLength();
		}
		QString text;
		if (list.isEmpty() != true) {
			text = list.at(0);
			label.setText("URL: <a href=\""+text+"\">"+text+"</a>");
			label.setOpenExternalLinks(true);
		} else {
			label.setText("<FONT COLOR=red>&nbsp; Service unavailable!</FONT>");
		}
	} else {
		label.setText(reply->errorString());
	}
}
void Shoter::upProgress(qint64 recieved,  qint64 total)
{
	if (total > 0 && recieved > 0) {
		label.setText("");
		progressBar.show();
		progressBar.setMinimum(0);
		progressBar.setMaximum(100);
		progressBar.setValue((int) recieved*100/total);
		if (progressBar.value() == 100) {
			progressBar.hide();
		}
	}
}

void Shoter::onPushSaveClicked()
{
	QString file_on_disk = QFileDialog::getSaveFileName(this, tr("Save File"),
														tr("untitled.png"), tr("Images(*.png *.xpm *.jpg)"));
	m_screenshot.save(file_on_disk);
}

void Shoter::onShotButtonClicked()
{
	int sec = ui->spinBox->value();
	setHidden(true);
	QTimer *timer = new QTimer(this);
	timer->singleShot(sec*1000, this, SLOT(reShot()));
	timer->start();
}

void Shoter::reShot()
{
	ui->label_2->clear();
	if (ui->comboBox_2->currentIndex() == 0) {
		shot(QApplication::desktop()->winId());
	} else if (ui->comboBox_2->currentIndex() == 1) {
#if defined Q_OS_WIN
		HWND winf = GetForegroundWindow();
		shot(winf);
#elif defined Q_OS_LINUX
		Window winf;
		Display *dpy;
		dpy = XOpenDisplay(0);
		int ret;
		XGetInputFocus(dpy, &winf, &ret);
		shot(winf);
#endif
	}
	setVisible(true);
	setFocus();
	showNormal();
}
void Shoter::resizeEvent(QResizeEvent* /* event */)
{
	QSize scaledSize = m_screenshot.size();
	scaledSize.scale(ui->label_2->size(), Qt::KeepAspectRatio);
	if (!ui->label_2->pixmap() || scaledSize != ui->label_2->pixmap()->size()) {
		setScreenShot();
	}
}
void Shoter::setScreenShot()
{
	ui->label_2->setPixmap(m_screenshot.scaled(ui->label_2->size(),
											   Qt::KeepAspectRatio, Qt::SmoothTransformation));
}
void Shoter::readSettings()
{
	qutim_sdk_0_3::Config p_settings;
	p_settings.beginGroup(QLatin1String("Screenshoter"));
	int host = p_settings.value(QLatin1String("default")).toInt();
	ui->comboBox->setCurrentIndex(host);
	p_settings.endGroup();
}
void Shoter::writeSettings()
{
	qutim_sdk_0_3::Config p_settings;
	p_settings.beginGroup(QLatin1String("Screenshoter"));
	p_settings.setValue(QLatin1String("default"), QString::number(ui->comboBox->currentIndex()));
	p_settings.endGroup();
}

void Shoter::upload(const QString &hostUrl,  QHttpMultiPart *multipart)
{
	QNetworkAccessManager *netMgr = new QNetworkAccessManager(this);
	QObject::connect(netMgr,  SIGNAL(finished(QNetworkReply*)),  this,  SLOT(finishedSlot(QNetworkReply*)));
	QUrl url(hostUrl);
	QNetworkRequest request(url);
	QNetworkReply *r = netMgr->post(request,  multipart);
	multipart->setParent(r);
	QObject::connect(r,  SIGNAL(uploadProgress(qint64,  qint64)),  this,  SLOT(upProgress(qint64,  qint64)));
}

void Shoter::shot(WId pwid)
{
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
	m_screenshot = QPixmap::grabWindow(pwid);
#elif QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)  
	screen = QGuiApplication::primaryScreen();
	m_screenshot = screen->grabWindow(pwid);
#else
#error Unsupported OS
#endif 
	setScreenShot();
}

void Shoter::startShoter()
{
	shot(QApplication::desktop()->winId());
	show();
}
