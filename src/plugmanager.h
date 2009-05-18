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
private slots:
	void installFromFile();
        void updatePackageView();
        void applyChanges();
signals:
	void closed();
};

#endif // PLUGMANAGER_H
