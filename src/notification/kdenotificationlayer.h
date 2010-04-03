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
#include <QGridLayout>
#include <qutim/notificationslayer.h>
#include <qutim/settingswidget.h>
#include "activenotification.h"

using namespace qutim_sdk_0_3;

class KDENotificationSettingsHelper : public SettingsWidget
{
    Q_OBJECT
public:
	KDENotificationSettingsHelper()
	{
		m_child = new KNotifyConfigWidget(this);
		m_child->setApplication();
        QGridLayout *layout = new QGridLayout(this);
        this->setLayout(layout);
		layout->addWidget(m_child);
		connect(m_child, SIGNAL(changed(bool)), this, SIGNAL(modifiedChanged(bool)));
	}

	virtual void loadImpl() {}
	virtual void saveImpl() { m_child->save(); }
	virtual void cancelImpl() {}
private:
	KNotifyConfigWidget *m_child;
};

class KDENotificationLayer : public PopupBackend
{
    Q_OBJECT
public:
    KDENotificationLayer ();
	virtual ~KDENotificationLayer ();
	virtual void show(Notifications::Type type,
					  QObject *sender,
					  const QString &body,
					  const QVariant &data);
private:
	ActiveNotifications m_activenotifications;
    int cutCount;
    QPointer<KNotifyConfigWidget> m_settings_widget;
	QPointer<KDENotificationSettingsHelper> m_settings_helper;
};

#endif // KDENOTIFICATIONLAYER_H
