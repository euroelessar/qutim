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

#include "plugxmlhandler.h"
#include <QDebug>
#include <QSettings>
#include <QFileInfo>
#include <QDir>
#include <QUrl>
#include <QFile>
#include <QApplication> //FIXME

plugXMLHandler::plugXMLHandler(QObject *parent)
    : QObject(parent)
{
    QSettings settings(QSettings::defaultFormat(), QSettings::UserScope, "qutim/plugman", "plugman");
    QFileInfo config_dir = settings.fileName();
    QDir current_dir = qApp->applicationDirPath();
    if( config_dir.canonicalPath().contains( current_dir.canonicalPath() ) )
        package_db_path = current_dir.relativeFilePath( config_dir.absolutePath() );
    else
        package_db_path = config_dir.absolutePath();
    package_db_path.append("/packages.xml");
    settings.beginGroup("global");
    globalCount = settings.value("count","0").toInt();
    settings.endGroup();
}

plugXMLHandler::~plugXMLHandler() {

}

QDomElement plugXMLHandler::createElementFromPackage(const packageInfo& package_info, int id) {
    QDomDocument doc;
    QDomElement package = doc.createElement("package");
    package.setAttribute("id",QString::number(id)); //надеюсь NULL она просто не добавит

    QDomElement package_element;
    QHash<QString,QString>::const_iterator it = package_info.properties.begin();
    for (it = package_info.properties.begin(); it!=package_info.properties.end();it++) {
        package_element = doc.createElement(it.key());
        package_element.appendChild(doc.createTextNode(it.value()));
        package.appendChild(package_element);
    }

    package_element = doc.createElement("files");
    package.appendChild(package_element);
    foreach (QString name, package_info.files) {
        QDomElement file_name = doc.createElement("name");
        file_name.appendChild(doc.createTextNode(name));
        package_element.appendChild(file_name);
    }
    return package;
}

packageInfo plugXMLHandler::createPackageInfoFromDom(const QDomDocument& doc, QString id) {
    // 	if (!QString::isEmpty(id))
    // 		QDomNode n = doc.elementById();
    // 	else
    // 		doc.firstChild();
    return packageInfo();
}

packageInfo plugXMLHandler::createPackageInfoFromNode(QDomNode n) {

    // 	QDomNode n = doc.documentElement().firstChild();
    // 	qDebug () << "start parser";
    packageInfo package_info;
    while (!n.isNull()) {
        QDomElement e = n.toElement(); // try to convert the node to an element.
        if (!e.isNull()) {
            if (e.tagName() == "files") {
                package_info.files = createFilesList(e.firstChild());
            }
            else {
                package_info.properties[e.tagName()] =e.text();
                //                                                qDebug() << e.tagName() << " : " << e.text();
            }
        }
        n = n.nextSibling();
    }
    return package_info;
}

QStringList plugXMLHandler::createFilesList(QDomNode n) {
    QStringList files;
    while (!n.isNull()) {
        QDomElement e = n.toElement();
        //  		qDebug() << e.tagName() << " : " << e.text();
        if (!e.isNull())
            files.append(e.text());
        n = n.nextSibling();
    }
    return files;
}


QHash< QString, packageInfo > plugXMLHandler::createPackageList(const QDomDocument& root) {

    QDomElement packages = root.documentElement();
    QHash< QString, packageInfo > packages_list;
    QDomNode n = packages.firstChild();
    while (!n.isNull()) {
        QDomElement e = n.toElement(); // try to convert the node to an element.
        if (!e.isNull()) {
            // 			qDebug() << e.tagName() << " : " << e.attribute("id");
            packageInfo package = createPackageInfoFromNode(n.firstChild());
            package.id = e.attribute("id").toInt();
            QString key = package.properties.value("type")+"/"+package.properties.value("name");
            //            if (package.isValid())
            //                qDebug() << QString("package %1 is valid").arg(package.properties.value("name"));
            packages_list.insert(key,package);
        }
        n = n.nextSibling();
    }
    return packages_list;
}

bool plugXMLHandler::registerPackage(const packageInfo &package_info) {
    QDomDocument doc;
    qDebug () << package_db_path;
    QFile input(package_db_path);
    if (input.exists()) {
        if (!input.open(QIODevice::ReadOnly)) {
            qDebug() << "Unable to open file";
            return false;
        }
        if (!doc.setContent(&input)) {
            qDebug() << "Unable to set content";
            return false;
        }
        input.close();
    }
    if (doc.documentElement().isNull()) {
        QDomElement packages = doc.createElement("packages");
        packages.setAttribute("version","0.1pre");
        doc.appendChild(packages);

    }
    doc.documentElement().appendChild(createElementFromPackage(package_info, globalCount));
    QFile output(QString(package_db_path+".lock"));
    if (!output.open(QIODevice::WriteOnly)) {
        qDebug() << "Unable to write file";
        return false;
    }
    QTextStream out(&output);
    doc.save(out,2,QDomNode::EncodingFromTextStream);
    output.close();
    input.remove();
    output.rename(package_db_path);
    globalCount++;
    updateGlobalCount();
    return true;
}

