#include "plugmansettings.h"
#include "plugxmlhandler.h"
#include <QListWidgetItem>
#include "pluginstaller.h"
#include <QDebug>
plugmanSettings::plugmanSettings(const QString &profile_name)
{
	setupUi(this);
	m_profile_name = profile_name;
    QSettings settings(QSettings::defaultFormat(), QSettings::UserScope, "qutim", "plugman"); //FIXME на Элесаровской либе переделать
    settings.beginGroup("interface");
	GroupedCheckBox->setChecked(settings.value("isGrouped", false).toBool());
    settings.endGroup();	
}

plugmanSettings::~plugmanSettings()
{
}

void plugmanSettings::saveSettings()
{
	//Settings settings = Settings(); ждем элесарушку
    QSettings settings(QSettings::defaultFormat(), QSettings::UserScope, "qutim", "plugman"); //FIXME на Элесаровской либе переделать
    settings.beginGroup("interface");
	settings.setValue("isGrouped", GroupedCheckBox->isChecked());
    settings.endGroup();
}
