/*
    SoundSettings

    Copyright (c) 2008-2009 by Nigmatullin Ruslan <euroelessar@gmail.com>
                       2008 by Denis Novikov <Denisss>

 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
*/

#ifndef SOUNDLAYERSETTINGS_H
#define SOUNDLAYERSETTINGS_H

#include <QtGui/QWidget>
#include <QList>
#include <QTreeWidgetItem>
#include "include/qutim/plugininterface.h"

#include "ui_soundlayersettings.h"

using namespace qutim_sdk_0_2;

class SoundLayerSettings : public QWidget
{
    Q_OBJECT

public:
    SoundLayerSettings(const QString &profile_name, QWidget *parent = 0);
    ~SoundLayerSettings() {};
    void loadSettings();
    void saveSettings();

private slots:
	// Play tab widgets
//	void on_systemCombo_currentIndexChanged(int index);
//	void on_commandButton_clicked();

	// Events tab widgets
	void on_eventsTree_currentItemChanged(QTreeWidgetItem *current,
		QTreeWidgetItem *previous);
	void on_eventsTree_itemChanged(QTreeWidgetItem *item, int column);
	void on_fileEdit_textChanged(const QString &text);
	void on_applyButton_clicked();
	void on_openButton_clicked();
	void on_playButton_clicked();
	void on_exportButton_clicked();
	void on_importButton_clicked();

        void widgetStateChanged() { if (!changed) { emit settingsChanged(); changed = true; } }

signals:
	void settingsChanged();
	void settingsSaved();

private:
    void appendStatus(const QString &statusText);
    void appendEvent(const QString &eventName, const NotificationType event);
    inline QString getCurrentFile() const;
//    inline qutim_sdk_0_2::SoundEngineSystem getCurrentSoundSystem() const;
    bool changed;
    QString m_profile_name;

    Ui::SoundSettingsClass ui;
    QString lastDir;
    QList <QTreeWidgetItem *> statusList, eventList;
};

#endif // SOUNDLAYERSETTINGS_H
