/*
This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Library General Public
License version 2 as published by the Free Software Foundation.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Library General Public License for more details.

You should have received a copy of the GNU Library General Public License
along with this library; see the file COPYING.LIB.  If not, write to
the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
Boston, MA 02110-1301, USA.
*/

#include "pluginstaller.h"
#include <QDebug>
#include <QSettings>
#include <osdabzip/unzip.h>
#include "plugdownloader.h"
#include "collisionprotect.h"
#include <QDir>
#include <QFileDialog>

plugInstaller::plugInstaller()
: package_info()
{
	
    QSettings settings(QSettings::defaultFormat(), QSettings::UserScope, "qutim", "plugman");
	settings.beginGroup("features");
	collision_protect = settings.value("collisionprotect",true).toBool();
	settings.endGroup();
    outPath = settings.fileName().section("/",0,-2);
    qDebug() << outPath;
// 	connect (this,SIGNAL(finished()),this,SLOT(deleteLater())); // в случае завершения установки обьект может быть удалён
	connect (this,SIGNAL(error(QString)),this,SLOT(errorHandler(QString))); //в случае ошибки вызывается этот класс
}

plugInstaller::~plugInstaller() {
	
}

QStringList plugInstaller::unpackArch(QString& inPath) {
//

    UnZip uz;
    UnZip::ErrorCode ec = uz.openArchive(inPath);
    if (ec != UnZip::Ok) {
        lastError = tr("Unable to open archive");
        return QStringList();
    }
    QStringList packFiles = uz.fileList();
	if (collision_protect) {
		CollisionProtect protect;
		if (!protect.checkPackageFiles(packFiles))
			return QStringList();
		}
	
    uz.extractAll(outPath);
    if (ec != UnZip::Ok) {
        lastError = tr ("Unable to extract archive");
        return QStringList();
    }
    uz.closeArchive(); // Close the zip file and free used resources
    qDebug() << "Unpack archive:" << outPath;
	m_progressBar->setValue(75);
    return packFiles;
}

void plugInstaller::installPackage() {
	QString path = QFileDialog::getOpenFileName(0,tr("Install package from file"),".",
												tr("Archives (*.zip);;XML files (*.xml)"));
	if (path.isEmpty())
		return;
	m_progressBar->setVisible(true);
	m_progressBar->setValue(0);
	if ((path.section(".",-1))=="zip") {
	    this->installFromFile(path);
	}
	else if ((path.section(".",-1))=="xml")
	{
	    this->installFromXML(path);
	}
}


void plugInstaller::installFromFile(QString& inPath) {
	//FIXME переписать на регэкспах
	QString name = inPath.section("/",-1).section(".",0,-2);
	qDebug() << name;
	if (collision_protect) {
		CollisionProtect protect;
		if (!protect.checkPackageName(name)) {
			emit error("Exist name");
			return;
		}
	}
	package_info.properties["name"] = name;
	package_info.properties["type"] = "other";
	install(inPath);
}

void plugInstaller::installFromXML(QString& inPath) {
	plugXMLHandler plug_handler;
    plugDownloader *plug_loader = new plugDownloader;
	plug_loader->setProgressbar(m_progressBar);
	package_info = plug_handler.getPackageInfo(inPath);
	if (!isValid(package_info)) {
		emit error("Invalid package");
		return;
	}
	if (collision_protect) {
		CollisionProtect protect;
		if (!protect.checkPackageName(package_info.properties["name"])) {
			emit error("Exist name");
			return;
		}
	}	
    connect(plug_loader,SIGNAL(downloadFinished(QString)),this,SLOT(install(QString)));
	if (!isValid(package_info));
    plug_loader->startDownload(plugDownloader::downloaderItem(package_info.properties["url"],
															  package_info.properties["name"]
															  +"-"+package_info.properties["version"]
															  +".zip")
															 ); //FIXME
}

void plugInstaller::install(QString inPath) {
	//FIXME
	m_progressBar->setValue(50);
	package_info.files = unpackArch(inPath);
	plugXMLHandler plug_handler;
	m_progressBar->setValue(100);	
	plug_handler.registerPackage(package_info);
	deleteLater();
}

bool plugInstaller::isValid(const packageInfo& package_info) {
	return package_info.properties["url"].section(".",-1)=="zip";
}


bool plugInstaller::removePackage(QString name, QStringList &files) {
	return true;
}

void plugInstaller::errorHandler(const QString& error) {
	qDebug() << error;
	deleteLater();
}
