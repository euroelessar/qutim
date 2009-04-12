#include "plugmansettings.h"
#include "plugxmlhandler.h"
#include <QListWidgetItem>
#include <pluginstaller.h>

plugmanSettings::plugmanSettings(const QString &profile_name) 
{
	setupUi(this);
	m_profile_name = profile_name;
	connect(installfromfileBtn, SIGNAL(clicked()), this,  SIGNAL(installfromfileclick()));
	progressBar->setVisible(false);
	getPackageList();
}

plugmanSettings::~plugmanSettings()
{
	
}

void plugmanSettings::getPackageList() {
	plugXMLHandler plug_handler;
	QHash<quint16, packageInfo> package_list= plug_handler.getPackageList();
	QHash<quint16, packageInfo>::iterator it = package_list.begin();
	for (;it != package_list.end();it++) {
 		QListWidgetItem *listitem = new QListWidgetItem(package_list[it.key()].properties["name"]);
		listitem->setToolTip(package_list[it.key()].properties.value("shortdesc"));

		listWidget->addItem(listitem);
		;
	}
	
	return;
}

void plugmanSettings::on_installfromfileBtn_clicked() {
	plugInstaller *plug_install = new plugInstaller;
	plug_install->setParent(this);
	plug_install->setProgressBar(progressBar);
	plug_install->installPackage();
// 	connect(plug_install,SIGNAL(finished()),this,SLOT(updatePackageList()));
	connect(plug_install,SIGNAL(destroyed(QObject*)),this,SLOT(updatePackageList()));
}

void plugmanSettings::updatePackageList() {
	listWidget->clear();
	getPackageList();
 	progressBar->setVisible(false);
}
