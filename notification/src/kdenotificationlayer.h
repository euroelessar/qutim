/*
   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License version 2 as published by the Free Software Foundation.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#ifndef KDENOTIFICATIONLAYER_H
#define KDENOTIFICATIONLAYER_H
#include <kcomponentdata.h>
#include <KAboutData>
#include <KNotifyConfigWidget>
#include <qutim/layerinterface.h>
#include <qutim/plugininterface.h>
#include "activenotification.h"

using namespace qutim_sdk_0_2;

class KDENotificationSettingsHelper : public QWidget
{
    Q_OBJECT
public:
    KDENotificationSettingsHelper() {}
    void setWidget(QWidget *child)
    {
        QGridLayout *layout = new QGridLayout(this);
        this->setLayout(layout);
        layout->addWidget(child);
        connect(child, SIGNAL(changed(bool)), this, SIGNAL(settingsChanged()));
    }
signals:
    void settingsChanged();
};

class KDENotificationLayer : public QObject, public NotificationLayerInterface
{
    Q_OBJECT
public:
    KDENotificationLayer ();
    virtual ~KDENotificationLayer ();
    virtual bool init(PluginSystemInterface *plugin_system);
    virtual void release() {}
    virtual void removeGuiLayerSettings() {}
    virtual void saveGuiSettingsPressed() {}
    virtual QList<SettingsStructure> getLayerSettingsList();
    virtual void saveLayerSettings();
    virtual void removeLayerSettings();
    virtual void setLayerInterface(LayerType type, LayerInterface* interface);
    virtual void setProfileName(const QString& profile_name) {
        Q_UNUSED(profile_name);
    }
    virtual void showPopup(const TreeModelItem &item, const QString &message, NotificationType type);
    virtual void playSound(const TreeModelItem &item, NotificationType type);
    virtual void notify(const TreeModelItem &item, const QString &message, NotificationType type);
private:
    ActiveNotifications m_activenotifications;
    KComponentData m_component_data;
    int cutCount;
    QPointer<KNotifyConfigWidget> m_settings_widget;
    QPointer<KDENotificationSettingsHelper> m_settings_helper;
//	KComponentData m_component_data;
};

#endif // KDENOTIFICATIONLAYER_H
