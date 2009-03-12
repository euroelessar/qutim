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
#include "plugxmlhandler.h"
plugInstaller::plugInstaller() {

    QSettings settings(QSettings::defaultFormat(), QSettings::UserScope, "qutim", "plugman");
    outPath = settings.fileName().section("/",0,-2);
		
    qDebug() << outPath;
	connect (this,SIGNAL(finished()),this,SLOT(deleteLater())); // в случае завершения установки обьект может быть удалён
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
    uz.extractAll(outPath);
    if (ec != UnZip::Ok) {
        lastError = tr ("Unable to extract archive");
        return QStringList();
    }
    uz.closeArchive(); // Close the zip file and free used resources
    qDebug() << "Unpack archive:" << outPath;
    return packFiles;
}

bool plugInstaller::installFromFile(QString& inPath) {
    QStringList files = unpackArch(inPath);
    registerPackage(inPath.section("/",0,-1),files);
	emit finished();
    return true;
}

void plugInstaller::installFromXML(QString& inPath) {
	plugXMLHandler plug_handler;
    plugDownloader *plug_loader = new plugDownloader;
	plugXMLHandler::packageInfo package_info = plug_handler.getPackageInfo(inPath);
    connect(plug_loader,SIGNAL(downloadFinished(QString)),this,SLOT(readytoInstall(QString)));
    plug_loader->startDownload(plugDownloader::downloaderItem(package_info.properties["url"],
															  package_info.properties["name"]
															  +"-"+package_info.properties["version"]
															  +".zip")
															 ); //FIXME
}

void plugInstaller::readytoInstall(QString inPath) {
	unpackArch(inPath);
	emit finished();
}


bool plugInstaller::registerPackage(QHash< QString, QString >, QStringList &files) {
    //пока ещё не решил, как реализовывать, надеюсь кутишного ini хватит
	return true;
}

bool plugInstaller::registerPackage(QString name, QStringList &files) {
    //
	qDebug() << "Package name : " << name;
	return true;
}

bool plugInstaller::removePackage(QString name, QStringList &files) {
	return true;
}

