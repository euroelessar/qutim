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

#ifndef PLUGPACKAGE_H
#define PLUGPACKAGE_H
#include <QHash>
#include <QStringList>
#include <QIcon>
#include "utils/plugversion.h"

// TODO: Rewrite, add functions for generating to and from XML
struct packageInfo {
    packageInfo ();
    packageInfo  (	QString name,
                   QStringList packageFiles,
                   QString type = "other",
                   QString description = "Unknown package",
                   QString author = "Unknown author",
                   QString licence = "Unknown licence",
                   quint16 id = 0
                                //недописанный конструктор
                 );
    bool isValid();
    //предварительный вариант необходимой инфы о пакете
    QHash<QString,QString> properties;
    QHash<QString,plugVersion> dependencyList; //!new stuff from EuroElessar
    QStringList files; //установленные файлы
    QString toString(); //! x3
    quint16 id;
    QString ErrorString; //текст первой ошибки, которая привела к isValid = false
};

enum packageAttribute {
    isUpgradable,
    isDowngradable,
    isInstallable,
    installed,
};

enum itemType {
    buddy,
    group
};

enum packageChecked {
    unchecked,
    markedForInstall,
    markedForUpgrade,
    markedForDowngrade,
    markedForRemove,
};

struct ItemData {
    ItemData(itemType type,
             QIcon icon = QIcon(),
             packageInfo packageItem = packageInfo(),
             packageAttribute attribute = installed,
             packageChecked checked = unchecked);
    ItemData();
    QString name; //для удобства, состоит из packageitem.type/packageitem.name
    packageInfo packageItem;
    QIcon icon;
    itemType type;
    packageAttribute attribute;
    packageChecked checked;
};
#endif // PLUGPACKAGE_H

