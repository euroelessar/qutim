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
#include <QZipReader/qzipreader.h>
#include "plugdownloader.h"

class plugInstaller : public QObject
{
    Q_OBJECT
public:
    plugInstaller ();
    ~plugInstaller ();
	void installPackage(packageInfo *info);
    void removePackage (const QString& name);
	void commit();
private:
    QStringList unpackArch (const QString &inPath, const QString &prefix);
    QStringList getFileList (const QString& list);
    QString getPackagePrefix(const packageInfo *package_info);
	void install();
	void remove();
    bool collision_protect;
    QHash<QString,QString> package_prefix;
    bool needUpdate;
    bool backup;
	QList<packageInfo *> installPackagesList;
	QStringList removePackagesList;
signals:
    void finished ();
    void error(QString);
    void updateProgressBar(const uint &completed, const uint &total, const QString &format);
public slots:
    void errorHandler(const QString &error);
	void install(const QList< downloaderItem >& items); //готов для установки (то есть скачался)
};

#endif // PLUGINSTALLER_H
