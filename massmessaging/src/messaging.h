#ifndef SIMPLEPLUGIN_H
#define SIMPLEPLUGIN_H
#include <qutim/plugininterface.h>
#include <QAction>


class Manager;
class MessagingDialog;
using namespace qutim_sdk_0_2;

class Messaging : public QObject, PluginInterface
{
    Q_OBJECT
    Q_INTERFACES ( qutim_sdk_0_2::PluginInterface )
public:
    bool init ( PluginSystemInterface *plugin_system ); //инициализация нашего плагина
    void release();

    virtual void processEvent ( PluginEvent &event );
    virtual QWidget *settingsWidget(); //указатель для настройки
    virtual void setProfileName ( const QString &profile_name ); //установка текущего профиля
    QString name(); //Функция, возращающая имя плагина
    QString description(); //Функция, возращающая описание
    QString type(); //this function is an unknown stuff, created by m0rph, noone can tell, what the hell it is
    QIcon *icon(); //Функция, возращающая иконку
    virtual void removeSettingsWidget();
    virtual void saveSettings();
private:
    QString m_profile_name;
    QString m_account_name;
    Manager *m_manager;
    MessagingDialog *m_dialog;
private slots:
    void onMessagingActionTriggered ();
};
#endif
