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
#include "utils/plugversion.h"

plugInstaller::plugInstaller()
{

    QSettings settings(QSettings::defaultFormat(), QSettings::UserScope, "qutim", "plugman");
    settings.beginGroup("features");
    collision_protect = settings.value("collisionprotect",true).toBool();
    settings.endGroup();
    QFileInfo config_dir = settings.fileName();
    QDir current_dir = qApp->applicationDirPath();
    if( config_dir.canonicalPath().contains( current_dir.canonicalPath() ) )
        outPath = current_dir.relativeFilePath( config_dir.absolutePath() );
    else
        outPath = config_dir.absolutePath();
    outPath.append("/");
    //    outPath = settings.fileName().section("/",0,-2) + "/";
    //  connect (this,SIGNAL(finished()),this,SLOT(deleteLater())); // в случае завершения установки обьект может быть удалён
    connect (this,SIGNAL(error(QString)),this,SLOT(errorHandler(QString))); //в случае ошибки вызывается этот класс
}

plugInstaller::~plugInstaller() {

}

QStringList plugInstaller::unpackArch(const QString& inPath) {
    //
    //FIXME need SANDBOX!
    UnZip uz;
    UnZip::ErrorCode ec = uz.openArchive(inPath);
    if (ec != UnZip::Ok) {
        emit error(tr("Unable to open archive"));
        return QStringList();
    }
    QStringList packFiles = uz.fileList();
    if (collision_protect) {
        CollisionProtect protect(outPath);
        if (!protect.checkPackageFiles(packFiles))
            return QStringList();
    }

    uz.extractAll(outPath);
    if (ec != UnZip::Ok) {
        emit error (tr ("Unable to extract archive"));
        return QStringList();
    }
    uz.closeArchive(); // Close the zip file and free used resources
    //     qDebug() << "Unpack archive:" << outPath;
    //FIXME Костыль
    //TODO дописать в osdabzip возможность удалять файлы прямо из архива.
    packFiles.removeOne("Pinfo.xml");
    QFile::remove(outPath+"/Pinfo.xml");
    m_progressBar->setValue(75);
    return packFiles;
}

void plugInstaller::installPackage() {
    QString path = QFileDialog::getOpenFileName(0,tr("Install package from file"),".",
                                                tr("Archives (*.zip)"));
    if (path.isEmpty())
        return;
    m_progressBar->setValue(0);
    if ((path.section(".",-1))=="zip") {
        this->installFromFile(path);
    }
}


void plugInstaller::installFromFile(const QString& inPath) {
    install(inPath);
    deleteLater();
}

packageInfo plugInstaller::getPackageInfo(const QString& archPath) {
    UnZip uz;
    UnZip::ErrorCode ec = uz.openArchive(archPath);
    if (ec != UnZip::Ok) {
        emit error(tr("Unable to open archive"));
        return packageInfo();
    }
    QStringList packFiles = uz.fileList();
    if (!packFiles.contains("Pinfo.xml"))
        return packageInfo();
    QString tmp_path = outPath + "plugman/cache/"; //FIXME need SANDBOX!
    uz.extractFile("Pinfo.xml",tmp_path);
    if (ec != UnZip::Ok) {
        emit error (tr("Unable to extract archive"));
        return packageInfo();
    }
    plugXMLHandler handler;
    tmp_path.append("Pinfo.xml");
    packageInfo package_info = handler.getPackageInfo(tmp_path);
    QFile::remove(tmp_path);
    uz.closeArchive();
    qDebug () << package_info.properties.value("name");
    return package_info;
}

void plugInstaller::install(QString inPath) {
    //FIXME
    packageInfo package_info = getPackageInfo(inPath);
    if (!package_info.isValid())
        return;
    plugXMLHandler plug_handler;
    packageInfo installed_package_info = plug_handler.getPackageInfoFromDB(package_info.properties.value("name"),package_info.properties.value("type"));
    if (installed_package_info.isValid()) {//если он valid, значит он существует ^_^
        qDebug () << "found installed package : " << installed_package_info.properties.value("name");
        if (plugVersion(installed_package_info.properties.value("version"))<plugVersion(package_info.properties.value("version")))
            removePackage(package_info.properties.value("name"),package_info.properties.value("type"));
        else {
            emit error("Unable to update package: installed version is later");
            return;
        }


    }
    m_progressBar->setValue(50);
    package_info.files = unpackArch(inPath);
    if (package_info.files.isEmpty()) {
        emit error(tr("Unable to install package"));
        return;
    }
    m_progressBar->setValue(100);
    plug_handler.registerPackage(package_info);
    m_progressBar->setVisible(false);
}

void plugInstaller::install(QStringList fileList)
{
	foreach (QString filePath,fileList) 
        install(filePath);
    deleteLater();
}

void plugInstaller::installPackages(const QList<packageInfo> &packages_list) {
    plugDownloader *plug_loader = new plugDownloader();
    plug_loader->setParent(this);
    plug_loader->setProgressbar(m_progressBar);
    foreach (packageInfo package_info, packages_list) {
        if (!package_info.isValid()) {
            emit error("Invalid package");
            continue;
        }
        plug_loader->addItem(downloaderItem(package_info.properties["url"],
                                            package_info.properties["name"]
                                            +"-"+package_info.properties["version"]
                                            +".zip")
                             );
    }
    connect(plug_loader,SIGNAL(downloadFinished(QStringList)),this,SLOT(install(QStringList)));
    plug_loader->startDownload();
}

void plugInstaller::removePackage(const QString &name, const QString &type) {
    plugXMLHandler plug_handler;
    QStringList fileList = plug_handler.removePackage(name);
    for (uint i = fileList.count();i>0;i--) {
        QFile output (outPath+fileList.at(i-1));
        output.remove();
        m_progressBar->setValue(qRound((fileList.count()-i)/fileList.count()*100));
    }
}

void plugInstaller::errorHandler(const QString& error) {
    qDebug() << error;
    deleteLater();
}

void plugInstaller::setProgressBar(QProgressBar* progressBar)
{
	m_progressBar = progressBar;
	connect(m_progressBar,SIGNAL(destroyed(QObject*)),SLOT(deleteLater()));
}
