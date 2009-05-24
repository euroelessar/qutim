#include "plugmansettings.h"
#include "plugxmlhandler.h"
#include <QListWidgetItem>
#include "pluginstaller.h"
#include <QDebug>
plugmanSettings::plugmanSettings(const QString &profile_name)
{
	setupUi(this);
	m_profile_name = profile_name;
}

plugmanSettings::~plugmanSettings()
{
}
