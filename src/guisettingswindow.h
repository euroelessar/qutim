/*
    GuiSetttingsWindow

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

#ifndef GUISETTTINGSWINDOW_H
#define GUISETTTINGSWINDOW_H

#include <QtGui/QDialog>
#include <QPoint>
#include "ui_guisettingswindow.h"

class GuiSetttingsWindow : public QDialog
{
    Q_OBJECT

public:
    GuiSetttingsWindow(const QString &profile_name, QWidget *parent = 0);
	~GuiSetttingsWindow();
    
private slots:
	void enableApplyButton();
	void on_applyButton_clicked();
	void on_okButton_clicked();
    
private:
	void reloadContent();
	QPoint desktopCenter();
	void addUiContent();
	void addEmoticonThemes(const QString &path);
	void addListThemes(const QString &path);
	void addChatThemes(const QString &path);
	void addWebkitThemes(const QString &path);
	void addPopupThemes(const QString &path);
	void addStatusThemes(const QString &path);
	void addSystemThemes(const QString &path);
	void addLanguages(const QString &path);
	void addApplicationStyles(const QString &path);
	void addBorderThemes(const QString &path);
	void saveSettings();
	void loadSettings();
	QString m_profile_name;
    Ui::GuiSetttingsWindowClass ui;
};

#endif // GUISETTTINGSWINDOW_H
