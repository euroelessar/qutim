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

#include "kdenotificationcore.h"
#include <kaboutdata.h>
#include <kglobalsettings.h>
#include <knotification.h>
#include <QDebug>
#include <kapplication.h>
#include <kcmdlineargs.h>
#include <QApplication>
#include <kstyle.h>
#include <shared/shareddata.h>

void KDENotificationCore::setCmdArgs( int argc, char **argv )
{
//      Элесарушко я вернул в зад как у меня было, а ты уж сам разбирайся от чего неработает
//    //	if( qApp )
//    //		delete qApp;
//
//    //	KCmdLineArgs::init( argc, argv, data );
//    //	new KApplication( true );
}

bool KDENotificationCore::init(PluginSystemInterface *plugin_system)
{
	PluginInterface::init(plugin_system);
	KdeIntegration::ensureActiveComponent();
    PluginInterface::init(plugin_system);
    KDENotificationIcon = new QIcon(":/icons/plugin.png");
    m_notification_layer = new KDENotificationLayer();
	if( SystemsCity::PluginSystem()->setLayerInterface (NotificationLayer ,m_notification_layer) )
		return true;
    return true;
}

void KDENotificationCore::release()
{
}

void KDENotificationCore::processEvent(PluginEvent &event)
{
    //	if (event.system_event_type == PointersAreInitialized)
    Q_UNUSED(event);
}

QString KDENotificationCore::name()
{
    return "KDENotification";
}

QString KDENotificationCore::description()
{
    return "KDE notification plugin";
}

QIcon *KDENotificationCore::icon()
{
    return KDENotificationIcon;
}

QString KDENotificationCore::type()
{
    return "Notification Layer";
}

void KDENotificationCore::setProfileName(const QString &profileName)
{
    m_profile_name = profileName;
}

Q_EXPORT_PLUGIN2(KDENotificationCore, KDENotificationCore);