QStringList plugXMLHandler::removePackage(const QString &name, const QString &type) {
    QDomDocument doc_root;
    QFile input(package_db_path);
    QStringList files_list;
    if (!input.open(QIODevice::ReadWrite)) {
        //x3
        input.close();
        qDebug() << "Can't read database. Check your pesmissions.";
        return QStringList();
    }
    if (!doc_root.setContent(&input)) {
        // x3
        input.close();
        qDebug() << "Broken package database";
        return QStringList();
    }

    QDomElement packages = doc_root.documentElement();
    QDomNodeList packages_node = packages.childNodes();
    //foreach (QDomNode package, packages.childNodes())
    for (int num = 0; num < packages_node.count(); num++)
    {
        QDomNode package = packages_node.item(num);
        if (package.firstChildElement("name").text() == name)
        {
            files_list = createFilesList(package.firstChildElement("files").firstChild());
            package.parentNode().removeChild(package);
            qDebug() << doc_root.toString();
            input.close();
            QFile output(QString(package_db_path+".lock"));
            if (!output.open(QIODevice::WriteOnly)) {
                qDebug() << "Unable to write file";
                return QStringList ();
            }
            //globalCount--; //obsolete stuff
            QTextStream out(&output);
            doc_root.save(out,2,QDomNode::EncodingFromTextStream);
            output.close();
            input.remove();
            output.rename(package_db_path);
            return files_list;
        }
    }
    /*QDomNode n = packages.firstChild();
        while (!n.isNull()) {
                QDomElement e = n.toElement(); // try to convert the node to an element.
                QDomNode p = n.firstChild();
                bool package_found = false;
                while (!p.isNull()){
                                e = p.toElement();
                                if (e.tagName()=="name"&&e.text()==name) {
                                        package_found = true;
                                        continue;
                                }
                                if (e.tagName()=="files"&&package_found) {
                                        files_list = createFilesList(e.firstChild());
                                        break;
                                }
                                p = p.nextSibling();
                }
                if (package_found&&!files_list.isEmpty()) {
                        doc_root.removeChild(n);
                        qDebug() << doc_root.toString();
                        return files_list;
                }
                 n = n.nextSibling();
        }*/
    input.close();
    return files_list;
}


packageInfo plugXMLHandler::getPackageInfo(const QString& filename) {
    QDomDocument doc;

    QFile input(filename);
    if (!input.open(QIODevice::ReadOnly)) {
        qDebug() << "unable to open file";
    }
    if (!doc.setContent(&input)) {
        qDebug() << "unable to set content";
    }
    input.close();
    return createPackageInfoFromNode(doc.documentElement().firstChild());
}

QHash< QString, packageInfo > plugXMLHandler::getPackageList(QString path) {
    QDomDocument doc_root;
    if (path.isNull())
        path = package_db_path;
    QFile input(path);
    if (!input.open(QIODevice::ReadOnly)) {
        qDebug() <<"Unable to open file" << input.error();
    }
    if (!doc_root.setContent(&input)) {
        qDebug() << "Unable to set content";
    }
    input.close();
    return createPackageList(doc_root);
}

bool plugXMLHandler::isValid(QDomDocument doc) {
    return true;
}

void plugXMLHandler::updateGlobalCount() {
    QSettings settings(QSettings::defaultFormat(), QSettings::UserScope, "qutim/plugman", "plugman");
    settings.beginGroup("global");
    settings.setValue("count",globalCount);
    settings.endGroup();
}

bool plugXMLHandler::rebuildGlobalCount() {
    return true;
}

QSet< QString > plugXMLHandler::getPackageNames() {
    QFile input(package_db_path);
    QDomDocument doc_root;
    if (!input.open(QIODevice::ReadOnly)) {
        //x3
    }
    if (!doc_root.setContent(&input)) {
        // x3
    }
    input.close();
    QDomElement packages = doc_root.documentElement();
    QDomNode n = packages.firstChild();
    QSet<QString> names;
    while (!n.isNull()) {
        QDomElement e = n.toElement(); // try to convert the node to an element.
        names.insert(e.firstChildElement("name").text());
        n = n.nextSibling();
    }
    return names;
}

packageInfo plugXMLHandler::getPackageInfoFromDB(const QString &name, const QString &type)
{
    QDomDocument doc_root;
    QFile input(package_db_path);
    QStringList files_list;
    if (!input.open(QIODevice::ReadOnly)) {
        //x3
        input.close();
        qDebug() << "Can't read database. Check your pesmissions.";
        return packageInfo();
    }
    if (!doc_root.setContent(&input)) {
        // x3
        input.close();
        qDebug() << "Broken package database";
        return packageInfo();
    }

    QDomElement packages = doc_root.documentElement();
    QDomNodeList packages_node = packages.childNodes();
    for (int num = 0; num < packages_node.count(); num++)
    {
        QDomNode package = packages_node.item(num);
        qDebug () << package.firstChildElement("name").text() << name ;
        if (package.firstChildElement("name").text() == name)
        {
            return createPackageInfoFromNode(package.firstChildElement());
        }
    }
    return packageInfo();
}


