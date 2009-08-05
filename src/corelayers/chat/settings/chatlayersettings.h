/*
   ChatLayerSettings

    Copyright (c) 2009 by Rustam Chakin <qutim.develop@gmail.com>

 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
*/

#ifndef CHATLAYERSETTINGS_H
#define CHATLAYERSETTINGS_H

#include <QtGui>

namespace Ui {
    class ChatSettingsWidget;
}

class ChatLayerSettings : public QWidget {
    Q_OBJECT
    Q_DISABLE_COPY(ChatLayerSettings)
public:
    explicit ChatLayerSettings(const QString &profile_name,QWidget *parent = 0);
    virtual ~ChatLayerSettings();
    void saveSettings();
private slots:
    void widgetStateChanged() { m_changed = true; emit settingsChanged(); }

signals:
    void settingsChanged();
    void settingsSaved();

protected:
    virtual void changeEvent(QEvent *e);

private:
    void loadSettings();
    Ui::ChatSettingsWidget *m_ui;
    QString m_profile_name;
    bool m_changed;

};

#endif // CHATLAYERSETTINGS_H
