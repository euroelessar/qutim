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
#include <QProgressBar>
#include "plugxmlhandler.h"

class plugInstaller : public QObject
{
    Q_OBJECT
public:
    plugInstaller ();
    ~plugInstaller ();
    void installPackage(); //TODO переименовать во что нить более понятное
    void installPackages(const QList<packageInfo> &packages_list); //Install or upgrade multiply packages
    void removePackage (const QString& name, const QString& type = "all");
    QString outPath; //папка, куда будут распаковываться архивы, TODO QStringList paths = plugin_system->getSharePaths(); support
    void setProgressBar (QProgressBar *progressBar);
private:
    QStringList unpackArch (const QString &inPath);
    packageInfo getPackageInfo (const QString &archPath);
    bool collision_protect;
    void installFromFile (const QString &inPath);
//  void installFromXML (const QString &inPath);
    QProgressBar *m_progressBar;
    void install(QString);
signals:
    void finished ();
    void error(QString);
public slots:
    void errorHandler(const QString &error);
    void install(QStringList); //готов для установки (то есть скачался)
};

#endif // PLUGINSTALLER_H
