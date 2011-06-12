/****************************************************************************
**
** qutIM instant messenger
**
** Copyright (C) 2011 Ruslan Nigmatullin <euroelessar@ya.ru>
**
*****************************************************************************
**
** $QUTIM_BEGIN_LICENSE$
** This program is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 2 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
** See the GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program.  If not, see http://www.gnu.org/licenses/.
** $QUTIM_END_LICENSE$
**
****************************************************************************/

#ifndef ICONSLOADERIMPL_H
#define ICONSLOADERIMPL_H

#include <qutim/iconloader.h>
#include <qutim/settingswidget.h>
#include <qutim/settingslayer.h>
#include <QComboBox>

using namespace qutim_sdk_0_3;

namespace Core
{
class IconLoaderSettings : public SettingsWidget
{
	Q_OBJECT
public:
	IconLoaderSettings();
	
	virtual void loadImpl();
	virtual void saveImpl();
	virtual void cancelImpl();
protected slots:
	void onCurrentIndexChanged(int index);
private:
	QComboBox *m_box;
	int m_index;
};

class IconLoaderImpl : public IconLoader
{
	Q_OBJECT
public:
	IconLoaderImpl();
	virtual QIcon loadIcon(const QString &name);
	virtual QMovie *loadMovie(const QString &name);
	virtual QString iconPath(const QString &name, uint iconSize);
	virtual QString moviePath(const QString &name, uint iconSize);
public slots:
	void onSettingsChanged();
	void initSettings();
private:
	QScopedPointer<SettingsItem> m_settings;
};
}

#endif // ICONSLOADERIMPL_H
