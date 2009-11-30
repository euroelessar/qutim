#include "messaging.h"
#include "messagingdialog.h"
#include "manager.h"

bool Messaging::init ( PluginSystemInterface *plugin_system )
{
    qRegisterMetaType<TreeModelItem> ( "TreeModelItem" );

    PluginInterface::init ( plugin_system );
    return true;
}

void Messaging::release()
{
    delete(m_manager);
}

void Messaging::processEvent ( PluginEvent &/*event*/ )
{

}

QWidget *Messaging::settingsWidget()
{
    QWidget* form = new QWidget;
    return form;
}

void Messaging::setProfileName ( const QString &profile_name )
{
    m_manager = new Manager(this);
    SystemsCity::PluginSystem()->registerEventHandler (
            "Core/ContactList/ItemAdded",
            m_manager,
            SLOT(addItem(TreeModelItem,QString))
            );
    m_dialog = new MessagingDialog(m_manager); 
    QAction *messaging_dialog = new QAction(SystemsCity::PluginSystem()->getIcon("multiple"),tr("Multiply Sending"),this);
    SystemsCity::PluginSystem()->registerMainMenuAction(messaging_dialog);
    m_profile_name = profile_name;
    connect(messaging_dialog,SIGNAL(triggered(bool)),SLOT(onMessagingActionTriggered()));
}

QString Messaging::name()
{
    return "Mass Messaging";
}

QString Messaging::description()
{
    return "qutIM Mass messaging plugin";
}

QIcon *Messaging::icon()
{
    return new QIcon();
}

QString Messaging::type()
{
    return "messaging";
}


void Messaging::removeSettingsWidget()
{
    //X3
}

void Messaging::saveSettings()
{
    //X3
}

void Messaging::onMessagingActionTriggered()
{
    m_dialog->show();
}


Q_EXPORT_PLUGIN2 ( Messaging,Messaging );

