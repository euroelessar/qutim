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
#include <QUrl>
#include <QFile>
#include <QHash>
#include "plugpackage.h"

//Обработчик xml для qutIM plugin manager'а

class QUrl;
class QFile;
class plugXMLHandler : public QObject
{
    Q_OBJECT
public:
    plugXMLHandler ();
    ~plugXMLHandler ();
    bool registerPackage (const packageInfo &package_info);
    QStringList removePackage (int package_id);
    /*!удаляет пакет из базы данных и возращает список файлов для удаления
    */
    packageInfo getPackageInfo (const QString &filename);
    /*!
    получаем необходимую инфу установщика из файла
    */
	packageInfo getPackageInfo (const QByteArray &content);
	/*!
	получаем необходимую инфу установщика из QByteArray
	*/	
    packageInfo getPackageInfo (const QUrl &url);
    /*!
    получаем необходимую инфу установщика из интернета (x3)
    */
	packageInfo getPackageInfo (const int id);
    /*!
    получаем из файла package.xml инфу о пакете с id
    */	
	QHash<quint16, packageInfo> getPackageList (QString path = QString::null);
    /*!
	получаем из файла инфу обо всех установленных пакетах (по умолчанию package.xml)
    */
	QSet<QString> getPackageNames ();
private:
    QDomElement createElementFromPackage (const packageInfo &package_info, int id = NULL);
    /*!создает Dom document,
    @param packageInfo - информация о пакете
    @param id - package id, необходим в том случае, когда инфа о пакете записывается в общую базу данных
    В том случае, если он NULL то это поле игнорируется
    */
    packageInfo createPackageInfoFromDom (const QDomDocument& doc, QString id);
    /*!создает struct packageInfo и Dom (обратная операция для createDomFromPackage),
    @param doc - x3
    @param id - ежели берется список с несколькими пакетами то нужный ищется по id
    */
    packageInfo createPackageInfoFromNode(QDomNode n);
    /*!создает struct packageInfo и Dom (обратная операция для createDomFromPackage). Самый простой вариант парсера
    @param doc - x3
    */
	QHash<quint16, packageInfo> createPackageList (const QDomDocument& root);
    void updateGlobalCount (); //!увеличиает количество пакетов на 1 при добавлении и уменьшаяет на 1 при удалении
    bool rebuildGlobalCount (); //!более брутальный способ, применять при ошибках
    bool updatePackageId (int begin_id = 0);
    /*!при удалении пакета, номера вслед за ним идущих должны быть пересчитаны
    @param begin_id - id, с которого начинать пересчёт, если ничего не вводить, то буит обновлятся вся база
    */
    bool isValid (QDomDocument doc); //! защита от дураков и ССЗБ
	QStringList createFilesList (QDomNode n); //!создаем список файлов, принадлежащих пакету
// 	QSet<QString> createFilesSet (const QDomNode &n); 
	QString package_db_path; //installed package database path
	int globalCount;
signals:
    void error (QString); //! в случае ошибки посылается этот сигнал
};

#endif // PLUGXMLHANDLER_H
