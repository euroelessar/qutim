/*
    StatusPresetCaption

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

#ifndef STATUSPRESETCAPTION_H
#define STATUSPRESETCAPTION_H

#include <QtGui/QDialog>
#include "ui_statuspresetcaption.h"
#include <QApplication>
#include <QDesktopWidget>
#include <QPoint>

class StatusPresetCaption : public QDialog
{
    Q_OBJECT

public:
    StatusPresetCaption(QWidget *parent = 0);
    ~StatusPresetCaption();
    void setButtonIcon(const QIcon &ok_button, const QIcon &cancel_button)
    {
    	ui.okButton->setIcon(ok_button);
    	ui.cancelButton->setIcon(cancel_button);
    }
    QString getCaption() const 
    {
    	return ui.presetEdit->text();
    }
    
private slots:
	void on_presetEdit_textChanged(const QString &text);

private:
    Ui::StatusPresetCaptionClass ui;
    QPoint desktopCenter();
};

#endif // STATUSPRESETCAPTION_H
