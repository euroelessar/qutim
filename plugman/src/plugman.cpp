#include "plugman.h"
#include <QDebug>
#include <QAction>
#include <QDir>
#include <QProgressBar>
#include <QProcess>
#include <qutim/iconmanagerinterface.h>

bool plugMan::init ( PluginSystemInterface *plugin_system )
{
    qRegisterMetaType<TreeModelItem> ( "TreeModelItem" );

    PluginInterface::init ( plugin_system );
    isPlugManagerOpened = false;

    return true;
}

void plugMan::release()
{
    QSettings settings(QSettings::defaultFormat(), QSettings::UserScope, "qutim/plugman", "plugman");
    if (settings.value("needUpdate", false ).toBool()) {
       QProcess::startDetached(qAppName());
    }
    settings.setValue("needUpdate",false);
    settings.setValue("locked", false);
}

void plugMan::processEvent ( PluginEvent  &event)
{
    eventitem = *(TreeModelItem*)(event.args.at(0));
}

QWidget *plugMan::settingsWidget()
{
    settingswidget = new plugmanSettings(m_profile_name);
    return settingswidget;
}

void plugMan::setProfileName ( const QString &profile_name )
{
    QAction *plugman_action = new QAction(SystemsCity::IconManager()->getIcon("network"),tr("Manage packages"),this);
    SystemsCity::PluginSystem()->registerMainMenuAction(plugman_action);
    connect(plugman_action, SIGNAL(triggered()), this, SLOT(onManagerBtnClicked()));
    m_profile_name = profile_name;
}

QString plugMan::name()
{
    return "Plugin's manager";
}

QString plugMan::description()
{
    return "Simple add-ons manager for qutIM";
}

QIcon *plugMan::icon()
{
	QIcon *icon = new QIcon (SystemsCity::IconManager()->getIconPath("package")); //FIXME КОСТЫЛЬ
	return icon;
}

QString plugMan::type()
{
    return "other";
}


void plugMan::removeSettingsWidget()
{
    delete settingswidget;
    settingswidget = 0;
}

void plugMan::saveSettings()
{
    settingswidget->saveSettings();
}


void plugMan::onManagerBtnClicked() {
    if (!isPlugManagerOpened) {
        plugManager *m_manager = new plugManager ();
        connect(m_manager,SIGNAL(closed()),SLOT(onManagerClose()));
        SystemsCity::PluginSystem()->centerizeWidget(m_manager);
        m_manager->show();
        isPlugManagerOpened = true;
    }
}

void plugMan::onManagerClose()
{
    isPlugManagerOpened = false;
}




Q_EXPORT_PLUGIN2 ( plugman,plugMan );


