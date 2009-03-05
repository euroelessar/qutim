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

#ifndef PLUGINSTALLER_H
#define PLUGINSTALLER_H

#include <QObject>
#include <QHash>


class plugInstaller : public QObject
{
Q_OBJECT
public:
	plugInstaller ();
	~plugInstaller ();
	bool installFromFile (QString &inPath);
	bool installFromXML (QString &inPath);
	bool registerPackage (QHash<QString, QString>, QStringList &files); //записывает в базу инфу об установленных пакетах
	bool registerPackage (QString name, QStringList &files);
	bool removePackage (QString &name,QStringList &files);
	QString outPath; //папка, куда будут распаковываться архивы
public slots:
	QStringList unpackArch (QString &inPath);
private:
	QString lastError;
};

#endif // PLUGINSTALLER_H
