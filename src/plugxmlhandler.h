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

#ifndef PLUGXMLHANDLER_H
#define PLUGXMLHANDLER_H

#include <QObject>
#include <QStringList>
#include <QDomDocument>
#include <QHash>
#include "plugpackage.h"

//Обработчик xml для qutIM plugin manager'а

class QUrl;
class QFile;
class plugXMLHandler : public QObject
{
    Q_OBJECT
public:
    plugXMLHandler (QObject *parent = 0);
    ~plugXMLHandler ();
    void registerPackage (const packageInfo &package_info);
    QStringList removePackage (const QString &name, const QString &type = "");
    /*!удаляет пакет из базы данных и возращает список файлов для удаления
    */
    packageInfo getPackageInfo (const QString &filename);
    /*!
    получаем необходимую инфу установщика из файла
    */
    packageInfo getPackageInfoFromDB (const QString &name, const QString &type = "all");
    /*!
    получаем необходимую инфу из базы данных пакетов
    */
	packageInfo getPackageInfo (const QByteArray &content);
	/*!
	получаем необходимую инфу установщика из QByteArray
	*/
	QHash<QString, packageInfo> getPackageList (QString path = QString::null);
	/*!
		JSon variant
	*/
	QHash<QString, packageInfo> getPackageListJSon(const QString &path);
    /*!
	получаем из файла инфу обо всех установленных пакетах (по умолчанию package.xml)
    */
	static QDomElement createElementFromPackage (const packageInfo &package_info);
private:
    /*!создает Dom document,
    @param packageInfo - информация о пакете
    @param id - package id, необходим в том случае, когда инфа о пакете записывается в общую базу данных
    В том случае, если он NULL то это поле игнорируется
    */
    packageInfo createPackageInfoFromNode(QDomNode n);
    /*!создает struct packageInfo и Dom (обратная операция для createDomFromPackage). Самый простой вариант парсера
    @param doc - x3
    */
	QHash<QString, packageInfo> createPackageList (const QDomDocument& root);
	QStringList createFilesList (QDomNode n); //!создаем список файлов, принадлежащих пакету
	QString package_db_path; //installed package database path
signals:
    void error (QString); //! в случае ошибки посылается этот сигнал
};

#endif // PLUGXMLHANDLER_H
