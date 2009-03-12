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

//Обработчик xml для qutIM plugin manager'а
class QUrl;
class plugXMLHandler : public QObject
{
Q_OBJECT
struct packageInfo {
	packageInfo  (QString name,
				  QStringList files,
				  QString type = "other",
				  QString description = "Unknown package",
				  QString author = "Unknown author",
				  QString licence = "Unknown licence"
				  /*, QHash<QString,QString> info; */ //более красивый вариант, чуть позже его сделаю
				 );
	//предварительный вариант необходимой инфы о пакете
	QString name; 
	QString type; //тип пакета, сделал обычным строковым чтобы проще с ним работать было
	QString version;
	QString description;
	QStringList files; //установленные файлы
	QString author; //автор в формате "Имя, name@mail.ru"
	QString licence; //полезная фича, пусть буит
};
public:
	plugXMLHandler ();
	~plugXMLHandler ();
	bool registerPackage (packageInfo package_info);
	QStringList removePackage (int package_id);
	/*!удаляет пакет из базы данных и возращает список файлов для удаления
	*/
	packageInfo getPackageInfo (const QString &filename);
	/*!
	получаем необходимую инфу установщика из файла
	*/
	packageInfo getPackageInfo (const QUrl &url);
	/*!
	получаем необходимую инфу установщика из интернета (x3)
	*/
private:
	QDomDocument createDomFromPackage (packageInfo package_info, int id = NULL); 
	/*!создает Dom document, 
	@param packageInfo - информация о пакете
	@param id - package id, необходим в том случае, когда инфа о пакете записывается в общую базу данных
	В том случае, если он NULL то это поле игнорируется
	*/
	packageInfo createPackageInfoFromDom (QDomDocument &doc, int id = NULL); 
	/*!создает struct packageInfo и Dom (обратная операция для createDomFromPackage), 
	@param doc - x3
	@param id - ежели берется список с несколькими пакетами то нужный ищется по id
	*/
	bool updateGlobalCount (bool up); //!увеличиает количество пакетов на 1 при добавлении и уменьшаяет на 1 при удалении
	bool rebuildGlobalCount (); //!более брутальный способ, применять при ошибках
	bool updatePackageId (int begin_id = 0); 
	/*!при удалении пакета, номера вслед за ним идущих должны быть пересчитаны
	@param begin_id - id, с которого начинать пересчёт, если ничего не вводить, то буит обновлятся вся база
	*/
	bool isValid (QDomDocument doc); //! защита от дураков и ССЗБ
signals:
	void error (QString); //! в случае ошибки посылается этот сигнал
};

#endif // PLUGXMLHANDLER_H
