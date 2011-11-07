/****************************************************************************
 *  xsettingswindow.h
 *
 *  Copyright (c) 2010 by Sidorov Aleksey <sauron@citadelspb.com>
 *
 ***************************************************************************
 *                                                                         *
 *   This library is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
*****************************************************************************/

#ifndef XSETTINGSWINDOW_H
#define XSETTINGSWINDOW_H

#include <QMainWindow>
#include <qutim/settingslayer.h>

class QListWidgetItem;
class QStackedWidget;
class QActionGroup;
namespace qutim_sdk_0_3 {
	class ActionToolBar;
}

namespace Core
{

using namespace qutim_sdk_0_3;

struct XSettingsWindowPrivate;
class XSettingsWindow : public QMainWindow
{
    Q_OBJECT
public:
	explicit XSettingsWindow(const SettingsItemList &settings,QObject *controller, QWidget *parent = 0);
	void update(const SettingsItemList &settings);
    virtual ~XSettingsWindow();
public slots:
	void save();
	void cancel();
protected:
	void loadSettings(const SettingsItemList &settings);
	void ensureActions();
    virtual void closeEvent(QCloseEvent *ev);
private:
	QAction *get(Settings::Type);
	QScopedPointer<XSettingsWindowPrivate> p;
private slots:
    void onGroupActionTriggered(QAction*);
	void onCurrentItemChanged(QListWidgetItem* item);
    void onModifiedChanged(bool haveChanges);
};

}

#endif // XSETTINGSWINDOW_H
