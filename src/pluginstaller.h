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

#include <QtCore/QObject>
#include <osdabzip/unzip.h>
#include <QtCore/QHash>


class plugInstaller : public QObject
{
Q_OBJECT
public:
        plugInstaller ();
        ~plugInstaller ();
        QString outPath; //папка, куда будут распаковываться архивы
        bool registerPackage (QHash<QString, QString>); //записывает в базу инфу об установленных пакетах
		bool removePackage (QString &type, QString &name); //для имеющих xml
		bool removePackage (QString &path); //для простых zip архивов
public slots:
        bool unpackArch (QString &inPath);
};

#endif // PLUGINSTALLER_H
