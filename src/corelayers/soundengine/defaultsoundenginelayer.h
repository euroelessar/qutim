/*
    DefaultSoundEngineLayer

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

#ifndef DEFAULTSOUNDENGINELAYER_H
#define DEFAULTSOUNDENGINELAYER_H

#include "../../../include/qutim/plugininterface.h"
#include "../../../include/qutim/layerinterface.h"

using namespace qutim_sdk_0_2;

class DefaultSoundEngineLayer : public SoundEngineLayerInterface
{
public:
    enum EngineType {
        NoSound = 0,
        LibSound,
        UserCommand,
        UnknownSoundEngine,
        UnknownSoundEngine2,
        UnknownSoundEngine3,
        UnknownSoundEngine4
    };
    DefaultSoundEngineLayer();
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
    virtual void setProfileName(const QString &profile_name) { m_profile_name = profile_name; loadSettings(); }
    void loadSettings();
    virtual void setLayerInterface( LayerType, LayerInterface *) {}

    virtual void saveLayerSettings();
    virtual QList<SettingsStructure> getLayerSettingsList();
    virtual void removeLayerSettings();

    virtual void saveGuiSettingsPressed() {}
    virtual void removeGuiLayerSettings() {}

	virtual void playSound(QIODevice *device);
	virtual void playSound(const QString &filename);
    virtual QIODevice *grabSound();
private:
    EngineType m_engine;
    QString m_cmd;
    QString m_profile_name;
};

#endif // DEFAULTSOUNDENGINELAYER_H
