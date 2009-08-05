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

#include "defaultsoundenginelayer.h"
#include "soundenginesettings.h"
#include "src/iconmanager.h"
#include <QFile>
#include <QFileInfo>
#include <QProcess>
#if defined(Q_WS_WIN) || defined(Q_WS_MAC)
#include <QSound>
#endif
DefaultSoundEngineLayer::DefaultSoundEngineLayer()
{
    m_engine=NoSound;
    m_cmd = "play \"%1\"";
}

void DefaultSoundEngineLayer::loadSettings()
{
    QSettings settings(QSettings::defaultFormat(), QSettings::UserScope, "qutim/qutim."+m_profile_name, "profilesettings");
    settings.beginGroup("sounds");
    m_engine = static_cast<EngineType>(settings.value("soundengine", 0).toInt());
    m_cmd = settings.value("command", "play \"%1\"").toString();
    if(m_cmd.indexOf("%1")<0)
		m_cmd+=" \"%1\"";
    settings.endGroup();
}

void DefaultSoundEngineLayer::saveLayerSettings()
{
    foreach(const SettingsStructure &settings, m_settings)
    {
        dynamic_cast<SoundEngineSettings*>(settings.settings_widget)->saveSettings();
    }
    loadSettings();
}

QList<SettingsStructure> DefaultSoundEngineLayer::getLayerSettingsList()
{
	SettingsStructure settings;
	settings.settings_item = new QTreeWidgetItem();
	settings.settings_item ->setText(0, QObject::tr("Sound"));
	settings.settings_item ->setIcon(0, IconManager::instance().getIcon("soundsett"));
	settings.settings_widget = new SoundEngineSettings(m_profile_name);
	m_settings.append(settings);
	return m_settings;
}

void DefaultSoundEngineLayer::removeLayerSettings()
{
	foreach(const SettingsStructure &settings, m_settings)
	{
		delete settings.settings_item;
		delete settings.settings_widget;
	}
	m_settings.clear();
}


void DefaultSoundEngineLayer::playSound(QIODevice *device)
{
	Q_UNUSED(device);
//	if(!device)
//		return;
//
//	QFile *file = dynamic_cast<QFile *>(device);
//	if(!file)
//	{
//		if(device->isOpen())
//			device->close();
//		else
//			delete device;
//		return;
//	}
//
//	QString file_name = QFileInfo(*file).absoluteFilePath();
//
//	if(file->isOpen())
//		file->close();
//	else
//		delete file;
//
//	switch(m_engine)
//	{
//	#if defined(Q_WS_WIN) || defined(Q_WS_MAC)
//	case LibSound:
//		QSound::play(file_name);
//		break;
//	#endif
//	case UserCommand:
//		QProcess::startDetached(m_cmd.arg(file_name));
//		break;
//	default:
//		break;
//	}
}

void DefaultSoundEngineLayer::playSound(const QString &filename)
{

	switch(m_engine)
	{
#if defined(Q_WS_WIN) || defined(Q_WS_MAC)
	case LibSound:
		QSound::play(filename);
		break;
#endif
	case UserCommand:
		QProcess::startDetached(m_cmd.arg(filename));
		break;
	default:
		break;
	}
}

QIODevice *DefaultSoundEngineLayer::grabSound()
{
    return 0;
}
