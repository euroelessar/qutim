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

#include "plugmanager.h"
#include <QDebug>
plugManager::plugManager(QWidget* parent)
: m_package_model(new plugPackageModel)
{
	setupUi(this);
	setParent(parent);

	progressBar->setVisible(false);
	treeView->setModel(m_package_model);
	
    QMenu *menu = new QMenu(tr("Actions"),this);
	m_actions.append(new QAction(QIcon(":/icons/open.png"),tr("Install package from file"),this));
	connect(m_actions.at(0),SIGNAL(triggered(bool)),this,SLOT(installFromFile()));
	m_actions.append(new QAction(QIcon(":/icons/internet.png"),tr("Update packages list"),this));
// 	connect(m_actions.at(1),SIGNAL(triggered(bool)),this,SLOT(installFromFile()));
	m_actions.append(new QAction(QIcon(),tr("Upgrade all"),this));
	menu->addActions(m_actions);
	menu->addSeparator();
	m_actions.append(new QAction(QIcon(":/icons/revert.png"),tr("Revert changes"),this));
	menu->addAction(m_actions.back());
    actionsButton->setMenu(menu);

	updatePackageList();
}

plugManager::~plugManager() {
    qDeleteAll(m_actions);
}

void plugManager::installFromFile() {
    plugInstaller *plug_install = new plugInstaller;
    plug_install->setParent(this);
    plug_install->setProgressBar(progressBar);
    // 	progressBar->setFormat(tr("Install package : %p%"));
    plug_install->installPackage();
 	connect(plug_install,SIGNAL(destroyed(QObject*)),this,SLOT(updatePackageList()));
}

void plugManager::updatePackageList() {
	m_package_model->clear();
	plugPackageHandler plug_package_handler (m_package_model);
	plug_package_handler.getInstalledPackageList();
	progressBar->setVisible(false);
}
