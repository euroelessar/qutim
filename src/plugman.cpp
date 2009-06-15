#include "plugman.h"
#include <QDebug>
#include <QAction>
#include <QDir>
#include <QProgressBar>
#ifdef Q_OS_WIN
#include <QProcess>
#endif

bool plugMan::init ( PluginSystemInterface *plugin_system )
{
    qRegisterMetaType<TreeModelItem> ( "TreeModelItem" );

    PluginInterface::init ( plugin_system );
    isPlugManagerOpened = false;
    return true;
}

void plugMan::release()
{
#ifdef Q_OS_WIN
    QSettings settings(QSettings::defaultFormat(), QSettings::UserScope, "qutim/plugman", "plugman");
    if (settings.value("update", false ).toBool()) {
        QFileInfo config_dir = settings.fileName();
        QDir current_dir = qApp->applicationDirPath();
        QString path;
        if( config_dir.canonicalPath().contains( current_dir.canonicalPath() ) )
            path = current_dir.relativeFilePath( config_dir.absolutePath() );
        else
            path = config_dir.absolutePath();
        settings.setValue("update",false);
        settings.setValue("isLocked",false);
        QProcess::startDetached("cmd.exe", QStringList() << "/c postinst.bat", path );
    }
#endif
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


