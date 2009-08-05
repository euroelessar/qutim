/*
	AbstractTrayLayer

    Copyright (c) 2009 by Nigmatullin Ruslan <euroelessar@gmail.com>

 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
*/

#ifndef DEFAULTTRAYLAYER_H
#define DEFAULTTRAYLAYER_H

#include "../../../include/qutim/plugininterface.h"
#include "../../../include/qutim/layerinterface.h"

#include <QMenu>

#if defined(Q_OS_WIN32) || defined(Q_OS_MAC)
        #include "exsystrayicon.h"
#else
        #include <QSystemTrayIcon>
#endif

using namespace qutim_sdk_0_2;

class DefaultTrayLayer : public QObject, public TrayLayerInterface
{
	Q_OBJECT
public:
    DefaultTrayLayer();
	virtual bool init(PluginSystemInterface *plugin_system);
    void createTrayIcon();
    virtual void release();
    virtual void setProfileName(const QString &);
    virtual void setLayerInterface( LayerType, LayerInterface *) {}

    virtual void saveLayerSettings() {}
    virtual void removeLayerSettings() {}

    virtual void saveGuiSettingsPressed() {}
    virtual void removeGuiLayerSettings() {}
    virtual QMenu *contextMenu() const;
    virtual QIcon icon() const;
    virtual void setContextMenu(QMenu * menu);
    virtual void setIcon(const QIcon & icon);
    virtual void setToolTip(const QString & tip);
    virtual void showMessage(const QString & title, const QString & message, int timeout_hint = 10000);
    virtual QString toolTip() const;
public slots:
	void activated(QSystemTrayIcon::ActivationReason reason);
private:
    QAction *m_quit_action;
	QMenu *m_tray_menu;
	quint16 m_event_context;
	quint16 m_event_trigger;
	quint16 m_event_double_click;
	quint16 m_event_middle_click;
#ifndef Q_OS_WIN32
    QSystemTrayIcon *m_tray_icon;
#else
    ExSysTrayIcon *m_tray_icon;
#endif
};

#endif // DEFAULTTRAYLAYER_H
