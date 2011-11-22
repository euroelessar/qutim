/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright (C) 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
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

#include "plugpackagehandler.h"
#include <k8json/k8json.h>
#include <qutim/plugininterface.h>
#include "hacks/plughacks.h"

#include <qutim/iconmanagerinterface.h>

plugPackageHandler::plugPackageHandler(plugPackageModel* plug_package_model, QObject* parent)
        : m_plug_package_model(0)
{
    setParent(parent);
    m_plug_package_model = plug_package_model;
    default_attribute = installed;
    cachePath = plugPathes::getCachePath()+"sources/";
}

plugPackageHandler::plugPackageHandler()
        : m_plug_package_model(0)
{
    cachePath = plugPathes::getCachePath()+"sources/";
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
    plugDownloader *loader = new plugDownloader ();
    foreach (packageInfo package_info, packages_list) {
        if (!package_info.isValid())
            continue;

        ItemData *item = new ItemData(	buddy,
                                       qutim_sdk_0_2::SystemsCity::IconManager()->getIcon("package"),
                                       package_info,
                                       default_attribute
                                     );
        m_plug_package_model->addItem(item);
    }
    return;
}

void plugPackageHandler::updatePlugPackageModel(const QList<downloaderItem> &items)
{
    foreach (downloaderItem item,items) {
        updatePlugPackageModel(item.filename);
    }
    this->deleteLater();
    return;
}

QList<mirrorInfo> plugPackageHandler::getMirrorList()
{
    readMirrorList();
    return mirror_list;
}

void plugPackageHandler::readMirrorList()
{
    mirror_list.clear();
    // Hardcoded one
    mirrorInfo info = { "Testing", QUrl("http://sauron.savel.pp.ru/files/packages.xml"), "Testing repo" };
    mirror_list << info;
    // Try to guess other mirrors
    QDir plugman_dir = plugPathes::getConfigDir();
    QFileInfo mirror;
    if (plugman_dir.exists("mirrors.json"))
        mirror = plugman_dir.filePath("mirrors.json");
    else
    {
        QStringList paths = qutim_sdk_0_2::SystemsCity::PluginSystem()->getSharePaths();
        foreach(const QString &path, paths)
        {
            QDir dir = path;
            if (!dir.cd("plugman") || !dir.exists("mirrors.json"))
                continue;
            mirror = dir.filePath("mirrors.json");
            if (!mirror.isReadable())
                mirror = QFileInfo();
            else
                break;
        }
    }
    // File with mirrors is found, read it
    if (mirror.isFile() && mirror.isReadable())
    {
        qDebug() << "Found: " << mirror.absoluteFilePath();
        QFile file(mirror.absoluteFilePath());
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
            return;
        int len = file.size();
        QByteArray array;
        const uchar *fmap = file.map(0, file.size());
        if (!fmap)
        {
            array = file.readAll();
            fmap = (uchar *)array.constData();
        }
        const uchar *s = K8JSON::skipBlanks(fmap, &len);
        QVariant val;
        uchar qch = *s;
        if (!s || (qch != '[' && qch != '{'))
            return;
        qch = (qch == '{' ? '}' : ']');
        s++;
        len--;
        bool first = true;
        while (s)
        {
            s = K8JSON::skipBlanks(s, &len);
            if (len < 2 || (s && *s == qch))
                break;
            if (!s)
                break;
            if ((!first && *s != ',') || (first && *s == ',')) {
                //					fprintf(stderr, "ERROR: invalid JSON file (delimiter)!\n");
                break;
            }
            first = false;
            if (*s == ',')
            {
                s++;
                len--;
            }
            val.clear();
            s = K8JSON::parseRec(val, s, &len);
            if (!s) {
                //					fprintf(stderr, "ERROR: invalid JSON file!\n");
                break;
            }
            QVariantMap mirror = val.toMap();
            mirrorInfo info = { mirror.value("name").toString(),
                                mirror.value("url").toString(),
                                mirror.value("description").toString()
                              };
            qDebug() << info.name << info.url << info.description;
            mirror_list << info;
        }
    }
}

void plugPackageHandler::updatePackagesCache()
{
    readMirrorList();
    default_attribute = isInstallable;
    plugDownloader *loader = new plugDownloader(cachePath,this);
    connect(loader,SIGNAL(downloadFinished(QList<downloaderItem>)),SLOT(updatePlugPackageModel(QList<downloaderItem>)));
    foreach (mirrorInfo mirror_info, mirror_list) {
        if (!mirror_info.isValid())
            continue;
//          downloaderItem item = {mirror_info.url,
//                                 mirror_info.name + (mirror_info.url.path().endsWith(".xml") ? ".xml" : ".json")};
        downloaderItem item;
        item.url = mirror_info.url;
        item.filename = mirror_info.name + (mirror_info.url.path().endsWith(".xml") ? ".xml" : ".json");
        //TODO спросить у Элессара как такое красиво записывать
        loader->addItem(item);
    }
    loader->startDownload();
}

