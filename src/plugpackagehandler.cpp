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

#include "plugpackagehandler.h"

plugPackageHandler::plugPackageHandler(plugPackageModel* plug_package_model, QProgressBar *progress_bar, QObject* parent)
        : m_plug_package_model(0)
{
    setParent(parent);
    m_plug_package_model = plug_package_model;
    m_progress_bar = progress_bar;
    default_attribute = installed;
    QSettings settings(QSettings::defaultFormat(), QSettings::UserScope, "qutim/plugman/cache/sources", "plugman");
    QFileInfo config_dir = settings.fileName();
    QDir current_dir = qApp->applicationDirPath();
    if ( config_dir.canonicalPath().contains( current_dir.canonicalPath() ) )
        cachePath = current_dir.relativeFilePath( config_dir.absolutePath() );
    else
        cachePath = config_dir.absolutePath();
    cachePath.append("/");
}

plugPackageHandler::~plugPackageHandler() {

}

void plugPackageHandler::getPackageList() {
    default_attribute = installed;
    updatePlugPackageModel();
    updatePackagesCache();
    return;
}



void plugPackageHandler::setPlugPackageModel(plugPackageModel* plug_package_model) {
    if (m_plug_package_model!=0) {
        delete(m_plug_package_model);
        m_plug_package_model = 0;
    }
    m_plug_package_model = plug_package_model;
}

void plugPackageHandler::updatePlugPackageModel(const QString& filename) {
    plugXMLHandler plug_xml_handler;
    QHash<QString,packageInfo> packages_list = plug_xml_handler.getPackageList(filename);
    foreach (packageInfo package_info, packages_list) {
		if (!package_info.isValid())
			continue;
        ItemData *item = new ItemData(	buddy,
                                       QIcon(":/icons/hi64-action-package.png"),
                                       package_info,
                                       default_attribute
                                     );
        m_plug_package_model->addItem(item);
    }
    return;
}

void plugPackageHandler::updatePlugPackageModel(const QStringList& fileList)
{
    foreach (QString filePath,fileList)
		updatePlugPackageModel(filePath);
    this->deleteLater();
    m_progress_bar->setVisible(false);
    return;
}


void plugPackageHandler::readMirrorList()
{
    mirror_list.clear();
    mirror_list.append(mirrorInfo( "Testing",
                                   QUrl ("http://sauron.savel.pp.ru/files/packages.xml"),
                                   ""
                                 ));
}

void plugPackageHandler::updatePackagesCache()
{
    readMirrorList();
    default_attribute = isInstallable;
    plugDownloader *loader = new plugDownloader(cachePath,this);
    loader->setProgressbar(m_progress_bar);
    connect(loader,SIGNAL(downloadFinished(QStringList)),this,SLOT(updatePlugPackageModel(QStringList)));
    foreach (mirrorInfo mirror_info, mirror_list) {
        if (!mirror_info.isValid())
            continue;
        loader->addItem(downloaderItem(	mirror_info.url,
                                        mirror_info.name + ".xml"
                                      ));
    }
    loader->startDownload();
}
