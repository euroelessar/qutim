#include "plugmansettings.h"
#include "plugxmlhandler.h"
#include <QListWidgetItem>
#include "pluginstaller.h"
#include <QDebug>
plugmanSettings::plugmanSettings(const QString &profile_name, plugManager *plug_manager)
: m_package_model(new plugPackageModel)
{
// 	m_plug_manager = plug_manager;
	m_plug_manager = new plugManager ();
	setupUi(this);
	m_profile_name = profile_name;
	PackagesTab->layout()->addWidget(m_plug_manager); 
}

plugmanSettings::~plugmanSettings()
{
	delete(m_package_model);
}

void plugmanSettings::getPackageList() {
	plugXMLHandler plug_handler;
	m_package_model->clear();
	QHash<quint16, packageInfo> package_list= plug_handler.getPackageList();
	QHash<quint16, packageInfo>::iterator it = package_list.begin();
	for (;it != package_list.end();it++) {
		ItemData item(buddy,QIcon(":/icons/hi64-action-package.png"),package_list.value(it.key()),it.key());
		m_package_model->addItem(item,item.packageItem.properties.value("name"));
	}
	return;
}

void plugmanSettings::updatePackageList() {
	getPackageList();
	m_plug_manager->progressBar->setVisible(false);
}

plugManager* plugmanSettings::getPlugManager() {
	return m_plug_manager;
}

void plugmanSettings::setPlugManager(plugManager* plug_manager) {
	m_plug_manager = plug_manager;
}
