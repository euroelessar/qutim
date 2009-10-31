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

#ifndef KDENOTIFICATIONCORE_H
#define KDENOTIFICATIONCORE_H
#include <qutim/layerscity.h>
#include "kdenotificationlayer.h"
#include <QIcon>
#include <QWidget>

using namespace qutim_sdk_0_2;

class KDENotificationCore : public QObject, public LayerPluginInterface, public CmdArgsHandler
{
	Q_OBJECT
	Q_INTERFACES(qutim_sdk_0_2::PluginInterface qutim_sdk_0_2::LayerPluginInterface qutim_sdk_0_2::CmdArgsHandler)
public:
	virtual void setCmdArgs( int argc, char **argv );
	virtual bool init(PluginSystemInterface* plugin_system);
	
	virtual void release();
	virtual void processEvent(PluginEvent &event);
	virtual void setProfileName(const QString &profileName);
	virtual QString name();
	virtual QString description();
	virtual QString type();
	virtual QIcon *icon();
private:
	QIcon *KDENotificationIcon;
	QString m_profile_name;
	KDENotificationLayer *m_notification_layer;
};

#endif // KDENOTIFICATIONCORE_H
