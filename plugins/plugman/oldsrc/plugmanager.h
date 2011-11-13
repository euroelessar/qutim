/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright (C) 2011 Ruslan Nigmatullin euroelessar@yandex.ru
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

#ifndef PLUGMANAGER_H
#define PLUGMANAGER_H
#include "ui_manager.h"
#include <QDesktopWidget>
#include <QMenu>
#include "plugpackagehandler.h"
#include "pluginstaller.h"

class plugManager : public QWidget, public Ui::manager
{
    Q_OBJECT
public:
    plugManager(QWidget* parent = 0);
    ~plugManager();
    void closeEvent(QCloseEvent* );
private:
    QList<QAction *> m_actions;
    plugPackageModel *m_package_model;
public slots:
    void updatePackageList();
    void updateProgressBar(const uint &completed, const uint &total, const QString &format);
private slots:
    void updatePackageView();
    void applyChanges();
signals:
    void closed();
};

#endif // PLUGMANAGER_H

