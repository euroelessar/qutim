/****************************************************************************
 *  mobilesettingswindow.h
 *
 *  Copyright (c) 2010 by Aleksey Sidorov <sauron@citadelspb.com>
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
#include <slidingstackedwidget.h>

class QListWidgetItem;
class QStackedWidget;
class QActionGroup;
class QModelIndex;
namespace qutim_sdk_0_3 {
	class ActionToolBar;
}

namespace Core
{

using namespace qutim_sdk_0_3;

struct MobileSettingsWindowPrivate;
class MobileSettingsWindow : public QMainWindow
{
    Q_OBJECT
public:
	explicit MobileSettingsWindow(const SettingsItemList &settings,QObject *controller);
	void update(const SettingsItemList &settings);
	virtual ~MobileSettingsWindow();
public slots:
	void save();
	void cancel();
protected:
	void loadSettings(const SettingsItemList &settings);
	void ensureActions();
    virtual void closeEvent(QCloseEvent *ev);
private:
	QListWidgetItem *get(Settings::Type);
	QScopedPointer<MobileSettingsWindowPrivate> p;
private slots:
	void onCategoryActivated(const QModelIndex &index);
	void onCategoryActivated(QListWidgetItem *item);
	void onCurrentItemActivated(const QModelIndex &index);
    void onModifiedChanged(bool haveChanges);
	void onWidgetDestroyed(QObject *obj);
	void slideUp();
	void slideDown(QWidget *w);
	void fingerGesture(enum SlidingStackedWidget::SlideDirection);
};

}

#endif // XSETTINGSWINDOW_H
