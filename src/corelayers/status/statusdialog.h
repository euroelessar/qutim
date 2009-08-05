/*
    StatusDialog

    Copyright (c) 2008 by Rustam Chakin <qutim.develop@gmail.com>

 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
*/

#ifndef STATUSDIALOG_H
#define STATUSDIALOG_H

#include <QtGui/QDialog>
#include <QApplication>
#include <QDesktopWidget>
#include <QPoint>
#include <QtXml>
#include "ui_statusdialogvisual.h"
#include "iconmanager.h"

class StatusDialog : public QDialog
{
    Q_OBJECT

public:
    StatusDialog(const QString &profile_name, QWidget *parent = 0);
    ~StatusDialog();
    void setStatusMessage(const QString &status_message)
    {
    	ui.statusTextEdit->setPlainText(status_message);
    }
    
    QString getStatusMessage() const
    {
    	return ui.statusTextEdit->toPlainText();
    }
    
    bool getDshowFlag() const
    {
    	return ui.dshowBox->isChecked();
    }
    
private slots:
	void on_saveButton_clicked();
	void on_presetComboBox_currentIndexChanged(int index);
	void on_delButton_clicked();
private:
    Ui::StatusDialogVisualClass ui;
    IconManager &m_icon_manager;
    QString m_profile_name;
    QPoint desktopCenter();
    void savePreset(const QString &preset_caption);
    void loadPresets();
    QString m_preset_file;
};

#endif // STATUSDIALOG_H
