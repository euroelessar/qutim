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
#include <QtCore/QDebug>
#include <QtCore/QSettings>

plugInstaller::plugInstaller() {

    QSettings settings(QSettings::defaultFormat(), QSettings::UserScope, "qutim", "plugman");
    outPath = settings.fileName().section("/",0,-2);
    qDebug() << outPath;
}

plugInstaller::~plugInstaller() {

}

bool plugInstaller::unpackArch(QString& inPath) {
//

    UnZip uz;
    UnZip::ErrorCode ec = uz.openArchive(inPath);
    if (ec != UnZip::Ok)
        return false;

    uz.extractAll(outPath);
    if (ec != UnZip::Ok)
        return false;
    uz.closeArchive(); // Close the zip file and free used resources
    return true;
}

bool plugInstaller::registerPackage(QHash< QString, QString >) {
	//пока ещё не решил, как реализовывать, надеюсь кутишного ini хватит
    QSettings settings(QSettings::defaultFormat(), QSettings::UserScope, "qutim/plugman", "packages");
}

