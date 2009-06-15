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
#include "plugitemdelegate.h"
//#include <qutim/settings.h>
#include <QDebug>

//using namespace qutim_sdk_0_2;

plugManager::plugManager(QWidget* parent)
{

    //Settings settings = Settings(); ждем элесарушку
    QSettings settings(QSettings::defaultFormat(), QSettings::UserScope, "qutim/plugman", "plugman"); //FIXME на Элесаровской либе переделать
    settings.beginGroup("interface");
    bool isGrouped = settings.value("isGrouped", false).toBool();
    settings.endGroup();

    m_package_model = new plugPackageModel (isGrouped, this);
    setupUi(this);
    setParent(parent);
    progressBar->setVisible(false);
    treeView->setModel(m_package_model);
    treeView->setAnimated(true);
    treeView->setItemDelegate(new plugItemDelegate(this));
    findField->setVisible(false);
    findButton->setVisible(false);


    QMenu *menu = new QMenu(tr("Actions"),this);
    m_actions.append(new QAction(QIcon(":/icons/open.png"),tr("Install package from file"),this));
    connect(m_actions.at(0),SIGNAL(triggered(bool)),this,SLOT(installFromFile()));
    m_actions.append(new QAction(QIcon(":/icons/internet.png"),tr("Update packages list"),this));
    connect(m_actions.at(1),SIGNAL(triggered(bool)),this,SLOT(updatePackageList()));
    m_actions.append(new QAction(QIcon(),tr("Upgrade all"),this));
    connect(m_actions.at(2),SIGNAL(triggered(bool)),m_package_model,SLOT(upgradeAll()));
    menu->addActions(m_actions);
    menu->addSeparator();
    m_actions.append(new QAction(QIcon(":/icons/revert.png"),tr("Revert changes"),this));
    connect(m_actions.at(3),SIGNAL(triggered(bool)),m_package_model,SLOT(uncheckAll()));
    menu->addAction(m_actions.back());
    actionsButton->setMenu(menu);
    connect(okButton,SIGNAL(clicked()),this,SLOT(applyChanges()));
    updatePackageList();
    //connect (m_package_model,SIGNAL(dataChanged()),this,SLOT(updatePackageList()));
}

plugManager::~plugManager() {
    qDeleteAll(m_actions);
    delete(m_package_model);
}

void plugManager::installFromFile() {
    plugInstaller *plug_install = new plugInstaller;
    plug_install->setParent(this);
    progressBar->setVisible(true);
    plug_install->setProgressBar(progressBar);
    // 	progressBar->setFormat(tr("Install package : %p%"));
    plug_install->installPackage();
    connect(plug_install,SIGNAL(destroyed(QObject*)),this,SLOT(updatePackageList()));
}

void plugManager::updatePackageList() {
    QSettings settings(QSettings::defaultFormat(), QSettings::UserScope, "qutim/plugman", "plugman"); //FIXME на Элесаровской либе переделать
    bool isLocked = settings.value("locked", false).toBool();
    treeView->setDisabled(isLocked);
    actionsButton->setDisabled(isLocked);
    okButton->setDisabled(isLocked);
    m_package_model->clear();
    progressBar->setVisible(true);
    plugPackageHandler *plug_package_handler = new plugPackageHandler (m_package_model,progressBar);
    plug_package_handler->getPackageList();
    connect(plug_package_handler,SIGNAL(destroyed()),this,SLOT(updatePackageView()));
}

void plugManager::updatePackageView() {
    progressBar->setVisible(false);
}

void plugManager::applyChanges() {
    QHash<QString, plugPackageItem *> checked_packages = m_package_model->getCheckedPackages();
    plugInstaller *plug_install = new plugInstaller;
    plug_install->setParent(this);
    progressBar->setVisible(true);
    plug_install->setProgressBar(progressBar);
    QList<packageInfo> packages_list;    //this packages marked for install or upgrade
    foreach (plugPackageItem *package, checked_packages) {
        if (package->getItemData()->checked == markedForRemove)
            plug_install->removePackage(package->getItemData()->packageItem.properties.value("name"),
                                        package->getItemData()->packageItem.properties.value("type")
                                        );
        else if (package->getItemData()->checked == markedForInstall || package->getItemData()->checked == markedForUpgrade)
            packages_list.append(package->getItemData()->packageItem);
    }
    plug_install->installPackages(packages_list);
    connect(plug_install,SIGNAL(destroyed(QObject*)),this,SLOT(updatePackageList()));
}

void plugManager::closeEvent(QCloseEvent* event)
{
    emit closed();
    deleteLater();
}
