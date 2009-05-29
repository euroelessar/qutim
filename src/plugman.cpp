#include "plugman.h"
#include <QDebug>
#include <QAction>
#include <QDir>
#include <QProgressBar>
//#include <qutim/settings.h>

bool plugMan::init ( PluginSystemInterface *plugin_system )
{
    qRegisterMetaType<TreeModelItem> ( "TreeModelItem" );

    PluginInterface::init ( plugin_system );
    m_plugin_system = plugin_system;

}

void plugMan::release()
{
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
    m_actions.insert("install", new QAction(QIcon ( ":/icons/open.png" ),tr("Install package from file"),this));
    m_plugin_system->registerMainMenuAction(m_actions.value("install"));
    connect(m_actions.value("install"), SIGNAL(triggered()), this, SLOT(onInstallfromfileBtnClicked()));

    m_actions.insert("manager", new QAction(QIcon ( ":/icons/internet.png" ),tr("Manage packages"),this));
    m_plugin_system->registerMainMenuAction(m_actions.value("manager"));
    connect(m_actions.value("manager"), SIGNAL(triggered()), this, SLOT(onManagerBtnClicked()));

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
    return new QIcon (":/icons/plugin.png");
}

QString plugMan::type()
{
    return "other";
}


void plugMan::removeSettingsWidget()
{
    delete settingswidget;
}

void plugMan::saveSettings()
{

}

void plugMan::onInstallfromfileBtnClicked()
{
    plugInstaller *plug_install = new plugInstaller;
    plug_install->setParent(this);
    plug_install->setProgressBar(new QProgressBar);
    plug_install->installPackage();
}

void plugMan::onManagerBtnClicked() {
    if (!isPlugManagerOpened) {
        m_manager = new plugManager ();
        connect(m_manager,SIGNAL(closed()),SLOT(onManagerClose()));
        m_manager->show();
        isPlugManagerOpened = true;
    }
}

void plugMan::onManagerClose()
{
    isPlugManagerOpened = false;
}


Q_EXPORT_PLUGIN2 ( plugman,plugMan );


