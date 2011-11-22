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

#include "plugmanager.h"
#include "plugitemdelegate.h"
#include <qutim/iconmanagerinterface.h>
//#include <qutim/settings.h>
#include <QDebug>

using namespace qutim_sdk_0_2;

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
    m_actions.append(new QAction(SystemsCity::IconManager()->getIcon("network"),tr("Update packages list"),this));
    connect(m_actions.at(0),SIGNAL(triggered(bool)),this,SLOT(updatePackageList()));
	m_actions.append(new QAction(SystemsCity::IconManager()->getIcon("up"),tr("Upgrade all"),this));
    connect(m_actions.at(1),SIGNAL(triggered(bool)),m_package_model,SLOT(upgradeAll()));
    menu->addActions(m_actions);
    menu->addSeparator();
	m_actions.append(new QAction(SystemsCity::IconManager()->getIcon("remove"),tr("Revert changes"),this));
    connect(m_actions.at(2),SIGNAL(triggered(bool)),m_package_model,SLOT(uncheckAll()));
    menu->addAction(m_actions.back());
    actionsButton->setMenu(menu);
	actionsButton->setIcon(SystemsCity::IconManager()->getIcon("network"));
	findButton->setIcon(SystemsCity::IconManager()->getIcon("find"));
    connect(okButton,SIGNAL(clicked()),this,SLOT(applyChanges()));
    updatePackageList();
    //connect (m_package_model,SIGNAL(dataChanged()),this,SLOT(updatePackageList()));
}

plugManager::~plugManager() {
    qDeleteAll(m_actions);
    delete(m_package_model);
}

void plugManager::updatePackageList() {
    QSettings settings(QSettings::defaultFormat(), QSettings::UserScope, "qutim/plugman", "plugman"); //FIXME на Элесаровской либе переделать
    bool isLocked = settings.value("locked", false).toBool();
    treeView->setDisabled(isLocked);
    actionsButton->setDisabled(isLocked);
    okButton->setDisabled(isLocked);
    m_package_model->clear();
    progressBar->setVisible(true);
    plugPackageHandler *plug_package_handler = new plugPackageHandler (m_package_model,this);
    connect(plug_package_handler,SIGNAL(updateProgressBar(uint,uint,QString)),SLOT(updateProgressBar(uint,uint,QString)));
    plug_package_handler->getPackageList();
    connect(plug_package_handler,SIGNAL(destroyed()),this,SLOT(updatePackageView()));
}

void plugManager::updatePackageView() {
    progressBar->setVisible(false);
}

void plugManager::applyChanges() {
    QHash<QString, plugPackageItem *> checked_packages = m_package_model->getCheckedPackages();
	if (checked_packages.count()==0)
		return;
    plugInstaller *plug_install = new plugInstaller;
    plug_install->setParent(this);
    progressBar->setVisible(true);
    connect(plug_install,SIGNAL(updateProgressBar(uint,uint,QString)),SLOT(updateProgressBar(uint,uint,QString)));
    QList<packageInfo *> packages_list;    //this packages marked for install or upgrade
    foreach (plugPackageItem *package, checked_packages) {
        if (package->getItemData()->checked == markedForRemove)
			plug_install->removePackage(package->getItemData()->packageItem.properties.value("name"));
        else if (package->getItemData()->checked == markedForInstall || package->getItemData()->checked == markedForUpgrade)
			plug_install->installPackage(new packageInfo (package->getItemData()->packageItem));
    }
    connect(plug_install,SIGNAL(destroyed(QObject*)),this,SLOT(updatePackageList()));
	plug_install->commit();
}

void plugManager::closeEvent(QCloseEvent* event)
{
    emit closed();
    deleteLater();
}


void plugManager::updateProgressBar(const uint& completed, const uint& total, const QString& format)
{
	progressBar->setMaximum(total);
	progressBar->setValue(completed);
        progressBar->setFormat(format);
}

