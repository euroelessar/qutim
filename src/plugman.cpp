#include "plugman.h"
#include <QDebug>
#include <QAction>
#include <QDir>
#include <QProgressBar>

bool plugMan::init ( PluginSystemInterface *plugin_system )
{
    qRegisterMetaType<TreeModelItem> ( "TreeModelItem" );

    PluginInterface::init ( plugin_system );
    plug_icon = new QIcon ( ":/icons/plugin.png" );
    download_icon = QIcon ( ":/icons/open.png");
    m_plugin_system = plugin_system;
    TreeModelItem contact;
    plug_action = new QAction(download_icon,tr("Install package from file"),this);
    m_plugin_system->registerMainMenuAction(plug_action);
    connect(plug_action, SIGNAL(triggered()), this, SLOT(on_installfromfileBtn_clicked()));

	
	QSettings settings(QSettings::defaultFormat(), QSettings::UserScope, "qutim/plugman", "plugman"); //костыль
    QString outPath = settings.fileName().section("/",0,-2)+"/";
	QDir dir;
	if (!dir.exists(outPath))
		dir.mkpath(outPath);

    return true;
}

void plugMan::release()
{
//	x3 =)
}

void plugMan::processEvent ( PluginEvent  &event)
{
	eventitem = *(TreeModelItem*)(event.args.at(0));
}

QWidget *plugMan::settingsWidget()
{
	settingswidget = new plugmanSettings(m_profile_name);
//  connect(settingswidget, SIGNAL(installfromfileclick()), this,  SLOT(on_installfromfileBtn_clicked()));
    return settingswidget;
}

void plugMan::setProfileName ( const QString &profile_name )
{
    m_profile_name = profile_name;
}

QString plugMan::name()
{
    return "Plugin's manager";
}

QString plugMan::description()
{
    return "x3 (=";
}

QIcon *plugMan::icon()
{
    return plug_icon;
}

QString plugMan::type()
{
    return "demo";
}


void plugMan::removeSettingsWidget()
{
    //X3
}

void plugMan::saveSettings()
{
    //X3
}

void plugMan::on_installfromfileBtn_clicked()
{
	plugInstaller *plug_install = new plugInstaller;
	plug_install->setParent(this);
	plug_install->setProgressBar(new QProgressBar);
	plug_install->installPackage();
}

Q_EXPORT_PLUGIN2 ( plugman,plugMan );


