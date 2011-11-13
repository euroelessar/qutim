/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright (C) 2011 Ruslan Nigmatullin euroelessar@yandex.ru
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

#include "pluginstaller.h"
#include <QDebug>
#include <QSettings>
#include "collisionprotect.h"
#include <QDir>
#include <QFileDialog>
#include "utils/plugversion.h"
#include <qutim/plugininterface.h>
#include "hacks/plughacks.h"

using namespace qutim_sdk_0_2;

plugInstaller::plugInstaller()
{
    //TODO переделать после того как Элессар сделает настоящую систему настроек
    QString outPath = plugPathes::getConfigPath();

    QSettings settings(QSettings::defaultFormat(), QSettings::UserScope, "qutim/plugman", "plugman");
    settings.beginGroup("features");
    collision_protect = settings.value("collisionprotect",true).toBool();
    needUpdate = false;
#ifdef Q_OS_WIN
    backup = true; //Винда по другому не может обновлять плагины
#else
    backup = settings.value("backup", false).toBool();
#endif
    settings.endGroup();
    settings.beginGroup("prefix");
	QString appDir = qApp->applicationDirPath() +"/";
    QString libDir = appDir;
    package_prefix.insert("art",settings.value("art",outPath).toString());
    package_prefix.insert("core",settings.value("core",appDir).toString());
    package_prefix.insert("lib",settings.value("lib",libDir).toString());
    package_prefix.insert("plugin",settings.value("plugin",outPath).toString());
    package_prefix.insert("backup",settings.value("backup",outPath + "plugman/backup/").toString());
    settings.endGroup();

    connect (this,SIGNAL(error(QString)),this,SLOT(errorHandler(QString))); //в случае ошибки вызывается этот класс
}

plugInstaller::~plugInstaller() {
    QSettings settings(QSettings::defaultFormat(), QSettings::UserScope, "qutim/plugman", "plugman");
    if (needUpdate) {
        SystemsCity::PluginSystem()->systemNotification(TreeModelItem(),tr("Need restart!"));
        settings.setValue("needUpdate",true);
#ifdef Q_OS_WIN
        settings.setValue("locked",true); //костыль для винды
#endif
    }
}

QStringList plugInstaller::getFileList(const QString& inPath)
{
	QZipReader reader(inPath,QIODevice::ReadOnly);
	if (!reader.isReadable()) {
		emit error(tr("Unable to open archive: %1").arg(inPath));
		return QStringList();
	}
	QList< QZipReader::FileInfo > list = reader.fileInfoList();
	reader.close();
    QStringList fileList;
    foreach (QZipReader::FileInfo info, list) {
        fileList.append(info.filePath);
    }
    return fileList;
}


QStringList plugInstaller::unpackArch(const QString& inPath, const QString& prefix) {
	QZipReader reader(inPath,QIODevice::ReadOnly);
	QStringList packFiles = getFileList(inPath);
    if (packFiles.isEmpty()) {
		return QStringList();
	}
	if (collision_protect) {
		CollisionProtect protect(prefix);
		if (!protect.checkPackageFiles(packFiles)) {
			emit error(tr("warning: trying to overwrite existing files!"));
			return QStringList();
		}
	}
	qDebug() << "unpack arch to" << prefix << inPath;
    if (!reader.extractAll(prefix)) {
        emit error (tr ("Unable to extract archive: %1 to %2").arg(inPath, prefix));
        return QStringList();
    }
    reader.close();
    //FIXME для совместимости со старыми пакетами
    packFiles.removeOne("Pinfo.xml");
    QFile::remove(prefix+"/Pinfo.xml");
    emit updateProgressBar(75,100,tr("Installing:"));
    return packFiles;
}

QString plugInstaller::getPackagePrefix(const packageInfo* package_info)
{
    QString category = package_info->properties.value("category").isEmpty() ? "art" : package_info->properties.value("category");
	qDebug() << "Category for:" << package_info->properties.value("name") << category;
	return package_prefix.value(category);
}

void plugInstaller::commit()
{
	connect(this,SIGNAL(finished()),SLOT(deleteLater()));
	remove();
	if (installPackagesList.isEmpty()) {
		emit finished();
		return;
	}
	install();
}


void plugInstaller::install()
{
    plugDownloader *plug_loader = new plugDownloader();
    plug_loader->setParent(this);
    connect(plug_loader,SIGNAL(updateProgressBar(uint,uint,QString)),SIGNAL(updateProgressBar(uint,uint,QString)));
    foreach (packageInfo *package_info, installPackagesList) {
        if (!package_info->isValid()) {
            emit error(tr("Invalid package: %1").arg(package_info->properties.value("name")));
            continue;
        }
        downloaderItem item = { package_info->properties["url"],
                                package_info->properties["name"]
                                +"-"+package_info->properties["version"]
                                +".zip",
                                package_info
                              };
        plug_loader->addItem(item);
    }
    connect(plug_loader,SIGNAL(downloadFinished(QList<downloaderItem>)),SLOT(install(QList<downloaderItem>)));
    plug_loader->startDownload();
}


