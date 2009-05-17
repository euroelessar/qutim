#ifndef PLUGMAN_H
#define PLUGMAN_H
#include <qutim/plugininterface.h>
#include "pluginstaller.h"
#include "plugmansettings.h"
#include "plugmanager.h"

//Менеджер тем оформления и плагинов

using namespace qutim_sdk_0_2;

class plugMan : public QObject, SimplePluginInterface
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
    QString type(); //installer
    QIcon *icon(); //Функция, возращающая иконку
    virtual void removeSettingsWidget();
    virtual void saveSettings();
private:
	QHash<QString, QAction *> m_actions;
    PluginSystemInterface *m_plugin_system;
    QString m_profile_name;
    QString m_account_name;
    plugmanSettings *settingswidget;
    TreeModelItem eventitem;
	bool isPlugManagerOpened;
	plugManager *m_manager;
private slots:
	void onInstallfromfileBtnClicked();
	void onManagerBtnClicked();
	void onManagerClose();
};
#endif
