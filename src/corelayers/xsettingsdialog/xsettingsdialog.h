/****************************************************************************
 *  xsettingsdialog.h
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

#ifndef XSETTINGSDIALOG_H
#define XSETTINGSDIALOG_H

#include <QDialog>
#include "xsettingslayerimpl.h"

class XSettingsGroup;
class QStateMachine;
class QState;
class XToolBar;
namespace Ui {
	class XSettingsDialog;
}

class XSettingsDialog : public QDialog {
	Q_OBJECT
public:
	XSettingsDialog(const SettingsItemList &settings,QWidget *parent = 0);
	~XSettingsDialog();
	//void setSettingsList(const SettingsItemList &settings);//TODO
protected:
	void changeEvent(QEvent *e);
	virtual void closeEvent(QCloseEvent* );
protected slots:
	void onActionTriggered(QAction *action);
	void onWidgetModifiedChanged(bool haveChanges);
	void onWidgetModifiedChanged(SettingsWidget *widget);
	void onTitleChanged(const QString &title);
	void onSaveButtonTriggered();
	void onCancelButtonTriggered();
private:
	inline void addAction(QAction* action, Settings::Type type);
	Ui::XSettingsDialog *ui;
	QVector<SettingsItemList> m_settings_items;
	QVector<XSettingsGroup *> m_group_widgets;
	QList<SettingsWidget *> m_modified_widgets;
};

#endif // XSETTINGSDIALOG_H
