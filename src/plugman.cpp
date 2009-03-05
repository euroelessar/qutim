#include "plugman.h"
#include "plugdownloader.h"
#include "plugparser.h"

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
    //plug_loader = new plugDownoloader;
    //plug_parser = new plugParser;
    plug_install = new plugInstaller;

    return true;
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
	connect(settingswidget, SIGNAL(installfromfileclick()), this,  SLOT(on_installfromfileBtn_clicked()));
    return settingswidget;
}

void plugMan::setProfileName ( const QString &profile_name )
{
	m_profile_name = profile_name;
}

QString plugMan::name()
{
    return "Plugin manager";
}

QString plugMan::description()
{
    return "";
}

QIcon *plugMan::icon()
{
    return plug_icon;
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
    QString path = QFileDialog::getOpenFileName(0,tr("Install package from file"),".",
                                                  tr("Archives (*.zip);;XML files (*.xml)"));
    if ((path.section(".",-1))=="zip")
	{
        plug_install->installFromFile(path);
	}
    else if ((path.section(".",-1))=="xml")
        {
		plug_install->installFromXML(path);
//         qDebug()<<"Not yet implemented";
// 		plugParser plug_parser;
// 		plugDownloader *plug_loader = new plugDownloader;
//         QHash<QString, QString> packInfo = plug_parser.parseItem(path);
//         plugDownloader::downloaderItem item;
//         item.url = packInfo["url"];
//         item.filename = packInfo["name"];
//         connect(plug_loader,SIGNAL(downloadFinished(QString)),plug_install,SLOT(unpackArch(QString)));
//         plug_loader->startDownload(item);
        }

}

Q_EXPORT_PLUGIN2 ( plugman,plugMan );


