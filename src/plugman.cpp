#include "plugman.h"
#include <QDebug>
#include <QAction>
#include <QDir>
#include <QProgressBar>
#include <QProcess>


bool plugMan::init ( PluginSystemInterface *plugin_system )
{
    qRegisterMetaType<TreeModelItem> ( "TreeModelItem" );

    PluginInterface::init ( plugin_system );
    isPlugManagerOpened = false;
    QSettings settings(QSettings::defaultFormat(), QSettings::UserScope, "qutim/plugman", "plugman");
    settings.setValue("needUpdate",false);
    return true;
}

void plugMan::release()
{
    QSettings settings(QSettings::defaultFormat(), QSettings::UserScope, "qutim/plugman", "plugman");
    if (settings.value("needUpdate", false ).toBool()) {
//       QProcess::startDetached(qAppName());
    }
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
    QAction *plugman_action = new QAction(QIcon ( ":/icons/internet.png" ),tr("Manage packages"),this);
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
    return new QIcon (":/icons/plugin.png");
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

void plugMan::onInstallfromfileBtnClicked()
{
    plugInstaller *plug_install = new plugInstaller;
    plug_install->setParent(this);
    plug_install->setProgressBar(new QProgressBar);
    plug_install->installPackage();
}

void plugMan::onManagerBtnClicked() {
    if (!isPlugManagerOpened) {
        plugManager *m_manager = new plugManager ();
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


