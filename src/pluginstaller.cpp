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
#include "plugparser.h"

plugInstaller::plugInstaller() {

    QSettings settings(QSettings::defaultFormat(), QSettings::UserScope, "qutim", "plugman");
    outPath = settings.fileName().section("/",0,-2);
    qDebug() << outPath;
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
    return true;
}

bool plugInstaller::installFromXML(QString& inPath) {

    qDebug()<<"Not yet implemented";
    plugParser plug_parser;
    plugDownloader *plug_loader = new plugDownloader;
    QHash<QString, QString> packInfo = plug_parser.parseItem(inPath);
    plugDownloader::downloaderItem item;
    item.url = packInfo["url"];
    item.filename = packInfo["name"];
    connect(plug_loader,SIGNAL(downloadFinished(QString)),this,SLOT(unpackArch(QString)));
    plug_loader->startDownload(item);
}


bool plugInstaller::registerPackage(QHash< QString, QString >, QStringList &files) {
    //пока ещё не решил, как реализовывать, надеюсь кутишного ini хватит

}

bool plugInstaller::registerPackage(QString name, QStringList &files) {
    //
	qDebug() << name;
}

bool plugInstaller::removePackage(QString name, QStringList &files) {
}

