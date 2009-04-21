#include "plugman.h"
#include <QDebug>
#include <QAction>
#include <QDir>
#include <QProgressBar>

bool plugMan::init ( PluginSystemInterface *plugin_system )
{
    qRegisterMetaType<TreeModelItem> ( "TreeModelItem" );

    PluginInterface::init ( plugin_system );
    m_plugin_system = plugin_system;
    TreeModelItem contact;
	plugManager *m_plug_manager = new plugManager ();	
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
	settingswidget = new plugmanSettings(m_profile_name,m_plug_manager);
    return settingswidget;
}

void plugMan::setProfileName ( const QString &profile_name )
{
	//FIXME костыль!!!
	m_actions.insert("install", new QAction(QIcon ( ":/icons/open.png" ),tr("Install package from file"),this));
	m_plugin_system->registerMainMenuAction(m_actions.value("install"));
	connect(m_actions.value("install"), SIGNAL(triggered()), this, SLOT(on_installfromfileBtn_clicked()));
	
// 	m_actions.insert("manager", new QAction(QIcon ( ":/icons/internet.png" ),tr("Manage packages"),this));
// 	m_plugin_system->registerMainMenuAction(m_actions.value("manager"));
// 	connect(m_actions.value("manager"), SIGNAL(triggered()), this, SLOT(on_managerBtn_clicked()));
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
	return new QIcon (":/icons/plugin.png");
}

QString plugMan::type()
{
    return "other";
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

void plugMan::on_managerBtn_clicked() {
// 	m_plug_manager->show();
}

plugMan::~plugMan() {

}


Q_EXPORT_PLUGIN2 ( plugman,plugMan );


