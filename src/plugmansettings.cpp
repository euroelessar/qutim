#include "plugmansettings.h"
#include "plugxmlhandler.h"
#include <QListWidgetItem>
#include "pluginstaller.h"
#include <QDebug>
#include "plugpackagehandler.h"
plugmanSettings::plugmanSettings(const QString &profile_name)
{
    setupUi(this);
    m_profile_name = profile_name;
    QSettings settings(QSettings::defaultFormat(), QSettings::UserScope, "qutim/plugman", "plugman"); //FIXME на Элесаровской либе переделать
    settings.beginGroup("interface");
    GroupedCheckBox->setChecked(settings.value("isGrouped", false).toBool());
    settings.endGroup();
    plugPackageHandler handler;
    QList<mirrorInfo> mirrorList = handler.getMirrorList();
	mirrorWidget->setColumnCount(3);
	mirrorWidget->setHorizontalHeaderLabels (QStringList() << tr("Name") << tr("Description") << tr("Url") );
	mirrorWidget->setRowCount(mirrorList.count());
	for (int key = 0;key!=mirrorList.count();key++) {
        QTableWidgetItem *name = new QTableWidgetItem(mirrorList.at(key).name);
        QTableWidgetItem *description = new QTableWidgetItem(mirrorList.at(key).description);
        QTableWidgetItem *url = new QTableWidgetItem(mirrorList.at(key).url.toString());
		mirrorWidget->setItem(key,0,name);
        mirrorWidget->setItem(key,1,description);
        mirrorWidget->setItem(key,2,url);
        qDebug() << "Item added:" << name->text() << description->text() << url->text();
    }
	mirrorWidget->resizeColumnsToContents();
	mirrorWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
	mirrorWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
}

plugmanSettings::~plugmanSettings()
{
}

void plugmanSettings::saveSettings()
{
    //Settings settings = Settings(); ждем элесарушку
    QSettings settings(QSettings::defaultFormat(), QSettings::UserScope, "qutim/plugman", "plugman"); //FIXME на Элесаровской либе переделать
    settings.beginGroup("interface");
    settings.setValue("isGrouped", GroupedCheckBox->isChecked());
    settings.endGroup();
}
