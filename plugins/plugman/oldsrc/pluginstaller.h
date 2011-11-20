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