void plugInstaller::remove()
{
    foreach (QString name, removePackagesList) {
        plugXMLHandler plug_handler;
        connect(&plug_handler,SIGNAL(error(QString)),SLOT(errorHandler(QString)));
        packageInfo package_info = plug_handler.getPackageInfoFromDB(name);
        QString category = package_info.properties.value("category").isEmpty() ? "art" : package_info.properties.value("category");
        QStringList fileList = plug_handler.removePackage(name);
        QString path = package_prefix.value(category);
        bool isArt = package_info.properties.value("category")=="art";
        qDebug() << backup << isArt;
        if (backup&&!isArt) {
            QString backup_path = package_prefix.value("backup");
            //needUpdate = true;
            QDir dir;
            dir.mkpath(backup_path);
            for (uint i = fileList.count();i>0;i--) {
                QFile output (path+fileList.at(i-1));
				qDebug() << "File removed:" << path+fileList.at(i-1);
                QFile::remove(backup_path+fileList.at(i-1));
                output.rename(backup_path+fileList.at(i-1));
                int value = qRound((fileList.count()-i)/fileList.count()*100);
                emit updateProgressBar(value,100,tr("Removing:"));
            }
        }
        else {
            for (uint i = fileList.count();i>0;i--) {
                QFile output (path+fileList.at(i-1));
                qDebug() << "File removed:"  << path+fileList.at(i-1);
                output.remove();
                int value = qRound((fileList.count()-i)/fileList.count()*100);
                emit updateProgressBar(value,100,tr("Removing:"));
            }
        }
    }
}


void plugInstaller::installPackage(packageInfo* info)
{
    installPackagesList.append(info);
}

void plugInstaller::install(const QList< downloaderItem >& items)
{
    foreach (downloaderItem item,items) {
        if (!item.info->isValid()) {
            emit error(item.info->ErrorString);
            return;
        }
		QString prefix = getPackagePrefix(item.info);
        plugXMLHandler plug_handler;
        connect(&plug_handler,SIGNAL(error(QString)),SLOT(errorHandler(QString)));
        packageInfo installed_package_info = plug_handler.getPackageInfoFromDB(item.info->properties.value("name"),item.info->properties.value("type"));
        if (installed_package_info.isValid()) {//если он valid, значит он существует ^_^
            if (plugVersion(installed_package_info.properties.value("version"))<plugVersion(item.info->properties.value("version"))) {
				const QStringList &replaceItemFiles = getFileList(item.filename);
				const QStringList &originalItemFiles = installed_package_info.files;
				if (collision_protect) {
					CollisionProtect protect(prefix);
					QStringList result;
					for(QStringList::const_iterator it = replaceItemFiles.begin(); it != replaceItemFiles.end(); it++)
					{
						bool exist = false;
						for(QStringList::const_iterator jt = originalItemFiles.begin(); jt != originalItemFiles.end(); jt++)
						{
							if(*it == *jt)
							{
								exist = true;
								break;
							}
						}
						if(!exist)
							result << *it;
					}
					if (!protect.checkPackageFiles(result)) {
						emit error(tr("warning: trying to overwrite existing files!"));
						continue;
					}
				}
				removePackagesList.append(item.info->properties.value("name"));
				remove();
			}
            else {
                emit error(tr("Unable to update package %1: installed version is later").arg(item.info->properties.value("name")));
                return;
            }
        }
        emit updateProgressBar(50,100,tr("Installing:"));
        packageInfo package; //FIXME надо бы покрасивее, то бишь скопировать сюда инфу из item.info
        package.properties = item.info->properties;
        package.dependencyList = item.info->dependencyList;
        package.files = unpackArch(item.filename,prefix);
        if (package.files.isEmpty()) {
            emit error(tr("Unable to install package: %1").arg(item.info->properties.value("name")));
            continue;
        }
		qDebug() << package.files;
        emit updateProgressBar(100,100,tr("Installing:"));
        plug_handler.registerPackage(package);
        QString category = item.info->properties.value("category").isEmpty() ? "art" : item.info->properties.value("category");
        if (category!="art")
            needUpdate = true;
    }
	emit finished();
}


void plugInstaller::removePackage(const QString& name)
{
    removePackagesList.append(name);
}


void plugInstaller::errorHandler(const QString& error) {
    qDebug() << "Error:" << error;

    SystemsCity::PluginSystem()->systemNotification(TreeModelItem(),error);

    emit finished();
}

