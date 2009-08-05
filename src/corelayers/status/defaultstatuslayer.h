/*
	DefaultStatusLayer

    Copyright (c) 2008 by Rustam Chakin <qutim.develop@gmail.com>
                  2009 by Nigmatullin Ruslan <euroelessar@gmail.com>

 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
*/

#ifndef DEFAULTSTATUSLAYER_H
#define DEFAULTSTATUSLAYER_H

#include "../../../include/qutim/plugininterface.h"
#include "../../../include/qutim/layerinterface.h"
#include <QString>
#include "iconmanager.h"

using namespace qutim_sdk_0_2;

class DefaultStatusLayer : public StatusLayerInterface
{
public:
    virtual bool init(PluginSystemInterface *plugin_system)
    {
        m_name = "qutim";
        quint8 major, minor, secminor;
        quint16 svn;
        plugin_system->getQutimVersion(major, minor, secminor, svn);
        m_version = QString("%1.%2.%3 r%4").arg(major).arg(minor).arg(secminor).arg(svn);
        return true;
    }
    virtual void release() {}
    virtual void setProfileName(const QString &profile_name) { m_profile_name = profile_name; }
    virtual void setLayerInterface( LayerType, LayerInterface *) {}

    virtual void saveLayerSettings() {}
    virtual void removeLayerSettings() {}

    virtual void saveGuiSettingsPressed() {}
    virtual void removeGuiLayerSettings() {}

	virtual bool setStatusMessage(const TreeModelItem &item, const QString &status_type, QString &status_message, bool &dshow);
private:
    QString m_profile_name;
};

#endif // DEFAULTSTATUSLAYER_H
