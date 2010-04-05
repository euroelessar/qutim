/****************************************************************************
 *  xsettingsgroup.h
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

#ifndef XSETTINGSGROUP_H
#define XSETTINGSGROUP_H
#include <QWidget>
#include <QSet>
#include "xsettingslayerimpl.h"

namespace Ui
{
	class XSettingsGroup;
}

class XSettingsGroup : public QWidget
{
	Q_OBJECT
public:
	XSettingsGroup (const SettingsItemList &settings,QWidget* parent = 0);
	virtual ~XSettingsGroup();
signals:
	void modifiedChanged(SettingsWidget *item);
	void titleChanged(const QString &title);
private:
	SettingsItemList m_setting_list;
	Ui::XSettingsGroup *ui;
protected slots:
	void currentRowChanged ( int );
	void onWidgetModifiedChanged(bool haveChanges);
};

#endif // XSETTINGSGROUP_H
